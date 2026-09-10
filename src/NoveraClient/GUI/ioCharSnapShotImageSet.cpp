

#include "StdAfx.h"

#include "../io3DEngine/ioRenderSystem.h"
#include "../io3DEngine/ioRenderTexture.h"
#include "../io3DEngine/ioTextureManager.h"

#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSet.h"
#include "../io3DEngine/ioUIImageSetManager.h"

#include "../ioItem.h"
#include "../CharacterInfo.h"

#include "../ioCreateChar.h"
#include "../ioBaseChar.h"
#include "../CharacterInfo.h"

#include "ioUI3DRender.h"
#include "ioCharSnapShotImageSet.h"

//42, 27

#define SNAPSHOT_WIDTH		64
#define SNAPSHOT_HEIGHT		64
#define SNAPSHOT_TEX_SIZE	256

#define SNAP_WIDTH_COUNT	4
#define SNAP_HEIGHT_COUNT	4

ioCharSnapShotImageSet::ioCharSnapShotImageSet()
{
	m_pSystem = NULL;
	m_pRenderTexture = NULL;

	m_pUI3DRender = NULL;
}

ioCharSnapShotImageSet::~ioCharSnapShotImageSet()
{
	m_pSystem = NULL;

	DESTROY_TEXTURE( m_pRenderTexture );
	SAFEDELETE( m_pUI3DRender );

	DestorySnapShotPool();
}

bool ioCharSnapShotImageSet::Initialize( ioRenderSystem *pSystem )
{
	m_pSystem = pSystem;

	SAFEDELETE( m_pUI3DRender );
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "snapshot_cam" );

	DestorySnapShotPool();

	// Render Texture »ý¼º
	m_pRenderTexture = g_TextureMgr.CreateRTT( "_CharSnapShotRTT",
												SNAPSHOT_TEX_SIZE,
												SNAPSHOT_TEX_SIZE,
												D3DFMT_A8R8G8B8,
												D3DFMT_D16,
												false );

	if( !m_pRenderTexture )
	{
		LOG.PrintTimeAndLog( 0, "CreateRTT Failed" );
		return false;
	}

	char szImageName[MAX_PATH];

	// ÃÖÃÊ UIImageSet »ý¼º
	ioUIImageSet *pSnapShotSet = g_UIImageSetMgr.GetImageSet( "_CharSnapShotSet" );
	if( !pSnapShotSet )
	{
		pSnapShotSet = g_UIImageSetMgr.CreateImageSet( "_CharSnapShotSet", "_CharSnapShotRTT" );
		if( pSnapShotSet )
		{
			int iNameCount = 1;
			for( int iY=0 ; iY<SNAP_HEIGHT_COUNT ; iY++ )
			{
				for( int iX=0 ; iX<SNAP_WIDTH_COUNT ; iX++ )
				{
					RECT rcArea;
					rcArea.left   = iX * SNAPSHOT_WIDTH;
					rcArea.right  = (iX+1) * SNAPSHOT_WIDTH;
					rcArea.top    = iY * SNAPSHOT_HEIGHT;
					rcArea.bottom = (iY+1) * SNAPSHOT_HEIGHT;

					wsprintf( szImageName, "_SnapShot%d", iNameCount );
					pSnapShotSet->DefineImage( szImageName, rcArea );

					iNameCount++;
				}
			}
		}
	}

	if( pSnapShotSet )
	{
		m_SnapShotPool.reserve( SNAP_WIDTH_COUNT * SNAP_HEIGHT_COUNT );

		for( int i=0; i<SNAP_WIDTH_COUNT * SNAP_HEIGHT_COUNT ; i++ )
		{
			SnapShot kSnapShot;

			wsprintf( szImageName, "_SnapShot%d", i + 1 );
			kSnapShot.m_pImg = g_UIImageSetMgr.CreateImage( "_CharSnapShotSet", szImageName );
			if( kSnapShot.m_pImg )
			{
				m_SnapShotPool.push_back( kSnapShot );
			}
		}
	}

	return true;
}

void ioCharSnapShotImageSet::AddCharSnapShot( ioBaseChar *pChar )
{
	if( !m_pRenderTexture || !pChar )
		return;

	SnapShot *pCurSnapShot = FindEnableSnapShot( pChar->GetCharName() );
	if( !pCurSnapShot || !pCurSnapShot->m_pImg )
		return;

	ioCreateChar *pCreateChar = m_pUI3DRender->CreateChar( pChar->GetCharacterInfo(),
														   "SnapShotChar",
														   false );
	if( !pCreateChar )	return;

	if( !pChar->GetNowFaceName().IsEmpty() )
		pCreateChar->DirectFaceChange( pChar->GetNowFaceName().c_str(), 100000 );
	pCreateChar->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 30.0f ) );

	m_pUI3DRender->SetLookAt( D3DXVECTOR3( 0.0f, 110.0f, 0.0f ) );

	float fAspect = (float)SNAPSHOT_WIDTH/(float)SNAPSHOT_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 300.0f, 45.0f, fAspect );

	// °©¿Ê, ¸Á¶Ç¸¸ ÀåÂø
	// ÄÚ½ºÆ¬ °ü·Ã (??)
	ioItem *pItem = pChar->GetEquipedItem( ES_ARMOR );
	if( pItem )
	{
		pCreateChar->EquipItem( pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom(), pItem->GetCostumeCode() );
	}

	pItem = pChar->GetEquipedItem( ES_CLOAK );
	if( pItem )
	{
		pCreateChar->EquipItem( pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom(), pItem->GetCostumeCode() );
	}

	pCreateChar->SetLoopAni( "public_delay.ani" );
	pCreateChar->Update( 0.001 );
	pCreateChar->Update( 0.001 );

	pCurSnapShot->m_Name = pChar->GetCharName();

	UpdateSnapShot( pCurSnapShot );

	SAFEDELETE( pCreateChar );
}

