

#include "stdafx.h"

#include "ioUserKeyInput.h"

ioUserKeyInput::tagUseKeyValue::tagUseKeyValue()
{
	m_wLeftKey			= KEY_NULL;
	m_wRightKey			= KEY_NULL;
	m_wUpKey			= KEY_NULL;
	m_wDownKey			= KEY_NULL;
	m_wLeftUpKey        = KEY_NULL;
	m_wLeftDownKey      = KEY_NULL;
	m_wRightUpKey       = KEY_NULL;
	m_wRightDownKey     = KEY_NULL;
	m_wDefenseKey       = KEY_NULL;
	m_wJumpKey	        = KEY_NULL;
	m_wAttackKey        = KEY_NULL;
	m_wWeaponSkillKey   = KEY_NULL;
	m_wArmorSkillKey    = KEY_NULL;
	m_wHelmetSkillKey   = KEY_NULL;
	m_wCloakSkillKey    = KEY_NULL;
}

void ioUserKeyInput::tagUseKeyValue::SetValue( WORD wLeft, WORD wRight, WORD wUp, WORD wDown 
											  , WORD wLeftUp, WORD wLeftDown , WORD wRightUp, WORD wRightDown
											  , WORD wDefense, WORD wJump, WORD wAttack 
											  , WORD wWeaponSkill, WORD wArmorSkill, WORD wHelmetSkill, WORD wCloakSkill )
{
	m_wLeftKey			= wLeft;
	m_wRightKey			= wRight;
	m_wUpKey			= wUp;
	m_wDownKey			= wDown;
	m_wLeftUpKey        = wLeftUp;
	m_wLeftDownKey      = wLeftDown;
	m_wRightUpKey       = wRightUp;
	m_wRightDownKey     = wRightDown;
	m_wDefenseKey       = wDefense;
	m_wJumpKey	        = wJump;
	m_wAttackKey        = wAttack;
	m_wWeaponSkillKey   = wWeaponSkill;
	m_wArmorSkillKey    = wArmorSkill;
	m_wHelmetSkillKey   = wHelmetSkill;
	m_wCloakSkillKey    = wCloakSkill;
}

void ioUserKeyInput::tagUseKeyValue::SetDefaultFirstKey()
{
	SetValue(KEY_LEFTARROW, KEY_RIGHTARROW, KEY_UPARROW, KEY_DOWNARROW
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL
		, KEY_S, KEY_A, KEY_D
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL);
}

void ioUserKeyInput::tagUseKeyValue::SetDefaultSecondKey()
{
	SetValue(KEY_NUM4, KEY_NUM6, KEY_NUM8, KEY_NUM2
		, KEY_NUM7, KEY_NUM1, KEY_NUM9, KEY_NUM3
		, KEY_X, KEY_Z, KEY_C
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL);
}

void ioUserKeyInput::tagUseKeyValue::SetDefaultJoyKey()
{
	SetValue(KEY_LSTICKLEFT, KEY_LSTICKRIGHT, KEY_LSTICKUP, KEY_LSTICKDOWN
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL
		, KEY_JOYBUTN1, KEY_JOYBUTN3, KEY_JOYBUTN2
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL);
}

void ioUserKeyInput::tagUseKeyValue::Clear()
{
	SetValue(KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL
		, KEY_NULL, KEY_NULL, KEY_NULL
		, KEY_NULL, KEY_NULL, KEY_NULL, KEY_NULL);
}

