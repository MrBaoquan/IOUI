#pragma once
#include <functional>
#include <iostream>

class TISocket;
class ChairDevice;

typedef std::function<void(const char*)> OnUDPReceivedDelegate;

class  ChairDevManager
{
public:
	static ChairDevManager& Instance();

	void BindFeedBack(OnUDPReceivedDelegate _callback);

	int InitDevice(int DevID);

	int Reset();

	const float Pitch() const;
	const float Yaw() const;
	const float Roll() const;
	

	void Pitch(const float _pitch);
	void Yaw(const float _yaw);
	void Roll(const float _roll);

	void X(const float _x);
	void Y(const float _y);
	void Z(const float _z);

	void Angle(const float _angle);

	void Speed(const float _speed);

	const unsigned char Speed() const;

	int DoAction();

	void Exit();

private:
	ChairDevManager();
	~ChairDevManager();
	ChairDevManager(ChairDevManager&);

private:
	std::string ip ="127.0.0.1";
	int port = 5050;
	unsigned int bufferLength = 32;

	float speed = 0;

	float limitPitch = 15.f;
	float limitYaw = 15.f;
	float limitRoll = 18.f;
	float limitSpeed = 10.f;

	float limitX = 255;
	float limitY = 255;
	float limitZ = 255;

	float limitAngle = 360;

	std::string command="A3";
	
	TISocket* udpSocket = nullptr;
	ChairDevice* chDevice = nullptr;
};