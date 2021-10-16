#include "Explorer/StartUI.h"
#include "Explorer/Explorer.h"
#include "Explorer/ExplorerGUI.h"
#include "Render/Renderer.h"
#include "GUI/Sprite.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/GridPane.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/ScrollBar.h"
#include "GUI/Components/Toggle.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/Layouts/SplitLayout.h"

StartUI::StartUI(Explorer& e)
: Component(new StackLayout()), explorer(e){}
StartUI::~StartUI(){}

void StartUI::create(){
	auto& ui = *explorer.ui;
	auto& tex = explorer.renderer->textures;
	auto& spr = explorer.ui->sprites;

	setBackground(tex["title_bg"]);
	this->bgSprite->scalingMode = ScalingMode::STRETCH;

	auto root = addL<SplitLayout>(SplitLayout::HORIZONTAL, 0.35);
	root->setBackground(false);
	root->insets = {8, 8, 8, 8};

	auto menu = root->addNew<GridPane>();
	menu->direction = GridPane::LR_TB;
	menu->setGrid(1, 16);
	menu->setSpacing(8, 4);
	menu->insets = {8, 8, 8, 8};
	menu->setBackground({1, 1, 1, 0.5});
	menu->setBackground(spr["panel2"]);

	auto scroll = menu->addT(Sh<ScrollBar>());
	menu->setComponentSize(0, 1, 1);
	scroll->setButton(ui.clButton(""));
	scroll->setBackground(spr["panel2"]);
	scroll->increment = 1;
	scroll->max = 2;
	auto lbl = scroll->addT(ui.clLabel());
	glSelectText = lbl.get();
	lbl->setBackground(false);
	lbl->enabled = false;
	scroll->addValueListener([this, lbl](float v){
		setGL(v);
	});

	glTextProps = menu->addT(ui.clLabel()).get();
	glTextProps->textOrientation = Orientation::LEFT_TOP;
	glTextProps->textSize = 3;
	glTextProps->setBackground(false);
	menu->setComponentSize(1, 1, 12);

	menu->add(ui.clButton("Begin!", [this](auto&){
		explorer.loggingEnabled = toggleLogging->checked;
		explorer.openMainMenu(this->glversion);
	}));

	toggleLogging = ui.clToggle(explorer.loggingEnabled, [this](auto tog, bool checked){
		if(checked) {
			tog->text = "Logging: ON";
		} else {
			tog->text = "Logging: OFF";
		}
	});
	menu->add(toggleLogging);
	toggleLogging->fireValueListeners();

	menu->add(ui.clButton("Quit", [this](auto&){
		explorer.exit();
	}));
	setGL(0);
	scroll->setValue(1);
}

void StartUI::setGL(unsigned int v){
	glversion = v;
	std::string ver;
	std::string& prop = glTextProps->text;
	switch(v){
	default:
	case 0:
		ver = "1.1";
		prop = R"(
$c- VBOs
$c- Shadows
$c- Shaders
$c- VAOs
)";
		break;
	case 1:
		ver = "1.5";
		prop = R"(
$a+ VBOs
$c- Shadows
$c- Shaders
$c- VAOs
)";
		break;
	case 2:
		ver = "2.0";
		prop = R"(
$a+ VBOs
$e+ Shadows (slow)
$a+ Shaders
$c- VAOs
)";
		break;
	case 3:
		ver = "3.0";
		prop = R"(
$a+ VBOs
$a+ Shadows (fast)
$a+ Shaders
$a+ VAOs
)";
		break;
	}
	glSelectText->text = "OpenGL " + ver;
}