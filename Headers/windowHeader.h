#include <SFML/Graphics.hpp>

#define radiusStartStop 25.f
#define scaleX 2.f
#define scaleY 1.f
#define startPositionX 20.f
#define startPositionY 50.f
#define fontSize 20
#define parentShapesOffset 50.f
#define actionEntityWidth 100.f
#define actionEntityHeight 50.f
#define decisionWidth 100.f
#define decisionHeight 50.f
#define inputOutputHeight 50.f
#define inputOutputBigBase 100.f
#define inputOutputSmallBase 50.f
#define color sf::Color(100, 100, 200)
#define textColor sf::Color::White

#define horizonLineY 50.f

struct parentShapes {
    sf::CircleShape startEntity;
    sf::RectangleShape actionEntity;
    sf::ConvexShape decisionEntity;
    sf::ConvexShape inputEntity;
    sf::ConvexShape outputEntity;
    sf::CircleShape stopEntity;
    sf::Font font;
    sf::Text* startText;
    sf::Text* actionText;
    sf::Text* decisionText;
    sf::Text* inputText;
    sf::Text* outputText;
    sf::Text* stopText;

    sf::RectangleShape topLine;
    sf::RectangleShape bottomLine;
};

parentShapes createParentShapes()
{
    parentShapes shapes;
    if (!shapes.font.openFromFile("C:/Users/Stefan/Desktop/Fonts/Roboto/Roboto-Italic-VariableFont_wdth,wght.ttf")) {
        throw std::runtime_error("Failed to load font from file.");
    }

    shapes.topLine.setFillColor(sf::Color::White);
    shapes.topLine.setSize(sf::Vector2f(1100.f, 1.f));
    shapes.topLine.setPosition(sf::Vector2f(0.f, 890.f));

    shapes.bottomLine.setFillColor(sf::Color::White);
    shapes.bottomLine.setSize(sf::Vector2f(1100.f, 1.f));
    shapes.bottomLine.setPosition(sf::Vector2f(0.f, 80.f));

    //Start Entity
    shapes.startEntity.setRadius(radiusStartStop);
    shapes.startEntity.setFillColor(color);
    shapes.startEntity.setScale({ scaleX, scaleY });
    shapes.startEntity.setPosition({ startPositionX , horizonLineY - radiusStartStop * scaleY });

    // Action Entity
    float actionPositionX = startPositionX + 2 * scaleX * radiusStartStop + parentShapesOffset;
    float actionPositionY = horizonLineY - (actionEntityHeight / 2.f);
    shapes.actionEntity.setSize({ actionEntityWidth , actionEntityHeight });
    shapes.actionEntity.setFillColor(color);
    shapes.actionEntity.setPosition({ actionPositionX, actionPositionY });

    // Decision Entity
    float decisionPositionX = actionPositionX + actionEntityWidth + parentShapesOffset;
    float decisionPositionY = horizonLineY - (decisionHeight / 2.f);

    shapes.decisionEntity.setPointCount(4);
    shapes.decisionEntity.setPoint(0, sf::Vector2f(decisionWidth / 2.f, 0.f));               // Top Middle
    shapes.decisionEntity.setPoint(1, sf::Vector2f(decisionWidth, decisionHeight / 2.f));    // Right Middle
    shapes.decisionEntity.setPoint(2, sf::Vector2f(decisionWidth / 2.f, decisionHeight));    // Bottom Middle
    shapes.decisionEntity.setPoint(3, sf::Vector2f(0.f, decisionHeight / 2.f));              // Left Middle

    shapes.decisionEntity.setFillColor(color);
    shapes.decisionEntity.setPosition(sf::Vector2f{ decisionPositionX, decisionPositionY });

    // Input Entity
    float inputPositionX = decisionPositionX + decisionWidth + parentShapesOffset;
    float inputPositionY = horizonLineY - (inputOutputHeight / 2.f);
    float inputIndent = (inputOutputBigBase - inputOutputSmallBase) / 2.f;

    shapes.inputEntity.setPointCount(4);
    shapes.inputEntity.setPoint(0, sf::Vector2f(0.f, 0.f));                                  // Top Left
    shapes.inputEntity.setPoint(1, sf::Vector2f(inputOutputBigBase, 0.f));                   // Top Right
    shapes.inputEntity.setPoint(2, sf::Vector2f(inputOutputBigBase - inputIndent, inputOutputHeight)); // Bottom Right
    shapes.inputEntity.setPoint(3, sf::Vector2f(inputIndent, inputOutputHeight));            // Bottom Left

    shapes.inputEntity.setFillColor(color);
    shapes.inputEntity.setPosition(sf::Vector2f{ inputPositionX, inputPositionY });

    // Output Entity
    float outputPositionX = inputPositionX + inputOutputBigBase + parentShapesOffset;
    float outputPositionY = horizonLineY - (inputOutputHeight / 2.f);
    float outputIndent = (inputOutputBigBase - inputOutputSmallBase) / 2.f;

    shapes.outputEntity.setPointCount(4);
    shapes.outputEntity.setPoint(0, sf::Vector2f(outputIndent, 0.f));                        // Top Left (indented)
    shapes.outputEntity.setPoint(1, sf::Vector2f(inputOutputBigBase - outputIndent, 0.f));   // Top Right (indented)
    shapes.outputEntity.setPoint(2, sf::Vector2f(inputOutputBigBase, inputOutputHeight));    // Bottom Right
    shapes.outputEntity.setPoint(3, sf::Vector2f(0.f, inputOutputHeight));                   // Bottom Left

    shapes.outputEntity.setFillColor(color);
    shapes.outputEntity.setPosition(sf::Vector2f{ outputPositionX, outputPositionY });

    // Stop Entity
    float stopPositionX = outputPositionX + inputOutputBigBase + parentShapesOffset;
    shapes.stopEntity.setRadius(radiusStartStop);
    shapes.stopEntity.setFillColor(color);
    shapes.stopEntity.setScale({ scaleX, scaleY });
    shapes.stopEntity.setPosition({ stopPositionX , horizonLineY - radiusStartStop * scaleY });


    //create text
    shapes.startText = new sf::Text(shapes.font, "Start", fontSize);
    shapes.startText->setFillColor(textColor);
    shapes.actionText = new sf::Text(shapes.font, "Action", fontSize);
    shapes.actionText->setFillColor(textColor);
    shapes.decisionText = new sf::Text(shapes.font, "Decision", fontSize);
    shapes.decisionText->setFillColor(textColor);
    shapes.inputText = new sf::Text(shapes.font, "Input", fontSize);
    shapes.inputText->setFillColor(textColor);
    shapes.outputText = new sf::Text(shapes.font, "Output", fontSize);
    shapes.outputText->setFillColor(textColor);
    shapes.stopText = new sf::Text(shapes.font, "Stop", fontSize);
    shapes.stopText->setFillColor(textColor);

    //calculate text position
    sf::FloatRect startBounds = shapes.startEntity.getGlobalBounds();
    float startCenterX = startBounds.position.x + startBounds.size.x / 2.f;
    float startCenterY = startBounds.position.y + startBounds.size.y / 2.f;
    sf::FloatRect startTextBounds = shapes.startText->getLocalBounds();
    float startOffsetX = startTextBounds.position.x + startTextBounds.size.x / 2.f;
    float startOffsetY = startTextBounds.position.y + startTextBounds.size.y / 2.f;
    shapes.startText->setPosition({ startCenterX - startOffsetX, startCenterY - startOffsetY });
    sf::Vector2f actionPosition = shapes.actionEntity.getPosition();
    sf::Vector2f actionSize = shapes.actionEntity.getSize();
    float actionCenterX = actionPosition.x + actionSize.x / 2.f;
    float actionCenterY = actionPosition.y + actionSize.y / 2.f;
    sf::FloatRect actionTextBounds = shapes.actionText->getLocalBounds();
    float actionOffsetX = actionTextBounds.position.x + actionTextBounds.size.x / 2.f;
    float actionOffsetY = actionTextBounds.position.y + actionTextBounds.size.y / 2.f;
    shapes.actionText->setPosition({ actionCenterX - actionOffsetX , actionCenterY - actionOffsetY });
    sf::FloatRect decisionBounds = shapes.decisionEntity.getGlobalBounds();
    float decisionCenterX = decisionBounds.position.x + decisionBounds.size.x / 2.f;
    float decisionCenterY = decisionBounds.position.y + decisionBounds.size.y / 2.f;
    sf::FloatRect decisionTextBounds = shapes.decisionText->getLocalBounds();
    float decisionOffsetX = decisionTextBounds.position.x + decisionTextBounds.size.x / 2.f;
    float decisionOffsetY = decisionTextBounds.position.y + decisionTextBounds.size.y / 2.f;
    shapes.decisionText->setPosition({ decisionCenterX - decisionOffsetX , decisionCenterY - decisionOffsetY });
    sf::FloatRect inputBounds = shapes.inputEntity.getGlobalBounds();
    float inputCenterX = inputBounds.position.x + inputBounds.size.x / 2.f;
    float inputCenterY = inputBounds.position.y + inputBounds.size.y / 2.f;
    sf::FloatRect inputTextBounds = shapes.inputText->getLocalBounds();
    float inputOffsetX = inputTextBounds.position.x + inputTextBounds.size.x / 2.f;
    float inputOffsetY = inputTextBounds.position.y + inputTextBounds.size.y / 2.f;
    shapes.inputText->setPosition({ inputCenterX - inputOffsetX , inputCenterY - inputOffsetY });
    sf::FloatRect outputBounds = shapes.outputEntity.getGlobalBounds();
    float outputCenterX = outputBounds.position.x + outputBounds.size.x / 2.f;
    float outputCenterY = outputBounds.position.y + outputBounds.size.y / 2.f;
    sf::FloatRect outputTextBounds = shapes.outputText->getLocalBounds();
    float outputOffsetX = outputTextBounds.position.x + outputTextBounds.size.x / 2.f;
    float outputOffsetY = outputTextBounds.position.y + outputTextBounds.size.y / 2.f;
    shapes.outputText->setPosition({ outputCenterX - outputOffsetX , outputCenterY - outputOffsetY });
    sf::FloatRect stopBounds = shapes.stopEntity.getGlobalBounds();
    float stopCenterX = stopBounds.position.x + stopBounds.size.x / 2.f;
    float stopCenterY = stopBounds.position.y + stopBounds.size.y / 2.f;
    sf::FloatRect stopTextBounds = shapes.stopText->getLocalBounds();
    float stopOffsetX = stopTextBounds.position.x + stopTextBounds.size.x / 2.f;
    float stopOffsetY = stopTextBounds.position.y + stopTextBounds.size.y / 2.f;
    shapes.stopText->setPosition({ stopCenterX - stopOffsetX , stopCenterY - stopOffsetY });


    return shapes;
}



void drawParentShapes(sf::RenderWindow& window, parentShapes& shapes)
{
    window.draw(shapes.startEntity);
    window.draw(shapes.actionEntity);
    window.draw(shapes.decisionEntity);
    window.draw(shapes.inputEntity);
    window.draw(shapes.outputEntity);
    window.draw(shapes.stopEntity);
    window.draw(*shapes.startText);
    window.draw(*shapes.actionText);
    window.draw(*shapes.decisionText);
    window.draw(*shapes.inputText);
    window.draw(*shapes.outputText);
    window.draw(*shapes.stopText);
    window.draw(shapes.topLine);
    window.draw(shapes.bottomLine);
}
