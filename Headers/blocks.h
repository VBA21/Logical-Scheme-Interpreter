#include <SFML/Graphics.hpp>
#include <iostream>

#define anchorRadius 4.f
#define anchorColor sf::Color(255, 50, 50)
#define anchorOutlineColor sf::Color::White
#define anchorOutlineThickness 1.f

#define startBlockColor sf::Color(102, 204, 102)
#define actionBlockColor sf::Color(102, 153, 255)
#define decisionBlockColor sf::Color(255, 204, 0)
#define inputBlockColor sf::Color(153, 204, 255)
#define outputBlockColor sf::Color(153, 200, 204)
#define stopBlockColor sf::Color(102, 51, 153)

#define startBlockType 0
#define actionBlockType 1
#define decisionBlockType 2
#define inputBlockType 3
#define outputBlockType 4
#define stopBlockType 5 

#define anchorTopType 0
#define anchorRightType 1
#define anchorBottomType 2
#define anchorLeftType 3

#define minBlockWidth 100.f
#define minBlockHeight 50.f
#define textPadding 20.f

auto initAnchor = [](sf::CircleShape& a) {
    a.setRadius(anchorRadius);
    a.setFillColor(anchorColor);
    a.setOutlineColor(anchorOutlineColor);
    a.setOutlineThickness(0.f);
    };


struct anchor {
    sf::CircleShape shape;
    bool isSelected = false;
    int positionType;
    int blockType;
};

class Block {
public:
    bool belowTopLine = false;

    bool selected = false;
    Block* connectedToTruth = nullptr;
    Block* connectedToFalse = nullptr;
    int blockType;

    sf::Text* text;
    static sf::Font font;

    std::vector<anchor> anchorPoints;

    virtual ~Block() {}

    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void setPosition(sf::Vector2f pos) = 0;
    // virtual sf::Vector2f shapeLowestCoordinate() = 0; // Needs to be implemented (if it is the case, you can use anchorBottom coordinates)
    virtual sf::Vector2f getPosition() = 0;
    virtual sf::FloatRect getBounds() = 0;

    virtual bool hasEditableText() const { return false; }
    virtual std::string getText() const { return ""; }
    virtual void setText(const std::string& str) {}
    virtual void updateSize() {}
    virtual anchor* getAnchorByType(int type) {
        for (auto& a : anchorPoints) {
            if (a.positionType == type) return &a;
        }
        return nullptr;
    }
};


class startBlock : public Block {
public:
    sf::CircleShape shape;

    startBlock(sf::CircleShape& originalShape) : shape(originalShape) {

        shape.setFillColor(startBlockColor);

        anchor a;

        initAnchor(a.shape);
        a.blockType = startBlockType;

        a.positionType = anchorBottomType;
        this->anchorPoints.push_back(a);

        text = new sf::Text(font, "Start", fontSize);
        text->setFillColor(textColor);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);

        for (auto& a : anchorPoints)
            window.draw(a.shape);
        sf::FloatRect blkBounds = shape.getGlobalBounds();
        float blkCenterX = blkBounds.position.x + blkBounds.size.x / 2.f;
        float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
        sf::FloatRect textBounds = text->getLocalBounds();
        float textOffsetX = textBounds.position.x + textBounds.size.x / 2.f;
        float textOffsetY = textBounds.position.y + textBounds.size.y / 2.f;
        text->setPosition({ blkCenterX - textOffsetX, blkCenterY - textOffsetY });
        window.draw(*text);
    }

    void setPosition(sf::Vector2f pos) override {
        shape.setPosition(pos);

        sf::Vector2f center = shape.getPosition();
        float verticalRadius = shape.getRadius() * shape.getScale().y;


        anchorPoints[0].shape.setPosition(sf::Vector2f{ pos.x + shape.getRadius() * 2 - anchorRadius, pos.y + shape.getRadius() * 2 - anchorRadius });
    }

    virtual sf::Vector2f getPosition() override {
        return shape.getPosition();
    }

    sf::FloatRect getBounds() override {
        return shape.getGlobalBounds();
    }
};


class actionBlock : public Block {
public:
    sf::RectangleShape shape;
    sf::CircleShape anchorPointAbove, anchorPointBelow;
    actionBlock(sf::RectangleShape& originalShape) : shape(originalShape) {

        shape.setFillColor(actionBlockColor);
        anchor a;

        initAnchor(a.shape);
        a.blockType = actionBlockType;

        a.positionType = anchorTopType;
        this->anchorPoints.push_back(a);

        a.positionType = anchorBottomType;
        this->anchorPoints.push_back(a);

        text = new sf::Text(font, "Action", fontSize);
        text->setFillColor(textColor);
    }

