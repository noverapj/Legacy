// SoundPlayerManager.h: interface for the CSoundPlayerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDPLAYERMANAGER_H__29817139_FFBB_4128_8924_4C95100D9A34__INCLUDED_)
#define AFX_SOUNDPLAYERMANAGER_H__29817139_FFBB_4128_8924_4C95100D9A34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SoundPlayer.h"

class __EX CSoundPlayerManager  
{
public:
	typedef std::map< ioHashStringVC, CSoundPlayer* > MSoundPlayer;
protected:
	LPDIRECTSOUND8          m_pDS;
	MSoundPlayer            m_mSoundPlayer;

	bool SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
	bool CreateBuffer(LPDIRECTSOUNDBUFFER &pDSBuffer);

	CSoundPlayer* GetSoundPlayer( const ioHashStringVC &rkName );

	bool Initialize( HWND  hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
public:
	void AddSoundPlayer( const ioHashStringVC &rkName, const bool bMyVoice = false);
	void RemoveSoundPlayer( const ioHashStringVC &rkName );
	void RemoveAllSoundPlayer();
	bool IsSoundPlayer( const ioHashStringVC &rkName );

	void Reset(const ioHashStringVC &rcName);
	void Stop(const ioHashStringVC &rcName);
	void Play(const ioHashStringVC &rcName);
	bool PushData(const ioHashStringVC &rcName, const VOICEDATA &rData);

	bool IsCheckPlayStop(const ioHashStringVC &rcName);
	bool IsRefuse(const ioHashStringVC &rcName);
	bool IsPlay(const ioHashStringVC &rcName);

	void SetRefuse(const ioHashStringVC &rcName, bool bRefuse);

	int GetPeak(const ioHashStringVC &rcName);
	
	void CheckPlayStop();

	void SetAllVolume(int iVolume);
public:
	CSoundPlayerManager();
	virtual ~CSoundPlayerManager();
};

#endif // !defined(AFX_SOUNDPLAYERMANAGER_H__29817139_FFBB_4128_8924_4C95100D9A34__INCLUDED_)
