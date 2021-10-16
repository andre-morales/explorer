#include "Explorer/MainMenuUI.h"
#include "Explorer/Explorer.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Window.h"
#include "Render/Renderer.h"
#include "GUI/Sprite.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/GridPane.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/TextField.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/Layouts/SplitLayout.h"

MainMenuUI::MainMenuUI(Explorer& e) : explorer(e){}
MainMenuUI::~MainMenuUI(){}

void MainMenuUI::create(){
	setLayout(new StackLayout());
	this->name = "main_menu";
	auto& tex = explorer.renderer->textures;
    auto& spr = explorer.ui->sprites;

	setBackground(tex["title_bg"]);
	bgSprite->scalingMode = ScalingMode::STRETCH;
	auto root = add(Sh<Component>(new SplitLayout(SplitLayout::HORIZONTAL, 0.35)));
	root->setBackground({0, 0, 0, 0});
	root->insets = {8, 8, 8, 8};

	auto menu = mkShared<GridPane>();
	root->add(menu);
	menu->direction = GridPane::LR_BT;
	menu->setGrid(1, 8);
	menu->setSpacing(8, 4);
	menu->insets = {8, 8, 8, 8};
	menu->setBackground({1, 1, 1, 0.5f});
	menu->setBackground(spr["panel2"]);

	auto exitBtn = explorer.ui->clButton("Exit");
	exitBtn->addActionListener([this](auto&){
		explorer.renderer->window->setCloseFlag(true);
	});
	menu->add(exitBtn);

	auto nameField = mkShared<TextField>("Player");
	auto field = mkShared<TextField>("127.0.0.1");

	auto playBtn = explorer.ui->clButton("Join game...");
	playBtn->addActionListener([this, field, nameField](auto&){
		explorer.joinGame(field->text, 3333, nameField->text);
	});
	menu->add(playBtn);

	field->setBackground(spr["panel2"]);
	field->setFont(explorer.renderer->font);
	menu->add(field);

	nameField->setBackground(spr["panel2"]);
	nameField->setFont(explorer.renderer->font);
	menu->add(nameField);
}
