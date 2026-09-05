
#include "stdafx.h"

#include "ioKeyManager.h"

#include "SP2Packet.h"

CKeyInfo::CKeyInfo() : m_bUse( true ), m_bAutoFix( false )
{
	m_vKeyInfo.reserve(30);
	m_vMecroTextInfo.reserve(30);
}

CKeyInfo::~CKeyInfo()
{
	m_vKeyInfo.clear();
	m_vMecroTextInfo.clear();
}

WORD CKeyInfo::GetKey( KeyTypeArray eKeyTypeArray )
{
	if( !COMPARE(eKeyTypeArray, 0, (int)m_vKeyInfo.size() ) )
		return KEY_NULL;

	return m_vKeyInfo[eKeyTypeArray];
}

bool CKeyInfo::SetKey( KeyTypeArray eKeyTypeArray, WORD wKey )
{
	if( !COMPARE(eKeyTypeArray, 0, (int)m_vKeyInfo.size()) )
		return false;

	if( !IsRightKey( wKey ) )
		return false;

	if( m_vKeyInfo[eKeyTypeArray] == wKey )
		return false;

	m_bUse = true;
	m_vKeyInfo[eKeyTypeArray] = wKey;
	return true;
}

bool CKeyInfo::IsRightKey( WORD wKey )
{
	if(	wKey == KEY_ENTER  ||
		wKey == KEY_ESCAPE ||
		wKey == KEY_LWIN   ||
		wKey == KEY_RWIN)
	{
		return false;
	}

	return true;
}

bool CKeyInfo::IsRightMacro( const CKeyInfo &rKeyInfo )
{
	if( m_vMecroTextInfo.empty() )
		return false;
	if( m_vMecroTextInfo.size() < MAX_MACRO )
		return false;

	if( rKeyInfo.m_vMecroTextInfo.empty() )
		return false;
	if( rKeyInfo.m_vMecroTextInfo.size() < MAX_MACRO )
		return false;

	for(int i = 0;i < MAX_MACRO;i++)
	{
		if( m_vMecroTextInfo[i].m_bAll != rKeyInfo.m_vMecroTextInfo[i].m_bAll )
			return false;
		if( m_vMecroTextInfo[i].m_szText != rKeyInfo.m_vMecroTextInfo[i].m_szText )
			return false;
	}
	return true;
}

char *CKeyInfo::GetINIKeyName(KeyTypeArray eKeyTypeArray)
{
	switch(eKeyTypeArray)
	{
		// game key
	case KTA_UP:
		return "UP";
	case KTA_DOWN:
		return "DOWN";
	case KTA_LEFT:
		return "LEFT";
	case KTA_RIGHT: 
		return "RIGHT";
	case KTA_ATTACK:
		return "ATTACK";
	case KTA_DEFENSE:
		return "DEFENSE";
	case KTA_JUMP:
		return "JUMP";
		// second game key
	case KTA_SECOND_UP:     
		return "SECOND_UP";
	case KTA_SECOND_DOWN:   
		return "SECOND_DOWN";
	case KTA_SECOND_LEFT:   
		return "SECOND_LEFT";
	case KTA_SECOND_RIGHT:  
		return "SECOND_RIGHT";
	case KTA_SECOND_ATTACK: 
		return "SECOND_ATTACK";
	case KTA_SECOND_DEFENSE:
		return "SECOND_DEFENSE";
	case KTA_SECOND_JUMP:   
		return "SECOND_JUMP";
		// skill key
	case KTA_CLOAK_SKILL:
		return "CLOAK_SKILL";
	case KTA_HELM_SKILL:
		return "HELM_SKILL";
	case KTA_ARMOR_SKILL:
		return "ARMOR_SKILL";
	case KTA_WEAPON_SKILL:
		return "WEAPON_SKILL";
		// diagonal
	case KTA_LEFTUP:
		return "LEFTUP";
	case KTA_RIGHTUP:    
		return "RIGHTUP";
	case KTA_LEFTDOWN:   
		return "LEFTDOWN";
	case KTA_RIGHTDOWN:  
		return "RIGHTDOWN";
		// function
	case KTA_CONSUMPTION1:
		return "CONSUMPTION1";
	case KTA_CONSUMPTION2:
		return "CONSUMPTION2";
	case KTA_CONSUMPTION3:
		return "CONSUMPTION3";
	case KTA_CONSUMPTION4:
		return "CONSUMPTION4";
	case KTA_HELP:
		return "HELP";
	case KTA_SHOP:
		return "SHOP";
	case KTA_GROWTH:
		return "GROWTH";
	case KTA_QUEST:
		return "QUEST";	
	case KTA_PRESENT:
		return "PRESENT";
	case KTA_MYHOMEPAGE:
		return "MYHOMEPAGE";	
	case KTA_OPTION:     
		return "OPTION";
	case KTA_MESSENGER:  
		return "MESSENGER";
	case KTA_VOICE:      
		return "VOICE";
	case KTA_SCREENSHOT: 
		return "SCREENSHOT";
	case KTA_MOVIECAPTURE:
		return "MOVIECAPTURE";
	case KTA_SPECIAL_MOTION:
		return "SPECIALMOTION";
	case KTA_ZOOMIN:
		return "ZOOMIN";
	case KTA_ZOOMOUT:
		return "ZOOMOUT";
	case KTA_USERS_CUSTOM_SOUND:
		return "USERS_CUSTOM_SOUND";
	case KTA_PET:
		return "PET";
	case KTA_COOLTIME_RESET:
		return "COOLTIME_RESET";
		// soldier
	case KTA_BATTLE_MODE_ENTRY:
		return "BATTLE_MODE_ENTRY";
	case KTA_BATTLE_MODE_SOLDIER_SELECT:
		return "BATTLE_MODE_SOLDIER_SELECT";
	case KTA_BATTLE_MODE_TAG_SWITCH:
		return "BATTLE_MODE_TAG_SWITCH";
	case KTA_SOLDIER1:  
		return "SOLDIER1";
	case KTA_SOLDIER2:  
		return "SOLDIER2";
	case KTA_SOLDIER3:  
		return "SOLDIER3";
	case KTA_SOLDIER4:  
		return "SOLDIER4";
	case KTA_SOLDIER5:  
		return "SOLDIER5";
	case KTA_SOLDIER6:  
		return "SOLDIER6";
	case KTA_SOLDIER7:  
		return "SOLDIER7";
	case KTA_SOLDIER8:  
		return "SOLDIER8";
	case KTA_SOLDIER9:  
		return "SOLDIER9";
	case KTA_SOLDIER10: 
		return "SOLDIER10";
	case KTA_SOLDIER11: 
		return "SOLDIER11";
	case KTA_SOLDIER12: 
		return "SOLDIER12";
		// macro
	case KTA_MACRO1:  
		return "MACRO1";
	case KTA_MACRO2:  
		return "MACRO2";
	case KTA_MACRO3:  
		return "MACRO3";
	case KTA_MACRO4:  
		return "MACRO4";
	case KTA_MACRO5:  
		return "MACRO5";
	case KTA_MACRO6:  
		return "MACRO6";
	case KTA_MACRO7:  
		return "MACRO7";
	case KTA_MACRO8:  
		return "MACRO8";
	case KTA_MACRO9:  
		return "MACRO9";
	case KTA_MACRO10: 
		return "MACRO10";
	case KTA_MACRO11: 
		return "MACRO11";
	case KTA_MACRO12: 
		return "MACRO12";
		// soldier
	case KTA_SOLDIER13:  
		return "SOLDIER13";
	case KTA_SOLDIER14:  
		return "SOLDIER14";
	case KTA_SOLDIER15:  
		return "SOLDIER15";
	case KTA_SOLDIER16:  
		return "SOLDIER16";
	case KTA_SOLDIER17:  
		return "SOLDIER17";
	case KTA_SOLDIER18:  
		return "SOLDIER18";
	case KTA_SOLDIER19:  
		return "SOLDIER19";
	case KTA_SOLDIER20:  
		return "SOLDIER20";
	case KTA_SOLDIER21:  
		return "SOLDIER21";
	case KTA_SOLDIER22: 
		return "SOLDIER22";
	case KTA_SOLDIER23: 
		return "SOLDIER23";
	case KTA_SOLDIER24: 
		return "SOLDIER24";
	case KTA_SOLDIER25:  
		return "SOLDIER25";
	case KTA_SOLDIER26:  
		return "SOLDIER26";
	case KTA_SOLDIER27:  
		return "SOLDIER27";
	case KTA_SOLDIER28:  
		return "SOLDIER28";
	case KTA_SOLDIER29:  
		return "SOLDIER29";
	case KTA_SOLDIER30:  
		return "SOLDIER30";
	case KTA_SOLDIER31:  
		return "SOLDIER31";
	case KTA_SOLDIER32:  
		return "SOLDIER32";
	case KTA_SOLDIER33:  
		return "SOLDIER33";
	case KTA_SOLDIER34: 
		return "SOLDIER34";
	case KTA_SOLDIER35: 
		return "SOLDIER35";
	case KTA_SOLDIER36: 
		return "SOLDIER36";
	case KTA_SOLDIER37: 
		return "SOLDIER37";
	case KTA_SOLDIER38: 
		return "SOLDIER38";
	case KTA_SOLDIER39: 
		return "SOLDIER39";
	case KTA_SOLDIER40: 
		return "SOLDIER40";
	case KTA_SOLDIER41: 
		return "SOLDIER41";
	case KTA_SOLDIER42: 
		return "SOLDIER42";
	case KTA_SOLDIER43: 
		return "SOLDIER43";
	case KTA_SOLDIER44: 
		return "SOLDIER44";
	case KTA_SOLDIER45: 
		return "SOLDIER45";
	case KTA_SOLDIER46: 
		return "SOLDIER46";
	case KTA_SOLDIER47: 
		return "SOLDIER47";
	case KTA_SOLDIER48: 
		return "SOLDIER48";
	case KTA_SOLDIER49: 
		return "SOLDIER49";
	case KTA_SOLDIER50: 
		return "SOLDIER50";
	}

	return "NONE";
}