bool ioUserKeyInput::tagUseKeyValue::IsEmpty()
{
	if( m_wLeftKey			== KEY_NULL &&
		m_wRightKey			== KEY_NULL &&
		m_wUpKey			== KEY_NULL &&
		m_wDownKey			== KEY_NULL &&
		m_wLeftUpKey        == KEY_NULL &&
		m_wLeftDownKey      == KEY_NULL &&
		m_wRightUpKey       == KEY_NULL &&
		m_wRightDownKey     == KEY_NULL &&
		m_wDefenseKey       == KEY_NULL &&
		m_wJumpKey	        == KEY_NULL &&
		m_wAttackKey        == KEY_NULL &&
		m_wWeaponSkillKey   == KEY_NULL &&
		m_wArmorSkillKey    == KEY_NULL &&
		m_wHelmetSkillKey   == KEY_NULL &&
		m_wCloakSkillKey    == KEY_NULL )
		return true;

	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioUserKeyInput::ioUserKeyInput()
: m_DirKeyQueue(3)
{
	m_CurDirKey = DKI_NONE;
	m_PreDirKey = DKI_NONE;
	m_FourDirKey = DKI_NONE;
	m_bNetworkkeySetted = false;

	m_Keys.SetDefaultFirstKey();
	m_SecondKeys.SetDefaultSecondKey();
	m_JoyKeys.SetDefaultJoyKey();
	m_AIKeys.Clear();

	m_KeptKeys.Clear();
	m_KeptSecondKeys.Clear();

	m_dwKeyCheckTime = 50;

	m_dwLeftPressTime = 0;
	m_dwRightPressTime = 0;
	m_dwUpPressTime = 0;
	m_dwDownPressTime = 0;
	m_dwLastDirKeyReleaseTime = 0;

	m_dwLeftUpPressTime    = 0;
	m_dwLeftDownPressTime  = 0;
	m_dwRightUpPressTime   = 0;
	m_dwRightDownPressTime = 0;

	m_CurActionKey = AKI_NONE;
	m_PreActionKey = AKI_NONE;
	m_CurMixKey    = DMI_NONE;

	m_dwDefensePressTime = 0;
	m_dwJumpPressTime = 0;
	m_dwAttackPressTime = 0;

	m_dwWeaponPressTime = 0;
	m_dwArmorPressTime = 0;
	m_dwHelmetPressTime = 0;
	m_dwCloakPressTime = 0;

	m_dwDashCheckDuration = 800;

	m_dwLastAttackKeyRepeatTime = 0;
	m_dwAttackKeyRepeatGap = 200;
	m_iAttackRepeatCnt = 0;

	m_bEnableKeyInput = true;
	m_bDirDoubleClick = false;

	m_bChangeKeyValue = false;

	m_bUpKey		= false;
	m_bRightUpKey	= false;
	m_bRightKey		= false;
	m_bRightDownKey	= false;
	m_bDownKey		= false;
	m_bLeftDownKey	= false;
	m_bLeftKey		= false;
	m_bLeftUpKey	= false;

	m_bInputProcessAI = false;

	m_dwMixStartTime = 0;
}

ioUserKeyInput::~ioUserKeyInput()
{
}

void ioUserKeyInput::SetDashCheckDuration( DWORD dwDuration )
{
	m_dwDashCheckDuration = dwDuration;
}

void ioUserKeyInput::SetUpKeyValue( const UseKeyValue &rkUseKeyValue )
{
	m_Keys	= rkUseKeyValue;
	m_KeptKeys.Clear();
}

void ioUserKeyInput::SetUpJoyKeyValue( const UseKeyValue &rkUseKeyValue )
{
	m_JoyKeys = rkUseKeyValue;
}

void ioUserKeyInput::SetUpSecondKeyValue( const UseKeyValue &rkUseKeyValue )
{
	m_SecondKeys = rkUseKeyValue;
	m_KeptSecondKeys.Clear();
}

void ioUserKeyInput::SetUpAIKeyValue( bool bAIProcess )
{
	m_bInputProcessAI = bAIProcess;
	m_AIKeys.Clear();

	m_Keys.Clear();
	m_SecondKeys.Clear();
	m_JoyKeys.Clear();
}

void ioUserKeyInput::UpdateKeyInputNetwork()
{
	if( m_bNetworkkeySetted )
	{
		m_bNetworkkeySetted = false;
	}
	else
	{
		m_PreDirKey = m_CurDirKey;
		m_PreActionKey = m_CurActionKey;
	}
}

void ioUserKeyInput::SetNetworkKeyInput( DirKeyInput eDir )
{
	m_PreDirKey = m_CurDirKey;
	m_CurDirKey = eDir;
	m_bNetworkkeySetted = true;
}

void ioUserKeyInput::SetNetworkActionKeyInput( ActionKeyInput eAction )
{
	m_PreActionKey = m_CurActionKey;
	m_CurActionKey = eAction;
	m_bNetworkkeySetted = true;
}

void ioUserKeyInput::SetAIDirKeyInput( WORD	wLeftKey, WORD wRightKey, WORD wUpKey, WORD wDownKey, 
									   WORD wLeftUpKey, WORD wLeftDownKey, WORD wRightUpKey, WORD wRightDownKey )
{
	m_AIKeys.m_wLeftKey		= wLeftKey;
	m_AIKeys.m_wRightKey	= wRightKey;
	m_AIKeys.m_wUpKey		= wUpKey;
	m_AIKeys.m_wDownKey		= wDownKey;
	m_AIKeys.m_wLeftUpKey	= wLeftUpKey;
	m_AIKeys.m_wLeftDownKey	= wLeftDownKey;
	m_AIKeys.m_wRightUpKey	= wRightUpKey;
	m_AIKeys.m_wRightDownKey= wRightDownKey;
}

void ioUserKeyInput::SetAIActionKeyInput( WORD wDefenseKey, WORD wJumpKey, WORD wAttackKey )
{
	m_AIKeys.m_wDefenseKey = wDefenseKey;
	m_AIKeys.m_wJumpKey    = wJumpKey;
	m_AIKeys.m_wAttackKey  = wAttackKey;
}

void ioUserKeyInput::EnableKeyInput( bool bEnable )
{
	m_bEnableKeyInput = bEnable;

	if( !m_bInputProcessAI && !bEnable )
	{
		for( int i=0 ; i<3 ; i++ )
		{
			m_DirKeyQueue[i].m_DirKey = DKI_NONE;
			m_DirKeyQueue[i].m_dwKeyTime = 0;
		}

		m_CurDirKey = DKI_NONE;
		m_PreDirKey = DKI_NONE;
		m_FourDirKey = DKI_NONE;

		m_CurActionKey = AKI_NONE;
		m_PreActionKey = AKI_NONE;
		m_CurMixKey    = DMI_NONE;
	}
}

void ioUserKeyInput::EnableKeyInputMust( bool bEnable )
{
	m_bEnableKeyInput = bEnable;
	
	if( bEnable )
	{
		for( int i=0 ; i < 3 ; i++ )
		{
			m_DirKeyQueue[i].m_DirKey = DKI_NONE;
			m_DirKeyQueue[i].m_dwKeyTime = 0;
		}

		m_CurDirKey = DKI_NONE;
		m_PreDirKey = DKI_NONE;
		m_FourDirKey = DKI_NONE;

		m_CurActionKey = AKI_NONE;
		m_PreActionKey = AKI_NONE;
		m_CurMixKey    = DMI_NONE;
		SetNetworkKeyInput( DKI_NONE );
	}
}

void ioUserKeyInput::UpdateKeyInput()
{
	if( !m_bInputProcessAI && !m_bEnableKeyInput )	return;

	CheckKeyPressTime();

	EvaluateDirectionMixKey();
	EvaluateDirectionKey();
	EvaluateActionKey();

	ChangeKeyValue();

	ClearReleaseKeyPressTime();
}

void ioUserKeyInput::EvaluateRealKey()
{
	m_bUpKey		= false;
	m_bRightUpKey	= false;
	m_bRightKey		= false;
	m_bRightDownKey	= false;
	m_bDownKey		= false;
	m_bLeftDownKey	= false;
	m_bLeftKey		= false;
	m_bLeftUpKey	= false;

	if( IsUpKeyDown() && !IsDownKeyDown() )
		m_bUpKey = true;
	if( IsDownKeyDown() && !IsUpKeyDown() )
		m_bDownKey = true;

	if( IsUpKeyDown() && IsDownKeyDown() )
	{
		if( m_dwUpPressTime >= m_dwDownPressTime )
			m_bUpKey = true;
		else if( m_dwUpPressTime < m_dwDownPressTime )
			m_bDownKey = true;
	}

	if( IsLeftKeyDown() && !IsRightKeyDown() )
		m_bLeftKey = true;
	if( IsRightKeyDown() && !IsLeftKeyDown() )
		m_bRightKey = true;

	if( IsLeftKeyDown() && IsRightKeyDown() )
	{
		if( m_dwLeftPressTime >= m_dwRightPressTime )
			m_bLeftKey = true;
		else if( m_dwLeftPressTime < m_dwRightPressTime )
			m_bRightKey = true;
	}

	if( IsRightUpKeyDown() && !IsLeftDownKeyDown() )
		m_bRightUpKey = true;
	if( IsRightDownKeyDown() && !IsLeftUpKeyDown() )
		m_bRightDownKey = true;

	if( IsRightUpKeyDown() && IsLeftDownKeyDown() )
	{
		if( m_dwRightUpPressTime >= m_dwLeftDownPressTime )
			m_bRightUpKey = true;
		else if( m_dwRightUpPressTime < m_dwLeftDownPressTime )
			m_bLeftDownKey = true;
	}

	if( IsLeftUpKeyDown() && !IsRightDownKeyDown() )
		m_bLeftUpKey = true;
	if( IsLeftDownKeyDown() && !IsRightUpKeyDown() )
		m_bLeftDownKey = true;

	if( IsLeftUpKeyDown() && IsRightDownKeyDown() )
	{
		if( m_dwLeftUpPressTime >= m_dwRightDownPressTime )
			m_bLeftUpKey = true;
		else if( m_dwLeftUpPressTime < m_dwRightDownPressTime )
			m_bRightDownKey = true;
	}
}

void ioUserKeyInput::EvaluateRealFourKey()
{
	m_bFourUpKey = false;
	m_bFourRightKey	= false;
	m_bFourDownKey = false;
	m_bFourLeftKey = false;


	DWORD dwCurTime = 0;
	int iCheckIndex = 0;

	if( IsUpKeyDown() )
	{
		iCheckIndex = 1;
		dwCurTime = m_dwUpPressTime;

		if( IsRightKeyDown() )
		{
			if( dwCurTime < m_dwRightPressTime )
			{
				iCheckIndex = 2;
				dwCurTime = m_dwRightPressTime;
			}
		}

		if( IsDownKeyDown() )
		{
			if( dwCurTime < m_dwDownPressTime )
			{
				iCheckIndex = 3;
				dwCurTime = m_dwDownPressTime;
			}
		}

		if( IsLeftKeyDown() )
		{
			if( dwCurTime < m_dwLeftPressTime )
			{
				iCheckIndex = 4;
			}
		}
	}
	else if( IsRightKeyDown() )
	{
		iCheckIndex = 2;
		dwCurTime = m_dwRightPressTime;

		if( IsDownKeyDown() )
		{
			if( dwCurTime < m_dwDownPressTime )
			{
				iCheckIndex = 3;
				dwCurTime = m_dwDownPressTime;
			}
		}

		if( IsLeftKeyDown() )
		{
			if( dwCurTime < m_dwLeftPressTime )
			{
				iCheckIndex = 4;
			}
		}
	}
	else if( IsDownKeyDown() )
	{
		iCheckIndex = 3;
		dwCurTime = m_dwDownPressTime;

		if( IsLeftKeyDown() )
		{
			if( dwCurTime < m_dwLeftPressTime )
			{
				iCheckIndex = 4;
			}
		}
	}
	else if( IsLeftKeyDown() )
	{
		iCheckIndex = 4;
	}

	switch( iCheckIndex )
	{
	case 1:
		m_bFourUpKey = true;
		break;
	case 2:
		m_bFourRightKey = true;
		break;
	case 3:
		m_bFourDownKey = true;
		break;
	case 4:
		m_bFourLeftKey = true;
		break;
	}
}

void ioUserKeyInput::EvaluateDirectionMixKey()
{
	m_CurMixKey = DMI_NONE;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMixStartTime + 50 < dwCurTime )
	{
		m_dwMixStartTime = dwCurTime;

		bool bUp = false;
		bool bLeft = false;
		bool bRight = false;
		bool bDown = false;

		DWORDVec::iterator iter = m_MixKeyVec.begin();
		for( ; iter != m_MixKeyVec.end(); ++iter )
		{
			switch( *iter )
			{
			case DKI_LEFT:
				bLeft = true;
				break;
			case DKI_RIGHT:
				bRight = true;
				break;
			case DKI_DOWN:
				bDown = true;
				break;
			case DKI_UP:
				bUp = true;
				break;
			}
		}

		if( bLeft && bDown && bRight )
		{
			m_CurMixKey = DMI_LEFT_UP_RIGHT;
		}
		m_MixKeyVec.clear();
	}
	else
	{
		if( IsLeftKeyRelease() )
		{
			m_MixKeyVec.push_back( DKI_LEFT );
		}

		if( IsRightKeyRelease() )
		{
			m_MixKeyVec.push_back( DKI_RIGHT );
		}

		if( IsDownKeyRelease() )
		{
			m_MixKeyVec.push_back( DKI_DOWN );
		}

		if( IsUpKeyRelease() )
		{
			m_MixKeyVec.push_back( DKI_UP );
		}		
	}

}

