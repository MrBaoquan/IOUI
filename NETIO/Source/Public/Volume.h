#ifndef __VOLUME_H__
#define __VOLUME_H__

#define VOLUME_API 


VOLUME_API float GetVolume();
VOLUME_API void SetVolume(float nVolume);
VOLUME_API void SetMute(bool bMute);
VOLUME_API bool GetMute();

// 增加音量
VOLUME_API void IncreaseVolume(float nVolume=0.1f);
// 减少音量
VOLUME_API void DecreaseVolume(float nVolume=0.1f);

#endif

