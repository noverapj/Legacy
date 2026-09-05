

#include "stdafx.h"

#include "ioCreateChar.h"
#include "RaceSetting.h"
#include "CharacterInfo.h"

#include "io3DSkillCharIcon.h"

enum
{
	VIEW_WIDTH = 128,
	VIEW_HEIGHT= 114,
};

io3DSkillCharIcon::io3DSkillCharIcon()
{
	m_pUI3DRender = NULL;
	m_pAvata = NULL;

	m_dwSetStart = 0;

	m_iXOffSet = 0;
	m_iYOffSet = 0;
}

io3DSkillCharIcon::~io3DSkillCharIcon()
{
	SAFEDELETE( m_pAvata );
	SAFEDELETE( m_pUI3DRender );
}

void io3DSkillCharIcon::Initialize()
{
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "skill_char_cam" );

	Load();
}

void io3DSkillCharIcon::Load()
{
	ioINILoader_e kLoader( "config/sp2_gui.ini" );
	kLoader.SetTitle_e( "skill_char_icon" );

	m_iXOffSet   = kLoader.LoadInt_e( "x_offset", 0 );
	m_iYOffSet   = kLoader.LoadInt_e( "y_offset", 0 );

	char szBuf[MAX_PATH] = "";
	kLoader.LoadString_e( "char_motion", "", szBuf, MAX_PATH );
	m_szCharMotion = szBuf;
}

void io3DSkillCharIcon::SetAvata( const CHARACTER &rkInfo )
{
	if( !m_pUI3DRender )	return;

	SAFEDELETE( m_pAvata );

	m_pAvata = m_pUI3DRender->CreateChar( rkInfo, "SkillCharIcon" );
	if( !m_pAvata )
	{
		LOG.PrintTimeAndLog( 0, "io3DSkillCharIcon::SetAvata - CreateAvata Fail" );
		return;
	}

	m_pAvata->DestroyHandAndBody();

	D3DXVECTOR3 vCharPos( 0.0f, 0.0f, FLOAT500 );
	m_pAvata->SetPosition( vCharPos );
	m_pAvata->SetScale( FLOAT1 );
	m_pAvata->SetYaw( 0.0f );

	m_pUI3DRender->SetLookAt( vCharPos );

	/*
	D3DXVECTOR3 vCharPos( 0.0f, 0.0f, 900.0f );
	m_pAvata->SetPosition( vCharPos );
	m_pAvata->SetYaw( 0.0f );
	m_pUI3DRender->SetLookAt( vCharPos );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );
	*/

	if( !m_szCharMotion.IsEmpty() )
		m_pAvata->SetLoopAni( m_szCharMotion );

	m_pAvata->Update( 0.01f );

	m_pAvata->ChangeHairOutLineThick( 2.0f );
	m_pAvata->ChangeHeadOutLineThick( 2.0f );
}

void io3DSkillCharIcon::FaceChange( const char *szTexPostfix, int iDuration )
{
	if( m_pAvata )
	{
		m_pAvata->DirectFaceChange( szTexPostfix, iDuration );
	}
}

void io3DSkillCharIcon::Update( float fTimePerSec )
{
	if( m_pAvata )
	{
		m_pAvata->Update( fTimePerSec );
	}
}

void io3DSkillCharIcon::Render( int iXPos, int iYPos )
{
	m_iXPos = iXPos + m_iXOffSet;
	m_iYPos = iYPos + m_iYOffSet;

	if( m_pAvata && m_pUI3DRender )
	{
		D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( m_iXPos, m_iYPos );
		m_pUI3DRender->Render( &matUI, true );
	}
}


