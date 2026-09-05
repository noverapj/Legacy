
#include "stdafx.h"
#include "ioUIRenderer.h"
#include "ioRenderSystem.h"
#include "ioINILoader.h"
#include "ioCamera.h"
#include "ioCameraController.h"
#include "ioLookAtCameraController.h"
#include "ioSceneManager.h"
#include "ioEffectFactory.h"
#include "ioNode.h"
#include "ioSceneNode.h"
#include "ioEffect.h"
#include "ioUI3DEffectRender.h"

class io3DEffectUIQuad : public IUIQuad
{
protected:
	ioUI3DEffectRender *m_p3DRender;
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
	io3DEffectUIQuad( ioUI3DEffectRender *pRender, const D3DXMATRIX *pExtraProj, bool bZClear )
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

	virtual ~io3DEffectUIQuad(){}
};
//////////////////////////////////////////////////////////////////////////
class io3DEffectUIViewPortQuad : public io3DEffectUIQuad
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
		kNewView.MaxZ = 1.0f;
		pRenderSystem->SetViewPort( kNewView );

		if( m_p3DRender )
			m_p3DRender->RenderNow( NULL, m_bZClear );

		pRenderSystem->SetViewPort( kOldView );
	}

public:
	io3DEffectUIViewPortQuad( ioUI3DEffectRender *pRender, const D3DRECT *pRcView, bool bZClear ) : io3DEffectUIQuad( pRender, NULL, bZClear )
	{
		if( pRcView )
			m_rcView = *pRcView;
		else
			LOG.PrintTimeAndLog( 0, "io3DEffectUIViewPortQuad ERROR" );
	}
	virtual ~io3DEffectUIViewPortQuad(){}
};
//////////////////////////////////////////////////////////////////////////
ioUI3DEffectRender::ioUI3DEffectRender()
{
	m_pSceneMgr = NULL;
	m_pCamera = NULL;

	m_iXPos = 0;
	m_iYPos = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iRenderType = 0;

	m_bShow = true;

	m_fDist = 0.0f;
}

ioUI3DEffectRender::~ioUI3DEffectRender()
{
	Destroy();
}