CKeyInfo::MecroTextInfo *CKeyInfo::GetMacroTextInfo( KeyTypeArray eKeyTypeArray )
{
	if( !COMPARE( eKeyTypeArray, KTA_MACRO1, KTA_MACRO12 +1 ) )
		return NULL;

	int iMacroArray = eKeyTypeArray - KTA_MACRO1;

	if( !COMPARE( iMacroArray, 0, (int) m_vMecroTextInfo.size() ) )
		return NULL;

	return &m_vMecroTextInfo[iMacroArray];
}

bool CKeyInfo::SetMacroTextInfo( KeyTypeArray eKeyTypeArray, const MecroTextInfo &rMacroTextInfo )
{
	if( !COMPARE( eKeyTypeArray, KTA_MACRO1, KTA_MACRO12 +1 ) )
		return false;

	int iMacroArray = eKeyTypeArray - KTA_MACRO1;

	if( !COMPARE( iMacroArray, 0, (int) m_vMecroTextInfo.size() ) )
		return false;

	if( m_vMecroTextInfo[iMacroArray].m_szText == rMacroTextInfo.m_szText &&
		m_vMecroTextInfo[iMacroArray].m_bAll   == rMacroTextInfo.m_bAll )
		return false;

	m_vMecroTextInfo[iMacroArray] = rMacroTextInfo;

	return true;
}

CKeyInfo::KeyTypeArray CKeyInfo::GetEqual( WORD wKey )
{
	for (int i = 0; i < KTA_MAX ; i++)
	{
		KeyTypeArray eType = (KeyTypeArray) i ;
		if( wKey == GetKey( eType ) )
			return eType;
	}

	return KTA_NONE;
}


void CKeyInfo::Load()
{
	for (int i = 0; i < KTA_MAX ; i++)
	{
		m_vKeyInfo.push_back( 0 );
	}

	for (int i = 0; i < MAX_MACRO ; i++)
	{
		MecroTextInfo kInfo;
		m_vMecroTextInfo.push_back( kInfo );
	}
	m_bUse = false;
}

CKeyInfo& CKeyInfo::operator = ( IN CKeyInfo &rKeyInfo )
{
	for (int i = 0; i < CKeyInfo::KTA_MAX ; i++)
	{
		CKeyInfo::KeyTypeArray eType = (CKeyInfo::KeyTypeArray)i;
		SetKey( eType, rKeyInfo.GetKey( eType ) );
	}

	for( int i = 0; i < CKeyInfo::MAX_MACRO; i++)
	{
		CKeyInfo::KeyTypeArray eType = (CKeyInfo::KeyTypeArray) (i+CKeyInfo::KTA_MACRO1);
		CKeyInfo::MecroTextInfo *pInfo = rKeyInfo.GetMacroTextInfo( eType );
		if( pInfo )
			SetMacroTextInfo( eType,  *pInfo);
	}
	m_bUse = rKeyInfo.IsUseKeys();
	return *this;
}

void CKeyInfo::SaveKey()
{

}

void CKeyInfo::SaveMacroTextInfo()
{

}

//---------------------------------------------------------------------------------------------------------------------
CDefaultKeyInfo::CDefaultKeyInfo()
{

}

CDefaultKeyInfo::~CDefaultKeyInfo()
{
}

void CDefaultKeyInfo::Load()
{
	m_vKeyInfo.clear();
	m_vMecroTextInfo.clear();

	ioINILoader_e kLoader( "config/sp2_default_key.ini" );
	kLoader.SetTitle( "Key" );
	for (int i = 0; i < KTA_MAX ; i++)
	{
		char szBuf[MAX_PATH]="";
		WORD wKey = kLoader.LoadInt(GetINIKeyName((KeyTypeArray)i), 0);
		m_vKeyInfo.push_back(wKey);
	}

	kLoader.SetTitle_e( "MacroText" );
	for (int i = 0; i < MAX_MACRO ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "Text%d", i+1);
		char szBuf[MAX_PATH]="";
		kLoader.LoadString(szKeyName, "", szBuf, sizeof(szBuf));

		MecroTextInfo kInfo;
		kInfo.m_szText = szBuf;

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "Type%d", i+1);
		kInfo.m_bAll = kLoader.LoadBool( szKeyName, true );

		m_vMecroTextInfo.push_back( kInfo );
	}
}

//----------------------------------------------------------------------------------------------------------------------
CCustomKeyInfo::CCustomKeyInfo()
{

}

CCustomKeyInfo::~CCustomKeyInfo()
{

}

void CCustomKeyInfo::Load( const ControlKeys &rkControlKeys )
{
	if( g_MyInfo.GetPublicID().IsEmpty() )
		return;

	m_vKeyInfo.clear();

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\customkey.ini", g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Common" );

	int iVersion = kLoader.LoadInt_e("Version", 0);
	int i = 0;
	int iControlKeysSize = strlen( rkControlKeys.m_szControlKeys );
	if( rkControlKeys.IsRight() && (iControlKeysSize/2) == KTA_MAX ) // 서버 저장값이 MAX값이 아닌 이전 값이라면 초기화 한다. 처음 기획상은 유지하기로 하였는데 몬스터 던저 추가시 초기화 하기로함
	{
		enum { HAX_STRING = 3, MAX_POS = 2, };
		int iPos = 0;
		char szHax[HAX_STRING]="";
		for (i = 0; i < MAX_CONTROL_KEYS_PLUS_ONE ; i++)
		{
			if( rkControlKeys.m_szControlKeys[i] == NULL )
				break;
			szHax[iPos]=rkControlKeys.m_szControlKeys[i];
			iPos++;
			if( iPos >= MAX_POS )
			{
				iPos=0;
				char *pTemp=NULL;
				WORD wKey = strtoul( szHax, &pTemp, 16 );
				m_vKeyInfo.push_back(wKey);
			}
		}

		// key 확장된 경우 빈값 셋팅
		int iLoop = KTA_MAX - m_vKeyInfo.size();
		for (int i = 0; i < iLoop ; i++)
		{
			m_vKeyInfo.push_back( 0 );
		}
	}
	else if( iVersion != CUSTOMKEY_VERSION ) 
	{
		CKeyInfo::Load();
	}
	else
	{
		kLoader.SetTitle( "Key" );
		for (i = 0; i < KTA_MAX ; i++)
		{
			WORD wKey = kLoader.LoadInt( GetINIKeyName((KeyTypeArray)i), 0 );
			m_vKeyInfo.push_back(wKey);
		}
	}

	ioINILoader_e kDefaultLoader( "config/sp2_default_key.ini" );
	// 배틀모드 단축키는 일단 변경되지 않으니 매번 sp2_default_key에 있는 키값과 비교해서 다르면 갱신
	// (처음 배틀모드 들어갔을때는 서버에 키 값 갯수만 갱신되고 키 값은 변경되지 않아서 이렇게 갱신함)
	/*kDefaultLoader.SetTitle( "Key" );
	kLoader.SetTitle( "Key" );
	bool bSaveKey = false;
	for (int iKeyIdx = KTA_BATTLE_MODE_ENTRY; iKeyIdx < KTA_MAX ; iKeyIdx++)
	{
		char szBuf[MAX_PATH]="";
		WORD wDefaultKey = kDefaultLoader.LoadInt(GetINIKeyName((KeyTypeArray)iKeyIdx), 0);
		WORD wLocalCustomKey = kLoader.LoadInt( GetINIKeyName((KeyTypeArray)iKeyIdx), 0 );
		if( iKeyIdx < (int)m_vKeyInfo.size() )
		{
			if( m_vKeyInfo[ iKeyIdx ] != wDefaultKey )
				m_vKeyInfo[ iKeyIdx ] = wDefaultKey;
			if( m_vKeyInfo[ iKeyIdx ] != wLocalCustomKey )
				bSaveKey = true;
		}
	}
	if( bSaveKey )
		SaveKey();*/

	// 서버에서 받은 키값과 customkey.ini에 저장하고 있던 키값이랑 일치하지 않으면 서버에서 보내준 키값을 customkey.ini에 기록한다. (배틀모드 키 값부터...)
	// 버전이 달라도 매크로 텍스트는 로딩한다. - 저장한 내용이 없으면 default setting
	m_vMecroTextInfo.clear();
	enum { MAX_MACRO_LEN = 50 };	
	if( iVersion == 0 )
	{
		kDefaultLoader.SetTitle_e( "MacroText" );
	}
	else
	{
		kLoader.SetTitle_e( "MacroText" );
	}

	for (i = 0; i < MAX_MACRO ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "Text%d", i+1);

		char szBuf[MAX_PATH]="";
		
		if( iVersion == 0 )
		{
			kDefaultLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
		}
		else
		{
			kLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
		}

		int iCursor = 0;
		int iAmount = 0;
		int iCopyCursor = 0;
		char szCopyBuf[MAX_PATH] = "";

		int iTextLen = strlen( szBuf );
		while( iCursor < iTextLen )	// MAX_MACRO_LEN을 넘는 입력제한, '%', '\t' 삭제
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szBuf[iCursor] ) )
#else
			if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szBuf[iCursor] ) )
