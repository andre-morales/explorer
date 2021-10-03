#include "GridPane.h"

GridPane::GridPane(){}
GridPane::~GridPane(){}

Shared<Component> GridPane::add(Shared<Component> comp, int p){
	Component::add(comp, p);
	relayout();
	return comp;
}

void GridPane::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
	relayout();
}

void GridPane::setGrid(int w, int h){
	gw = w;
	gh = h;
	relayout();
}

void GridPane::setSpacing(float x, float y){
	hs = x;
	vs = y;
	relayout();
}

void GridPane::relayout(){
	int size = children.size();
	if(size == 0) return;

	auto& in = insets;
	float fgw = width - in.left - in.right;  // Full grid width
	float fgh = height - in.top - in.bottom; // Full grid height

	float tw = (fgw - hs * (gw - 1)) / gw;
	float th = (fgh - vs * (gh - 1)) / gh;

	int i = 0; // Grid position.
	int ci;    // Component index.
	for(ci = 0; ci < size; ci++){
		Component& comp = *children[ci];
		vec2i size{1, 1};
		if(ci < sizes.size()){
			size = sizes[ci];
		}

		float tx, ty;
		int txi, tyi;
		switch(direction){
		    default:
			case LR_TB:
				txi = (i % gw);
				tyi = (i / gw);
				tx = in.left + txi * (tw + hs);
				ty = in.top  + tyi * (th + vs);
				break;
			case LR_BT:
				txi = (i % gw);
				tyi = ((gh - i - 1) / gw);
				tx = in.left + txi * (tw + hs);
				ty = in.top  + tyi * (th + vs);
				break;
		}

		i += size.h;

		float cx = tx;
		float cy = ty;
		float cw = tw * size.w;
		float ch = th * size.h + vs * (size.h - 1);
		comp.setBounds(cx, cy, cw, ch);
	}
}

void GridPane::setComponentSize(int i, int w, int h){
	sizes.insert(sizes.begin() + i, {w, h});
}
