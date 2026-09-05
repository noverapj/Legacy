#include "StdAfx.h"
#include "DiceGameWnd.h"
#include "../ioDiceGame.h"
#include "ioPetView.h"

DiceToolTip::DiceToolTip()
{
	m_pIconBg      = NULL;
	m_pIcon        = NULL;
	m_szTtitle.Clear();
}

DiceToolTip::~DiceToolTip()
{
	SAFEDELETE( m_pIconBg );
}

void DiceToolTip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBg" )
	{
		SAFEDELETE( m_pIconBg );
		m_pIconBg = pImage;
	}
	else
		AddRenderImage( szType, pImage );
}

void DiceToolTip::iwm_show()
{
}

void DiceToolTip::iwm_hide()
{
	m_pIcon = NULL;
	m_szTtitle.Clear();
}

void DiceToolTip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	UpdateWndSize();

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	pMouse->SetToolTipCursor();
	POINT pt = pMouse->GetMousePos();
	int iXPos = pt.x + 8;
	int iYPos = pt.y + 8;
	int iYOffset = pt.y + GetHeight() + 8;
	if( Setting::Height() < iYOffset )
	{
		iYPos -= ( iYOffset - Setting::Height() ) + 8;
	}

	int	iNewXPos, iNewYPos;
	Help::GetNotCutWndPos( iXPos, iYPos, GetWidth(), GetHeight(), iNewXPos, iNewYPos );
	SetScreenWndPos( iNewXPos, iNewYPos );
}

void DiceToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { ICON_XOFFSET = 48, ICON_YOFFSET = 49,
		PRINT_XOFFSET = 14, FIRST_YOFFSET = 99, };

	if( m_pIconBg )
		m_pIconBg->Render( iXPos + ICON_XOFFSET, iYPos + ICON_YOFFSET, UI_RENDER_MULTIPLY );

	if( m_pIcon )
	{
		m_pIcon->SetScale( FLOAT1 );
		m_pIcon->Render( iXPos + ICON_XOFFSET, iYPos + ICON_YOFFSET );
	}

	PrintManual( iXPos + PRINT_XOFFSET, iYPos + FIRST_YOFFSET, FONT_SIZE_12 );
}

void DiceToolTip::PrintManual( int iXPos, int iYPos, float fScale )
{
	if( !m_szTtitle.IsEmpty() )
	{
		/*ioComplexStringPrinter kPrinterA, kPrinterB;
		ioComplexStringPrinter kTitle;
		int	iYPos	= 41;

		kTitle.SetTextStyle( TS_NORMAL );
		kTitle.SetBkColor( 0, 0, 0 );	
		kTitle.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kTitle.AddTextPiece( FONT_SIZE_17, "%s", m_szTtitle.c_str() );
		
		if( kTitle.GetFullWidth() > DESC_WIDTH - 99 )
			iYPos	-= ( kTitle.GetFullWidth() / ( DESC_WIDTH - 99 ) ) * 6;

		while( kTitle.GetFullWidth() > DESC_WIDTH - 99 )
		{
			kTitle.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH - 99 );
			kPrinterA.PrintFullText( GetDerivedPosX() + 99, GetDerivedPosY() + iYPos, TAT_LEFT );
			kPrinterB.CopyFullText( kTitle );
			kPrinterA.ClearList();
			kPrinterB.ClearList();

			iYPos	+= 17;
		}
		kTitle.PrintFullText( GetDerivedPosX() + 99, GetDerivedPosY() + iYPos, TAT_LEFT );*/

		ioComplexStringPrinter kTitle;
		int	iYPos	= 41;

		kTitle.SetTextStyle( TS_NORMAL );
		kTitle.SetBkColor( 0, 0, 0 );	
		kTitle.SetTextColor( TCT_DEFAULT_DARKGRAY );

		const StringVector &vTextList = ioStringConverter::Split( m_szTtitle.c_str(), "#" );
		iYPos	-= ( vTextList.size() - 1 ) * 8;

		StringVector::const_iterator iter;
		for( iter=vTextList.begin() ; iter!=vTextList.end() ; ++iter )
		{
			kTitle.AddTextPiece( FONT_SIZE_17, "%s", iter->c_str() );
			kTitle.PrintFullText( GetDerivedPosX() + 99, GetDerivedPosY() + iYPos, TAT_LEFT );
			kTitle.ClearList();

			iYPos	+= 19;
		}
	}
}

void DiceToolTip::UpdateWndSize()
{
	SetSize( GetWidth(), 105 );
}

