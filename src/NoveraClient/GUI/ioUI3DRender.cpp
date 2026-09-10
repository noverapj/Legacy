

#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioCamera.h"
#include "../io3DEngine/ioCameraController.h"
#include "../io3DEngine/ioLookAtCameraController.h"

#include "../io3DEngine/ioNode.h"
#include "../io3DEngine/ioSceneNode.h"
#include "../io3DEngine/ioSceneManager.h"
#include "../io3DEngine/ioRenderSystem.h"
#include "../io3DEngine/ioEntityGroup.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ErrorReport.h"
#include "../io3DEngine/HelpFunc.h"

#include "../io3DEngine/ioEffect.h"
#include "../io3DEngine/ioEffectFactory.h"
#include "../io3DEngine/ioResourceLoader.h"

#include "../GameEnumType.h"
#include "../ioCreateChar.h"
#include "ioPetView.h"
#include "../CharacterInfo.h"
#include "../RaceSetting.h"
#include "../Setting.h"
#include "../ioUICharAniEventHandler.h"
#include "../ioUIPetAniEventHandler.h"
#include "../EtcHelpFunc.h"
#include "../ioApplication.h"

#include "ioUI3DRender.h"
#include <strsafe.h>

class io3DUIQuad : public IUIQuad
{
protected:
	ioUI3DRender *m_p3DRender;
	D3DXMATRIX m_ExtraProj;
	bool m_bZClear;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray )
	{
		pQuadArray->AddDirectQuadInfo( this );
	}

	virtual int GetQuadCount() const
	{
		return 0;
	}

public:
	virtual bool IsDirectQuad() const
	{
		return true;
	}

	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
		if( m_p3DRender )
		{
			m_p3DRender->RenderNow( &m_ExtraProj, m_bZClear );
		}
	}

public:
	io3DUIQuad( ioUI3DRender *pRender, const D3DXMATRIX *pExtraProj, bool bZClear )
	{
		m_p3DRender = pRender;

		if( pExtraProj )
		{
			m_ExtraProj = *pExtraProj;
		}
		else
		{
			D3DXMatrixIdentity( &m_ExtraProj );
		}

		m_bZClear = bZClear;		
	}

	virtual ~io3DUIQuad(){}
};
//////////////////////////////////////////////////////////////////////////
class io3DUIViewPortQuad : public io3DUIQuad
{
protected:
	D3DRECT		 m_rcView;

public:
	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
		D3DVIEWPORT9 kOldView, kNewView;
		kOldView = pRenderSystem->GetViewPort();

		kNewView.X = m_rcView.x1;
		kNewView.Y = m_rcView.y1;
		kNewView.Width  = m_rcView.x2 - m_rcView.x1;
		kNewView.Height = m_rcView.y2 - m_rcView.y1;
		kNewView.MinZ = 0.0f;
		kNewView.MaxZ = FLOAT1;
		pRenderSystem->SetViewPort( kNewView );

		if( m_p3DRender )
			m_p3DRender->RenderNow( NULL, m_bZClear );

		pRenderSystem->SetViewPort( kOldView );
	}

public:
	io3DUIViewPortQuad( ioUI3DRender *pRender, const D3DRECT *pRcView, bool bZClear ) : io3DUIQuad( pRender, NULL, bZClear )
	{
		if( pRcView )
			m_rcView = *pRcView;
		else
			LOG.PrintTimeAndLog( 0, "io3DUIViewPortQuad : Not View Pointer!!!" );
	}
	virtual ~io3DUIViewPortQuad(){}
};
//////////////////////////////////////////////////////////////////////////

ioUI3DRender::SceneMgrList ioUI3DRender::m_CreatedSceneMgrList;

ioUI3DRender::ioUI3DRender()
{
	m_pSceneMgr = NULL;
	m_pCamera = NULL;
}

ioUI3DRender::~ioUI3DRender()
{
	Destroy();
}