#endif

#else
			if( IsDBCSLeadByte( (BYTE)szBuf[iCursor] ) )
#endif

				iAmount = 2;
			else
				iAmount = 1;

			if( iCursor + iAmount > MAX_MACRO_LEN )
				break;

			if( iAmount == 2 )
			{
				szCopyBuf[iCopyCursor++] = szBuf[iCursor++];
				szCopyBuf[iCopyCursor++] = szBuf[iCursor++];
			}
			else	// iAmount == 1
			{
				if( szBuf[iCursor] != '%' && szBuf[iCursor] != '\t' )
				{
					szCopyBuf[iCopyCursor++] = szBuf[iCursor];
				}

				iCursor++;
			}
		}

		szCopyBuf[iCopyCursor] = '\0';

		MecroTextInfo kInfo;
		kInfo.m_szText = szCopyBuf;

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "Type%d", i+1);
		
		if( iVersion == 0 )
		{
			kInfo.m_bAll = kDefaultLoader.LoadBool( szKeyName, true );
		}
		else
		{
			kInfo.m_bAll = kLoader.LoadBool( szKeyName, true );
		}

		m_vMecroTextInfo.push_back( kInfo );
	}

	// 주요키가 잘못 셋팅되어 있다면 커스텀키에 디폴트키값을 넣는다.
	if( !COMPARE( GetKey( KTA_UP )   , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_DOWN ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_LEFT ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_RIGHT ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_ATTACK ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_DEFENSE ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_JUMP ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_SECOND_ATTACK ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_SECOND_DEFENSE ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  ||
		!COMPARE( GetKey( KTA_SECOND_JUMP ) , KEY_BACKSPACE, KEY_NUMKEYS + 1 )  )
	{
		// set default
		kDefaultLoader.SetTitle( "Key" );
		for (i = 0; i < KTA_MAX ; i++)
			m_vKeyInfo[i] = kDefaultLoader.LoadInt( GetINIKeyName((KeyTypeArray)i), 0 );

		if( !rkControlKeys.IsRight() )
			SaveKey();

		if( iVersion != 0 || rkControlKeys.IsRight() ) // 처음하는 유저거나 파일이 없는 유저는 에러를 입력하지 않았기 때문에 AutoFix가 아니고 안내 없이 디폴트값만 셋팅
			m_bAutoFix = true;
	}
}

void CCustomKeyInfo::SaveKey()
{
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\customkey.ini",g_MyInfo.GetPublicID().c_str() );

	FILE *fp = fopen( fName, "r" );
	if( !fp )
	{
		StringCbPrintf( fName, sizeof( fName ), "Save\\%s" , g_MyInfo.GetPublicID().c_str() );
		CreateDirectory( "Save", NULL );
		CreateDirectory( fName, NULL );
		StringCbPrintf( fName, sizeof( fName ), "Save\\%s\\customkey.ini", g_MyInfo.GetPublicID().c_str() );

		fp = fopen( fName, "w" );
		if( !fp )
			return;
	}
	fclose( fp );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Common" );
	kLoader.SaveInt("Version" , CUSTOMKEY_VERSION );

	kLoader.SetTitle( "Key" );
	int iMax = m_vKeyInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( i >= KTA_MAX ) break;
		kLoader.SaveInt(GetINIKeyName((KeyTypeArray)i), m_vKeyInfo[i]);
	}
}

