
#include "StdAfx.h"

#include "PetIconBtn.h"

#include "../ioComplexStringPrinter.h"

#include "PetToolTip.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PetIconBtn::PetIconBtn()
{
	m_pEmpty = NULL;
	m_pIcon       = NULL;
	m_pIconBack   = NULL;
	m_pBaseFrm = NULL;
	m_pSelectFrm = NULL;
	m_pEquipFrm = NULL;
	m_pEmptyFrm = NULL;
	m_pRankImg = NULL;
	m_pEquipText = NULL;

	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;

	m_nPetCode = 0;
	m_nPetLevel = 0;
	m_nPetRank = 0;
	m_nSlotIndex = 0;
	m_nCurExp = 0;

	m_nMaxLevel = 0;
	m_fExpRate = 0;

	m_szPetRank.Clear();

	m_bDesc = false;
	m_bSelect = false;
	m_bEquip = false;
	m_bPreOver = false;
	m_bAddEquipText = false;
}

PetIconBtn::~PetIconBtn()
{
	SAFEDELETE( m_pEmpty );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pBaseFrm );
	SAFEDELETE( m_pSelectFrm );
	SAFEDELETE( m_pEquipFrm );
	SAFEDELETE( m_pEmptyFrm );
	SAFEDELETE( m_pRankImg );
	SAFEDELETE( m_pEquipText );
}

void PetIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "EquipText" )
	{
		SAFEDELETE( m_pEquipText );
		m_pEquipText = pImage;
	}
	else if( szType == "RankBar" )
	{
		SAFEDELETE( m_pRankImg );
		m_pRankImg = pImage;
	}
	else if( szType == "Empty" )
	{
		SAFEDELETE( m_pEmpty );
		m_pEmpty = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );	
}

void PetIconBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "EmptyFrm" )
	{
		SAFEDELETE( m_pEmptyFrm );
		m_pEmptyFrm = pFrame;
	}
	else if( szType == "BaseFrm" )
	{
		SAFEDELETE( m_pBaseFrm );
		m_pBaseFrm = pFrame;
	}
	else if( szType == "SelectFrm" )
	{
		SAFEDELETE( m_pSelectFrm );
		m_pSelectFrm = pFrame;
	}
	else if( szType == "EquipFrm" )
	{
		SAFEDELETE( m_pEquipFrm );
		m_pEquipFrm = pFrame;
	}
	else
		ioButton::AddRenderFrame( szType, pFrame );
}

void PetIconBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_dwChangeTime = xElement.GetIntAttribute_e( "Change_Time" );
}

void PetIconBtn::OnDrawNormal( int nXPos, int nYPos )
{
	if ( m_nPetCode > 0 )
 	{
		//프레임렌더
		RenderBase( nXPos, nYPos );
 		RenderEquip( nXPos, nYPos );
 		RenderSelect( nXPos, nYPos );

 		RenderBack( nXPos, nYPos );
 		RenderIcon( nXPos, nYPos );
 		RenderRankColor( nXPos, nYPos );
 		RenderDesc( nXPos, nYPos );
 		RenderEquipText( nXPos, nYPos );
 	}
 	else
 		RenderEmpty( nXPos, nYPos );
}

void PetIconBtn::OnDrawOvered( int nXPos, int nYPos )
{	
	if ( m_nPetCode > 0 )
	{
		//프레임 렌더
		RenderBase( nXPos, nYPos );
		RenderEquip( nXPos, nYPos );
		RenderSelect( nXPos, nYPos );

		RenderBack( nXPos, nYPos );
		RenderIcon( nXPos, nYPos );
		RenderRankColor( nXPos, nYPos );
		RenderDesc( nXPos, nYPos );
		RenderEquipText( nXPos, nYPos );

		//오버 추가
		ioButton::OnDrawOveredAdd( nXPos, nYPos );
	}
	else
		RenderEmpty( nXPos, nYPos );
}

