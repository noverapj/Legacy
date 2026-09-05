// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__80EFA9F7_450A_4B79_A006_24BDC62E465D__INCLUDED_)
#define AFX_MIXER_H__80EFA9F7_450A_4B79_A006_24BDC62E465D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMixer  
{
private:
	UINT m_iNumMixers;
	HMIXER m_hMixer;
	MIXERCAPS m_kMxcaps;
	
	// record control
	DWORD m_dwRecordType;
	DWORD m_dwRecordSelectID;
	DWORD m_dwRecordMultipleItems;

	// mic
	DWORD m_dwMicIndex;
	
	// playback mic mute
	DWORD m_dwPlayBackMicMuteID;

	// recoder volume
	DWORD m_dwRecoderLineID;
	DWORD m_dwRecoderID;
	DWORD m_dwRecoderMinimum;
	DWORD m_dwRecoderMaximum;

	// Stereo mix
	DWORD m_dwStereoMixIndex;
	
public:
	bool Initialize(HWND hWnd);
	bool Clear();

	char *GetMixerName();
	HMIXER GetHMixer();

	// record control
	bool GetRecordSelectControl();
	bool GetRecordSelectValue( bool &bVal, DWORD dwIndex ) const;
	bool SetRecordSelectValue( bool bVal, DWORD dwIndex ) const;

	// playback mic mute
	bool GetPlayBackMicMuteControl();
	bool GetPlayBackMicMuteValue(bool &bMute) const;
	bool SetPlayBackMicMuteValue(bool bMute) const;

	// recoder volume
	bool GetRecoderVolumeControl();
	bool GetRecoderVolumeValue(DWORD &dwVal) const;
	bool SetRecoderVolumeValue(DWORD dwVal) const;
	int GetRecoderMinimum();
	int GetRecoderMaximum();
	DWORD GetRecoderID();

	DWORD GetMicIndex() const { return m_dwMicIndex; }
	DWORD GetStereoMixIndex() const { return m_dwStereoMixIndex; }

public:
	CMixer();
	virtual ~CMixer();
};

#endif // !defined(AFX_MIXER_H__80EFA9F7_450A_4B79_A006_24BDC62E465D__INCLUDED_)
