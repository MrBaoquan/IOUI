#pragma once
#include <functional>
#ifdef CHAIRDEVICE_EXPORTS
#define CHDEVAPI __declspec(dllexport)
#else
#define CHDEVAPI __declspec(dllimport)
#endif

class TISocket;
class ChairDevice;

typedef std::function<void(const char*)> OnUDPReceivedDelegate;

class CHDEVAPI  ChairDevManager
{
public:
	static ChairDevManager& Instance();

	void BindFeedBack(OnUDPReceivedDelegate _callback);

	int InitDevice();

	int Reset();

	const float Pitch() const;

	const float Roll() const;

	const unsigned char Speed() const;

	void Pitch(const float _pitch);
	
	void Roll(const float _roll);

	void Speed(const float _speed);

	int DoAction();

	void Exit();

private:
	ChairDevManager();
	~ChairDevManager();
	ChairDevManager(ChairDevManager&);

private:
	char* ip="127.0.0.1";
	int port = 5050;
	unsigned int bufferLength = 32;

	float speed = 0;

	float limitPitch = 15.f;

	float limitRoll = 18.f;
	
	TISocket* udpSocket = nullptr;
	ChairDevice* chDevice = nullptr;
};