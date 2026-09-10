
#include "StdAfx.h"

#include "PetEggResultWnd.h"

#include "PetWnd.h"

//////////////////////////////////////////////////////////////////////////

PetEggResultWnd::PetEggResultWnd()
{
	m_pIcon	= NULL;
	m_pEffect = NULL;

	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pManual = NULL;

	m_bResult = false;
	m_bFinalResult = false;
	
	m_dwEtcItemType = 0;

	m_nPetCode = 0;
	m_nPetRank = 0;
	m_nPetSlotIndex = 0;
	m_nPetEggArray = 0;
}

PetEggResultWnd::~PetEggResultWnd()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	m_vEndEfffect.clear();
	m_vRollingEffect.clear();

	for (int i = 0; i < MAX_EGG ; i++)
	{
		m_vIconList[i].clear();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void PetEggResultWnd::iwm_show()
{

}

void PetEggResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}

	m_nPetCode = 0;
	m_nPetRank = 0;
	m_nPetSlotIndex = 0;
}

void PetEggResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd( PET_WND ) );
			{
				if( pPetWnd )
				{
					pPetWnd->SetSelectPetSlot( m_nPetSlotIndex );
					pPetWnd->ShowWnd();
				}
			}
			g_GUIMgr.HideWnd( MY_INVENTORY_WND );
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemType, pUserEtcItem );
			}
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void PetEggResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PetEggResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );		

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	char szName[MAX_PATH]="";
	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.clear();
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf_e( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if ( !szEffect.IsEmpty() )
			m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_End_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf_e( szName, sizeof( szName ), "End_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if ( !szEffect.IsEmpty() )
			m_vEndEfffect.push_back( szEffect );
	}

	for (int i = 0; i < MAX_EGG ; i++)
	{		
		m_vIconList[i].clear();
		StringCbPrintf_e( szName, sizeof( szName ), "PetEgg%d_Max_Icon_Name", i+1 );

		int iMaxIcon = xElement.GetIntAttribute( szName );
		for (int j = 0; j < iMaxIcon ; j++)
		{
			char szIcon[MAX_PATH]="";
			StringCbPrintf_e( szIcon, sizeof( szIcon ), "PetEgg%d_Icon_Name_%d", i+1, j+1 );
			ioHashString szIconName = xElement.GetStringAttribute( szIcon );
			if ( !szIconName.IsEmpty() )
				m_vIconList[i].push_back( szIconName );
		}	
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void PetEggResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon( fTimePerSec );
	RollingEffect( fTimePerSec );
}

void PetEggResultWnd::OnRender()
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
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	if( m_bFinalResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(2) );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+18, FONT_SIZE_12, STR(3) );
	}

	// Text
	ioHashString szName = "?????";
	ioHashString szRank = "?????";

	ioComplexStringPrinter kPrinter;
	if( m_bFinalResult )
	{
		szName = g_PetInfoMgr.GetPetName( m_nPetCode, (PetRankType)m_nPetRank );
		szRank = g_PetInfoMgr.GetRankName( m_nPetRank );
	}

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter.PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(4), szRank.c_str() );
	kPrinter.PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
	kPrinter.ClearList();
}

/////////////////////////////////////////////////////////////////////////////////////////

void PetEggResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

//펫 확인
void PetEggResultWnd::ShowRandomBox( DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();

	m_bResult = false;
	m_bFinalResult = false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	m_nPetCode = 0;
	m_nPetRank = 0;
	m_nPetSlotIndex = 0;
	m_nPetEggArray = GetPetEggArray();
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

//버튼 재배치
void PetEggResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);
		HideChildWnd(ID_GO);
		HideChildWnd(ID_USE);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_GO);
		HideChildWnd(ID_USE);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemType, kSlot ))
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}

//아이콘 롤링
void PetEggResultWnd::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bResult && m_dwRollingEndTime < dwCurTime  )			// RollingEnd
	{
		// Icon 설정
		if( !m_bFinalResult )		// 한 번만 하기 위해
		{
			if ( m_nPetCode > 0 && m_nPetRank > 0 )
			{
				SAFEDELETE( m_pIcon );

				ioHashString szIconName = g_PetInfoMgr.GetPetIcon( m_nPetCode, (PetRankType)m_nPetRank );
				if( !szIconName.IsEmpty() )
					m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
			}
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

			if( !m_szResultSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szResultSound );

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;		
		}
	}
	else if( (int)m_vIconList[m_nPetEggArray].size() > 0 )				// Rolling
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )		// 교체
		{
			SAFEDELETE( m_pIcon );

			m_iCurIconArray++;

			if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList[m_nPetEggArray].size() ) )
				m_iCurIconArray = 0;

			ioHashString szCurIcon = m_vIconList[m_nPetEggArray][m_iCurIconArray];
			if( !szCurIcon.IsEmpty() )
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );

			m_dwRollingCheckTime = dwCurTime;
		}
	}
}

//이펙트 롤링
void PetEggResultWnd::RollingEffect( float fTimePerSec )
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
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

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
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

//롤링 확인
bool PetEggResultWnd::IsRolling()
{
	if( m_bResult && !m_bFinalResult )
		return true;

	return false;
}

//결과 설정
void PetEggResultWnd::OnResultInfo( int nPetCode, int nPetRank, int nPetSlotIndex )
{
	if ( nPetCode <= 0 || nPetRank <= 0 || nPetSlotIndex <=0 )
		return;

	m_nPetCode = nPetCode;
	m_nPetRank = nPetRank;
	m_nPetSlotIndex = nPetSlotIndex;

	m_bResult = true;	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
}

int PetEggResultWnd::GetPetEggArray()
{
	if ( COMPARE( m_dwEtcItemType, ioEtcItem::EIT_ETC_PET_EGG_001, ioEtcItem::EIT_ETC_PET_EGG_100+1 ) )
	{
		return m_dwEtcItemType - ioEtcItem::EIT_ETC_PET_EGG_001;
	}
	return 0;
}