void ioUI3DEffectRender::Initialize( ioHashString &szType, ioHashString &szININame, ioHashString &szCamera)
{
	m_szType = szType;

    if (szININame.IsEmpty())
        szININame = "config/sp2_3DEffect_cam.ini";

    if (szCamera.IsEmpty())
        szCamera = "UIDefault";

    ioINILoader kLoader( szININame.c_str() );

	m_pCamera = new ioCamera;

    kLoader.SetTitle( szCamera.c_str() );
	
	D3DXVECTOR3 vCamPos;
	vCamPos.x = kLoader.LoadFloat( "cam_offset_x", 0.0f );
	vCamPos.y = kLoader.LoadFloat( "cam_offset_y", 0.0f );
	vCamPos.z = kLoader.LoadFloat( "cam_offset_z", 0.0f );

	D3DXVECTOR3 vLookAtPos;
	vLookAtPos.x = kLoader.LoadFloat( "lookat_offset_x", 0.0f );
	vLookAtPos.y = kLoader.LoadFloat( "lookat_offset_y", 0.0f );
	vLookAtPos.z = kLoader.LoadFloat( "lookat_offset_z", 200.0f );

	float fNearP = kLoader.LoadFloat( "cam_near", 100.0f );
	float fFarP = kLoader.LoadFloat( "cam_far", 10000.0f );
	float fFov = kLoader.LoadFloat( "cam_fov", 45.0f );
	float fAspectRatio = 1.6f; // 1280 / 800 = 1.6

	ioLookAtCameraController *pCtrl = new ioLookAtCameraController;

	pCtrl->SetPosition( vCamPos );
	pCtrl->SetLookAt( vLookAtPos );
	pCtrl->SetSpeed( 0.0f, 0.0f, 0.0f );
	m_pCamera->SetController( pCtrl );

	m_pCamera->SetProjection( fNearP, fFarP, fFov, fAspectRatio );

	m_pSceneMgr = new ioSceneManager( &RenderSystem() );

	//------------------------- Init Lights ---------------------

	kLoader.SetTitle( "ambient" );
	m_UISceneAmbient.r = kLoader.LoadFloat( "red", 127.0f ) / 255.0f;
	m_UISceneAmbient.g = kLoader.LoadFloat( "green", 127.0f ) / 255.0f;
	m_UISceneAmbient.b = kLoader.LoadFloat( "blue", 127.0f ) / 255.0f;

	char szBuf[MAX_PATH];
	kLoader.SetTitle( "main_light" );
	kLoader.LoadString( "toon_texture", "toongray.dds", szBuf, MAX_PATH );

	D3DCOLORVALUE rDiffuse;
	rDiffuse.r = kLoader.LoadFloat( "red", 127.0f ) / 255.0f;
	rDiffuse.g = kLoader.LoadFloat( "green", 127.0f ) / 255.0f;
	rDiffuse.b = kLoader.LoadFloat( "blue", 127.0f ) / 255.0f;
	rDiffuse.a = 1.0f;

	D3DXVECTOR3 vPos,vTarget;
	vPos.x = kLoader.LoadFloat( "pos_x", 0.0f );
	vPos.y = kLoader.LoadFloat( "pos_y", 0.0f );
	vPos.z = kLoader.LoadFloat( "pos_z", 0.0f );

	vTarget.x = kLoader.LoadFloat( "target_x", 0.0f );
	vTarget.y = kLoader.LoadFloat( "target_y", -1.0f );
	vTarget.z = kLoader.LoadFloat( "target_z", -1.0f );

	D3DXVECTOR3 vLightDir = vTarget - vPos;				// Light Direction Calc
	D3DXVec3Normalize( &vLightDir, &vLightDir );
	SetMainLight( vLightDir, rDiffuse, rDiffuse, szBuf );
}

void ioUI3DEffectRender::RenderNow( const D3DXMATRIX *pExtraProj, bool bZClear )
{
	if( !m_pSceneMgr || !m_pCamera )
		return;
	if( m_EffectList.empty() )
		return;

	if(m_bShow == false) 
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

void ioUI3DEffectRender::RenderViewPort( const D3DRECT *pRcView, bool bZClear )
{
	if( m_EffectList.empty() )
		return;
	if(m_bShow == false) 
		return;

	io3DEffectUIViewPortQuad *pQuad = new io3DEffectUIViewPortQuad( this, pRcView, bZClear );
	if( pQuad )
		g_UIRenderer.AddUIQuad( pQuad );
}

void ioUI3DEffectRender::Render( const D3DXMATRIX *pExtraProj, bool bZClear )
{
    if( !m_pSceneMgr || !m_pCamera )
        return;

    if( m_pSceneMgr->GetEntityGrpCnt() > 0 || !m_EffectList.empty() )
    {
        io3DEffectUIQuad *pQuad = new io3DEffectUIQuad( this, pExtraProj, true );
        if( pQuad )
        {
            g_UIRenderer.AddUIQuad( pQuad );
        }
    }
}

void ioUI3DEffectRender::Render( int iRenderType, int iXPos, int iYPos )
{
	if( iRenderType != m_iRenderType )
		return;

	// 절대좌표면 부모위치 무시.
	if(m_bPosAbsolute)
	{
		iXPos = 0;
		iYPos = 0;
	}

	Render( iXPos, iYPos );
}

void ioUI3DEffectRender::Render( int iXPos, int iYPos )
{
	if(m_bShow == false) 
		return;

    D3DXMATRIX kExtraProj = MakeUIProjection( iXPos + m_iXPos, iYPos + m_iYPos, m_iWidth, m_iHeight );
    Render( &kExtraProj, true );
}


void ioUI3DEffectRender::Destroy()
{
	SAFEDELETE(m_pCamera);

	if( m_pSceneMgr )
	{
        ClearInfoList();
		DestroyAllEffects();

		m_pSceneMgr->ClearScene();
        SAFEDELETE(m_pSceneMgr);
	}
}

void ioUI3DEffectRender::SetLookAt( const D3DXVECTOR3 &vLookAt )
{
	if( !m_pCamera )	return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		pCtrl->SetLookAt( vLookAt );
	}
}

