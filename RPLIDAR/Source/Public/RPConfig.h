#pragma once

#include <string>

struct RPPoint {
	RPPoint() {
		x = 0;
		y = 0;
	}
	RPPoint(float _x, float _y) {
		x = _x;
		y = _y;
	}
	float x;
	float y;
};

struct RPConfig {
	std::string PortName;
	RPPoint Screen;
	int BaudRate;
	int AngleOffset;
	float DebugRadius = 1.5f;
	RPPoint Corners[4];
	int debugMode = -1000;
};

class RPConfigMgr {
public:
	void Read();
	void Save();

	RPConfig config;
};