    bool hasEditableText() const override { return true; }

    std::string getText() const override {
        return text->getString();
    }

    void setText(const std::string& str) override {
        text->setString(str);
        updateSize();
    }

    void updateSize() override
    {
        sf::FloatRect textBounds = text->getLocalBounds();
        float newWidth = std::max(minBlockWidth, textBounds.size.x + textPadding * 2);
        float newHeight = std::max(minBlockHeight, textBounds.size.y + textPadding);

        sf::FloatRect currentBounds = shape.getGlobalBounds();
        sf::Vector2f currentCenter(currentBounds.position.x + currentBounds.size.x / 2.f, currentBounds.position.y + currentBounds.size.y / 2.f);

        shape.setSize(sf::Vector2f(newWidth, newHeight));

        sf::Vector2f newPos(currentCenter.x - newWidth / 2.f, currentCenter.y - newHeight / 2.f);
        setPosition(newPos);
    }
    void draw(sf::RenderWindow& window) override {
        window.draw(shape);

        for (auto& a : anchorPoints)
            window.draw(a.shape);

        sf::Vector2f blkPosition = shape.getPosition();
        sf::Vector2f blkSize = shape.getSize();
        float blkCenterX = blkPosition.x + blkSize.x / 2.f;
        float blkCenterY = blkPosition.y + blkSize.y / 2.f;
        sf::FloatRect textBounds = text->getLocalBounds();
        float textOffsetX = textBounds.position.x + textBounds.size.x / 2.f;
        float textOffsetY = textBounds.position.y + textBounds.size.y / 2.f;
        text->setPosition({ blkCenterX - textOffsetX , blkCenterY - textOffsetY });
        window.draw(*text);
    }

    void setPosition(sf::Vector2f pos) override {
        shape.setPosition(pos);

        float center = pos.x + shape.getSize().x / 2;

        anchorPoints[0].shape.setPosition(sf::Vector2f{ center - anchorRadius, pos.y - anchorRadius });
        anchorPoints[1].shape.setPosition(sf::Vector2f{ center - anchorRadius, pos.y + shape.getSize().y - anchorRadius });
    }

    virtual sf::Vector2f getPosition() override {
        return shape.getPosition();
    }

    sf::FloatRect getBounds() override {
        return shape.getGlobalBounds();
    }
};


class decisionBlock : public Block {
public:
    sf::ConvexShape shape;
    sf::CircleShape anchorTop, anchorRight, anchorBottom, anchorLeft;
    decisionBlock(sf::ConvexShape& originalShape) : shape(originalShape) {

        shape.setFillColor(decisionBlockColor);

        anchor a;

        initAnchor(a.shape);
        a.blockType = decisionBlockType;

        a.positionType = anchorTopType;
        this->anchorPoints.push_back(a);

        a.positionType = anchorRightType;
        this->anchorPoints.push_back(a);

        a.positionType = anchorBottomType;
        this->anchorPoints.push_back(a);

        a.positionType = anchorLeftType;
        this->anchorPoints.push_back(a);

        text = new sf::Text(font, "Decision", fontSize);
        text->setFillColor(textColor);
    }

    bool hasEditableText() const override { return true; }

    std::string getText() const override {
        return text->getString();
    }

    void setText(const std::string& str) override {
        text->setString(str);
        updateSize();
    }

    void updateSize() override
    {
        sf::FloatRect textBounds = text->getLocalBounds();
        float newWidth = std::max(minBlockWidth, textBounds.size.x + textPadding * 2.f);
        float newHeight = std::max(minBlockHeight, textBounds.size.y + textPadding);
        float indent = newWidth * 0.15f;

        sf::FloatRect currentBounds = shape.getGlobalBounds();
        sf::Vector2f currentCenter(currentBounds.position.x + currentBounds.size.x / 2.f, currentBounds.position.y + currentBounds.size.y / 2.f);

        shape.setPoint(0, sf::Vector2f(newWidth / 2.f, 0.f));
        shape.setPoint(1, sf::Vector2f(newWidth, newHeight / 2.f));
        shape.setPoint(2, sf::Vector2f(newWidth / 2.f, newHeight));
        shape.setPoint(3, sf::Vector2f(0.f, newHeight / 2.f));

        sf::Vector2f newPos(currentCenter.x - newWidth / 2.f, currentCenter.y - newHeight / 2.f);

        setPosition(newPos);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);

        for (auto& a : anchorPoints)
            window.draw(a.shape);