void ioUserKeyInput::EvaluateDirectionKey()
{
	DirKeyInput eCurKey = DKI_NONE;
	DirKeyInput eFourCurKey = DKI_NONE;

	DWORD dwFirstTime = GetDirFirstKeyDownTime();
	DWORD dwGap = FRAMEGETTIME() - dwFirstTime;

	EvaluateRealKey();
	EvaluateRealFourKey();

	if( dwGap > 10 )
	{
		if( m_bLeftKey )
		{
			if( m_bUpKey )
			{
				eCurKey = DKI_LEFTUP;
			}
			else if( m_bDownKey )
			{
				eCurKey = DKI_LEFTDOWN;
			}
			else
			{
				eCurKey = DKI_LEFT;
			}
		}
		else if( m_bRightKey )
		{
			if( m_bUpKey )
			{
				eCurKey = DKI_RIGHTUP;
			}
			else if( m_bDownKey )
			{
				eCurKey = DKI_RIGHTDOWN;
			}
			else
			{
				eCurKey = DKI_RIGHT;
			}
		}
		else if( m_bUpKey )
		{
			eCurKey = DKI_UP;
		}
		else if( m_bDownKey )
		{
			eCurKey = DKI_DOWN;
		}
		else if( m_bLeftUpKey )
		{
			eCurKey = DKI_LEFTUP;
		}
		else if( m_bLeftDownKey )
		{
			eCurKey = DKI_LEFTDOWN;
		}
		else if( m_bRightUpKey )
		{
			eCurKey = DKI_RIGHTUP;
		}
		else if( m_bRightDownKey )
		{
			eCurKey = DKI_RIGHTDOWN;
		}

		// fourkey Check
		if( m_bFourUpKey )
			eFourCurKey = DKI_UP;
		else if( m_bFourRightKey )
			eFourCurKey = DKI_RIGHT;
		else if( m_bFourDownKey )
			eFourCurKey = DKI_DOWN;
		else if( m_bFourLeftKey )
			eFourCurKey = DKI_LEFT;
	}
	else
	{
		if( IsUpKeyDown()       ||
			IsDownKeyDown()     ||
			IsLeftKeyDown()     ||
			IsRightKeyDown()    ||
			IsLeftUpKeyDown()   ||
			IsLeftDownKeyDown() ||
			IsRightUpKeyDown()  ||
			IsRightDownKeyDown() )
		{
			eCurKey = m_CurDirKey;
			eFourCurKey = m_FourDirKey;
		}
	}

	FixDiagonalDirRelease( eCurKey );

	m_FourDirKey = eFourCurKey;	 
}