void ioUI3DRender::Initialize( const char *szININame, const char *szCamInfo )
{
	ioINILoader kLoader( szININame );

	m_pCamera = new ioCamera;

	D3DXVECTOR3 vCamPos;
	kLoader.SetTitle( szCamInfo );
	vCamPos.x = kLoader.LoadFloat_e( "cam_offset_x", 0.0f );
	vCamPos.y = kLoader.LoadFloat_e( "cam_offset_y", 0.0f );
	vCamPos.z = kLoader.LoadFloat_e( "cam_offset_z", 0.0f );

	ioLookAtCameraController *pCtrl = new ioLookAtCameraController;
	pCtrl->SetPosition( vCamPos );
	pCtrl->SetLookAt( D3DXVECTOR3( 0.0f, 0.0f, 200.0f ) );
	pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );
	m_pCamera->SetController( pCtrl );

	m_pSceneMgr = new ioSceneManager( &RenderSystem() );
	AddSceneMgr( m_pSceneMgr );

	//------------------------- Init Lights ---------------------

	kLoader.SetTitle_e( "ambient" );
	m_UISceneAmbient.r = kLoader.LoadFloat( "red", 127.0f ) / 255.0f;
	m_UISceneAmbient.g = kLoader.LoadFloat_e( "green", 127.0f ) / 255.0f;
	m_UISceneAmbient.b = kLoader.LoadFloat_e( "blue", 127.0f ) / 255.0f;

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	kLoader.SetTitle_e( "main_light" );

	int iMax = kLoader.LoadInt_e( "model_animation_cnt", 0 );

	m_ModelAnimation.clear();
	m_ModelAnimation.reserve( iMax );
	for( int i=0; i<iMax; i++ )
	{
		wsprintf_e( szKey, "model_animation%d", i+1 );
		kLoader.LoadString( szKey, "public_delay.ani", szBuf, MAX_PATH );
		m_ModelAnimation.push_back( szBuf );
	}

	if( m_ModelAnimation.empty() )
	{
		m_ModelAnimation.push_back( ioHashString( "public_delay.ani" ) );
	}

	kLoader.LoadString_e( "toon_texture", "toongray.dds", szBuf, MAX_PATH );

	D3DCOLORVALUE rDiffuse;
	rDiffuse.r = kLoader.LoadFloat( "red", 127.0f ) / 255.0f;
	rDiffuse.g = kLoader.LoadFloat_e( "green", 127.0f ) / 255.0f;
	rDiffuse.b = kLoader.LoadFloat_e( "blue", 127.0f ) / 255.0f;
	rDiffuse.a = FLOAT1;

	D3DXVECTOR3 vPos,vTarget;
	vPos.x = kLoader.LoadFloat_e( "pos_x", 0.0f );
	vPos.y = kLoader.LoadFloat_e( "pos_y", 0.0f );
	vPos.z = kLoader.LoadFloat_e( "pos_z", 0.0f );

	vTarget.x = kLoader.LoadFloat_e( "target_x", 0.0f );
	vTarget.y = kLoader.LoadFloat_e( "target_y", -FLOAT1 );
	vTarget.z = kLoader.LoadFloat_e( "target_z", -FLOAT1 );

	D3DXVECTOR3 vLightDir = vTarget - vPos;				// Light Direction Calc
	D3DXVec3Normalize( &vLightDir, &vLightDir );
	SetMainLight( vLightDir, rDiffuse, rDiffuse, szBuf );
}

void ioUI3DRender::SetSoftSkinning( bool bSoft )
{
	SceneMgrList::iterator iter = m_CreatedSceneMgrList.begin();
	while( iter!=m_CreatedSceneMgrList.end() )
	{
		(*iter)->SetSoftwareVertexBlending( bSoft );
		++iter;
	}
}

void ioUI3DRender::ModifyEntityGroupMaterial( ioEntityGrpMaterialModifier *pMod )
{
	SceneMgrList::iterator iter = m_CreatedSceneMgrList.begin();
	while( iter!=m_CreatedSceneMgrList.end() )
	{
		(*iter)->ModifyEntityGroupMaterial( pMod );
		++iter;
	}
}

