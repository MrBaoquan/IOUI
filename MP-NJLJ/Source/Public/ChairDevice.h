#pragma once
#include <string>

struct MPVec3
{
public:
	MPVec3():X(0.f),Y(0.f),Z(0.f){}

	float X=0.f, Y=0.f, Z=0.f;
	const MPVec3& operator=(const MPVec3& rhs)
	{
		this->X = rhs.X;
		this->Z = rhs.Z;
		this->Y = rhs.Y;
		return *this;
	}
	static MPVec3 Zero()
	{
		return MPVec3();
	}
};

#define	BUFFERSIZE	260

class ChairDevice
{
public:
	ChairDevice();
	~ChairDevice();

public:
	/*
	 *	座椅的方位信息
	 *  Euler.Pitch 范围 [-15,15]
	 *	Euler.Yaw	范围 [-28,28]
	 *	Euler.Roll	范围 [-18,18]
	 */
	struct MPVec3 Pose;
	struct MPVec3 Offset;
	float Speed = 0x00;
	float Angle = 0;
private:
	unsigned char buffer[BUFFERSIZE];

	unsigned char bufferSpeed[5];
};