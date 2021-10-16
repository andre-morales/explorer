#pragma once

namespace ClientVersion {
	#define E_VERSION_MAJOR "0"
	#define E_VERSION_MINOR "4"
	#define E_VERSION_REVISION "1"

	constexpr const char* const VERSION_STR = E_VERSION_MAJOR "." E_VERSION_MINOR "." E_VERSION_REVISION;

	#define E_BUILD_YEAR "21"
	#define E_BUILD_MONTH "10"
	#define E_BUILD_DAY "15"
	#define E_BUILD_INDEX "000"
	#define E_BUILD_T_STR E_BUILD_YEAR "-" E_BUILD_MONTH "-" E_BUILD_DAY "_" E_BUILD_INDEX

	#ifdef RELEASE
	#define E_BUILD_STR "r" E_BUILD_T_STR
	#elif DEBUG
	#define E_BUILD_STR "Debug"
	#else
	#define E_BUILD_STR "DevB" E_BUILD_T_STR
	#endif

	constexpr const char* const BUILD_STR = E_BUILD_STR;
}


