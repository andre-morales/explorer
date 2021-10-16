#include "ExplorerImpl.h"
#include "Explorer.h"
#include "Explorer/Game.h"
#include "Explorer/Camera.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Window.h"
#include "Explorer/Version.h"
#include "Render/Renderer.h"
#include "GUI/Components/Component.h"
#include "Game/Planet.h"
#include "Game/Instance.h"
#include <sstream>

ExplorerImpl::ExplorerImpl(Explorer& e) : This(e){}
ExplorerImpl::~ExplorerImpl(){}

void ExplorerImpl::exit() {
	This.renderer->window->setCloseFlag(true);
}

void ExplorerImpl::quitGame(){
	if(This.gameInstance){
		This.gameInstance->stop();
		This.explorerInstance->universes.clear();
		This.gameInstance->disconnect();
		This.gameInstance->shutdown();
		This.gameInstance.reset();
		This.ui->getPanel("main_menu")->setVisible(true);
	}

	This.renderer->window->freeCursor();
}

void ExplorerImpl::updateDebugText(){
	auto& text = debugText;

	constexpr auto ogn = [](float n){ // Output green number
		std::ostringstream oss;

		oss << "$a" << n << "$r";
		return oss.str();
	};
	constexpr auto ognp = [](float n, int p){ // Output green number
		std::ostringstream oss;
		oss.precision(p);
		oss << "$a" << n << "$r";
		return oss.str();
	};

	std::ostringstream ss;
	ss << "Explorer Alpha v" << ClientVersion::VERSION_STR << " build " << ClientVersion::BUILD_STR;
	ss << "\n$rFPS: " << ogn(This.fps);
	ss << "\nFT: " << ognp(This.frameTime * 1000.0f, 6) << ", BST: " << ognp(This.swapTime  * 1000.0f, 6);

	if(This.gameInstance){
		auto& gi = *This.gameInstance;
		auto& pos = gi.camera->pos;
		auto& rot = gi.camera->rot;
		ss << "\nDownloaded: " << ogn(gi.downBytesCounterFull / 1024.0 / 1024.0 ) << " Mib";
		ss << "\nDownload Speed: $a";
		if(gi.downKiBps > 2048) {
			ss << gi.downKiBps / 1024.0 << "$r MiB/s";
		} else {
			ss << gi.downKiBps << "$r KiB/s";
		}

		ss << "\nUploaded: " << ogn(gi.upBytesCounterFull / 1024.0  / 1024.0 ) << " Mib";
		ss << "\nUpload Speed: $a";
		if(gi.upKiBps > 2048) {
			ss << gi.upKiBps / 1024.0 << "$r MiB/s";
		} else {
			ss << gi.upKiBps << "$r KiB/s";
		}

		ss << "\n$rP: " << ogn(pos.x) << " / " << ogn(pos.y) << " / " << ogn(pos.z) << " [" << ogn(rot.x) << ", " << ogn(rot.y) << "]";
		if(gi.planet) {
			ss << "\n$rTime: " << gi.planet->dayTime;
		}
	}
	text = ss.str();
}