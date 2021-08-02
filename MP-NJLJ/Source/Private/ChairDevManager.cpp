#include "ChairDevManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "TISocket.hpp"
#include "ChairDevice.h"
#include "Paths.hpp"

#define clamp(val, lower, upper) (min(upper, max(val, lower)))


std::string float2string(float InVal) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << InVal;
	return stream.str();
}

ChairDevManager& ChairDevManager::Instance()
{
	static ChairDevManager instance;
	return instance;
}

void ChairDevManager::BindFeedBack(OnUDPReceivedDelegate _callback)
{
	//udpSocket->BindOnUDPReveivedDelegate(_callback);
}

int ChairDevManager::InitDevice(int DevID)
{
	using std::string;
	using namespace rapidxml;
	using namespace DevelopHelper;

	string file_path = Paths::Instance().GetConfigDir() + "MP-NJLJ/config.xml";

	xml_document<> doc;
	try
	{

		
		file<> fdoc(file_path.data());
		doc.parse<0>(fdoc.data());
		xml_node<>* root = doc.first_node("MPRoot")->first_node("ChairDevice");

		for (int _idx = 0; _idx < DevID; ++_idx) {
			root = root->next_sibling("ChairDevice");
		}
		

		speed = static_cast<float>(std::atoi(root->first_attribute("speed")->value()));
		chDevice->Speed = speed;
		
		limitPitch = static_cast<float>(std::atof(root->first_attribute("lim_pitch")->value()));
		limitRoll = static_cast<float>(std::atof(root->first_attribute("lim_roll")->value()));
		limitSpeed = static_cast<float>(std::atof(root->first_attribute("lim_speed")->value()));

		limitX = static_cast<float>(std::atof(root->first_attribute("lim_x")->value()));
		limitY = static_cast<float>(std::atof(root->first_attribute("lim_y")->value()));
		limitZ = static_cast<float>(std::atof(root->first_attribute("lim_z")->value()));

		command = root->first_attribute("command")->value();

		limitAngle = static_cast<float>(std::atof(root->first_attribute("lim_angle")->value()));

		xml_node<>* udpNode = root->first_node("UDP");
		ip = udpNode->first_attribute("ip")->value();
		port = std::atoi(udpNode->first_attribute("port")->value());
		int localPort = std::atoi(udpNode->first_attribute("localPort")->value());
		udpSocket->Initialize(TIType::UDP, localPort);
		udpSocket->SetRemoteAddr(ip, port);

		udpSocket->StartReceived();
		return 1;
	}
	catch (const std::exception error)
	{
		MessageBoxA(nullptr, error.what(), "配置文件有误", MB_OK);
		exit(-1);
	}
	return 0;
}

int ChairDevManager::Reset()
{
	chDevice->Pose = MPVec3::Zero();
	chDevice->Offset = MPVec3::Zero();
	return this->DoAction();
}

const float ChairDevManager::Pitch() const
{
	return chDevice->Pose.X;
}

void ChairDevManager::Pitch(const float _pitch)
{
	chDevice->Pose.X = clamp(_pitch, -limitPitch, limitPitch);;
}

const float ChairDevManager::Roll() const
{
	return chDevice->Pose.Z;
}

void ChairDevManager::Roll(const float _roll)
{
	chDevice->Pose.Z = clamp(_roll,-limitRoll,limitRoll);
}

void ChairDevManager::X(const float _x)
{
	chDevice->Offset.X = clamp(_x, -limitX, limitX);
}

void ChairDevManager::Y(const float _y)
{
	chDevice->Offset.Y = clamp(_y, -limitY, limitY);
}

void ChairDevManager::Z(const float _z)
{
	chDevice->Offset.Z = clamp(_z, -limitZ, limitZ);
}

void ChairDevManager::Angle(const float _angle)
{
	chDevice->Angle = clamp(_angle, -limitAngle, limitAngle);
}

const unsigned char ChairDevManager::Speed() const
{
	return chDevice->Speed;
}

void ChairDevManager::Speed(const float _speed)
{
	chDevice->Speed = clamp(_speed, -limitSpeed, limitSpeed);
}

int ChairDevManager::DoAction()
{
	std::string _actionData = "";
	if (command == "A3") {
		_actionData = "@" + command + ":" + float2string(chDevice->Pose.X) + "," + float2string(chDevice->Pose.Y) + "," + float2string(chDevice->Pose.Z) + ",00#";
	}
	return udpSocket->Send(_actionData.data(),_actionData.size());
}

void ChairDevManager::Exit()
{
	udpSocket->Exit();
}

ChairDevManager::ChairDevManager()
{
	udpSocket = new TISocket();
	chDevice = new ChairDevice();
}

ChairDevManager::ChairDevManager(ChairDevManager&)
{

}

ChairDevManager::~ChairDevManager()
{
	delete udpSocket;
	delete chDevice;
}
