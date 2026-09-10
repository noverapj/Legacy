#include "StdAfx.h"
#include "CenterKillInfoWnd.h"
#include "../GameEnumType.h"
#include "../Setting.h"

CenterKillInfoWnd::CenterKillInfoWnd(void)
{
	m_pSoldierIcon			= NULL;
	m_pSoldierSubIcon = NULL;
	m_pIconBG				= NULL;
	m_pNoneItemKillerIcon	= NULL;
	
	m_pKillRedLine			= NULL;
	m_pKillBlueLine			= NULL;

	m_pOwnerKillRedImg		= NULL;
	m_pOwnerKillBlueImg		= NULL;

	m_pOwnerDieRedImg		= NULL;
	m_pOwnerDieBlueImg		= NULL;

	m_pTeamCharKillRedImg	= NULL;
	m_pTeamCharKillBlueImg	= NULL;

	m_pTeamCharDieRedImg	= NULL;
	m_pTeamCharDieBlueImg	= NULL;

	m_pCurOtherImg			= NULL;
	m_pCurKillImg			= NULL;

	ClearData();

	m_ReserveCenterKillInfo.Init();
}

CenterKillInfoWnd::~CenterKillInfoWnd(void)
{
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	SAFEDELETE( m_pIconBG );
	SAFEDELETE( m_pNoneItemKillerIcon );

	SAFEDELETE( m_pKillRedLine );
	SAFEDELETE( m_pKillBlueLine );

	SAFEDELETE( m_pOwnerKillRedImg );
	SAFEDELETE( m_pOwnerKillBlueImg );

	SAFEDELETE( m_pOwnerDieRedImg );
	SAFEDELETE( m_pOwnerDieBlueImg );

	SAFEDELETE( m_pTeamCharKillRedImg );
	SAFEDELETE( m_pTeamCharKillBlueImg );

	SAFEDELETE( m_pTeamCharDieRedImg );
	SAFEDELETE( m_pTeamCharDieBlueImg );

	SAFEDELETE( m_pCurOtherImg );
	SAFEDELETE( m_pCurKillImg );

	m_ReserveCenterKillInfo.Init();
}

void CenterKillInfoWnd::ClearData()
{
	m_KillInfoPrinter.ClearList();
	m_szCurCenterInfoSound.Clear();

	m_CurCenterKillType				= CKT_NONE;

	//Line
	m_LineAniType					= CKAT_NONE;
	m_fCurLineAlpha					= 0.0f;
	m_fCurLineAlphaSpeed			= 0.0f;
	m_dwCurLineLoopTime				= 0;
	m_dwCurLineFadeOutTime			= 0;
	m_dwLineAniStartTime			= 0;

	//One Kill
	m_OneKillAniType				= CKAT_NONE;
	m_fCurOneKillAlpha				= 0.0f;
	m_fCurOneKillAlphaSpeed			= 0.0f;
	m_fCurOneKillScale				= 0.0f;
	m_fCurOneKillScaleSpeed			= 0.0f;
	m_dwOneKillAniStartTime			= 0;

	//Other
	m_OtherImgAniType				= CKAT_NONE;
	m_KillImgAniType				= CKAT_NONE;
	m_dwOtherImgStartTime			= 0;
	m_fCurOtherImgAlpha				= 0.0f;
	m_fCurOtherImgAlphaSpeed		= 0.0f;
	m_fCurOtherImgScale				= 0.0f;
	m_fCurOtherImgScaleSpeed		= 0.0f;
	m_dwOtherImgTwinkleStartTime	= 0;
	m_bOtherImgTwinkle				= false;

	m_dwKillImgStartTime			= 0;
	m_fCurKillImgAlpha				= 0.0f;
	m_fCurKillImgAlphaSpeed			= 0.0f;
	m_fCurKillImgScale				= 0.0f;
	m_fCurKillImgScaleSpeed			= 0.0f;
	m_bKillImgTwinkle				= false;
	m_dwKillImgTwinkleStartTime		= 0;
	m_fCurTwinkleAlhpa				= 0.0f;
	m_dwSoundStartTime				= 0;
}

void CenterKillInfoWnd::iwm_hide()
{
	ClearData();
}

void CenterKillInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	char szBuf[MAX_PATH];

	//Sound
	m_dwSoundWaitTime				= (DWORD)xElement.GetIntAttribute_e( "SoundWaitTime" );

	//Line
	m_dwOneKillLineLoopTime			= (DWORD)xElement.GetIntAttribute_e( "OneKillLineLoopTime" );
	m_dwOneKillLineFadeOutTime		= (DWORD)xElement.GetIntAttribute_e( "OneKillLineFadeOutTime" );
	m_dwOtherKillLineLoopTime		= (DWORD)xElement.GetIntAttribute_e( "OtherKillLineLoopTime" );
	m_dwOtherKillLineFadeOutTime	= (DWORD)xElement.GetIntAttribute_e( "OtherKillLineFadeOutTime" );
	
	//One Kill
	m_dwOneKillFadeInTime			= (DWORD)xElement.GetIntAttribute_e( "OneKillFadeInTime" );
	m_dwOneKillLoopTime				= (DWORD)xElement.GetIntAttribute_e( "OneKillLoopTime" );
	m_dwOneKillFadeOutTime			= (DWORD)xElement.GetIntAttribute_e( "OneKillFadeOutTime" );
	m_fOneKillStartAlpha			= xElement.GetFloatAttribute_e( "OneKillStartAlpha" );
	m_fOneKillStartScale			= xElement.GetFloatAttribute_e( "OneKillStartScale" );

	m_szOwnerKillSound				= xElement.GetStringAttribute( "Owner_Kill_Sound" );
	m_szOwnerDieSound				= xElement.GetStringAttribute( "Owner_Die_Sound" );
	m_szTeamCharKillSound			= xElement.GetStringAttribute( "TeamChar_Kill_Sound" );
	m_szTeamCharDieSound			= xElement.GetStringAttribute( "TeamChar_Die_Sound" );
	
	//Other Kill
	m_dwOtherImgFadeInTime			= (DWORD)xElement.GetIntAttribute_e( "OtherImgFadeInTime" );
	m_dwOtherImgLoopTime			= (DWORD)xElement.GetIntAttribute_e( "OtherImgLoopTime" );
	m_dwOtherImgLoopAddTime			= (DWORD)xElement.GetIntAttribute_e( "OtherImgLoopAddTime" );
	m_dwOtherImgTwinkleTime			= (DWORD)xElement.GetIntAttribute_e( "OtherImgTwinkleTime" );
	m_dwOtherImgFadeOutTime			= (DWORD)xElement.GetIntAttribute_e( "OtherImgFadeOutTime" );
	m_fOtherImgStartAlpha			= xElement.GetFloatAttribute_e( "OtherImgStartAlpha" );
	m_fOtherImgStartScale			= xElement.GetFloatAttribute_e( "OtherImgStartScale" );

	m_dwKillImgWaitTime				= (DWORD)xElement.GetIntAttribute_e( "KillImgWaitTime" );
	m_dwKillImgFadeInTime			= (DWORD)xElement.GetIntAttribute_e( "KillImgFadeInTime" );
	m_dwKillImgLoopTime				= (DWORD)xElement.GetIntAttribute_e( "KillImgLoopTime" );
	m_dwKillImgLoopAddTime			= (DWORD)xElement.GetIntAttribute_e( "KillImgLoopAddTime" );
	m_dwKillImgFadeOutTime			= (DWORD)xElement.GetIntAttribute_e( "KillImgFadeOutTime" );
	m_fKillImgStartAlpha			= xElement.GetFloatAttribute_e( "KillImgStartAlpha" );
	m_fKillImgStartScale			= xElement.GetFloatAttribute_e( "KillImgStartScale" );
	
	m_dwKillImgTwinkleTime			= (DWORD)xElement.GetIntAttribute_e( "KillImgTwinkleTime" );

	m_iKillComboMaxCount			= xElement.GetIntAttribute_e( "Kill_Combo_Max_Cnt" );

	m_fTwinkleBlueAlpha				= xElement.GetFloatAttribute_e( "TwinkleBlueAlpha" );
	m_fTwinkleRedAlpha				= xElement.GetFloatAttribute_e( "TwinkleRedAlpha" );

	enum { MIN_KILL = 2 };
	for ( int i=MIN_KILL ; i <= m_iKillComboMaxCount ; ++i )
	{
		KillCountInfo Info;

		wsprintf_e( szBuf, "Kill_Combo_%d_Sound", i );
		ioHashString szName = xElement.GetStringAttribute( szBuf );
		if ( szName.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Number : %d Not Exist Sound", __FUNCTION__, i );
			continue;
		}
		Info.szKillComboSoundName = szName;

		szName.Clear();
		wsprintf_e( szBuf, "Kill_Combo_%d_Img_Red", i );
		szName = xElement.GetStringAttribute( szBuf );
		if ( szName.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Number : %d Not Exist Img_Red", __FUNCTION__, i );
			continue;
		}
		Info.szKillComboImgNameRed = szName;
		
		szName.Clear();
		wsprintf_e( szBuf, "Kill_Combo_%d_Img_RedKill", i );
		szName = xElement.GetStringAttribute( szBuf );
		if ( szName.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Number : %d Not Exist Img_RedKill", __FUNCTION__, i );
			continue;
		}
		Info.szKillComboImgNameRedKill = szName;

		szName.Clear();
		wsprintf_e( szBuf, "Kill_Combo_%d_Img_Blue", i );
		szName = xElement.GetStringAttribute( szBuf );
		if ( szName.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Number : %d Not Exist Img_Blue", __FUNCTION__, i );
			continue;
		}
		Info.szKillComboImgNameBlue = szName;

		szName.Clear();
		wsprintf_e( szBuf, "Kill_Combo_%d_Img_BlueKill", i );
		szName = xElement.GetStringAttribute( szBuf );
		if ( szName.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Number : %d Not Exist Img_BlueKill", __FUNCTION__, i );
			continue;
		}
		Info.szKillComboImgNameBlueKill = szName;
				
		m_KillCountInfoMap.insert( KillCountInfoMap::value_type( i, Info) );
	}
}

void CenterKillInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBG" )
	{
		SAFEDELETE( m_pIconBG );
		m_pIconBG = pImage;
	}
	else if ( szType == "NoneItemKillerIcon" )
	{
		SAFEDELETE( m_pNoneItemKillerIcon );
		m_pNoneItemKillerIcon = pImage;
		if ( m_pNoneItemKillerIcon )
			m_pNoneItemKillerIcon->SetScale( 0.45f );
	}
	else if( szType == "KillRedLine" )
	{
		SAFEDELETE( m_pKillRedLine );
		m_pKillRedLine = pImage;
	}
	else if( szType == "KillBlueLine" )
	{
		SAFEDELETE( m_pKillBlueLine );
		m_pKillBlueLine = pImage;
	}
	else if( szType == "OwnerKillImage_Red" )
	{
		SAFEDELETE( m_pOwnerKillRedImg );
		m_pOwnerKillRedImg = pImage;
	}
	else if( szType == "OwnerKillImage_Blue" )
	{
		SAFEDELETE( m_pOwnerKillBlueImg );
		m_pOwnerKillBlueImg = pImage;
	}
	else if( szType == "OwnerDieImage_Red" )
	{
		SAFEDELETE( m_pOwnerDieRedImg );
		m_pOwnerDieRedImg = pImage;
	}
	else if( szType == "OwnerDieImage_Blue" )
	{
		SAFEDELETE( m_pOwnerDieBlueImg );
		m_pOwnerDieBlueImg = pImage;
	}
	else if( szType == "TeamCharKillImage_Red" )
	{
		SAFEDELETE( m_pTeamCharKillRedImg );
		m_pTeamCharKillRedImg = pImage;
	}
	else if( szType == "TeamCharKillImage_Blue" )
	{
		SAFEDELETE( m_pTeamCharKillBlueImg );
		m_pTeamCharKillBlueImg = pImage;
	}
	else if( szType == "TeamCharDieImage_Red" )
	{
		SAFEDELETE( m_pTeamCharDieRedImg );
		m_pTeamCharDieRedImg = pImage;
	}
	else if( szType == "TeamCharDieImage_Blue" )
	{
		SAFEDELETE( m_pTeamCharDieBlueImg );
		m_pTeamCharDieBlueImg = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CenterKillInfoWnd::SetReserveInfo( const CenterKillType& KillType, const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName, 
										const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale )
{
	m_ReserveCenterKillInfo.Init();

	m_ReserveCenterKillInfo.KillType = KillType;

	m_ReserveCenterKillInfo.iKillCount = iKillCount;
	m_ReserveCenterKillInfo.KillCharTeam = eKillerTeam;
	m_ReserveCenterKillInfo.szKillCharName = szKillerName;

	m_ReserveCenterKillInfo.DieCharTeam = eDieCharTeam;
	m_ReserveCenterKillInfo.szDieCharName = szDieCharName;
	m_ReserveCenterKillInfo.dwKillCharItemCode = dwKillCharItemCode;
}

void CenterKillInfoWnd::SetRserveInfoStart()
{
	ClearData();
	if ( m_ReserveCenterKillInfo.KillType == CKT_ACCUMULATE_KILL )
	{
		SetStartOtherKillCenterInfo( m_ReserveCenterKillInfo.iKillCount, m_ReserveCenterKillInfo.KillCharTeam, m_ReserveCenterKillInfo.szKillCharName,
			m_ReserveCenterKillInfo.DieCharTeam, m_ReserveCenterKillInfo.szDieCharName, m_ReserveCenterKillInfo.dwKillCharItemCode, m_ReserveCenterKillInfo.bKillerMale );
	}
	else
	{
		SetStartOneKillCenterInfo( m_ReserveCenterKillInfo.KillType, m_ReserveCenterKillInfo.iKillCount, m_ReserveCenterKillInfo.KillCharTeam, m_ReserveCenterKillInfo.szKillCharName,
			m_ReserveCenterKillInfo.DieCharTeam, m_ReserveCenterKillInfo.szDieCharName, m_ReserveCenterKillInfo.dwKillCharItemCode, m_ReserveCenterKillInfo.bKillerMale );
	}

	m_ReserveCenterKillInfo.Init();	
}

void CenterKillInfoWnd::SetOtherKillCenterUI( const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName, 
											const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale )
{
	// Kill Count가 Max Count 보다 큰 경우엔 UI를 보여주지 않는다.
	if ( iKillCount > m_iKillComboMaxCount )
		return;

	if ( IsShow() )
	{
		if( m_ReserveCenterKillInfo.IsEmpty() )
			SetReserveInfo( CKT_ACCUMULATE_KILL, iKillCount, eKillerTeam, szKillerName, eDieCharTeam, szDieCharName, dwKillCharItemCode, bKillerMale );
		else
		{
			if ( iKillCount >= m_ReserveCenterKillInfo.iKillCount )
				SetReserveInfo( CKT_ACCUMULATE_KILL, iKillCount, eKillerTeam, szKillerName, eDieCharTeam, szDieCharName, dwKillCharItemCode, bKillerMale );
		}
	}
	else
	{
		SetStartOtherKillCenterInfo( iKillCount, eKillerTeam, szKillerName, eDieCharTeam, szDieCharName, dwKillCharItemCode, bKillerMale );
		ShowWnd();
	}
}

void CenterKillInfoWnd::SetOneKillCenterUI( const CenterKillType& KillType, const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName, const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale )
{
	if ( IsShow() )
	{
		if( m_ReserveCenterKillInfo.IsEmpty() )
			SetReserveInfo( KillType, iKillCount, eKillerTeam, szKillerName, eDieCharTeam, szDieCharName, dwKillCharItemCode, bKillerMale );
		else
		{
			if ( iKillCount <= m_ReserveCenterKillInfo.iKillCount )
				SetReserveInfo( KillType, iKillCount, eKillerTeam, szKillerName, eDieCharTeam, szDieCharName, dwKillCharItemCode, bKillerMale );
		}
	}
	else
	{
		SetStartOneKillCenterInfo( KillType, iKillCount, eKillerTeam, szKillerName, eDieCharTeam, szDieCharName, dwKillCharItemCode, bKillerMale );
		ShowWnd();
	}
}

void CenterKillInfoWnd::SetStartOneKillCenterInfo( const CenterKillType& KillType, const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName, const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale )
{
	m_CurCenterKillType = KillType;

	//Line
	SetLineCenterAniInfo( eKillerTeam, szKillerName, dwKillCharItemCode, m_dwOneKillLineLoopTime, m_dwOneKillLineFadeOutTime, bKillerMale );

	//One Kill
	SetOneKillAniInfo( KillType, eKillerTeam );
}

void CenterKillInfoWnd::SetStartOtherKillCenterInfo( const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName, const TeamType& eDieCharTeam, const ioHashString &szDieCharName, DWORD dwKillCharItemCode, bool bKillerMale )
{
	m_CurCenterKillType = CKT_ACCUMULATE_KILL;

	//Line
	SetLineCenterAniInfo( eKillerTeam, szKillerName, dwKillCharItemCode, m_dwOtherKillLineLoopTime, m_dwOtherKillLineFadeOutTime, bKillerMale );
	
	SetOtherKillAniInfo( iKillCount, eKillerTeam, szKillerName );
}

void CenterKillInfoWnd::SetLineCenterAniInfo( const TeamType& eKillerTeam, const ioHashString &szKillerName, DWORD dwKillCharItemCode, DWORD dwLoopTime, DWORD dwFadeOutTime, bool bKillerMale )
{
	m_KillInfoPrinter.ClearList();
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );

	m_KillInfoPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_KillInfoPrinter.SetBkColor( 0, 0, 0 );
	m_KillInfoPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	m_KillInfoPrinter.AddTextPiece( FONT_SIZE_18, "[%s]", szKillerName );

	if ( eKillerTeam != TEAM_NONE )
	{
		if ( eKillerTeam == TEAM_RED )
			m_pCurLine = m_pKillRedLine;
		else if( eKillerTeam == TEAM_BLUE )
			m_pCurLine = m_pKillBlueLine;
		else
		{
			if ( szKillerName == g_MyInfo.GetPublicID() )
				m_pCurLine = m_pKillBlueLine;
			else
				m_pCurLine = m_pKillRedLine;
		}
	}

	if ( dwKillCharItemCode != 0 )
	{
		int iClassType = ConvertItemToClassType( dwKillCharItemCode );
		m_pSoldierIcon = g_MyInfo.GetSoldierIcon( iClassType, bKillerMale );
		m_pSoldierSubIcon = g_MyInfo.GetSoldierSubIcon( iClassType );
	}	

	m_fCurLineAlpha = MAX_ALPHA_RATE;
	m_dwLineAniStartTime = FRAMEGETTIME();

	m_LineAniType = CKAT_LOOP;

	m_dwCurLineLoopTime = dwLoopTime;
	m_dwCurLineFadeOutTime = dwFadeOutTime;

	if( m_pCurLine )
		m_pCurLine->SetAlpha( m_fCurLineAlpha );

	if( m_pIconBG )
		m_pIconBG->SetAlpha( m_fCurLineAlpha );

	if( m_pSoldierIcon )
	{
		m_pSoldierIcon->SetScale( 0.45f );
		m_pSoldierIcon->SetAlpha( m_fCurLineAlpha );
	}
	if( m_pSoldierSubIcon )
	{
		m_pSoldierSubIcon->SetScale( 0.45f );
		m_pSoldierSubIcon->SetAlpha( m_fCurLineAlpha );
	}
}

