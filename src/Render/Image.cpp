#include "Image.h"
#include <string.h>
#include "Exception.h"
#include <lodepng.h>

Image::Image(){}
Image::Image(const char* file){
	loadFile(file);
}
Image::Image(const std::string& file){
	loadFile(file.c_str());
}
void Image::loadFile(const char* file){
	std::vector<uint8_t> _data;
	if(lodepng::decode(_data, width, height, file) != 0){
		char msg[64]; msg[63] = 0;
		snprintf(msg, 63, "Tried to load image '%s' but couldn't find the file!", file);
		throw Exception(msg);
	}
	data.reset(new unsigned char[_data.size()]);
	memcpy(data.get(), &_data[0], _data.size());
}