D3DXMATRIX ioUI3DEffectRender::MakeUIProjection( int iXPos, int iYPos, int iWidth, int iHeight )
{
	D3DXMATRIX matUIProj;
	D3DXMatrixIdentity( &matUIProj );

    float fCurWidth  = (float)RenderSystem().GetViewPort().Width;
    float fCurHeight = (float)RenderSystem().GetViewPort().Height;

	float fWidthRate  = iWidth / (float)fCurWidth;
	float fHeightRate = iHeight / (float)fCurHeight;

	float fMoveX = ( (float)iXPos / (float)fCurWidth ) * 2.0f;
	float fMoveY = ( (float)iYPos / (float)fCurHeight ) * 2.0f;

	matUIProj._11 = fWidthRate;
	matUIProj._22 = fHeightRate;
	matUIProj._41 = -( 1.0f - fWidthRate ) + fMoveX;
	matUIProj._42 = ( 1.0f - fHeightRate ) - fMoveY;

	if(m_bPosAbsolute)
	{
		matUIProj._41 = fMoveX;
		matUIProj._42 = fMoveY;
	}

	return matUIProj;
}

void ioUI3DEffectRender::ApplyUIAmbient()
{
	if( m_pSceneMgr )
	{
		m_BackUpAmbient = m_pSceneMgr->GetSceneAmbient();
		m_pSceneMgr->SetSceneAmbient( m_UISceneAmbient.r, m_UISceneAmbient.g, m_UISceneAmbient.b );
	}
}

void ioUI3DEffectRender::RestoreAmbient()
{
	if( m_pSceneMgr )
	{
		m_pSceneMgr->SetSceneAmbient( m_BackUpAmbient.r, m_BackUpAmbient.g, m_BackUpAmbient.b );
	}
}

