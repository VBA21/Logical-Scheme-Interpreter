#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <queue>
#include <cstring>
#include <optional>
#include <fstream>
#include "arrows.h"

extern std::vector<std::unique_ptr<Block>> blocks;
extern std::vector<std::unique_ptr<Arrow>> arrows;

#define startBlockType 0
#define actionBlockType 1
#define decisionBlockType 2
#define inputBlockType 3
#define outputBlockType 4
#define stopBlockType 5

#define errorOffset 10.f
#define codeSpacing 10.f
#define spaceSize 30.f

#define errorColor sf::Color::Red

std::string escapeText(std::string text) {
	if (text.empty()) return "<NEDEFINIT>";
	std::replace(text.begin(), text.end(), ' ', '_');
	return text;
}

std::string unescapeText(std::string text) {
	if (text == "<NEDEFINIT>") return "";
	std::replace(text.begin(), text.end(), '_', ' ');
	return text;
}

void saveProject(const std::string& filename, const std::vector<std::unique_ptr<Block>>& blocks, const std::vector<std::unique_ptr<Arrow>>& arrows) {

	std::ofstream file(filename);

	if (!file.is_open()) {
		return;
	}


	file << blocks.size() << "\n";

	for (const auto& b : blocks) {
		file << b->blockType << " " << b->getPosition().x << " " << b->getPosition().y << " " << escapeText(b->getText()) << "\n";
	}


	file << arrows.size() << "\n";

	for (const auto& a : arrows) {
		int fromIndex = -1;
		int toIndex = -1;

		for (size_t i = 0; i < blocks.size(); i++) {
			if (blocks[i].get() == a->from) fromIndex = i;
			if (blocks[i].get() == a->to) toIndex = i;
		}

		if (fromIndex != -1 && toIndex != -1) {
			file << fromIndex << " " << a->originAnchor->positionType << " "
				<< toIndex << " " << a->destinationAnchor->positionType << "\n";
		}
	}

	file.close();
	std::cout << "A reusit salvarea fisierului" << std::endl;
}

void loadProject(const std::string& filename, std::vector<std::unique_ptr<Block>>& blocks, std::vector<std::unique_ptr<Arrow>>& arrows, parentShapes& buildingShapes) {

	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "Nu am gasit fisierul";
		return;
	}

	blocks.clear();
	arrows.clear();

	int blockCount;
	file >> blockCount;

	for (int i = 0; i < blockCount; i++) {
		int type;
		float x, y;
		std::string textEncoded;

		file >> type >> x >> y >> textEncoded;
		std::string text = unescapeText(textEncoded);

		std::unique_ptr<Block> newBlock = nullptr;


		if (type == startBlockType) {

			newBlock = std::make_unique<startBlock>(buildingShapes.startEntity);
			newBlock->blockType = startBlockType;

		}
		else if (type == actionBlockType) {
			newBlock = std::make_unique<actionBlock>(buildingShapes.actionEntity);
			newBlock->blockType = actionBlockType;

		}
		else if (type == decisionBlockType) {
			newBlock = std::make_unique<decisionBlock>(buildingShapes.decisionEntity);
			newBlock->blockType = decisionBlockType;

		}
		else if (type == inputBlockType) {
			newBlock = std::make_unique<inputBlock>(buildingShapes.inputEntity);
			newBlock->blockType = inputBlockType;

		}
		else if (type == outputBlockType) {
			newBlock = std::make_unique<outputBlock>(buildingShapes.outputEntity);
			newBlock->blockType = outputBlockType;

		}
		else if (type == stopBlockType) {
			newBlock = std::make_unique<stopBlock>(buildingShapes.stopEntity);
			newBlock->blockType = stopBlockType;

		}

		if (newBlock) {
			if (newBlock->hasEditableText()) {
				newBlock->setText(text);
			}
			newBlock->setPosition(sf::Vector2f(x, y));
			blocks.push_back(std::move(newBlock));
		}
	}

	int arrowCount;
	file >> arrowCount;

	for (int i = 0; i < arrowCount; i++) {
		int fromIdx, fromAnchorType, toIdx, toAnchorType;
		file >> fromIdx >> fromAnchorType >> toIdx >> toAnchorType;
		if (fromIdx >= 0 && fromIdx < blocks.size() && toIdx >= 0 && toIdx < blocks.size()) {
			Block* bFrom = blocks[fromIdx].get();
			Block* bTo = blocks[toIdx].get();

			anchor* aFrom = bFrom->getAnchorByType(fromAnchorType);
			anchor* aTo = bTo->getAnchorByType(toAnchorType);

			if (aFrom && aTo) {
				auto newArrow = std::make_unique<Arrow>(aFrom, aTo);
				newArrow->from = bFrom;
				newArrow->to = bTo;

				if (bFrom->blockType == decisionBlockType && fromAnchorType == anchorRightType) {
					bFrom->connectedToFalse = bTo;
				}
				else {
					bFrom->connectedToTruth = bTo;
				}

				aFrom->isSelected = true;
				aTo->isSelected = true;

				arrows.push_back(std::move(newArrow));
			}
		}
	}
}

