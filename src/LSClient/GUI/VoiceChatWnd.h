#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class VoiceChatItem : public ioButton
{
protected:
	enum VoiceType 
	{
		VT_WAIT    = 0,
		VT_DISABLE = 1,
		VT_WAVE_1  = 2,
		VT_WAVE_2  = 3,
		VT_WAVE_3  = 4,
	};
	VoiceType m_eVoiceType;

protected:
	ioUIRenderImage *m_pHead;
	ioUIRenderImage *m_pHeadOutLine;
	ioUIRenderImage *m_pOverImg;
	ioUIRenderImage *m_pOverOutLineImg;
	ioUIRenderImage *m_pDisable;
	ioUIRenderImage *m_pWait;
	ioUIRenderImage *m_pSoundWav1;
	ioUIRenderImage *m_pSoundWav2;
	ioUIRenderImage *m_pSoundWav3;

protected:
	ioHashString m_sName;

	DWORD m_dwPreviousTime;
	DWORD m_dwCheckDuration;

	DWORD m_fNameWidth;

protected:
	VoiceType GetWaveType( int iSoundPeak );
	void PrecessVoiceType();

public:
    virtual void ParseExtraInfo(ioXMLElement &xElement );

protected:
	virtual void AddRenderImage(const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetInfo(const ioHashString &rkID, bool bDisable );

public:
	VoiceChatItem();
	virtual ~VoiceChatItem();
};
//---------------------------------------------------------------------------------------------------------------------------
class VoiceChatOptionBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pStrechNormal;
	ioUIRenderImage *m_pStrechOverAndPush;

public:
	void UpdateImgSize();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	VoiceChatOptionBtn();
	virtual ~VoiceChatOptionBtn();
};
//---------------------------------------------------------------------------------------------------------------------------
class VoiceChatOptionWnd : public ioWnd
{
public:
	enum
	{
		ID_OPTION_BTN_1 = 1,
		ID_OPTION_BTN_2 = 2,
	};

	enum
	{
		MAX_BTN = 2,
	};

protected:
	ioHashString m_sName;

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void iwm_show();
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetInfo( const ioHashString &rsName, bool bDisable );

public:
	VoiceChatOptionWnd();
	virtual ~VoiceChatOptionWnd();
};
//---------------------------------------------------------------------------------------------------------------------------

class VoiceChatWnd  : public ioMovingWnd
{
public:
	enum
	{
		ID_ITEM_WND_1 = 1,
		ID_ITEM_WND_2 = 2,

		ID_VERT_SCROLL = 1000,
	};

	enum 
	{
		MAX_ITEM      = 2,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	struct VoiceInfo
	{
		ioHashString m_sName;
		bool         m_bDisable;

		VoiceInfo()
		{
			m_sName.Clear();
			m_bDisable = false;
		}
	};
	typedef std::vector< VoiceInfo > vVoiceInfoVec;
	vVoiceInfoVec m_vVoiceInfoVec;

protected:
	bool         m_bInit;
	ioPlayStage *m_pPlayStage;
	int          m_iCurPage;
	int          m_iMaxPage;

protected:
	int m_iDefaultX;
	int m_iDefaultY;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void SendVoiceInfo( const ioHashString &rsName, bool bOn );
	void InitByProcess();

	void UpdatePage( int iNewPage );
	void UpdateScroll( bool bSetRange );

	void ChangeOptionAndSetVoice();

public:
	void SetVoiceOn(const ioHashString &rkID);
	void SetVoiceOff(const ioHashString &rkID);
	void SetVoiceEnable(const ioHashString &rkID);
	void SetVoiceDisable(const ioHashString &rkID);

	void AllOtherVoiceOff();
	void AllOtherVoiceDisable();
	void AllOtherVoiceEnable();

	bool IsDisable(const ioHashString &rkID);

	void SetPlayStage( ioPlayStage *pPlayStage );
	ioPlayStage *GetPlayStage() { return m_pPlayStage; }

	void SetAnnounceVoiceChat();
	void SetVoiceDisableEnable( const ioHashString &rkID );

public:
	VoiceChatWnd();
	virtual ~VoiceChatWnd();
};

/////////////////////////////////////////////////////////////////////
class ioVoiceDisableListManager  : public Singleton< ioVoiceDisableListManager >
{
public:
	enum
	{
		MAX_LIST = 100,
	};
protected:
	ioHashStringVec m_vIDList;
	bool m_bSave;

public:
	void LoadList();
	void SaveList();
	bool InsertList(const ioHashString &szID);
	void RemoveList( const ioHashString &szID );

public:
	bool IsVoiceDisable(const ioHashString &szID);

public:
	static ioVoiceDisableListManager &GetSingleton();

public:
	ioVoiceDisableListManager(void);
	virtual ~ioVoiceDisableListManager(void);
};
#define g_VoiceDisableListManager ioVoiceDisableListManager::GetSingleton()

/////////////////////////////////////////////////////////////////////////
class ioVoiceOffListManager : public Singleton< ioVoiceOffListManager >
{
protected:
	enum 
	{ 
		LIVE_TIME = 30000, 
	};

	typedef struct tagOffInfo
	{
		ioHashString m_szID;
		DWORD        m_dwOffTime;
		tagOffInfo()
		{
			m_szID.Clear();
			m_dwOffTime = 0;
		}
	}OffInfo;

	typedef std::vector<OffInfo> OffInfoVec;
	OffInfoVec          m_OffInfoVec;
	CRITICAL_SECTION	m_CriticalSection;

public:
	void Add( const ioHashString &rszID );
	void Delete( const ioHashString &rszID );
	void ProcessDelete();
	void DeleteAll();
	bool IsHave( const ioHashString &rszID );

public:
	static ioVoiceOffListManager &GetSingleton();

public:
	ioVoiceOffListManager();
	virtual ~ioVoiceOffListManager();
};

#define g_VoiceOffListManager ioVoiceOffListManager::GetSingleton()
