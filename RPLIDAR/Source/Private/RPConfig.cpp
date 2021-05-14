#include "RPConfig.h"
#include "Paths.hpp"
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <iostream>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "windows.h"

using namespace DevelopHelper;
using namespace rapidxml;

#define clamp(val, lower, upper) (min(upper, max(val, lower)))

void RPConfigMgr::Read()
{
	std::string _filepath = Paths::Instance().GetConfigDir() + "RPLIDAR\\config.xml";
	try
	{
		file<> fdoc(_filepath.data());
		xml_document<> doc;
		doc.parse<0>(fdoc.data());
		xml_node<>* root = doc.first_node("Rplidar");
		config.PortName = root->first_attribute("PortName")->value();
		config.BaudRate = std::atoi(root->first_attribute("BaudRate")->value());
		config.AngleOffset = std::atoi(root->first_attribute("AngleOffset")->value());
		config.DebugRadius = std::atoi(root->first_attribute("DebugRadius")->value());
		config.debugMode = std::atoi(root->first_attribute("DebugMode")->value());
		config.DebugUIScale = std::atof(root->first_attribute("DebugUIScale")->value());
		config.DebugUIScale = clamp(config.DebugUIScale, 0.2f, 1.0f);
		int _areaIndex = 0;
		for (auto _touchAreaNode = root->first_node("TouchArea"); _touchAreaNode; _touchAreaNode = _touchAreaNode->next_sibling("TouchArea"))
		{
			// IOToolkit 默认会 将值除以1000  以限制范围在 [-1,1]
			float _width = std::atoi(_touchAreaNode->first_attribute("Width")->value());
			float _height = std::atoi(_touchAreaNode->first_attribute("Height")->value());
			config.Screens.push_back(RPPoint(_width,_height));
			int poinIndex = 0;
			config.Areas.push_back(std::vector<RPPoint>());
			for (auto _pointNode = _touchAreaNode->first_node("Point"); _pointNode; _pointNode = _pointNode->next_sibling("Point")) {
				RPPoint _point;
				_point.x = std::atof(_pointNode->first_attribute("x")->value());
				_point.y = std::atof(_pointNode->first_attribute("y")->value());
				config.Areas[_areaIndex].push_back(_point);
				if (poinIndex++ >= 4) break;
			}
			_areaIndex++;
			if (_areaIndex > 6) break;
		}
		
	}
	catch (const std::exception&)
	{
		MessageBoxA(NULL, "配置文件有误", "提示", MB_OK);
	}
}


std::string nodeString(float val) {
	return val == 0 ? "0" : std::to_string(val);
}

void RPConfigMgr::Save()
{
	try
	{
		std::string _filepath = Paths::Instance().GetConfigDir() + "RPLIDAR\\config.xml";
		file<> fdoc(_filepath.data());
		xml_document<> doc;
		doc.parse<0>(fdoc.data());
		xml_node<>* root = doc.first_node("Rplidar");
		root->first_attribute("DebugRadius")->value(doc.allocate_string(nodeString(config.DebugRadius).data()));
		root->first_attribute("AngleOffset")->value(doc.allocate_string(nodeString(config.AngleOffset).data()));

		auto _touchAreaNode = root->first_node("TouchArea");
		int index = 0;
		std::string _val = nodeString(config.AngleOffset);

		int _areaIndex = 0;
		for (auto _touchAreaNode = root->first_node("TouchArea"); _touchAreaNode; _touchAreaNode = _touchAreaNode->next_sibling("TouchArea"))
		{
			int poinIndex = 0;
			config.Areas.push_back(std::vector<RPPoint>());
			for (auto _pointNode = _touchAreaNode->first_node("Point"); _pointNode; _pointNode = _pointNode->next_sibling("Point")) {
				_pointNode->first_attribute("x")->value(doc.allocate_string(std::to_string(config.Areas[_areaIndex][poinIndex].x).data()));
				_pointNode->first_attribute("y")->value(doc.allocate_string(std::to_string(config.Areas[_areaIndex][poinIndex].y).data()));
				if (poinIndex++ >= 4) break;
			}
			_areaIndex++;
		}

		std::string buffer;
		rapidxml::print(std::back_inserter(buffer), doc, 0);
		std::basic_ofstream<char> outFile(_filepath.data());
		outFile << buffer;
		outFile.close();
	}
	catch (const std::exception&)
	{

	}
}
