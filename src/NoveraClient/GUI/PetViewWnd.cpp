
#include "StdAfx.h"

#include "PetViewWnd.h"

#include "../ioComplexStringPrinter.h"

#include "ioPetView.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PetViewWnd::PetViewWnd()
{
	m_pUI3DRender = NULL;
	m_pPetView	  = NULL;
	m_pEffectDummy = NULL;

	m_p3DBack = NULL;
	m_pShadow = NULL;

	m_fRotateYaw = 0.0f;
	m_fPreYaw = 0.0f;
	m_fXMLRotateYaw = 0.0f;

	m_vCharPos = D3DXVECTOR3( 0.0f, 45.0f, FLOAT500 );
	m_vDummyPos = D3DXVECTOR3( 0.0f, 45.0f, FLOAT500 );
	m_vCameraPos = D3DXVECTOR3( 0.0f, 0.0f, FLOAT500 );

	m_bLeftRotateOn  = false;
	m_bRightRotateOn = false;
}

PetViewWnd::~PetViewWnd()
{
	SAFEDELETE( m_pEffectDummy );
	SAFEDELETE( m_pPetView );
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_p3DBack );
	SAFEDELETE( m_pShadow );
}

void PetViewWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "shop_cam" );
}

void PetViewWnd::iwm_show()
{
	HideChildWnd( ID_CHAR_CONTROL_WND );

	if ( m_pShadow )
		m_pShadow->SetScale( m_fXMLShadowScale );
}

void PetViewWnd::iwm_hide()
{
	HideChildWnd( ID_CHAR_CONTROL_WND );

	SAFEDELETE( m_pPetView );
	SAFEDELETE( m_pEffectDummy );
}

void PetViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CHAR_CONTROL_WND:
		if( cmd == IOBN_BTNDOWN)
		{
			if( param == NewShopHelpControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( m_fXMLRotateYaw );
			}
			else if( param == NewShopHelpControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( -m_fXMLRotateYaw );
			}
		}
		else if( cmd == IOBN_BTNUP )
		{
			if( param == NewShopHelpControlWnd::ID_DEFAULT_POS_BTN )
			{
				if( m_pPetView )
					m_pPetView->SetYaw( FLOAT10 );
				if( ioWnd::m_pParent )
					ioWnd::m_pParent->iwm_command( this, cmd, param );
				InitRotate();
			}
			else if( param == NewShopHelpControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
			else if( param == NewShopHelpControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
		}
		break;
	}
}

void PetViewWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fXMLRotateYaw = xElement.GetFloatAttribute_e( "RotateYaw" );
	m_fXMLShadowScale = xElement.GetFloatAttribute_e( "ShadowScale" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Shadow" )
	{
		SAFEDELETE(m_pShadow);
		m_pShadow = pImage;
	}
	else if( szType == "3D_Back" )
	{
		SAFEDELETE(m_p3DBack);
		m_p3DBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PetViewWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	if( m_pPetView )
	{
		m_pPetView->RotateY( m_fRotateYaw );
		m_pPetView->Update( fTimePerSec );
	}

	if ( m_pEffectDummy )
	{
		m_pEffectDummy->Update( fTimePerSec );
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	RECT rcNameRect;
	rcNameRect.left = GetDerivedPosX();
	rcNameRect.top  = GetDerivedPosY();
	rcNameRect.right  = rcNameRect.left + GetWidth();
	rcNameRect.bottom = rcNameRect.top + GetHeight();

	if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
	{
		ShowChildWnd( ID_CHAR_CONTROL_WND );		
	}
	else
	{
		NewShopHelpControlWnd *pControlWnd = dynamic_cast<NewShopHelpControlWnd*> (FindChildWnd( ID_CHAR_CONTROL_WND ) );
		if( pControlWnd )
			pControlWnd->HideWndAnimation();
	}
}

void PetViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_p3DBack )
	{
		int nNewXPos = iXPos + GetWidth()/2 - (m_p3DBack->GetWidth()/2);
		int nNewYPos = iYPos + GetHeight()/2 - (m_p3DBack->GetHeight()/2);

		m_p3DBack->SetScale( 2.0f );
		m_p3DBack->Render( nNewXPos, nNewYPos );
	}

	if ( m_pShadow )
	{
		enum
		{
			Y_OFFSET = 200,
		};
		int nNewXPos = iXPos + GetWidth()/2 - (m_pShadow->GetWidth()/2);
		int nNewYPos = iYPos - (m_pShadow->GetHeight()/2) + Y_OFFSET;
		m_pShadow->Render( nNewXPos, nNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( iXPos - 320, iYPos - 60 );
	m_pUI3DRender->Render( &matUI, true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetViewWnd::RotateY( float fYaw )
{
	m_fRotateYaw = fYaw;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetViewWnd::InitRotate()
{
	RotateY( 0.0f );
	m_bLeftRotateOn  = false;
	m_bRightRotateOn = false;
	if( m_pPetView )
		m_pPetView->SetYaw( FLOAT10 );
}

void PetViewWnd::CreatePet( int nPetCode, int nPetRank )
{
	if ( nPetCode <= 0 || nPetRank <= 0 )
		return;

	ioHashString szPetModel = g_PetInfoMgr.GetPetModel( nPetCode, (PetRankType)nPetRank );
	if ( szPetModel.IsEmpty() )
		return;

	ioPetView *pPet = m_pUI3DRender->CreatePet( szPetModel );
	if( !pPet )	
		return;

	if( m_pPetView )
		m_fPreYaw = m_pPetView->GetYaw();

	pPet->SetPosition( m_vCharPos );
	pPet->SetYaw( m_fPreYaw );

	m_pUI3DRender->SetLookAt( m_vCameraPos );

	SAFEDELETE( m_pPetView );
	m_pPetView = pPet;
}

void PetViewWnd::DestroyPet()
{
	SAFEDELETE( m_pPetView );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetViewWnd::SetPetScale( int nPetCode, int nPetRank )
{
	D3DXVECTOR3 vScale = g_PetInfoMgr.GetPetModelScale( nPetCode, (PetRankType)nPetRank );

	if( m_pPetView )
		m_pPetView->SetScale( vScale );
}

void PetViewWnd::SetLoopAni( ioHashString szPetViewAni )
{
	if ( !m_pPetView || szPetViewAni.IsEmpty() )
		return;
	
	m_pPetView->SetLoopAni( szPetViewAni );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetViewWnd::CreateEffectDummy()
{	
	ioPetView *pEffectDummy = m_pUI3DRender->CreatePet( "staticmodel/pet_ui_dummy.txt" );
	if( !pEffectDummy )	
		return;
	
	pEffectDummy->SetPosition( m_vDummyPos );
	pEffectDummy->SetScale( 2.0f );

	SAFEDELETE( m_pEffectDummy );
	m_pEffectDummy = pEffectDummy;
}

void PetViewWnd::DummyAttachEffect( ioHashString szPetEffect )
{
	if ( !m_pEffectDummy || szPetEffect.IsEmpty() )
		return;

	m_pEffectDummy->AttachEffect( szPetEffect, NULL );
}

void PetViewWnd::DummyEndEffect( ioHashString szPetEffect )
{
	if ( !m_pEffectDummy || szPetEffect.IsEmpty() )
		return;

	m_pEffectDummy->EndEffect( szPetEffect, false );
}
