// SoundCapture.h: interface for the CSoundCapture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDCAPTURE_H__C542A5F8_B74C_4EA5_A2AC_A8A20EC9779D__INCLUDED_)
#define AFX_SOUNDCAPTURE_H__C542A5F8_B74C_4EA5_A2AC_A8A20EC9779D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoiceChatDefine.h"
#include "SpeexEncoder.h"

class CVoiceChat;

class CSoundCapture  
{
private:	
	LPDIRECTSOUNDCAPTURE       m_pDSCapture;
	LPDIRECTSOUNDCAPTUREBUFFER m_pDSBCapture;
	LPDIRECTSOUNDNOTIFY        m_pDSNotify;
	DSBPOSITIONNOTIFY          m_aPosNotify[ NUM_REC_NOTIFICATIONS + 1 ];  
	DWORD                      m_dwNotifySize;

	DWORD                      m_dwCaptureBufferSize;
	DWORD                      m_dwNextCaptureOffset;

	CSpeexEncoder              m_CSpeexEncoder;

	bool                       m_bThreadLoop;
	HANDLE                     m_hNotificationEvent[NUM_REC_NOTIFICATIONS]; 
	HANDLE                     m_hThread;
	CVoiceChat                *m_pVoiceChat;
	bool                       m_bRecord;
	bool                       m_bExistCaptureData;
	bool                       m_bDoCapture;

	bool InitSoundCapture();
	void FreeSoundCapture();
	bool CreateCaptureBuffer();
	bool InitNotifications();
	bool CreateThreadNotify();
	static DWORD WINAPI ThreadNotify(LPVOID lpParameter);
public:
	HANDLE *GetHNotify() {return m_hNotificationEvent;}
	HANDLE GetHThread() {return m_hThread;}
	bool IsThreadLoop() {return m_bThreadLoop;}
	void SetThreadLoop(bool bThreadLoop) { m_bThreadLoop = bThreadLoop;}
	
	void Init();

	bool EncodeData(const char *pSrcData, int iSrcSize, char *pEncodeData, int &encodeSize, int encodeDataSize);
	bool RecordCapturedData(char *captureData, int captureDataSize, int &captureSize);
	bool IsSlient(char *pCaptureData, int captureDataSize);
	
	bool StartRecord();
	bool StopRecord();
	
	void PushCaptureVoiceData(const char *pData, int iSize);	
	void PushCaptureVoicePeak(int iPeak);
	void SetVoiceChat(const CVoiceChat *pVoiceChat);

	bool IsExistCaptureData() { return m_bExistCaptureData;}
	void SetExistCaptureData(bool bExistCaptureData){m_bExistCaptureData=bExistCaptureData;}

	bool IsDoCapture() {return m_bDoCapture;}
	void SetDoCapture(bool bDoCaupture) {m_bDoCapture=bDoCaupture;}
public:
	CSoundCapture();
	virtual ~CSoundCapture();

};

#endif // !defined(AFX_SOUNDCAPTURE_H__C542A5F8_B74C_4EA5_A2AC_A8A20EC9779D__INCLUDED_)
