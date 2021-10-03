#pragma once
#include <vector>
#include <functional>
#include "ilib/mem.h"
#include "ilib/types.h"
#include "ilib/obs_unordered_map.h"

class Label;
class GUI;
class Component;
class Button;
class TextField;
class Explorer;
class Sprite;
class ActionEvent;

class ExplorerGUI {
public:
    Explorer& explorer;
	std::vector<std::string> guiNames;
	obs_unordered_map<std::string, Shared<GUI>> guis;
	obs_unordered_map<std::string, Shared<Sprite>> sprites;

	ExplorerGUI(Explorer&);
	~ExplorerGUI();

	void render() const;
	void addGUI(Shared<GUI>);
	void addGUI(Shared<GUI>, const std::string&);
	void addGUI(Shared<GUI>, const std::string&, uint8);
	Shared<GUI> removeGUI(const std::string&);
	void removeGUIs();
	Shared<GUI> getGUI(const std::string&);

	void addPanel(Shared<Component>, int = -1);
	Shared<Component> removePanel(const std::string&);
	Shared<Component> getPanel(const std::string&);

	sh<GUI> createGUI();
	sh<Button> clButton(const char*);
	sh<Button> clButton(const char*, std::function<void(const ActionEvent&)>);
	sh<Label> clLabel();
	sh<Label> clLabel(const char*);
	sh<TextField> clTextField(const char*);
	void loadSprites();
};