void PetIconBtn::OnDrawPushed( int nXPos, int nYPos )
{
	ioButton::OnDrawPushed( nXPos, nYPos );

	if ( m_nPetCode > 0 )
	{
		RenderBase( nXPos, nYPos );
		RenderEquip( nXPos, nYPos );
		RenderSelect( nXPos, nYPos );
		RenderBack( nXPos, nYPos );
		RenderIcon( nXPos, nYPos );
		RenderRankColor( nXPos, nYPos );
		RenderDesc( nXPos, nYPos );
		RenderEquipText( nXPos, nYPos );
	}
	else
		RenderEmpty( nXPos, nYPos );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	//따르는 중
	ChangeEquipText();
	
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	RECT rcIn;
	rcIn.left = GetDerivedPosX();
	rcIn.top  = GetDerivedPosY();
	rcIn.right  = rcIn.left + GetWidth();
	rcIn.bottom = rcIn.top + GetHeight();

	if( !m_bOver && !m_bPreOver )
		return;

	if( m_bOver && !m_bClicked )
	{		
		m_bPreOver = true;

		if( m_nPetCode > 0 )
		{			
			PetToolTip *pPetToolTip = dynamic_cast<PetToolTip*>(g_GUIMgr.FindWnd( PET_TOOLTIP ));
			if ( pPetToolTip )
				pPetToolTip->SetData( m_nPetCode, m_nPetLevel, m_nPetRank, m_nCurExp );
		}
	}
	else if ( !m_bOver )
	{
		m_bPreOver = false;

		PetToolTip *pPetToolTip = dynamic_cast<PetToolTip*>(g_GUIMgr.FindWnd( PET_TOOLTIP ));
		if ( pPetToolTip )
			pPetToolTip->Clear();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetIconBtn::RenderBase( int nXPos, int nYPos )
{
	if( m_pBaseFrm )
		m_pBaseFrm->Render( nXPos, nYPos );	
}

void PetIconBtn::RenderEquip( int nXPos, int nYPos )
{
	if( !m_bEquip )
		return;

	if( m_pEquipFrm )
		m_pEquipFrm->Render( nXPos, nYPos );
}

void PetIconBtn::RenderSelect( int nXPos, int nYPos )
{
	if( !m_bSelect )
		return;

	if( m_pSelectFrm )
		m_pSelectFrm->Render( nXPos, nYPos );	
}

void PetIconBtn::RenderBack( int nXPos, int nYPos )
{
	enum 
	{
		Y_OFFSET   = 37,
	};

	int nNewXPos = nXPos + GetWidth() / 2;
	int nNewYPos = nYPos + Y_OFFSET;

	// Icon
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( m_fBackScale );

		if( m_fBackScale != FLOAT1 )
			m_pIconBack->Render( nNewXPos, nNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconBack->Render( nNewXPos, nNewYPos, UI_RENDER_MULTIPLY );
	}
}

void PetIconBtn::RenderIcon( int nXPos, int nYPos )
{
	enum 
	{
		Y_OFFSET   = 37,
	};

	int nNewXPos = nXPos + GetWidth() / 2;
	int nNewYPos = nYPos + Y_OFFSET;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );
		m_pIcon->Render( nNewXPos, nNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void PetIconBtn::RenderDesc(int nXPos, int nYPos )
{
	enum
	{
		LV_OFFSET_X = 45,
		LV_OFFSET_Y = 69,
		
		RANK_OFFSET_X = 44,
		RANK_OFFSET_Y = 82,
	};

	if ( !m_bDesc || m_szPetRank.IsEmpty() )
		return;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	//레벨
	if ( m_nPetLevel < m_nMaxLevel )
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
		kPrinter.AddTextPiece( FONT_SIZE_10, STR(1), m_nPetLevel, (int)m_fExpRate );
		kPrinter.PrintFullText( nXPos + LV_OFFSET_X, nYPos + LV_OFFSET_Y, TAT_CENTER );
		kPrinter.ClearList();
	}
	else
	{
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
		kPrinter.AddTextPiece( FONT_SIZE_10, STR(2), m_nPetLevel );
		kPrinter.PrintFullText( nXPos + LV_OFFSET_X, nYPos + LV_OFFSET_Y, TAT_CENTER );
		kPrinter.ClearList();
	}

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );		
	kPrinter.AddTextPiece( FONT_SIZE_11, STR(3), m_szPetRank.c_str() );
	kPrinter.PrintFullText( nXPos + RANK_OFFSET_X, nYPos + RANK_OFFSET_Y, TAT_CENTER );
	kPrinter.ClearList();	
}

void PetIconBtn::RenderEmpty( int nXPos, int nYPos )
{
	enum
	{
		X_OFFSET = 44,
		Y_OFFSET = 37,
	};

	if ( m_pEmptyFrm )
		m_pEmptyFrm->Render( nXPos, nYPos );

	if ( m_pEmpty )
	{
		m_pEmpty->SetScale( FLOAT08 );
		m_pEmpty->Render( nXPos+X_OFFSET, nYPos+Y_OFFSET, UI_RENDER_MULTIPLY );
	}
}

void PetIconBtn::RenderRankColor( int nXPos, int nYPos )
{
	enum
	{
		COLOR_X_OFFSET = 7,
		COLOR_Y_OFFSET = 82,
	};

	if ( !m_pRankImg || m_nPetCode <= 0 || m_nPetRank <= 0 )
		return;
		
	switch( (PetRankType)m_nPetRank )
	{
	case PRT_D:
	case PRT_C:
		m_pRankImg->SetColor( 185, 22, 0 );
		break;
	case PRT_B:
	case PRT_A:
		m_pRankImg->SetColor( 17, 114, 34 );
		break;
	case PRT_S:
		m_pRankImg->SetColor( 22, 70, 178 );
		break;
	}

	m_pRankImg->Render( nXPos+COLOR_X_OFFSET, nYPos+COLOR_Y_OFFSET );
}

void PetIconBtn::RenderEquipText( int nXPos, int nYPos )
{
	enum 
	{
		X_TEXT_OFFSET   = 24,
		Y_TEXT_OFFSET   = 50,
	};

	if( !m_bEquip )
		return;

	if ( m_pEquipText )
	{
		m_pEquipText->Render( nXPos+X_TEXT_OFFSET, nYPos+Y_TEXT_OFFSET );

		if ( m_bAddEquipText )
			m_pEquipText->Render( nXPos+X_TEXT_OFFSET, nYPos+Y_TEXT_OFFSET, UI_RENDER_SCREEN );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetIconBtn::ChangeEquipText()
{
	if ( !m_bEquip )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckTime + m_dwChangeTime > dwCurTime )
		return;

	m_dwCheckTime = dwCurTime;
	m_bAddEquipText = !m_bAddEquipText;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//아이콘 설정
bool PetIconBtn::SetIcon( int nPetCode, int nPetLevel, int nPetRank, float fScale, float fBackScale, bool bDesc )
{
	if ( nPetCode == 0 || nPetRank == PRT_NONE || nPetLevel < 0 )
		return false;

	ioHashString szPetIcon = g_PetInfoMgr.GetPetIcon( nPetCode, (PetRankType)nPetRank );
	if ( szPetIcon.IsEmpty() )
		return false;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szPetIcon );
	if( !pIcon )
		return false;

	m_nPetCode = nPetCode;
	m_nPetLevel = nPetLevel;
	m_nPetRank = nPetRank;

	m_nMaxLevel = g_PetInfoMgr.GetMaxLevel( (PetRankType)m_nPetRank );

	//경험치 퍼센트 설정
	int nMaxExp = g_PetInfoMgr.GetMaxExp( m_nPetLevel, (PetRankType)m_nPetRank );
	if ( nMaxExp > 0 )
		m_fExpRate = ((float)m_nCurExp/nMaxExp * FLOAT100);
	else
		m_fExpRate = 0.0f;

	m_fExpRate = min( FLOAT100, m_fExpRate );

	m_szPetRank = g_PetInfoMgr.GetRankName( m_nPetRank );

	m_fScale = fScale;
	m_fBackScale = fBackScale;		

	m_bDesc = bDesc;

	SAFEDELETE( m_pIcon );
	m_pIcon = pIcon;	

	return true;
}

//아이콘 초기화
void PetIconBtn::RemoveIcon()
{
	SAFEDELETE( m_pIcon );

	m_nPetCode = 0;
	m_nPetLevel = 0;
	m_nPetRank = 0;
	m_nSlotIndex = 0;
	m_nCurExp = 0;

	m_nMaxLevel = 0;
	m_fExpRate = 0;

	m_szPetRank.Clear();

	m_bDesc = false;
	m_bSelect = false;
	m_bEquip = false;
	m_bPreOver = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//펫 정보 얻기
int PetIconBtn::GetPetCode()
{
	return m_nPetCode;
}

int PetIconBtn::GetPetRank()
{
	return m_nPetRank;
}

int PetIconBtn::GetSlotIndex()
{
	return m_nSlotIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//펫 아이콘 개별 설정
void PetIconBtn::SetSlotIndex( int nSlotIndex )
{
	m_nSlotIndex = nSlotIndex;
}

void PetIconBtn::SetCurExp( int nCurExp )
{
	m_nCurExp = nCurExp;
}

void PetIconBtn::SetSelect( bool bSelect ) 
{ 
	m_bSelect = bSelect; 
}

void PetIconBtn::SetEquip( bool bEquip ) 
{
	m_bEquip = bEquip;

	m_dwCheckTime = FRAMEGETTIME();
	m_bAddEquipText = false;
}