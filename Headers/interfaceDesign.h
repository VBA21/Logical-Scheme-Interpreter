#include <SFML/Graphics.hpp>

#define headerY 100

sf::Font font;

auto calculateTextPosition = [](sf::Text* text, sf::RectangleShape& button) {
	sf::FloatRect buttonBounds = button.getGlobalBounds();
	float startCenterX = buttonBounds.position.x + buttonBounds.size.x / 2.f;
	float startCenterY = buttonBounds.position.y + buttonBounds.size.y / 2.f;
	sf::FloatRect TextBounds = text->getLocalBounds();
	float startOffsetX = TextBounds.position.x + TextBounds.size.x / 2.f;
	float startOffsetY = TextBounds.position.y + TextBounds.size.y / 2.f;
	text->setPosition({ startCenterX - startOffsetX, startCenterY - startOffsetY });
	};

struct mainMenu {
	sf::RectangleShape background;
	sf::Text* titleText;

	sf::RectangleShape start;
	sf::Text* startText;

	sf::RectangleShape open;
	sf::Text* openText;

	sf::RectangleShape help;
	sf::Text* helpText;

	sf::RectangleShape exit;
	sf::Text* exitText;

};

struct HUD {

	sf::RectangleShape goBack;
	sf::Text* goBackText;

	sf::RectangleShape save;
	sf::Text* saveText;

	sf::RectangleShape run;
	sf::Text* runText;

	sf::RectangleShape codeSpace;
	sf::RectangleShape codeTextBar;
	sf::Text* codeText;

};

mainMenu initializeMainMenu(int windowWidth, int windowHeight) {

	mainMenu menu;

	if (!font.openFromFile("C:/Users/Stefan/Desktop/Fonts/Roboto/Roboto-Italic-VariableFont_wdth,wght.ttf"))
	{
		throw std::runtime_error("Failed to load font from file.");
	}

	menu.background.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
	menu.background.setFillColor(sf::Color(50, 50, 50));

	menu.titleText = new sf::Text(font, "InterSchem", 48);
	menu.titleText->setFillColor(sf::Color::White);

	sf::FloatRect titleBounds = menu.titleText->getGlobalBounds();
	menu.titleText->setPosition(sf::Vector2f(windowWidth / 2.f - titleBounds.size.x / 2.f, headerY));

	// Buttons
	menu.start.setSize(sf::Vector2f(200.f, 50.f));
	menu.start.setFillColor(sf::Color(100, 100, 200));
	menu.start.setPosition(sf::Vector2f(windowWidth / 2.f - menu.start.getSize().x / 2.f, windowHeight / 2.f - 100.f));

	menu.open.setSize(sf::Vector2f(200.f, 50.f));
	menu.open.setFillColor(sf::Color(100, 100, 200));
	menu.open.setPosition(sf::Vector2f(windowWidth / 2.f - menu.open.getSize().x / 2.f, windowHeight / 2.f));

	menu.help.setSize(sf::Vector2f(200.f, 50.f));
	menu.help.setFillColor(sf::Color(100, 100, 200));
	menu.help.setPosition(sf::Vector2f(windowWidth / 2.f - menu.help.getSize().x / 2.f, windowHeight / 2.f + 100.f));


	menu.exit.setSize(sf::Vector2f(200.f, 50.f));
	menu.exit.setFillColor(sf::Color(100, 100, 200));
	menu.exit.setPosition(sf::Vector2f(windowWidth / 2.f - menu.exit.getSize().x / 2.f, windowHeight / 2.f + 200.f));

	// Text
	menu.startText = new sf::Text(font, "Start", 24);
	menu.startText->setFillColor(sf::Color::White);
	calculateTextPosition(menu.startText, menu.start);

	menu.openText = new sf::Text(font, "Open", 24);
	menu.openText->setFillColor(sf::Color::White);
	calculateTextPosition(menu.openText, menu.open);

	menu.helpText = new sf::Text(font, "Help", 24);
	menu.helpText->setFillColor(sf::Color::White);
	calculateTextPosition(menu.helpText, menu.help);

	menu.exitText = new sf::Text(font, "Exit", 24);
	menu.exitText->setFillColor(sf::Color::White);
	calculateTextPosition(menu.exitText, menu.exit);

	return menu;
}

