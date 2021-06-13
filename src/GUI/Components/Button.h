#pragma once
#include "GUI/Component.h"
#include "GUI/Event/ActionEvent.h"
#include <string>
#include <functional>

class Button : public Component {
public:
	using ActionListener = std::function<void(const ActionEvent&)>;
	std::vector<ActionListener> actionListeners;

	Button();
	Button(const std::string&);

	void addActionListener(ActionListener);
	void fireActionListeners(const ActionEvent&);

	virtual void paintComponent(GLContext&, vec2f) override;
private:
	bool mouseHeldOver;
};