void CCustomKeyInfo::SaveMacroTextInfo()
{
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\customkey.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Common" );
	kLoader.SaveInt("Version" , CUSTOMKEY_VERSION );

	kLoader.SetTitle_e( "MacroText" );
	int iMax = m_vMecroTextInfo.size();
	for(int i = 0; i < iMax; i++)
	{
		if(i >= MAX_MACRO ) break;

		char szKeyName[MAX_PATH]="";
		StringCbPrintf(szKeyName, sizeof(szKeyName), "Text%d", i+1);
		kLoader.SaveString(szKeyName, m_vMecroTextInfo[i].m_szText.c_str());

		ZeroMemory( szKeyName, sizeof(szKeyName) );
		StringCbPrintf(szKeyName, sizeof(szKeyName), "Type%d", i+1);
		kLoader.SaveBool( szKeyName, m_vMecroTextInfo[i].m_bAll );
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CJoyKeyInfo::CJoyKeyInfo() : m_bUse( true )
{
	m_vKeyInfo.reserve(30);
}

CJoyKeyInfo::~CJoyKeyInfo()
{
	m_vKeyInfo.clear();
}

void CJoyKeyInfo::Clear()
{
	m_vKeyInfo.clear();
	m_bUse = false;
}

WORD CJoyKeyInfo::GetKey( KeyTypeArray eKeyTypeArray )
{
	if( !COMPARE(eKeyTypeArray, 0, (int)m_vKeyInfo.size() ) )
		return KEY_NULL;

	return m_vKeyInfo[eKeyTypeArray];
}

bool CJoyKeyInfo::SetKey( KeyTypeArray eKeyTypeArray, WORD wKey )
{
	if( !COMPARE(eKeyTypeArray, 0, (int)m_vKeyInfo.size()) )
		return false;

	if( !IsRightKey( wKey ) )
		return false;

	if( m_vKeyInfo[eKeyTypeArray] == wKey )
		return false;

	m_bUse = true;
	m_vKeyInfo[eKeyTypeArray] = wKey;
	return true;
}

bool CJoyKeyInfo::IsRightKey( WORD wKey )
{
	if( wKey == KEY_ENTER  ||
		wKey == KEY_ESCAPE ||
		wKey == KEY_LWIN   ||
		wKey == KEY_RWIN)
	{
		return false;
	}

	return true;
}

char *CJoyKeyInfo::GetINIKeyName(KeyTypeArray eKeyTypeArray)
{
	switch(eKeyTypeArray)
	{
		// game key
	case JTA_UP:
		return "UP";
	case JTA_DOWN:
		return "DOWN";
	case JTA_LEFT:
		return "LEFT";
	case JTA_RIGHT: 
		return "RIGHT";
	case JTA_ATTACK:
		return "ATTACK";
	case JTA_DEFENSE:
		return "DEFENSE";
	case JTA_JUMP:
		return "JUMP";
		// skill key
	case JTA_CLOAK_SKILL:
		return "CLOAK_SKILL";
	case JTA_HELM_SKILL:
		return "HELM_SKILL";
	case JTA_ARMOR_SKILL:
		return "ARMOR_SKILL";
	case JTA_WEAPON_SKILL:
		return "WEAPON_SKILL";
		// soldier
	case JTA_SOLDIER1:  
		return "SOLDIER1";
	case JTA_SOLDIER2:  
		return "SOLDIER2";
	case JTA_SOLDIER3:  
		return "SOLDIER3";
	case JTA_SOLDIER4:  
		return "SOLDIER4";
	case JTA_SOLDIER5:  
		return "SOLDIER5";
	case JTA_SOLDIER6:  
		return "SOLDIER6";
	case JTA_SOLDIER7:  
		return "SOLDIER7";
	case JTA_SOLDIER8:  
		return "SOLDIER8";
	case JTA_SOLDIER9:  
		return "SOLDIER9";
	case JTA_SOLDIER10: 
		return "SOLDIER10";
	case JTA_SOLDIER11: 
		return "SOLDIER11";
	case JTA_SOLDIER12: 
		return "SOLDIER12";
	case JTA_SOLDIER13:  
		return "SOLDIER13";
	case JTA_SOLDIER14:  
		return "SOLDIER14";
	case JTA_SOLDIER15:  
		return "SOLDIER15";
	case JTA_SOLDIER16:  
		return "SOLDIER16";
	case JTA_SOLDIER17:  
		return "SOLDIER17";
	case JTA_SOLDIER18:  
		return "SOLDIER18";
	case JTA_SOLDIER19: 
		return "SOLDIER19";
	case JTA_SOLDIER20:  
		return "SOLDIER20";
	case JTA_SOLDIER21:  
		return "SOLDIER21";
	case JTA_SOLDIER22:  
		return "SOLDIER22";
	case JTA_SOLDIER23: 
		return "SOLDIER23";
	case JTA_SOLDIER24: 
		return "SOLDIER24";
	case JTA_SOLDIER25: 
		return "SOLDIER25";
	case JTA_SOLDIER26:  
		return "SOLDIER26";
	case JTA_SOLDIER27:  
		return "SOLDIER27";
	case JTA_SOLDIER28:  
		return "SOLDIER28";
	case JTA_SOLDIER29:  
		return "SOLDIER29";
	case JTA_SOLDIER30:  
		return "SOLDIER30";
	case JTA_SOLDIER31:  
		return "SOLDIER31";
	case JTA_SOLDIER32:  
		return "SOLDIER32";
	case JTA_SOLDIER33:  
		return "SOLDIER33";
	case JTA_SOLDIER34:  
		return "SOLDIER34";
	case JTA_SOLDIER35: 
		return "SOLDIER35";
	case JTA_SOLDIER36: 
		return "SOLDIER36";
	}

	return "NONE";
}

CJoyKeyInfo::KeyTypeArray CJoyKeyInfo::GetEqual( WORD wKey )
{
	for (int i = 0; i < JTA_MAX ; i++)
	{
		KeyTypeArray eType = (KeyTypeArray) i ;
		if( wKey == GetKey( eType ) )
			return eType;
	}

	return JTA_NONE;
}


void CJoyKeyInfo::Load()
{
	for (int i = 0; i < JTA_MAX ; i++)
	{
		m_vKeyInfo.push_back( 0 );
	}

	m_bUse = false;
}

CJoyKeyInfo& CJoyKeyInfo::operator = ( IN CJoyKeyInfo &rKeyInfo )
{
	for (int i = 0; i < CJoyKeyInfo::JTA_MAX ; i++)
	{
		CJoyKeyInfo::KeyTypeArray eType = (CJoyKeyInfo::KeyTypeArray)i;
		SetKey( eType, rKeyInfo.GetKey( eType ) );
	}

	m_bUse = rKeyInfo.IsUseKeys();
	return *this;
}

void CJoyKeyInfo::SaveKey()
{

}

//---------------------------------------------------------------------------------------------------------------------
CDefaultJoyKeyInfo::CDefaultJoyKeyInfo()
{

}

CDefaultJoyKeyInfo::~CDefaultJoyKeyInfo()
{
}

void CDefaultJoyKeyInfo::Load()
{
	m_vKeyInfo.clear();

	ioINILoader_e kLoader( "config/sp2_default_joykey.ini" );
	kLoader.SetTitle( "Key" );
	for (int i = 0; i < JTA_MAX ; i++)
	{
		WORD wKey = kLoader.LoadInt(GetINIKeyName((KeyTypeArray)i), 0);
		m_vKeyInfo.push_back(wKey);
	}
}

//----------------------------------------------------------------------------------------------------------------------
CCustomJoyKeyInfo::CCustomJoyKeyInfo()
{

}

CCustomJoyKeyInfo::~CCustomJoyKeyInfo()
{

}

void CCustomJoyKeyInfo::Load()
{
	if( g_MyInfo.GetPublicID().IsEmpty() )
		return;

	m_vKeyInfo.clear();

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\customjoykey.ini", g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Common" );

	int iVersion = kLoader.LoadInt_e("Version", 0);
	if( iVersion != CUSTOMKEY_VERSION ) 
	{
		//
		CJoyKeyInfo::Load();
	}
	else
	{
		// 
		int i = 0;
		kLoader.SetTitle( "Key" );
		for (i = 0; i < JTA_MAX ; i++)
		{
			WORD wKey = kLoader.LoadInt( GetINIKeyName((KeyTypeArray)i), 0 );
			m_vKeyInfo.push_back(wKey);
		}

		// 키 체크하여 키값이 NULL이면 디폴트 키값 세팅
		// HARDCODE :: 이렇게 할 경우 디폴트로 지정되지 않은 키와 디폴트로 지정된키를 바꾸면
		//             키가 중복되는 사항이 있어 차단. 2013.03.15. KDH
		/*
		ioINILoader_e kDefaultLoader( "config/sp2_default_joykey.ini" );
		kDefaultLoader.SetTitle_e( "Key" );
		int iKeySize = m_vKeyInfo.size();
		for (i = 0; i < iKeySize ; i++)
		{
			if( m_vKeyInfo[i] == 0 )
				m_vKeyInfo[i] = kDefaultLoader.LoadInt( GetINIKeyName((KeyTypeArray)i), 0 );
		}
		*/
	}
}

void CCustomJoyKeyInfo::SaveKey()
{
	if(g_MyInfo.GetPublicID().IsEmpty()) return;

	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\customjoykey.ini",g_MyInfo.GetPublicID().c_str() );

	FILE *fp = fopen( fName, "r" );
	if( !fp )
	{
		StringCbPrintf( fName, sizeof( fName ), "Save\\%s" , g_MyInfo.GetPublicID().c_str() );
		CreateDirectory( "Save", NULL );
		CreateDirectory( fName, NULL );
		StringCbPrintf( fName, sizeof( fName ), "Save\\%s\\customjoykey.ini", g_MyInfo.GetPublicID().c_str() );

		fp = fopen( fName, "w" );
		if( !fp )
			return;
	}
	fclose( fp );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Common" );
	kLoader.SaveInt("Version" , CUSTOMKEY_VERSION );

	kLoader.SetTitle( "Key" );
	int iMax = m_vKeyInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( i >= JTA_MAX ) break;
		kLoader.SaveInt(GetINIKeyName((KeyTypeArray)i), m_vKeyInfo[i]);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<> ioKeyManager* Singleton< ioKeyManager >::ms_Singleton = 0;

ioKeyManager::ioKeyManager(void)
{
	m_bUseCustom     = false;
	m_bSaveKey       = false;
	m_bUseJoyCustom     = false;
	m_bSaveJoyKey       = false;

	m_bSaveMacroText = false;

	m_kServerKeys.Clear();

	m_iLastSoldierKeyPressArray = 0;
}

ioKeyManager::~ioKeyManager(void)
{
	m_KeyTextMap.clear();
}

void ioKeyManager::Load()
{
	m_DefaultKeyInfo.Load();
	m_CustomKeyInfo.Load( m_kServerKeys );

	m_DefaultJoyKeyInfo.Load();
	m_CustomJoyKeyInfo.Load();

	if( m_CustomKeyInfo.IsUseKeys() )
		m_bUseCustom = true;
	else
		m_bUseCustom = false;

	if( m_CustomJoyKeyInfo.IsUseKeys() )
		m_bUseJoyCustom = true;
	else
		m_bUseJoyCustom = false;

	if( m_kServerKeys.IsRight() && m_CustomKeyInfo.IsAutoFix() )
		SendKey();

	// 키 텍스트
	m_KeyTextMap.clear();
	char szKeyText[MAX_PATH] = "";
	ioINILoader_e kLoader( "config/sp2_key_text.ini" );
	kLoader.SetTitle_e( "key_text" );

	kLoader.LoadString_e( "KEY_BACKSPACE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_BACKSPACE, szKeyText ) );

	kLoader.LoadString_e( "KEY_TAB", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_TAB, szKeyText ) );

	kLoader.LoadString_e( "KEY_ENTER", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_ENTER, szKeyText ) );

	kLoader.LoadString_e( "KEY_ESCAPE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_ESCAPE, szKeyText ) );

	kLoader.LoadString_e( "KEY_SPACE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_SPACE, szKeyText ) );

	kLoader.LoadString_e( "KEY_BANG", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_BANG, szKeyText ) );

	kLoader.LoadString_e( "KEY_DQUOTE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_DQUOTE, szKeyText ) );

	kLoader.LoadString_e( "KEY_POUND", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_POUND, szKeyText ) );

	kLoader.LoadString_e( "KEY_DOLLAR", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_DOLLAR, szKeyText ) );

	kLoader.LoadString_e( "KEY_PERCENT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PERCENT, szKeyText ) );

	kLoader.LoadString_e( "KEY_AMPERSAND", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_AMPERSAND, szKeyText ) );

	kLoader.LoadString_e( "KEY_APOSTROPHE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_APOSTROPHE, szKeyText ) );

	kLoader.LoadString_e( "KEY_LBRACE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LBRACE, szKeyText ) );

	kLoader.LoadString_e( "KEY_RBRACE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RBRACE, szKeyText ) );

	kLoader.LoadString_e( "KEY_ASTERISC", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_ASTERISC, szKeyText ) );

	kLoader.LoadString_e( "KEY_PLUS", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PLUS, szKeyText ) );

	kLoader.LoadString_e( "KEY_COMMA", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_COMMA, szKeyText ) );

	kLoader.LoadString_e( "KEY_DASH", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_DASH, szKeyText ) );

	kLoader.LoadString_e( "KEY_PERIOD", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PERIOD, szKeyText ) );

	kLoader.LoadString_e( "KEY_SLASH", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_SLASH, szKeyText ) );

	kLoader.LoadString_e( "KEY_0", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_0, szKeyText ) );

	kLoader.LoadString_e( "KEY_1", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_1, szKeyText ) );

	kLoader.LoadString_e( "KEY_2", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_2, szKeyText ) );

	kLoader.LoadString_e( "KEY_3", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_3, szKeyText ) );

	kLoader.LoadString_e( "KEY_4", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_4, szKeyText ) );

	kLoader.LoadString_e( "KEY_5", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_5, szKeyText ) );

	kLoader.LoadString_e( "KEY_6", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_6, szKeyText ) );

	kLoader.LoadString_e( "KEY_7", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_7, szKeyText ) );

	kLoader.LoadString_e( "KEY_8", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_8, szKeyText ) );

	kLoader.LoadString_e( "KEY_9", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_9, szKeyText ) );

	kLoader.LoadString_e( "KEY_COLON", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_COLON, szKeyText ) );

	kLoader.LoadString_e( "KEY_SEMICOLON", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_SEMICOLON, szKeyText ) );

	kLoader.LoadString_e( "KEY_LESSTHEN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LESSTHEN, szKeyText ) );

	kLoader.LoadString_e( "KEY_EQUALS", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_EQUALS, szKeyText ) );

	kLoader.LoadString_e( "KEY_GREATERTHEN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_GREATERTHEN, szKeyText ) );

	kLoader.LoadString_e( "KEY_QMARK", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_QMARK, szKeyText ) );

	kLoader.LoadString_e( "KEY_LBRACKET", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LBRACKET, szKeyText ) );

	kLoader.LoadString_e( "KEY_BACKSLASH", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_BACKSLASH, szKeyText ) );

	kLoader.LoadString_e( "KEY_RBRACKET", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RBRACKET, szKeyText ) );

	kLoader.LoadString_e( "KEY_CARETE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_CARETE, szKeyText ) );

	kLoader.LoadString_e( "KEY_UNDERSCORE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_UNDERSCORE, szKeyText ) );

	kLoader.LoadString_e( "KEY_GRAVE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_GRAVE, szKeyText ) );

	kLoader.LoadString_e( "KEY_A", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_A, szKeyText ) );

	kLoader.LoadString_e( "KEY_B", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_B, szKeyText ) );

	kLoader.LoadString_e( "KEY_C", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_C, szKeyText ) );

	kLoader.LoadString_e( "KEY_D", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_D, szKeyText ) );

	kLoader.LoadString_e( "KEY_E", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_E, szKeyText ) );

	kLoader.LoadString_e( "KEY_F", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F, szKeyText ) );

	kLoader.LoadString_e( "KEY_G", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_G, szKeyText ) );

	kLoader.LoadString_e( "KEY_H", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_H, szKeyText ) );

	kLoader.LoadString_e( "KEY_I", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_I, szKeyText ) );

	kLoader.LoadString_e( "KEY_J", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_J, szKeyText ) );

	kLoader.LoadString_e( "KEY_K", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_K, szKeyText ) );

	kLoader.LoadString_e( "KEY_L", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_L, szKeyText ) );

	kLoader.LoadString_e( "KEY_M", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_M, szKeyText ) );

	kLoader.LoadString_e( "KEY_N", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_N, szKeyText ) );

	kLoader.LoadString_e( "KEY_O", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_O, szKeyText ) );

	kLoader.LoadString_e( "KEY_P", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_P, szKeyText ) );

	kLoader.LoadString_e( "KEY_Q", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_Q, szKeyText ) );

	kLoader.LoadString_e( "KEY_R", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_R, szKeyText ) );

	kLoader.LoadString_e( "KEY_S", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_S, szKeyText ) );

	kLoader.LoadString_e( "KEY_T", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_T, szKeyText ) );

	kLoader.LoadString_e( "KEY_U", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_U, szKeyText ) );

	kLoader.LoadString_e( "KEY_V", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_V, szKeyText ) );

	kLoader.LoadString_e( "KEY_W", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_W, szKeyText ) );

	kLoader.LoadString_e( "KEY_X", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_X, szKeyText ) );

	kLoader.LoadString_e( "KEY_Y", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_Y, szKeyText ) );

	kLoader.LoadString_e( "KEY_Z", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_Z, szKeyText ) );

	kLoader.LoadString_e( "KEY_LCURLY", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LCURLY, szKeyText ) );

	kLoader.LoadString_e( "KEY_PIPE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PIPE, szKeyText ) );

	kLoader.LoadString_e( "KEY_RCURLY", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RCURLY, szKeyText ) );

	kLoader.LoadString_e( "KEY_TILDA", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_TILDA, szKeyText ) );

	kLoader.LoadString_e( "KEY_DELETE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_DELETE, szKeyText ) );

	kLoader.LoadString_e( "KEY_LEFTSHIFT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LEFTSHIFT, szKeyText ) );

	kLoader.LoadString_e( "KEY_RIGHTSHIFT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RIGHTSHIFT, szKeyText ) );

	kLoader.LoadString_e( "KEY_LEFTCTRL", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LEFTCTRL, szKeyText ) );

	kLoader.LoadString_e( "KEY_RIGHTCTRL", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RIGHTCTRL, szKeyText ) );

	kLoader.LoadString_e( "KEY_LEFTALT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LEFTALT, szKeyText ) );

	kLoader.LoadString_e( "KEY_RIGHTALT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RIGHTALT, szKeyText ) );

	kLoader.LoadString_e( "KEY_LEFTARROW", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LEFTARROW, szKeyText ) );

	kLoader.LoadString_e( "KEY_RIGHTARROW", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RIGHTARROW, szKeyText ) );

	kLoader.LoadString_e( "KEY_UPARROW", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_UPARROW, szKeyText ) );

	kLoader.LoadString_e( "KEY_DOWNARROW", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_DOWNARROW, szKeyText ) );

	kLoader.LoadString_e( "KEY_F1", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F1, szKeyText ) );

	kLoader.LoadString_e( "KEY_F2", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F2, szKeyText ) );

	kLoader.LoadString_e( "KEY_F3", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F3, szKeyText ) );

	kLoader.LoadString_e( "KEY_F4", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F4, szKeyText ) );

	kLoader.LoadString_e( "KEY_F5", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F5, szKeyText ) );

	kLoader.LoadString_e( "KEY_F6", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F6, szKeyText ) );

	kLoader.LoadString_e( "KEY_F7", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F7, szKeyText ) );

	kLoader.LoadString_e( "KEY_F8", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F8, szKeyText ) );

	kLoader.LoadString_e( "KEY_F9", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F9, szKeyText ) );

	kLoader.LoadString_e( "KEY_F10", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F10, szKeyText ) );

	kLoader.LoadString_e( "KEY_F11", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F11, szKeyText ) );

	kLoader.LoadString_e( "KEY_F12", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_F12, szKeyText ) );

	kLoader.LoadString_e( "KEY_INS", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_INS, szKeyText ) );

	kLoader.LoadString_e( "KEY_DEL", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_DEL, szKeyText ) );

	kLoader.LoadString_e( "KEY_HOME", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_HOME, szKeyText ) );

	kLoader.LoadString_e( "KEY_END", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_END, szKeyText ) );

	kLoader.LoadString_e( "KEY_PGUP", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PGUP, szKeyText ) );

	kLoader.LoadString_e( "KEY_PGDN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PGDN, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMSLASH", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMSLASH, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMSTAR", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMSTAR, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMMINUS", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMMINUS, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMPLUS", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMPLUS, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMENTER", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMENTER, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMPERIOD", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMPERIOD, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM0", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM0, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM1", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM1, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM2", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM2, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM3", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM3, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM4", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM4, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM5", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM5, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM6", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM6, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM7", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM7, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM8", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM8, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUM9", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUM9, szKeyText ) );

	kLoader.LoadString_e( "KEY_NUMLOCK", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_NUMLOCK, szKeyText ) );

	kLoader.LoadString_e( "KEY_CAPSLOCK", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_CAPSLOCK, szKeyText ) );

	kLoader.LoadString_e( "KEY_SCROLLLOCK", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_SCROLLLOCK, szKeyText ) );

	kLoader.LoadString_e( "KEY_PRINTSCRN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PRINTSCRN, szKeyText ) );

	kLoader.LoadString_e( "KEY_PAUSE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_PAUSE, szKeyText ) );

	kLoader.LoadString_e( "KEY_LWIN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LWIN, szKeyText ) );

	kLoader.LoadString_e( "KEY_RWIN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RWIN, szKeyText ) );

	kLoader.LoadString_e( "KEY_MOUSELEFT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_MOUSELEFT, szKeyText ) );

	kLoader.LoadString_e( "KEY_MOUSERIGHT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_MOUSERIGHT, szKeyText ) );

	kLoader.LoadString_e( "KEY_MOUSEMIDDLE", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_MOUSEMIDDLE, szKeyText ) );

	kLoader.LoadString_e( "KEY_MOUSEBUTN4", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_MOUSEBUTN4, szKeyText ) );

	kLoader.LoadString_e( "KEY_LSTICKLEFT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LSTICKLEFT, szKeyText ) );

	kLoader.LoadString_e( "KEY_LSTICKRIGHT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LSTICKRIGHT, szKeyText ) );

	kLoader.LoadString_e( "KEY_LSTICKUP", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LSTICKUP, szKeyText ) );

	kLoader.LoadString_e( "KEY_LSTICKDOWN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_LSTICKDOWN, szKeyText ) );

	kLoader.LoadString_e( "KEY_RSTICKLEFT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RSTICKLEFT, szKeyText ) );

	kLoader.LoadString_e( "KEY_RSTICKRIGHT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RSTICKRIGHT, szKeyText ) );

	kLoader.LoadString_e( "KEY_RSTICKUP", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RSTICKUP, szKeyText ) );

	kLoader.LoadString_e( "KEY_RSTICKDOWN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_RSTICKDOWN, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN1", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN1, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN2", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN2, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN3", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN3, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN4", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN4, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN5", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN5, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN6", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN6, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN7", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN7, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN8", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN8, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN9", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN9, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN10", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN10, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN11", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN11, szKeyText ) );

	kLoader.LoadString_e( "KEY_JOYBUTN12", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_JOYBUTN12, szKeyText ) );

	kLoader.LoadString_e( "KEY_POVLEFT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_POVLEFT, szKeyText ) );

	kLoader.LoadString_e( "KEY_POVRIGHT", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_POVRIGHT, szKeyText ) );

	kLoader.LoadString_e( "KEY_POVUP", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_POVUP, szKeyText ) );

	kLoader.LoadString_e( "KEY_POVDOWN", "", szKeyText, MAX_PATH );
	m_KeyTextMap.insert( KeyTextMap::value_type( KEY_POVDOWN, szKeyText ) );

	/*	KeyTextMap::iterator iCreate;
	for( iCreate = m_KeyTextMap.begin() ; iCreate != m_KeyTextMap.end() ; ++iCreate )
	{
	LOG.PrintTimeAndLog( 0, "%s : %d", iCreate->second.c_str(), iCreate->first );
	}
	*/
}

void ioKeyManager::SaveKey()
{
	if( m_bSaveKey )
	{
		m_CustomKeyInfo.SaveKey();
		SendKey();
		m_bSaveKey = false;
	}
}

void ioKeyManager::SaveJoyKey()
{
	if( m_bSaveJoyKey )
	{
		m_CustomJoyKeyInfo.SaveKey();
		m_bSaveJoyKey = false;
	}
}

void ioKeyManager::SaveMacroTextInfo()
{
	if( m_bSaveMacroText )
	{
		m_CustomKeyInfo.SaveMacroTextInfo();
		m_bSaveMacroText = false;
	}
}

void ioKeyManager::SetCustomKey( CKeyInfo::KeyTypeArray eKeyTypeArray, WORD wKey )
{
	if( m_CustomKeyInfo.SetKey(eKeyTypeArray, wKey) )
		m_bSaveKey = true;
}

WORD ioKeyManager::GetCustomKey( CKeyInfo::KeyTypeArray eKeyTypeArray )
{
	return m_CustomKeyInfo.GetKey(eKeyTypeArray);
}

WORD ioKeyManager::GetDefaultKey( CKeyInfo::KeyTypeArray eKeyTypeArray )
{
	return m_DefaultKeyInfo.GetKey(eKeyTypeArray);
}

void ioKeyManager::SetCustomJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray, WORD wKey )
{
	if( m_CustomJoyKeyInfo.SetKey(eKeyTypeArray, wKey) )
		m_bSaveJoyKey = true;
}

WORD ioKeyManager::GetCustomJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray )
{
	return m_CustomJoyKeyInfo.GetKey(eKeyTypeArray);
}

WORD ioKeyManager::GetDefaultJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray )
{
	return m_DefaultJoyKeyInfo.GetKey(eKeyTypeArray);
}