void ioUI3DRender::AddSceneMgr( ioSceneManager *pSceneMgr )
{
	m_CreatedSceneMgrList.insert( pSceneMgr );
}

void ioUI3DRender::RemoveSceneMgr( ioSceneManager *pSceneMgr )
{
	m_CreatedSceneMgrList.erase( pSceneMgr );
}

void ioUI3DRender::Render( const D3DXMATRIX *pExtraProj, bool bZClear )
{
	if( !m_pSceneMgr || !m_pCamera )
		return;

	if( m_pSceneMgr->GetEntityGrpCnt() > 0 || !m_EffectList.empty() )
	{
		// 소멸은 g_UIRenderer가 해준다.
		io3DUIQuad *pQuad = new io3DUIQuad( this, pExtraProj, bZClear );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}
}

void ioUI3DRender::RenderNow( const D3DXMATRIX *pExtraProj, bool bZClear )
{
	if( !m_pSceneMgr || !m_pCamera )
		return;

	ioLookAtCameraInput kInput;
	m_pCamera->Update( &kInput );

	ApplyUIAmbient();

	m_pSceneMgr->UpdateSceneGraph( m_pCamera, pExtraProj );

	if( bZClear )
	{
		m_pSceneMgr->ClearBack( D3DCLEAR_ZBUFFER );
	}

	m_pSceneMgr->RenderVisibleObject();

	RestoreAmbient();
}

void ioUI3DRender::RenderViewPort( const D3DRECT *pRcView, bool bZClear )
{
	io3DUIViewPortQuad *pQuad = new io3DUIViewPortQuad( this, pRcView, bZClear );
	if( pQuad )
	{
		g_UIRenderer.AddUIQuad( pQuad );
	}
}

void ioUI3DRender::Destroy()
{
	SAFEDELETE(m_pCamera);

	if( m_pSceneMgr )
	{
        RemoveSceneMgr( m_pSceneMgr );
		DestroyAllEffects();

		m_pSceneMgr->ClearScene();
		delete m_pSceneMgr;
		m_pSceneMgr = NULL;
	}
}

void ioUI3DRender::SetCameraPos( const D3DXVECTOR3 &vPos )
{
	if( !m_pCamera )	return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		pCtrl->SetPosition( vPos );
	}
}

void ioUI3DRender::SetCameraPos( float fXPos, float fYPos, float fZPos )
{
	SetCameraPos( D3DXVECTOR3( fXPos, fYPos, fZPos ) );
}

void ioUI3DRender::SetLookAt( const D3DXVECTOR3 &vLookAt )
{
	if( !m_pCamera )	return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		pCtrl->SetLookAt( vLookAt );
	}
}

D3DXMATRIX ioUI3DRender::MakeUIProjection( int iXPos, int iYPos )
{
	D3DXMATRIX matUIProj;
	D3DXMatrixIdentity( &matUIProj );

	float fCurWidth  = (float)Setting::Width();
	float fCurHeight = (float)Setting::Height();

	float fWidthRate  = 800.0f / (float)fCurWidth;
	float fHeightRate = 600.0f / (float)fCurHeight;

	float fMoveX = ( (float)iXPos / (float)fCurWidth ) * 2.0f;
	float fMoveY = ( (float)iYPos / (float)fCurHeight ) * 2.0f;

	matUIProj._11 = fWidthRate;
	matUIProj._22 = fHeightRate;
	matUIProj._41 = -( FLOAT1 - fWidthRate ) + fMoveX;
	matUIProj._42 = ( FLOAT1 - fHeightRate ) - fMoveY;

	return matUIProj;
}

