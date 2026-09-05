

#include "stdafx.h"

#include "ioWndType.h"
#include "ioWnd.h"
#include "ioButton.h"

#include "ioUIRenderFrame.h"
#include "ioUIRenderImage.h"
#include "ioUIRenderer.h"
#include "ioUITitle.h"

#include "ioFontManager.h"
#include "ioFrameTimer.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"
#include "ioSoundManager.h"
#include "ioUIImage.h"

ioButton::ioButton()
{
	m_pNormalFrm = NULL;
	m_pOverFrm = NULL;
	m_pOverAddFrm = NULL;
	m_pPushFrm = NULL;
	m_pBackFrm = NULL;
	m_pDisableFrm = NULL;
	m_pExDisableNorFrm = NULL;
	m_pExDisablePushFrm = NULL;

	m_pNormal = NULL;
	m_pOver = NULL;
	m_pOverAdd = NULL;
	m_pPush = NULL;
	m_pBack = NULL;
	m_pDisable = NULL;
	m_pExDisableNor = NULL;
	m_pExDisablePush = NULL;

	m_nDisableTextStyleOffsetX = 0;
	m_nDisableTextStyleOffsetY = 0;
	m_bTitleOffsetUse = false;
}

ioButton::~ioButton()
{
	SAFEDELETE( m_pNormalFrm );
	SAFEDELETE( m_pOverFrm );
	SAFEDELETE( m_pOverAddFrm );
	SAFEDELETE( m_pPushFrm );
	SAFEDELETE( m_pBackFrm );
	SAFEDELETE( m_pDisableFrm );
	SAFEDELETE( m_pExDisableNorFrm );
	SAFEDELETE( m_pExDisablePushFrm );

	SAFEDELETE( m_pNormal );
	SAFEDELETE( m_pOver );
	SAFEDELETE( m_pOverAdd );
	SAFEDELETE( m_pPush );
	SAFEDELETE( m_pBack );
	SAFEDELETE( m_pDisable );
	SAFEDELETE( m_pExDisableNor );
	SAFEDELETE( m_pExDisablePush );

	UIImageList::iterator iter = m_NormalList.begin();
	for(;iter != m_NormalList.end();iter++)
		SAFEDELETE( *iter );
	m_NormalList.clear();
	
	for(iter = m_DisableList.begin();iter != m_DisableList.end();iter++)
		SAFEDELETE( *iter );
	m_DisableList.clear();
}

