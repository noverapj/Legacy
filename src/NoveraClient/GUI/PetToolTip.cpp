
#include "StdAfx.h"

#include "PetToolTip.h"

#include "../ioComplexStringPrinter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PetToolTip::PetToolTip()
{
	m_pPetIcon = NULL;
	m_pIconBack   = NULL;
	m_pBottomImg = NULL;

	Clear();
}

PetToolTip::~PetToolTip()
{
	SAFEDELETE( m_pPetIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE(m_pBottomImg);
}

void PetToolTip::iwm_show()
{
}

void PetToolTip::iwm_hide()
{	
	Clear();
}

void PetToolTip::OnProcess( float fTimePerSec )
{
	if( !g_GUIMgr.IsShow( PET_WND ) )
	{
		HideWnd();
		return;
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) 
		return;

	if( IsShow() )
	{	
		if( !SetInfo() )                                          
			HideWnd();
		else
		{
			pMouse->SetToolTipWndID( 0 );
			pMouse->SetToolTipCursor();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
		}
	}
	else
	{
		if( SetInfo() )
		{
			ShowWnd();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
		}
	}
}

void PetToolTip::OnRender()
{
	enum 
	{ 
		PRINT_XOFFSET = 12, 
		FIRST_YOFFSET = 79, 
		SECOND_YOFFSET = 168, 
	};

	ioWnd::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	RenderDesc( nXPos, nYPos );
	RenderGrowth( nXPos, nYPos + FIRST_YOFFSET );
	PrintManual( nXPos+PRINT_XOFFSET, nYPos + SECOND_YOFFSET, FONT_SIZE_12 );
}

void PetToolTip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BottomImg" )
	{
		SAFEDELETE( m_pBottomImg );
		m_pBottomImg = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetToolTip::RenderDesc( int nXPos, int nYPos )
{
	enum 
	{
		X_OFFSET    = 71,
		Y_OFFSET	= 17,

		Y2_OFFSET	= 37,
		Y3_OFFSET	= 53,

		ICON_X_OFFSET = 39,
		ICON_Y_OFFSET = 41,
	};

	ioHashString szRank = g_PetInfoMgr.GetRankName( m_nSelectPetRank );
	if ( m_szPetName.IsEmpty() || szRank.IsEmpty() )
		return;

	m_szDesc.SetTextStyle( TS_NORMAL );
	m_szDesc.SetBkColor( 0, 0, 0 );	
	m_szDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDesc.AddTextPiece( FONT_SIZE_18, m_szPetName.c_str() );
	m_szDesc.PrintFullTextWidthCut( nXPos+X_OFFSET, nYPos + Y_OFFSET, TAT_LEFT, INVEN_TOOLTIP_WIDTHCUT_SIZE );
	m_szDesc.ClearList();

	if ( m_nSelectPetLevel < m_nMaxLevel )
	{
		m_szDesc.SetTextColor( TCT_DEFAULT_GRAY ); 
		m_szDesc.AddTextPiece( FONT_SIZE_12, STR(1), m_nSelectPetLevel, m_nMaxLevel, (int)m_fExpRate );
		m_szDesc.PrintFullTextWidthCut( nXPos+X_OFFSET, nYPos + Y2_OFFSET, TAT_LEFT, INVEN_TOOLTIP_WIDTHCUT_SIZE );
		m_szDesc.ClearList();
	}
	else
	{
		m_szDesc.SetTextColor( TCT_DEFAULT_GRAY ); 
		m_szDesc.AddTextPiece( FONT_SIZE_12, STR(2), m_nSelectPetLevel );
		m_szDesc.PrintFullTextWidthCut( nXPos+X_OFFSET, nYPos + Y2_OFFSET, TAT_LEFT, INVEN_TOOLTIP_WIDTHCUT_SIZE );
		m_szDesc.ClearList();
	}

	m_szDesc.SetTextColor( TCT_DEFAULT_GRAY ); 
	m_szDesc.AddTextPiece( FONT_SIZE_12, STR(3), szRank.c_str() );
	m_szDesc.PrintFullTextWidthCut( nXPos+X_OFFSET, nYPos + Y3_OFFSET, TAT_LEFT, INVEN_TOOLTIP_WIDTHCUT_SIZE );
	m_szDesc.ClearList();

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( FLOAT08 );
		m_pIconBack->Render( nXPos+ICON_X_OFFSET, nYPos+ICON_Y_OFFSET, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if ( m_pPetIcon )
	{
		m_pPetIcon->SetScale( FLOAT08 );
		m_pPetIcon->Render( nXPos+ICON_X_OFFSET, nYPos+ICON_Y_OFFSET, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void PetToolTip::RenderGrowth( int nXPos, int nYPos )
{	
	enum 
	{		
		YGAP                 = 16,
		
		LEFT_TEXT_XOFFSET    = 21,
		RIGHT_TEXT_XOFFSET   = 144,
		TEXT_YOFFSET         = 10,

		GROWTH_XOFFSET = 92,

		DIA_XOFFSET = 10,
		RIGHT_XOFFSET = 123,
		DIA_YOFFSET = 11,
	};	

	if( m_pBottomImg )
		m_pBottomImg->Render( nXPos, nYPos, UI_RENDER_MULTIPLY );

	for ( int i=0; i<MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL);
		kPrinter.SetBkColor( 0, 0, 0 );	
		if( m_nGrowthStat[i] == 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(i+1) );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(i+1) );

			kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
			if( m_nGrowthStat[i] < 100 )
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(9), m_nGrowthStat[i] );
			else
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(10), m_nGrowthStat[i] );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}		

		if ( i < MAX_ITEM_GROWTH )
		{
			kPrinter.PrintFullText( nXPos+RIGHT_TEXT_XOFFSET, nYPos+TEXT_YOFFSET+(YGAP*i), TAT_LEFT );
			kPrinter.ClearList();
			kPrinter.AddTextPiece( FONT_SIZE_9, "◆" );
			kPrinter.PrintFullText( nXPos+DIA_XOFFSET+RIGHT_XOFFSET, nYPos+DIA_YOFFSET+(YGAP*i), TAT_LEFT );
		}
		else
		{
			kPrinter.PrintFullText( nXPos+LEFT_TEXT_XOFFSET, nYPos+TEXT_YOFFSET+(YGAP*(i-MAX_ITEM_GROWTH)), TAT_LEFT );
			kPrinter.ClearList();
			kPrinter.AddTextPiece( FONT_SIZE_9, "◆" );
			kPrinter.PrintFullText( nXPos+DIA_XOFFSET, nYPos+DIA_YOFFSET+(YGAP*(i-MAX_ITEM_GROWTH)), TAT_LEFT );
		}
		kPrinter.ClearList();
	}
}

void PetToolTip::PrintManual( int nXPos, int nYPos , float fScale )
{
	ManualMgr *pManual = ioPetInvenManualMgr::GetSingletonPtr();

	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;

	ioComplexStringPrinter kPrinter;
	int iMaxLine = pManual->GetMaxLine( m_nManualIndex );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		if( !bAutoLine )
		{
			pManual->SetPrinter( m_nManualIndex, iLine, fScale, kPrinter);
			nYPos += pManual->GetSpaceLine( m_nManualIndex, iLine);
			kPrinter.PrintFullTextWidthCut( nXPos, nYPos, TAT_LEFT, DESC_WIDTH );
			kPrinter.ClearList();
		}
		else
		{
			pManual->SetPrinter( m_nManualIndex, iLine, fScale, kPrinter);
			enum { MAX_LOOP = 20, Y_GAP = 18, };
			bool bFirstGap = true;
			for (int i = 0; i < MAX_LOOP ; i++)
			{
				if( kPrinter.GetFullWidth() < DESC_WIDTH )
				{
					if( bFirstGap )
					{
						nYPos += pManual->GetSpaceLine( m_nManualIndex, iLine);
						bFirstGap = false;
					}
					else
						nYPos += Y_GAP;
					kPrinter.PrintFullText( nXPos, nYPos, TAT_LEFT );
					kPrinter.ClearList();
					break;
				}

				ioComplexStringPrinter kPrinterA, kPrinterB;
				kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
				if( bFirstGap )
				{
					nYPos += pManual->GetSpaceLine( m_nManualIndex, iLine);
					bFirstGap = false;
				}
				else
					nYPos += Y_GAP;
				kPrinterA.PrintFullText( nXPos, nYPos, TAT_LEFT );
				kPrinterB.CopyFullText( kPrinter );
				kPrinterA.ClearList();
				kPrinterB.ClearList();
			}
		}
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetToolTip::GetPetInfo()
{	
	if( m_nSelectPetCode <= 0 || m_nSelectPetRank == PRT_NONE )
		return;

	m_szPetName = g_PetInfoMgr.GetPetName( m_nSelectPetCode, (PetRankType)m_nSelectPetRank );
	m_szPetIcon = g_PetInfoMgr.GetPetIcon( m_nSelectPetCode, (PetRankType)m_nSelectPetRank );
	m_nManualIndex = g_PetInfoMgr.GetPetManual( m_nSelectPetCode )-1;

	for ( int i=0; i<MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
		m_nGrowthStat[i] = g_PetInfoMgr.GetTotalStat( m_nSelectPetCode, i, m_nSelectPetLevel );
}

bool PetToolTip::SetInfo()
{
	GetPetInfo();

	if( m_nSelectPetCode <= 0 || m_nSelectPetRank == PRT_NONE || m_szPetName.IsEmpty() || m_szPetIcon.IsEmpty() )
	{
		Clear();
		return false;
	}

	SAFEDELETE( m_pPetIcon );
	m_pPetIcon = g_UIImageSetMgr.CreateImageByFullName( m_szPetIcon );

	return true;
}

void PetToolTip::SetData( int nPetCode, int nPetLevel, int nPetRank, int nCurExp )
{
	m_nSelectPetCode = nPetCode;
	m_nSelectPetLevel = nPetLevel;
	m_nSelectPetRank = nPetRank;

	m_nMaxLevel = g_PetInfoMgr.GetMaxLevel( (PetRankType)m_nSelectPetRank );

	//경험치 퍼센트 설정
	int nMaxExp = g_PetInfoMgr.GetMaxExp( m_nSelectPetLevel, (PetRankType)m_nSelectPetRank );
	if ( nMaxExp > 0 )
		m_fExpRate = ((float)nCurExp/nMaxExp * FLOAT100);
	else
		m_fExpRate = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetToolTip::Clear()
{
	m_nSelectPetCode  = 0;
	m_nSelectPetLevel = 0;
	m_nSelectPetRank  = 0;
	m_nMaxLevel = 0;
	m_fExpRate = 0.0f;

	m_szPetName.Clear();
	m_szPetIcon.Clear();

	for ( int i=0; i<MAX_CHAR_GROWTH+MAX_ITEM_GROWTH; i++ )
		m_nGrowthStat[i] = 0;
}

void PetToolTip::UpdateWndSize()
{
	enum { DEFAULT_HEIGHT = 187, LINE_GAP = 18, };

	ManualMgr *pManual = ioPetInvenManualMgr::GetSingletonPtr();
	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;

	if( !bAutoLine )
	{
		int iMaxLine = pManual->GetMaxLine( m_nManualIndex );
		iMaxLine--; // 1번째는 타이틀이므로 삭제
		if( iMaxLine <= 0 )
			iMaxLine = 0;

		SetSize( GetWidth(), DEFAULT_HEIGHT+(LINE_GAP * iMaxLine) );
	}
	else
	{
		int iMaxLine = pManual->GetMaxLine( m_nManualIndex );
		int iNewLine = iMaxLine - 1; // 1번째는 타이틀이므로 삭제
		if( iNewLine <= 0 )
			iNewLine = 0;

		ioComplexStringPrinter kPrinter;
		int iTotalLineGap = 18;
		if( iNewLine > 0 )
		{
			for(int iLine = 1; iLine < iMaxLine; iLine++)
			{
				enum { MAX_LOOP = 20, Y_GAP = 18, };
				bool bFirstGap = true;
				for (int i = 0; i < MAX_LOOP ; i++)
				{
					if( kPrinter.GetFullWidth() < DESC_WIDTH )
					{
						if( bFirstGap )
						{
							iTotalLineGap += pManual->GetSpaceLine( m_nManualIndex, iLine);
							bFirstGap = false;
						}
						else
							iTotalLineGap += Y_GAP;
						kPrinter.ClearList();
						break;
					}

					ioComplexStringPrinter kPrinterA, kPrinterB;
					kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
					if( bFirstGap )
					{
						iTotalLineGap += pManual->GetSpaceLine( m_nManualIndex, iLine);
						bFirstGap = false;
					}
					else
						iTotalLineGap += Y_GAP;
					kPrinterB.CopyFullText( kPrinter );
					kPrinterA.ClearList();
					kPrinterB.ClearList();
				}
			}
		}
		SetSize( GetWidth(), DEFAULT_HEIGHT+iTotalLineGap );
	}
}