        sf::FloatRect blkBounds = shape.getGlobalBounds();
        float blkCenterX = blkBounds.position.x + blkBounds.size.x / 2.f;
        float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
        sf::FloatRect textBounds = text->getLocalBounds();
        float textOffsetX = textBounds.position.x + textBounds.size.x / 2.f;
        float textOffsetY = textBounds.position.y + textBounds.size.y / 2.f;
        text->setPosition({ blkCenterX - textOffsetX , blkCenterY - textOffsetY });
        window.draw(*text);
    }

    void setPosition(sf::Vector2f pos) override {
        shape.setPosition(pos);

        anchorPoints[0].shape.setPosition(sf::Vector2f{ pos.x + shape.getPoint(0).x - anchorRadius, pos.y + shape.getPoint(0).y - anchorRadius });
        anchorPoints[1].shape.setPosition(sf::Vector2f{ pos.x + shape.getPoint(1).x - anchorRadius, pos.y + shape.getPoint(1).y - anchorRadius });
        anchorPoints[2].shape.setPosition(sf::Vector2f{ pos.x + shape.getPoint(2).x - anchorRadius, pos.y + shape.getPoint(2).y - anchorRadius });
        anchorPoints[3].shape.setPosition(sf::Vector2f{ pos.x + shape.getPoint(3).x - anchorRadius, pos.y + shape.getPoint(3).y - anchorRadius });
    }

    virtual sf::Vector2f getPosition() override {
        return shape.getPosition();
    }

    sf::FloatRect getBounds() override {
        return shape.getGlobalBounds();
    }
};



class inputBlock : public Block {
public:
    sf::ConvexShape shape;
    sf::CircleShape anchorAbove, anchorBelow;

    inputBlock(sf::ConvexShape originalShape) : shape(originalShape) {

        shape.setFillColor(inputBlockColor);

        anchor a;

        initAnchor(a.shape);
        a.blockType = inputBlockType;

        a.positionType = anchorTopType;
        this->anchorPoints.push_back(a);

        a.positionType = anchorBottomType;
        this->anchorPoints.push_back(a);

        text = new sf::Text(font, "Input", fontSize);
        text->setFillColor(textColor);

    }

    bool hasEditableText() const override { return true; }

    std::string getText() const override {
        return text->getString();
    }

    void setText(const std::string& str) override {
        text->setString(str);
        updateSize();
    }

    void updateSize() override {
        sf::FloatRect textBounds = text->getLocalBounds();
        float newWidth = std::max(minBlockWidth, textBounds.size.x + textPadding * 2.f);
        float newHeight = std::max(minBlockHeight, textBounds.size.y + textPadding);
        float indent = newWidth * 0.15f;

        sf::FloatRect currentBounds = shape.getGlobalBounds();
        sf::Vector2f currentCenter(currentBounds.position.x + currentBounds.size.x / 2.f, currentBounds.position.y + currentBounds.size.y / 2.f);

        shape.setPointCount(4);
        shape.setPoint(0, sf::Vector2f(0.f, 0.f));
        shape.setPoint(1, sf::Vector2f(newWidth, 0.f));
        shape.setPoint(2, sf::Vector2f(newWidth - indent, newHeight));
        shape.setPoint(3, sf::Vector2f(indent, newHeight));
        sf::Vector2f newPos(currentCenter.x - newWidth / 2.f, currentCenter.y - newHeight / 2.f);

        setPosition(newPos);

    }
    void draw(sf::RenderWindow& window) override {
        window.draw(shape);

        for (auto& a : anchorPoints)
            window.draw(a.shape);

        sf::FloatRect blkBounds = shape.getGlobalBounds();
        float blkCenterX = blkBounds.position.x + blkBounds.size.x / 2.f;
        float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
        sf::FloatRect textBounds = text->getLocalBounds();
        float textOffsetX = textBounds.position.x + textBounds.size.x / 2.f;
        float textOffsetY = textBounds.position.y + textBounds.size.y / 2.f;
        text->setPosition({ blkCenterX - textOffsetX , blkCenterY - textOffsetY });
        window.draw(*text);
    }

    void setPosition(sf::Vector2f pos) override {
        shape.setPosition(pos);

        sf::Vector2f center = shape.getGlobalBounds().getCenter();
        sf::Vector2f size = center - shape.getPosition();

        anchorPoints[0].shape.setPosition(sf::Vector2f{ pos.x + size.x - anchorRadius, pos.y + shape.getPoint(0).y - anchorRadius });
        anchorPoints[1].shape.setPosition(sf::Vector2f{ pos.x + size.x - anchorRadius, pos.y + shape.getPoint(2).y - anchorRadius });
    }

    virtual sf::Vector2f getPosition() override {
        return shape.getPosition();
    }

    sf::FloatRect getBounds() override {
        return shape.getGlobalBounds();
    }
};


