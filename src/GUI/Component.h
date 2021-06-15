#pragma once
#include "Render/Color.h"
#include "Event/KeyEvent.h"
#include "ilib/mem.h"
#include "ilib/Math/vec2.h"
#include "ilib/Math/vec4.h"
#include <vector>
#include <functional>
#include <string>
class GLWindow;
class GUI;
class Texture;
class Sprite;
class Font;
struct MouseButtonEvent;
struct MouseMotionEvent;
class Image;
class GLContext;
struct KeyEvent;
struct MouseButtonEvent;
struct MouseMotionEvent;

class Component {
public:
	Component* parent;
	std::vector<Shared<Component>> children;
	float x = 0, y = 0;
	float width = 0, height = 0;
	vec4f insets;
	bool visible, enabled;
	bool focused;

	std::string text;
	Color bgColor, fgColor;

	vec4f spriteBorders;
	bool background;

	Shared<Sprite> bgSprite;

	using KeyListener = std::function<void(const KeyEvent&)>;
	using MouseButtonListener = std::function<void(const MouseButtonEvent&)>;
	using MouseMotionListener = std::function<void(const MouseMotionEvent&)>;
	using SizeListener = std::function<void(float, float)>;

	std::vector<KeyListener*> keyListeners;
	std::vector<MouseButtonListener*> mouseButtonListeners, superMouseButtonListeners;
	std::vector<MouseMotionListener*> mouseMotionListeners, superMouseMotionListeners;
	std::vector<SizeListener*> sizeListeners;

	Component();
	~Component();

	virtual Shared<Component> add(Shared<Component>);
	virtual void paint(GLContext&, vec2f);
	virtual void paintComponent(GLContext&, vec2f);
	virtual void paintChildren(GLContext&, vec2f);

	void setBackground(bool);
	void setBackground(const Color&);
    void setBackground(Shared<Texture>);
	void setBackground(Shared<Sprite>);
	virtual void setBounds(float, float, float, float);
	vec4f getBounds();
	vec2f getPosition();

    // Add / Fire / Remove Listeners
	MouseButtonListener* addMouseButtonListener(MouseButtonListener, bool = false);
	void fireMouseButtonListeners(const MouseButtonEvent&, bool = false);
	void removeMouseButtonListener(MouseButtonListener*, bool);

	MouseMotionListener* addMouseMotionListener(MouseMotionListener, bool = false);
	void fireMouseMotionListeners(const MouseMotionEvent&, bool = false);
	void removeMouseMotionListener(MouseMotionListener*, bool = false);

	KeyListener* addKeyListener(KeyListener);
	void fireKeyListeners(const KeyEvent&);
	void removeKeyListener(KeyListener*);

	SizeListener* addSizeListener(SizeListener);
	void fireSizeListeners(float, float);
	void removeSizeListener(SizeListener*);
protected:
    bool focusable;
private:
    friend class GUI;
};
