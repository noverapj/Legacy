#pragma once

#include "ioUserKeyInput.h"

// DB 저장되는 키 정보
#define MAX_CONTROL_KEYS_PLUS_ONE       256

struct ControlKeys
{
	char m_szControlKeys[MAX_CONTROL_KEYS_PLUS_ONE];

	bool IsRight() const
	{
		if( strcmp( m_szControlKeys, "" ) == 0 )
			return false;

		for (int i = 0; i < MAX_CONTROL_KEYS_PLUS_ONE ; i++)
		{
			if( m_szControlKeys[i] == NULL )
				break;

			if ((!COMPARE(m_szControlKeys[i], 'A', 'Z'+1)) &&
				(!COMPARE(m_szControlKeys[i], 'a', 'z'+1)) &&
				(!COMPARE(m_szControlKeys[i], '0', '9'+1)) )
			{
				return false;
			}

		}

		return true;
	}

	void Clear()
	{
		ZeroMemory( m_szControlKeys, sizeof( m_szControlKeys ) );
	}

	ControlKeys()
	{
		Clear();
	}
};
//

class CKeyInfo
{
public:
	// m_vKeyInfo vector의 array 역할을 하므로 0부터 순차적으로 증가해야 함. 
	// DB에도 설정이 저장되므로 순서를 유지하기 위해서 새로운 키값은 끝에 추가
	enum KeyTypeArray 
	{
		KTA_NONE     = -1,
		// game key
		KTA_UP       = 0,
		KTA_DOWN,
		KTA_LEFT,
		KTA_RIGHT,
		KTA_ATTACK,
		KTA_DEFENSE,
		KTA_JUMP,
		// second game key
		KTA_SECOND_UP,
		KTA_SECOND_DOWN,
		KTA_SECOND_LEFT,
		KTA_SECOND_RIGHT,
		KTA_SECOND_ATTACK,
		KTA_SECOND_DEFENSE,
		KTA_SECOND_JUMP,
		// skill key
		KTA_CLOAK_SKILL, // A+S 단축키
		KTA_HELM_SKILL,  // A+D 단축키
		KTA_ARMOR_SKILL, // A+S+D 단축키
		KTA_WEAPON_SKILL,// S+D 단축키
		// diagonal
		KTA_LEFTUP,
		KTA_RIGHTUP,
		KTA_LEFTDOWN,
		KTA_RIGHTDOWN,
		// function
		KTA_HEADQUARTERS,
		KTA_QUICK_START, 
		KTA_FIGHTING, 
		KTA_SHOP,
		KTA_HELP,
		KTA_SQUARE,
		KTA_GROWTH,
		KTA_GUILD,
		KTA_OPTION,
		KTA_MESSENGER,
		KTA_VOICE,
		KTA_SCREENSHOT,
		KTA_MOVIECAPTURE,
		KTA_SPECIAL_MOTION,
		KTA_ZOOMIN,
		KTA_ZOOMOUT,
		KTA_USERS_CUSTOM_SOUND,

		// soldier
		KTA_SOLDIER1,
		KTA_SOLDIER2,
		KTA_SOLDIER3,
		KTA_SOLDIER4,
		KTA_SOLDIER5,
		KTA_SOLDIER6,
		KTA_SOLDIER7,
		KTA_SOLDIER8,
		KTA_SOLDIER9,
		KTA_SOLDIER10,
		KTA_SOLDIER11,
		KTA_SOLDIER12,
		// Macro
		KTA_MACRO1,
		KTA_MACRO2,
		KTA_MACRO3,
		KTA_MACRO4,
		KTA_MACRO5,
		KTA_MACRO6,
		KTA_MACRO7,
		KTA_MACRO8,
		KTA_MACRO9,
		KTA_MACRO10,
		KTA_MACRO11,
		KTA_MACRO12,

		// function
		KTA_QUEST,		
		KTA_PRESENT,
		KTA_MYHOMEPAGE,