class RuntimeConsole {
public:
	std::vector<std::string> inputHistory;
	std::vector<std::string> outputHistory;
	std::string currentPrompt;
	std::string currentInput;
	bool waitingForInput = false;
	sf::Font font;

	float inputScroll = 0.0f;
	float outputScroll = 0.0f;
	const float panelWidth = 500.0f;
	const float lineHeight = 24.0f;

	sf::RectangleShape panelBg;

	RuntimeConsole() {
		if (!font.openFromFile("C:/Users/Stefan/Desktop/Fonts/Roboto/Roboto-Italic-VariableFont_wdth,wght.ttf")) {
			std::cout << "Nu s-a putut incarca fontul";
		}
	}

	void addOutput(const std::string& msg) {
		outputHistory.push_back(msg);
		if (outputHistory.size() * lineHeight > 200.0f) {
		}
	}

	void addInputLog(const std::string& msg) {
		inputHistory.push_back(msg);
	}

	void handleEvent(const sf::Event& event, sf::RenderWindow& window) {
		if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
			sf::Vector2u wSize = window.getSize();
			if (wheel->position.x > wSize.x - panelWidth) {
				if (wheel->position.y < wSize.y / 2) {
					inputScroll += wheel->delta * 20.0f;
					if (inputScroll > 0) inputScroll = 0;
				}
				else {
					outputScroll += wheel->delta * 20.0f;
					if (outputScroll > 0) outputScroll = 0;
				}
			}
		}
	}

	void draw(sf::RenderWindow& window) {
		sf::Vector2u wSize = window.getSize();
		float h = (float)wSize.y;
		float w = (float)wSize.x;
		float halfH = h / 2.0f;

		panelBg.setSize({ panelWidth, h });
		panelBg.setPosition({ w - panelWidth, 0.f });
		panelBg.setFillColor(sf::Color(40, 40, 40));
		panelBg.setOutlineColor(sf::Color::White);
		panelBg.setOutlineThickness(-2.f);
		window.draw(panelBg);

		sf::RectangleShape titleBar(sf::Vector2f(panelWidth, 30.f));
		titleBar.setFillColor(sf::Color(60, 60, 60));
		titleBar.setOutlineColor(sf::Color::Black);
		titleBar.setOutlineThickness(1.f);

		sf::Text title(font);
		title.setCharacterSize(20);
		title.setFillColor(sf::Color::White);
		title.setStyle(sf::Text::Style::Bold);

		titleBar.setPosition({ w - panelWidth, 0.f });
		window.draw(titleBar);
		title.setString("INPUT");
		title.setPosition({ w - panelWidth + 10.f, 2.f });
		window.draw(title);

		titleBar.setPosition({ w - panelWidth, halfH });
		window.draw(titleBar);
		title.setString("OUTPUT");
		title.setPosition({ w - panelWidth + 10.f, halfH + 2.f });
		window.draw(title);

		sf::View originalView = window.getView();

		sf::FloatRect inputViewport(
			{ (w - panelWidth) / w, 30.f / h },
			{ panelWidth / w, (halfH - 30.f) / h }
		);

		sf::View inputView(sf::FloatRect(
			{ 0.f, 0.f },
			{ panelWidth, halfH - 30.f }
		));

		inputView.setViewport(inputViewport);
		window.setView(inputView);

		sf::Text content(font);
		content.setCharacterSize(18);
		content.setFillColor(sf::Color(200, 200, 200));

		float currentY = 10.f + inputScroll;

		for (const auto& line : inputHistory) {
			content.setString(line);
			content.setPosition({ 10.f, currentY });
			window.draw(content);
			currentY += lineHeight;
		}

		if (waitingForInput) {
			content.setString(currentPrompt + currentInput + "_");
			content.setFillColor(sf::Color::Yellow);
			content.setPosition({ 10.f, currentY });
			window.draw(content);
		}

		sf::FloatRect outputViewport(
			{ (w - panelWidth) / w, (halfH + 30.f) / h },
			{ panelWidth / w, (halfH - 30.f) / h }
		);

		sf::View outputView(sf::FloatRect(
			{ 0.f, 0.f },
			{ panelWidth, halfH - 30.f }
		));

		outputView.setViewport(outputViewport);
		window.setView(outputView);

		currentY = 10.f + outputScroll;
		content.setFillColor(sf::Color(150, 255, 150));

		for (const auto& line : outputHistory) {
			content.setString(line);
			content.setPosition({ 10.f, currentY });
			window.draw(content);
			currentY += lineHeight;
		}

		window.setView(originalView);
	}
};

