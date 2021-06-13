#pragma once
#include <vector>
#include <functional>
#include <string>
#include "ilib/mem.h"
#include "Math/vec2.h"
#include "Math/vec4.h"
#include "Render/Color.h"
#include "Event/KeyEvent.h"
class GLWindow;
class GUI;
class Panel;
class Texture;
class Font;
struct MouseButtonEvent;
struct MouseMotionEvent;
class Image;
class GLContext;
struct KeyEvent;
struct MouseButtonEvent;
struct MouseMotionEvent;
enum class ScalingMode {
	STRETCH, SPRITE, MINIMUM,
	MAXIMUM_TOP_LEFT,    MAXIMUM_TOP_CENTER,    MAXIMUM_TOP_RIGHT,
	MAXIMUM_MIDDLE_LEFT, MAXIMUM_MIDDLE_CENTER, MAXIMUM_MIDDLE_RIGHT,
	MAXIMUM_BOTTOM_LEFT, MAXIMUM_BOTTOM_CENTER, MAXIMUM_BOTTOM_RIGHT,
};
class Component {
public:
	Panel* parent;
	std::vector<Shared<Component>> children;
	std::string text;

	float x = 0, y = 0;
	float width = 1, height = 1;
	vec4f insets;
	bool visible, enabled;
	bool focused, focusable;

	Shared<Font> font;
	Color bgColor, fgColor;

	vec4f spriteBorders;
	bool background;
	ScalingMode bgScaling;
	Shared<Texture> bgTexture;

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

	virtual void setFont(Shared<Font>);
	void setBackgroundColor(Color);
	void setBackgroundTexture(Shared<Texture>);
	virtual void setBounds(float, float, float, float);
	vec2f getPosition();

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
private:
	friend class GUI;
};
