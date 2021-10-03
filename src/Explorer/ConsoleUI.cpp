#include "Explorer/ConsoleUI.h"
#include "Explorer/Explorer.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Window.h"
#include "Render/Renderer.h"
#include "GUI/Sprite.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/SplitPane.h"
#include "GUI/Components/GridPane.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/TextField.h"
#include "GUI/Components/ScrollView.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/Keys.h"
#include "GUI/Event/KeyEvent.h"
#include "Game/Event.h"

ConsoleUI::ConsoleUI(Explorer& e) : explorer(e){
	name = "console";
	setVisible(false);
	setBackground(false);
	setLayout(Sh<StackLayout>());
}
ConsoleUI::~ConsoleUI(){}

void ConsoleUI::create(){
	subRoot = add(Sh<Component>(Sh<StackLayout>()));
	subRoot->setBackground({0.2f, 0.5f, 0.7f, 0.1f});
	consoleScroll = mkShared<ScrollView>();
	consoleScroll->setBackground({0, 0, 0, 0.4f});
	consoleLabel = mkShared<Label>();
	consoleLabel->setFont(explorer.renderer->font);
	consoleLabel->setBackground(false);
	consoleLabel->textOrientation = Orientation::LEFT_TOP;
	consoleLabel->textSize = 2;
	consoleScroll->add(consoleLabel);
	subRoot->add(consoleScroll);

	inputField = mkShared<TextField>();
	inputField->setBackground({0, 0, 0, 0.4f});
	inputField->setFont(explorer.renderer->font);
	inputField->textOrientation = Orientation::LEFT_TOP;
	inputField->addKeyListener([this](const auto& ev){
		if(ev.key == IKEY_ENTER && ev.action){
			explorer.localCommand(inputField->text);
			inputField->text = "";
		}
	});

	subRoot->add(inputField);
}

void ConsoleUI::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
	consoleScroll->setBounds(x + 8, y + 8, w - 16, h - 64);
	inputField->setBounds(x + 8, h - 40, w - 16, 32);
}

void ConsoleUI::log(const std::string& text){
	consoleLabel->text += text;

	auto bh = consoleLabel->getBodyHeight(consoleLabel->text);
	auto lh = consoleLabel->height;
	if(bh > lh){
		consoleLabel->y = lh - bh;
	}
}