void DiceToolTip::SetInfo( ioHashString szName, ioUIRenderImage* pIcon )
{
	if( !IsShow() )
		ShowWnd();

	m_szTtitle	= szName;
	m_pIcon		= pIcon;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DiceRewardBtn::DiceRewardBtn(void)
{
	m_pRewardIcon	= NULL;
	m_bIsVisited	= false;
}

DiceRewardBtn::~DiceRewardBtn(void)
{
	SAFEDELETE( m_pRewardIcon );
}

void	DiceRewardBtn::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	if( m_pRewardIcon )
	{
		m_pRewardIcon->SetScale( 0.6f, 0.6f );

		if( m_bIsVisited )
			m_pRewardIcon->Render( iXPos + RENDER_OFFSET_X, iYPos + RENDER_OFFSET_Y, UI_RENDER_GRAY, TFO_BILINEAR );
		else
			m_pRewardIcon->Render( iXPos + RENDER_OFFSET_X, iYPos + RENDER_OFFSET_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void	DiceRewardBtn::iwm_mouseover( const ioMouse& mouse )
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	if( pDiceGame->IsSnakeLadderPoint( GetID() - DiceGameWnd::ID_REWARD_SLOT1 + 1 ) )
		return;

	DiceGameWnd *pWnd = dynamic_cast<DiceGameWnd*>( GetParent() );
	if( pWnd )
	{
		if( pWnd->GetStatus() <= DiceGameWnd::DICE_GAME_STATUS_ROLLING )
			return;

		DiceToolTip *pChild = dynamic_cast<DiceToolTip*>( pWnd->FindChildWnd(DiceGameWnd::ID_TOOLTIP) );
		if( pChild )
		{
			pChild->SetInfo( m_szName, m_pRewardIcon );
		}
	}
}

void	DiceRewardBtn::iwm_mouseleave( const ioMouse& mouse )
{
	ioWnd *pWnd = GetParent();
	if( pWnd )
	{
		pWnd->HideChildWnd( DiceGameWnd::ID_TOOLTIP );
	}
}

void	DiceRewardBtn::SetRewardInfo( char* szName, char* szIconName )
{
	m_szName	= szName;

	ioHashString	szIconString	= szIconName;
	SAFEDELETE( m_pRewardIcon );
	m_pRewardIcon = g_UIImageSetMgr.CreateImageByFullName( szIconString );
}

void	DiceRewardBtn::ResetRewardInfo()
{
	m_szName.Clear();
	SAFEDELETE( m_pRewardIcon );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DiceItemEffect::DiceItemEffect(void)
{
	m_pEffectImage	= NULL;

	m_fEffectScale	= 1.0f;
}

DiceItemEffect::~DiceItemEffect(void)
{

}

void	DiceItemEffect::OnProcess( float fTimePerSec )
{
	DWORD	dwPassedTime	= REALGETTIME() - m_dwEffectStartTime;

	if( dwPassedTime <= m_dwEffectPlayTime / 2 )
	{
		m_fEffectScale	= m_fEffectScaleMin + ( ( ( m_fEffectScaleMax - m_fEffectScaleMin ) / (float)( m_dwEffectPlayTime / 2 ) ) * dwPassedTime );
	}
	else if( dwPassedTime > m_dwEffectPlayTime / 2 && dwPassedTime <= m_dwEffectPlayTime )
	{
		m_fEffectScale	= m_fEffectScaleMax - ( ( ( m_fEffectScaleMax - m_fEffectScaleMin ) / (float)( m_dwEffectPlayTime / 2 ) ) * ( dwPassedTime - ( m_dwEffectPlayTime / 2 ) ) );
	}
	else
	{
		HideWnd();
	}
}

void	DiceItemEffect::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwEffectPlayTime = xElement.GetIntAttribute_e( "Effect_Time" );

	m_fEffectScaleMin = xElement.GetFloatAttribute_e( "Effect_Scale_Min" );
	m_fEffectScaleMax = xElement.GetFloatAttribute_e( "Effect_Sclae_Max" );
}

void	DiceItemEffect::iwm_show()
{
	m_dwEffectStartTime	= REALGETTIME();
	m_fEffectScale	= m_fEffectScaleMin;
}

void	DiceItemEffect::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pEffectImage )
	{
		m_pEffectImage->SetScale( m_fEffectScale );
		m_pEffectImage->Render( iXPos + DiceRewardBtn::RENDER_OFFSET_X, iYPos + DiceRewardBtn::RENDER_OFFSET_X, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void	DiceItemEffect::SetImage( ioUIRenderImage* pImage )
{
	m_pEffectImage	= pImage;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DiceDevK::DiceDevK(void)
{
	for( unsigned int i = 0; i < m_DevKEffectList.size(); i++ )
	{
		m_DevKEffectList[i]	= NULL;
	}
	m_pDevKEffectImage	= NULL;
}

DiceDevK::~DiceDevK(void)
{
	for( unsigned int i = 0; i < m_DevKEffectList.size(); i++ )
	{
		SAFEDELETE( m_DevKEffectList[i] );
	}
}

void	DiceDevK::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwDevKEffectTime = xElement.GetIntAttribute_e( "DevK_Effect_Gap_Time" );
	m_DevKEffectList.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_DevK_Effect" );
	m_DevKEffectList.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]	= "";
		StringCbPrintf_e( szName, sizeof( szName ), "DevK_Effect_%d", i + 1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		ioUIRenderImage*	pImage	= g_UIImageSetMgr.CreateImageByFullName( szEffect );
		if( pImage )
			m_DevKEffectList.push_back( pImage );
	}
}

void	DiceDevK::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pDevKEffectImage )
		m_pDevKEffectImage->Render( iXPos, iYPos );
}

void	DiceDevK::DevKEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_DevKEffectList.size();
	if( iEffectListCnt == 0 )
		return;

	if( m_dwDevKEffectCheckTime == 0 || dwCurTime - m_dwDevKEffectCheckTime > m_dwDevKEffectTime )		// ±³Ã¼
	{
		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		if( m_DevKEffectList[m_iCurEffectArray] )
			m_pDevKEffectImage	= m_DevKEffectList[m_iCurEffectArray];
		else
			m_pDevKEffectImage	= NULL;

		m_dwDevKEffectCheckTime = dwCurTime;
	}
}

void	DiceDevK::ResetDevKEffect()
{
	if( m_iCurEffectArray == -1 )
		return;
	
	m_iCurEffectArray	= -1;	

	if( m_DevKEffectList[0] )
		m_pDevKEffectImage	= m_DevKEffectList[0];
	else
		m_pDevKEffectImage	= NULL;
}