void CenterKillInfoWnd::SetOneKillAniInfo( const CenterKillType& KillType, const TeamType& eKillerTeam )
{
	m_szCurCenterInfoSound.Clear();
	m_pCurOneKillImg = NULL;
	m_OneKillAniType = CKAT_FADE_IN;

	switch( KillType )
	{
	case CKT_OWNER_KILL:
		{
			if( eKillerTeam != TEAM_NONE )
			{
				if( eKillerTeam == TEAM_RED )
					m_pCurOneKillImg = m_pOwnerKillRedImg;
				else if( eKillerTeam == TEAM_BLUE )
					m_pCurOneKillImg = m_pOwnerKillBlueImg;
				else
					m_pCurOneKillImg = m_pOwnerKillBlueImg;
			}

			m_szCurCenterInfoSound = m_szOwnerKillSound;
		}
		break;
	case CKT_OWNER_DIE:
		{
			if( eKillerTeam != TEAM_NONE )
			{
				if( eKillerTeam == TEAM_RED )
					m_pCurOneKillImg = m_pOwnerDieRedImg;
				else if( eKillerTeam == TEAM_BLUE )
					m_pCurOneKillImg = m_pOwnerDieBlueImg;
				else
					m_pCurOneKillImg = m_pOwnerDieRedImg;
			}

			m_szCurCenterInfoSound = m_szOwnerDieSound;
		}
		break;
	case CKT_TEAM_CHAR_KILL:
		{
			if( eKillerTeam != TEAM_NONE )
			{
				if( eKillerTeam == TEAM_RED )
					m_pCurOneKillImg = m_pTeamCharKillRedImg;
				else if( eKillerTeam == TEAM_BLUE )
					m_pCurOneKillImg = m_pTeamCharKillBlueImg;
			}

			m_szCurCenterInfoSound = m_szTeamCharKillSound;
		}
		break;
	case CKT_TEAM_CHAR_DIE:
		{
			if( eKillerTeam != TEAM_NONE )
			{
				if( eKillerTeam == TEAM_RED )
					m_pCurOneKillImg = m_pTeamCharDieRedImg;
				else if( eKillerTeam == TEAM_BLUE )
					m_pCurOneKillImg = m_pTeamCharDieBlueImg;
			}

			m_szCurCenterInfoSound = m_szTeamCharDieSound;
		}
		break;
	}

	DWORD dwCurTime = FRAMEGETTIME();;
	m_dwOneKillAniStartTime = dwCurTime;
	m_fCurOneKillAlpha = m_fOneKillStartAlpha;
	float fSecond = (float)m_dwOneKillFadeInTime/FLOAT1000;
	m_fCurOneKillAlphaSpeed = m_fCurOneKillAlpha / fSecond;

	m_fCurOneKillScale = m_fOneKillStartScale;
	m_fCurOneKillScaleSpeed = m_fCurOneKillScale / fSecond;

	m_dwSoundStartTime = dwCurTime + m_dwSoundWaitTime;

	if( m_pCurOneKillImg )
	{
		m_pCurOneKillImg->SetAlpha( m_fCurOneKillAlpha );
		m_pCurOneKillImg->SetScale( m_fCurOneKillScale );
	}
}

