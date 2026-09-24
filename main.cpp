#include <SFML/Graphics.hpp>
#include <vector>

#include "windowHeader.h"
#include "interfaceDesign.h"
#include "interSchem.h"


#define windowWidth 1600
#define toolbar_distanceBetweenShapes 100
#define pare_Ydistance 20.0
#define windowHeight 1000
#define doubleClickWait 300

void removeBlockConnectionsTo(Block* blk);
void removeArrowConnectionsToAndFrom(Block* blk);
std::string getPath(const sf::Font& font, sf::RenderWindow& w);


sf::Font Block::font;

std::vector <std::unique_ptr<Block>> blocks;
std::vector<std::unique_ptr<Arrow>> arrows;

Block* selectedBlock = nullptr;
anchor* selectedAnchor = nullptr;
Block* anchorBlockOrigin = nullptr;

bool deletingBlocks = false;
bool deletingArrows = false;

bool mainMenuLoop = true;

bool waitingForSecondClick = false;
bool editing = false;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "InterSchem", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    if (!Block::font.openFromFile("D:/VS/Repos/Project2/Project2/Fonts/Roboto/Roboto-Italic-VariableFont_wdth,wght.ttf"))
    {
        throw std::runtime_error("Failed to load font from file.");
    }

    parentShapes buildingShapes = createParentShapes();
    sf::Vector2f dragOffset;
    Block* startBlk = nullptr;
    Block* currentEditingBlock = nullptr;
    std::string editBuffer;
    size_t cursorPosition = 0;
    sf::Clock clickClock;
    const sf::Time doubleClickTime = sf::milliseconds(doubleClickWait);
    sf::String textToAdd;

    HUD userInterface = initializeHUD(windowWidth, windowHeight);

    while (window.isOpen()) {

        if (mainMenuLoop) {
            mainMenu m = initializeMainMenu(windowWidth, windowHeight);
            int flagCode;
            while (flagCode = inMainMenu(window, m)) { if (flagCode != 100 && flagCode != 200) continue; else break; }
            if (flagCode == 100) {
                std::string path = getPath(font, window);
                loadProject(path, blocks, arrows, buildingShapes);
                for (auto& blk : blocks) {
                    if (blk->blockType == startBlockType) startBlk = blk.get();
                }
                mainMenuLoop = false;
            }
            else if (flagCode == 200)
            {
                showHelpWindow(window);
            }
            else {
                mainMenuLoop = false;
            }
            continue;
        }

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (editing && currentEditingBlock) {
                    if (textEvent->unicode == 8) // Backspace
                    {
                        if (!editBuffer.empty() && cursorPosition > 0)
                        {
                            editBuffer.erase(cursorPosition - 1, 1);
                            cursorPosition--;
                        }
                    }
                    else if (textEvent->unicode == 13) // Enter
                    {
                        currentEditingBlock->setText(editBuffer);
                        editing = false;
                        currentEditingBlock = nullptr;
                    }
                    else if (textEvent->unicode == 27) // Escape
                    {
                        if (currentEditingBlock) {
                            currentEditingBlock->setText(editBuffer);
                        }
                        editing = false;
                        currentEditingBlock = nullptr;
                    }
                    else if (textEvent->unicode >= 32 && textEvent->unicode < 127)
                    {
                        editBuffer.insert(cursorPosition, 1, static_cast<char>(textEvent->unicode));
                        cursorPosition++;
                    }

                    if (currentEditingBlock)
                    {
                        currentEditingBlock->setText(editBuffer + "|");
                    }
                }
            }

            else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(mouseEvent->position);
                    bool clickedSomething = false;

                    for (auto& block : blocks)
                    {
                        for (auto& a : block->anchorPoints) {
                            if (a.shape.getGlobalBounds().contains(mousePos)) {
                                if (selectedAnchor == nullptr) {

                                    if (!a.isSelected && (a.blockType == startBlockType || a.positionType == anchorBottomType || a.positionType == anchorRightType)) {
                                        selectedAnchor = &a;
                                        anchorBlockOrigin = block.get();

                                        a.isSelected = true;
                                        a.shape.setOutlineThickness(anchorOutlineThickness);
                                    }

                                }
                                else if (selectedAnchor != &a && block.get() != anchorBlockOrigin) {

                                    if (!a.isSelected && (a.blockType == stopBlockType || a.positionType == anchorTopType || a.positionType == anchorLeftType)) {
                                        a.isSelected = true;
                                        a.shape.setOutlineThickness(anchorOutlineThickness);


                                        if (anchorBlockOrigin->blockType == decisionBlockType && selectedAnchor->positionType == anchorRightType)
                                            anchorBlockOrigin->connectedToFalse = block.get();
                                        else
                                            anchorBlockOrigin->connectedToTruth = block.get();


                                        auto arr = std::make_unique<Arrow>(selectedAnchor, &a);
                                        arr->from = anchorBlockOrigin;
                                        arr->to = block.get();
                                        arrows.push_back(std::move(arr));



                                        anchorBlockOrigin = nullptr;
                                        selectedAnchor = nullptr;
                                    }
                                }
                                clickedSomething = true;
                                break;
                            }
                        }
                        if (clickedSomething) break;
                    }







                    if (editing && currentEditingBlock) {
                        bool clickedOnEditingBlock = currentEditingBlock->getBounds().contains(mousePos);

                        if (!clickedOnEditingBlock) {
                            currentEditingBlock->setText(editBuffer);
                            editing = false;
                            currentEditingBlock = nullptr;
                            clickedSomething = true;
                        }
                        else
                        {
                            clickedSomething = true;
                        }
                    }

                    if (!clickedSomething) {

                        if (startBlk == nullptr && buildingShapes.startEntity.getGlobalBounds().contains(mousePos))
                        {

                            auto ptr = std::make_unique<startBlock>(buildingShapes.startEntity);
                            selectedBlock = ptr.get();
                            startBlk = ptr.get();
                            dragOffset = selectedBlock->getPosition() - mousePos;

                            ptr->blockType = startBlockType;
                            blocks.push_back(std::move(ptr));
                            clickedSomething = true;

                        }
                        else if (buildingShapes.actionEntity.getGlobalBounds().contains(mousePos))
                        {

                            auto ptr = std::make_unique<actionBlock>(buildingShapes.actionEntity);
                            selectedBlock = ptr.get();
                            dragOffset = selectedBlock->getPosition() - mousePos;

                            ptr->blockType = actionBlockType;
                            blocks.push_back(std::move(ptr));
                            clickedSomething = true;
                        }
                        else if (buildingShapes.decisionEntity.getGlobalBounds().contains(mousePos))
                        {

                            auto ptr = std::make_unique<decisionBlock>(buildingShapes.decisionEntity);
                            selectedBlock = ptr.get();
                            dragOffset = selectedBlock->getPosition() - mousePos;

                            ptr->blockType = decisionBlockType;
                            blocks.push_back(std::move(ptr));
                            clickedSomething = true;
                        }
                        else if (buildingShapes.inputEntity.getGlobalBounds().contains(mousePos))
                        {
                            auto ptr = std::make_unique<inputBlock>(buildingShapes.inputEntity);
                            selectedBlock = ptr.get();
                            dragOffset = selectedBlock->getPosition() - mousePos;

                            ptr->blockType = inputBlockType;
                            blocks.push_back(std::move(ptr));
                            clickedSomething = true;
                        }
                        else if (buildingShapes.outputEntity.getGlobalBounds().contains(mousePos))
                        {
                            auto ptr = std::make_unique<outputBlock>(buildingShapes.outputEntity);
                            selectedBlock = ptr.get();
                            dragOffset = selectedBlock->getPosition() - mousePos;

                            ptr->blockType = outputBlockType;
                            blocks.push_back(std::move(ptr));
                            clickedSomething = true;
                        }
                        else if (buildingShapes.stopEntity.getGlobalBounds().contains(mousePos))
                        {
                            auto ptr = std::make_unique<stopBlock>(buildingShapes.stopEntity);
                            selectedBlock = ptr.get();
                            dragOffset = selectedBlock->getPosition() - mousePos;

                            ptr->blockType = stopBlockType;
                            blocks.push_back(std::move(ptr));
                            clickedSomething = true;
                        }
                        else if (userInterface.run.getGlobalBounds().contains(mousePos))
                        {
                            runProgram(window, *startBlk);
                        }
                        else if (userInterface.save.getGlobalBounds().contains(mousePos)) {

                            std::string path = getPath(font, window);


                            // select path and pass it to save Project
                            saveProject(path, blocks, arrows);
                        }
                        else if (userInterface.goBack.getGlobalBounds().contains(mousePos)) {
                            // Are you sure?
                            blocks.clear(); arrows.clear(); startBlk = nullptr;
                            codeLines.clear();
                            codeLinesToOutput.clear();
                            mainMenuLoop = true;

                        }

                    }

                    if (!clickedSomething && selectedAnchor == nullptr) {
                        for (auto& block : blocks)
                        {
                            if (block->hasEditableText() && block->getBounds().contains(mousePos))
                            {
                                if (waitingForSecondClick && clickClock.getElapsedTime() < doubleClickTime)
                                {
                                    currentEditingBlock = block.get();
                                    editBuffer = currentEditingBlock->getText();
                                    cursorPosition = editBuffer.length();
                                    editing = true;
                                    waitingForSecondClick = false;
                                    clickedSomething = true;
                                }
                                else
                                {
                                    waitingForSecondClick = true;
                                    clickClock.restart();
                                }
                                break;
                            }
                        }
                    }

                    if (!clickedSomething && selectedAnchor) {
                        selectedAnchor->isSelected = false;
                        selectedAnchor->shape.setOutlineThickness(0.f);
                        selectedAnchor = nullptr;
                        anchorBlockOrigin = nullptr;

                    }


                    if (!clickedSomething) {

                        for (auto& blk : blocks) {
                            if (blk->getBounds().contains(mousePos)) {
                                selectedBlock = blk.get();
                                dragOffset = selectedBlock->getPosition() - mousePos;
                                clickedSomething = true;
                                break;


                            }
                        }
                    }

                }
                else if (mouseEvent->button == sf::Mouse::Button::Right && selectedBlock == nullptr) {

                    sf::Vector2f mousePos = window.mapPixelToCoords(mouseEvent->position);


                    for (auto blk = blocks.begin(); blk != blocks.end(); blk++) {
                        if ((*blk)->getBounds().contains(mousePos)) {

                            if (anchorBlockOrigin == (*blk).get()) {
                                selectedAnchor = nullptr;
                                anchorBlockOrigin = nullptr;
                            }

                            if (startBlk == (*blk).get())
                                startBlk = nullptr;

                            // sterge legaturile
                            removeBlockConnectionsTo((*blk).get());
                            removeArrowConnectionsToAndFrom((*blk).get());

                            blocks.erase(blk);
                            break;
                        }
                    }



                }
            }
            else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    if (selectedBlock != nullptr)
                    {
                        sf::FloatRect blockBounds = selectedBlock->getBounds();

                        if (blockBounds.position.y < 80.f)
                        {
                            for (auto blk = blocks.begin(); blk != blocks.end(); ++blk)
                            {
                                if ((*blk).get() == selectedBlock)
                                {
                                    if (startBlk == selectedBlock) {
                                        startBlk = nullptr;
                                    }

                                    removeBlockConnectionsTo(selectedBlock);
                                    removeArrowConnectionsToAndFrom(selectedBlock);

                                    blocks.erase(blk);
                                    break;
                                }
                            }
                        }
                    }
                    selectedBlock = nullptr;
                }
            }

        }


        if (selectedBlock != nullptr) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sf::Vector2f newPos = mousePos + dragOffset;
            float maxX = windowWidth - 500.f;
            float maxY = windowHeight - 120.f;
            sf::FloatRect blockBounds = selectedBlock->getBounds();

            float blockPosX = selectedBlock->getPosition().x;
            float blockPosY = selectedBlock->getPosition().y;
            float blockRight = blockBounds.position.x + blockBounds.size.x;
            float blockBottom = blockBounds.position.y + blockBounds.size.y;

            float offsetX = blockBounds.position.x - blockPosX;
            float offsetY = blockBounds.position.y - blockPosY;

            if (newPos.y + offsetY >= 85.f)
            {
                selectedBlock->belowTopLine = true;
            }

            if (newPos.x + offsetX + blockBounds.size.x > maxX) // right
            {
                newPos.x = maxX - offsetX - blockBounds.size.x;
            }

            if (newPos.y + offsetY + blockBounds.size.y > maxY) // bottom
            {
                newPos.y = maxY - offsetY - blockBounds.size.y;
            }

            if (newPos.x + offsetX < 0) // left
            {
                newPos.x = -offsetX;
            }
            if (selectedBlock->belowTopLine)// top
            {
                if (newPos.y + offsetY < 85.f)
                {
                    newPos.y = 85.f - offsetY;
                }
            }

            selectedBlock->setPosition(newPos);
        }


        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            bool removed = false;

            for (auto arr = arrows.begin(); arr != arrows.end(); ) {

                removed = false;
                for (int i = 0; i < (*arr)->line.getVertexCount() - 1; i++) {
                    if ((*arr)->calculateDistanceFromCursor(mousePos, i, i + 1) < 5.0f) {
                        (*arr)->removeConnection();
                        arr = arrows.erase(arr);
                        removed = true;
                        break;
                    }
                }
                if (!removed)
                    arr++;

            }

        }

        window.clear();
        drawParentShapes(window, buildingShapes);

        for (auto& block : blocks) {
            block->draw(window);
        }
        for (auto& arr : arrows) {
            arr->draw(window);
        }

        drawHUD(window, userInterface);

        for (const auto& code : codeLinesToOutput) window.draw(code);

        window.display();


    }

    return 0;
}