void	DiceDevK::Reset()
{
	m_dwDevKEffectCheckTime	= 0;
	m_iCurEffectArray	= -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DiceDisabledBack::DiceDisabledBack(void)
{
	m_pGameDisbled	= NULL;
	m_pSnakeIcon	= NULL;
	m_pGameEnd		= NULL;
	m_pIconBack		= NULL;

	m_fSizeScaleFactor	= 100.0f;
	m_iAlphaScaleFactor	= 255;
}

DiceDisabledBack::~DiceDisabledBack(void)
{
	SAFEDELETE( m_pGameDisbled );
	SAFEDELETE( m_pSnakeIcon );
	SAFEDELETE( m_pGameEnd );
	SAFEDELETE( m_pIconBack );
}

void	DiceDisabledBack::Reset()
{
	m_fImageTextSize	= PROCESS_IMAGE_TEXT_SIZE_1 / m_fSizeScaleFactor;
	m_iDescriptTextAlpha	= PROCESS_DESCRIPT_TEXT_ALPHA_1 / m_fSizeScaleFactor * m_iAlphaScaleFactor;
	m_fBackImageSize	= PROCESS_BACK_IMAGE_SIZE_1 / m_fSizeScaleFactor;
	m_iBackImageAlpha	= PROCESS_BACK_IMAGE_ALPHA_1 / m_fSizeScaleFactor * m_iAlphaScaleFactor;
	m_fIconImageSize	= PROCESS_ICON_IMAGE_SIZE_1 / m_fSizeScaleFactor;
	m_iButtonAlpha		= PROCESS_BUTTON_ALPHA_1 / m_fSizeScaleFactor * m_iAlphaScaleFactor;

	m_dwOpenedTime	= REALGETTIME();
}

void	DiceDisabledBack::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "GameDisabled" )
	{
		SAFEDELETE( m_pGameDisbled );
		m_pGameDisbled	= pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void	DiceDisabledBack::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SnakeIcon" )
	{
		SAFEDELETE( m_pSnakeIcon );
		m_pSnakeIcon	= pImage;
	}
	else if( szType == "GameEnd" )
	{
		SAFEDELETE( m_pGameEnd );
		m_pGameEnd	= pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack	= pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void	DiceDisabledBack::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD	dwPassedTime	= REALGETTIME() - m_dwOpenedTime;

	auto CalcProcess = [] ( int iStart, int iEnd, int iTotalTime, int iPassedTime, float fScaleFactor, int iAlphaFactor )
	{
		return	( iStart + ( ( ( iEnd - iStart ) / (float)iTotalTime ) * iPassedTime ) ) / fScaleFactor * iAlphaFactor;
	};

	if( dwPassedTime >= ANIMATION_TIME_IMAGE_TEXT_SIZE )
		m_fImageTextSize	= PROCESS_IMAGE_TEXT_SIZE_2 / m_fSizeScaleFactor;
	else
		m_fImageTextSize	= CalcProcess( PROCESS_IMAGE_TEXT_SIZE_1, PROCESS_IMAGE_TEXT_SIZE_2, ANIMATION_TIME_IMAGE_TEXT_SIZE, dwPassedTime, m_fSizeScaleFactor, 1 );

	if( dwPassedTime >= ANIMATION_TIME_DESCRIPT_TEXT_ALPHA )
		m_iDescriptTextAlpha	= PROCESS_DESCRIPT_TEXT_ALPHA_2 / m_fSizeScaleFactor * m_iAlphaScaleFactor;
	else
		m_iDescriptTextAlpha	= CalcProcess( PROCESS_DESCRIPT_TEXT_ALPHA_1, PROCESS_DESCRIPT_TEXT_ALPHA_2, ANIMATION_TIME_DESCRIPT_TEXT_ALPHA, dwPassedTime, m_fSizeScaleFactor, m_iAlphaScaleFactor );

	if( dwPassedTime >= ANIMATION_TIME_BACK_IMAGE_SIZE_2 )
		m_fBackImageSize	= PROCESS_BACK_IMAGE_SIZE_3 / m_fSizeScaleFactor;
	else if( dwPassedTime >= ANIMATION_TIME_BACK_IMAGE_SIZE_1 )
		m_fBackImageSize	= CalcProcess( PROCESS_BACK_IMAGE_SIZE_2, PROCESS_BACK_IMAGE_SIZE_3, ANIMATION_TIME_BACK_IMAGE_SIZE_2 - ANIMATION_TIME_BACK_IMAGE_SIZE_1, dwPassedTime - ANIMATION_TIME_BACK_IMAGE_SIZE_1, m_fSizeScaleFactor, 1 );
	else
		m_fBackImageSize	= CalcProcess( PROCESS_BACK_IMAGE_SIZE_1, PROCESS_BACK_IMAGE_SIZE_2, ANIMATION_TIME_BACK_IMAGE_SIZE_1, dwPassedTime, m_fSizeScaleFactor, 1 );

	if( dwPassedTime >= ANIMATION_TIME_BACK_IMAGE_ALPHA )
		m_iBackImageAlpha	= PROCESS_BACK_IMAGE_ALPHA_2 / m_fSizeScaleFactor * m_iAlphaScaleFactor;
	else
		m_iBackImageAlpha	= CalcProcess( PROCESS_BACK_IMAGE_ALPHA_1, PROCESS_BACK_IMAGE_ALPHA_2, ANIMATION_TIME_BACK_IMAGE_ALPHA, dwPassedTime, m_fSizeScaleFactor, m_iAlphaScaleFactor );

	if( dwPassedTime >= ANIMATION_TIME_ICON_IMAGE_SIZE )
		m_fIconImageSize	= PROCESS_ICON_IMAGE_SIZE_2 / m_fSizeScaleFactor;
	else
		m_fIconImageSize	= CalcProcess( PROCESS_ICON_IMAGE_SIZE_1, PROCESS_ICON_IMAGE_SIZE_2, ANIMATION_TIME_ICON_IMAGE_SIZE, dwPassedTime, m_fSizeScaleFactor, 1 );

	if( dwPassedTime >= ANIMATION_TIME_BUTTON_ALPHA_2 )
		m_iButtonAlpha	= PROCESS_BUTTON_ALPHA_3 / m_fSizeScaleFactor * m_iAlphaScaleFactor;
	else if( dwPassedTime >= ANIMATION_TIME_BUTTON_ALPHA_1 )
		m_iButtonAlpha	= CalcProcess( PROCESS_BUTTON_ALPHA_2, PROCESS_BUTTON_ALPHA_3, ANIMATION_TIME_BUTTON_ALPHA_2 - ANIMATION_TIME_BUTTON_ALPHA_1, dwPassedTime - ANIMATION_TIME_BUTTON_ALPHA_1, m_fSizeScaleFactor, m_iAlphaScaleFactor );

	IoUIText*	pText	= GetText( "GameEndInfo" );
	if( pText )
		pText->SetAlpha( m_iDescriptTextAlpha );

	ioButton*	pBtn	= dynamic_cast<ioButton*>( FindChildWnd( ID_RESTART_GAME ) );
	if( pBtn )
		pBtn->SetWindowAlpha( m_iButtonAlpha );
}

void	DiceDisabledBack::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_RESTART_GAME:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
				if( pDiceGame )
				{
					pDiceGame->SendRestartGame();
				}
			}
		}
		break;
	}
}

void	DiceDisabledBack::iwm_show()
{
	Reset();
}

