// SoundPlayer.h: interface for the CSoundPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDPLAYER_H__36AD498E_6E4A_44DA_AA49_2C4BAE0BE4D5__INCLUDED_)
#define AFX_SOUNDPLAYER_H__36AD498E_6E4A_44DA_AA49_2C4BAE0BE4D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoiceChatDefine.h"
#include "SpeexDecoder.h"
#include "ioHashStringVC.h"

class CSoundPlayer  
{
private:
	ioHashStringVC          m_cName;
	LPDIRECTSOUNDBUFFER		m_pDSBuffer;
	DWORD                   m_dwNextWriteOffset;

	bool                    m_bPlay;
	bool                    m_bCheckPlayStop;
	bool                    m_bRefusedMyVoice;
	bool                    m_bMyVoice;
	
	int                     m_iPreviousPlayCursor;
	int                     m_iCntCircularPlay;
	int                     m_iCntCircularWrite;

	CSpeexDecoder           m_cSpeexDecoder;

	bool RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB);
public:
	bool GetBuffer(char* pBuffer, int iSize, DWORD dwPos);

	bool WriteBuffer(const char* pBuffer, int iSize);
	bool DecodeWriteBuffer(const char* pBuffer, int iSize);

	void CheckPlayStop();
	
	bool Reset();
	bool Play();
	bool Stop();

	bool IsPlay() {return m_bPlay;}

	void SetVolume(int iVolume);

	void SetCheckPlayStop(bool bCheckPlayStop);
	bool IsCheckPlayStop();

	bool IsRefusedMyVoice();
	void SetRefusedMyVoice(bool bRefusedMyVoice);

	void SetName(const ioHashStringVC &rcName);
	const ioHashStringVC& GetName() const;

	int GetPeak();

	LPDIRECTSOUNDBUFFER &GetDSBuffer();
	
public:
	void SetMyVoice(bool bMyVoice);
	
public:
	CSoundPlayer();
	virtual ~CSoundPlayer();

};

#endif // !defined(AFX_SOUNDPLAYER_H__36AD498E_6E4A_44DA_AA49_2C4BAE0BE4D5__INCLUDED_)