void ioUI3DEffectRender::SetMainLight( const D3DXVECTOR3 &vDir,
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

bool ioUI3DEffectRender::AddEffect( const ioHashString &szName,
                                    const D3DXVECTOR3 &vPos,
                                    const D3DXVECTOR3 &vScale,
                                    const D3DXVECTOR3 &vDir,
                                    const bool bStart)
{
    return AddEffect(szName, "NormalRender", vPos, vScale, vDir, bStart);
}

bool ioUI3DEffectRender::AddEffect( const ioHashString &rkName,
                                    const ioHashString &szRenderType,
							        const D3DXVECTOR3 &vPos,
							        const D3DXVECTOR3 &vScale,
							        const D3DXVECTOR3 &vDir,
                                    const bool bStart )
{
	if( rkName.IsEmpty() )
		return false;

    m_iRenderType = ParserRenderType(szRenderType);

    UIEffectInfo kInfo;
    kInfo.m_szName = rkName;
    kInfo.m_vPos   = vPos;
    kInfo.m_vScale = vScale;
    kInfo.m_vDir   = vDir;

    m_UIEffectInfoList.push_back(kInfo);

    if (bStart)
    {
        Start();
    }

	return true;
}

void ioUI3DEffectRender::ClearInfoList()
{
    m_UIEffectInfoList.clear();
}

void ioUI3DEffectRender::Start()
{
    if (m_UIEffectInfoList.empty())
    {
        return;
    }

    Stop();

    UIEffectInfoList::iterator iter = m_UIEffectInfoList.begin();
    while (iter != m_UIEffectInfoList.end())
    {
        UIEffectInfo* pInfo = &(*iter);
        if (pInfo == NULL)
        {
			++iter;
            continue;
        }

        if (pInfo->m_szName.IsEmpty())
        {
			++iter;
            continue;
        }

        ioEffect *pEffect = g_EffectFactory.CreateEffect(pInfo->m_szName);
        if (pEffect == NULL)
        {
			++iter;
            continue;
        }

        ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
        pSNode->AttachObject(pEffect);
        pSNode->SetPosition(pInfo->m_vPos);
        pSNode->SetScale(pInfo->m_vScale);

        if (ioMath::IsEqual(ioMath::UNIT_Y, pInfo->m_vDir) == false)
        {
            pEffect->ChangeEffectDirection(pInfo->m_vDir, true);
            m_fDist = pInfo->m_vDir.z;
        }
        pEffect->Update(0.1f);
        m_EffectList.push_back(pEffect);

        ++iter;
    }
}

void ioUI3DEffectRender::Stop()
{
    DestroyAllEffects();
}

bool ioUI3DEffectRender::IsLive()
{
    return (m_EffectList.size() > 0);
}

void ioUI3DEffectRender::UpdateEffectList( float fFramesPerSec )
{
	if( m_EffectList.empty() )
		return;

	EffectList::iterator iter = m_EffectList.begin();
	while( iter != m_EffectList.end() )
	{
		ioEffect *pEffect = *iter;

		if( pEffect->IsLive() )
		{
			pEffect->Update( fFramesPerSec );
			++iter;
		}
		else
		{
			iter = m_EffectList.erase( iter );
			DestroyEffect( pEffect );
		}
	}
}

void ioUI3DEffectRender::DestroyEffect( ioEffect *pEffect )
{
	if( !pEffect )	return;

	ioSceneNode *pScene = pEffect->GetParentSceneNode();
	if( pScene )
	{
		pScene->RemoveAndDestroyAllChildren();
		m_pSceneMgr->DestroySceneNode( pScene->GetUniqueID() );
	}

	if( ioEffectFactory::GetSingletonPtr() != NULL )
		g_EffectFactory.DestroyEffect( pEffect );
}

void ioUI3DEffectRender::DestroyAllEffects()
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

D3DXVECTOR3 ioUI3DEffectRender::GetLookAt()
{
	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		return pCtrl->GetLookAt();
	}

	return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

D3DXVECTOR3 ioUI3DEffectRender::GetCamPos()
{
	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		return pCtrl->GetPosition();
	}
	return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void ioUI3DEffectRender::SetCamPos(D3DXVECTOR3 vPos)
{
	ioLookAtCameraController *pCtrl = ToLookAtCtrl( m_pCamera->GetController() );
	if( pCtrl )
	{
		pCtrl->SetPosition(vPos);
	}
}

void ioUI3DEffectRender::SetSize(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    if( m_pCamera )
    {
        float fAspectRatio = (float)m_iWidth / (float)m_iHeight;
        m_pCamera->SetAspectRatio(fAspectRatio);
    }
}

int ioUI3DEffectRender::ParserRenderType(const ioHashString &szRenderType)
{
    if (szRenderType == "PreRender")
        return ioUI3DEffectRender::eERT_PRERENDER;
    else if (szRenderType == "NormalRender")
        return ioUI3DEffectRender::eERT_NORMALRENDER;
    else if (szRenderType == "AfterRender")
        return ioUI3DEffectRender::eERT_AFTERRENDER;

    return 0;
}

void ioUI3DEffectRender::SetDistince( float fdist, int nIndex )
{
	if(m_EffectList.size() > fdist)
	{
		D3DXVECTOR3 vDir = D3DXVECTOR3(0.0f, 0.0f, fdist);
		if( !ioMath::IsEqual( ioMath::UNIT_Y, vDir ) )
		{
			m_EffectList[nIndex]->ChangeEffectDirection( vDir, true );
			m_fDist = fdist;
		}
	}
}

float ioUI3DEffectRender::GetDistince()
{
	return m_fDist;
}
