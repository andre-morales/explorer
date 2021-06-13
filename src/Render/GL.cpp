#include "GL.h"

GL::GL() : GL(C_UNK){}
GL::GL(int c) : GL((GLContext)c){}
GL::GL(GLContext c) : context(c){
	version_ = get_version();
}

std::string GL::str() const {
	switch(context){
		case C1_1: return "1.1";
		case C1_4: return "1.4";
		case C1_5: return "1.5";
		case C2_0: return "2.0";
		case C3_0: return "3.0";
		case C3_2: return "3.2";
		default: return "Unknown";
	}
}

bool GL::isLegacy() const {
	return context <= C3_1;
}

bool GL::isCore() const {
	return context >= C3_2 && context <= C4_6;
}

bool GL::isCompatibility() const {
	return context >= C3_2_COMPAT;
}

int GL::major() const {
	if(version_ >= V1_0 && version_ <= V1_5){
		return 1;
	} else if(version_ <= V2_1){
		return 2;
	} else if(version_ <= V3_3){
		return 3;
	} else if(version_ <= V4_6){
		return 4;
	}
	return -1;
}

int GL::minor() const {
	switch(version_){
		case V1_0: return 0;
		case V1_1: return 1;
		case V1_2: return 2;
		case V1_3: return 3;
		case V1_4: return 4;
		case V1_5: return 5;
		case V2_0: return 0;
		case V2_1: return 1;
		case V3_0: return 0;
		case V3_1: return 1;
		case V3_2: return 2;
		case V3_3: return 3;
		case V4_0: return 0;
		case V4_1: return 1;
		case V4_2: return 2;
		case V4_3: return 3;
		case V4_4: return 4;
		case V4_5: return 5;
		case V4_6: return 6;
		case V_UNK: default: return -1;
	}
}

GL::GLVersion GL::version() const {
	return version_;
}

GL::GLVersion GL::get_version() const {
	switch(context){
		case C1_0: return V1_0;
		case C1_1: return V1_1;
		case C1_2: return V1_2;
		case C1_3: return V1_3;
		case C1_4: return V1_4;
		case C1_5: return V1_5;
		case C2_0: return V2_0;
		case C2_1: return V2_1;
		case C3_0: return V3_0;
		case C3_1: return V3_1;
		case C3_2: return V3_2;
		case C3_3: return V3_3;
		case C4_0: return V4_0;
		case C4_1: return V4_1;
		case C4_2: return V4_2;
		case C4_3: return V4_3;
		case C4_4: return V4_4;
		case C4_5: return V4_5;
		case C4_6: return V4_6;
		case C_UNK: default: return V_UNK;
	}
}

GL::operator std::string() const {
	return str();
}

GL::operator int() const {
	return (int)context;
}