void ioKeyManager::SetCustomMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray , const CKeyInfo::MecroTextInfo &rMacroTextInfo )
{
	if( m_CustomKeyInfo.SetMacroTextInfo( eKeyTypeArray, rMacroTextInfo ) )
		m_bSaveMacroText = true;
}

CKeyInfo::MecroTextInfo *ioKeyManager::GetCustomMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray )
{
	return m_CustomKeyInfo.GetMacroTextInfo(eKeyTypeArray);
}

CKeyInfo::MecroTextInfo * ioKeyManager::GetDefaultMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray )
{
	return m_DefaultKeyInfo.GetMacroTextInfo(eKeyTypeArray);
}

WORD ioKeyManager::GetKey( CKeyInfo::KeyTypeArray eKeyTypeArray )
{
	if(m_bUseCustom)
		return GetCustomKey( eKeyTypeArray );
	else
		return GetDefaultKey( eKeyTypeArray );
}

WORD ioKeyManager::GetJoyKey( CJoyKeyInfo::KeyTypeArray eKeyTypeArray )
{
	if(m_bUseJoyCustom)
		return GetCustomJoyKey( eKeyTypeArray );
	else
		return GetDefaultJoyKey( eKeyTypeArray );
}

void ioKeyManager::GetGameKey( OUT ioUserKeyInput::UseKeyValue &rkKeys )
{
	rkKeys.m_wLeftKey        = GetKey(CKeyInfo::KTA_LEFT);
	rkKeys.m_wRightKey	     = GetKey(CKeyInfo::KTA_RIGHT);		
	rkKeys.m_wUpKey          = GetKey(CKeyInfo::KTA_UP);	
	rkKeys.m_wDownKey	     = GetKey(CKeyInfo::KTA_DOWN);		
	rkKeys.m_wDefenseKey     = GetKey(CKeyInfo::KTA_DEFENSE);     
	rkKeys.m_wJumpKey        = GetKey(CKeyInfo::KTA_JUMP);      
	rkKeys.m_wAttackKey      = GetKey(CKeyInfo::KTA_ATTACK);     
	rkKeys.m_wCloakSkillKey  = GetKey(CKeyInfo::KTA_CLOAK_SKILL );
	rkKeys.m_wHelmetSkillKey = GetKey(CKeyInfo::KTA_HELM_SKILL );
	rkKeys.m_wArmorSkillKey  = GetKey(CKeyInfo::KTA_ARMOR_SKILL );
	rkKeys.m_wWeaponSkillKey = GetKey(CKeyInfo::KTA_WEAPON_SKILL );
}

