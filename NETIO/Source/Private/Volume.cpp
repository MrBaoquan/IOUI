#include "Volume.h"
#include <mmdeviceapi.h>
#include <Endpointvolume.h>
#include <algorithm>

#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL)  \
{ (punk)->Release(); (punk) = NULL; }


IAudioEndpointVolume* GetAudioEndpointVolume()
{
	HRESULT hr = NULL;

	IMMDeviceEnumerator* deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
	IMMDevice* defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume* endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;
	return endpointVolume;
}

void ReleaseAudioEndpointVolume(IAudioEndpointVolume* pAudioEndpointVolume)
{
	if (pAudioEndpointVolume != nullptr)
		pAudioEndpointVolume->Release();

	pAudioEndpointVolume = nullptr;
}

VOLUME_API float GetVolume()
{
	HRESULT hr = NULL;

	CoInitialize(NULL);

	IAudioEndpointVolume* endpointVolume = GetAudioEndpointVolume();
	// -------------------------
	float currentVolume = 0;
	endpointVolume->GetMasterVolumeLevel(&currentVolume);
	//printf("Current volume in dB is: %f\n", currentVolume);
	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	ReleaseAudioEndpointVolume(endpointVolume);
	CoUninitialize();

	return currentVolume;
}

VOLUME_API void SetVolume(float nVolume)
{
	nVolume = std::clamp(nVolume, 0.f, 1.f);
	HRESULT hr = NULL;
	CoInitialize(NULL);
	IAudioEndpointVolume* endpointVolume = GetAudioEndpointVolume();
	hr = endpointVolume->SetMasterVolumeLevelScalar(nVolume, NULL);
	ReleaseAudioEndpointVolume(endpointVolume);
	CoUninitialize();
}

VOLUME_API void SetMute(bool bMute)
{
	CoInitialize(NULL);
	IAudioEndpointVolume* endpointVolume = GetAudioEndpointVolume();
	endpointVolume->SetMute(bMute, NULL);
	ReleaseAudioEndpointVolume(endpointVolume);
	CoUninitialize();
}

VOLUME_API bool GetMute()
{
	CoInitialize(NULL);
	BOOL bMute = FALSE;
	IAudioEndpointVolume* endpointVolume = GetAudioEndpointVolume();
	endpointVolume->GetMute(&bMute);
	ReleaseAudioEndpointVolume(endpointVolume);
	CoUninitialize();
	return bMute == TRUE;
}

VOLUME_API void IncreaseVolume(float nVolume/*=0.1f*/)
{
	auto currentVolume = GetVolume();
	currentVolume += nVolume;
	SetVolume(currentVolume);
}

VOLUME_API void DecreaseVolume(float nVolume/*=0.1f*/)
{
	auto currentVolume = GetVolume();
	currentVolume -= nVolume;
	SetVolume(currentVolume);
}