void ioUserKeyInput::FixDiagonalDirRelease( DirKeyInput eNewDirKey )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( IsLeftKeyRelease() )
	{
		m_dwLastDirKeyReleaseTime = dwCurTime;
	}

	if( IsRightKeyRelease() )
	{
		m_dwLastDirKeyReleaseTime = dwCurTime;
	}

	if( IsUpKeyRelease() )
	{
		m_dwLastDirKeyReleaseTime = dwCurTime;
	}

	if( IsDownKeyRelease() )
	{
		m_dwLastDirKeyReleaseTime = dwCurTime;
	}

	if( IsDiagonalDir( m_CurDirKey ) && !IsDiagonalDir( eNewDirKey ) )
	{
		if( FRAMEGETTIME() - m_dwLastDirKeyReleaseTime < 100 )
		{
			m_PreDirKey = m_CurDirKey;
			// Not Update NewKey..
		}
		else
		{
			m_PreDirKey = m_CurDirKey;
			m_CurDirKey = eNewDirKey;
		}
	}
	else
	{
		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}
}

void ioUserKeyInput::ClearDirDoubleClick()
{
	for( int i=0 ; i<3 ; i++ )
	{
		m_DirKeyQueue[i].m_DirKey = DKI_NONE;
		m_DirKeyQueue[i].m_dwKeyTime = 0;
	}

	m_bDirDoubleClick = false;
}

void ioUserKeyInput::CheckDashInput()
{
	if( m_DirKeyQueue.back().m_DirKey == m_CurDirKey )
		return;

	DirKeyTime kKeyTime;
	kKeyTime.m_DirKey = m_CurDirKey;
	kKeyTime.m_dwKeyTime = FRAMEGETTIME();

	m_DirKeyQueue.pop_front();
	m_DirKeyQueue.push_back( kKeyTime );

	if( m_CurDirKey != DKI_NONE )
	{
		if( m_DirKeyQueue[0].m_DirKey == m_CurDirKey &&
			m_DirKeyQueue[1].m_DirKey == DKI_NONE )
		{
			DWORD dwGap = m_DirKeyQueue[2].m_dwKeyTime - m_DirKeyQueue[0].m_dwKeyTime;
			if( dwGap < m_dwDashCheckDuration )
			{
				m_bDirDoubleClick = true;
			}
		}
	}
	else
	{
		m_bDirDoubleClick = false;
	}
}

void ioUserKeyInput::CheckAttackKeyRepeat()
{
	if( IsAttackKeyPress() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( dwCurTime > m_dwLastAttackKeyRepeatTime + m_dwAttackKeyRepeatGap )
		{
			m_iAttackRepeatCnt = 1;
		}
		else
		{
			m_iAttackRepeatCnt++;
		}

		m_dwLastAttackKeyRepeatTime = dwCurTime;
	}
}

void ioUserKeyInput::ClearAttackKeyRepeat()
{
	m_iAttackRepeatCnt = 0;
	m_dwLastAttackKeyRepeatTime = 0;
}

int ioUserKeyInput::GetAttackKeyRepeatCnt() const
{
	return m_iAttackRepeatCnt;
}

bool ioUserKeyInput::IsDiagonalDir( DirKeyInput eDir ) const
{
	switch( eDir )
	{
	case DKI_RIGHTUP:
	case DKI_RIGHTDOWN:
	case DKI_LEFTDOWN:
	case DKI_LEFTUP:
		return true;
	}

	return false;
}

void ioUserKeyInput::EvaluateActionKey()
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_PreActionKey = m_CurActionKey;
	m_CurActionKey = AKI_NONE;

	DWORD dwFirstTime = GetActionFirstKeyDownTime();
	if( dwFirstTime == 0 )
	{
		m_CurActionKey = AKI_NONE;
		/*
		if( IsDefenseKeyDown() )
		{
			m_CurActionKey = AKI_DEFENSE;
		}
		else
		{
			m_CurActionKey = AKI_NONE;
		}
		*/
		return;
	}

	DWORD dwGap = dwCurTime - dwFirstTime;
	if( dwGap > m_dwKeyCheckTime || m_bInputProcessAI )
	{
		if( IsWeaponSkillKeyDown() )
		{
			m_CurActionKey = AKI_XC;
			m_dwDefensePressTime = m_dwAttackPressTime = m_dwJumpPressTime = 0;
			m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
		}
		else if( IsArmorSkillKeyDown() )
		{
			m_CurActionKey = AKI_ZXC;
			m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
			m_dwDefensePressTime = m_dwJumpPressTime = m_dwAttackPressTime = 0;
		}
		else if( IsHelmetSkillKeyDown() )
		{
			m_CurActionKey = AKI_ZC;
			m_dwDefensePressTime = m_dwAttackPressTime = m_dwJumpPressTime = 0;
			m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
		}
		else if( IsCloakSkillKeyDown() )
		{
			m_CurActionKey = AKI_ZX;
			m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
			m_dwDefensePressTime = m_dwAttackPressTime = m_dwJumpPressTime = 0;
		}
		else if( IsDefenseKeyDown() || m_dwDefensePressTime > 0 )
		{
			if( (IsJumpKeyDown() || m_dwJumpPressTime > 0) && (IsAttackKeyDown() || m_dwAttackPressTime > 0) )
			{
				m_CurActionKey = AKI_ZXC;
				m_dwDefensePressTime = m_dwJumpPressTime = m_dwAttackPressTime = 0;
				m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
			}
			else if( IsJumpKeyDown() || m_dwJumpPressTime > 0 )
			{
				m_CurActionKey = AKI_ZX;
				m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
				m_dwDefensePressTime = m_dwAttackPressTime = m_dwJumpPressTime = 0;
			}
			else if( IsAttackKeyDown() || m_dwAttackPressTime > 0 )
			{
				m_CurActionKey = AKI_XC;
				m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
				m_dwDefensePressTime = m_dwAttackPressTime = m_dwJumpPressTime = 0;
			}
		}
		else if( IsJumpKeyDown() || m_dwJumpPressTime > 0 )
		{
			if( IsAttackKeyDown() || m_dwAttackPressTime > 0 )
			{
				m_CurActionKey = AKI_ZC;
				m_dwDefensePressTime = m_dwAttackPressTime = m_dwJumpPressTime = 0;
				m_dwWeaponPressTime = m_dwArmorPressTime = m_dwHelmetPressTime = m_dwCloakPressTime = 0;
			}
		}

		if( m_CurActionKey == AKI_NONE )
		{
			if( !IsDefenseKeyNone() )
			{
				m_CurActionKey = AKI_DEFENSE;
				m_dwDefensePressTime = 0;
			}

			if( !IsJumpKeyNone() )
			{
				m_CurActionKey = AKI_JUMP;
				m_dwJumpPressTime = 0;
			}

			if( !IsAttackKeyNone() )
			{
				m_CurActionKey = AKI_ATTACK;
				m_dwAttackPressTime = 0;
			}
		}
	}
	else
	{
		if( IsJumpKeyRelease() )
		{
			if( (m_dwDefensePressTime == 0 && m_dwAttackPressTime == 0) )
			{
				m_CurActionKey = AKI_JUMP;
				m_dwJumpPressTime = 0;
			}
		}

		if( IsAttackKeyRelease() )
		{
			if( (m_dwDefensePressTime == 0 && m_dwJumpPressTime == 0) )
			{
				m_CurActionKey = AKI_ATTACK;
				m_dwAttackPressTime = 0;
			}
		}
	}
}