void ioKeyManager::GetSecondGameKey( OUT ioUserKeyInput::UseKeyValue &rkKeys )
{
	rkKeys.m_wLeftUpKey    = GetKey(CKeyInfo::KTA_LEFTUP);     
	rkKeys.m_wLeftDownKey  = GetKey(CKeyInfo::KTA_LEFTDOWN);     
	rkKeys.m_wRightUpKey   = GetKey(CKeyInfo::KTA_RIGHTUP);     
	rkKeys.m_wRightDownKey = GetKey(CKeyInfo::KTA_RIGHTDOWN);  
	rkKeys.m_wDefenseKey   = GetKey(CKeyInfo::KTA_SECOND_DEFENSE);     
	rkKeys.m_wJumpKey      = GetKey(CKeyInfo::KTA_SECOND_JUMP);      
	rkKeys.m_wAttackKey    = GetKey(CKeyInfo::KTA_SECOND_ATTACK);  

	// 방향 Second key 사용하지 않음
	rkKeys.m_wLeftKey   = KEY_NULL;
	rkKeys.m_wRightKey  = KEY_NULL;
	rkKeys.m_wUpKey     = KEY_NULL;
	rkKeys.m_wDownKey   = KEY_NULL;

	// 	SafeGetKey(rkKeys.m_wLeftKey,     CKeyInfo::KTA_SECOND_LEFT);
	// 	SafeGetKey(rkKeys.m_wRightKey,	  CKeyInfo::KTA_SECOND_RIGHT);		
	// 	SafeGetKey(rkKeys.m_wUpKey,		  CKeyInfo::KTA_SECOND_UP);	
	// 	SafeGetKey(rkKeys.m_wDownKey,	  CKeyInfo::KTA_SECOND_DOWN);		     
}