RuntimeConsole globalConsole;

struct variable {
	std::string variableName;
	int value;
};

std::vector <std::string> codeLines;
std::vector <sf::Text> codeLinesToOutput;
std::stack <Block*> decisionBlockStack;

std::vector<variable> variables;
std::vector<std::string> declaredVariables;
std::stack <std::string> exprStack;
std::stack <int> evalStack;
std::queue <std::string> exprPostfix;

sf::Text* error;

bool correctVariableName(std::string name)
{
	if (!isalpha(name[0]) && name[0] != '_') return 0;
	for (int i = 1; i < name.size(); i++)
	{
		if (!isalnum(name[i]) && name[i] != '_') return 0;
	}
	return 1;
}

bool operand(std::string c) {
	if (c.size() != 1) return 1;
	if (c != "+" && c != "-" && c != "*" && c != "/" && c != "%" && c != "(" && c != ")") return 1;
	return 0;
}

int priority(std::string c) {
	if (c == "+" || c == "-") return 1;
	if (c == "*" || c == "/" || c == "%") return 2;
	return 0;
}

int add(int stg, int drp) { return stg + drp; }
int sub(int stg, int drp) { return stg - drp; }
int mul(int stg, int drp) { return stg * drp; }
int divi(int stg, int drp) { return (drp != 0) ? stg / drp : 0; }
int mod(int stg, int drp) { return stg % drp; };

bool lessThan(int stg, int drp) { return (stg < drp) ? 1 : 0; }
bool lessThanEq(int stg, int drp) { return (stg <= drp) ? 1 : 0; }
bool greaterThan(int stg, int drp) { return (stg > drp) ? 1 : 0; }
bool greaterThanEq(int stg, int drp) { return (stg >= drp) ? 1 : 0; }
bool equal(int stg, int drp) { return (stg == drp) ? 1 : 0; }
bool notEqual(int stg, int drp) { return (stg != drp) ? 1 : 0; }

int& getVariableReference(const std::string& name) {
	for (auto& var : variables) {
		if (var.variableName == name) return var.value;
	}
	variables.push_back({ name, 0 });
	return variables.back().value;
}

int evalPostfix(std::queue<std::string>& exprPostfix) {
	while (!evalStack.empty()) evalStack.pop();

	int val = 0, stg = 0, drp = 0;
	std::string c;

	while (!exprPostfix.empty()) {
		c = exprPostfix.front();
		exprPostfix.pop();

		if (operand(c)) {
			if (isdigit(c[0])) {
				evalStack.push(std::stoi(c));
			}
			else {
				bool found = false;
				for (auto& var : variables) {
					if (var.variableName == c) {
						evalStack.push(var.value);
						found = true;
						break;
					}
				}
				if (!found) evalStack.push(0);
			}
		}
		else {
			if (evalStack.empty()) return 0;
			drp = evalStack.top(); evalStack.pop();

			if (evalStack.empty()) return drp;
			stg = evalStack.top(); evalStack.pop();

			if (c == "+") val = add(stg, drp);
			else if (c == "-") val = sub(stg, drp);
			else if (c == "*") val = mul(stg, drp);
			else if (c == "/") val = divi(stg, drp);
			else if (c == "%") val = mod(stg, drp);

			evalStack.push(val);
		}
	}

	if (evalStack.empty()) return 0;
	val = evalStack.top();
	evalStack.pop();
	return val;
}

bool foundVariableName(std::string stdStr)
{
	for (auto& var : variables)
	{
		if (var.variableName == stdStr)
		{
			return 1;
		}
	}
	return 0;
}