		// soldier
		KTA_SOLDIER13,
		KTA_SOLDIER14,
		KTA_SOLDIER15,
		KTA_SOLDIER16,
		KTA_SOLDIER17,
		KTA_SOLDIER18,
		KTA_SOLDIER19,
		KTA_SOLDIER20,
		KTA_SOLDIER21,
		KTA_SOLDIER22,
		KTA_SOLDIER23,
		KTA_SOLDIER24,
		KTA_SOLDIER25,
		KTA_SOLDIER26,
		KTA_SOLDIER27,
		KTA_SOLDIER28,
		KTA_SOLDIER29,
		KTA_SOLDIER30,
		KTA_SOLDIER31,
		KTA_SOLDIER32,
		KTA_SOLDIER33,
		KTA_SOLDIER34,
		KTA_SOLDIER35,
		KTA_SOLDIER36,

		KTA_CONSUMPTION1,			// 소모품 슬롯
		KTA_CONSUMPTION2,
		KTA_CONSUMPTION3,
		KTA_CONSUMPTION4,

		KTA_PET,
		KTA_COOLTIME_RESET,
		KTA_BATTLE_MODE_ENTRY,
		KTA_BATTLE_MODE_SOLDIER_SELECT,
		KTA_BATTLE_MODE_TAG_SWITCH,

		KTA_SOLDIER37,
		KTA_SOLDIER38,
		KTA_SOLDIER39,
		KTA_SOLDIER40,
		KTA_SOLDIER41,
		KTA_SOLDIER42,
		KTA_SOLDIER43,
		KTA_SOLDIER44,
		KTA_SOLDIER45,
		KTA_SOLDIER46,
		KTA_SOLDIER47,
		KTA_SOLDIER48,
		KTA_SOLDIER49,
		KTA_SOLDIER50,
		KTA_MAX,
	};
	enum 
	{
		MAX_MACRO = 12,
//		CUSTOMKEY_VERSION = 20100708,
//		CUSTOMKEY_VERSION = 20120912,
		CUSTOMKEY_VERSION = 20140403,
	};

	typedef struct tagMecroTextInfo
	{
		bool         m_bAll;
		ioHashString m_szText;

		tagMecroTextInfo()
		{
			m_bAll = true;
			m_szText.Clear();
		}
	}MecroTextInfo;
protected:
	typedef std::vector<WORD> vWORD;
	vWORD m_vKeyInfo;

	typedef std::vector<MecroTextInfo> vMecroTextInfo;
	vMecroTextInfo m_vMecroTextInfo;

	bool  m_bUse;
	bool  m_bAutoFix;

protected:
	char *CKeyInfo::GetINIKeyName(KeyTypeArray eKeyTypeArray);

public:
	CKeyInfo& operator = ( IN CKeyInfo &rKeyInfo );

public:
	WORD GetKey( KeyTypeArray eKeyTypeArray );
	bool SetKey( KeyTypeArray eKeyTypeArray, WORD wKey );

	void Load();
	virtual void SaveKey();
	virtual void SaveMacroTextInfo();

	bool IsRightKey(WORD wKey);
	bool IsUseKeys(){ return m_bUse; }
	bool IsRightMacro( const CKeyInfo &rKeyInfo );
	bool IsAutoFix() const { return m_bAutoFix; }

	MecroTextInfo *GetMacroTextInfo( KeyTypeArray eKeyTypeArray );
	bool SetMacroTextInfo(KeyTypeArray eKeyTypeArray, const MecroTextInfo &rMacroTextInfo );