void ioUserKeyInput::SetSkillAction( ActionKeyInput eSkill )
{
	switch( eSkill )
	{
	case AKI_ZXC:
		m_dwDefensePressTime = m_dwJumpPressTime = m_dwAttackPressTime = 0;
		m_CurActionKey = AKI_ZXC;
		break;
	case AKI_ZX:
		m_dwDefensePressTime = m_dwJumpPressTime = 0;
		m_CurActionKey = AKI_ZX;
		break;
	case AKI_XC:
		m_dwDefensePressTime = m_dwAttackPressTime = 0;
		m_CurActionKey = AKI_XC;
		break;
	case AKI_ZC:
		m_dwJumpPressTime = m_dwAttackPressTime = 0;
		m_CurActionKey = AKI_ZC;
		break;
	}
}

ioUserKeyInput::DirKeyInput ioUserKeyInput::GetCurDirKey() const
{
	return m_CurDirKey;
}

void ioUserKeyInput::ClearDirKey()
{
	m_CurDirKey = DKI_NONE;
}

ioUserKeyInput::DirKeyInput ioUserKeyInput::GetCurFourDirKey() const
{
	return m_FourDirKey;
}

ioUserKeyInput::DirKeyInput ioUserKeyInput::GetPreDirKey() const
{
	return m_PreDirKey;
}

ioUserKeyInput::ActionKeyInput ioUserKeyInput::GetCurActionKey() const
{
	return m_CurActionKey;
}


ioUserKeyInput::DirMixInput ioUserKeyInput::GetCurDirMixKey() const
{
	return m_CurMixKey;
}

bool ioUserKeyInput::IsChangeDirection() const
{
	if( m_PreDirKey != m_CurDirKey )
		return true;

	return false;
}

bool ioUserKeyInput::IsSettedDirection() const
{
	if( m_CurDirKey != DKI_NONE )
		return true;

	return false;
}

bool ioUserKeyInput::IsDirDoubleClick() const
{
	return m_bDirDoubleClick;
}

bool ioUserKeyInput::IsDefenseKey() const
{
	if( m_CurActionKey == AKI_DEFENSE )
		return true;

	return false;
}

bool ioUserKeyInput::IsJumpKey() const
{
	if( m_CurActionKey == AKI_JUMP )
		return true;

	return false;
}

bool ioUserKeyInput::IsAttackKey() const
{
	if( m_CurActionKey == AKI_ATTACK )
		return true;

	return false;
}

bool ioUserKeyInput::IsHelmetSkillKey() const
{
	if( m_CurActionKey == AKI_ZC )
		return true;

	return false;
}

bool ioUserKeyInput::IsPickItemKey() const
{
	if( m_bInputProcessAI )
		return false;

	if( IsJumpKeyDown() && GetCurDirKey() == DKI_NONE )
		return true;

	return false;
}

bool ioUserKeyInput::IsSpecialMotionKey() const
{
	if( g_GUIMgr.IsUIKeyBlock() )
		return false;

	if( g_GUIMgr.IsShow( TUTORIAL_WND ) )
		return false;

	if( g_GUIMgr.IsShow( PRACTICE_HELP_WND ) )
		return false;

	if( ioWnd::m_pTopModalWnd )
		return false;

	if( m_bInputProcessAI )
		return false;

	if( g_KeyManager.IsSpecialMotionKeyPress() && GetCurDirKey() == DKI_NONE )
		return true;

	return false;
}

int ioUserKeyInput::GetSkillInput() const
{
	switch( m_CurActionKey )
	{
	case AKI_XC:
		return 0;
	case AKI_ZXC:
		return 1;
	case AKI_ZC:
		return 2;
	case AKI_ZX:
		return 3;
	}

	return -1;
}

void ioUserKeyInput::CheckKeyPressTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( IsLeftKeyPress() )
		m_dwLeftPressTime = dwCurTime;

	if( IsRightKeyPress() )
		m_dwRightPressTime = dwCurTime;

	if( IsUpKeyPress() )
		m_dwUpPressTime = dwCurTime;

	if( IsDownKeyPress() )
		m_dwDownPressTime = dwCurTime;

	if( IsLeftUpKeyPress() )
		m_dwLeftUpPressTime = dwCurTime;

	if( IsLeftDownKeyPress() )
		m_dwLeftDownPressTime = dwCurTime;

	if( IsRightUpKeyPress() )
		m_dwRightUpPressTime = dwCurTime;

	if( IsRightDownKeyPress() )
		m_dwRightDownPressTime = dwCurTime;

	if( IsDefenseKeyPress() )
		m_dwDefensePressTime = dwCurTime;

	if( IsJumpKeyPress() )
	{
		m_dwJumpPressTime = dwCurTime;
	}

	if( IsAttackKeyPress() )
		m_dwAttackPressTime = dwCurTime;

	if( IsWeaponSkillKeyPress() )
		m_dwWeaponPressTime = dwCurTime;

	if( IsArmorSkillKeyPress() )
		m_dwArmorPressTime = dwCurTime;

	if( IsHelmetSkillKeyPress() )
		m_dwHelmetPressTime = dwCurTime;

	if( IsCloakSkillKeyPress() )
		m_dwCloakPressTime = dwCurTime;
}