std::string getPath(const sf::Font& font, sf::RenderWindow& w) {
    sf::Text message(font, "Path to file: ", 24);
    sf::Text userInput(font, "", 24);

    sf::Vector2f center = static_cast<sf::Vector2f>(w.getSize()) / 2.f;

    sf::FloatRect msgBounds = message.getLocalBounds();
    message.setOrigin(msgBounds.position + msgBounds.size / 2.f);
    message.setPosition(center - sf::Vector2f(0.f, 30.f));

    userInput.setFillColor(sf::Color::Cyan);

    std::string currentInput = "";

    while (w.isOpen()) {
        while (auto event = w.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                w.close();
                return "";
            }
            else if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                char32_t code = textEvent->unicode;

                if (code == 13) {
                    if (!currentInput.empty()) {
                        return currentInput;
                    }
                }
                else if (code == 8) {
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                    }
                }
                else if (code < 128) {

                    if (code > 31 && code != 127) {
                        currentInput += static_cast<char>(code);
                    }
                }

                userInput.setString(currentInput);

                sf::FloatRect inputBounds = userInput.getLocalBounds();
                userInput.setOrigin(inputBounds.position + inputBounds.size / 2.f);
                userInput.setPosition(center + sf::Vector2f(0.f, 30.f));
            }
        }

        w.clear();
        w.draw(message);
        w.draw(userInput);
        w.display();
    }

    return "";
}

void removeBlockConnectionsTo(Block* blk) {

    for (auto& b : blocks)
        if (b->connectedToTruth == blk) b->connectedToTruth = nullptr;
        else if (b->connectedToFalse == blk) b->connectedToFalse = nullptr;
}

void removeArrowConnectionsToAndFrom(Block* blk) {

    for (auto arr = arrows.begin(); arr != arrows.end(); ) {

        if ((*arr)->from == blk || (*arr)->to == blk) {
            (*arr)->removeConnection();

            arr = arrows.erase(arr);
        }
        else {
            ++arr;
        }
    }
}