D3DXMATRIX ioUI3DRender::MakeUIProjection( float vwidth, float vheight, float fnear, float ffar )
{
	D3DXMATRIX matUIProj;
	D3DXMatrixIdentity( &matUIProj );

	float fCurWidth  = (float)Setting::Width();
	float fCurHeight = (float)Setting::Height();

	float fWidthRate  = 800.0f / (float)fCurWidth;
	float fHeightRate = 600.0f / (float)fCurHeight;

	float fMoveX = ( (float)0 / (float)fCurWidth ) * 2.0f;
	float fMoveY = ( (float)0 / (float)fCurHeight ) * 2.0f;

	matUIProj._11 = fWidthRate;
	matUIProj._22 = fHeightRate;
	matUIProj._41 = -( FLOAT1 - fWidthRate ) + fMoveX;
	matUIProj._42 = ( FLOAT1 - fHeightRate ) - fMoveY;

	D3DXMATRIX matUIProj2;
	D3DXMatrixIdentity( &matUIProj2 );

	float n = fnear;
	float f = ffar;
	float w = vwidth;
	float h = vheight;

	matUIProj2._11 = 2 * n / w;
	matUIProj2._22 = 2 * n / h;
	matUIProj2._33 = f / ( f - n );
	matUIProj2._34 = FLOAT1;
	matUIProj2._43 = ( -f * n ) / ( f - n );

	return matUIProj2;;
}

void ioUI3DRender::SetLookAt( float fXPos, float fYPos, float fZPos )
{
	SetLookAt( D3DXVECTOR3( fXPos, fYPos, fZPos ) );
}

void ioUI3DRender::SetProjection( float fNear, float fFar, float fFov, float fAspect )
{
	if( m_pCamera )
	{
		m_pCamera->SetProjection( fNear, fFar, fFov, fAspect );
	}
}

D3DXVECTOR3 ioUI3DRender::GetCameraPosition() const
{
	if( !m_pCamera )	return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		return pCtrl->GetPosition();
	}
	return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

void ioUI3DRender::ApplyUIAmbient()
{
	if( m_pSceneMgr )
	{
		m_BackUpAmbient = m_pSceneMgr->GetSceneAmbient();
		m_pSceneMgr->SetSceneAmbient( m_UISceneAmbient.r, m_UISceneAmbient.g, m_UISceneAmbient.b );
	}
}

void ioUI3DRender::RestoreAmbient()
{
	if( m_pSceneMgr )
	{
		m_pSceneMgr->SetSceneAmbient( m_BackUpAmbient.r, m_BackUpAmbient.g, m_BackUpAmbient.b );
	}
}

void ioUI3DRender::SetMainLight( const D3DXVECTOR3 &vDir,
								 const D3DCOLORVALUE &rkDiffuse,
								 const D3DCOLORVALUE &rkSpecular,
								 const ioHashString &rkLightTexture )
{
	ioLight *pMainLight = m_pSceneMgr->GetLight( "MainLight" );
	if( !pMainLight )
	{
		pMainLight = m_pSceneMgr->CreateLight( "MainLight" );
		pMainLight->SetAttribute( ioLight::LAM_REAL_TIME );
		pMainLight->SetType( LT_DIRECTIONAL );
	}

	pMainLight->SetDiffuseColor( rkDiffuse.r, rkDiffuse.g, rkDiffuse.b );
	pMainLight->SetSpecularColor( rkSpecular.r, rkSpecular.g, rkSpecular.b );
	pMainLight->SetLightTexture( rkLightTexture );

	D3DXVECTOR3 vNormalizeDir;
	D3DXVec3Normalize( &vNormalizeDir, &vDir );
	pMainLight->SetDirection( vNormalizeDir );
}