void ioUserKeyInput::ClearReleaseKeyPressTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwLeftPressTime > 0 && !IsLeftKeyDown() )
		m_dwLeftPressTime = 0;

	if( m_dwRightPressTime > 0 && !IsRightKeyDown() )
		m_dwRightPressTime = 0;

	if( m_dwUpPressTime > 0 && !IsUpKeyDown() )
		m_dwUpPressTime = 0;

	if( m_dwDownPressTime > 0 && !IsDownKeyDown() )
		m_dwDownPressTime = 0;

	if( m_dwLeftUpPressTime > 0 && !IsLeftUpKeyDown() )
		m_dwLeftUpPressTime  = 0;

	if( m_dwLeftDownPressTime > 0 && !IsLeftDownKeyDown() )
		m_dwLeftDownPressTime = 0;

	if( m_dwRightUpPressTime > 0 && !IsRightUpKeyDown() )
		m_dwRightUpPressTime = 0;

	if( m_dwRightDownPressTime > 0 && !IsRightDownKeyDown() )
		m_dwRightDownPressTime = 0;

	if( m_dwDefensePressTime > 0 && !IsDefenseKeyDown() )
	{
		if( (m_dwAttackPressTime == 0 && m_dwJumpPressTime == 0) )
			m_dwDefensePressTime = 0;
	}

	if( m_dwJumpPressTime > 0 && !IsJumpKeyDown() )
	{
		if( (m_dwAttackPressTime == 0 && m_dwDefensePressTime == 0) )
		{
			m_dwJumpPressTime = 0;
		}
	}

	if( m_dwAttackPressTime > 0 && !IsAttackKeyDown() )
	{
		if( (m_dwDefensePressTime == 0 && m_dwJumpPressTime == 0) )
			m_dwAttackPressTime = 0;
	}

	if( m_dwWeaponPressTime > 0 && !IsWeaponSkillKeyDown() )
		m_dwWeaponPressTime = 0;

	if( m_dwArmorPressTime > 0 && !IsArmorSkillKeyDown() )
		m_dwArmorPressTime = 0;

	if( m_dwHelmetPressTime > 0 && !IsHelmetSkillKeyDown() )
		m_dwHelmetPressTime = 0;

	if( m_dwCloakPressTime > 0 && !IsCloakSkillKeyDown() )
		m_dwCloakPressTime = 0;
}

void ioUserKeyInput::ResetKeyPressTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwLeftPressTime != 0 )
		m_dwLeftPressTime = dwCurTime;

	if( m_dwRightPressTime != 0 )
		m_dwRightPressTime = dwCurTime;

	if( m_dwUpPressTime != 0 )
		m_dwUpPressTime = dwCurTime;

	if( m_dwDownPressTime != 0 )
		m_dwDownPressTime = dwCurTime;

	if( m_dwLeftUpPressTime != 0 )
		m_dwLeftUpPressTime = dwCurTime;

	if( m_dwLeftDownPressTime != 0 )
		m_dwLeftDownPressTime = dwCurTime;

	if( m_dwRightUpPressTime != 0 )
		m_dwRightUpPressTime = dwCurTime;

	if( m_dwRightDownPressTime != 0 )
		m_dwRightDownPressTime = dwCurTime;

	if( m_dwDefensePressTime != 0 )
		m_dwDefensePressTime = dwCurTime;

	if( m_dwJumpPressTime != 0 )
		m_dwJumpPressTime = dwCurTime;

	if( m_dwAttackPressTime != 0 )
		m_dwAttackPressTime = dwCurTime;

	if( m_dwWeaponPressTime != 0 )
		m_dwWeaponPressTime = dwCurTime;

	if( m_dwArmorPressTime != 0 )
		m_dwArmorPressTime = dwCurTime;

	if( m_dwHelmetPressTime != 0 )
		m_dwHelmetPressTime = dwCurTime;

	if( m_dwCloakPressTime != 0 )
		m_dwCloakPressTime = dwCurTime;
}

DWORD ioUserKeyInput::GetDirFirstKeyDownTime()
{
	int iKeyTimeCnt = 0;
	DWORD dwKeyTimeList[8];

	if( m_dwLeftPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwLeftPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwRightPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwRightPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwUpPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwUpPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwDownPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwDownPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwLeftUpPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwLeftUpPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwLeftDownPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwLeftDownPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwRightUpPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwRightUpPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwRightDownPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwRightDownPressTime;
		iKeyTimeCnt++;
	}

	DWORD dwFirstDownKeyTime = 0;
	if( iKeyTimeCnt > 0 )
	{
		dwFirstDownKeyTime = dwKeyTimeList[0];
		for( int i=1 ; i<iKeyTimeCnt ; i++ )
		{
			dwFirstDownKeyTime = min( dwFirstDownKeyTime, dwKeyTimeList[i] );
		}
	}

	return dwFirstDownKeyTime;
}

DWORD ioUserKeyInput::GetActionFirstKeyDownTime()
{
	int iKeyTimeCnt = 0;
	DWORD dwKeyTimeList[3];

	if( m_dwDefensePressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwDefensePressTime;
		iKeyTimeCnt++;
	}

	if( m_dwJumpPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwJumpPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwAttackPressTime > 0 )
	{
		dwKeyTimeList[iKeyTimeCnt] = m_dwAttackPressTime;
		iKeyTimeCnt++;
	}

	if( m_dwWeaponPressTime > 0 )
	{
		dwKeyTimeList[0] = m_dwWeaponPressTime;
		iKeyTimeCnt = 1;
	}

	if( m_dwArmorPressTime > 0 )
	{
		dwKeyTimeList[0] = m_dwArmorPressTime;
		iKeyTimeCnt = 1;
	}

	if( m_dwHelmetPressTime > 0 )
	{
		dwKeyTimeList[0] = m_dwHelmetPressTime;
		iKeyTimeCnt = 1;
	}

	if( m_dwCloakPressTime > 0 )
	{
		dwKeyTimeList[0] = m_dwCloakPressTime;
		iKeyTimeCnt = 1;
	}

	DWORD dwFirstDownKeyTime = 0;
	if( iKeyTimeCnt > 0 )
	{
		dwFirstDownKeyTime = dwKeyTimeList[0];
		for( int i=1 ; i<iKeyTimeCnt ; i++ )
		{
			dwFirstDownKeyTime = min( dwFirstDownKeyTime, dwKeyTimeList[i] );
		}
	}

	return dwFirstDownKeyTime;
}

ioUserKeyInput::DirKeyInput ioUserKeyInput::GetXformDir( DirKeyInput eNewUp, DirKeyInput eXformDir )
{
	if( eNewUp == eXformDir )
		return DKI_UP;

	DirKeyInput eDiffDir = (DirKeyInput)(eNewUp - DKI_UP);
	if( eNewUp > eXformDir )
		return (DirKeyInput)(DKI_LEFTUP + eXformDir - eDiffDir);

	return (DirKeyInput)(eXformDir - eDiffDir); // eNewUp > eXformDir
}

bool ioUserKeyInput::IsOppositDir( DirKeyInput eCurDir, DirKeyInput eCheckDir )
{
	int iSumValue = abs( (int)eCurDir - (int)eCheckDir );

	if( iSumValue == 4 )
		return true;

	return false;
}

bool ioUserKeyInput::IsKeyDownException() const
{
	if( g_App.IsWebShow() )
	{
		if( ioWnd::m_pTopModalWnd == NULL )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}		
		return false;
	}
	return true;
}

