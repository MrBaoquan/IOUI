#pragma once

struct EulerAngle
{
public:
	EulerAngle():Pitch(0.f),Yaw(0.f),Roll(0.f){}

	float Pitch=0.f, Yaw=0.f, Roll=0.f;
	const EulerAngle& operator=(const EulerAngle& rhs)
	{
		this->Pitch = rhs.Pitch;
		this->Roll = rhs.Roll;
		this->Yaw = rhs.Yaw;
		return *this;
	}
	static EulerAngle Zero()
	{
		return EulerAngle();
	}
};

#define	BUFFERSIZE	260

class ChairDevice
{
public:
	ChairDevice();
	~ChairDevice();

	const unsigned char* GetActionCommand();

public:
	/*
	 *	座椅的方位信息
	 *  Euler.Pitch 范围 [-15,15]
	 *	Euler.Yaw	范围 [-28,28]
	 *	Euler.Roll	范围 [-18,18]
	 */
	struct EulerAngle	Euler;
	float speed = 0x00;
private:
	unsigned char buffer[BUFFERSIZE];

	unsigned char bufferSpeed[5];
};