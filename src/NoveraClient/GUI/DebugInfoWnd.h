#pragma once

class SP2Packet;
class ioBaseChar;

class DeveloperInfoWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
	};

protected:
	struct UserInfo
	{
		ioHashString m_szName;
		int m_iGradeLevel;
		int m_iKillDeathLevel;
		int m_iSoloKO;
		int m_iPartyKO;
		int m_iSoloKOed;		
		int m_iPartyKOed;

		UserInfo()
		{
			m_iGradeLevel	= 0;
			m_iKillDeathLevel	= 0;
			m_iSoloKO	= 0;
			m_iPartyKO	= 0;
			m_iSoloKOed	= 0;		
			m_iPartyKOed	= 0;
		}
	};
	typedef std::vector< UserInfo > vUserInfo;
	vUserInfo m_vUserInfo;

	int m_iRoomAverageLevel;
	int m_iRoomLowLevelLimit;
	int m_iRoomHighLevelLimit;
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	void OnDeveloperInfo( SP2Packet &rkPacket );

public:
	DeveloperInfoWnd();
	virtual ~DeveloperInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class DeveloperP2PWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
	};

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	DeveloperP2PWnd();
	virtual ~DeveloperP2PWnd();
};
//////////////////////////////////////////////////////////////////////////
class DeveloperFrameWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
	};

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	DeveloperFrameWnd();
	virtual ~DeveloperFrameWnd();
};
//////////////////////////////////////////////////////////////////////////
class FontTextureTestWnd : public ioWnd
{
protected:
	float m_fFontScale;

public:
	void SetFontSize( int iSize );
	void FontTextureRender();

public:
	FontTextureTestWnd();
	virtual ~FontTextureTestWnd();
};

//////////////////////////////////////////////////////////////////////////
class CharGameInfoListWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,

		ID_LABEL_WEAPON		= 11,
		ID_LABEL_ARMOR		= 12,
		ID_LABEL_HELMET		= 13,
		ID_LABEL_CLOAK		= 14,
		ID_LABEL_OBJECT		= 15,
		ID_LABEL_WEAR		= 16,

		ID_ITEM_TAB			= 21,
		ID_CHAR_INFO_TAB	= 22,
		ID_BUFF_TAB			= 23,

		ID_WEAPON_EDIT		= 101,
		ID_ARMOR_EDIT		= 102,
		ID_HELMET_EDIT		= 103,
		ID_CLOAK_EDIT		= 104,
		ID_OBJECT_EDIT		= 105,
		ID_WEAR_EDIT		= 106,

		ID_WEAPON_APPLY		= 1001,
		ID_ARMOR_APPLY		= 1002,
		ID_HELMET_APPLY		= 1003,
		ID_CLOAK_APPLY		= 1004,
		ID_OBJECT_APPLY		= 1005,
		ID_WEAR_APPLY		= 1006,
	};

	struct CharStateInfo
	{
		ioHashString	szName;
		int				iSlot[MAX_EQUIP_SLOT];
		ioHashStringVec	BuffList;

		float			fGravity;
		float			fForce;
		float			fMoveSpeed;
		float			fJumpPower;

		CharStateInfo()
		{
			Clear();
		}
		
		void Clear()
		{
			memset( iSlot, 0, sizeof( iSlot ) );
			BuffList.clear();
			szName.Clear();

			fGravity	= 0.0f;
			fForce		= 0.0f;
			fMoveSpeed	= 0.0f;
			fJumpPower	= 0.0f;
		}
	};

protected:
	CharStateInfo	m_CharStateInfo;
	ioEdit*			m_pPreEdit;
	DWORD			m_dwCurrTab;	

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void UpdateTab( DWORD dwID );
	void SetFrameElementAlpha( float fRate );
	void EnableEditFocus( DWORD dwID );
	void DisableEditFocus();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void EquipItem( DWORD dwEditID );
	void SetStateInfoChar( ioBaseChar* pChar );

public:
	CharGameInfoListWnd();
	virtual ~CharGameInfoListWnd();
};

//////////////////////////////////////////////////////////////////////////
class SingleSettingWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_BOUND			= 2,
		ID_EXTRA			= 3,
		ID_MEMORY			= 4,
		ID_MODE				= 5,		
		ID_MODE_SUB			= 6,		
		ID_MAP				= 7,
		ID_MODE_APPLY		= 8,
		ID_SPHERE			= 9,
		ID_PAUSE			= 10,
		ID_CHARACTER		= 11,
		ID_OWER				= 12,
		ID_WND_BOUNDARY		= 13,
		

		ID_MODE_LIST		= 1001,
		ID_MODE_SUB_LIST	= 1002,
		ID_MAP_LIST			= 1003,
		ID_CHARACTER_LIST	= 1004,
		ID_OWER_LIST		= 1005,
		ID_SCROLL			= 1006,
	};

	enum
	{
		SUB_MAX			= 20,	//클라이언트에는 서브인덱스에 대한 정보가 없기 때문에 MAX를 사용
		ALPHA_MAX		= 100,
		ALPHA_MIN		= 10,
	};

protected:
	int				m_CurrModeType;	
	int				m_CurrModeSub;
	int				m_CurrModeMapIndex;	

protected:
	static bool		m_bSingle;
	static bool		m_bAutoShow;	
	static bool		m_bPause;
	
protected:
	ioHashString	m_CurrOwner;
	
protected:
	float			m_fCurrAlpha;

public:
	static void SetSingle( bool bSingle );
	static void SetAutoShow( bool bAutoShow );
	static void SetPause( bool bPause );

	static bool IsPause();
	
public:
	inline const ioHashString& GetOwnerChar(){ return m_CurrOwner; }

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnProcess( float fTimePerSec );

protected:
	void UpdateCheckBox();
	void UpdatePauseButton();
	void UpdateMemButtonDsiable();
	bool GetModeNameByType( IN int eType, OUT ioHashString &szName );

protected:
	void ChangeModeButtonTitle( const ioHashString& szNmae );
	void CreateModeList();
	void OpenModekList( ioWnd *pOpenWnd );
	void UpdateMode( int eModeType );

protected:
	void ChangeModeSubButtonTitle( const ioHashString& szName );
	void CreateModeSubList();
	void OpenModeSubList( ioWnd *pOpenWnd );
	void UpdateModeSub( int Param );

protected:
	void ChangeMapIdxButtonTitle( const ioHashString& szName );
	void CreateMapIdxList();
	void OpenMapIdxList( ioWnd *pOpenWnd );
	void UpdateMapIdx( int Param );

public:
	void ChangeCharacterButtonTitle( const ioHashString& szName );

protected:
	void CreateCharacterList();
	void OpenCharacterList( ioWnd *pOpenWnd );	

public:
	void ChangeOwnerButtonTitle( const ioHashString& szName );

public:
	void ParseScroll();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	void SetAlpha(  float fAlpha );

protected:
	void CreateOwnerList();
	void OpenOwnerList( ioWnd *pOpenWnd );	
	void UpdteOwner();

protected:
	virtual void OnRender();

public:
	SingleSettingWnd();
	virtual ~SingleSettingWnd();
};