int convertToPostfix(Block& blk, std::string expr, bool& errorFlag) {

	while (!exprStack.empty()) exprStack.pop();
	while (!exprPostfix.empty()) exprPostfix.pop();

	std::string c;
	std::string last;
	last.clear();

	for (size_t i = 0; i < expr.size();) {
		if (expr[i] == ' ') {
			i++;
		}
		else {
			if (isalnum(expr[i]))
			{
				c = expr[i++];
				while (i < expr.size() && isalnum(expr[i])) {
					c += expr[i++];
				}
			}
			else {
				c = expr[i++];
			}

			if (operand(c))
			{
				if (isdigit(c[0]) || foundVariableName(c))
				{
					exprPostfix.push(c);
					last = c;
				}
				else
				{
					error = new sf::Text(globalConsole.font, "Eroare: '" + c + "' nu este definit\nApasa o tasta pentru a continua", fontSize);
					error->setFillColor(errorColor);
					sf::FloatRect blkBounds = blk.getBounds();
					float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
					sf::FloatRect errorTextBounds = error->getLocalBounds();
					float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
					error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
					errorFlag = 1;
					return 0;
				}
			}
			else {
				if (c == "-" && (last.empty() || last == "(" || last == "+" || last == "-" || last == "*" || last == "/" || last == "%"))
				{
					std::string next;
					next.clear();
					while (i < expr.size() && expr[i] == ' ')
					{
						i++;
					}
					if (i < expr.size())
					{
						if (expr[i] == '(')
						{
							exprPostfix.push("0");
							exprStack.push("-");
							last = "-";
							continue;
						}
						else if (isalnum(expr[i]))
						{
							while (i < expr.size() && isalnum(expr[i]))
							{
								next += expr[i++];
							}
							exprPostfix.push("0");
							if (isdigit(next[0]) || foundVariableName(next))
							{
								exprPostfix.push(next);
								exprPostfix.push("-");
								last = next;
							}
							else
							{
								error = new sf::Text(globalConsole.font, "Eroare: '" + next + "' nu este definit\nApasa o tasta pentru a continua", fontSize);
								error->setFillColor(errorColor);
								sf::FloatRect blkBounds = blk.getBounds();
								float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
								sf::FloatRect errorTextBounds = error->getLocalBounds();
								float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
								error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
								errorFlag = 1;
								return 0;
							}
							continue;
						}
					}
				}
				if (c == "(") {
					exprStack.push(c);
				}
				else if (c == ")") {
					while (!exprStack.empty() && exprStack.top() != "(") {
						exprPostfix.push(exprStack.top());
						exprStack.pop();
					}
					if (!exprStack.empty()) exprStack.pop();
				}
				else {
					while (!exprStack.empty() && exprStack.top() != "(" && priority(exprStack.top()) >= priority(c)) {
						exprPostfix.push(exprStack.top());
						exprStack.pop();
					}
					exprStack.push(c);
				}
			}
		}
	}
	while (!exprStack.empty()) {
		exprPostfix.push(exprStack.top());
		exprStack.pop();
	}
	return evalPostfix(exprPostfix);
}


int readInput(sf::RenderWindow& window, RuntimeConsole& console, std::string& variableName) {
	console.waitingForInput = true;
	console.currentPrompt = variableName + " = ";
	console.currentInput = "";

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
				exit(0);
			}

			console.handleEvent(*event, window);

			if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
				if (textEvent->unicode == 13) {
					if (!console.currentInput.empty()) {
						try {
							int val = std::stoi(console.currentInput);
							console.addInputLog(console.currentPrompt + console.currentInput);
							console.waitingForInput = false;
							return val;
						}
						catch (...) { console.currentInput = ""; }
					}
				}
				else if (textEvent->unicode == 8) {
					if (!console.currentInput.empty()) console.currentInput.pop_back();
				}
				else if (textEvent->unicode >= 48 && textEvent->unicode <= 57 || textEvent->unicode == '-') {
					console.currentInput += static_cast<char>(textEvent->unicode);
				}
			}
		}

		window.clear(sf::Color(25, 25, 25));
		for (auto& blk : blocks) blk->draw(window);
		for (auto& arr : arrows) arr->draw(window);
		console.draw(window);
		window.display();
	}
	return 0;
}


bool evaluateAction(Block& blk, RuntimeConsole& console) {
	sf::String sfStr = blk.text->getString();
	std::string stdStr = sfStr.toAnsiString();

	std::string varToken;
	std::string textToEval;
	bool foundEqualSign = false;
	size_t i = 0;


	for (i = 0; i < stdStr.size() && !foundEqualSign;) {
		if (stdStr[i] == ' ') {
			i++;
		}
		else {
			if (isalnum(stdStr[i])) {
				while (i < stdStr.size() && isalnum(stdStr[i])) {
					varToken += stdStr[i++];
				}
			}
			else if (stdStr[i] == '=') {
				foundEqualSign = true;
				i++;
			}
			else {
				i++;
			}
		}
	}

	if (!foundEqualSign || varToken.empty() || !correctVariableName(varToken)) {
		error = new sf::Text(console.font, "Format gresit\nApasa o tasta pentru a continua", fontSize);
		error->setFillColor(errorColor);
		sf::FloatRect blkBounds = blk.getBounds();
		float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
		sf::FloatRect errorTextBounds = error->getLocalBounds();
		float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
		error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
		return false;
	}


	for (size_t j = i; j < stdStr.size(); j++) {
		textToEval += stdStr[j];
	}

	bool errorFlag = 0;

	int result = convertToPostfix(blk, textToEval, errorFlag);
	if (errorFlag == 1)
	{
		return false;
	}
	int& varRef = getVariableReference(varToken);
	varRef = result;


	return true;


}

