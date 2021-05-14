#pragma once

#include <string>
#include <vector>
#include <memory>

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
	int BaudRate;
	int AngleOffset;
	float DebugRadius = 1.5f;
	float DebugUIScale = 0.5f;
	std::vector<std::vector<RPPoint>> Areas;
	std::vector<RPPoint> Screens;
	int debugMode = -1000;
};

class RPConfigMgr {
public:
	void Read();
	void Save();

	RPConfig config;
};



