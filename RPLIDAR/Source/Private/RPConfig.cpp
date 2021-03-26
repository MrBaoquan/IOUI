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

		auto _screenNode = root->first_node("Screen");
		// IOToolkit 默认会 将值除以1000  以限制范围在 [-1,1]
		config.Screen.x = std::atoi(_screenNode->first_attribute("Width")->value());
		config.Screen.y = std::atoi(_screenNode->first_attribute("Width")->value());
		//xml_dock

		auto _touchAreaNode = root->first_node("TouchArea");
		int index = 0;
		for (auto _point = _touchAreaNode->first_node("Point"); _point; _point = _point->next_sibling("Point")) {
			config.Corners[index].x = std::atof(_point->first_attribute("x")->value());
			config.Corners[index].y = std::atof(_point->first_attribute("y")->value());
			index++;
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
		for (auto _point = _touchAreaNode->first_node("Point"); _point; _point = _point->next_sibling("Point")) {
			_point->first_attribute("x")->value(doc.allocate_string(std::to_string(config.Corners[index].x).data()));
			_point->first_attribute("y")->value(doc.allocate_string(std::to_string(config.Corners[index].y).data()));
			index++;
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