void	DiceDisabledBack::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pGameDisbled )
	{
		m_pGameDisbled->SetSize( 754, 506 );
		m_pGameDisbled->Render( iXPos, iYPos, UI_RENDER_NORMAL );
	}

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( m_fBackImageSize );
		m_pIconBack->Render( iXPos + 377, iYPos + 246, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_pSnakeIcon )
	{
		m_pSnakeIcon->SetScale( m_fIconImageSize );
		m_pSnakeIcon->Render( iXPos + 377, iYPos + 246, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	if( m_pGameEnd )
	{
		m_pGameEnd->SetScale( m_fImageTextSize );
		m_pGameEnd->Render( iXPos + 375, iYPos + 122, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DiceGameWnd::DiceGameWnd(void)
{
	m_pDiceCountNum	= NULL;
	m_pDiceCount	= NULL;
	m_pGameDisbled	= NULL;
	m_BoardBackList.clear();

	for( int i = 0; i < DICE_EFFECT_MAX; i++ )
	{
		m_pDiceEffect[i] = NULL;
	}

	m_vCharPos = D3DXVECTOR3( 0.0f, 0.0f, FLOAT500 );
	m_vCameraPos = D3DXVECTOR3( 0.0f, 0.0f, FLOAT500 );

	m_iStartPosition	= 0;
	m_iEndPosition		= 0;
	m_iJumpPosition		= 0;
	m_dwPlayerSpeed		= 0;
	m_dwPlayerSnakeLadderSpeed	= 0;
	m_dwRandomRewardStartTime	= 0;
	m_fPlayerPieceXPos	= 0.0f;
	m_fPlayerPieceYPos	= 0.0f;
	m_LineDirectionList.clear();

	m_dwPrevMoveTime	= 0;
	m_dwPrevJumpTime	= 0;
}


DiceGameWnd::~DiceGameWnd(void)
{
	SAFEDELETE( m_pDiceCountNum );
	SAFEDELETE( m_pDiceCount );
	SAFEDELETE( m_pGameDisbled );

	for( DWORD i = 0; i < m_BoardBackList.size(); i++ )
	{
		SAFEDELETE( m_BoardBackList[i] );
	}
	m_BoardBackList.clear();

	m_LineDirectionList.clear();
}

void	DiceGameWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "GameDisabled" )
	{
		SAFEDELETE( m_pGameDisbled );
		m_pGameDisbled	= pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void	DiceGameWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	char szBuff[MAX_PATH];
	strcpy_s( szBuff, "BoardBack" );

	if( szType == "DiceCountNum" )
	{
		SAFEDELETE( m_pDiceCountNum );
		m_pDiceCountNum	= pImage;
	}
	else if( szType == "DiceCount" )
	{
		SAFEDELETE( m_pDiceCount );
		m_pDiceCount	= pImage;
	}
	else if( !strncmp( szBuff, szType.c_str(), 9 ) )
	{
		m_BoardBackList.push_back( pImage );
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void	DiceGameWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_iStatus == DICE_GAME_STATUS_ROLLING && REALGETTIME() - m_dwRandomRewardStartTime < m_dwRandomRewardShowTime && REALGETTIME() - m_dwRandomRewardPrevTime < TIME_RANDOM_REWARD_SHOW )
	{
		SetRandomReward();

		m_dwRandomRewardPrevTime	= REALGETTIME();
	}
	else if( m_iStatus == DICE_GAME_STATUS_ROLLING )
	{
		SetReward();

		m_iStatus = DICE_GAME_STATUS_PLAYING;
	}

	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;
	
	if( m_iStatus == DICE_GAME_STATUS_PLAYING )
	{
		float	fMoveForceX	= ( REALGETTIME() - m_dwPrevMoveTime ) * m_fForceX;
		float	fMoveForceY	= ( REALGETTIME() - m_dwPrevMoveTime ) * m_fForceY;

		if( m_iCurrentPosition != m_iEndPosition )
		{
			if( MoveToNextPosition( fMoveForceX, fMoveForceY ) )
			{
				m_iCurrentPosition++;
				m_dwPrevMoveTime	= REALGETTIME();

				if( m_iCurrentPosition == m_iEndPosition )
					m_dwPrevJumpTime	= REALGETTIME();
			}
		}
		else if( MoveToSnakeLadderPosition( m_iEndPosition, m_iJumpPosition ) )
		{
			m_iStatus	= DICE_GAME_STATUS_ITEM_EFFECT;

			DiceRewardBtn*	pBtn	= dynamic_cast<DiceRewardBtn*>( FindChildWnd( ID_REWARD_SLOT1 + m_iEndPosition - 1 ) );
			DiceItemEffect*	pEffectBtn	= dynamic_cast<DiceItemEffect*>( FindChildWnd( ID_ITEM_EFFECT ) );
			if( pBtn )
			{
				if( pEffectBtn && !pBtn->IsVisited() && pBtn->GetIcon() )
				{
					pEffectBtn->SetWndPos( pBtn->GetXPos(), pBtn->GetYPos() );
					pEffectBtn->SetImage( pBtn->GetIcon() );
					pEffectBtn->ShowWnd();
					g_SoundMgr.PlaySound( m_szGetItemSound );
				}

				pBtn->SetVisited( true );
			}

			DiceDevK*	pDevK	= dynamic_cast<DiceDevK*>( FindChildWnd( ID_DEV_K ) );
			if( pDevK )
				pDevK->ResetDevKEffect();
		}
		
		DiceDevK*	pDevK	= dynamic_cast<DiceDevK*>( FindChildWnd( ID_DEV_K ) );
		if( pDevK )
			pDevK->DevKEffect( fTimePerSec );

		if( m_iCurrentPosition == ioDiceGame::MAX_SLOT )
			m_iStatus	= DICE_GAME_STATUS_RESULT;
	}
	else if( m_iStatus == DICE_GAME_STATUS_IDLE )
	{
		/*DiceDevK*	pDevK	= dynamic_cast<DiceDevK*>( FindChildWnd( ID_DEV_K ) );
		if( pDevK )
			pDevK->ResetDevKEffect();*/
	}
	else if( m_iStatus == DICE_GAME_STATUS_DICE_ROLL )
	{
		if( REALGETTIME() - m_dwDiceRollingStartTime > m_dwDiceRollingTime )
		{
			m_iStatus	= DICE_GAME_STATUS_PLAYING;
			m_dwPrevMoveTime	= REALGETTIME();

			if( m_pDiceEffect[m_iDiceEffectNumber] )
				m_pDiceEffect[m_iDiceEffectNumber]->Stop();

			if( m_pDiceEffect[DICE_EFFECT_IDLE] )
			{
				m_pDiceEffect[DICE_EFFECT_IDLE]->Start();
				m_pDiceEffect[DICE_EFFECT_IDLE]->SetPosX( GetWidth() / 2 );
				m_pDiceEffect[DICE_EFFECT_IDLE]->SetPosY( GetHeight() / 2 );
			}
		}
	}
	else if( m_iStatus == DICE_GAME_STATUS_ITEM_EFFECT )
	{
		DiceItemEffect*	pEffectBtn	= dynamic_cast<DiceItemEffect*>( FindChildWnd( ID_ITEM_EFFECT ) );
		if( pEffectBtn )
		{
			if( !pEffectBtn->IsShow() )
				m_iStatus	= DICE_GAME_STATUS_IDLE;
		}
	}
	else if( m_iStatus == DICE_GAME_STATUS_RESULT )
	{
		if( m_pDiceEffect[m_iDiceEffectNumber] )
			m_pDiceEffect[m_iDiceEffectNumber]->Stop();

		if( m_pDiceEffect[DICE_EFFECT_IDLE] )
			m_pDiceEffect[DICE_EFFECT_IDLE]->Stop();

		if( !FindChildWnd( ID_DISABLED_BACK )->IsShow() )
		{
			ShowChildWnd( ID_DISABLED_BACK );
			g_SoundMgr.PlaySound( m_szResultSound );
		}
	}

	DiceDevK*	pDevK	= dynamic_cast<DiceDevK*>( FindChildWnd( ID_DEV_K ) );
	if( pDevK )
		pDevK->SetWndPos( (int)m_fPlayerPieceXPos, (int)m_fPlayerPieceYPos );
}

void	DiceGameWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szBuff[MAX_PATH];
	LineDirection	lineDirection;

	m_dwPlayerSpeed	= xElement.GetIntAttribute_e( "PlayerPieceSpeed" );
	m_dwPlayerSnakeLadderSpeed	= xElement.GetIntAttribute_e( "PlayerSnakeLadderSpeed" );
	m_dwRandomRewardShowTime	= xElement.GetIntAttribute_e( "RandomRewardShowTime" );
	m_dwDiceRollingTime	= xElement.GetIntAttribute_e( "DiceRollingTime" );

	int	iCount	= xElement.GetIntAttribute_e( "LeftLineCount" );
	for( int i = 0; i < iCount; i++ )
	{
		sprintf_s( szBuff, "LeftLineStart%d", i + 1 );
		lineDirection.m_iLineStart	= xElement.GetIntAttribute( szBuff );
		sprintf_s( szBuff, "LeftLineEnd%d", i + 1 );
		lineDirection.m_iLineEnd	= xElement.GetIntAttribute( szBuff );
		sprintf_s( szBuff, "LeftLineCount%d", i + 1 );
		lineDirection.m_iLineCount	= xElement.GetIntAttribute( szBuff );
		lineDirection.m_iDirection	= LINE_DIRECTION_LEFT;
		m_LineDirectionList.push_back( lineDirection );
	}

	iCount	= xElement.GetIntAttribute_e( "RightLineCount" );
	for( int i = 0; i < iCount; i++ )
	{
		sprintf_s( szBuff, "RightLineStart%d", i + 1 );
		lineDirection.m_iLineStart	= xElement.GetIntAttribute( szBuff );
		sprintf_s( szBuff, "RightLineEnd%d", i + 1 );
		lineDirection.m_iLineEnd	= xElement.GetIntAttribute( szBuff );
		sprintf_s( szBuff, "RightLineCount%d", i + 1 );
		lineDirection.m_iLineCount	= xElement.GetIntAttribute( szBuff );
		lineDirection.m_iDirection	= LINE_DIRECTION_RIGHT;
		m_LineDirectionList.push_back( lineDirection );
	}

	m_iStartPosX	= xElement.GetIntAttribute_e( "StartPosX" );
	m_iStartPosY	= xElement.GetIntAttribute_e( "StartPosY" );
	m_iWidthTerm	= xElement.GetIntAttribute_e( "WidthTerm" );
	m_iHeightTerm	= xElement.GetIntAttribute_e( "HeightTerm" );

	m_szText[TEXT_CHANGE_REWARD]	= xElement.GetStringAttribute_e( "ChangeReward" );
	m_szText[TEXT_CHANGE_BOARD]	= xElement.GetStringAttribute_e( "ChangeBoard" );
	m_szText[TEXT_ERROR_DICE]	= xElement.GetStringAttribute_e( "ErrorDice" );
	m_szText[TEXT_ERROR_REWARD]	= xElement.GetStringAttribute_e( "ErrorReward" );
	m_szText[TEXT_ERROR_BOARD]	= xElement.GetStringAttribute_e( "ErrorBoard" );

	m_szRollingSound	= xElement.GetStringAttribute_e( "RollingSound" );
	m_szResultSound	= xElement.GetStringAttribute_e( "ResultSound" );
	m_szGetItemSound	= xElement.GetStringAttribute_e( "GetItemSound" );
}

void	DiceGameWnd::iwm_create()
{
	if( !m_pDiceEffect[DICE_EFFECT_IDLE] )
		m_pDiceEffect[DICE_EFFECT_IDLE]	= GetEffect( "DiceEffectIdle" );

	char	szEffectName[MAX_PATH];
	for( int i = 1; i < DICE_EFFECT_MAX; i++ )
	{
		sprintf_s( szEffectName, "DiceEffect%d", i );

		if( !m_pDiceEffect[i] )
			m_pDiceEffect[i]	= GetEffect( szEffectName );
	}
}

void	DiceGameWnd::iwm_show()
{
	Reset();
}

void	DiceGameWnd::iwm_hide()
{
	if( m_pDiceEffect[m_iDiceEffectNumber] )
		m_pDiceEffect[m_iDiceEffectNumber]->Stop();

	if( m_pDiceEffect[DICE_EFFECT_IDLE] )
		m_pDiceEffect[DICE_EFFECT_IDLE]->Stop();

	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	pDiceGame->SendGameEnd();

	HideChildWnd( ID_DISABLED_BACK );
}

void	DiceGameWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	case ID_DICE_ROLL:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( m_iStatus == DICE_GAME_STATUS_IDLE )
				{
					ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
					if( pDiceGame && m_iStatus != DICE_GAME_STATUS_DICE_ROLL )
					{
						pDiceGame->SendDiceRoll();
					}
				}
			}
		}
		break;
	case ID_REWARD_CHANGE:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( m_iStatus == DICE_GAME_STATUS_IDLE )
				{
					ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
					if( pDiceGame )
					{
						pDiceGame->SendRewardChange();
					}
				}
			}
		}
		break;
	case ID_BOARD_CHANGE:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( m_iStatus == DICE_GAME_STATUS_IDLE )
				{
					ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
					if( pDiceGame )
					{
						pDiceGame->SendBoardChange();
					}
				}
			}
		}
		break;
	}
}

