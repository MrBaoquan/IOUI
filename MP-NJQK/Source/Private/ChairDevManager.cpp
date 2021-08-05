#include "ChairDevManager.h"
#include <iostream>
#include <math.h>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "TISocket.hpp"
#include "ChairDevice.h"
#include "Paths.hpp"

#define clamp(val, lower, upper) (min(upper, max(val, lower)))

ChairDevManager& ChairDevManager::Instance()
{
	static ChairDevManager instance;
	return instance;
}

void ChairDevManager::BindFeedBack(OnUDPReceivedDelegate _callback)
{
	//udpSocket->BindOnUDPReveivedDelegate(_callback);
}

int ChairDevManager::InitDevice()
{
	using std::string;
	using namespace rapidxml;
	using namespace DevelopHelper;

	//	��ȡԶ�� ip �� port
	string file_path = Paths::Instance().GetConfigDir() + "MOTION-PLAT/config.xml";

	xml_document<> doc;
	try
	{
		file<> fdoc(file_path.data());
		doc.parse<0>(fdoc.data());
		xml_node<>* root = doc.first_node("ChairDevice");

		speed = static_cast<float>(std::atoi(root->first_attribute("speed")->value()));
		chDevice->Speed = speed;
		limitPitch = static_cast<float>(std::atof(root->first_attribute("lim_pitch")->value()));
		limitRoll = static_cast<float>(std::atof(root->first_attribute("lim_roll")->value()));
		limitSpeed = static_cast<float>(std::atof(root->first_attribute("lim_speed")->value()));

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
		MessageBoxA(nullptr, error.what(), "Error:ȷ���ļ��Ƿ����", MB_OK);
		exit(-1);
	}
	return 0;
}

int ChairDevManager::Reset()
{
	chDevice->Pose = MPVec3::Zero();
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
	return udpSocket->Send(reinterpret_cast<char*>(const_cast<unsigned char*>(chDevice->GetActionCommand())), bufferLength);
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