void ioButton::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "Normal" )
	{
		SAFEDELETE( m_pNormalFrm );
		m_pNormalFrm = pFrame;
	}
	else if( szType == "Over" )
	{
		SAFEDELETE( m_pOverFrm );
		m_pOverFrm = pFrame;
	}
	else if( szType == "OverAdd" )
	{
		SAFEDELETE( m_pOverAddFrm );
		m_pOverAddFrm = pFrame;
	}
	else if( szType == "Push" )
	{
		SAFEDELETE( m_pPushFrm );
		m_pPushFrm = pFrame;
	}
	else if( szType == "Back" )
	{
		SAFEDELETE( m_pBackFrm );
		m_pBackFrm = pFrame;
	}
	else if( szType == "Disable" )
	{
		SAFEDELETE( m_pDisableFrm );
		m_pDisableFrm = pFrame;
	}
	else if( szType == "ExDisableNor" )
	{
		SAFEDELETE( m_pExDisableNorFrm );
		m_pExDisableNorFrm = pFrame;
	}
	else if( szType == "ExDisablePush" )
	{
		SAFEDELETE( m_pExDisablePushFrm );
		m_pExDisablePushFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ioButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Normal" )
	{
		SAFEDELETE( m_pNormal );
		m_pNormal = pImage;
	}
	else if( szType == "Over" )
	{
		SAFEDELETE( m_pOver );
		m_pOver = pImage;
	}
	else if( szType == "OverAdd" )
	{
		SAFEDELETE( m_pOverAdd );
		m_pOverAdd = pImage;
	}
	else if( szType == "Push" )
	{
		SAFEDELETE( m_pPush );
		m_pPush = pImage;
	}
	else if( szType == "Back" )
	{
		SAFEDELETE( m_pBack );
		m_pBack = pImage;
	}
	else if( szType == "Disable" )
	{
		SAFEDELETE( m_pDisable );
		m_pDisable = pImage;
	}
	else if( szType == "ExDisableNor" )
	{
		SAFEDELETE( m_pExDisableNor );
		m_pExDisableNor = pImage;
	}
	else if( szType == "ExDisablePush" )
	{
		SAFEDELETE( m_pExDisablePush );
		m_pExDisablePush = pImage;
	}
	else if( szType == "NormalList" )
	{
		m_NormalList.push_back( pImage );
	}
	else if( szType == "DisableList" )
	{
		m_DisableList.push_back( pImage );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioButton::SetWindowAlpha( int iAlpha )
{
	ioWnd::SetWindowAlpha( iAlpha );

	if( m_pNormalFrm )
		m_pNormalFrm->SetAlpha( iAlpha );

	if( m_pOverFrm )
		m_pOverFrm->SetAlpha( iAlpha );

	if( m_pOverAddFrm )
		m_pOverAddFrm->SetAlpha( iAlpha );

	if( m_pPushFrm )
		m_pPushFrm->SetAlpha( iAlpha );

	if( m_pBackFrm )
		m_pBackFrm->SetAlpha( iAlpha );

	if( m_pDisableFrm )
		m_pDisableFrm->SetAlpha( iAlpha );
	
	if( m_pExDisableNorFrm )
		m_pExDisableNorFrm->SetAlpha( iAlpha );

	if( m_pExDisablePushFrm )
		m_pExDisablePushFrm->SetAlpha( iAlpha );

	if( m_pNormal )
		m_pNormal->SetAlpha( iAlpha );

	if( m_pOver )
		m_pOver->SetAlpha( iAlpha );

	if( m_pOverAdd )
		m_pOverAdd->SetAlpha( iAlpha );

	if( m_pPush )
		m_pPush->SetAlpha( iAlpha );

	if( m_pBack )
		m_pBack->SetAlpha( iAlpha );

	if( m_pDisable )
		m_pDisable->SetAlpha( iAlpha );
	
	if( m_pExDisableNor )
		m_pExDisableNor->SetAlpha( iAlpha );

	if( m_pExDisablePush )
		m_pExDisablePush->SetAlpha( iAlpha );

	if( m_pUITitle )
		m_pUITitle->SetAlpha( iAlpha );

	for( UIImageList::iterator iter = m_NormalList.begin(); iter != m_NormalList.end(); ++iter )
	{
		ioUIRenderImage* pImage = *iter;
		if( pImage )
			pImage->SetAlpha( iAlpha );
	}

	for( UIImageList::iterator iter = m_DisableList.begin(); iter != m_DisableList.end(); ++iter )
	{
		ioUIRenderImage* pImage = *iter;
		if( pImage )
			pImage->SetAlpha( iAlpha );
	}
}

const char* ioButton::GetHelp()
{
	return m_szHelp.c_str();
}

void ioButton::SetHelp( const char *szHelp )
{
	m_szHelp = szHelp;
}

void ioButton::PrintNormalTitle( int iXPos, int iYPos )
{
	if( m_pUITitle )
	{
		m_pUITitle->PrintNormal( iXPos, iYPos );
	}
}

void ioButton::PrintOverTitle( int iXPos, int iYPos )
{
	if( m_pUITitle )
	{
		m_pUITitle->PrintOver( iXPos, iYPos );
	}
}

void ioButton::PrintPushTitle( int iXPos, int iYPos )
{
	if( m_pUITitle )
	{
		m_pUITitle->PrintPush( iXPos, iYPos );
	}
}

void ioButton::PrintDisableTitle( int iXPos, int iYPos )
{
	if( m_pUITitle )
	{
		if( m_pUITitle->GetTextStyle() != m_pUITitle->GetDisableTextStyle() )
		{
			if( m_bTitleOffsetUse )
			{
				m_pUITitle->PrintDisable( iXPos + m_nDisableTextStyleOffsetX, iYPos + m_nDisableTextStyleOffsetY );
			}
			else
			{
				if( m_pUITitle->GetDisableTextStyle() == TS_NORMAL && m_pUITitle->GetVertAlign() == TVA_TOP )
					m_pUITitle->PrintDisable( iXPos + 2, iYPos + 2 );	
				else
					m_pUITitle->PrintDisable( iXPos, iYPos );
			}
		}
		else
		{
			m_pUITitle->PrintDisable( iXPos, iYPos );
		}
	}
}

void ioButton::OnDrawBack( int iXPos, int iYPos )
{
	if( m_pBackFrm )
	{
		m_pBackFrm->Render( iXPos, iYPos );
	}

	if( m_pBack )
	{
		m_pBack->Render( iXPos, iYPos );
	}
}

void ioButton::CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight )
{
	ioWnd::CheckFrameReSize( iWidth, iHeight, iPrevWidth, iPrevHeight );

	if( m_pNormalFrm && m_pNormalFrm->IsAutoReSize() )
		m_pNormalFrm->SetSize( iWidth, iHeight );

	if( m_pOverFrm && m_pOverFrm->IsAutoReSize() )
		m_pOverFrm->SetSize( iWidth, iHeight );

	if( m_pOverAddFrm && m_pOverAddFrm->IsAutoReSize() )
		m_pOverAddFrm->SetSize( iWidth, iHeight );
	
	if( m_pPushFrm && m_pPushFrm->IsAutoReSize() )
		m_pPushFrm->SetSize( iWidth, iHeight );
	
	if( m_pBackFrm && m_pBackFrm->IsAutoReSize() )
		m_pBackFrm->SetSize( iWidth, iHeight );

	if( m_pDisableFrm && m_pDisableFrm->IsAutoReSize() )
		m_pDisableFrm->SetSize( iWidth, iHeight );

	if( m_pExDisableNorFrm && m_pExDisableNorFrm->IsAutoReSize() )
		m_pExDisableNorFrm->SetSize( iWidth, iHeight );

	if( m_pExDisablePushFrm && m_pExDisablePushFrm->IsAutoReSize() )
		m_pExDisablePushFrm->SetSize( iWidth, iHeight );
}

void ioButton::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDrawBack( iXPos, iYPos );

	if( IsDisabled() )
	{
		OnDrawDisabled( iXPos, iYPos );
		OnDrawDisableList( iXPos, iYPos );
		PrintDisableTitle( iXPos, iYPos );
	}
	else if( IsExDisabled() )
	{
		if( IsPushed() )
		{
			OnDrawExDisabledPushed( iXPos, iYPos );
		}
		else if( IsOvered() )
		{
			OnDrawExDisabledOvered( iXPos, iYPos );
		}
		else
		{
			OnDrawExDisabledNormal( iXPos, iYPos );
		}
		OnDrawDisableList( iXPos, iYPos );
		PrintDisableTitle( iXPos, iYPos );
	}
	else
	{
		if( IsPushed() )
		{
			OnDrawPushed( iXPos, iYPos );
			PrintPushTitle( iXPos, iYPos );
		}
		else if( IsOvered() )
		{
			OnDrawOvered( iXPos, iYPos );
			PrintOverTitle( iXPos, iYPos );
		}
		else
		{
			OnDrawNormal( iXPos, iYPos );
			PrintNormalTitle( iXPos, iYPos );
		}
		OnDrawNormalList( iXPos, iYPos );
	}

	std::list<IoUIText*>::iterator it = m_pTextList.begin();
	for(; it != m_pTextList.end(); ++it)
	{
		if((*it)->GetVisible())
		{
			(*it)->PrintNormal(iXPos, iYPos);
		}
	}
}

