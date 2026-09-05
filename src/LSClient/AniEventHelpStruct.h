#pragma once


#include "EncryptValue.h"

typedef struct tagAniSoundEventInfo
{
	ioHashString m_AniName;
	ioHashString m_EventName;
	UniqueObjID m_SoundID;

	tagAniSoundEventInfo()
	{
		Init();
	}

	void Init()
	{
		m_SoundID = 0;
	}

} AniSoundEventInfo;

typedef std::vector< AniSoundEventInfo > AniSoundEventInfoList;


