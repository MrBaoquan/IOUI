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
	 *	���εķ�λ��Ϣ
	 *  Euler.Pitch ��Χ [-15,15]
	 *	Euler.Yaw	��Χ [-28,28]
	 *	Euler.Roll	��Χ [-18,18]
	 */
	struct EulerAngle	Euler;
	unsigned char bSpeed = 0x00;
private:
	unsigned char buffer[BUFFERSIZE];
};