#include "RPConfig.h"
#include "Paths.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "windows.h"


using namespace DevelopHelper;
using namespace rapidxml;

//static float clamp_float(float val, float lower, float upper) {
//	return std::min(upper, std::max(lower, val));
//}



std::string float_to_string(float val)
{
	// 删除多余0，控制精度
	char buf[32];
	sprintf_s(buf, "%.6f", val);
	// 去除末尾多余的0和小数点
	std::string s = buf;
	s.erase(s.find_last_not_of('0') + 1, std::string::npos);
	if (!s.empty() && s.back() == '.') s.pop_back();
	return s.empty() ? "0" : s;
}

void RPConfigMgr::Read()
{
	configs.clear();

	try
	{
		std::string filepath = Paths::Instance().GetConfigDir() + "RPLIDAR\\config.xml";
		file<> fdoc(filepath.data());
		xml_document<> doc;
		doc.parse<0>(fdoc.data());

		xml_node<>* root = doc.first_node("Root");
		if (!root)
		{
			MessageBoxA(NULL, "配置文件中缺少Root节点", "错误", MB_OK);
			return;
		}

		// 读取所有 <Rplidar> 节点，支持多设备配置
		for (xml_node<>* rplidarNode = root->first_node("Rplidar"); rplidarNode; rplidarNode = rplidarNode->next_sibling("Rplidar"))
		{
			RPConfig cfg;

			if (auto attr = rplidarNode->first_attribute("Driver"))
				cfg.Driver = attr->value();

			if (auto attr = rplidarNode->first_attribute("IP"))
				cfg.IP = attr->value();

			if (auto attr = rplidarNode->first_attribute("PortName"))
				cfg.PortName = attr->value();

			if (auto attr = rplidarNode->first_attribute("Port"))
				cfg.Port = std::atoi(attr->value());

			if (auto attr = rplidarNode->first_attribute("BaudRate"))
				cfg.BaudRate = std::atoi(attr->value());

			if (auto attr = rplidarNode->first_attribute("AngleOffset"))
				cfg.AngleOffset = std::atoi(attr->value());

			if (auto attr = rplidarNode->first_attribute("DebugRadius"))
				cfg.DebugRadius = std::atof(attr->value());

			if (auto attr = rplidarNode->first_attribute("DebugMode"))
				cfg.debugMode = std::atoi(attr->value());

			if (auto attr = rplidarNode->first_attribute("DebugUIScale"))
			{
				cfg.DebugUIScale = std::atof(attr->value());
				cfg.DebugUIScale = std::clamp(cfg.DebugUIScale, 0.2f, 1.0f);
			}

			// 读取该激光雷达下所有 TouchArea 节点
			for (xml_node<>* touchAreaNode = rplidarNode->first_node("TouchArea"); touchAreaNode; touchAreaNode = touchAreaNode->next_sibling("TouchArea"))
			{
				float width = 0;
				float height = 0;
				if (auto attr = touchAreaNode->first_attribute("Width"))
					width = std::atof(attr->value());
				if (auto attr = touchAreaNode->first_attribute("Height"))
					height = std::atof(attr->value());
				cfg.Screens.push_back(RPPoint(width, height));

				std::vector<RPPoint> points;
				int pointCount = 0;
				for (xml_node<>* pointNode = touchAreaNode->first_node("Point"); pointNode && pointCount < 4; pointNode = pointNode->next_sibling("Point"))
				{
					float x = 0, y = 0;
					if (auto attr = pointNode->first_attribute("x"))
						x = std::atof(attr->value());
					if (auto attr = pointNode->first_attribute("y"))
						y = std::atof(attr->value());

					points.emplace_back(x, y);
					pointCount++;
				}

				cfg.Areas.push_back(std::move(points));
			}

			configs.push_back(std::move(cfg));
		}
	}
	catch (const std::exception& ex)
	{
		MessageBoxA(NULL, ("读取配置文件异常: " + std::string(ex.what())).c_str(), "提示", MB_OK);
	}
}