void ioKeyManager::GetJoyGameKey( OUT ioUserKeyInput::UseKeyValue &rkKeys )
{
	if( Setting::UseJoyPad() )
	{
		rkKeys.m_wLeftKey        = GetJoyKey(CJoyKeyInfo::JTA_LEFT);
		rkKeys.m_wRightKey       = GetJoyKey(CJoyKeyInfo::JTA_RIGHT);
		rkKeys.m_wUpKey          = GetJoyKey(CJoyKeyInfo::JTA_UP);
		rkKeys.m_wDownKey        = GetJoyKey(CJoyKeyInfo::JTA_DOWN);
		
		rkKeys.m_wDefenseKey     = GetJoyKey(CJoyKeyInfo::JTA_DEFENSE);
		rkKeys.m_wJumpKey        = GetJoyKey(CJoyKeyInfo::JTA_JUMP);
		rkKeys.m_wAttackKey      = GetJoyKey(CJoyKeyInfo::JTA_ATTACK);
		
		rkKeys.m_wCloakSkillKey  = GetJoyKey(CJoyKeyInfo::JTA_CLOAK_SKILL );
		rkKeys.m_wHelmetSkillKey = GetJoyKey(CJoyKeyInfo::JTA_HELM_SKILL );
		rkKeys.m_wArmorSkillKey  = GetJoyKey(CJoyKeyInfo::JTA_ARMOR_SKILL );
		rkKeys.m_wWeaponSkillKey = GetJoyKey(CJoyKeyInfo::JTA_WEAPON_SKILL );
	}
	else
	{
		rkKeys.m_wLeftKey   = KEY_NULL;
		rkKeys.m_wRightKey  = KEY_NULL;
		rkKeys.m_wUpKey     = KEY_NULL;
		rkKeys.m_wDownKey   = KEY_NULL;

		rkKeys.m_wDefenseKey	= KEY_NULL;
		rkKeys.m_wJumpKey		= KEY_NULL;
		rkKeys.m_wAttackKey		= KEY_NULL;

		rkKeys.m_wCloakSkillKey		= KEY_NULL;
		rkKeys.m_wHelmetSkillKey	= KEY_NULL;
		rkKeys.m_wArmorSkillKey		= KEY_NULL;
		rkKeys.m_wWeaponSkillKey	= KEY_NULL;
	}
}

CKeyInfo::MecroTextInfo *ioKeyManager::GetMacroTextInfo( CKeyInfo::KeyTypeArray eKeyTypeArray )
{
	if( !m_DefaultKeyInfo.IsRightMacro( m_CustomKeyInfo ) )
		return GetCustomMacroTextInfo( eKeyTypeArray );
	else
		return GetDefaultMacroTextInfo( eKeyTypeArray );
}

ioKeyManager & ioKeyManager::GetSingleton()
{
	return Singleton< ioKeyManager >::GetSingleton(); 
}

bool ioKeyManager::IsHelpKeyPress() 
{
	return g_Input.IsKeyPress( GetKey(CKeyInfo::KTA_HELP) );
}

bool ioKeyManager::IsShopKeyPress() 
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_SHOP ) );
}

bool ioKeyManager::IsGrowthKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_GROWTH ) );
}

bool ioKeyManager::IsQuestKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_QUEST ) );
}

bool ioKeyManager::IsPresentKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_PRESENT ) );
}

bool ioKeyManager::IsMyHomePageKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_MYHOMEPAGE ) );
}

bool ioKeyManager::IsOptionKeyPress() 
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_OPTION) );
}

bool ioKeyManager::IsMessengerKeyPress() 
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_MESSENGER ) );
}

bool ioKeyManager::IsVoiceChatKeyPress() 
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_VOICE) );
}

bool ioKeyManager::IsScreenShotKeyPress() 
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_SCREENSHOT ) );
}

bool ioKeyManager::IsMovieCaptureKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_MOVIECAPTURE ) );
}

bool ioKeyManager::IsSpecialMotionKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_SPECIAL_MOTION ) );
}

bool ioKeyManager::IsUsersCustomSoundKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_USERS_CUSTOM_SOUND ) );
}

bool ioKeyManager::IsZoomInKeyRepeat()
{
	return g_Input.IsKeyRepeat( GetKey( CKeyInfo::KTA_ZOOMIN ) );
}

bool ioKeyManager::IsZoomOutKeyRepeat()
{
	return g_Input.IsKeyRepeat( GetKey( CKeyInfo::KTA_ZOOMOUT ) );
}

bool ioKeyManager::IsPetKeyPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_PET ) );
}

bool ioKeyManager::IsCoolTimeResetPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_COOLTIME_RESET ) );
}

bool ioKeyManager::IsBattleModeEntryPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_BATTLE_MODE_ENTRY ) );
}

bool ioKeyManager::IsBattleModeSoldierSelectPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_BATTLE_MODE_SOLDIER_SELECT ) );
}

bool ioKeyManager::IsBattleModeTagSwitchPress()
{
	return g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_BATTLE_MODE_TAG_SWITCH ) );
}

int ioKeyManager::GetConsumptionKeyPress()
{
	if( g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_CONSUMPTION1 ) ))
		return ((int)CKeyInfo::KTA_CONSUMPTION1);
	else if( g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_CONSUMPTION2 ) ))
		return ((int)CKeyInfo::KTA_CONSUMPTION2);
	else if( g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_CONSUMPTION3 ) ))
		return ((int)CKeyInfo::KTA_CONSUMPTION3);
	else if( g_Input.IsKeyPress( GetKey( CKeyInfo::KTA_CONSUMPTION4 ) ))
		return ((int)CKeyInfo::KTA_CONSUMPTION4);

	return -1;
}

int ioKeyManager::GetSoldierKeyPressArray()
{
	for (int i = 0; i < MAX_CHARACTER_KEY ; i++)
	{
		enum { FIRST_KEY_MAX = 12, SECOND_KEY_MAX = 36, THIRD_KEY_MAX = 50 };
		CKeyInfo::KeyTypeArray eKeyTypeArray = CKeyInfo::KTA_NONE;
		if( COMPARE( i, 0, FIRST_KEY_MAX ) )
			eKeyTypeArray = (CKeyInfo::KeyTypeArray) ( CKeyInfo::KTA_SOLDIER1 + i );
		else if( COMPARE( i, FIRST_KEY_MAX, SECOND_KEY_MAX ) )
			eKeyTypeArray = (CKeyInfo::KeyTypeArray) ( CKeyInfo::KTA_SOLDIER13 + ( i - FIRST_KEY_MAX ) );
		else if( COMPARE( i, SECOND_KEY_MAX, THIRD_KEY_MAX ) )
			eKeyTypeArray = (CKeyInfo::KeyTypeArray) ( CKeyInfo::KTA_SOLDIER37 + ( i - SECOND_KEY_MAX ) );

		if( g_Input.IsKeyPress( GetKey( eKeyTypeArray ) ) )
		{
			m_iLastSoldierKeyPressArray = i;
			return i;
		}

		if( Setting::UseJoyPad() )
		{
			if( g_Input.IsKeyPress( GetJoyKey( (CJoyKeyInfo::KeyTypeArray)( CJoyKeyInfo::JTA_SOLDIER1+i) ) ) )
			{
				m_iLastSoldierKeyPressArray = i;
				return i;
			}
		}
	}

	return -1;
}

CKeyInfo::KeyTypeArray ioKeyManager::GetMacroKeyPress() 
{
	for (int i = 0; i < CKeyInfo::MAX_MACRO ; i++)
	{
		if( g_Input.IsKeyPress( GetKey( (CKeyInfo::KeyTypeArray)( CKeyInfo::KTA_MACRO1+i) ) ) )
			return (CKeyInfo::KeyTypeArray)(i+CKeyInfo::KTA_MACRO1);
	}

	return CKeyInfo::KTA_NONE;
}