bool needIntAction(Block& blk)
{
	sf::String sfStr = blk.text->getString();
	std::string stdStr = sfStr.toAnsiString();

	std::string varToken;

	bool foundEqualSign = false;
	for (int i = 0; i < stdStr.size() && !foundEqualSign;) {
		if (stdStr[i] == ' ') {
			i++;
		}
		else {
			if (isalnum(stdStr[i])) {
				while (i < stdStr.size() && isalnum(stdStr[i])) {
					varToken += stdStr[i++];
				}
			}
			else if (stdStr[i] == '=') {
				foundEqualSign = true;
				i++;
			}
			else {
				i++;
			}
		}
	}
	for (auto& var : variables)
	{
		if (var.variableName == varToken)
		{
			return 0;
		}
	}
	return 1;
}

int evaluateDecision(Block& blk, RuntimeConsole& console) {
	sf::String sfStr = blk.text->getString();
	std::string stdStr = sfStr.toAnsiString();
	std::string untilOperator;
	std::string afterOperator;
	std::string op;

	untilOperator.clear();
	afterOperator.clear();

	int i = 0;
	int stg = 0, drp = 0;

	bool errorFlag = 0;

	while (i < stdStr.size() && stdStr[i] != '<' && stdStr[i] != '>' && stdStr[i] != '=' && stdStr[i] != '!')
	{
		if (stdStr[i] != ' ')
		{
			untilOperator += stdStr[i];
		}
		i++;
	}

	if (i == stdStr.size())
	{
		error = new sf::Text(console.font, "Operator invalid sau lipsa\nOperatori permisi: <, <=, >, >=, ==, !=\nApasa o tasta pentru a continua", fontSize);
		error->setFillColor(errorColor);
		sf::FloatRect blkBounds = blk.getBounds();
		float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
		sf::FloatRect errorTextBounds = error->getLocalBounds();
		float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
		error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
		return -1;
	}

	if (stdStr[i] == '<')
	{
		if (i + 1 < stdStr.size() && stdStr[i + 1] == '=')
		{
			//less than or equal to path
			i = i + 2;
			while (i < stdStr.size())
			{
				afterOperator += stdStr[i++];
			}
			stg = convertToPostfix(blk, untilOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			drp = convertToPostfix(blk, afterOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			return lessThanEq(stg, drp);
		}
		else
		{
			//less than path
			i = i + 1;
			while (i < stdStr.size())
			{
				afterOperator += stdStr[i++];
			}
			stg = convertToPostfix(blk, untilOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			drp = convertToPostfix(blk, afterOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			return lessThan(stg, drp);
		}
	}
	else if (stdStr[i] == '>')
	{
		if (i + 1 < stdStr.size() && stdStr[i + 1] == '=')
		{
			//greater than or equal to path
			i = i + 2;
			while (i < stdStr.size())
			{
				afterOperator += stdStr[i++];
			}
			stg = convertToPostfix(blk, untilOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			drp = convertToPostfix(blk, afterOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			return greaterThanEq(stg, drp);
		}
		else
		{
			//greater than path
			i = i + 1;
			while (i < stdStr.size())
			{
				afterOperator += stdStr[i++];
			}
			stg = convertToPostfix(blk, untilOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			drp = convertToPostfix(blk, afterOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			return greaterThan(stg, drp);
		}
	}
	else if (stdStr[i] == '=')
	{
		if (i + 1 < stdStr.size() && stdStr[i + 1] == '=')
		{
			//equal path
			i = i + 2;
			while (i < stdStr.size())
			{
				afterOperator += stdStr[i++];
			}
			stg = convertToPostfix(blk, untilOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			drp = convertToPostfix(blk, afterOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			return equal(stg, drp);
		}
		else
		{
			error = new sf::Text(console.font, "Operator invalid\nApasa o tasta pentru a continua", fontSize);
			error->setFillColor(errorColor);
			sf::FloatRect blkBounds = blk.getBounds();
			float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
			sf::FloatRect errorTextBounds = error->getLocalBounds();
			float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
			error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
			return -1;
		}
	}
	else if (stdStr[i] == '!')
	{
		if (stdStr[i + 1] == '=')
		{
			// not equal path
			i = i + 2;
			while (i < stdStr.size())
			{
				afterOperator += stdStr[i++];
			}
			stg = convertToPostfix(blk, untilOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			drp = convertToPostfix(blk, afterOperator, errorFlag);
			if (errorFlag == 1)
			{
				return -1;
			}
			return notEqual(stg, drp);
		}
		else
		{
			error = new sf::Text(console.font, "Operator invalid\nApasa o tasta pentru a continua", fontSize);
			error->setFillColor(errorColor);
			sf::FloatRect blkBounds = blk.getBounds();
			float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
			sf::FloatRect errorTextBounds = error->getLocalBounds();
			float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
			error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
			return -1;
		}
	}
	error = new sf::Text(console.font, "Operator invalid sau lipsa\nOperatori permisi: <, <=, >, >=, ==, !=\nApasa o tasta pentru a continua", fontSize);
	error->setFillColor(errorColor);
	sf::FloatRect blkBounds = blk.getBounds();
	float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
	sf::FloatRect errorTextBounds = error->getLocalBounds();
	float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
	error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
	return -1;
}

bool evaluateInput(sf::RenderWindow& window, RuntimeConsole& console, Block& blk) {
	sf::String sfStr = blk.text->getString();
	std::string varName = sfStr.toAnsiString();
	varName.erase(remove(varName.begin(), varName.end(), ' '), varName.end());

	if (varName.empty() || !correctVariableName(varName))
	{
		error = new sf::Text(console.font, "Format gresit\nApasa o tasta pentru a continua", fontSize);
		error->setFillColor(errorColor);
		sf::FloatRect blkBounds = blk.getBounds();
		float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
		sf::FloatRect errorTextBounds = error->getLocalBounds();
		float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
		error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
		return false;
	}


	int val = readInput(window, console, varName);
	int& varRef = getVariableReference(varName);
	varRef = val;
	return true;
}

bool needIntInput(Block& blk)
{
	sf::String sfStr = blk.text->getString();
	std::string stdStr = sfStr.toAnsiString();

	std::string varToken;

	for (int i = 0; i < stdStr.size();)
	{
		if (stdStr[i] == ' ')
		{
			i++;
		}
		else
		{
			varToken += stdStr[i++];
		}
	}

	for (auto& var : variables)
	{
		if (var.variableName == varToken)
		{
			return 0;
		}
	}
	return 1;
}

bool evaluateOutput(sf::RenderWindow& window, RuntimeConsole& console, Block& blk) {
	sf::String sfStr = blk.text->getString();
	std::string varName = sfStr.toAnsiString();

	int i = 0;
	while (varName[i] == ' ')
	{
		i++;
	}
	if (varName[i] == '"')
	{
		std::string stringToOutput;
		stringToOutput.clear();
		i++;
		while (varName[i] != '"' && i < varName.size())
		{
			stringToOutput += varName[i];
			i++;
		}
		if (i == varName.size())
		{
			error = new sf::Text(console.font, "Format gresit\nApasa o tasta pentru a continua", fontSize);
			error->setFillColor(errorColor);
			sf::FloatRect blkBounds = blk.getBounds();
			float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
			sf::FloatRect errorTextBounds = error->getLocalBounds();
			float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
			error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
			return false;
		}
		console.addOutput(stringToOutput);
	}
	else
	{
		varName.erase(remove(varName.begin(), varName.end(), ' '), varName.end());

		bool found = false;
		for (auto& var : variables) {
			if (var.variableName == varName) {
				console.addOutput(varName + " : " + std::to_string(var.value));
				found = true;
				break;
			}
		}

		if (!found) {
			error = new sf::Text(console.font, "Eroare: '" + varName + "' nu este definit\nApasa o tasta pentru a continua", fontSize);
			error->setFillColor(errorColor);
			sf::FloatRect blkBounds = blk.getBounds();
			float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
			sf::FloatRect errorTextBounds = error->getLocalBounds();
			float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
			error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
			return false;
		}
	}
	window.clear(sf::Color(25, 25, 25));
	for (auto& b : blocks) b->draw(window);
	for (auto& a : arrows) a->draw(window);
	console.draw(window);
	window.display();
	sf::sleep(sf::milliseconds(100));
	return true;
}

void goThroughBlocks(sf::RenderWindow& window, Block* blk, RuntimeConsole& console)
{
	if (!blk) return;

	if (blk->connectedToTruth == nullptr && blk->blockType != stopBlockType) {
		error = new sf::Text(console.font, "Ramura trebuie sa se termine cu un bloc Stop\nApasa o tasta pentru a continua", fontSize);
		error->setFillColor(errorColor);
		sf::FloatRect blkBounds = blk->getBounds();
		float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
		sf::FloatRect errorTextBounds = error->getLocalBounds();
		float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
		error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
		return;
	}

	sf::String sfStr = blk->text->getString();
	std::string stdStr = sfStr.toAnsiString();
	bool isWhile = false;
	bool foundEqual = false;
	bool needInt = true;
	std::string varToken;
	std::string extractedVar;
	varToken.clear();

	size_t i;

	if (!decisionBlockStack.empty() && blk == decisionBlockStack.top())
	{
		return;
	}
	switch (blk->blockType)
	{
	case startBlockType:
		goThroughBlocks(window, blk->connectedToTruth, console);
		break;
	case actionBlockType:
		for (i = 0; i < stdStr.size() && !foundEqual; i++) {
			if (stdStr[i] == ' ')
			{
				i++;
			}
			if (stdStr[i] == '=') {
				foundEqual = true;
				break;
			}
			if (isalnum(stdStr[i]) || stdStr[i] == '_')
			{
				extractedVar += stdStr[i];
			}

			for (const auto& declaredVar : declaredVariables) {
				if (declaredVar == extractedVar) {
					needInt = false;
					break;
				}
			}
			if (needInt)
			{
				declaredVariables.push_back(extractedVar);
				codeLines.push_back("int " + stdStr + ";");
			}
			else
			{
				codeLines.push_back(stdStr + ";");
			}
			goThroughBlocks(window, blk->connectedToTruth, console);
			break;
	case decisionBlockType:
		if (blk->connectedToTruth == nullptr || blk->connectedToFalse == nullptr)
		{
			error = new sf::Text(console.font, "Ramura trebuie sa se termine cu un bloc Stop\nApasa o tasta pentru a continua", fontSize);
			error->setFillColor(errorColor);
			sf::FloatRect blkBounds = blk->getBounds();
			float blkCenterY = blkBounds.position.y + blkBounds.size.y / 2.f;
			sf::FloatRect errorTextBounds = error->getLocalBounds();
			float errorOffsetY = errorTextBounds.position.y + errorTextBounds.size.y / 2.f;
			error->setPosition({ blkBounds.position.x + blkBounds.size.x + errorOffset, blkCenterY - errorOffsetY });
			return;
		}
		for (i = 0; i < blk->anchorPoints.size(); i++)
		{
			if (blk->anchorPoints[i].positionType == anchorLeftType && blk->anchorPoints[i].isSelected)
			{
				isWhile = true;
				break;
			}
		}
		if (isWhile)
		{
			decisionBlockStack.push(blk);
			codeLines.push_back("while( " + stdStr + " ) {");
			goThroughBlocks(window, blk->connectedToTruth, console);
			decisionBlockStack.pop();
			codeLines.push_back("}");
			goThroughBlocks(window, blk->connectedToFalse, console);
		}
		else
		{
			codeLines.push_back("if( " + stdStr + " ) {");
			goThroughBlocks(window, blk->connectedToTruth, console);
			codeLines.push_back("}");
			codeLines.push_back("else {");
			goThroughBlocks(window, blk->connectedToFalse, console);
			codeLines.push_back("}");
		}
		break;
	case inputBlockType:
		for (i = 0; i < stdStr.size(); i++)
		{
			if (stdStr[i] != ' ') {
				extractedVar += stdStr[i];
			}
		}
		for (const auto& declaredVar : declaredVariables) {
			if (declaredVar == extractedVar) {
				needInt = false;
				break;
			}
		}
		if (needInt && !extractedVar.empty()) {
			declaredVariables.push_back(extractedVar);
			codeLines.push_back("int " + stdStr + ";");
		}
		codeLines.push_back("cin >> " + stdStr + ";");
		goThroughBlocks(window, blk->connectedToTruth, console);
		break;
	case outputBlockType:
		codeLines.push_back("cout << " + stdStr + ";");
		if (blk->connectedToTruth->blockType == stopBlockType) {
			codeLines.push_back("return 0;");
		}
		goThroughBlocks(window, blk->connectedToTruth, console);
		break;
	case stopBlockType:
		return;
	default:
		return;
		}
	}
}

void outputCode(sf::RenderWindow& window, RuntimeConsole& console, Block& blk)
{
	int spaces = 0;
	float codeTextPositionX = 1120.f;
	float codeTextPositionY = 50.f;

	codeLines.clear();
	codeLinesToOutput.clear();
	declaredVariables.clear();

	std::string firstLine, secondLine, thirdLine;
	sf::Text* text;

	sf::String sfStr = blk.text->getString();
	std::string stdStr = sfStr.toAnsiString();

	firstLine = "#include <iostream>";
	secondLine = "using namespace std;";
	thirdLine = "int main(){";
	codeLines.push_back(firstLine);
	codeLines.push_back(secondLine);
	codeLines.push_back(thirdLine);

	Block* crt = &blk;

	goThroughBlocks(window, &blk, console);

	if (error != nullptr)
	{
		return;
	}

	codeLines.push_back("return 0;");
	codeLines.push_back("}");

	for (size_t i = 0; i < codeLines.size(); i++)
	{
		sf::Text text(globalConsole.font);
		text.setString(codeLines[i]);
		text.setCharacterSize(fontSize);
		text.setFillColor(textColor);
		if (codeLines[i] == "}")
		{
			spaces--;
		}
		text.setPosition(sf::Vector2f(codeTextPositionX + spaces * spaceSize, codeTextPositionY + i * fontSize + i * codeSpacing));
		if (codeLines[i][codeLines[i].size() - 1] == '{')
		{
			spaces++;
		}
		codeLinesToOutput.push_back(text);
	}

}

bool runProgram(sf::RenderWindow& window, Block& blk)
{
	variables.clear();
	codeLines.clear();
	RuntimeConsole console;

	Block* crt = &blk;

	outputCode(window, console, blk);

	if (error != nullptr) {
		while (window.isOpen())
		{
			while (auto event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>())
					window.close();
				if (event->is<sf::Event::KeyPressed>()) {
					delete error;
					error = nullptr;
					return 0;
				}
			}

			window.clear(sf::Color(25, 25, 25));
			for (auto& b : blocks) b->draw(window);
			for (auto& a : arrows) a->draw(window);

			window.draw(*error);

			window.display();
		}
		return 0;
	}

	while (crt != nullptr && crt->blockType != stopBlockType)
	{

		while (auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) window.close();
			console.handleEvent(*event, window);
		}

		switch (crt->blockType)
		{
		case startBlockType:
			crt = crt->connectedToTruth;
			break;
		case actionBlockType:
			if (evaluateAction(*crt, console)) crt = crt->connectedToTruth;
			else
			{
				while (window.isOpen())
				{
					while (auto event = window.pollEvent())
					{
						if (event->is<sf::Event::Closed>())
							window.close();
						if (event->is<sf::Event::KeyPressed>()) {
							delete error;
							error = nullptr;
							return 0;
						}
					}

					window.clear(sf::Color(25, 25, 25));
					crt->draw(window);

					window.draw(*error);

					window.display();
				}
			}
			break;
		case decisionBlockType:
		{
			int res = evaluateDecision(*crt, console);
			if (res == -1)
			{
				while (window.isOpen())
				{
					while (auto event = window.pollEvent())
					{
						if (event->is<sf::Event::Closed>())
							window.close();
						if (event->is<sf::Event::KeyPressed>()) {
							delete error;
							error = nullptr;
							return 0;
						}
					}

					window.clear(sf::Color(25, 25, 25));
					crt->draw(window);

					window.draw(*error);

					window.display();
				}
				return 0;
			}
			if (res == 1) crt = crt->connectedToTruth;
			else crt = crt->connectedToFalse;
			break;
		}
		case inputBlockType:
			if (evaluateInput(window, console, *crt)) crt = crt->connectedToTruth;
			else {
				while (window.isOpen())
				{
					while (auto event = window.pollEvent())
					{
						if (event->is<sf::Event::Closed>())
							window.close();
						if (event->is<sf::Event::KeyPressed>()) {
							delete error;
							error = nullptr;
							return 0;
						}
					}
					window.clear(sf::Color(25, 25, 25));
					crt->draw(window);

					window.draw(*error);

					window.display();
				}
			}
			break;
		case outputBlockType:
			if (evaluateOutput(window, console, *crt)) crt = crt->connectedToTruth;
			else
			{
				while (window.isOpen())
				{
					while (auto event = window.pollEvent())
					{
						if (event->is<sf::Event::Closed>())
							window.close();
						if (event->is<sf::Event::KeyPressed>()) {
							delete error;
							error = nullptr;
							return 0;
						}
					}
					window.clear(sf::Color(25, 25, 25));
					crt->draw(window);

					window.draw(*error);

					window.display();
				}
			}
			break;
		default:
			return 0;
		}
	}

	sf::sleep(sf::milliseconds(350));
	console.addOutput("Apasa pentru a iesi");

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) {
				window.close();
				return 0;
			}

			console.handleEvent(*event, window);

			if (event->is<sf::Event::KeyPressed>() || event->is<sf::Event::MouseButtonPressed>()) {
				return 1;
			}
		}

		window.clear(sf::Color(25, 25, 25));
		for (auto& b : blocks) b->draw(window);
		for (auto& a : arrows) a->draw(window);

		console.draw(window);
		window.display();
	}
	return 1;
}