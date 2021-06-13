#pragma once
#include <string>

struct GL {
	enum GLContext {
		C_UNK,
		C1_0, C1_1, C1_2, C1_3, C1_4, C1_5,
		C2_0, C2_1,
		C3_0, C3_1, C3_2, C3_3, 
		C4_0, C4_1, C4_2, C4_3, C4_4, C4_5, C4_6,

		C3_2_COMPAT, C3_3_COMPAT,
		C4_0_COMPAT, C4_1_COMPAT, C4_2_COMPAT, C4_3_COMPAT, C4_4_COMPAT, C4_5_COMPAT, C4_6_COMPAT
	};
	enum GLVersion {
		V_UNK,
		V1_0, V1_1, V1_2, V1_3, V1_4, V1_5,
		V2_0, V2_1,
		V3_0, V3_1, V3_2, V3_3,
		V4_0, V4_1, V4_2, V4_3, V4_4, V4_5, V4_6
	};
	GLContext context;

	GL();
	GL(int);
	GL(GLContext);
	bool isLegacy() const;
	bool isCore() const;
	bool isCompatibility() const;
	int major() const;
	int minor() const;
	GLVersion version() const;
	std::string str() const;
	operator int() const;
	operator std::string() const;
private:
	GLVersion version_;
	GLVersion get_version() const;
};