void CenterKillInfoWnd::SetOtherKillAniInfo( const int& iKillCount, const TeamType& eKillerTeam, const ioHashString &szKillerName )
{
	if( m_pCurOtherImg )
		SAFEDELETE( m_pCurOtherImg );
	if( m_pCurKillImg )
		SAFEDELETE( m_pCurKillImg );
	m_szCurCenterInfoSound.Clear();


	KillCountInfoMap::iterator iter = m_KillCountInfoMap.find( iKillCount );
	if( iter == m_KillCountInfoMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "%s Kill Count : %d Not Exist Kill Count Info", __FUNCTION__, iKillCount );
		return;
	}
	
	m_szCurCenterInfoSound =  iter->second.szKillComboSoundName;

	if( eKillerTeam == TEAM_BLUE )
	{
		m_pCurOtherImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameBlue );
		m_pCurKillImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameBlueKill );
		m_fCurTwinkleAlhpa = m_fTwinkleBlueAlpha;
	}
	else if( eKillerTeam == TEAM_RED )
	{
		m_pCurOtherImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameRed );
		m_pCurKillImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameRedKill );
		m_fCurTwinkleAlhpa = m_fTwinkleRedAlpha;
	}
	else
	{
		if( szKillerName == g_MyInfo.GetPublicID() )
		{
			m_pCurOtherImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameBlue );
			m_pCurKillImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameBlueKill );
			m_fCurTwinkleAlhpa = m_fTwinkleBlueAlpha;
		}
		else
		{
			m_pCurOtherImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameRed );
			m_pCurKillImg = g_UIImageSetMgr.CreateImageByFullName( iter->second.szKillComboImgNameRedKill );
			m_fCurTwinkleAlhpa = m_fTwinkleRedAlpha;
		}
	}

	if( m_pCurOtherImg == NULL )
		LOG.PrintTimeAndLog( 0, "%s Kill Count : %d Not Exist Other Img", __FUNCTION__, iKillCount );

	if( m_pCurKillImg == NULL )
		LOG.PrintTimeAndLog( 0, "%s Kill Count : %d Not Exist Kill Img", __FUNCTION__, iKillCount );

	m_OtherImgAniType = CKAT_FADE_IN;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwOtherImgStartTime = dwCurTime;
	m_fCurOtherImgAlpha = m_fOtherImgStartAlpha;
	float fSecond = (float)m_dwOtherImgFadeInTime/FLOAT1000;
	m_fCurOtherImgAlphaSpeed = m_fCurOtherImgAlpha / fSecond;

	m_fCurOtherImgScale = m_fOtherImgStartScale;
	m_fCurOtherImgScaleSpeed = m_fCurOtherImgScale / fSecond;

	if( m_pCurOtherImg )
	{
		m_pCurOtherImg->SetAlpha( m_fCurOtherImgAlpha );
		m_pCurOtherImg->SetScale( m_fCurOtherImgScale );
	}

	m_KillImgAniType = CKAT_WAIT;
	m_dwKillImgStartTime = dwCurTime;
	m_fCurKillImgAlpha = 0;
	if( m_pCurKillImg )
		m_pCurKillImg->SetAlpha( m_fCurKillImgAlpha );

	m_dwSoundStartTime = dwCurTime + m_dwSoundWaitTime;
}

void CenterKillInfoWnd::OnProcess( float fTimePerSec )
{
	if ( !IsShow() )
		return;

	OnLineProcess( fTimePerSec );

	if ( m_CurCenterKillType == CKT_ACCUMULATE_KILL )
	{
		OnOtherKillProcess( fTimePerSec );
		//모든 애니 상태 체크
		if ( m_LineAniType == CKAT_NONE && m_OtherImgAniType == CKAT_NONE && m_KillImgAniType == CKAT_NONE )
			m_CurCenterKillType = CKT_NONE;
	}
	else
	{
		OnOneKillProcess( fTimePerSec );
		
		//모든 애니 상태 체크
		if ( m_LineAniType == CKAT_NONE && m_OneKillAniType == CKAT_NONE )
			m_CurCenterKillType = CKT_NONE;
	}

	if ( Setting::IsPlayKillSound() )
	{
		if ( !m_szCurCenterInfoSound.IsEmpty() )
		{
			if ( m_dwSoundStartTime > 0 && m_dwSoundStartTime < FRAMEGETTIME() )
			{
				m_dwSoundStartTime = 0;
				g_SoundMgr.PlaySound( m_szCurCenterInfoSound );
			}
		}
	}

	if ( m_CurCenterKillType == CKT_NONE )
	{
		if ( m_ReserveCenterKillInfo.IsEmpty() )
			HideWnd();
		else
		{
			SetRserveInfoStart();
			ShowWnd();
		}
	}
}