	KeyTypeArray GetEqual( WORD wKey );

public:
	CKeyInfo();
	virtual ~CKeyInfo();
};
//----------------------------------------------------------------------------------------------------------------------
class CDefaultKeyInfo : public CKeyInfo
{
public:
	virtual void Load();

public:
	CDefaultKeyInfo();
	virtual ~CDefaultKeyInfo();
};
//----------------------------------------------------------------------------------------------------------------------
class CCustomKeyInfo : public CKeyInfo
{
public:
	virtual void Load( const ControlKeys &rkControlKeys );
	virtual void SaveKey();
	virtual void SaveMacroTextInfo();

public:
	CCustomKeyInfo();
	virtual ~CCustomKeyInfo();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CJoyKeyInfo
{
public:
	enum KeyTypeArray // m_vKeyInfo vector의 array 역할을 하므로 0부터 순차적으로 증가해야 함.
	{
		JTA_NONE     = -1,
		// game key
		JTA_UP       = 0,
		JTA_DOWN,
		JTA_LEFT,
		JTA_RIGHT,
		JTA_ATTACK,
		JTA_DEFENSE,
		JTA_JUMP,
		// skill key
		JTA_CLOAK_SKILL, // A+S 단축키
		JTA_HELM_SKILL,  // A+D 단축키
		JTA_ARMOR_SKILL, // A+S+D 단축키
		JTA_WEAPON_SKILL,// S+D 단축키
		// soldier
		JTA_SOLDIER1,
		JTA_SOLDIER2,
		JTA_SOLDIER3,
		JTA_SOLDIER4,
		JTA_SOLDIER5,
		JTA_SOLDIER6,
		JTA_SOLDIER7,
		JTA_SOLDIER8,
		JTA_SOLDIER9,
		JTA_SOLDIER10,
		JTA_SOLDIER11,
		JTA_SOLDIER12,
		JTA_SOLDIER13,
		JTA_SOLDIER14,
		JTA_SOLDIER15,
		JTA_SOLDIER16,
		JTA_SOLDIER17,
		JTA_SOLDIER18,
		JTA_SOLDIER19,
		JTA_SOLDIER20,
		JTA_SOLDIER21,
		JTA_SOLDIER22,
		JTA_SOLDIER23,
		JTA_SOLDIER24,
		JTA_SOLDIER25,
		JTA_SOLDIER26,
		JTA_SOLDIER27,
		JTA_SOLDIER28,
		JTA_SOLDIER29,
		JTA_SOLDIER30,
		JTA_SOLDIER31,
		JTA_SOLDIER32,
		JTA_SOLDIER33,
		JTA_SOLDIER34,
		JTA_SOLDIER35,
		JTA_SOLDIER36,

		JTA_MAX,
	};
	enum 
	{
//		CUSTOMKEY_VERSION = 20100708,
		CUSTOMKEY_VERSION = 20120912,
	};

protected:
	typedef std::vector<WORD> vWORD;
	vWORD m_vKeyInfo;

	bool  m_bUse;
	
protected:
	char *CJoyKeyInfo::GetINIKeyName(KeyTypeArray eKeyTypeArray);

public:
	CJoyKeyInfo& operator = ( IN CJoyKeyInfo &rKeyInfo );

public:
	WORD GetKey( KeyTypeArray eKeyTypeArray );
	bool SetKey( KeyTypeArray eKeyTypeArray, WORD wKey );

	void Load();
	virtual void SaveKey();

	bool IsRightKey(WORD wKey);
	bool IsUseKeys(){ return m_bUse; }

	KeyTypeArray GetEqual( WORD wKey );

	void Clear();

public:
	CJoyKeyInfo();
	virtual ~CJoyKeyInfo();
};
//----------------------------------------------------------------------------------------------------------------------
class CDefaultJoyKeyInfo : public CJoyKeyInfo
{
public:
	virtual void Load();

public:
	CDefaultJoyKeyInfo();
	virtual ~CDefaultJoyKeyInfo();
};
//----------------------------------------------------------------------------------------------------------------------
class CCustomJoyKeyInfo : public CJoyKeyInfo
{
public:
	virtual void Load();
	virtual void SaveKey();

public:
	CCustomJoyKeyInfo();
	virtual ~CCustomJoyKeyInfo();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioKeyManager : public Singleton< ioKeyManager >
{
private:
	CDefaultKeyInfo m_DefaultKeyInfo;
	CCustomKeyInfo  m_CustomKeyInfo;

	CDefaultJoyKeyInfo m_DefaultJoyKeyInfo;
	CCustomJoyKeyInfo m_CustomJoyKeyInfo;

	typedef std::map< WORD, ioHashString > KeyTextMap;
	KeyTextMap m_KeyTextMap;

	bool m_bUseCustom;
	bool m_bSaveKey;
	bool m_bUseJoyCustom;
	bool m_bSaveJoyKey;

	bool m_bSaveMacroText;

	ControlKeys m_kServerKeys;

	int m_iLastSoldierKeyPressArray;		// 마지막에 선택한 용병 슬롯 키 배열 값
	
private:
	WORD GetKey( CKeyInfo::KeyTypeArray eKeyTypeArray );
	WORD GetJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray );

	void SendKey();

public:
	void Load();	
	void SaveKey();
	void SaveJoyKey();
	void SaveMacroTextInfo();

	void SetCustomKey( CKeyInfo::KeyTypeArray eKeyTypeArray, WORD wKey );
	WORD GetCustomKey( CKeyInfo::KeyTypeArray eKeyTypeArray );
	WORD GetDefaultKey( CKeyInfo::KeyTypeArray eKeyTypeArray );