bool	DiceGameWnd::iwm_esc()
{
	HideWnd();
	return	true;
}

void	DiceGameWnd::iwm_destroy()
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	if( IsShow() )
		pDiceGame->SendGameEnd();
}

void	DiceGameWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();

	if( !pDiceGame )
		return;

	BYTE	byIndex	= pDiceGame->GetBoardIndex();
	if( byIndex <= m_BoardBackList.size() )
		m_BoardBackList[byIndex - 1]->Render( iXPos + 10, iYPos + 44 );

	int	iDiceCount	= pDiceGame->GetDiceCount();

	m_pDiceCount->SetScale( 0.6f, 0.6f );

	if( iDiceCount >= 100 )
	{
		m_pDiceCountNum->RenderNum( iXPos + 565, iYPos + 114, iDiceCount, 0.0f, 0.6f, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pDiceCount->Render( iXPos + 598, iYPos + 102, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( iDiceCount >= 10 )
	{
		m_pDiceCountNum->RenderNum( iXPos + 574, iYPos + 114, iDiceCount, 0.0f, 0.6f, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pDiceCount->Render( iXPos + 586, iYPos + 102, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		m_pDiceCountNum->RenderNum( iXPos + 585, iYPos + 114, iDiceCount, 0.0f, 0.6f, UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pDiceCount->Render( iXPos + 576, iYPos + 102, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void	DiceGameWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
}

bool	DiceGameWnd::MoveToNextPosition( float fForceX, float fForceY )
{
	int iDirection	= GetDirection( m_iCurrentPosition );
	JumpPoint	jumpPoint	= GetJumpPosition( m_iCurrentPosition + 1 );

	switch( iDirection )
	{
	case LINE_DIRECTION_RIGHT:
		{
			m_fPlayerPieceXPos += fForceX;

			if( (float)jumpPoint.iPositionX - m_fPlayerPieceXPos < -0.5f )
			{
				m_fPlayerPieceXPos	= (float)jumpPoint.iPositionX;
				return	true;
			}
		}
		break;

	case LINE_DIRECTION_LEFT:
		{
			m_fPlayerPieceXPos -= fForceX;

			if( (float)jumpPoint.iPositionX - m_fPlayerPieceXPos > 0.5f )
			{
				m_fPlayerPieceXPos	= (float)jumpPoint.iPositionX;
				return	true;
			}
		}
		break;

	case LINE_DIRECTION_UP:
		{
			m_fPlayerPieceYPos -= fForceY;

			if( jumpPoint.iPositionY - m_fPlayerPieceYPos > 0.5f )
			{
				m_fPlayerPieceYPos	= (float)jumpPoint.iPositionY;
				return	true;
			}
		}
		break;
	}

	return	false;
}

int	DiceGameWnd::GetDirection( int iPosition )
{
	int	iDirection	= LINE_DIRECTION_RIGHT;

	for( unsigned int i = 0; i < m_LineDirectionList.size(); i++ )
	{
		LineDirection	lineDirection	= m_LineDirectionList[i];

		if( lineDirection.m_iLineEnd == iPosition )
		{
			iDirection	= LINE_DIRECTION_UP;
		}
		else if( lineDirection.m_iLineStart <= iPosition && lineDirection.m_iLineEnd > iPosition )
		{
			iDirection	= lineDirection.m_iDirection;
		}
	}

	return	iDirection;
}

void	DiceGameWnd::Reset()
{
	m_fForceX	= m_iWidthTerm / (float)m_dwPlayerSpeed;
	m_fForceY	= m_iHeightTerm / (float)m_dwPlayerSpeed;

	m_iStatus	= DICE_GAME_STATUS_ROLLING;

	m_dwRandomRewardPrevTime	= m_dwRandomRewardStartTime	= REALGETTIME();

	m_iDiceEffectNumber	= DICE_EFFECT_1;

	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	m_iCurrentPosition	= m_iEndPosition	= pDiceGame->GetCurrentPosition();

	JumpPoint	jumpPoint	= GetJumpPosition( m_iCurrentPosition );
	m_fPlayerPieceXPos	= (float)(jumpPoint.iPositionX);
	m_fPlayerPieceYPos	= (float)(jumpPoint.iPositionY);

	HideChildWnd( ID_DISABLED_BACK );

	for( int i = ID_REWARD_SLOT1; i <= ID_REWARD_SLOT87; i++ )
	{
		DiceRewardBtn*	pBtn	= dynamic_cast<DiceRewardBtn*>( FindChildWnd( i ) );
		if( pBtn )
			pBtn->SetVisited( pDiceGame->GetTraceInfo( i - ID_REWARD_SLOT1 + 1 ) );
	}

	DiceDevK*	pDevK	= dynamic_cast<DiceDevK*>( FindChildWnd( ID_DEV_K ) );
	if( pDevK )
		pDevK->Reset();

	if( m_pDiceEffect[DICE_EFFECT_IDLE] )
	{
		m_pDiceEffect[DICE_EFFECT_IDLE]->Start();
		m_pDiceEffect[DICE_EFFECT_IDLE]->SetPosX( GetWidth() / 2 );
		m_pDiceEffect[DICE_EFFECT_IDLE]->SetPosY( GetHeight() / 2 );
	}

	SetRewardChangeItemCount();
	SetBoardChangeItemCount();
}

void	DiceGameWnd::SetRewardBtn( int iIndex, char* szName, char* szIconName )
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	DiceRewardBtn*	pBtn	= dynamic_cast<DiceRewardBtn*>( FindChildWnd( ID_REWARD_SLOT1 + iIndex ) );
	if( pBtn )
	{
		if( pDiceGame->IsSnakeLadderPoint( iIndex + 1 ) )
			pBtn->ResetRewardInfo();
		else
			pBtn->SetRewardInfo( szName, szIconName );
	}
}

void	DiceGameWnd::SetDiceRoll( int iDice, int iMidPosition, int iEndPosition )
{
	m_iStatus	= DICE_GAME_STATUS_DICE_ROLL;

	m_iEndPosition	= iMidPosition;
	m_iJumpPosition	= iEndPosition;

	m_dwDiceRollingStartTime	= REALGETTIME();

	m_iDiceEffectNumber	= iDice;

	if( m_pDiceEffect[DICE_EFFECT_IDLE] )
		m_pDiceEffect[DICE_EFFECT_IDLE]->Stop();

	if( m_pDiceEffect[m_iDiceEffectNumber] )
	{
		m_pDiceEffect[m_iDiceEffectNumber]->Start();
		m_pDiceEffect[m_iDiceEffectNumber]->SetPosX( GetWidth() / 2 );
		m_pDiceEffect[m_iDiceEffectNumber]->SetPosY( GetHeight() / 2 );
	}

	g_SoundMgr.PlaySound( m_szRollingSound );
}

void	DiceGameWnd::ChangeRewardList()
{
	m_iStatus	= DICE_GAME_STATUS_ROLLING;
	m_dwRandomRewardPrevTime	= m_dwRandomRewardStartTime	= REALGETTIME();
}

void	DiceGameWnd::ChangeBoard()
{
	m_iStatus	= DICE_GAME_STATUS_ROLLING;
	m_dwRandomRewardPrevTime	= m_dwRandomRewardStartTime	= REALGETTIME();
}

void	DiceGameWnd::SetRewardChangeItemCount()
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	char	szText[MAX_PATH];
	ioButton*	pButton	= dynamic_cast<ioButton*>( FindChildWnd( ID_REWARD_CHANGE ) );
	if( pButton )
	{
		ioUITitle*	pTitle	= pButton->GetTitle();
		if( pTitle )
		{
			sprintf_s( szText, m_szText[TEXT_CHANGE_REWARD].c_str(), pDiceGame->GetRewardChangeCount() );
			pTitle->SetText( szText, "" );
		}
	}
}

void	DiceGameWnd::SetBoardChangeItemCount()
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	char	szText[MAX_PATH];
	ioButton*	pButton	= dynamic_cast<ioButton*>( FindChildWnd( ID_BOARD_CHANGE ) );
	if( pButton )
	{
		ioUITitle*	pTitle	= pButton->GetTitle();
		if( pTitle )
		{
			sprintf_s( szText, m_szText[TEXT_CHANGE_BOARD].c_str(), pDiceGame->GetBoardChangeCount() );
			pTitle->SetText( szText, "" );
		}
	}
}

void	DiceGameWnd::SetRandomReward()
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	for( int i = 0; i < ioDiceGame::MAX_REWARD_INDEX ; i++ )
	{
		LSC_SnakeLadders_Reward*	rewardInfo	= pDiceGame->GetRandomReward( false );

		if( i < ioDiceGame::MAX_REWARD_INDEX - 1 )
		{
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ), rewardInfo->Reward1name, rewardInfo->Reward1icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 1, rewardInfo->Reward2name, rewardInfo->Reward2icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 2, rewardInfo->Reward3name, rewardInfo->Reward3icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 3, rewardInfo->Reward4name, rewardInfo->Reward4icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 4, rewardInfo->Reward5name, rewardInfo->Reward5icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 5, rewardInfo->Reward6name, rewardInfo->Reward6icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 6, rewardInfo->Reward7name, rewardInfo->Reward7icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 7, rewardInfo->Reward8name, rewardInfo->Reward8icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 8, rewardInfo->Reward9name, rewardInfo->Reward9icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 9, rewardInfo->Reward10name, rewardInfo->Reward10icon );
		}
		else
		{
			LSC_SnakeLadders_Reward*	finalRewardInfo	= pDiceGame->GetRandomReward( true );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ), rewardInfo->Reward1name, rewardInfo->Reward1icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 1, rewardInfo->Reward2name, rewardInfo->Reward2icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 2, rewardInfo->Reward3name, rewardInfo->Reward3icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 3, rewardInfo->Reward4name, rewardInfo->Reward4icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 4, rewardInfo->Reward5name, rewardInfo->Reward5icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 5, rewardInfo->Reward6name, rewardInfo->Reward6icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 6, finalRewardInfo->Reward1name, finalRewardInfo->Reward1icon );
		}
	}
}

