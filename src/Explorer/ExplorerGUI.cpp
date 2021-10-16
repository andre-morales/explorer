#include "ExplorerGUI.h"
#include "Explorer/Explorer.h"
#include "Explorer/Window.h"
#include "Render/Renderer.h"
#include "Render/Texture.h"
#include "Render/GLContext.h"
#include "GUI/GUI.h"
#include "GUI/Sprite.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/TextField.h"
#include "GUI/Components/Toggle.h"
#include "GUI/Components/ScrollBar.h"
#include "GUI/Event/MouseMotionEvent.h"
#include <string>
#include "ilib/Math/mat4.h"
#include "ilib/mem.h"
ExplorerGUI::ExplorerGUI(Explorer& e) : explorer(e){}
ExplorerGUI::~ExplorerGUI(){}

void ExplorerGUI::loadSprites(){
	auto& tex = explorer.renderer->textures;
	vec4f clborder = {.0625, .0625, .0625, .0625};
	sprites.emplace("button", Sh<Sprite>(tex["gui_button"], ScalingMode::SPRITE, clborder));
	sprites.emplace("panel",  Sh<Sprite>(tex["gui_panel"],  ScalingMode::SPRITE, clborder));
	sprites.emplace("panel2", Sh<Sprite>(tex["gui_panel2"], ScalingMode::SPRITE, clborder));
}

void ExplorerGUI::addGUI(Shared<GUI> gui){
	addGUI(gui, std::to_string((uintptr)gui.get()));
}

void ExplorerGUI::addGUI(Shared<GUI> gui, const std::string& name){
	guis.emplace(name, gui);
	guiNames.emplace_back(name);
}

void ExplorerGUI::addGUI(Shared<GUI> gui, const std::string& name, uint8 p){
	guis.emplace(name, gui);
	guiNames.insert(guiNames.begin() + p, name);
}

void ExplorerGUI::render() const {
	for(auto& name : guiNames){
		guis[name]->render();
	}
}

Shared<GUI> ExplorerGUI::removeGUI(const std::string& name){
	auto& v = guiNames;
	auto er = std::remove(v.begin(), v.end(), name);
	if(er != v.end()){
		v.erase(er, v.end());
		Shared<GUI> el = guis[name];
		guis.erase(name);
		return el;
	}
	return { nullptr };
}

void ExplorerGUI::removeGUIs(){
	guiNames.clear();
	guis.clear();
}

Shared<GUI> ExplorerGUI::getGUI(const std::string& name){
	return guis[name];
}

void ExplorerGUI::addPanel(Shared<Component> comp, int p){
	auto main = guis["main"];
	if(p < 0) {
		return main->add(comp);
	} else {
		return main->add(comp, p);
	}
}

Shared<Component> ExplorerGUI::removePanel(const std::string& name){
	return guis["main"]->remove(name);
}

Shared<Component> ExplorerGUI::getPanel(const std::string& name){
	return guis["main"]->root->getChild(name);
}

Shared<Button> ExplorerGUI::clButton(const char* text){
	auto btn = Sh<Button>(text);
	btn->setFont(explorer.renderer->font);
	btn->setBackground(sprites["button"]);
	btn->insets = {4, 4, 4, 4};
	btn->addMouseMotionListener([this](const MouseMotionEvent& mme){
		if(mme.action == 1){
			mme.source->setBackground({1, 1, 0.5, 1});
		} else if(mme.action == 2){
			mme.source->setBackground({1, 1, 1, 1});
		}
	});
	return btn;
}

sh<Button> ExplorerGUI::clButton(const char* text, std::function<void(const ActionEvent&)> listener){
	auto btn = clButton(text);
	btn->addActionListener((Button::ActionListener)listener);
	return btn;
}
sh<Label> ExplorerGUI::clLabel(){
	return clLabel("\0");
}

sh<Label> ExplorerGUI::clLabel(const char* text){
	auto field = Sh<Label>(text);
	field->setBackground(explorer.ui->sprites["panel2"]);
	field->setFont(explorer.renderer->font);
	return field;
}

sh<TextField> ExplorerGUI::clTextField(const char* text){
	auto field = Sh<TextField>(text);
	field->setBackground(explorer.ui->sprites["panel2"]);
	field->setFont(explorer.renderer->font);
	return field;
}

sh<Toggle> ExplorerGUI::clToggle(bool checked, std::function<void(Toggle*, bool)> vl){
	auto tog = Sh<Toggle>(checked);
	tog->addMouseMotionListener([this](auto& mme){
		if(mme.action == 1){
			mme.source->setBackground({1, 1, 0.5, 1});
		} else if(mme.action == 2){
			mme.source->setBackground({1, 1, 1, 1});
		}
	});
	tog->setFont(explorer.renderer->font);
	tog->setBackground(sprites["button"]);
	tog->insets = {4, 4, 4, 4};
	tog->addValueListener(vl);
	return tog;
}

sh<ScrollBar> ExplorerGUI::clScroll(){
	auto sc = Sh<ScrollBar>();
	sc->setButton(clButton(""));
	sc->setBackground(sprites["panel2"]);

	auto lbl = sc->add(clLabel(""));
	lbl->setBackground(false);
	lbl->enabled = false;
	return sc;
}

sh<ScrollBar> ExplorerGUI::clScroll(float v){
	auto sc = Sh<ScrollBar>(v);
	sc->setButton(clButton(""));
	sc->setBackground(sprites["panel2"]);

	auto lbl = sc->add(clLabel(""));
	lbl->setBackground(false);
	lbl->enabled = false;
	return sc;
}

sh<ScrollBar> ExplorerGUI::clScroll(float v, std::function<void(float)> vl){
	auto sc = clScroll(v);
	sc->addValueListener(vl);
	return sc;
}

sh<GUI> ExplorerGUI::createGUI(){
	return mkShared<GUI>(explorer.renderer->context);
}