bool ioUserKeyInput::IsLeftKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wLeftKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wLeftKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wLeftKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsLeftKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftKey == KEY_REPEAT || m_AIKeys.m_wLeftKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wLeftKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wLeftKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wLeftKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsLeftKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wLeftKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wLeftKey) == KEY_RELEASE || 
			g_Input.GetKeyState( m_JoyKeys.m_wLeftKey ) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsRightKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wRightKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wRightKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wRightKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsRightKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightKey == KEY_REPEAT || m_AIKeys.m_wRightKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wRightKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wRightKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wRightKey) )
			return IsKeyDownException();
	}	
	return false;
}

bool ioUserKeyInput::IsRightKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wRightKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wRightKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wRightKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsUpKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wUpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wUpKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wUpKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wUpKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsUpKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wUpKey == KEY_REPEAT || m_AIKeys.m_wUpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wUpKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wUpKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wUpKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsUpKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wUpKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wUpKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wUpKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wUpKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsDownKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDownKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wDownKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wDownKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wDownKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsDownKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDownKey == KEY_REPEAT || m_AIKeys.m_wDownKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wDownKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wDownKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wDownKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsDownKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDownKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wDownKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wDownKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wDownKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsLeftUpKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftUpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wLeftUpKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wLeftUpKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wLeftUpKey) )
		{
			return IsKeyDownException();
		}
	}

	return false;
}

bool ioUserKeyInput::IsLeftUpKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftUpKey == KEY_REPEAT || m_AIKeys.m_wLeftUpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wLeftUpKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wLeftUpKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wLeftUpKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsLeftUpKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftUpKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wLeftUpKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wLeftUpKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wLeftUpKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsLeftDownKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftDownKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wLeftDownKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wLeftDownKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wLeftDownKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsLeftDownKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftDownKey == KEY_REPEAT || m_AIKeys.m_wLeftDownKey == KEY_PRESS )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wLeftDownKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wLeftDownKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wLeftDownKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsLeftDownKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wLeftDownKey == KEY_RELEASE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wLeftDownKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wLeftDownKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wLeftDownKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsRightUpKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightUpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wRightUpKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wRightUpKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wRightUpKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsRightUpKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightUpKey == KEY_REPEAT || m_AIKeys.m_wRightUpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wRightUpKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wRightUpKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wRightUpKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsRightUpKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightUpKey == KEY_RELEASE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wRightUpKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wRightUpKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wRightUpKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsRightDownKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightDownKey == KEY_PRESS )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wRightDownKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wRightDownKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wRightDownKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsRightDownKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightDownKey == KEY_REPEAT || m_AIKeys.m_wRightDownKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wRightDownKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wRightDownKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wRightDownKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsRightDownKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wRightDownKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wRightDownKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wRightDownKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wRightDownKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsDefenseKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDefenseKey == KEY_PRESS )
			return true;
	}
	else	
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wDefenseKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wDefenseKey ) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wDefenseKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsDefenseKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDefenseKey == KEY_REPEAT || m_AIKeys.m_wDefenseKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wDefenseKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wDefenseKey ) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wDefenseKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsDefenseKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDefenseKey == KEY_RELEASE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wDefenseKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wDefenseKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wDefenseKey ) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsDefenseKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wDefenseKey == KEY_NONE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wDefenseKey ) == KEY_NONE &&
			g_Input.GetKeyState( m_SecondKeys.m_wDefenseKey ) == KEY_NONE &&
			g_Input.GetKeyState( m_JoyKeys.m_wDefenseKey ) == KEY_NONE )
			return true;
	}
	return false;
}


bool ioUserKeyInput::IsJumpKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wJumpKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wJumpKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wJumpKey ) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wJumpKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsJumpKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wJumpKey == KEY_REPEAT || m_AIKeys.m_wJumpKey == KEY_PRESS )
			return true;
	}
	else	
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wJumpKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wJumpKey ) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wJumpKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsJumpKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wJumpKey == KEY_RELEASE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wJumpKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wJumpKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wJumpKey ) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsJumpKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wJumpKey == KEY_NONE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wJumpKey ) == KEY_NONE &&
			g_Input.GetKeyState( m_SecondKeys.m_wJumpKey ) == KEY_NONE &&
			g_Input.GetKeyState( m_JoyKeys.m_wJumpKey ) == KEY_NONE )
			return true;
	}

	return false;
}

bool ioUserKeyInput::IsAttackKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wAttackKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wAttackKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wAttackKey ) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wAttackKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsAttackKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wAttackKey == KEY_REPEAT || m_AIKeys.m_wAttackKey == KEY_PRESS )
			return true;
	}
	else	
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wAttackKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wAttackKey ) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wAttackKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsAttackKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wAttackKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wAttackKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wAttackKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wAttackKey ) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsAttackKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wAttackKey == KEY_NONE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wAttackKey ) == KEY_NONE &&
			g_Input.GetKeyState( m_SecondKeys.m_wAttackKey ) == KEY_NONE &&
			g_Input.GetKeyState( m_JoyKeys.m_wAttackKey ) == KEY_NONE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsSpaceKeyPress() const
{
	if( !m_bInputProcessAI )
	{
		if( !m_bEnableKeyInput ) return false;

		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( m_Keys.m_wAttackKey != KEY_NULL && g_Input.IsKeyPress( KEY_SPACE ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsWeaponSkillKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wWeaponSkillKey == KEY_PRESS )
			return true;
	}	
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wWeaponSkillKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wWeaponSkillKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wWeaponSkillKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsWeaponSkillKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wWeaponSkillKey == KEY_REPEAT || m_AIKeys.m_wWeaponSkillKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wWeaponSkillKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wWeaponSkillKey ) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wWeaponSkillKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsWeaponSkillKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wWeaponSkillKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wWeaponSkillKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wWeaponSkillKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wWeaponSkillKey ) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsWeaponSkillKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wWeaponSkillKey == KEY_NONE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wWeaponSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_SecondKeys.m_wWeaponSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_JoyKeys.m_wWeaponSkillKey ) == KEY_NONE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsArmorSkillKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wArmorSkillKey == KEY_PRESS )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wArmorSkillKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wArmorSkillKey ) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wArmorSkillKey ) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsArmorSkillKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wArmorSkillKey == KEY_REPEAT || m_AIKeys.m_wArmorSkillKey == KEY_PRESS )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wArmorSkillKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wArmorSkillKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wArmorSkillKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsArmorSkillKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wArmorSkillKey == KEY_RELEASE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wArmorSkillKey ) == KEY_RELEASE || 
			g_Input.GetKeyState( m_SecondKeys.m_wArmorSkillKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wArmorSkillKey ) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsArmorSkillKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wArmorSkillKey == KEY_NONE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wArmorSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_SecondKeys.m_wArmorSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_JoyKeys.m_wArmorSkillKey ) == KEY_NONE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsHelmetSkillKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wHelmetSkillKey == KEY_PRESS )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wHelmetSkillKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wHelmetSkillKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wHelmetSkillKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsHelmetSkillKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wHelmetSkillKey == KEY_REPEAT || m_AIKeys.m_wHelmetSkillKey == KEY_REPEAT )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wHelmetSkillKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wHelmetSkillKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wHelmetSkillKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsHelmetSkillKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wHelmetSkillKey == KEY_RELEASE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wHelmetSkillKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wHelmetSkillKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wHelmetSkillKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsHelmetSkillKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wHelmetSkillKey == KEY_NONE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wHelmetSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_SecondKeys.m_wHelmetSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_JoyKeys.m_wHelmetSkillKey ) == KEY_NONE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsCloakSkillKeyPress() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wCloakSkillKey == KEY_PRESS )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyPress( m_Keys.m_wCloakSkillKey ) ||
			g_Input.IsKeyPress( m_SecondKeys.m_wCloakSkillKey) ||
			g_Input.IsKeyPress( m_JoyKeys.m_wCloakSkillKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsCloakSkillKeyDown() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wCloakSkillKey == KEY_REPEAT || m_AIKeys.m_wCloakSkillKey == KEY_PRESS )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.IsKeyDown( m_Keys.m_wCloakSkillKey ) ||
			g_Input.IsKeyDown( m_SecondKeys.m_wCloakSkillKey) ||
			g_Input.IsKeyDown( m_JoyKeys.m_wCloakSkillKey) )
			return IsKeyDownException();
	}
	return false;
}