void	DiceGameWnd::SetReward()
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();
	if( !pDiceGame )
		return;

	for( int i = 0; i < ioDiceGame::MAX_REWARD_INDEX ; i++ )
	{
		LSC_SnakeLadders_Reward*	rewardInfo	= pDiceGame->GetReward( i );

		if( i < ioDiceGame::MAX_REWARD_INDEX - 1 )
		{
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ), rewardInfo->Reward1name, rewardInfo->Reward1icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 1, rewardInfo->Reward2name, rewardInfo->Reward2icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 2, rewardInfo->Reward3name, rewardInfo->Reward3icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 3, rewardInfo->Reward4name, rewardInfo->Reward4icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 4, rewardInfo->Reward5name, rewardInfo->Reward5icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 5, rewardInfo->Reward6name, rewardInfo->Reward6icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 6, rewardInfo->Reward7name, rewardInfo->Reward7icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 7, rewardInfo->Reward8name, rewardInfo->Reward8icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 8, rewardInfo->Reward9name, rewardInfo->Reward9icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 9, rewardInfo->Reward10name, rewardInfo->Reward10icon );
		}
		else
		{
			LSC_SnakeLadders_Reward*	finalRewardInfo	= pDiceGame->GetReward( ioDiceGame::MAX_SLOT );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ), rewardInfo->Reward1name, rewardInfo->Reward1icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 1, rewardInfo->Reward2name, rewardInfo->Reward2icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 2, rewardInfo->Reward3name, rewardInfo->Reward3icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 3, rewardInfo->Reward4name, rewardInfo->Reward4icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 4, rewardInfo->Reward5name, rewardInfo->Reward5icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 5, rewardInfo->Reward6name, rewardInfo->Reward6icon );
			SetRewardBtn( ( i * ioDiceGame::MAX_REWARD_INFO_PER_LINE ) + 6, finalRewardInfo->Reward1name, finalRewardInfo->Reward1icon );
		}
	}
}