void ioButton::OnDrawNormalList( int iXPos, int iYPos )
{
	UIImageList::iterator iter = m_NormalList.begin();
	for(;iter != m_NormalList.end();iter++)
		(*iter)->Render( iXPos, iYPos );
}

void ioButton::OnDrawDisableList( int iXPos, int iYPos )
{
	UIImageList::iterator iter = m_DisableList.begin();
	for(;iter != m_DisableList.end();iter++)
		(*iter)->Render( iXPos, iYPos );
}

void ioButton::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_pNormalFrm )
	{
		m_pNormalFrm->Render( iXPos, iYPos );
	}

	if( m_pNormal )
	{
		m_pNormal->Render( iXPos, iYPos );
	}

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void ioButton::OnDrawOvered( int iXPos, int iYPos )
{
	if( HasExWndStyle( IWS_EX_OVER_SCREEN ) )
	{
		if( m_pOverFrm )
		{
			m_pOverFrm->Render( iXPos, iYPos );
			m_pOverFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		if( m_pOver )
		{
			m_pOver->Render( iXPos, iYPos );
			m_pOver->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}
		OnDrawOveredAdd( iXPos, iYPos );
		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
		RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
	}
	else
	{
		if( m_pOverFrm )
		{
			m_pOverFrm->Render( iXPos, iYPos );
		}

		if( m_pOver )
		{
			m_pOver->Render( iXPos, iYPos );
		}
		OnDrawOveredAdd( iXPos, iYPos );
		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
}

void ioButton::OnDrawOveredAdd( int iXPos, int iYPos )
{
	if( m_pOverAddFrm )
	{
		m_pOverAddFrm->Render( iXPos, iYPos, UI_RENDER_ADD );
	}

	if( m_pOverAdd )
	{
		m_pOverAdd->Render( iXPos, iYPos, UI_RENDER_ADD );
	}
}

void ioButton::OnDrawPushed( int iXPos, int iYPos )
{
	if( HasExWndStyle( IWS_EX_PUSH_SCREEN ) )
	{
		if( m_pPushFrm )
		{
			m_pPushFrm->Render( iXPos, iYPos );
			m_pPushFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		if( m_pPush )
		{
			m_pPush->Render( iXPos, iYPos );
			m_pPush->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
		RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
	}
	else if( HasExWndStyle( IWS_EX_PUSH_DOUBLE ) )
	{
		if( m_pPushFrm )
		{
			m_pPushFrm->Render( iXPos, iYPos );
			m_pPushFrm->Render( iXPos, iYPos );
		}

		if( m_pPush )
		{
			m_pPush->Render( iXPos, iYPos );
			m_pPush->Render( iXPos, iYPos );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
	else if( HasExWndStyle( IWS_EX_PUSHED_OVER_SCREEN ) )
	{
		if( m_pPushFrm )
		{
			m_pPushFrm->Render( iXPos, iYPos );

			if( m_bOver )
				m_pPushFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		if( m_pPush )
		{
			m_pPush->Render( iXPos, iYPos );

			if( m_bOver )
				m_pPush->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );

		if( m_bOver )
		{
			RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
		}
	}
	else
	{
		if( m_pPushFrm )
		{
			m_pPushFrm->Render( iXPos, iYPos );
		}

		if( m_pPush )
		{
			m_pPush->Render( iXPos, iYPos );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
}

void ioButton::OnDrawDisabled( int iXPos, int iYPos )
{
	if( m_pDisableFrm )
		m_pDisableFrm->Render( iXPos, iYPos );
	else if( m_pNormalFrm )
		m_pNormalFrm->Render( iXPos, iYPos );

	if( m_pDisable )
		m_pDisable->Render( iXPos, iYPos );
	else if( m_pNormal )
		m_pNormal->Render( iXPos, iYPos );

	RenderImageList( iXPos, iYPos, UI_RENDER_GRAY );
}

void ioButton::OnDrawExDisabledNormal( int iXPos, int iYPos )
{
	if( !m_pExDisableNorFrm && !m_pExDisableNor )
	{
		OnDrawNormal( iXPos, iYPos );
	}
	else
	{
		if( m_pExDisableNorFrm )
		{
			m_pExDisableNorFrm->Render( iXPos, iYPos );
		}

		if( m_pExDisableNor )
		{
			m_pExDisableNor->Render( iXPos, iYPos );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
}

void ioButton::OnDrawExDisabledOvered( int iXPos, int iYPos )
{
	if( !m_pExDisableNorFrm && !m_pExDisableNor )
	{
		OnDrawOvered( iXPos, iYPos );
	}
	else if( HasExWndStyle( IWS_EX_OVER_SCREEN ) )
	{
		if( m_pExDisableNorFrm )
		{
			m_pExDisableNorFrm->Render( iXPos, iYPos );
			m_pExDisableNorFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		if( m_pExDisableNor )
		{
			m_pExDisableNor->Render( iXPos, iYPos );
			m_pExDisableNor->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}
		OnDrawOveredAdd( iXPos, iYPos );
		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
		RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
	}
	else
	{
		if( m_pExDisableNorFrm )
		{
			m_pExDisableNorFrm->Render( iXPos, iYPos );
		}

		if( m_pExDisableNor )
		{
			m_pExDisableNor->Render( iXPos, iYPos );
		}
		OnDrawOveredAdd( iXPos, iYPos );
		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
}

void ioButton::OnDrawExDisabledPushed( int iXPos, int iYPos )
{
	if( !m_pExDisablePushFrm && !m_pExDisablePush )
	{
		OnDrawPushed( iXPos, iYPos );
	}
	else if( HasExWndStyle( IWS_EX_PUSH_SCREEN ) )
	{
		if( m_pExDisablePushFrm )
		{
			m_pExDisablePushFrm->Render( iXPos, iYPos );
			m_pExDisablePushFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		if( m_pExDisablePush )
		{
			m_pExDisablePush->Render( iXPos, iYPos );
			m_pExDisablePush->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
		RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
	}
	else if( HasExWndStyle( IWS_EX_PUSH_DOUBLE ) )
	{
		if( m_pExDisablePushFrm )
		{
			m_pExDisablePushFrm->Render( iXPos, iYPos );
			m_pExDisablePushFrm->Render( iXPos, iYPos );
		}

		if( m_pExDisablePush )
		{
			m_pExDisablePush->Render( iXPos, iYPos );
			m_pExDisablePush->Render( iXPos, iYPos );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
	else if( HasExWndStyle( IWS_EX_PUSHED_OVER_SCREEN ) )
	{
		if( m_pExDisablePushFrm )
		{
			m_pExDisablePushFrm->Render( iXPos, iYPos );

			if( m_bOver )
				m_pExDisablePushFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		if( m_pExDisablePush )
		{
			m_pExDisablePush->Render( iXPos, iYPos );

			if( m_bOver )
				m_pExDisablePush->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );

		if( m_bOver )
		{
			RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
		}
	}
	else
	{
		if( m_pExDisablePushFrm )
		{
			m_pExDisablePushFrm->Render( iXPos, iYPos );
		}

		if( m_pExDisablePush )
		{
			m_pExDisablePush->Render( iXPos, iYPos );
		}

		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	}
}

void ioButton::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );

	if( !m_szClickSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szClickSound );
	}

	if( m_pParent )
	{
		if( IsExDisabled() )
			m_pParent->iwm_command( this, IOEX_BTNDOWN, 0 );
		else
			m_pParent->iwm_command( this, IOBN_BTNDOWN, 0 );
	}
}

void ioButton::iwm_lbuttonup( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttonup( mouse );

	if( m_pParent )
	{
		if( IsExDisabled() )
			m_pParent->iwm_command( this, IOEX_BTNUP, 0 );
		else
			m_pParent->iwm_command( this, IOBN_BTNUP, 0 );
	}
}

void ioButton::iwm_lbuttonDBLCLK( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttonDBLCLK( mouse );

	if( m_pParent )
	{
		if( IsExDisabled() )
			m_pParent->iwm_command( this, IOEX_LDBLCLK, 0 );
		else
			m_pParent->iwm_command( this, IOBN_LDBLCLK, 0 );
	}
}

void ioButton::iwm_rbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_rbuttondown( mouse );

	if( m_pParent )
	{
		if( IsExDisabled() )
			m_pParent->iwm_command( this, IOBN_RBTNDOWN, 0 );
		else
			m_pParent->iwm_command( this, IOEX_RBTNDOWN, 0 );
	}
}

void ioButton::iwm_rbuttonup( const ioMouse& mouse )
{
	ioWnd::iwm_rbuttonup( mouse );

	if( m_pParent )
	{
		if( IsExDisabled() )
			m_pParent->iwm_command( this, IOEX_RBTNUP, 0 );
		else
			m_pParent->iwm_command( this, IOBN_RBTNUP, 0 );
	}
}

void ioButton::iwm_mouseover( const ioMouse& mouse )
{
	ioWnd::iwm_mouseover( mouse );

	if( !m_szOverSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szOverSound );
	}
}

DWORD ioButton::GetWndType() const
{
	return IWT_BUTTON;
}

void ioButton::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szHelp = xElement.GetStringAttribute( "Help" );
	m_szOverSound = xElement.GetStringAttribute( "OverSound" );
	m_szClickSound = xElement.GetStringAttribute( "ClickSound" );
	m_bTitleOffsetUse = xElement.GetBoolAttribute( "DisableTextOffsetUse" );
	m_nDisableTextStyleOffsetX = xElement.GetIntAttribute( "DisableTextOffsetX" );
	m_nDisableTextStyleOffsetY = xElement.GetIntAttribute( "DisableTextOffsetY" );
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들
void	ioButton::SetSaveData( ioXMLElement& myxml )
{
	// save propertys
	ioWnd::SetSaveData( myxml );
	ioXMLElement imageryxml = SaveImagery( myxml );

	// frame
	SetFrame( imageryxml, m_pNormalFrm, "Normal" );
	SetFrame( imageryxml, m_pOverFrm, "Over" );
	SetFrame( imageryxml, m_pOverAddFrm, "OverAdd" );
	SetFrame( imageryxml, m_pPushFrm, "Push" );
	SetFrame( imageryxml, m_pBackFrm, "Back" );
	SetFrame( imageryxml, m_pDisableFrm, "Disable" );
	SetFrame( imageryxml, m_pExDisableNorFrm, "ExDisableNor" );
	SetFrame( imageryxml, m_pExDisablePushFrm, "ExDisablePush" );

	// image
	SetImagery( imageryxml, m_pNormal, "Normal" );
	SetImagery( imageryxml, m_pOver, "Over" );
	SetImagery( imageryxml, m_pOverAdd, "OverAdd" );
	SetImagery( imageryxml, m_pPush, "Push" );
	SetImagery( imageryxml, m_pBack, "Back" );
	SetImagery( imageryxml, m_pDisable, "Disable" );
	SetImagery( imageryxml, m_pExDisableNor, "ExDisableNor" );
	SetImagery( imageryxml, m_pExDisablePush, "ExDisablePush" );

	// ExtraInfo
	ioXMLElement xmlextra = myxml.CreateChild( "ExtraInfo" );
	xmlextra.SetStringAttribute( "Help", m_szHelp.c_str() );
	xmlextra.SetStringAttribute( "OverSound", m_szOverSound.c_str() );
	xmlextra.SetStringAttribute( "ClickSound", m_szClickSound.c_str() );
}

void	ioButton::Copy( ioWnd* wnd )
{
	ioWnd::Copy( wnd );

	ioButton* btnwnd = dynamic_cast<ioButton*>(wnd);
	if ( !wnd ) {
		return;
	}

	CLONEFRAME( btnwnd, m_pNormalFrm );
	CLONEFRAME( btnwnd, m_pOverFrm );
	CLONEFRAME( btnwnd, m_pOverAddFrm );
	CLONEFRAME( btnwnd, m_pPushFrm );
	CLONEFRAME( btnwnd, m_pBackFrm );
	CLONEFRAME( btnwnd, m_pDisableFrm );
	CLONEFRAME( btnwnd, m_pExDisableNorFrm );
	CLONEFRAME( btnwnd, m_pExDisablePushFrm );

	CLONEIMAGE( btnwnd, m_pNormal );
	CLONEIMAGE( btnwnd, m_pOver );
	CLONEIMAGE( btnwnd, m_pOverAdd );
	CLONEIMAGE( btnwnd, m_pPush );
	CLONEIMAGE( btnwnd, m_pBack );
	CLONEIMAGE( btnwnd, m_pDisable );
	CLONEIMAGE( btnwnd, m_pExDisableNor );
	CLONEIMAGE( btnwnd, m_pExDisablePush );

	btnwnd->m_szHelp = m_szHelp;
	btnwnd->m_szOverSound = m_szOverSound;
	btnwnd->m_szClickSound = m_szClickSound;

	{
		UIImageList::iterator iter = m_NormalList.begin();
		UIImageList::iterator eiter = m_NormalList.end();
		for ( ; iter != eiter; ++iter ) {
			btnwnd->m_NormalList.push_back( dynamic_cast<ioUIRenderImage*>( (*iter)->Clone() ) );
		}
	}
	{
		UIImageList::iterator iter = m_DisableList.begin();
		UIImageList::iterator eiter = m_DisableList.end();
		for ( ; iter != eiter; ++iter ) {
			btnwnd->m_DisableList.push_back( dynamic_cast<ioUIRenderImage*>( (*iter)->Clone() ) );
		}
	}
}
static const char* g_iobuttonfixedframe[ 8 ] = {
	"Normal",
	"Over",
	"OverAdd",
	"Push",
	"Back",
	"Disable",
	"ExDisableNor",
	"ExDisablePush"
};
int ioButton::GetNumFixedRenderFrame() const
{
	return ioWnd::GetNumFixedRenderFrame() + countof( g_iobuttonfixedframe );
}
const char* ioButton::GetFixedRenderFrameType( int index )
{
	const char* ret = ioWnd::GetFixedRenderFrameType( index );
	if ( ret == NULL ) {
		index = index - ioWnd::GetNumFixedRenderFrame();
	} else {
		return ret;
	}

	if ( index < 0 || index >= countof( g_iobuttonfixedframe ) ) {
		return NULL;
	}
	return g_iobuttonfixedframe[ index ];
}
ioUIRenderElement** ioButton::GetRenderFrame( const char* type )
{
	if ( strcmp( type, "Normal" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pNormalFrm);
	} else if ( strcmp( type, "Over" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pOverFrm);
	} else if ( strcmp( type, "OverAdd" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pOverAddFrm);
	} else if ( strcmp( type, "Push" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pPushFrm);
	} else if ( strcmp( type, "Back" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pBackFrm);
	} else if ( strcmp( type, "Disable" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pDisableFrm);
	} else if ( strcmp( type, "ExDisableNor" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pExDisableNorFrm);
	} else if ( strcmp( type, "ExDisablePush" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pExDisablePushFrm);
	}
	return ioWnd::GetRenderFrame( type );
}
// FixedRenderimage ( Get )
int ioButton::GetNumFixedRenderImage() const
{
	return countof( g_iobuttonfixedframe );
}
const char* ioButton::GetFixedRenderImageType( int index )
{
	if ( index < 0 || index >= GetNumFixedRenderImage() ) {
		return NULL;
	}
	return g_iobuttonfixedframe[ index ];
}
ioUIRenderElement** ioButton::GetRenderImage( const char* type )
{
	if ( strcmp( type, "Normal" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pNormal);
	} else if ( strcmp( type, "Over" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pOver);
	} else if ( strcmp( type, "OverAdd" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pOverAdd);
	} else if ( strcmp( type, "Push" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pPush);
	} else if ( strcmp( type, "Back" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pBack);
	} else if ( strcmp( type, "Disable" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pDisable);
	} else if ( strcmp( type, "ExDisableNor" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pExDisableNor);
	} else if ( strcmp( type, "ExDisablePush" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pExDisablePush);
	}
	return 0;
}
ioUIRenderImage* ioButton::GetRenderImg( eRenerImg eType )
{
	switch( eType )
	{
	case eRI_NORMAL:
		return m_pNormal;
	case eRI_OVER:
		return m_pOver;
	case eRI_OVERADD:
		return m_pOverAdd;
	case eRI_PUSH:
		return m_pPush;
	case eRI_BACK:
		return m_pBack;
	case eRI_DISABLE:
		return m_pDisable;
	}
	return NULL;
}

void ioButton::ChangeRenderImage( ioUIImage* pIcon )
{
	if( !pIcon )
		return;

	ioUIRenderImage* pRenderImg = NULL;
	RECT rt;
	rt.left = 0;
	rt.top = 0;
	rt.right = pIcon->GetWidth();
	rt.bottom = pIcon->GetHeight();

	SetWndRect( rt );
	pRenderImg = GetRenderImg(ioButton::eRI_NORMAL);
	if( pRenderImg )
	{
		pRenderImg->SetImage(pIcon);
		pRenderImg->SetRenderRect(rt);
	}
	pRenderImg = GetRenderImg(ioButton::eRI_OVER);
	if( pRenderImg )
	{
		pRenderImg->SetImage(pIcon);
		pRenderImg->SetRenderRect(rt);
	}
	pRenderImg = GetRenderImg(ioButton::eRI_PUSH);
	if( pRenderImg )
	{
		pRenderImg->SetImage(pIcon);
		pRenderImg->SetRenderRect(rt);
	}
	pRenderImg = GetRenderImg(ioButton::eRI_OVERADD);
	if( pRenderImg )
	{
		pRenderImg->SetImage(pIcon);
		pRenderImg->SetRenderRect(rt);
	}
	/*else
	{
		ioUIRenderFrame* pFrame = static_cast<ioUIRenderFrame*>(*GetRenderFrame( "OverAdd" ));
		if( pFrame )
		{
			pFrame->SetRenderRect(rt);
		}
	}*/
}
//************************************************************************************************************************

ioScreenButton::ioScreenButton()
{
}

ioScreenButton::~ioScreenButton()
{
}

void ioScreenButton::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_pNormalFrm )
	{
		m_pNormalFrm->Render( iXPos, iYPos );
		m_pNormalFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}

	if( m_pNormal )
	{
		m_pNormal->Render( iXPos, iYPos );
		m_pNormal->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );

}

void ioScreenButton::OnDrawPushed( int iXPos, int iYPos )
{
	if( m_pNormalFrm )
	{
		m_pNormalFrm->Render( iXPos, iYPos );
		m_pNormalFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		m_pNormalFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}

	if( m_pNormal )
	{
		m_pNormal->Render( iXPos, iYPos );
		m_pNormal->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		m_pNormal->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
	RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
	RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
}

ioConvexButton::ioConvexButton()
{
}

ioConvexButton::~ioConvexButton()
{
}

void ioConvexButton::OnDrawOvered( int iXPos, int iYPos )
{
	OnDrawNormal( iXPos, iYPos - 1 );
}

void ioConvexButton::OnDrawPushed( int iXPos, int iYPos )
{
	OnDrawNormal( iXPos, iYPos + 2 );
}

ioScaleButton::ioScaleButton()
{
	m_fScaleRate  = 0.2f;
	m_dwScaleTime = 300;

	m_fCurScaleRate = 1.0f;
	m_dwScaleStartTime = 0;
}

ioScaleButton::~ioScaleButton()
{
}

void ioScaleButton::SetScaleRate( float fRate )
{
	m_fScaleRate = fRate;
}

void ioScaleButton::SetScaleDuration( DWORD dwDuration )
{
	m_dwScaleTime = max( dwDuration, 1 );
}

void ioScaleButton::ImageScale( float fRate )
{
	if( m_pNormal )
	{
		m_pNormal->SetScale( fRate );
	}

	UIElementList::iterator iter;
	for( iter=m_ElementList.begin() ; iter!=m_ElementList.end() ; ++iter )
	{
		(*iter)->SetScale( fRate );
	}
}

void ioScaleButton::OnProcess( float fTimePerSec )
{
	if( IsOvered() )
	{
		if( m_dwScaleStartTime == 0 )
			m_dwScaleStartTime = FRAMEGETTIME();

		DWORD dwGapTime = FRAMEGETTIME() - m_dwScaleStartTime;
		if( dwGapTime > m_dwScaleTime )
		{
			m_dwScaleStartTime += m_dwScaleTime;
			dwGapTime -= m_dwScaleTime;
		}

		float fTimeRate = (float)dwGapTime / (float)m_dwScaleTime;
		m_fCurScaleRate = 1.0f + m_fScaleRate * sinf( D3DX_PI * fTimeRate );
		ImageScale( m_fCurScaleRate );
	}
	else if( IsPushed() )
	{
		m_fCurScaleRate = 1.0f;
		m_dwScaleStartTime = 0;
		ImageScale( m_fCurScaleRate );
	}
	else
	{
		if( m_dwScaleStartTime > 0 )
		{
			DWORD dwGapTime = FRAMEGETTIME() - m_dwScaleStartTime;
			if( dwGapTime > m_dwScaleTime )
			{
				m_fCurScaleRate = 1.0f;
				m_dwScaleStartTime = 0;
			}
			else
			{
				float fTimeRate = (float)dwGapTime / (float)m_dwScaleTime;
				m_fCurScaleRate = 1.0f + m_fScaleRate * sinf( D3DX_PI * fTimeRate );
				ImageScale( m_fCurScaleRate );
			}
		}
	}
}

void ioScaleButton::OnDrawOvered( int iXPos, int iYPos )
{
	OnDrawNormal( iXPos, iYPos );
}

void ioScaleButton::OnDrawPushed( int iXPos, int iYPos )
{
	OnDrawNormal( iXPos, iYPos );
}

bool ioScaleButton::IsNeedProcess() const
{
	return true;
}

void ioScaleButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	float fScaleRate = xElement.GetFloatAttribute( "ScaleRate" );
	DWORD dwScaleDuration = xElement.GetIntAttribute( "ScaleDuration" );

	if( fScaleRate != 0.0f )
	{
		SetScaleRate( fScaleRate );
	}
	if( dwScaleDuration > 0 )
	{
		SetScaleDuration( dwScaleDuration );
	}
}

//------------------------------------------------------------------------------------------------

ioRadioButton::ioRadioButton()
{
}

ioRadioButton::~ioRadioButton()
{
}

void ioRadioButton::iwm_lbuttonup( const ioMouse& mouse )
{
	if(m_pParent)
		m_pParent->iwm_command( this, IOBN_BTNUP, 0 );
}

void ioRadioButton::iwm_mouseleave( const ioMouse& mouse )
{
	m_bOver = false;
	
	if( m_pParent )
	{
		m_pParent->iwm_command( this, IOWN_LEAVED, 0 );
	}
}

DWORD ioRadioButton::GetWndType() const
{
	return IWT_RADIO_BTN;
}

void ioRadioButton::OnDrawPushed( int iXPos, int iYPos )
{
//	ioButton::OnDrawNormal( iXPos, iYPos );
	ioButton::OnDrawPushed( iXPos, iYPos );
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들



//************************************************************************************************************************

ioCheckButton::ioCheckButton()
{
	m_pCheckImg = NULL;
	m_bChecked = false;

	m_dwUnCheckTextColor = 0;
	m_dwUnCheckBkColor = 0;
	m_bUnCheckColorSetted = false;
}

ioCheckButton::~ioCheckButton()
{
	SAFEDELETE( m_pCheckImg );
}

void ioCheckButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Check" )
	{
		m_pCheckImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ioCheckButton::SetWindowAlpha( int iAlpha )
{
	ioButton::SetWindowAlpha( iAlpha );

	if( m_pCheckImg )
	{
		m_pCheckImg->SetAlpha( iAlpha );
	}
}

void ioCheckButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	ioHashString szColor;
	szColor = xElement.GetStringAttribute( "UnCheckText" );
	if( szColor.IsEmpty() )
	{
		m_bUnCheckColorSetted = false;
		return;
	}
	m_dwUnCheckTextColor = ioStringConverter::ParseColor( szColor );

	szColor = xElement.GetStringAttribute( "UnCheckBack" );
	m_dwUnCheckBkColor = ioStringConverter::ParseColor( szColor );
	m_bUnCheckColorSetted = true;
}

void ioCheckButton::OnDrawBack( int iXPos, int iYPos )
{
	ioButton::OnDrawBack( iXPos, iYPos );
}

void ioCheckButton::OnRender()
{
	if( m_bChecked )
	{
		ioButton::OnRender();

        if( m_pCheckImg )
		{
			int iXPos = GetDerivedPosX();
			int iYPos = GetDerivedPosY();
			m_pCheckImg->Render( iXPos, iYPos );
		}
	}
	else
	{
		if( !m_bUnCheckColorSetted )
		{
			ioButton::OnRender();
		}
		else
		{
			int iXPos = GetDerivedPosX();
			int iYPos = GetDerivedPosY();

			OnDrawBack( iXPos, iYPos );

			if( IsDisabled() )
			{
				OnDrawDisabled( iXPos, iYPos );
				PrintDisableTitle( iXPos, iYPos );

				if( m_bChecked  && m_pCheckImg )
				{
					m_pCheckImg->Render( iXPos, iYPos );
				}
			}
			else
			{
				if( IsPushed() )
				{
					OnDrawPushed( iXPos, iYPos );
				}
				else if( IsOvered() )
				{
					OnDrawOvered( iXPos, iYPos );
				}
				else
				{
					OnDrawNormal( iXPos, iYPos );
				}

				if( m_pUITitle )
				{
					m_pUITitle->Print( iXPos, iYPos, m_dwUnCheckTextColor, m_dwUnCheckBkColor, m_pUITitle->GetTextStyle() );
				}
			}
		}
	}
}

void ioCheckButton::iwm_lbuttonup( const ioMouse& mouse )
{
	m_bChecked = !m_bChecked;

	ioButton::iwm_lbuttonup( mouse );
}

DWORD ioCheckButton::GetWndType() const
{
	return IWT_CHECK_BOX;
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들
void	ioCheckButton::SetSaveData( ioXMLElement& xml )
{
	// set save data
	ioButton::SetSaveData( xml );

	// custom
	ioXMLElement elem = xml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "Imagery" ) == 0 )
		{
			SetImagery( elem, m_pCheckImg, "Check" );
			break;
		}
		elem = elem.NextSibling();
	}
}
void	ioCheckButton::Copy( ioWnd* wnd )
{
	ioButton::Copy( wnd );

	ioCheckButton* chwnd = dynamic_cast<ioCheckButton*>( wnd );
	if ( chwnd )
	{
		CLONEIMAGE( chwnd, m_pCheckImg );

		chwnd->m_bChecked = false;
		chwnd->m_dwUnCheckTextColor = m_dwUnCheckTextColor;
		chwnd->m_dwUnCheckBkColor = m_dwUnCheckBkColor;
		chwnd->m_bUnCheckColorSetted = m_bUnCheckColorSetted;
	}
}
static const char* g_iocheckbuttonfixedimage[ 1 ] = {
	"Check"
};
void ioCheckButton::SetUnCheckTextColor( DWORD color )
{
	m_dwUnCheckTextColor = color;
	m_bUnCheckColorSetted = true;
}
void ioCheckButton::SetUnCheckBackColor( DWORD color )
{
	m_dwUnCheckBkColor = color;
	m_bUnCheckColorSetted = true;
}
int ioCheckButton::GetNumFixedRenderImage() const
{
	return ioButton::GetNumFixedRenderImage() + countof( g_iocheckbuttonfixedimage );
}
const char* ioCheckButton::GetFixedRenderImageType( int index )
{
	const char* ret = ioButton::GetFixedRenderImageType( index );
	if ( ret == NULL ) {
		index = index - ioButton::GetNumFixedRenderImage();
	} else {
		return ret;
	}

	if ( index < 0 || index >= countof( g_iocheckbuttonfixedimage ) ) {
		return NULL;
	}
	return g_iocheckbuttonfixedimage[ index ];
}
ioUIRenderElement** ioCheckButton::GetRenderImage( const char* type )
{
	if ( strcmp( type, "Check" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pCheckImg);
	}
	return ioButton::GetRenderImage( type );
}
//************************************************************************************************************************


//////////////////////////////////////////////////////////////////////////
ioActiveScreenBtn::ioActiveScreenBtn()
{
	m_bScreen = false;
	m_dwScreenTimer = FRAMEGETTIME();
}

ioActiveScreenBtn::~ioActiveScreenBtn()
{

}

void ioActiveScreenBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	ioButton::OnProcess( fTimePerSec );

	if( FRAMEGETTIME() - m_dwScreenTimer > 500 )
	{
		m_dwScreenTimer = FRAMEGETTIME();
		m_bScreen = !m_bScreen;
	}
}

void ioActiveScreenBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bScreen )
	{
		ioButton::OnDrawOvered( iXPos, iYPos );
	}
	else
	{
		ioButton::OnDrawNormal( iXPos, iYPos );
	}
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들
void	ioActiveScreenBtn::Copy( ioWnd* wnd )
{
	ioButton::Copy( wnd );

	ioActiveScreenBtn* acwnd = dynamic_cast<ioActiveScreenBtn*>( wnd );
	if ( acwnd )
	{
		acwnd->m_bScreen = m_bScreen;
		acwnd->m_dwScreenTimer = FRAMEGETTIME();
	}
}
//************************************************************************************************************************


//////////////////////////////////////////////////////////////////////////
ioToggleBtn::ioToggleBtn()
	: m_bToggle( false )
{
}

ioToggleBtn::~ioToggleBtn()
{
}

void ioToggleBtn::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );

	m_bToggle = !m_bToggle;
}

void ioToggleBtn::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDrawBack( iXPos, iYPos );

	if( IsDisabled() )
	{
		OnDrawDisabled( iXPos, iYPos );
		OnDrawDisableList( iXPos, iYPos );
		PrintDisableTitle( iXPos, iYPos );
	}
	else if( IsExDisabled() )
	{
		if( IsPushed() )
		{
			OnDrawExDisabledPushed( iXPos, iYPos );
		}
		else if( IsOvered() )
		{
			OnDrawExDisabledOvered( iXPos, iYPos );
		}
		else
		{
			OnDrawExDisabledNormal( iXPos, iYPos );
		}
		OnDrawDisableList( iXPos, iYPos );
		PrintDisableTitle( iXPos, iYPos );
	}
	else
	{
		if( IsPushed() || m_bToggle )
		{
			OnDrawPushed( iXPos, iYPos );
			PrintPushTitle( iXPos, iYPos );
		}
		else if( IsOvered() && !m_bToggle )
		{
			OnDrawOvered( iXPos, iYPos );
			PrintOverTitle( iXPos, iYPos );
		}
		else
		{
			OnDrawNormal( iXPos, iYPos );
			PrintNormalTitle( iXPos, iYPos );
		}
		OnDrawNormalList( iXPos, iYPos );
	}
}