ioCreateChar* ioUI3DRender::LoadChar( const CHARACTER &rkInfo,
									  const ioHashString &rkName,
									  bool bThread )
{
	ioEntityGroup *pGrp = m_pSceneMgr->CreateEntityGrp();
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioUI3DRender::LoadChar - CreateEntityGrp Fail" );
		return NULL;
	}

	ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
	if( !pSNode )
	{
		LOG.PrintTimeAndLog( 0, "ioUI3DRender::LoadChar - CreateChildSceneNode Fail" );
		return NULL;
	}

	pSNode->AttachObject( pGrp );
	
	RaceDetailType eType = GetRaceDetailType( rkInfo );
	switch( eType )
	{
	case RDT_HUMAN_MAN:
		pGrp->LoadGroup( "animodel/sp2_human_man.txt", bThread );		
		break;
	case RDT_HUMAN_WOMAN:
		pGrp->LoadGroup( "animodel/sp2_human_woman.txt", bThread );
		break;
	case RDT_ELF_MAN:
		pGrp->LoadGroup( "animodel/sp2_elf_man.txt", bThread );
		break;
	case RDT_ELF_WOMAN:
		pGrp->LoadGroup( "animodel/sp2_elf_woman.txt", bThread );
		break;
	case RDT_DWARF_MAN:
		pGrp->LoadGroup( "animodel/sp2_dwarf_man.txt", bThread );
		break;
	case RDT_DWARF_WOMAN:
		pGrp->LoadGroup( "animodel/sp2_dwarf_woman.txt", bThread );
		break;
	}

	RaceType eRace = (RaceType)( (int)eType / 2 );

	bool bMale = true;
	if( (int)eType & 1 )
		bMale = false;

	ioCreateChar *pNewChar = new ioCreateChar( pGrp );
	if( !pNewChar )
	{
		LOG.PrintTimeAndLog( 0, "ioUI3DRender::LoadChar - CreateChar NULL" );
		return NULL;
	}

	pNewChar->Initialize( eRace, bMale, bThread );
	pNewChar->SetName( rkName );

	ioUICharAniEventHandler *pHandler = new ioUICharAniEventHandler;
	pHandler->SetCreateChar( pNewChar );
	pGrp->SetAniEventHandler( pHandler );

	return pNewChar;
}

bool ioUI3DRender::AddEffect( const ioHashString &rkName,
							  const D3DXVECTOR3 &vPos,
							  const D3DXVECTOR3 &vScale,
							  const D3DXVECTOR3 &vDir )
{
	if( rkName.IsEmpty() )
		return false;

	ioEffect *pEffect = g_EffectFactory.CreateEffect( rkName );
	if( !pEffect )	return false;

	ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
	pSNode->AttachObject( pEffect );
	pSNode->SetPosition( vPos );
	pSNode->SetScale( vScale );

	if( !ioMath::IsEqual( ioMath::UNIT_Y, vDir ) )
	{
		pEffect->ChangeEffectDirection( vDir, true );
	}

	m_EffectList.push_back( pEffect );

	return true;
}

void ioUI3DRender::UpdateEffectList()
{
	if( m_EffectList.empty() )
		return;

	float fTimeGap = g_FrameTimer.GetCurFrameGap();

	EffectList::iterator iter = m_EffectList.begin();
	while( iter != m_EffectList.end() )
	{
		ioEffect *pEffect = *iter;

		if( pEffect->IsLive() )
		{
			pEffect->Update( fTimeGap );
			++iter;
		}
		else
		{
			iter = m_EffectList.erase( iter );
			DestroyEffect( pEffect );
		}
	}
}

void ioUI3DRender::DestroyEffect( ioEffect *pEffect )
{
	if( !pEffect )	return;

	ioSceneNode *pScene = pEffect->GetParentSceneNode();
	if( pScene )
	{
		pScene->RemoveAndDestroyAllChildren();
		m_pSceneMgr->DestroySceneNode( pScene->GetUniqueID() );
	}

	g_EffectFactory.DestroyEffect( pEffect );
}

void ioUI3DRender::DestroyAllEffects()
{
	if( m_EffectList.empty() )
		return;

	EffectList::iterator iter = m_EffectList.begin();
	for( ; iter!=m_EffectList.end() ; ++iter )
	{
		DestroyEffect( *iter );
	}

	m_EffectList.clear();
}