	void SetCustomJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray, WORD wKey );
	WORD GetCustomJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray );
	WORD GetDefaultJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray );

	void SetCustomMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray , const CKeyInfo::MecroTextInfo &rMacroTextInfo );
	CKeyInfo::MecroTextInfo *GetCustomMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray );
	CKeyInfo::MecroTextInfo *GetDefaultMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray );

public:
	void GetGameKey( OUT ioUserKeyInput::UseKeyValue &rkKeys );
	void GetSecondGameKey( OUT ioUserKeyInput::UseKeyValue &rkKeys );
	void GetJoyGameKey( OUT ioUserKeyInput::UseKeyValue &rkKeys );

	CKeyInfo::MecroTextInfo *GetMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray );

public:
	bool IsHelpKeyPress();
	bool IsShopKeyPress();
	bool IsGrowthKeyPress();
	bool IsQuestKeyPress();
	bool IsPresentKeyPress();
	bool IsMyHomePageKeyPress();
	bool IsOptionKeyPress();
	bool IsMessengerKeyPress();
	bool IsVoiceChatKeyPress();
	bool IsScreenShotKeyPress();
	bool IsMovieCaptureKeyPress();
	bool IsSpecialMotionKeyPress();
	bool IsPetKeyPress();
	bool IsCoolTimeResetPress();
	bool IsBattleModeEntryPress();				// [ 배틀모드 전용 ]
	bool IsBattleModeSoldierSelectPress();		// [ 배틀모드 전용 ]
	bool IsBattleModeTagSwitchPress();			// [ 배틀모드 전용 ]

	bool IsUsersCustomSoundKeyPress();
	bool IsZoomInKeyRepeat();
	bool IsZoomOutKeyRepeat();

	int  GetConsumptionKeyPress();

public:
	// return 0 ~ MAX_CHARACTER_KEY, error : -1
	int GetSoldierKeyPressArray();
	// return KTA_MACRO1 ~ KTA_MACRO36 error : KTA_NONE
	CKeyInfo::KeyTypeArray GetMacroKeyPress();

	void GetCurrent( OUT CKeyInfo &rKeyInfo );
	void GetDefault( OUT CKeyInfo &rKeyInfo );
	void GetCustom( OUT CKeyInfo &rKeyInfo );
	void GetDefaultTextInfo( OUT CKeyInfo &rKeyInfo );
	void GetCustomTextInfo( OUT CKeyInfo &rKeyInfo );

	void GetJoyCurrent( OUT CJoyKeyInfo &rKeyInfo );
	void GetJoyDefault( OUT CJoyKeyInfo &rKeyInfo );
	void GetJoyCustom( OUT CJoyKeyInfo &rKeyInfo );

	const char *GetKeyText(WORD wKey);

public:
	WORD GetCurrentSoldierKey( int iSoldierNumber );
	WORD GetCurrentConsumptionKey(int nSlot);
	WORD GetCurrentHelpKey();
	WORD GetCurrentShopKey();
	WORD GetCurrentGrowthKey();
	WORD GetCurrentQuestKey();
	WORD GetCurrentPresentKey();
	WORD GetCurrentMyHomePageKey();	
	WORD GetCurrentOptionKey();
	WORD GetCurrentMessengerKey();
	WORD GetCurrentScreenShotKey();
	WORD GetCurrentMovieCaptureKey();
	WORD GetCurrentPetKey();
	
public:
	bool SetCustomKey( IN CKeyInfo &rKeyInfo );
	bool SetCustomJoyKey( IN CJoyKeyInfo &rKeyInfo );

	bool SetCustomMacroTextInfo( IN CKeyInfo &rKeyInfo );

	void SetServerKeys( const ControlKeys &rkServerKeys);

public:
	bool IsAutoFix();
	int GetLastSoldierSelectKey();				// 마지막으로 선택한 용병 선택 슬롯 키 값 반환
	void SetLastSoldierSelectKey( int iLastSoldierKeyPressArray ) { m_iLastSoldierKeyPressArray = iLastSoldierKeyPressArray; }

public:
	static ioKeyManager &GetSingleton();

public:
	ioKeyManager(void);
	virtual ~ioKeyManager(void);
};

#define g_KeyManager ioKeyManager::GetSingleton()