HUD initializeHUD(int windowWidth, int windowHeight) {

	HUD userInterface;

	if (!font.openFromFile("C:/Users/Stefan/Desktop/Fonts/Roboto/Roboto-Italic-VariableFont_wdth,wght.ttf"))
	{
		throw std::runtime_error("Failed to load font from file.");
	}

	userInterface.run.setSize(sf::Vector2f(100.f, 50.f));
	userInterface.run.setFillColor(sf::Color(50, 200, 100));
	userInterface.run.setPosition(sf::Vector2f(windowWidth / 2.f - userInterface.run.getSize().x / 2.f, windowHeight - 100.f));

	userInterface.runText = new sf::Text(font, "RUN", fontSize);
	userInterface.runText->setFillColor(textColor);
	calculateTextPosition(userInterface.runText, userInterface.run);

	userInterface.save.setSize(sf::Vector2f(100.f, 50.f));
	userInterface.save.setFillColor(sf::Color(50, 100, 100));
	userInterface.save.setPosition(sf::Vector2f(windowWidth / 4.f - userInterface.save.getSize().x / 2.f, windowHeight - 100.f));

	userInterface.saveText = new sf::Text(font, "Save", 20);
	userInterface.saveText->setFillColor(sf::Color::White);
	calculateTextPosition(userInterface.saveText, userInterface.save);

	userInterface.goBack.setSize(sf::Vector2f(100.f, 50.f));
	userInterface.goBack.setFillColor(sf::Color(50, 100, 100));
	userInterface.goBack.setPosition(sf::Vector2f(windowWidth / 6.f - userInterface.goBack.getSize().x / 2.f, windowHeight - 100.f));

	userInterface.goBackText = new sf::Text(font, "Go Back", 20);
	userInterface.goBackText->setFillColor(sf::Color::White);
	calculateTextPosition(userInterface.goBackText, userInterface.goBack);

	userInterface.codeSpace.setSize(sf::Vector2f(500.f, windowHeight));
	userInterface.codeSpace.setFillColor(sf::Color(40, 40, 40));
	userInterface.codeSpace.setOutlineColor(sf::Color::White);
	userInterface.codeSpace.setOutlineThickness(-2.f);
	userInterface.codeSpace.setPosition(sf::Vector2f(windowWidth - 500.f, 0.f));

	userInterface.codeTextBar.setSize(sf::Vector2f(500.f, 30.f));
	userInterface.codeTextBar.setFillColor(sf::Color(60, 60, 60));
	userInterface.codeTextBar.setOutlineColor(sf::Color::Black);
	userInterface.codeTextBar.setOutlineThickness(1.f);
	userInterface.codeTextBar.setPosition(sf::Vector2f(windowWidth - 500.f, 0.f));

	userInterface.codeText = new sf::Text(font, "C++ Code", fontSize);
	userInterface.codeText->setFillColor(textColor);
	userInterface.codeText->setPosition(sf::Vector2f(windowWidth - 500.f + 10.f, 2.f));
	userInterface.codeText->setStyle(sf::Text::Style::Bold);
	return userInterface;

}

void drawMainMenu(sf::RenderWindow& window, mainMenu& menu) {
	window.draw(menu.background);
	window.draw(*menu.titleText);
	window.draw(menu.start);
	window.draw(*menu.startText);
	window.draw(menu.open);
	window.draw(*menu.openText);
	window.draw(menu.help);
	window.draw(*menu.helpText);
	window.draw(menu.exit);
	window.draw(*menu.exitText);
}