const ioRay& ioUI3DRender::GetCurPickRay() const
{
	return m_pSceneMgr->GetCurCameraPickRay();
}

ioHashString ioUI3DRender::GetModelAnimation( int iIndex, bool bMale ) const
{
	int iModelAnimationCount = m_ModelAnimation.size();
	if( !COMPARE( iIndex, 0, iModelAnimationCount ) )
		return ioHashString();

	ioHashString szNewAniName = Help::ConvertTailName( m_ModelAnimation[iIndex], bMale );

	if( !bMale )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\animation\\%s" , szNewAniName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			szNewAniName = Help::ConvertTailName( m_ModelAnimation[iIndex], true ); // 남자로
		}
	}
	
	return szNewAniName;
}

ioCreateChar* ioUI3DRender::CreateChar( const CHARACTER &rkInfo,
										const ioHashString &rkName,
										bool bThread )
{
	ioCreateChar *pNewChar = LoadChar( rkInfo, rkName, bThread );
	if( pNewChar )
	{
		ChangeBeard( pNewChar, rkInfo );
		ChangeFace( pNewChar, rkInfo );
		ChangeHair( pNewChar, rkInfo );
		ChangeAccessory( pNewChar, rkInfo );
		ChangeUnderwear( pNewChar, rkInfo );

		ChangeHairColor( pNewChar, rkInfo );
		ChangeSkinColor( pNewChar, rkInfo );
	}

	return pNewChar;
}

void ioUI3DRender::ChangeModel( ioCreateChar* &pOldChar, const CHARACTER &rkInfo )
{
	if( !pOldChar )	return;

	ioCreateChar *pNewChar = LoadChar( rkInfo, pOldChar->GetName(), true );
	if( pNewChar )
	{
		ChangeBeard( pNewChar, rkInfo );
		ChangeFace( pNewChar, rkInfo );
		ChangeHair( pNewChar, rkInfo );
		ChangeAccessory( pNewChar, rkInfo );
		ChangeUnderwear( pNewChar, rkInfo );

		ChangeHairColor( pNewChar, rkInfo );
		ChangeSkinColor( pNewChar, rkInfo );

		pOldChar->InheritToNewChar( pNewChar );
	}

	SAFEDELETE( pOldChar );
	pOldChar = pNewChar;
}

void ioUI3DRender::ChangeDeco( ioCreateChar* pChar, const CHARACTER &rkInfo )
{
	if( !pChar ) return;

	ChangeBeard( pChar, rkInfo );
	ChangeFace( pChar, rkInfo );
	ChangeHair( pChar, rkInfo );
	ChangeAccessory( pChar, rkInfo );
	ChangeUnderwear( pChar, rkInfo );
	ChangeHairColor( pChar, rkInfo );
	ChangeSkinColor( pChar, rkInfo );
}

void ioUI3DRender::ChangeBeard( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );

	ioHashString szBeard = RaceSetting::GetBeardParts( rkInfo.m_beard );
	pChar->SetDefaultBeard( szBeard );
}

void ioUI3DRender::ChangeFace( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );

	ioHashString szFace = RaceSetting::GetFaceParts( rkInfo.m_face );
	ioHashString szEffect = RaceSetting::GetFaceEffect( rkInfo.m_face );
	pChar->SetDefaultFace( szFace, szEffect, rkInfo.m_face );
}

void ioUI3DRender::ChangeHair( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );

	ioHashString szHair = RaceSetting::GetHairParts( rkInfo.m_hair );
	pChar->SetDefaultHair( szHair );
}

void ioUI3DRender::ChangeAccessory( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );

	ioHashString szAccessories = RaceSetting::GetAccessoryParts( rkInfo.m_accessories );
	pChar->SetDefaultAccessory( szAccessories );
}

void ioUI3DRender::ChangeSkinColor( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );

	D3DCOLORVALUE kSkinColor = RaceSetting::GetSkinColor( rkInfo.m_skin_color );
	D3DCOLORVALUE kSkinOutLineColor = RaceSetting::GetSkinOutLineColor( rkInfo.m_skin_color );
	pChar->SetSkinColor( kSkinColor, kSkinOutLineColor );
}

