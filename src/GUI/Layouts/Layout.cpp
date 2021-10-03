#include "Layout.h"

Layout::Layout(){}
Layout::~Layout(){}

void Layout::relayout(){
	imp_relayout(*component);
}
void Layout::added(Component&){}
void Layout::imp_relayout(Component&){}