void ioCharSnapShotImageSet::AddDummyCharSnapShot( const ioHashString &szName )
{
	if( !m_pRenderTexture || szName.IsEmpty() )
		return;

	SnapShot *pCurSnapShot = FindEnableSnapShot( szName );
	if( !pCurSnapShot || !pCurSnapShot->m_pImg )
		return;

	CHARACTER kDummy;
	kDummy.m_kindred	 = 1;
	kDummy.m_sex		 = 1;
	kDummy.m_beard		 = 1;
	kDummy.m_face		 = 1;
	kDummy.m_hair		 = 1;
	kDummy.m_skin_color  = 1;
	kDummy.m_hair_color  = 1;
	kDummy.m_accessories = 1;
	kDummy.m_underwear   = 1;

	ioCreateChar *pCreateChar = m_pUI3DRender->CreateChar( kDummy, "SnapShotChar", false );
	if( !pCreateChar )	return;

	pCreateChar->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 30.0f ) );
	m_pUI3DRender->SetLookAt( D3DXVECTOR3( 0.0f, 110.0f, 0.0f ) );

	float fAspect = (float)SNAPSHOT_WIDTH/(float)SNAPSHOT_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 300.0f, 45.0f, fAspect );

	pCreateChar->SetLoopAni( "public_delay.ani" );
	pCreateChar->Update( 0.001 );
	pCreateChar->Update( 0.001 );

	pCurSnapShot->m_Name = szName;

	UpdateSnapShot( pCurSnapShot );

	SAFEDELETE( pCreateChar );
}

void ioCharSnapShotImageSet::ClearSnapShotNames()
{
	SnapShotPool::iterator iter = m_SnapShotPool.begin();
	for( ; iter!=m_SnapShotPool.end() ; ++iter )
	{
		iter->m_Name.Clear();
	}
}

void ioCharSnapShotImageSet::DestorySnapShotPool()
{
	SnapShotPool::iterator iter = m_SnapShotPool.begin();
	for( ; iter!=m_SnapShotPool.end() ; ++iter )
	{
		SAFEDELETE( iter->m_pImg );
	}

	m_SnapShotPool.clear();
}

void ioCharSnapShotImageSet::UpdateSnapShot( SnapShot *pSnapShot )
{
	RECT rcImg = pSnapShot->m_pImg->GetImageRect();

	D3DRECT rcD3D;
	rcD3D.x1 = rcImg.left;
	rcD3D.y1 = rcImg.top;
	rcD3D.x2 = rcImg.right;
	rcD3D.y2 = rcImg.bottom;

	D3DVIEWPORT9 kOldView, kNewView;
	kOldView = m_pSystem->GetViewPort();

	kNewView.X = rcD3D.x1;
	kNewView.Y = rcD3D.y1;
	kNewView.Width  = rcD3D.x2 - rcD3D.x1;
	kNewView.Height = rcD3D.y2 - rcD3D.y1;
	kNewView.MinZ = 0.0f;
	kNewView.MaxZ = FLOAT1;

	m_pSystem->SetRenderTarget( m_pRenderTexture );
	m_pSystem->SetViewPort( kNewView );

	m_pSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, &rcD3D );
	if( m_pSystem->BeginScene() )
	{
		m_pUI3DRender->RenderNow( NULL, false );
		m_pSystem->EndScene();
	}

	m_pSystem->RestoreRenderTarget();
	m_pSystem->SetViewPort( kOldView );
}

ioCharSnapShotImageSet::SnapShot* ioCharSnapShotImageSet::FindEnableSnapShot( const ioHashString &rkName )
{
	SnapShotPool::iterator iter = m_SnapShotPool.begin();
	for( ; iter!=m_SnapShotPool.end() ; ++iter )
	{
		if( iter->m_Name == rkName )
			return &(*iter);
	}

	iter = m_SnapShotPool.begin();
	for( ; iter!=m_SnapShotPool.end() ; ++iter )
	{
		if( iter->m_Name.IsEmpty() )
			return &(*iter);
	}

	return NULL;
}

ioUIRenderImage* ioCharSnapShotImageSet::GetSnapShotImg( const ioHashString &rkName )
{
	SnapShotPool::iterator iter = m_SnapShotPool.begin();
	for( ; iter!=m_SnapShotPool.end() ; ++iter )
	{
		if( iter->m_Name == rkName )
			return iter->m_pImg;
	}

	return NULL;
}

void ioCharSnapShotImageSet::RenderCharSnapShot( const ioHashString &rkName, int iXPos, int iYPos )
{
	ioUIRenderImage *pImg = GetSnapShotImg( rkName );
	if( pImg )
	{
		pImg->SetScale( 0.42f, 0.42f );
		pImg->Render( iXPos - 10, iYPos - 18, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}
