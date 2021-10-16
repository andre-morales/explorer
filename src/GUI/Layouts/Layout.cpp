#include "Layout.h"
#include <iostream>

void Layout::relayout(){
	imp_relayout(*component);
}

void Layout::added(Component&){}
void Layout::imp_relayout(Component&){}