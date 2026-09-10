#include "stdafx.h"

#include "ioShuffleModeUIRender.h"
#include "ioBaseChar.h"

ioShuffleCharStateUI::ioShuffleCharStateUI()
{
	m_bShuffleUIRender = false;

	m_pStarBg  = NULL;
	m_pStarNum = NULL;

	m_iStarCount   = 0;
	m_iStarXOffset = 0;
}

ioShuffleCharStateUI::~ioShuffleCharStateUI()
{	
	SAFEDELETE( m_pStarBg );
	SAFEDELETE( m_pStarNum );
}

void ioShuffleCharStateUI::Load()
{
	SAFEDELETE( m_pStarBg );
	SAFEDELETE( m_pStarNum );
	
	//UI이기 때문에 _e를 사용 X
	ioINILoader kLoader( "config/sp2_char.ini" );
	kLoader.SetTitle( "common" );

	char szBuf[MAX_PATH];
	kLoader.LoadString( "ui_star_bg", "", szBuf, MAX_PATH );
	ioHashString szStarBg = szBuf;

	kLoader.LoadString( "ui_star_num", "", szBuf, MAX_PATH );
	ioHashString szStarNum = szBuf;

	m_pStarBg  = g_UIImageSetMgr.CreateImageByFullName( szStarBg );
	m_pStarNum = g_UIImageSetMgr.CreateImageByFullName( szStarNum );
}

void ioShuffleCharStateUI::UpdateShuffleModeUI( bool bSystemVisible, ioBaseChar* pOwner, ioBaseChar *pCameraTarget )
{
	if( !pOwner )
		return;

	if( !bSystemVisible || ( !pOwner->IsCanInvisible() && !pOwner->IsVisibleChar() ) )
	{		
		m_bShuffleUIRender = false;
		return;
	}
	else
	{
		switch( pOwner->GetState() )
		{
		case CS_ESCAPE_PRISONER:
		case CS_READY:
		case CS_LOADING:
		case CS_OBSERVER:
		case CS_VIEW:
			{
				m_bShuffleUIRender = false;
				return;
			}
			break;
		case CS_DIE:
			if( pCameraTarget->GetDieState() == DS_FADE_OUT )
			{
				m_bShuffleUIRender = false;
				return;
			}
			break;
		}
	}

	if( g_GUIMgr.IsNoneUIMode() )
	{
		m_bShuffleUIRender = false;
		return;
	}

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT1 );
	vPos.y += 30.0f;

	D3DXVECTOR3 v2DPos = Setting::Convert3DTo2D( vPos );
	
	if( pOwner->IsVisibleID() )
		v2DPos.y -= 20.0f;

	if( pOwner->IsOwnerChar() )
		v2DPos.y -= 40.0f;

	if( m_pStarBg )
	{
		v2DPos.y -= m_pStarBg->GetHeight()/2 + m_pStarBg->GetOffsetY()/2;

		m_iStarXOffset = m_pStarBg->GetWidth()/2 - m_pStarBg->GetOffsetX() - 8.0f;
	}

	m_iXPos = (int)v2DPos.x;
	m_iYPos = (int)v2DPos.y;
	m_bShuffleUIRender = true;
}

void ioShuffleCharStateUI::RenderShuffleModeUI( ioBaseChar* pOwner )
{
	if( !m_bShuffleUIRender )
		return;
	
	if( m_pStarBg )
		m_pStarBg->Render( m_iXPos, m_iYPos );

	if( m_pStarNum )
	{
		m_pStarNum->RenderNum( m_iXPos + m_iStarXOffset, m_iYPos, m_iStarCount, -8.0f, FLOAT1, UI_RENDER_NORMAL, TFO_BILINEAR, UI_RENDER_NORMAL, ioUIRenderImage::RNF_REVERSE );		
	}	
}