bool	DiceGameWnd::MoveToSnakeLadderPosition( int iPosition, int iJumpPosition )
{
	ioDiceGame*	pDiceGame	= g_MyInfo.GetDiceGame();

	if( !pDiceGame || pDiceGame->GetSnakeLadderEndPoint( (SHORT)iPosition ) == 0 )
		return	true;

	if( iJumpPosition == 0 )
		return	true;

	JumpPoint	jumpPosition	= GetJumpPosition( iJumpPosition );
	JumpPoint	prevPosition	= GetJumpPosition( iPosition );

	bool	bIsXArrived	= false;
	bool	bIsYArrived	= false;
	INT	iVerticalDirection,	iHorizontalDirection;
	if( prevPosition.iPositionX > jumpPosition.iPositionX )
		iHorizontalDirection	= LADDER_SNAKE_DIRECTION_LEFT;
	else if( prevPosition.iPositionX < jumpPosition.iPositionX )
		iHorizontalDirection	= LADDER_SNAKE_DIRECTION_RIGHT;
	else
		iHorizontalDirection	= LADDER_SNAKE_DIRECTION_NONE;

	if( prevPosition.iPositionY > jumpPosition.iPositionY )
		iVerticalDirection	= LADDER_SNAKE_DIRECTION_UP;
	else if( prevPosition.iPositionY < jumpPosition.iPositionY )
		iVerticalDirection	= LADDER_SNAKE_DIRECTION_DOWN;
	else
		iVerticalDirection	= LADDER_SNAKE_DIRECTION_NONE;

	switch( iHorizontalDirection )
	{
	case LADDER_SNAKE_DIRECTION_LEFT:
		{
			if( m_fPlayerPieceXPos < (float)jumpPosition.iPositionX )
				bIsXArrived	= true;
		}
		break;

	case LADDER_SNAKE_DIRECTION_RIGHT:
		{
			if( m_fPlayerPieceXPos > (float)jumpPosition.iPositionX )
				bIsXArrived	= true;
		}
		break;

	case LADDER_SNAKE_DIRECTION_NONE:
		{
			bIsXArrived	= true;
		}
		break;
	}

	switch( iVerticalDirection )
	{
	case LADDER_SNAKE_DIRECTION_UP:
		{
			if( m_fPlayerPieceYPos < (float)jumpPosition.iPositionY )
				bIsYArrived	= true;
		}
		break;

	case LADDER_SNAKE_DIRECTION_DOWN:
		{
			if( m_fPlayerPieceYPos > (float)jumpPosition.iPositionY )
				bIsYArrived	= true;
		}
		break;

	case LADDER_SNAKE_DIRECTION_NONE:
		{
			bIsYArrived	= true;
		}
		break;
	}

	if( !bIsXArrived || !bIsYArrived )
	{
		float	fRatio	= ( REALGETTIME() - m_dwPrevJumpTime ) / (float)m_dwPlayerSnakeLadderSpeed;
		float	fDistX	= jumpPosition.iPositionX - prevPosition.iPositionX;
		float	fDistY	= jumpPosition.iPositionY - prevPosition.iPositionY;

		m_fPlayerPieceXPos += fDistX * fRatio;
		m_fPlayerPieceYPos += fDistY * fRatio;
	}
	else
	{
		m_fPlayerPieceXPos = (float)jumpPosition.iPositionX;
		m_fPlayerPieceYPos = (float)jumpPosition.iPositionY;

		m_iCurrentPosition	= m_iEndPosition	= iJumpPosition;
		
		return	true;
	}

	m_dwPrevJumpTime	= REALGETTIME();

	return	false;
}