void RPConfigMgr::Save()
{
	try
	{
		std::string filepath = Paths::Instance().GetConfigDir() + "RPLIDAR\\config.xml";

		// 读取旧文件以保留结构
		file<> fdoc(filepath.data());
		xml_document<> doc;
		doc.parse<0>(fdoc.data());

		xml_node<>* root = doc.first_node("Root");
		if (!root)
		{
			MessageBoxA(NULL, "配置文件格式异常，无Root根节点", "错误", MB_OK);
			return;
		}

		// 删除所有Rplidar节点
		for (xml_node<>* node = root->first_node("Rplidar"); node; )
		{
			xml_node<>* to_remove = node;
			node = node->next_sibling("Rplidar");
			root->remove_node(to_remove);
		}

		for (const auto& cfg : configs)
		{
			xml_node<>* rplidarNode = doc.allocate_node(node_element, "Rplidar");

			// 写入Driver，IP和PortName
			if (!cfg.Driver.empty()) {
				rplidarNode->append_attribute(doc.allocate_attribute("Driver", doc.allocate_string(cfg.Driver.c_str())));
			}
			if (!cfg.IP.empty()) {
				rplidarNode->append_attribute(doc.allocate_attribute("IP", doc.allocate_string(cfg.IP.c_str())));
			}
			rplidarNode->append_attribute(doc.allocate_attribute("PortName", doc.allocate_string(cfg.PortName.c_str())));

			char buf[64];

			// Port
			sprintf_s(buf, "%d", cfg.Port);
			rplidarNode->append_attribute(doc.allocate_attribute("Port", doc.allocate_string(buf)));

			// BaudRate
			sprintf_s(buf, "%d", cfg.BaudRate);
			rplidarNode->append_attribute(doc.allocate_attribute("BaudRate", doc.allocate_string(buf)));

			// AngleOffset
			sprintf_s(buf, "%d", cfg.AngleOffset);
			rplidarNode->append_attribute(doc.allocate_attribute("AngleOffset", doc.allocate_string(buf)));

			// DebugRadius
			std::string debugRadiusStr = float_to_string(cfg.DebugRadius);
			rplidarNode->append_attribute(doc.allocate_attribute("DebugRadius", doc.allocate_string(debugRadiusStr.c_str())));

			// debugMode
			sprintf_s(buf, "%d", cfg.debugMode);
			rplidarNode->append_attribute(doc.allocate_attribute("DebugMode", doc.allocate_string(buf)));

			// DebugUIScale
			std::string debugUIScaleStr = float_to_string(cfg.DebugUIScale);
			rplidarNode->append_attribute(doc.allocate_attribute("DebugUIScale", doc.allocate_string(debugUIScaleStr.c_str())));

			size_t areaCount = cfg.Areas.size();
			for (size_t i = 0; i < areaCount; i++)
			{
				xml_node<>* touchAreaNode = doc.allocate_node(node_element, "TouchArea");

				std::string widthStr = "0", heightStr = "0";
				if (i < cfg.Screens.size()) {
					widthStr = float_to_string(cfg.Screens[i].x);
					heightStr = float_to_string(cfg.Screens[i].y);
				}

				touchAreaNode->append_attribute(doc.allocate_attribute("Width", doc.allocate_string(widthStr.c_str())));
				touchAreaNode->append_attribute(doc.allocate_attribute("Height", doc.allocate_string(heightStr.c_str())));

				size_t pointCount = cfg.Areas[i].size();
				for (size_t j = 0; j < pointCount && j < 4; j++)
				{
					xml_node<>* pointNode = doc.allocate_node(node_element, "Point");
					std::string xStr = float_to_string(cfg.Areas[i][j].x);
					std::string yStr = float_to_string(cfg.Areas[i][j].y);

					pointNode->append_attribute(doc.allocate_attribute("x", doc.allocate_string(xStr.c_str())));
					pointNode->append_attribute(doc.allocate_attribute("y", doc.allocate_string(yStr.c_str())));

					touchAreaNode->append_node(pointNode);
				}

				rplidarNode->append_node(touchAreaNode);
			}

			root->append_node(rplidarNode);
		}

		std::string outBuffer;
		rapidxml::print(std::back_inserter(outBuffer), doc, 0);
		std::ofstream ofs(filepath, std::ios::out | std::ios::trunc);
		if (!ofs)
		{
			MessageBoxA(NULL, "保存配置文件失败", "错误", MB_OK);
			return;
		}
		ofs << outBuffer;
		ofs.close();
	}
	catch (const std::exception& ex)
	{
		MessageBoxA(NULL, ("保存配置文件异常: " + std::string(ex.what())).c_str(), "提示", MB_OK);
	}
}



