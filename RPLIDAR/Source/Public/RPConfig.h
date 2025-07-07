#pragma once

#include <string>
#include <vector>
#include <memory>

struct RPPoint {
	RPPoint() : x(0), y(0) {}
	RPPoint(float _x, float _y) : x(_x), y(_y) {}
	float x;
	float y;
};

struct RPConfig {
	std::string PortName;
	std::string Driver;
	std::string IP;
	int Port;
	int BaudRate = 256000;
	int AngleOffset = 0;
	float DebugRadius = 1.5f;
	float DebugUIScale = 0.5f;
	int debugMode = -1000;
	

	std::vector<std::vector<RPPoint>> Areas;
	std::vector<RPPoint> Screens;
};

class RPConfigMgr {
public:
	static RPConfigMgr& Instance() {
		static RPConfigMgr instance;
		return instance;
	}

	// 禁止复制和赋值
	RPConfigMgr(const RPConfigMgr&) = delete;
	RPConfigMgr& operator=(const RPConfigMgr&) = delete;

	void Read();
	void Save();

	size_t GetConfigCount() const { return configs.size(); }

	const RPConfig* GetConfig(size_t index) const {
		if (index >= configs.size())
			return &GetDefaultConfig();
		return &configs[index];
	}

	RPConfig* GetConfig(size_t index) {
		if (index >= configs.size())
			return &GetDefaultConfig();
		return &configs[index];
	}

private:
	RPConfigMgr() = default;
	~RPConfigMgr() = default;

	std::vector<RPConfig> configs;

	static RPConfig& GetDefaultConfig() {
		static RPConfig defaultConfig = []() -> RPConfig {
			RPConfig cfg;
			cfg.PortName = "";
			cfg.Driver = "";
			cfg.IP = "";
			cfg.Port = 0;
			cfg.BaudRate = 256000;
			cfg.AngleOffset = 0;
			cfg.DebugRadius = 1.5f;
			cfg.DebugUIScale = 0.5f;
			cfg.debugMode = -1000;
			cfg.Areas.clear();
			cfg.Areas.emplace_back(std::vector<RPPoint>{
				RPPoint(0.0f, 0.0f),
					RPPoint(1.0f, 0.0f),
					RPPoint(1.0f, 1.0f),
					RPPoint(0.0f, 1.0f)
			});
			cfg.Screens.clear();
			cfg.Screens.emplace_back(RPPoint(1920.0f, 1080.0f));
			return cfg;
			}();
		return defaultConfig;
	}
};
