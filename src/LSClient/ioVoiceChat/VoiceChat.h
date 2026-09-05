// VoiceChat.h: interface for the CVoiceChat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOICECHAT_H__46F321C4_7AE4_4294_91FB_01988F6C4CAB__INCLUDED_)
#define AFX_VOICECHAT_H__46F321C4_7AE4_4294_91FB_01988F6C4CAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VoiceChatDefine.h"
#include "SoundPlayerManager.h"
#include "SoundCapture.h"
#include "Mixer.h"

class __EX CVoiceChat  : public CSoundPlayerManager
{
protected:
	friend class CSoundCapture;
public:
	typedef std::deque< VOICEDATA* > QVoiceData;
	typedef std::deque< int > QVoicePeak;

protected:	
	static CVoiceChat	*sg_Instance;

	QVoiceData			m_qVoiceData;
	QVoicePeak			m_qCapturePeak; 
	CSoundCapture		m_cSoundCapture;
	CMixer				m_cMixer;
	CRITICAL_SECTION	m_CriticalSection;

protected:
	bool				m_bWorkRecordVolume;
	
protected:
	void DeleteAllCaptureVoiceData();
	void PushCaptureVoiceData(const char *pData, int iSize);
	void PushCapturePeak(int iPeak);

public:
	static CVoiceChat& GetInstance();
	static void ReleaseInstance();

public:
	void Init(HWND hWnd);
	void InitCapture();
	void PopData(VOICEDATA &rOutVD);
	bool IsEmptyData();
	
	void PopCapturePeak(int &iPeak);
	bool IsEmptyCapturePeak();

	void StartCapture();
	void StopCapture();
	bool IsExistCaptureData() { return m_cSoundCapture.IsExistCaptureData();}
	
public:
	// hd 사운드 드라이버에서는 동작하지 않음.
	void SetMic(); 
	void SetPlayBackMic(bool bPlay);
	void SetStereoMix();

public:
	bool GetRecoderVolumeValue(DWORD &dwVal) const;
	bool SetRecoderVolumeValue(DWORD dwVal) const;
	int GetRecoderMinimum();
	int GetRecoderMaximum();
	bool IsWorkRecordVolume() const;

private:
	CVoiceChat();
	virtual ~CVoiceChat();
};

#define g_VoiceChat CVoiceChat::GetInstance()

#endif // !defined(AFX_VOICECHAT_H__46F321C4_7AE4_4294_91FB_01988F6C4CAB__INCLUDED_)

