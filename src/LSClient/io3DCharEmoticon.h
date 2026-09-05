#pragma once

class ioUI3DRender;
class ioCreateChar;
class ioBaseChar;

class ioINILoader;

struct CHARACTER;

class io3DCharEmoticon
{
private:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_pAvata;

	int	m_iXPos;
	int	m_iYPos;

	enum FaceChangeState
	{
		FC_READY,
		FC_DURATION,
		FC_DONE,
	};

	ioHashString m_ChangeFaceTex;
	FaceChangeState m_FaceChange;
	
	DWORD m_dwSetStart;
	int	  m_iMaxChangeCount;
	int   m_iCurChangeCount;

	ioHashString m_CurChatSound;

	SpecialAtcionType m_SpecialType;

private:
	typedef std::vector< ioHashString > SoundList;
	typedef std::map< int, SoundList > TalkSoundMap;
	static TalkSoundMap m_TalkSoundMap;

	static float m_fCharScale;
	static int m_iXOffset;
	static int m_iYOffset;

	static ioHashString m_FacePostfix;
	static ioHashString m_KillFace;
	static ioHashString m_DieFace;
	static ioHashString m_ActionFace;
	static ioHashString m_FaceChangeType;

	static DWORD m_dwChangeStartGap;
	static DWORD m_dwChangeDuration;
	static DWORD m_dwChangeOnOffGap;

	static DWORD m_dwSpecialStartGap;
	static DWORD m_dwSpecialDuration;
	static DWORD m_dwSpecialOnOffGap;

	static float m_fHairOutLineThick;
	static float m_fHeadOutLineThick;
	static bool m_bChangeOutLineThick;

public:
	static void LoadStaticValue( ioINILoader &rkLoader );

public:
	void Initialize( const ioHashString &rkCamName );
	void SetAvata( const CHARACTER &rkInfo, int iChatLen, bool bNoSound, SpecialAtcionType iType );
	void SetAvata( const CHARACTER &rkInfo, int iChatLen, bool bNoSound, SpecialAtcionType iType, int iViewPortW, int iViewPortH );

protected:
	void PlayTalkSound( int iChangeCount );
	void PlayRandomTalk( const SoundList &rkList );

public:
	void Update( int iXPos, int iYPos );
	void Render();
	void RenderViewPort( int iXPos, int iYPos, int iViewWidth, int iViewHeight );

public:
	io3DCharEmoticon();
	~io3DCharEmoticon();
};

