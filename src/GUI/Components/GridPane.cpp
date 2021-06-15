#include "GridPane.h"

GridPane::GridPane(){}
GridPane::~GridPane(){}

Shared<Component> GridPane::add(Shared<Component> comp){
	Component::add(comp);
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
	float cw = width - in.left - in.right;
	float ch = height - in.top - in.bottom;

	float tw = (cw - hs * (gw - 1)) / gw;
	float th = (ch - vs * (gh - 1)) / gh;

	for(int i = 0; i < size; i++){
		auto comp = children[i];

		float tx, ty;
		int txi, tyi;
		switch(orientation){
		    default:
			case Orientation::LR_TB:
				txi = (i % gw);
				tyi = (i / gw);
				tx = in.left + txi * (tw + hs);
				ty = in.top  + tyi * (th + vs);
				break;
			case Orientation::LR_BT:
				txi = (i % gw);
				tyi = ((gh - i - 1) / gw);
				tx = in.left + txi * (tw + hs);
				ty = in.top  + tyi * (th + vs);
				break;
		}

		comp->setBounds(tx, ty, tw, th);
	}
}

