
#include "StdAfx.h"

#include "SelectMaterialItemBtn.h"

SelectMaterialItemBtn::SelectMaterialItemBtn(void)
{
	m_iCode = 0;
	m_iCurCnt = 0;
	m_nRecommandCode = 0;

	m_pMaterialFrm = NULL;
	m_pMaterialImg = NULL;	
	m_pRecommandMaterialImg = NULL;
	m_pIconImg = NULL;
	m_pSubIconImg = NULL;
	m_pIconBackImg = NULL;
}

SelectMaterialItemBtn::~SelectMaterialItemBtn(void)
{
	SAFEDELETE( m_pMaterialFrm );
	SAFEDELETE( m_pMaterialImg );
	SAFEDELETE( m_pRecommandMaterialImg );
	SAFEDELETE( m_pIconImg );
	SAFEDELETE( m_pSubIconImg );
	SAFEDELETE( m_pIconBackImg );
}

void SelectMaterialItemBtn::iwm_hide()
{
	m_iCode = 0;
	m_iCurCnt = 0;
	m_nRecommandCode = 0;
}

void SelectMaterialItemBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "material_frm" )
	{
		SAFEDELETE( m_pMaterialFrm );
		m_pMaterialFrm = pFrame;
	}
	else
		ioButton::AddRenderFrame( szType, pFrame );
}
void SelectMaterialItemBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "material_image" )
	{
		SAFEDELETE( m_pMaterialImg );
		m_pMaterialImg = pImage;
	}
	else if( szType == "icon_back_image" )
	{
		SAFEDELETE( m_pIconBackImg );
		m_pIconBackImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void SelectMaterialItemBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
		{
			pMouse->SetToolTipWndID( GetID() );
		}
	}
}

void SelectMaterialItemBtn::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
			
	if( !m_pIconImg )
	{		
		if( m_pMaterialImg )
			m_pMaterialImg->Render( iXPos, iYPos );

		if( m_pRecommandMaterialImg )
		{
			m_pRecommandMaterialImg->SetScale( 0.75f );
			m_pRecommandMaterialImg->Render( iXPos+32, iYPos+32, UI_RENDER_GRAY, TFO_BILINEAR );
		}
		if( m_pSubIconImg )
		{
			m_pSubIconImg->SetScale( 0.75f );
			m_pSubIconImg->Render( iXPos+32, iYPos+32, UI_RENDER_GRAY, TFO_BILINEAR );
		}
	}
	else
	{
		if( m_pMaterialFrm )
			m_pMaterialFrm->Render( iXPos, iYPos );

		if( m_pIconBackImg )
		{
			m_pIconBackImg->SetScale( 0.75f );
			m_pIconBackImg->Render( iXPos+32, iYPos+32, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		m_pIconImg->SetScale( 0.75f );
		m_pIconImg->Render( iXPos+32, iYPos+32, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_pSubIconImg )
		{
			m_pSubIconImg->SetScale( 0.75f );
			m_pSubIconImg->Render( iXPos+32, iYPos+32, UI_RENDER_GRAY, TFO_BILINEAR );
		}
		if( IsOver() )
			OnDrawOvered( iXPos, iYPos );
	}
	
	if( !m_CountText.IsEmpty() )
		m_CountText.PrintFullText( iXPos+32, iYPos+42, TAT_CENTER );
}

void SelectMaterialItemBtn::InitData()
{
	m_iCode = 0;
	m_iCurCnt = 0;
	SAFEDELETE( m_pIconImg );
	SAFEDELETE( m_pSubIconImg );
	SAFEDELETE( m_pRecommandMaterialImg );

	m_CountText.ClearList();
	m_CountText.SetTextStyle( TS_NORMAL );
	m_CountText.SetTextColor( 153, 153, 153 );
	m_CountText.AddTextPiece( FONT_SIZE_12, "재료" );
}

void SelectMaterialItemBtn::SetData( int iCode, int iCnt )
{	
	m_iCode = iCode;
	m_iCurCnt = iCnt;

	ioHashString szIcon;
	ioHashString szSubIcon;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iCode );
	if( pEtcItem )
	{
		szIcon = pEtcItem->GetIconName();
		szSubIcon = pEtcItem->GetSubIconName();
	}
	if( szIcon.IsEmpty() )
		szIcon = "UIIconPack13#quest_002";

	SAFEDELETE( m_pIconImg );
	SAFEDELETE( m_pSubIconImg );
	SAFEDELETE( m_pRecommandMaterialImg );
	m_pIconImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );
	m_pSubIconImg = g_UIImageSetMgr.CreateImageByFullName( szSubIcon );

	char szCount[MAX_PATH]="";
	Help::ConvertNumToStrComma( m_iCurCnt, szCount, sizeof( szCount ) );
	m_CountText.ClearList();
	m_CountText.SetTextStyle( TS_OUTLINE_FULL_3X );
	m_CountText.SetBkColor( TCT_DEFAULT_DARKGRAY );
	m_CountText.SetTextColor( TCT_DEFAULT_WHITE );
	m_CountText.AddTextPiece( FONT_SIZE_12, szCount );
}

void SelectMaterialItemBtn::SetRecommand( int iCode, int iCnt )
{	
	m_iCode = 0;
	m_nRecommandCode = iCode;
	m_iCurCnt = iCnt;

	ioHashString szIcon;
	ioHashString szSubIcon;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iCode );
	if( pEtcItem )
	{
		szIcon = pEtcItem->GetIconName();
		szSubIcon = pEtcItem->GetSubIconName();
	}

	if( szIcon.IsEmpty() )
		szIcon = "UIIconPack13#quest_002";

	SAFEDELETE( m_pIconImg );
	SAFEDELETE( m_pSubIconImg );
	SAFEDELETE( m_pRecommandMaterialImg );
	m_pRecommandMaterialImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );
	m_pSubIconImg = g_UIImageSetMgr.CreateImageByFullName( szSubIcon );

	char szCount[MAX_PATH]="";
	Help::ConvertNumToStrComma( m_iCurCnt, szCount, sizeof( szCount ) );
	m_CountText.ClearList();
	m_CountText.SetTextStyle( TS_OUTLINE_FULL_3X );
	m_CountText.SetBkColor( TCT_DEFAULT_DARKGRAY );
	m_CountText.SetTextColor( TCT_DEFAULT_WHITE );
	m_CountText.AddTextPiece( FONT_SIZE_12, szCount );
}

//코드 얻기
int SelectMaterialItemBtn::GetCode()
{
	if ( m_iCode <= 0)
	{
		if ( m_nRecommandCode > 0 )
			return m_nRecommandCode;
	}
	else
		return m_iCode;

	return 0; 
}