bool ioUserKeyInput::IsCloakSkillKeyRelease() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wCloakSkillKey == KEY_RELEASE )
			return true;	
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wCloakSkillKey ) == KEY_RELEASE ||
			g_Input.GetKeyState( m_SecondKeys.m_wCloakSkillKey) == KEY_RELEASE ||
			g_Input.GetKeyState( m_JoyKeys.m_wCloakSkillKey) == KEY_RELEASE )
			return true;
	}
	return false;
}

bool ioUserKeyInput::IsCloakSkillKeyNone() const
{
	if( m_bInputProcessAI )
	{
		if( m_AIKeys.m_wCloakSkillKey == KEY_NONE )
			return true;
	}
	else
	{
		if( !CDirectInput::GetSingletonPtr() )
			return false;

		if( g_Input.GetKeyState( m_Keys.m_wCloakSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_SecondKeys.m_wCloakSkillKey ) == KEY_NONE ||
			g_Input.GetKeyState( m_JoyKeys.m_wCloakSkillKey ) == KEY_NONE )
			return true;
	}
	return false;
}

const char* ioUserKeyInput::GetDirKeyText() const
{
	switch( m_CurDirKey )
	{
	case DKI_NONE:
		return "DKI_NONE";
	case DKI_UP:
		return "DKI_UP";
	case DKI_RIGHTUP:
		return "DKI_RIGHTUP";
	case DKI_RIGHT:
		return "DKI_RIGHT";
	case DKI_RIGHTDOWN:
		return "DKI_RIGHTDOWN";
	case DKI_DOWN:
		return "DKI_DOWN";
	case DKI_LEFTDOWN:
		return "DKI_LEFTDOWN";
	case DKI_LEFT:
		return "DKI_LEFT";
	case DKI_LEFTUP:
		return "DKI_LEFTUP";
	}

	return "unknown dir";
}

const char* ioUserKeyInput::GetActionKeyText() const
{
	switch( m_CurActionKey )
	{
	case AKI_NONE:
		return "AKI_NONE";
	case AKI_DEFENSE:
		return "AKI_DEFENSE";
	case AKI_JUMP:
		return "AKI_JUMP";
	case AKI_ATTACK:
		return "AKI_ATTACK";
	case AKI_ZC:
		return "AKI_ZC";
	case AKI_ZXC:
		return "AKI_ZXC";
	case AKI_ZX:
		return "AKI_ZX";
	case AKI_XC:
		return "AKI_XC";
	}

	return "unknown action";
}

void ioUserKeyInput::ChangeKeyValue()
{
	if( g_GUIMgr.IsUIKeyBlock() )
	{
		if(!m_bChangeKeyValue)
		{
			m_bChangeKeyValue = true;

			if( m_KeptKeys.IsEmpty() )
			{
				m_KeptKeys       = m_Keys;
				m_Keys.SetDefaultFirstKey();
				m_Keys.m_wDefenseKey = KEY_NULL;
				m_Keys.m_wJumpKey    = KEY_NULL;
				m_Keys.m_wAttackKey  = KEY_NULL;
			}

			if( m_KeptSecondKeys.IsEmpty() )
			{
				m_KeptSecondKeys = m_SecondKeys;
				m_SecondKeys.Clear();
			}
		}
	}
	else
	{
		if( m_bChangeKeyValue )
		{
			m_bChangeKeyValue = false;

			if(!m_KeptKeys.IsEmpty())
			{
				m_Keys = m_KeptKeys;
				m_KeptKeys.Clear();
			}

			if(!m_KeptSecondKeys.IsEmpty())
			{
				m_SecondKeys = m_KeptSecondKeys;
				m_KeptSecondKeys.Clear();
			}
		}
	}
}

bool ioUserKeyInput::IsDirKeyRelease( DirKeyInput eCurDir )
{
	if( !CDirectInput::GetSingletonPtr() )
		return false;

	bool bRelease = false;

	switch( eCurDir )
	{
	case DKI_UP:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wUpKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wUpKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wUpKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wUpKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_DOWN:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wDownKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wDownKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wDownKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wDownKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_LEFT:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wLeftKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wLeftKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wLeftKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wLeftKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_RIGHT:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wRightKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wRightKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wRightKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wRightKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_RIGHTUP:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wRightUpKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wRightUpKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wRightUpKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wRightUpKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_RIGHTDOWN:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wRightDownKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wRightDownKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wRightDownKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wRightDownKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_LEFTDOWN:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wLeftDownKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wLeftDownKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wLeftDownKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wLeftDownKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	case DKI_LEFTUP:
		if( m_bInputProcessAI )
		{
			if( m_AIKeys.m_wLeftUpKey == KEY_RELEASE )
				bRelease = true;
		}
		else
		{
			if( g_Input.GetKeyState( m_Keys.m_wLeftUpKey ) == KEY_RELEASE ||
				g_Input.GetKeyState( m_SecondKeys.m_wLeftUpKey) == KEY_RELEASE ||
				g_Input.GetKeyState( m_JoyKeys.m_wLeftUpKey) == KEY_RELEASE )
				bRelease = true;
		}
		break;
	}

	return bRelease;
}