void showHelpWindow(sf::RenderWindow& window) {

	sf::Font helpFont;
	if (!helpFont.openFromFile("C:/Users/Stefan/Desktop/Fonts/Roboto/Roboto-Italic-VariableFont_wdth,wght.ttf"))
	{
		throw std::runtime_error("Failed to load font from file.");
	}

	sf::RectangleShape background(sf::Vector2f(1600.f, 1000.f));
	background.setFillColor(sf::Color(40, 40, 40));

	sf::Text title(helpFont, "InterSchem Help", 36);
	title.setFillColor(sf::Color::White);
	title.setStyle(sf::Text::Style::Bold);
	sf::FloatRect titleBounds = title.getGlobalBounds();
	title.setPosition(sf::Vector2f(800 - titleBounds.size.x / 2.f, 20.f));

	std::vector<sf::Text> helpTexts;
	std::vector<std::string> helpContent = {
		"Introducere:",
		"- Trage blocurile din bara de instrumente pentru a crea schema",
		"- Click pe un bloc pentru a-l selecta",
		"- Click dreapta pentru a sterge blocuri sau sageti",
		"",
		"Tipuri de Blocuri:",
		"- Start: Inceputul programului (obligatoriu)",
		"- Action: Efectueaza calcule si atribuiri (ex: a = b + c)",
		"- Decision: Expresii conditionale (ex: a > 5)",
		"- Input: Citeste date de la utilizator",
		"- Output: Afiseaza valori sau text (foloseste ghilimele pentru text)",
		"- Stop: Sfarsitul programului (obligatoriu pe fiecare ramura)",
		"",
		"Conectarea Blocurilor:",
		"- Click pe punctele de ancorare rosii pentru a crea sageti",
		"- Primul click: sursa",
		"- Al doilea click: destinatie",
		"- Blocurile Decision au ramuri Adevarat (jos) si Fals (dreapta)",
		"- Ancora din stanga a unui bloc Decision e pentru instructiuni repetitive",
		"",
		"Editarea Blocurilor:",
		"- Dublu-click pe text-ul dintr-un bloc pentru a-l edita",
		"- Apasa Enter, Escape sau click in afara blocului pentru a confirma",
		"",
		"Rularea Programelor:",
		"- Apasa RUN pentru a executa schema",
		"- Introdu valori",
		"- Vezi rezultatul in panoul de jos",
		"",
		"Salvare/Incarcare:",
		"- Foloseste Save pentru a salva schema",
		"- Foloseste Open pentru a incarca proiecte existente"
	};

	float yPos = 80.f;
	for (const auto& line : helpContent) {
		sf::Text text(helpFont, line, 30);
		text.setFillColor(sf::Color(220, 220, 220));

		if (line.find(":") != std::string::npos && !line.empty() && line[0] != '-') {
			text.setStyle(sf::Text::Style::Bold);
			text.setFillColor(sf::Color(150, 200, 255));
		}

		text.setPosition(sf::Vector2f(40.f, yPos));
		helpTexts.push_back(text);
		yPos += 40.f;
	}

	sf::RectangleShape closeButton(sf::Vector2f(120.f, 40.f));
	closeButton.setFillColor(sf::Color(100, 100, 200));
	sf::FloatRect closeButtonBounds = closeButton.getLocalBounds();
	closeButton.setPosition(sf::Vector2f(800.f - closeButtonBounds.size.x / 2.f, 900.f));

	sf::Text closeText(helpFont, "Close", 20);
	closeText.setFillColor(sf::Color::White);
	sf::FloatRect closeBounds = closeText.getLocalBounds();
	closeText.setPosition(sf::Vector2f(800.f - closeBounds.size.x / 2.f, 905.f));

	float scrollOffset = 0.f;
	const float maxScroll = std::max(0.f, yPos - 900.f);

	bool helpActive = true;

	while (window.isOpen() && helpActive) {
		while (auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseEvent->button == sf::Mouse::Button::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(mouseEvent->position);
					if (closeButton.getGlobalBounds().contains(mousePos)) {
						helpActive = false;
					}
				}
			}
			else if (const auto* wheelEvent = event->getIf<sf::Event::MouseWheelScrolled>()) {
				scrollOffset += wheelEvent->delta * 20.f;
				scrollOffset = std::max(-maxScroll, std::min(0.f, scrollOffset));
			}
		}

		window.clear();
		window.draw(background);
		window.draw(title);

		for (auto& text : helpTexts) {
			sf::Vector2f pos = text.getPosition();
			text.setPosition(sf::Vector2f(pos.x, pos.y + scrollOffset));
			if (text.getPosition().y > 60.f && text.getPosition().y < 900.f) {
				window.draw(text);
			}
			text.setPosition(pos);
		}

		window.draw(closeButton);
		window.draw(closeText);
		window.display();
	}
}


void openProject() {

}

void saveProject() {

}

int inMainMenu(sf::RenderWindow& window, mainMenu& menu) {

	while (window.isOpen()) {

		while (auto event = window.pollEvent()) {

			if (event->is<sf::Event::Closed>()) {
				window.close();
				return 1;
			}
			else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseEvent->button == sf::Mouse::Button::Left) {

					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

					if (menu.start.getGlobalBounds().contains(mousePos)) {
						return 0;
					}
					else if (menu.open.getGlobalBounds().contains(mousePos)) {
						return 100;
					}
					else if (menu.help.getGlobalBounds().contains(mousePos)) {
						return 200;
					}
					else if (menu.exit.getGlobalBounds().contains(mousePos)) {
						window.close();
						return 1;
					}
				}
			}
		}
		window.clear();
		drawMainMenu(window, menu);
		window.display();
	}

	return 0;
}

void drawHUD(sf::RenderWindow& window, HUD& UI) {
	window.draw(UI.run);
	window.draw(*UI.runText);
	window.draw(UI.save);
	window.draw(*UI.saveText);
	window.draw(UI.goBack);
	window.draw(*UI.goBackText);
	window.draw(UI.codeSpace);
	window.draw(UI.codeTextBar);
	window.draw(*UI.codeText);
}