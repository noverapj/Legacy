

#include "stdafx.h"

#include "ioAutoShaderParamSource.h"
#include "ioRenderable.h"
#include "ioCamera.h"
#include "ioPass.h"
#include "ioTextureUnitState.h"
#include "HelpFunc.h"

ioAutoShaderParamSource::ioAutoShaderParamSource()
{
	D3DXMatrixIdentity( &m_matIdentity );

	m_ShadowColor.r = m_ShadowColor.g = m_ShadowColor.b = 0.0f;
	m_ShadowColor.a = 1.0f;

	m_bWorldMatrixDirty = true;
	m_bWorldViewMatrixDirty = true;
	m_bViewProjMatrixDirty = true;
	m_bWorldViewProjMatrixDirty = true;
	m_bInvWorldMatrixDirty = true;
	m_bInvWorldViewMatrixDirty = true;
	m_bInvTransposeWorldMatrixDirty = true;
	m_bInvTransposeWorldViewMatrixDirty = true;

	m_bLightWorldViewProjMatrixDirty = true;
	m_bShadowMapMatrixDirty = true;

	m_bCameraPosObjectSpaceDirty = true;
	m_bCameraDirObjectSpaceDirty = true;
	m_bAlphaFactorDirty = true;
	m_bOutLineConstantDirty = true;

	m_pCurPass = NULL;
	m_pCurRenderable = NULL;
	m_pCurCamera = NULL;
	m_pApplyLightList = NULL;

	m_bIgnoreAmbient = false;
	m_bAmbientLightChanged = true;
}

ioAutoShaderParamSource::~ioAutoShaderParamSource()
{
}

void ioAutoShaderParamSource::SetCurrentPass( const ioPass *pPass )
{
	m_pCurPass = pPass;
}

void ioAutoShaderParamSource::SetCurrentRenderable( const ioRenderable *pRend )
{
	m_pCurRenderable = pRend;

	m_bWorldMatrixDirty = true;
	m_bWorldViewMatrixDirty = true;
	m_bWorldViewProjMatrixDirty = true;
	m_bInvWorldMatrixDirty = true;
	m_bInvWorldViewMatrixDirty = true;
	m_bInvTransposeWorldMatrixDirty = true;
	m_bInvTransposeWorldViewMatrixDirty = true;

	m_bLightWorldViewProjMatrixDirty = true;
	m_bShadowMapMatrixDirty = true;

	m_bCameraPosObjectSpaceDirty = true;
	m_bCameraDirObjectSpaceDirty = true;

	m_bAlphaFactorDirty = true;
}

void ioAutoShaderParamSource::SetCurrentCamera( const ioCamera *pCam )
{
	m_pCurCamera = pCam;
	
	m_CameraPos = pCam->GetPosition();
	m_CameraDir = pCam->GetDirection();

	m_CameraFovNearFar.x = DEGtoRAD( pCam->GetFovX() );
	m_CameraFovNearFar.y = DEGtoRAD( pCam->GetFovY() );
	m_CameraFovNearFar.z = pCam->GetNearP();
	m_CameraFovNearFar.w = pCam->GetFarP();

	m_bWorldViewMatrixDirty = true;
	m_bViewProjMatrixDirty = true;
	m_bWorldViewProjMatrixDirty = true;
	m_bInvWorldViewMatrixDirty = true;
	m_bInvTransposeWorldViewMatrixDirty = true;

	m_bProjMatrixDirty = true;

	m_bCameraPosObjectSpaceDirty = true;
	m_bCameraDirObjectSpaceDirty = true;
	m_bAlphaFactorDirty = true;
	m_bOutLineConstantDirty = true;
}

void ioAutoShaderParamSource::SetApplyLightList( const ioLightList *pList )
{
	m_pApplyLightList = pList;
}

void ioAutoShaderParamSource::SetLightViewProjMatrix( const D3DXMATRIX &matLightViewProj )
{
	m_LightViewProjMatrix = matLightViewProj;
	m_bLightWorldViewProjMatrixDirty = true;
	m_bShadowMapMatrixDirty = true;
}

void ioAutoShaderParamSource::SetShadowMapTexMatrix( const D3DXMATRIX &matShadowTex )
{
	m_ShadowMapTextureMatrix = matShadowTex;
	m_bShadowMapMatrixDirty = true;
}

void ioAutoShaderParamSource::SetViewPort( const D3DVIEWPORT9 &kViewPort )
{
	m_ViewPort.x = (float)kViewPort.Width;
	m_ViewPort.y = (float)kViewPort.Height;
	m_ViewPort.z = 1.0f / m_ViewPort.x;
	m_ViewPort.w = 1.0f / m_ViewPort.y;

	m_bOutLineConstantDirty = true;
}

void ioAutoShaderParamSource::SetExtraProjectionMatrix( const D3DXMATRIX *pExtraProj )
{
	if( pExtraProj )
	{
		m_ExtraProjMatrix = *pExtraProj;
	}
	else
	{
		D3DXMatrixIdentity( &m_ExtraProjMatrix );
	}

	m_bProjMatrixDirty = true;
}