void CenterKillInfoWnd::OnLineProcess( float fTimePerSec )
{
	if ( m_LineAniType == CKAT_NONE )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_LineAniType )
	{
	case CKAT_LOOP:
		{
			if ( m_dwLineAniStartTime > 0 && m_dwLineAniStartTime + m_dwCurLineLoopTime < dwCurTime )
			{
				m_dwLineAniStartTime = FRAMEGETTIME();
				m_LineAniType = CKAT_FADE_OUT;

				float fSecond = (float)m_dwCurLineFadeOutTime/FLOAT1000;
				m_fCurLineAlphaSpeed = MAX_ALPHA_RATE / fSecond;
			}
		}
		break;
	case CKAT_FADE_OUT:
		{
			if ( m_dwLineAniStartTime > 0 && m_dwLineAniStartTime + m_dwCurLineFadeOutTime < dwCurTime )
			{
				m_dwLineAniStartTime = 0;
				m_LineAniType = CKAT_NONE;
			}

			m_fCurLineAlpha -= m_fCurLineAlphaSpeed*fTimePerSec;
			m_fCurLineAlpha = min( MAX_ALPHA_RATE, max( 0, m_fCurLineAlpha ) );

			if( m_pCurLine )
				m_pCurLine->SetAlpha( m_fCurLineAlpha );
			if( m_pIconBG )
				m_pIconBG->SetAlpha( m_fCurLineAlpha );
			if( m_pSoldierIcon )
				m_pSoldierIcon->SetAlpha( m_fCurLineAlpha );
			if( m_pSoldierSubIcon )
				m_pSoldierSubIcon->SetAlpha( m_fCurLineAlpha );
		}
		break;
	}
}

void CenterKillInfoWnd::OnOneKillProcess( float fTimePerSec )
{
	if ( m_OneKillAniType == CKAT_NONE )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_OneKillAniType )
	{
	case CKAT_FADE_IN:
		{
			if ( m_dwOneKillAniStartTime > 0 && m_dwOneKillAniStartTime + m_dwOneKillFadeInTime < dwCurTime )
			{
				m_dwOneKillAniStartTime = FRAMEGETTIME();
				m_OneKillAniType = CKAT_LOOP;
				
				m_fCurOneKillAlpha = MAX_ALPHA_RATE;
				m_fCurOneKillAlphaSpeed = 0.0f;

				m_fCurOneKillScale = FLOAT1;
				m_fCurOneKillScaleSpeed = 0.0f;

				if ( m_pCurOneKillImg )
				{
					m_pCurOneKillImg->SetAlpha( m_fCurOneKillAlpha );
					m_pCurOneKillImg->SetScale( m_fCurOneKillScale );
				}
				break;
			}

			m_fCurOneKillAlpha += m_fCurOneKillAlphaSpeed*fTimePerSec;
			m_fCurOneKillAlpha = max( 0, min( MAX_ALPHA_RATE, m_fCurOneKillAlpha ) );

			m_fCurOneKillScale -= m_fCurOneKillScaleSpeed*fTimePerSec;
			m_fCurOneKillScale = max( FLOAT1, m_fCurOneKillScale );

			if ( m_pCurOneKillImg )
			{
				m_pCurOneKillImg->SetAlpha( m_fCurOneKillAlpha );
				m_pCurOneKillImg->SetScale( m_fCurOneKillScale );
			}
		}
		break;
	case CKAT_LOOP:
		{
			if ( m_dwOneKillAniStartTime > 0 && m_dwOneKillAniStartTime + m_dwOneKillLoopTime < dwCurTime )
			{
				m_dwOneKillAniStartTime = FRAMEGETTIME();
				m_OneKillAniType = CKAT_FADE_OUT;

				float fSecond = (float)m_dwOneKillFadeOutTime/FLOAT1000;
				m_fCurOneKillAlphaSpeed = MAX_ALPHA_RATE / fSecond;
				m_fCurOneKillAlpha = MAX_ALPHA_RATE;
			}
		}
		break;
	case CKAT_FADE_OUT:
		{
			if ( m_dwOneKillAniStartTime > 0 && m_dwOneKillAniStartTime + m_dwOneKillFadeOutTime < dwCurTime )
			{
				m_dwOneKillAniStartTime = 0;
				m_OneKillAniType = CKAT_NONE;
			}

			m_fCurOneKillAlpha -= m_fCurOneKillAlphaSpeed*fTimePerSec;
			m_fCurOneKillAlpha = max( 0, min( MAX_ALPHA_RATE, m_fCurOneKillAlpha ) );

			if ( m_pCurOneKillImg )
				m_pCurOneKillImg->SetAlpha( m_fCurOneKillAlpha );
		}
		break;
	}
}