DiceGameWnd::JumpPoint	DiceGameWnd::GetJumpPosition( SHORT sPosition )
{
	JumpPoint	jumpPosition;

	for( unsigned int i = 0; i < m_LineDirectionList.size(); i++ )
	{
		LineDirection	lineDirection	= m_LineDirectionList[i];

		if( lineDirection.m_iLineStart <= sPosition && lineDirection.m_iLineEnd >= sPosition )
		{
			jumpPosition.iPositionY	= m_iStartPosY - (( lineDirection.m_iLineCount - 1 ) * m_iHeightTerm );

			if( lineDirection.m_iDirection == LINE_DIRECTION_RIGHT )
				jumpPosition.iPositionX	= m_iStartPosX + ( ( sPosition - lineDirection.m_iLineStart ) * m_iWidthTerm );
			else if( lineDirection.m_iDirection == LINE_DIRECTION_LEFT )
				jumpPosition.iPositionX	= m_iStartPosX + ( ( lineDirection.m_iLineEnd - sPosition ) * m_iWidthTerm );

			break;
		}
	}

	return	jumpPosition;
}

void	DiceGameWnd::SetErrorMsg( int iType )
{
	switch( iType )
	{
	case	DICE_GAME_ERROR_DICE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szText[TEXT_ERROR_DICE].c_str() );
		}
		break;

	case	DICE_GAME_ERROR_REWARD:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szText[TEXT_ERROR_REWARD].c_str() );
		}
		break;

	case	DICE_GAME_ERROR_BOARD:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szText[TEXT_ERROR_BOARD].c_str() );
		}
		break;
	}
}