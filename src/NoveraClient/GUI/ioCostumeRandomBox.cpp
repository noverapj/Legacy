
#include "stdafx.h"

#include "ioCostumeRandomBox.h"
#include "../ioExtendSoundManager.h"

//////////////////////////////////////////////////////////////////////////
ioCostumeRandomBox::ioCostumeRandomBox()
{
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;
	
	m_pIcon	= NULL;
	m_pEffect = NULL;
	m_pCostumeMark = NULL;

	m_bResult = false;
	m_bFinalResult = false;

	m_nPeriodTime = -1;
}

ioCostumeRandomBox::~ioCostumeRandomBox()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);
	
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);
	SAFEDELETE( m_pCostumeMark );
}

void ioCostumeRandomBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioCostumeRandomBox::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );
	
	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_End_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "End_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect.push_back( szEffect );
	}
}

void ioCostumeRandomBox::ShowRandomBox( int nMachineCode )
{
	m_bResult = false;
	m_bFinalResult = false;
	m_nPeriodTime = -1;

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_vIconList.clear();

	CostumeShopSubInfoList vTotalList;
	CostumeShopSubInfoList::iterator iter = vTotalList.begin();

	g_CostumeShopInfoMgr.GetCostumeItemList( nMachineCode, vTotalList );
	for( ; iter!=vTotalList.end(); ++iter )
	{
		CostumeShopSubInfo &rkInfo = (*iter);
		CostumeInfo sCostumeInfo;
		if ( !g_CostumeInfoMgr.GetCostumeInfo( rkInfo.m_nCostumeCode, sCostumeInfo ) )
			continue;

		m_vIconList.push_back( sCostumeInfo.m_szIconName );
	}

	if( (int)m_vIconList.size() == 1)
		m_dwRollingEndTime = dwCurTime + 200;

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void ioCostumeRandomBox::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon( fTimePerSec );
	RollingEffect( fTimePerSec );
}

void ioCostumeRandomBox::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bResult && m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		CostumeInfo sCostumeInfo;
		if ( !m_bFinalResult && g_CostumeInfoMgr.GetCostumeInfo( m_nCostumeCode, sCostumeInfo ) )
		{
			ioHashString szIcon = sCostumeInfo.m_szIconName;
			if( !szIcon.IsEmpty() )
			{
				SAFEDELETE( m_pIcon );
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
			}

			m_szCostumeName = sCostumeInfo.m_szName;
			m_dwScaleStartTime = dwCurTime;
		}

		// IconScale 설정
		float fScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bFinalResult )
		{
			m_bFinalResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( g_ExSoundMgr.IsSound( ExSound::EST_GASHAPON_GET ) )
			{
				g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
			}
			else
			{
				if( !m_szResultSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szResultSound );
			}
			
			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
		}
	}
	else if( (int)m_vIconList.size() > 0 )				// Rolling
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )		// 교체
		{
			SAFEDELETE( m_pIcon );

			m_iCurIconArray++;

			if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList.size() ) )
				m_iCurIconArray = 0;

			ioHashString szCurIcon = m_vIconList[m_iCurIconArray];
			if( !szCurIcon.IsEmpty() )
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );

			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

void ioCostumeRandomBox::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bFinalResult )
		iEffectListCnt = m_vEndEfffect.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bFinalResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vEndEfffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void ioCostumeRandomBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bFinalResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// UI코스튬 관련 (코스튬 마크)
	if( m_pCostumeMark )
		m_pCostumeMark->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}
	
	if( m_bFinalResult )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, "장비를 획득했습니다" );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, "아이템을 선택중입니다." );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+18, FONT_SIZE_12, "잠시만 기다려주세요." );
	}
	
	// Text
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = "?????";
	ioHashString szDesc = "?????";

	if( m_bFinalResult )
	{
		char szBuf[MAX_PATH] = "";

		if( !m_szCostumeName.IsEmpty() )
			szName = m_szCostumeName;

		if( m_nPeriodTime == 0 )
		{
			SafeSprintf( szBuf, sizeof( szBuf ), "영구사용" );
			szDesc = szBuf;
		}
		else if( ( m_nPeriodTime / 24 ) > 0 )
		{
			SafeSprintf( szBuf, sizeof( szBuf), "%d일", m_nPeriodTime/24 );
			szDesc = szBuf;
		}
		else
		{
			SafeSprintf( szBuf, sizeof( szBuf), "%d시간", m_nPeriodTime );
			szDesc = szBuf;
		}
	}

	// Name
	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	kPrinter[1].ClearList();
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
	kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
}

void ioCostumeRandomBox::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void ioCostumeRandomBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowCostumeTabDirect( g_MyInfo.GetClassType() );
		}
		break;
	}
}

void ioCostumeRandomBox::ReBatchButton( int iType )
{
	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		ShowChildWnd(ID_GO);
		break;
	}
}

void ioCostumeRandomBox::OnResultInfo( int nCostumeCode, int nPeriodTime )
{
	m_bResult = true;
	m_nCostumeCode = nCostumeCode;
	m_nPeriodTime = nPeriodTime;
}