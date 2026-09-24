#include <SFML/Graphics.hpp>
#include <cmath>

#include "blocks.h"

#define arrowColor sf::Color::White
#define lineThickness 2.f
#define arrowWidth 5.f
#define arrowHeight 10.f
#define lineStickingOut 15.f
#define distArrBlock 20.f

#define linePointCount 6

class Arrow {
public:

	anchor* originAnchor = nullptr;
	anchor* destinationAnchor = nullptr;

	Block* from = nullptr;
	Block* to = nullptr;

	sf::VertexArray line;
	sf::ConvexShape arrowHead;

	Arrow(anchor* from, anchor* to) : originAnchor(from), destinationAnchor(to) {

		line.setPrimitiveType(sf::PrimitiveType::LineStrip);
		line.resize(linePointCount);

		arrowHead.setPointCount(3);
		arrowHead.setFillColor(arrowColor);

		arrowHead.setPoint(0, sf::Vector2f{ -arrowWidth, -arrowHeight });
		arrowHead.setPoint(1, sf::Vector2f{ 0, 0 });
		arrowHead.setPoint(2, sf::Vector2f{ arrowWidth, -arrowHeight });

		if (destinationAnchor->positionType == anchorLeftType)
			arrowHead.setRotation(sf::degrees(270.f));

	}

	void updatePosition() {
		sf::Vector2f originAnchorCenter = originAnchor->shape.getGlobalBounds().getCenter();
		sf::Vector2f destinationAnchorCenter = destinationAnchor->shape.getGlobalBounds().getCenter();

		line[0].position = originAnchorCenter;

		if (originAnchor->positionType == anchorTopType) line[1].position = originAnchorCenter - sf::Vector2f{ 0.f, lineStickingOut };
		else if (originAnchor->positionType == anchorRightType) line[1].position = originAnchorCenter + sf::Vector2f{ lineStickingOut, 0.f };
		else if (originAnchor->positionType == anchorBottomType) line[1].position = originAnchorCenter + sf::Vector2f{ 0.f, lineStickingOut };
		else line[1].position = originAnchorCenter - sf::Vector2f{ lineStickingOut, 0.f };

		sf::Vector2f targetEntryPos;
		if (destinationAnchor->positionType == anchorTopType) targetEntryPos = destinationAnchorCenter - sf::Vector2f{ 0.f, lineStickingOut };
		else if (destinationAnchor->positionType == anchorRightType) targetEntryPos = destinationAnchorCenter + sf::Vector2f{ lineStickingOut, 0.f };
		else if (destinationAnchor->positionType == anchorBottomType) targetEntryPos = destinationAnchorCenter + sf::Vector2f{ 0.f, lineStickingOut };
		else targetEntryPos = destinationAnchorCenter - sf::Vector2f{ lineStickingOut, 0.f };

		line[4].position = targetEntryPos;

		if (destinationAnchor->positionType == anchorTopType) {
			if (line[1].position.y < line[4].position.y) {
				float midY = (line[1].position.y + line[4].position.y) / 2.f;
				line[2].position = sf::Vector2f{ line[1].position.x, midY };
				line[3].position = sf::Vector2f{ line[4].position.x, midY };
			}
			else {
				float midX = (originAnchorCenter.x + destinationAnchorCenter.x) / 2.f;
				line[2].position = sf::Vector2f{ midX, line[1].position.y };
				line[3].position = sf::Vector2f{ midX, line[4].position.y };
			}
		}
		else if (originAnchor->positionType == anchorRightType && destinationAnchor->positionType == anchorLeftType) {
			float midY = (line[1].position.y + line[4].position.y) / 2.f;
			line[2].position = sf::Vector2f{ line[1].position.x, midY };
			line[3].position = sf::Vector2f{ line[4].position.x, midY };
		}
		else if (originAnchor->positionType == anchorBottomType && destinationAnchor->positionType == anchorLeftType) {
			line[2].position = sf::Vector2f{ targetEntryPos.x, line[1].position.y };
			line[3].position = targetEntryPos;
		}
		else {
			if (originAnchor->positionType == anchorBottomType) {
				if (destinationAnchor->positionType == anchorLeftType)
					line[2].position = sf::Vector2f{ destinationAnchorCenter.x - lineStickingOut, originAnchorCenter.y + lineStickingOut };
			}
			else if (originAnchor->positionType == anchorRightType)
				line[2].position = sf::Vector2f{ destinationAnchorCenter.x, originAnchorCenter.y };

			line[3].position = destinationAnchorCenter;
		}

		line[5].position = destinationAnchorCenter;
		arrowHead.setPosition(destinationAnchorCenter);
	}

	float calculateDistanceFromCursor(sf::Vector2f cursor, int indexPoint1, int indexPoint2) {

		sf::Vector2f point1 = line[indexPoint1].position;
		sf::Vector2f point2 = line[indexPoint2].position;

		float l2 = std::pow((point1.x - point2.x), 2) + std::pow((point1.y - point2.y), 2);
		if (l2 == 0.0f) return std::sqrt(std::pow(cursor.x - point1.x, 2) + std::pow(cursor.y - point1.y, 2));


		float t = std::max(0.0f, std::min(1.0f, ((cursor.x - point1.x) * (point2.x - point1.x) + (cursor.y - point1.y) * (point2.y - point1.y)) / l2));
		sf::Vector2f projection = point1 + t * (point2 - point1);

		return std::sqrt(std::pow((cursor.x - projection.x), 2) + std::pow((cursor.y - projection.y), 2));
	}

	void removeConnection() {
		originAnchor->isSelected = false;
		destinationAnchor->isSelected = false;

		originAnchor->shape.setOutlineThickness(0.f);
		destinationAnchor->shape.setOutlineThickness(0.f);

		if (from->blockType == decisionBlockType && originAnchor->positionType == anchorRightType)
			from->connectedToFalse = nullptr;
		else
			from->connectedToTruth = nullptr;

		originAnchor = nullptr;
		destinationAnchor = nullptr;

		from = nullptr;
		to = nullptr;
	}

	void draw(sf::RenderWindow& window) {
		updatePosition();
		window.draw(line);
		window.draw(arrowHead);
	}

};