void ioUI3DRender::ChangeHairColor( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );
	
	ioHashString szColorTex = RaceSetting::GetHairColorTex( rkInfo.m_hair, rkInfo.m_hair_color );
	D3DCOLORVALUE kOutLineColor = RaceSetting::GetHairOutLineColor( rkInfo.m_hair, rkInfo.m_hair_color );
	pChar->SetHairColor( szColorTex, kOutLineColor );

	if( rkInfo.m_beard > 0 && !pChar->GetDefaultBeardMesh().IsEmpty() )
	{
		szColorTex    = RaceSetting::GetBeardColorTex( rkInfo.m_beard, rkInfo.m_hair_color );
		kOutLineColor = RaceSetting::GetBeardOutLineColor( rkInfo.m_beard, rkInfo.m_hair_color );
		pChar->SetBeardColor( szColorTex, kOutLineColor );
	}
}

void ioUI3DRender::ChangeUnderwear( ioCreateChar *pChar, const CHARACTER &rkInfo )
{
	if( !pChar )	return;

	RaceSetting::SetCurRaceDetail( rkInfo );

	ioHashString szUnderwear = RaceSetting::GetUnderwearParts( rkInfo.m_underwear );
	pChar->SetDefaultUnderwear( szUnderwear );
}

void ioUI3DRender::UpdateCameraByMouse( float fForward, float fStrafe, float fRot )
{
	if( !m_pCamera ) return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( !pCtrl ) return;

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	pCtrl->SetSpeed( fForward, fStrafe, fRot );

	ioLookAtCameraInput kInput;
	kInput.m_fXMoveAmt = (float)g_App.GetMouseAmount().x;
	kInput.m_fYMoveAmt = (float)g_App.GetMouseAmount().y;
	kInput.m_fWheelAmt = 0.0f;
	
	if( pMouse->GetWheelDelta() == 0 )
	{
		if( pMouse->IsRBtnDown() )
		{
			kInput.m_bMiddle = true;
		}
		else if( pMouse->IsLBtnDown() )
		{
			if( kInput.m_fYMoveAmt > 0 )
				kInput.m_bForward = true;
			if( kInput.m_fYMoveAmt < 0 )
				kInput.m_bBack = true;
			if( kInput.m_fXMoveAmt > 0 )
				kInput.m_bLeft = true;
			if( kInput.m_fXMoveAmt < 0 )
				kInput.m_bRight = true;
		}
	}
	else
	{
		kInput.m_fXMoveAmt = 0.0f;
		kInput.m_fYMoveAmt = -( (float)pMouse->GetWheelDelta() / 15.0f );
		kInput.m_bZoom = true;
	}
	m_pCamera->Update( &kInput );
}

ioPetView* ioUI3DRender::CreatePet( const ioHashString &rkName,	bool bThread )
{
	ioEntityGroup *pGrp = m_pSceneMgr->CreateEntityGrp();
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioUI3DRender::LoadPet - CreateEntityGrp Fail" );
		return NULL;
	}

	ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
	if( !pSNode )
	{
		LOG.PrintTimeAndLog( 0, "ioUI3DRender::LoadPet - CreateChildSceneNode Fail" );
		return NULL;
	}

	pSNode->AttachObject( pGrp );
	
	pGrp->LoadGroup( rkName.c_str(), true );
	ioPetView *pNewPet = new ioPetView( pGrp );
	if( !pNewPet )
	{
		LOG.PrintTimeAndLog( 0, "ioUI3DRender::LoadPet - CreatePet NULL" );
		return NULL;
	}

	ioUIPetAniEventHandler *pHandler = new ioUIPetAniEventHandler;
	pHandler->SetCreatePet( pNewPet );
	pGrp->SetAniEventHandler( pHandler );

	return pNewPet;
}