class outputBlock : public Block {
public:
    sf::ConvexShape shape;
    sf::CircleShape anchorAbove, anchorBelow;
    outputBlock(sf::ConvexShape originalShape) : shape(originalShape) {
        shape.setFillColor(outputBlockColor);
        anchor a;

        initAnchor(a.shape);
        a.blockType = outputBlockType;

        a.positionType = anchorTopType;
        this->anchorPoints.push_back(a);

        a.positionType = anchorBottomType;
        this->anchorPoints.push_back(a);

        text = new sf::Text(font, "Output", fontSize);
        text->setFillColor(textColor);
    }

    bool hasEditableText() const override { return true; }

    std::string getText() const override {
        return text->getString();
    }

    void setText(const std::string& str) override {
        text->setString(str);
        updateSize();
    }

    void updateSize() override {
        sf::FloatRect textBounds = text->getLocalBounds();
        float newWidth = std::max(minBlockWidth, textBounds.size.x + textPadding * 2.f);
        float newHeight = std::max(minBlockHeight, textBounds.size.y + textPadding);
        float indent = newWidth * 0.15f;

        sf::Vector2f currentPos = shape.getPosition();

        sf::FloatRect currentBounds = shape.getGlobalBounds();
        sf::Vector2f currentCenter(currentBounds.position.x + currentBounds.size.x / 2.f, currentBounds.position.y + currentBounds.size.y / 2.f);

        shape.setPointCount(4);
        shape.setPoint(0, sf::Vector2f(indent, 0.f));
        shape.setPoint(1, sf::Vector2f(newWidth - indent, 0.f));
        shape.setPoint(2, sf::Vector2f(newWidth, newHeight));
        shape.setPoint(3, sf::Vector2f(0.f, newHeight));

        sf::Vector2f newPos(currentCenter.x - newWidth / 2.f, currentCenter.y - newHeight / 2.f);

        setPosition(newPos);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);

        for (auto& a : anchorPoints)
            window.draw(a.shape);
        sf::FloatRect blkBounds = shape.getGlobalBounds();
        float blkCenterX = blkBounds.position.x + blkBounds.size.x / 2.f;
        float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
        sf::FloatRect textBounds = text->getLocalBounds();
        float textOffsetX = textBounds.position.x + textBounds.size.x / 2.f;
        float textOffsetY = textBounds.position.y + textBounds.size.y / 2.f;
        text->setPosition({ blkCenterX - textOffsetX , blkCenterY - textOffsetY });
        window.draw(*text);
    }

    void setPosition(sf::Vector2f pos) override {
        shape.setPosition(pos);

        sf::Vector2f center = shape.getGlobalBounds().getCenter();
        sf::Vector2f size = center - shape.getPosition();

        anchorPoints[0].shape.setPosition(sf::Vector2f{ pos.x + size.x - anchorRadius, pos.y + shape.getPoint(0).y - anchorRadius });
        anchorPoints[1].shape.setPosition(sf::Vector2f{ pos.x + size.x - anchorRadius, pos.y + shape.getPoint(2).y - anchorRadius });
    }

    virtual sf::Vector2f getPosition() override {
        return shape.getPosition();
    }

    sf::FloatRect getBounds() override {
        return shape.getGlobalBounds();
    }
};


class stopBlock : public Block {
public:
    sf::CircleShape shape;
    sf::CircleShape anchorPoint;
    stopBlock(sf::CircleShape originalShape) : shape(originalShape) {
        shape.setFillColor(stopBlockColor);

        anchor a;

        initAnchor(a.shape);
        a.blockType = stopBlockType;

        a.positionType = anchorTopType;
        this->anchorPoints.push_back(a);

        text = new sf::Text(font, "Stop", fontSize);
        text->setFillColor(textColor);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);

        for (auto& a : anchorPoints)
            window.draw(a.shape);
        sf::FloatRect blkBounds = shape.getGlobalBounds();
        float blkCenterX = blkBounds.position.x + blkBounds.size.x / 2.f;
        float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
        sf::FloatRect textBounds = text->getLocalBounds();
        float textOffsetX = textBounds.position.x + textBounds.size.x / 2.f;
        float textOffsetY = textBounds.position.y + textBounds.size.y / 2.f;
        text->setPosition({ blkCenterX - textOffsetX , blkCenterY - textOffsetY });
        window.draw(*text);
    }

    void setPosition(sf::Vector2f pos) override {
        shape.setPosition(pos);

        anchorPoints[0].shape.setPosition(sf::Vector2f{ pos.x + shape.getRadius() * 2 - anchorRadius, pos.y - anchorRadius });
    }

    virtual sf::Vector2f getPosition() override {
        return shape.getPosition();
    }

    sf::FloatRect getBounds() override {
        return shape.getGlobalBounds();
    }
};