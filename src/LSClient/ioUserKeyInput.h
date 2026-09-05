#pragma once

class ioUserKeyInput
{
public:
	enum DirKeyInput
	{
		DKI_NONE,

		DKI_UP,
		DKI_RIGHTUP,
		DKI_RIGHT,
		DKI_RIGHTDOWN,
		DKI_DOWN,
		DKI_LEFTDOWN,
		DKI_LEFT,
		DKI_LEFTUP
	};

	enum ActionKeyInput
	{
		AKI_NONE,
		AKI_DEFENSE,
		AKI_JUMP,
		AKI_ATTACK,
		AKI_XC,
		AKI_ZXC,
		AKI_ZC,
		AKI_ZX
	};

	enum DirMixInput
	{
		DMI_NONE,
		DMI_LEFT_UP_RIGHT,
	};

	typedef struct tagUseKeyValue
	{
	    // dir
		WORD	m_wLeftKey;
		WORD	m_wRightKey;
		WORD	m_wUpKey;
		WORD	m_wDownKey;
		// diagonal
		WORD    m_wLeftUpKey;
		WORD    m_wLeftDownKey;
		WORD    m_wRightUpKey;
		WORD    m_wRightDownKey;
		// action
		WORD	m_wDefenseKey;
		WORD	m_wJumpKey;
		WORD	m_wAttackKey;
        // skill
		WORD	m_wWeaponSkillKey;
		WORD	m_wArmorSkillKey;
		WORD	m_wHelmetSkillKey;
		WORD	m_wCloakSkillKey;

		tagUseKeyValue();
		void SetValue(WORD wLeft, WORD wRight, WORD wUp, WORD wDown
			        , WORD wLeftUp, WORD wLeftDown , WORD wRightUp, WORD wRightDown
					, WORD wDefense, WORD wJump, WORD wAttack
					, WORD wWeaponSkill, WORD wArmorSkill, WORD wHelmetSkill, WORD wCloakSkill );

		void SetDefaultFirstKey();
		void SetDefaultSecondKey();
		void SetDefaultJoyKey();

		void Clear();
		bool IsEmpty();

	}UseKeyValue;

private:
	DirKeyInput m_CurDirKey;
	DirKeyInput m_PreDirKey;
	DirKeyInput m_FourDirKey;
	bool	m_bNetworkkeySetted;

	UseKeyValue m_Keys;
	UseKeyValue m_SecondKeys;
	UseKeyValue m_JoyKeys;
	UseKeyValue m_AIKeys;            //AI 키들은 키값이 들어가는게 아니라 KEY_NONE / KEY_RELEASE / KEY_PRESS / KEY_REPEAT의 값들이 들어간다.

	UseKeyValue m_KeptKeys;
	UseKeyValue m_KeptSecondKeys;

	DWORD	m_dwLeftPressTime;
	DWORD	m_dwRightPressTime;
	DWORD	m_dwUpPressTime;
	DWORD	m_dwDownPressTime;
	DWORD	m_dwLastDirKeyReleaseTime;

	DWORD	m_dwLeftUpPressTime;
	DWORD	m_dwLeftDownPressTime;
	DWORD	m_dwRightUpPressTime;
	DWORD	m_dwRightDownPressTime;

	typedef struct tagDirKeyTime
	{
		DirKeyInput m_DirKey;
		DWORD m_dwKeyTime;
		tagDirKeyTime()
		{
			m_DirKey = DKI_NONE;
			m_dwKeyTime = 0;
		}
	}DirKeyTime;

	typedef std::deque< DirKeyTime > DirKeyQueue;
	DirKeyQueue m_DirKeyQueue;

	ActionKeyInput m_CurActionKey;
	ActionKeyInput m_PreActionKey;
	DirMixInput m_CurMixKey;
	
	DWORD	m_dwKeyCheckTime;

	DWORD	m_dwDefensePressTime;
	DWORD	m_dwJumpPressTime;
	DWORD	m_dwAttackPressTime;

	DWORD	m_dwWeaponPressTime;
	DWORD	m_dwArmorPressTime;
	DWORD	m_dwHelmetPressTime;
	DWORD	m_dwCloakPressTime;
	
	DWORD	m_dwDashCheckDuration;

	DWORD	m_dwLastAttackKeyRepeatTime;
	DWORD	m_dwAttackKeyRepeatGap;
	int		m_iAttackRepeatCnt;

	bool	m_bEnableKeyInput;
	bool	m_bDirDoubleClick;

	bool    m_bChangeKeyValue;

	bool m_bUpKey;
	bool m_bRightUpKey;
	bool m_bRightKey;
	bool m_bRightDownKey;
	bool m_bDownKey;
	bool m_bLeftDownKey;
	bool m_bLeftKey;
	bool m_bLeftUpKey;

	bool m_bFourUpKey;
	bool m_bFourRightKey;
	bool m_bFourDownKey;
	bool m_bFourLeftKey;

	bool m_bInputProcessAI;

	DWORD m_dwMixStartTime;
	DWORDVec m_MixKeyVec;

public:
	void SetDashCheckDuration( DWORD dwDuration );
	void SetUpKeyValue( const UseKeyValue &rkUseKeyValue );
	void SetUpSecondKeyValue( const UseKeyValue &rkUseKeyValue );
	void SetUpJoyKeyValue( const UseKeyValue &rkUseKeyValue );
	void SetUpAIKeyValue( bool bAIProcess );