void CenterKillInfoWnd::OnOtherKillProcess( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_KillImgAniType )
	{
	case CKAT_WAIT:
		{
			if ( m_dwKillImgStartTime > 0 && m_dwKillImgStartTime + m_dwKillImgWaitTime < dwCurTime )
			{
				m_dwKillImgStartTime = FRAMEGETTIME();
				m_KillImgAniType = CKAT_FADE_IN;

				m_fCurKillImgAlpha = m_fKillImgStartAlpha;
				float fSecond = (float)m_dwKillImgFadeInTime/FLOAT1000;
				m_fCurKillImgAlphaSpeed = m_fCurKillImgAlpha / fSecond;

				m_fCurKillImgScale = m_fKillImgStartScale;
				m_fCurKillImgScaleSpeed = m_fCurKillImgScale / fSecond;

				if ( m_pCurKillImg )
				{
					m_pCurKillImg->SetAlpha( m_fCurKillImgAlpha );
					m_pCurKillImg->SetScale( m_fCurKillImgScale );
				}
			}
		}
		break;
	case CKAT_FADE_IN:
		{
			if ( m_dwKillImgStartTime > 0 && m_dwKillImgStartTime + m_dwKillImgFadeInTime < dwCurTime )
			{
				m_dwKillImgStartTime = FRAMEGETTIME();
				m_KillImgAniType = CKAT_LOOP_ADD;
				m_dwKillImgTwinkleStartTime = FRAMEGETTIME();

				m_fCurKillImgAlpha = MAX_ALPHA_RATE;
				m_fCurKillImgAlphaSpeed = 0.0f;

				m_fCurKillImgScale = FLOAT1;
				m_fCurKillImgScaleSpeed = 0.0f;

				if ( m_pCurKillImg )
				{
					m_pCurKillImg->SetAlpha( m_fCurKillImgAlpha );
					m_pCurKillImg->SetScale( m_fCurKillImgScale );
				}
				break;
			}

			m_fCurKillImgAlpha += m_fCurKillImgAlphaSpeed*fTimePerSec;
			m_fCurKillImgAlpha = max( 0, min( MAX_ALPHA_RATE, m_fCurKillImgAlpha ) );

			m_fCurKillImgScale -= m_fCurKillImgScaleSpeed*fTimePerSec;
			m_fCurKillImgScale = max( FLOAT1, m_fCurKillImgScale );

			if ( m_pCurKillImg )
			{
				m_pCurKillImg->SetAlpha( m_fCurKillImgAlpha );
				m_pCurKillImg->SetScale( m_fCurKillImgScale );
			}
		}
		break;
	case CKAT_LOOP_ADD:
		{
			if ( m_dwKillImgStartTime > 0 && m_dwKillImgStartTime + m_dwKillImgLoopAddTime < dwCurTime )
			{
				m_dwKillImgStartTime = FRAMEGETTIME();
				m_KillImgAniType = CKAT_LOOP;
				m_bKillImgTwinkle = false;
				m_dwKillImgTwinkleStartTime = 0;
				break;
			}

			if ( m_dwKillImgTwinkleStartTime > 0 && m_dwKillImgTwinkleStartTime + m_dwKillImgTwinkleTime < dwCurTime )
			{
				m_dwKillImgTwinkleStartTime = FRAMEGETTIME();
				m_bKillImgTwinkle = !m_bKillImgTwinkle;
			}
		}
		break;
	case CKAT_LOOP:
		{
			if ( m_dwKillImgStartTime > 0 && m_dwKillImgStartTime + m_dwKillImgLoopTime < dwCurTime )
			{
				m_dwKillImgStartTime = FRAMEGETTIME();
				m_KillImgAniType = CKAT_FADE_OUT;
				float fSecond = (float)m_dwKillImgFadeOutTime/FLOAT1000;
				m_fCurKillImgAlphaSpeed = MAX_ALPHA_RATE / fSecond;
				m_fCurKillImgAlpha = MAX_ALPHA_RATE;
			}
		}
		break;
	case CKAT_FADE_OUT:
		{
			if ( m_dwKillImgStartTime > 0 && m_dwKillImgStartTime + m_dwKillImgFadeOutTime < dwCurTime )
			{
				m_dwKillImgStartTime = 0;
				m_KillImgAniType = CKAT_NONE;
				break;
			}

			m_fCurKillImgAlpha -= m_fCurKillImgAlphaSpeed*fTimePerSec;
			m_fCurKillImgAlpha = max( 0, min( MAX_ALPHA_RATE, m_fCurKillImgAlpha ) );

			if ( m_pCurKillImg )
				m_pCurKillImg->SetAlpha( m_fCurKillImgAlpha );
		}
		break;
	}
	
	switch( m_OtherImgAniType )
	{
	case CKAT_FADE_IN:
		{
			if ( m_dwOtherImgStartTime > 0 && m_dwOtherImgStartTime + m_dwOtherImgFadeInTime < dwCurTime )
			{
				m_dwOtherImgStartTime = FRAMEGETTIME();
				m_OtherImgAniType = CKAT_WAIT_LOOP;

				m_fCurOtherImgAlpha = MAX_ALPHA_RATE;
				m_fCurOtherImgAlphaSpeed = 0.0f;

				m_fCurOtherImgScale = FLOAT1;
				m_fCurOtherImgScaleSpeed = 0.0f;

				if ( m_pCurOtherImg )
				{
					m_pCurOtherImg->SetAlpha( m_fCurOtherImgAlpha );
					m_pCurOtherImg->SetScale( m_fCurOtherImgScale );
				}
				break;
			}

			m_fCurOtherImgAlpha += m_fCurOtherImgAlphaSpeed*fTimePerSec;
			m_fCurOtherImgAlpha = max( 0, min( MAX_ALPHA_RATE, m_fCurOtherImgAlpha ) );

			m_fCurOtherImgScale -= m_fCurOtherImgScaleSpeed*fTimePerSec;
			m_fCurOtherImgScale = max( FLOAT1, m_fCurOtherImgScale );

			if ( m_pCurOtherImg )
			{
				m_pCurOtherImg->SetAlpha( m_fCurOtherImgAlpha );
				m_pCurOtherImg->SetScale( m_fCurOtherImgScale );
			}
		}
		break;
	case CKAT_WAIT_LOOP:
		{
			if ( m_KillImgAniType == CKAT_LOOP_ADD )
			{
				m_OtherImgAniType = CKAT_LOOP_ADD;
				m_dwOtherImgStartTime = FRAMEGETTIME();
				m_dwOtherImgTwinkleStartTime = FRAMEGETTIME();
			}
		}
		break;
	case CKAT_LOOP_ADD:
		{
			if ( m_dwOtherImgStartTime > 0 && m_dwOtherImgStartTime + m_dwOtherImgLoopAddTime < dwCurTime )
			{
				m_dwOtherImgStartTime = FRAMEGETTIME();
				m_OtherImgAniType = CKAT_LOOP;
				m_bOtherImgTwinkle = false;
				m_dwOtherImgTwinkleStartTime = 0;
				break;
			}

			if ( m_dwOtherImgTwinkleStartTime > 0 && m_dwOtherImgTwinkleStartTime + m_dwOtherImgTwinkleTime < dwCurTime )
			{
				m_dwOtherImgTwinkleStartTime = FRAMEGETTIME();
				m_bOtherImgTwinkle = !m_bOtherImgTwinkle;
			}
		}
		break;
	case CKAT_LOOP:
		{
			if ( m_dwOtherImgStartTime > 0 && m_dwOtherImgStartTime + m_dwOtherImgLoopTime < dwCurTime )
			{
				m_dwOtherImgStartTime = FRAMEGETTIME();
				m_OtherImgAniType = CKAT_FADE_OUT;

				float fSecond = (float)m_dwOtherImgFadeOutTime/FLOAT1000;
				m_fCurOtherImgAlphaSpeed = MAX_ALPHA_RATE / fSecond;
				m_fCurOtherImgAlpha = MAX_ALPHA_RATE;
			}
		}
		break;
	case CKAT_FADE_OUT:
		{
			if ( m_dwOtherImgStartTime > 0 && m_dwOtherImgStartTime + m_dwOtherImgFadeOutTime < dwCurTime )
			{
				m_dwOtherImgStartTime = 0;
				m_OtherImgAniType = CKAT_NONE;
				break;
			}

			m_fCurOtherImgAlpha -= m_fCurOtherImgAlphaSpeed*fTimePerSec;
			m_fCurOtherImgAlpha = max( 0, min( MAX_ALPHA_RATE, m_fCurOtherImgAlpha ) );

			if ( m_pCurOtherImg )
				m_pCurOtherImg->SetAlpha( m_fCurOtherImgAlpha );
		}
		break;
	}
}

void CenterKillInfoWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos, iYPos;

	enum { GAP_POS_Y = 3,};

	iXPos = GetDerivedPosX() + ( GetWidth() / 2 );
	//xml에서 Y값을 centerinfownd의 Y값과 같게 세팅해 그 값보다 (갭 + 높이) 만큼 위로 올린 상태를 Y값으로 잡는다.
	iYPos = GetDerivedPosY() - (GetHeight() + GAP_POS_Y );
	
	OnLineRender( iXPos, iYPos );
	OnOneKillRender( iXPos, iYPos );
	OnOtherKillRender( iXPos, iYPos );
}

void CenterKillInfoWnd::OnLineRender( int iXPos, int iYPos )
{
	if ( m_LineAniType == CKAT_NONE )
		return;

	enum 
	{
		LINE_OFFSET_Y = 16,

		TEXT_OFFSET_X = 19,
		TEXT_OFFSET_Y = 9,

		ICON_BG_OFFSET_X = -3,
		ICON_BG_OFFSET_Y = 22,

		ICON_OFFSET_X = -2,
		ICON_OFFSET_Y = 23,
	};

	if( m_pCurLine )
		m_pCurLine->Render( iXPos, iYPos + LINE_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	if( !m_KillInfoPrinter.IsEmpty() )
		m_KillInfoPrinter.PrintFullText( iXPos + TEXT_OFFSET_X, iYPos + TEXT_OFFSET_Y, TAT_CENTER, m_fCurLineAlpha );

	float fWidth = m_KillInfoPrinter.GetFullWidth() * FLOAT05;

	if( m_pIconBG )
		m_pIconBG->Render( iXPos - (int)fWidth + ICON_BG_OFFSET_X, iYPos + ICON_BG_OFFSET_Y, UI_RENDER_ADD );

	if( m_pSoldierIcon )
	{
		m_pSoldierIcon->Render( iXPos - (int)fWidth + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_pSoldierSubIcon )
			m_pSoldierSubIcon->Render( iXPos - (int)fWidth + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_pNoneItemKillerIcon )
		m_pNoneItemKillerIcon->Render( iXPos - (int)fWidth + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void CenterKillInfoWnd::OnOneKillRender( int iXPos, int iYPos )
{
	if( m_OneKillAniType == CKAT_NONE )
		return;

	enum {	ONEKILL_OFFSET_Y = 33 };

	if( m_pCurOneKillImg )
		m_pCurOneKillImg->Render( iXPos, iYPos + ONEKILL_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void CenterKillInfoWnd::OnOtherKillRender( int iXPos, int iYPos )
{
	if( m_OtherImgAniType == CKAT_NONE && m_KillImgAniType == CKAT_NONE )
		return;

	enum {	ONEKILL_OFFSET_Y = 33 };

	if ( m_pCurOtherImg )
	{
		m_pCurOtherImg->Render( iXPos, iYPos + ONEKILL_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		if ( m_bOtherImgTwinkle )
		{
			m_pCurOtherImg->SetAlpha( m_fCurTwinkleAlhpa );
			m_pCurOtherImg->Render( iXPos, iYPos + ONEKILL_OFFSET_Y, UI_RENDER_ADD, TFO_BILINEAR );
			m_pCurOtherImg->SetAlpha( m_fCurOtherImgAlpha );
		}
	}

	if ( m_pCurKillImg )
	{
		m_pCurKillImg->Render( iXPos, iYPos + ONEKILL_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		if ( m_bKillImgTwinkle )
		{
			m_pCurKillImg->SetAlpha( m_fCurTwinkleAlhpa );
			m_pCurKillImg->Render( iXPos, iYPos + ONEKILL_OFFSET_Y, UI_RENDER_ADD, TFO_BILINEAR );
			m_pCurKillImg->SetAlpha( m_fCurKillImgAlpha );
		}
	}
}