void ioAutoShaderParamSource::SetIgnoreAmbient( bool bIgnore )
{
	m_bIgnoreAmbient = bIgnore;
}

void ioAutoShaderParamSource::SetShadowColor( const D3DCOLORVALUE &cShadow )
{
	m_ShadowColor = cShadow;
}

void ioAutoShaderParamSource::SetAmbientLightColor( const D3DCOLORVALUE &cAmbient )
{
	m_bAmbientLightChanged = false;

	if( m_AmbientLight != cAmbient )
	{
		m_AmbientLight = cAmbient;
		m_bAmbientLightChanged = true;
	}
}

void ioAutoShaderParamSource::SetFogFactor( const D3DXVECTOR4 &vFogFactor )
{
	m_FogFactor = vFogFactor;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetWorldMatrix() const
{
	if( m_bWorldMatrixDirty )
	{
		m_pCurRenderable->GetWorldTransform( &m_WorldMatrix );
		m_bWorldMatrixDirty = false;
	}

	return &m_WorldMatrix;
}

const D3DXVECTOR4* ioAutoShaderParamSource::GetBipedList( int &iVectorCnt ) const
{
	return m_pCurRenderable->GetBipedList( iVectorCnt );
}

const D3DXMATRIX* ioAutoShaderParamSource::GetViewMatrix() const
{
	return m_pCurCamera->GetViewMatrix();
}

const D3DXMATRIX* ioAutoShaderParamSource::GetViewProjectionMatrix() const
{
	if( m_bViewProjMatrixDirty )
	{
		D3DXMatrixMultiply( &m_ViewProjMatrix,
							GetViewMatrix(),
							GetProjectionMatrix() );

		m_bViewProjMatrixDirty = false;
	}

	return &m_ViewProjMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetProjectionMatrix() const
{
	if( m_bProjMatrixDirty )
	{
		D3DXMatrixMultiply( &m_ProjMatrix, m_pCurCamera->GetProjMatrix(), &m_ExtraProjMatrix );
		m_bProjMatrixDirty = false;
	}

	return &m_ProjMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetWorldViewMatrix() const
{
	if( m_bWorldViewMatrixDirty )
	{
		D3DXMatrixMultiply( &m_WorldViewMatrix,
							GetWorldMatrix(),
							GetViewMatrix() );

		m_bWorldViewMatrixDirty = false;
	}

	return &m_WorldViewMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetWorldViewProjMatrix() const
{
	if( m_bWorldViewProjMatrixDirty )
	{
		D3DXMatrixMultiply( &m_WorldViewProjMatrix,
							GetWorldMatrix(),
							GetViewProjectionMatrix() );

		m_bWorldViewProjMatrixDirty = false;
	}

	return &m_WorldViewProjMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetInvWorldMatrix() const
{
	if( m_bInvWorldMatrixDirty )
	{
		D3DXMatrixInverse( &m_InvWorldMatrix, NULL, GetWorldMatrix() );
		m_bInvWorldMatrixDirty = false;
	}

	return &m_InvWorldMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetInvWorldViewMatrix() const
{
	if( m_bInvWorldViewMatrixDirty )
	{
		D3DXMatrixInverse( &m_InvWorldViewMatrix, NULL, GetWorldViewMatrix() );
		m_bInvWorldViewMatrixDirty = false;
	}

	return &m_InvWorldViewMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetInvViewMatrix() const
{
	return m_pCurCamera->GetViewInverseMatrix();
}

const D3DXMATRIX* ioAutoShaderParamSource::GetInvTransposeWorldMatrix() const
{
	if( m_bInvTransposeWorldMatrixDirty )
	{
		D3DXMatrixTranspose( &m_InvTransposeWorldMatrix, GetInvWorldMatrix() );
		m_bInvTransposeWorldMatrixDirty = false;
	}

	return &m_InvTransposeWorldMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetInvTransposeWorldViewMatrix() const
{
	if( m_bInvTransposeWorldViewMatrixDirty )
	{
		D3DXMatrixTranspose( &m_InvTransposeWorldViewMatrix, GetInvWorldViewMatrix() );
		m_bInvTransposeWorldViewMatrixDirty = false;
	}

	return &m_InvTransposeWorldViewMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetTextureMatrix( int iUnitIndex ) const
{
	if( COMPARE( iUnitIndex, 0, m_pCurPass->GetNumTextureUnitStates() ) )
	{
		ioTextureUnitState *pState = m_pCurPass->GetTextureUnitState( iUnitIndex );
		if( pState )
		{
			return &pState->GetTextureTransform();
		}
	}

	return &m_matIdentity;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetLightWorldViewProjMatrix() const
{
	if( m_bLightWorldViewProjMatrixDirty )
	{
		D3DXMatrixMultiply( &m_LightWorldViewProjMatrix,
							GetWorldMatrix(),
							&m_LightViewProjMatrix );

		m_bLightWorldViewProjMatrixDirty = false;
	}

	return &m_LightWorldViewProjMatrix;
}

const D3DXMATRIX* ioAutoShaderParamSource::GetShadowMapMatrix() const
{
	if( m_bShadowMapMatrixDirty )
	{
		D3DXMatrixMultiply( &m_ShadowMapMatrix, 
							GetLightWorldViewProjMatrix(),
							&m_ShadowMapTextureMatrix );

		m_bShadowMapMatrixDirty = false;
	}

	return &m_ShadowMapMatrix;
}

const D3DXVECTOR4& ioAutoShaderParamSource::GetCameraPosObjectSpace() const
{
	if( m_bCameraPosObjectSpaceDirty )
	{
		const D3DXMATRIX *pWorldMtl = GetWorldMatrix();

		D3DXVECTOR3 vModelPos;
		vModelPos.x = pWorldMtl->_41;
		vModelPos.y = pWorldMtl->_42;
		vModelPos.z = pWorldMtl->_43;

		D3DXVECTOR3 vCamDiff = m_CameraPos - vModelPos;
		float fCamModelDist = D3DXVec3Length( &vCamDiff );

		D3DXVECTOR3 vCameraPosObjSpace;
		D3DXVec3TransformCoord( &vCameraPosObjSpace, &m_CameraPos, GetInvWorldMatrix() );
		D3DXVec3Normalize( &vCameraPosObjSpace, &vCameraPosObjSpace );

		m_CameraPosObjectSpace.x = vCameraPosObjSpace.x * fCamModelDist;
		m_CameraPosObjectSpace.y = vCameraPosObjSpace.y * fCamModelDist;
		m_CameraPosObjectSpace.z = vCameraPosObjSpace.z * fCamModelDist;
		m_CameraPosObjectSpace.w = 1.0f;

		m_bCameraPosObjectSpaceDirty = false;
	}

	return m_CameraPosObjectSpace;
}

const D3DXVECTOR4& ioAutoShaderParamSource::GetCameraDirObjectSpace() const
{
	if( m_bCameraDirObjectSpaceDirty )
	{
		D3DXVECTOR3 vCamDirObjSpace;
		D3DXVec3TransformNormal( &vCamDirObjSpace, &m_CameraDir, GetInvWorldMatrix() );
		D3DXVec3Normalize( &vCamDirObjSpace, &vCamDirObjSpace );

		m_CameraDirObjectSpace.x = vCamDirObjSpace.x;
		m_CameraDirObjectSpace.y = vCamDirObjSpace.y;
		m_CameraDirObjectSpace.z = vCamDirObjSpace.z;
		m_CameraDirObjectSpace.w = 0.0f;

		m_bCameraDirObjectSpaceDirty = false;
	}

	return m_CameraDirObjectSpace;
}

int ioAutoShaderParamSource::GetApplyLightCnt() const
{
	if( m_pApplyLightList )
		return m_pApplyLightList->size();

	return 0;
}

const ioLight* ioAutoShaderParamSource::GetApplyLight( int iLight ) const
{
	if( m_pApplyLightList &&
		COMPARE( iLight, 0, GetApplyLightCnt() ) )
	{
		return (*m_pApplyLightList)[iLight];
	}

	return NULL;
}

D3DCOLORVALUE ioAutoShaderParamSource::GetAmbientEmissiveColor() const
{
	if( !m_bIgnoreAmbient )
		return m_pCurPass->GetAmbientEmissiveColor( m_AmbientLight, m_bAmbientLightChanged );

	return m_pCurPass->GetEmissive();
}

D3DCOLORVALUE ioAutoShaderParamSource::GetMaterialDiffuseColor() const
{
	return m_pCurPass->GetDiffuse() * m_pCurRenderable->GetDiffuseRate();
}

const D3DXVECTOR4& ioAutoShaderParamSource::GetAlphaFactor() const
{
	if( m_bAlphaFactorDirty )
	{
		D3DXVECTOR2 vCamFactor = m_pCurCamera->GetDistAlphaFactor();
		
		m_AlphaFactor.x = vCamFactor.x;
		m_AlphaFactor.y = vCamFactor.y;
		m_AlphaFactor.z = m_pCurRenderable->GetAlphaRate() / 255.0f;
		m_bAlphaFactorDirty = false;
	}

	return m_AlphaFactor;
}

D3DXVECTOR4 ioAutoShaderParamSource::GetOutLineConstant() const
{
	if( m_bOutLineConstantDirty )
	{
		m_OutLineConstant.x = DEGtoRAD( m_pCurCamera->GetFovX() );
		m_OutLineConstant.y = m_ViewPort.x;
		m_OutLineConstant.z = m_OutLineConstant.x * m_ViewPort.z;	//	fovX / ViewWidth
		m_OutLineConstant.w = 1.0f;

		m_bOutLineConstantDirty = false;
	}

	return m_OutLineConstant;
}

int ioAutoShaderParamSource::GetWeightedBipedCnt() const
{
	return m_pCurRenderable->GetWeightedBipedCnt();
}