	void UpdateKeyInput();
	void UpdateKeyInputNetwork();

	void SetNetworkKeyInput( DirKeyInput eDir );
	void SetNetworkActionKeyInput( ActionKeyInput eAction );

	void SetAIDirKeyInput( WORD	wLeftKey, WORD wRightKey, WORD wUpKey, WORD wDownKey, 
		                   WORD wLeftUpKey, WORD wLeftDownKey, WORD wRightUpKey, WORD wRightDownKey );
	void SetAIActionKeyInput( WORD wDefenseKey, WORD wJumpKey, WORD wAttackKey );

public:
	void EnableKeyInput( bool bEnable );
	void EnableKeyInputMust( bool bEnable );
	void CheckDashInput();

private:
	void EvaluateRealKey();
	void EvaluateRealFourKey();
	void EvaluateDirectionMixKey();
	void EvaluateDirectionKey();
	void EvaluateActionKey();

	DirKeyInput CheckDirectionKey();
	void FixDiagonalDirRelease( DirKeyInput eNewDirKey );
	bool IsDiagonalDir( DirKeyInput eDir ) const;

	void CheckKeyPressTime();
	void ClearReleaseKeyPressTime();

	DWORD GetDirFirstKeyDownTime();
	DWORD GetActionFirstKeyDownTime();

	void ChangeKeyValue();
	
	bool IsKeyDownException() const; 

public:
	DirKeyInput GetCurDirKey() const;
	void ClearDirKey();
	DirKeyInput GetCurFourDirKey() const;
	DirKeyInput GetPreDirKey() const;
	ActionKeyInput GetCurActionKey() const;
	DirMixInput GetCurDirMixKey() const;

	DirKeyInput GetXformDir( DirKeyInput eNewUp, DirKeyInput eXformDir );

	bool IsChangeDirection() const;
	bool IsSettedDirection() const;
	bool IsDirDoubleClick() const;

	bool IsDefenseKey() const;
	bool IsJumpKey() const;
	bool IsAttackKey() const;

	bool IsSpecialMotionKey() const;
	bool IsPickItemKey() const;

	int  GetSkillInput() const;

	bool IsOppositDir( DirKeyInput eCurDir, DirKeyInput eCheckDir );

public:
	void CheckAttackKeyRepeat();
	void ClearAttackKeyRepeat();
	int  GetAttackKeyRepeatCnt() const;

public:
	void ClearDirDoubleClick();

	bool IsLeftKeyPress() const;
	bool IsLeftKeyDown() const;
	bool IsLeftKeyRelease() const;

	bool IsRightKeyPress() const;
	bool IsRightKeyDown() const;
	bool IsRightKeyRelease() const;
	
	bool IsUpKeyPress() const;
	bool IsUpKeyDown() const;
	bool IsUpKeyRelease() const;
	
	bool IsDownKeyPress() const;
	bool IsDownKeyDown() const;
	bool IsDownKeyRelease() const;
	
	bool IsLeftUpKeyPress() const;
	bool IsLeftUpKeyDown() const;
	bool IsLeftUpKeyRelease() const;
	
	bool IsLeftDownKeyPress() const;
	bool IsLeftDownKeyDown() const;
	bool IsLeftDownKeyRelease() const;
	
	bool IsRightUpKeyPress() const;
	bool IsRightUpKeyDown() const;
	bool IsRightUpKeyRelease() const;
	
	bool IsRightDownKeyPress() const;
	bool IsRightDownKeyDown() const;
	bool IsRightDownKeyRelease() const;

	bool IsDefenseKeyPress() const;
	bool IsDefenseKeyDown() const;
	bool IsDefenseKeyRelease() const;
	bool IsDefenseKeyNone() const;

	bool IsJumpKeyPress() const;
	bool IsJumpKeyDown() const;
	bool IsJumpKeyRelease() const;
	bool IsJumpKeyNone() const;

	bool IsAttackKeyPress() const;
	bool IsAttackKeyDown() const;
	bool IsAttackKeyRelease() const;
	bool IsAttackKeyNone() const;

	bool IsSpaceKeyPress() const;

	bool IsWeaponSkillKeyDown() const;
	bool IsWeaponSkillKeyPress() const;
	bool IsWeaponSkillKeyRelease() const;
	bool IsWeaponSkillKeyNone() const;

	bool IsArmorSkillKeyDown() const;
	bool IsArmorSkillKeyPress() const;
	bool IsArmorSkillKeyRelease() const;
	bool IsArmorSkillKeyNone() const;

	bool IsHelmetSkillKey() const;
	bool IsHelmetSkillKeyDown() const;
	bool IsHelmetSkillKeyPress() const;
	bool IsHelmetSkillKeyRelease() const;
	bool IsHelmetSkillKeyNone() const;

	bool IsCloakSkillKeyDown() const;
	bool IsCloakSkillKeyPress() const;
	bool IsCloakSkillKeyRelease() const;
	bool IsCloakSkillKeyNone() const;

	bool IsDirKeyRelease( DirKeyInput eCurDir );

	void SetSkillAction( ActionKeyInput eSkill );

	void ResetKeyPressTime();

public:
	const char* GetDirKeyText() const;
	const char* GetActionKeyText() const;

public:
	ioUserKeyInput();
	~ioUserKeyInput();
};

