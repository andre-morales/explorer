#pragma once
#include "Render/Color.h"
#include "ilib/mem.h"
#include "ilib/Math/vec2.h"
#include "ilib/Math/vec4.h"
#include <vector>
#include <functional>
#include <string>
class GUI;
class GLWindow;
class GUI;
class Texture;
class Sprite;
class Font;
struct CharEvent;
struct KeyEvent;
struct MouseButtonEvent;
struct MouseMotionEvent;
class Image;
class GLContext;

class Component : public std::enable_shared_from_this<Component> {
public:
	GUI* ownerGUI = 0;
	Component* parent = 0;
	std::string name;
	std::vector<sh<Component>> children;
	un<class Layout> layout;
	float x = 0, y = 0;
	float width = 0, height = 0;
	vec4f insets;
	bool visible, enabled;
	bool focused = false;
	Color bgColor, fgColor;

	std::string text;
	bool background;
	sh<Sprite> bgSprite;

	using KeyListener = std::function<void(const KeyEvent&)>;
	using CharListener = std::function<void(const CharEvent&)>;
	using MouseButtonListener = std::function<void(const MouseButtonEvent&)>;
	using MouseMotionListener = std::function<void(const MouseMotionEvent&)>;
	using SizeListener = std::function<void(float, float)>;

	std::vector<KeyListener*> keyListeners, superKeyListeners;
	std::vector<CharListener*> charListeners;
	std::vector<MouseButtonListener*> mouseButtonListeners, superMouseButtonListeners;
	std::vector<MouseMotionListener*> mouseMotionListeners, superMouseMotionListeners;
	std::vector<SizeListener*> sizeListeners;

	// Constructor/Destructor
	Component();
	/*Constructs a component given a newly built layout. 
	  See Component::setLayout. */
	Component(Layout*&&); // Constructs
	virtual ~Component();

	// Hierarchy
	/* Adds a component and returns it right back with an optional position. */
	virtual sh<Component> add(sh<Component>, int = -1);

	/* Adds a component and returns it right back with an optional position.
	   This templated version of the function ensures the same return type as the argument. */
	template <class T> sh<T> addT(sh<T>&& c){
		add(c);
		return c;
	}
	/* Constructs a component of the specified type with the arguments emplaced. */
	template <class T, class... Args> sh<T> addNew(Args&&... args){
		auto ptr = std::make_shared<T>(args...);
		add(ptr);
		return ptr;
	}

	/* Constructs a regular base component with the specified layout
	   as a template and its layout arguments emplaced. Returns the
	   constructed element. */
	template <class LayoutType, class... LayoutArgs>
	sh<Component> addL(LayoutArgs&&... args){
		return add(std::make_shared<Component>(new LayoutType(args...)));
	}
	virtual Shared<Component> remove(Shared<Component>);
	virtual Shared<Component> remove(const std::string&);
	Shared<Component> getChild(const std::string&);
	/* Changes the layout used by this component to organize its children.
	   The component assumes ownership of layout given to it so be careful. */
	void setLayout(Layout*&&);
	void relayout();

	// Drawing
	virtual void paint(GLContext&, vec2f);
	virtual void paintComponent(GLContext&, vec2f);
	virtual void paintChildren(GLContext&, vec2f);

	// Propery Set/Get
	void setVisible(bool);
	bool isVisible();
	void setBackground(bool);
	void setBackground(const Color&);
    void setBackground(Shared<Texture>);
	void setBackground(Shared<Sprite>);
	virtual void setBounds(float, float, float, float);
	vec4f getBounds();
	vec2f getPosition();

	void requestFocus();
	void releaseFocus();

    // Add / Fire / Remove Listeners
	MouseButtonListener* addMouseButtonListener(MouseButtonListener, bool = false);
	void fireMouseButtonListeners(const MouseButtonEvent&, bool = false);
	void removeMouseButtonListener(MouseButtonListener*, bool);

	MouseMotionListener* addMouseMotionListener(MouseMotionListener, bool = false);
	void fireMouseMotionListeners(const MouseMotionEvent&, bool = false);
	void removeMouseMotionListener(MouseMotionListener*, bool = false);

	KeyListener* addKeyListener(KeyListener, bool = false);
	void fireKeyListeners(const KeyEvent&, bool = false);
	void removeKeyListener(KeyListener*, bool = false);

	CharListener* addCharListener(CharListener);
	void fireCharListeners(const CharEvent&);
	void removeCharListener(CharListener*);

	SizeListener* addSizeListener(SizeListener);
	void fireSizeListeners(float, float);
	void removeSizeListener(SizeListener*);
protected:
    bool focusable;
private:
    friend class GUI;
	Component(const Component&) = delete;
    void _changeOwnerGUI(Component&, GUI*);
};