WORD ioKeyManager::GetCurrentSoldierKey( int iSoldierNumber )
{
	enum { FIRST_KEY_MAX = 12, SECOND_KEY_MAX = 36, THIRD_KEY_MAX = 50 };
	CKeyInfo::KeyTypeArray eKeyTypeArray = CKeyInfo::KTA_NONE;
	if( COMPARE( iSoldierNumber, 0, FIRST_KEY_MAX ) )
		eKeyTypeArray = (CKeyInfo::KeyTypeArray) ( CKeyInfo::KTA_SOLDIER1 + iSoldierNumber );
	else if( COMPARE( iSoldierNumber, FIRST_KEY_MAX, SECOND_KEY_MAX ) )
		eKeyTypeArray = (CKeyInfo::KeyTypeArray) ( CKeyInfo::KTA_SOLDIER13 + ( iSoldierNumber - FIRST_KEY_MAX ) );
	else if( COMPARE( iSoldierNumber, SECOND_KEY_MAX, THIRD_KEY_MAX ) )
		eKeyTypeArray = (CKeyInfo::KeyTypeArray) ( CKeyInfo::KTA_SOLDIER37 + ( iSoldierNumber - SECOND_KEY_MAX ) );
		
	return GetKey( eKeyTypeArray );
}

WORD ioKeyManager::GetCurrentConsumptionKey(int nSlot)
{
	return GetKey( (CKeyInfo::KeyTypeArray)(CKeyInfo::KTA_CONSUMPTION1 + nSlot));
}

WORD ioKeyManager::GetCurrentHelpKey()
{
	return GetKey( CKeyInfo::KTA_HELP );
}

WORD ioKeyManager::GetCurrentShopKey()
{
	return GetKey( CKeyInfo::KTA_SHOP );
}

WORD ioKeyManager::GetCurrentGrowthKey()
{
	return GetKey( CKeyInfo::KTA_GROWTH );
}

WORD ioKeyManager::GetCurrentQuestKey()
{
	return GetKey( CKeyInfo::KTA_QUEST );
}

WORD ioKeyManager::GetCurrentPresentKey()
{
	return GetKey( CKeyInfo::KTA_PRESENT );
}

WORD ioKeyManager::GetCurrentMyHomePageKey()
{
	return GetKey( CKeyInfo::KTA_MYHOMEPAGE );
}

WORD ioKeyManager::GetCurrentOptionKey()
{
	return GetKey( CKeyInfo::KTA_OPTION );
}

WORD ioKeyManager::GetCurrentMessengerKey()
{
	return GetKey( CKeyInfo::KTA_MESSENGER );
}

WORD ioKeyManager::GetCurrentScreenShotKey()
{
	return GetKey( CKeyInfo::KTA_SCREENSHOT );
}

WORD ioKeyManager::GetCurrentMovieCaptureKey()
{
	return GetKey( CKeyInfo::KTA_MOVIECAPTURE );
}

WORD ioKeyManager::GetCurrentPetKey()
{
	return GetKey( CKeyInfo::KTA_PET );
}

const char *ioKeyManager::GetKeyText(WORD wKey)
{
	KeyTextMap::iterator iter = m_KeyTextMap.find( wKey );
	if( iter != m_KeyTextMap.end() )
		return iter->second.c_str();
	return STR(1);
}

void ioKeyManager::GetCurrent( OUT CKeyInfo &rKeyInfo )
{
	if( m_bUseCustom )
		GetCustom( rKeyInfo );
	else
		GetDefault( rKeyInfo );
	
	if( !m_DefaultKeyInfo.IsRightMacro( m_CustomKeyInfo ) )
		GetCustomTextInfo( rKeyInfo );
}

void ioKeyManager::GetDefault( OUT CKeyInfo &rKeyInfo )
{
	rKeyInfo = m_DefaultKeyInfo;
}

void ioKeyManager::GetCustom( OUT CKeyInfo &rKeyInfo )
{
	rKeyInfo = m_CustomKeyInfo;
}

void ioKeyManager::GetDefaultTextInfo( OUT CKeyInfo &rKeyInfo )
{
	for( int i = 0; i < CKeyInfo::MAX_MACRO; i++)
	{
		CKeyInfo::KeyTypeArray eType = (CKeyInfo::KeyTypeArray) (i+CKeyInfo::KTA_MACRO1);
		CKeyInfo::MecroTextInfo *pInfo = m_DefaultKeyInfo.GetMacroTextInfo( eType );
		if( pInfo )
			rKeyInfo.SetMacroTextInfo( eType,  *pInfo);
	}
}

void ioKeyManager::GetCustomTextInfo( OUT CKeyInfo &rKeyInfo )
{
	for( int i = 0; i < CKeyInfo::MAX_MACRO; i++)
	{
		CKeyInfo::KeyTypeArray eType = (CKeyInfo::KeyTypeArray) (i+CKeyInfo::KTA_MACRO1);
		CKeyInfo::MecroTextInfo *pInfo = m_CustomKeyInfo.GetMacroTextInfo( eType );
		if( pInfo )
			rKeyInfo.SetMacroTextInfo( eType,  *pInfo);
	}
}

void ioKeyManager::GetJoyCurrent( OUT CJoyKeyInfo &rKeyInfo )
{
	if(m_bUseJoyCustom)
		GetJoyCustom( rKeyInfo );
	else
		GetJoyDefault( rKeyInfo );
}

void ioKeyManager::GetJoyDefault( OUT CJoyKeyInfo &rKeyInfo )
{
	rKeyInfo = m_DefaultJoyKeyInfo;
}

void ioKeyManager::GetJoyCustom( OUT CJoyKeyInfo &rKeyInfo )
{
	rKeyInfo = m_CustomJoyKeyInfo;
}

bool ioKeyManager::SetCustomKey( IN CKeyInfo &rKeyInfo )
{
	for (int i = 0; i < CKeyInfo::KTA_MAX ; i++)
	{
		CKeyInfo::KeyTypeArray eType = (CKeyInfo::KeyTypeArray)i;
		SetCustomKey( eType, rKeyInfo.GetKey( eType ) );
	}

	if(m_bSaveKey)
	{
		if( m_CustomKeyInfo.IsUseKeys() )
			m_bUseCustom = true;
		else
			m_bUseCustom = false;
	}

	return m_bSaveKey;
}

bool ioKeyManager::SetCustomJoyKey( IN CJoyKeyInfo &rKeyInfo )
{
	for (int i = 0; i < CJoyKeyInfo::JTA_MAX ; i++)
	{
		CJoyKeyInfo::KeyTypeArray eType = (CJoyKeyInfo::KeyTypeArray)i;
		SetCustomJoyKey( eType, rKeyInfo.GetKey( eType ) );
	}

	if(m_bSaveJoyKey)
	{
		if( m_CustomJoyKeyInfo.IsUseKeys() )
			m_bUseJoyCustom = true;
		else
			m_bUseJoyCustom = false;
	}

	return m_bSaveJoyKey;
}

bool ioKeyManager::SetCustomMacroTextInfo( IN CKeyInfo &rKeyInfo )
{
	for( int i = 0; i < CKeyInfo::MAX_MACRO; i++)
	{
		CKeyInfo::KeyTypeArray eType = (CKeyInfo::KeyTypeArray) (i+CKeyInfo::KTA_MACRO1);
		CKeyInfo::MecroTextInfo *pInfo = rKeyInfo.GetMacroTextInfo( eType );
		if( pInfo )
			SetCustomMacroTextInfo( eType,  *pInfo);
	}

	return m_bSaveMacroText;
}

void ioKeyManager::SendKey()
{
	ControlKeys kControlKeys;
	enum { MAX_HAX = 3, };
	for (int i = 0; i < CKeyInfo::KTA_MAX ; i++)
	{
		char szTempHex[MAX_HAX]="";
		StringCbPrintf( szTempHex, sizeof( szTempHex ), "%02x", GetKey( (CKeyInfo::KeyTypeArray) i ) );
		StringCbCat( kControlKeys.m_szControlKeys, sizeof( kControlKeys.m_szControlKeys ), szTempHex );
	}

	m_kServerKeys = kControlKeys;
	SP2Packet kPacket( CTPK_CONTROL_KEYS );
	kPacket << kControlKeys;
	TCPNetwork::SendToServer( kPacket );
}

void ioKeyManager::SetServerKeys( const ControlKeys &rkServerKeys )
{
	m_kServerKeys = rkServerKeys;
}

bool ioKeyManager::IsAutoFix()
{
	return m_CustomKeyInfo.IsAutoFix();
}

int ioKeyManager::GetLastSoldierSelectKey()
{
	return m_iLastSoldierKeyPressArray;
}