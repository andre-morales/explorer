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
	sh<class Layout> layout;
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
	Component(sh<Layout>);
	virtual ~Component();

	// Hierarchy
	virtual sh<Component> add(sh<Component>, int = -1);
	template <class T> sh<T> addT(sh<T>&& c){
		add(c);
		return c;
	}
	template <class T, class... Args> sh<T> addNew(Args&&... args){
		auto ptr = std::shared_ptr<T>(new T(args...));
		add(ptr);
		return ptr;
	}
	virtual Shared<Component> remove(Shared<Component>);
	virtual Shared<Component> remove(const std::string&);
	Shared<Component> getChild(const std::string&);
	void setLayout(sh<Layout>);
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
