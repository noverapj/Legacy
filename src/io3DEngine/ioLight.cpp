

#include "stdafx.h"

#include "ioLight.h"

#include "ioMath.h"
#include "HelpFunc.h"
#include "ioNode.h"
#include "ioLightSystem.h"
#include "ioFrameTimer.h"

#include "ioTexture.h"
#include "ioTextureManager.h"

ioLight::ioLight()
{
	m_pParentNode = NULL;

	m_LightType	= LT_POINT;
	m_dwLightAttribute = 0;
	m_dwLightApplyMask = 0xFFFFFFFF;

	m_Diffuse.r = m_Diffuse.g = m_Diffuse.b = m_Diffuse.a = 1.0f;
	m_Specular.r = m_Specular.g = m_Specular.b = m_Specular.a = 0.0f;

	m_pLightTexture = NULL;
	m_pDecalTexture = NULL;

	m_fSpotInner = 43.0f;
	m_fSpotOuter = 45.0f;
	m_fSpotFallOff = 1.0f;
	m_fRange = 5000.0f;

	m_fAttenuationConst  = 1.0f;
	m_fAttenuationLinear = 0.0f;
	m_fAttenuationQuad   = 0.0f;

	m_dwLightStartTime = 0;
	m_dwLightDuration  = 0;
	m_dwRatedUpdatedFrame = 0;

	m_vPosition = m_vDerivedPosition   = ioMath::VEC3_ZERO;
	m_vDirection = m_vDerivedDirection = ioMath::UNIT_Z;

	m_bNoUpdatePosition = false;
	m_bLightAttributeChange = true;
	m_bLocalTransformDirty = false;
}

ioLight::ioLight( const ioHashString &kName )
{
	m_pParentNode = NULL;

	m_Name = kName;
	m_LightType	= LT_POINT;
	m_dwLightAttribute = 0;
	m_dwLightApplyMask = 0xFFFFFFFF;

	m_Diffuse.r = m_Diffuse.g = m_Diffuse.b = m_Diffuse.a = 1.0f;
	m_Specular.r = m_Specular.g = m_Specular.b = m_Specular.a = 0.0f;

	m_pLightTexture = NULL;
	m_pDecalTexture = NULL;

	m_fSpotInner = 43.0f;
	m_fSpotOuter = 45.0f;
	m_fSpotFallOff = 1.0f;
	m_fRange = 5000.0f;

	m_fAttenuationConst  = 1.0f;
	m_fAttenuationLinear = 0.0f;
	m_fAttenuationQuad   = 0.0f;

	m_dwLightStartTime = 0;
	m_dwLightDuration  = 0;
	m_dwRatedUpdatedFrame = 0;

	m_vPosition = m_vDerivedPosition   = ioMath::VEC3_ZERO;
	m_vDirection = m_vDerivedDirection = ioMath::UNIT_Z;

	m_bLightAttributeChange = true;
	m_bLocalTransformDirty = false;
	m_bNoUpdatePosition = false;
}

ioLight::~ioLight()
{
	DESTROY_TEXTURE( m_pLightTexture );
	DESTROY_TEXTURE( m_pDecalTexture );
}

void ioLight::SetAttribute( DWORD dwMask )
{
	m_dwLightAttribute = dwMask;
}

void ioLight::AddAttribute( DWORD dwMask )
{
	m_dwLightAttribute |= dwMask;
}

void ioLight::RemoveAttribute( DWORD dwMask )
{
	m_dwLightAttribute &= ~dwMask;
}

bool ioLight::HasAttribute( DWORD dwMask ) const
{
	if( m_dwLightAttribute & dwMask )
		return true;

	return false;
}

void ioLight::SetLightApplyMask( DWORD dwMask )
{
	m_dwLightApplyMask = dwMask;
}

void ioLight::AddLightApplyMask( DWORD dwMask )
{
	m_dwLightApplyMask |= dwMask;
}

void ioLight::RemoveLightApplyMask( DWORD dwMask )
{
	m_dwLightApplyMask &= ~dwMask;
}

bool ioLight::HasLightApplyMask( DWORD dwMask ) const
{
	if( m_dwLightApplyMask & dwMask )
		return true;

	return false;
}

bool ioLight::IsValidLightByReceiveType( LightReceiveType eType )
{
	switch( eType )
	{
	case LRT_ALL_REAL_TIME:
		return HasAttribute( LAM_REAL_TIME );
	case LRT_JUST_PURE_REAL_TIME:
		if( HasAttribute( LAM_REAL_TIME ) && !HasAttribute( LAM_VERTEX_COLOR ) )
			return true;
		break;
	case LRT_ALL_VERTEX_COLOR:
		return HasAttribute( LAM_VERTEX_COLOR );
	}

	return false;
}

void ioLight::SetLightSystem( const ioLightSystem *pSys )
{
	m_LightType   = pSys->Light_Type;
	m_dwLightAttribute = pSys->dwLightAttribute;

	m_vPosition  = pSys->m_vPosition;
	m_vDirection = pSys->m_vDirection;
	m_vTargetPos = pSys->m_vTargetPos;

	m_Diffuse  = pSys->m_Diffuse;
	m_Specular = pSys->m_Specular;

	SetLightTexture( pSys->szLightTexture );
	SetDecalTexture( pSys->szDecalTexture );

	m_fSpotInner   = pSys->m_fSpotInner;
	m_fSpotOuter   = pSys->m_fSpotOuter;
	m_fSpotFallOff = pSys->m_fSpotFallOff;
	m_fRange = pSys->m_fRange;

	m_fAttenuationConst  = pSys->m_fAttenuationConst,
	m_fAttenuationLinear = pSys->m_fAttenuationLinear;
	m_fAttenuationQuad   = pSys->m_fAttenuationQuad;

	m_LightBrightFactor = pSys->m_BrightRate;
	m_bNoUpdatePosition = pSys->m_bNoUpdatePosition;

	m_bLightAttributeChange = true;
	m_bLocalTransformDirty = true;
}

void ioLight::SetD3DLight( const D3DLIGHT9 &kD3DLight )
{
	switch( kD3DLight.Type )
	{
	case D3DLIGHT_DIRECTIONAL:
		m_LightType = LT_DIRECTIONAL;
		break;
	case D3DLIGHT_POINT:
		m_LightType = LT_POINT;
		break;
	case D3DLIGHT_SPOT:
		m_LightType = LT_SPOT;
		break;
	}

	m_vPosition  = kD3DLight.Position;
	m_vDirection = kD3DLight.Direction;

	m_Diffuse  = kD3DLight.Diffuse;
	m_Specular = kD3DLight.Specular;

	m_fSpotInner = RADtoDEG( kD3DLight.Theta );
	m_fSpotOuter = RADtoDEG( kD3DLight.Phi );
	m_fSpotFallOff = kD3DLight.Falloff;
	m_fRange = kD3DLight.Range;

	m_fAttenuationConst  = kD3DLight.Attenuation0;
	m_fAttenuationLinear = kD3DLight.Attenuation1;
	m_fAttenuationQuad   = kD3DLight.Attenuation2;

	m_bLightAttributeChange = true;
	m_bLocalTransformDirty = true;
}

const D3DLIGHT9* ioLight::GetD3DLight() const
{
	UpdateD3DLight();

	return &m_D3DLight;
}

void ioLight::SetType( LightType eType )
{
	m_LightType = eType;
	m_bLightAttributeChange = true;
}

void ioLight::SetPosition( float fX, float fY, float fZ )
{
	SetPosition( D3DXVECTOR3( fX, fY, fZ ) );
}

void ioLight::SetPosition( const D3DXVECTOR3 &vPos )
{
	m_vPosition = vPos;

	m_bLightAttributeChange = true;
	m_bLocalTransformDirty = true;
}

void ioLight::SetDirection( float fX, float fY, float fZ )
{
	SetDirection( D3DXVECTOR3( fX, fY, fZ ) );
}

void ioLight::SetDirection( const D3DXVECTOR3 &vDirection )
{
	m_vDirection = vDirection;

	m_bLightAttributeChange = true;
	m_bLocalTransformDirty = true;
}

void ioLight::SetTargetPos( float fX, float fY, float fZ )
{
	m_vTargetPos.x = fX;
	m_vTargetPos.y = fY;
	m_vTargetPos.z = fZ;
}

void ioLight::SetTargetPos( const D3DXVECTOR3 &vTargetPos )
{
	m_vTargetPos = vTargetPos;
}

void ioLight::SetSpotLightRange( float fInnerAngle, float fOuterAngle, float fFallOff )
{
	m_fSpotInner = fInnerAngle;
	m_fSpotOuter = fOuterAngle;
	m_fSpotFallOff = fFallOff;

	m_bLightAttributeChange = true;
}

void ioLight::SetDiffuseColor( float fRed, float fGreen, float fBlue )
{
	m_Diffuse.r = fRed;
	m_Diffuse.g = fGreen;
	m_Diffuse.b = fBlue;
	m_Diffuse.a = 1.0f;
	m_bLightAttributeChange = true;
}

void ioLight::SetDiffuseColor( const D3DCOLORVALUE &rkDiffuse )
{
	m_Diffuse = rkDiffuse;
	m_bLightAttributeChange = true;
}

const D3DCOLORVALUE& ioLight::GetRatedDiffuseColor() const
{
	if( m_dwLightStartTime == 0 || m_dwLightDuration == 0 )
		return m_Diffuse;

	UpdateRatedLightColor();
	return m_RatedDiffuse;
}

void ioLight::SetSpecularColor( float fRed, float fGreen, float fBlue )
{
	m_Specular.r = fRed;
	m_Specular.g = fGreen;
	m_Specular.b = fBlue;
	m_Specular.a = 1.0f;
	m_bLightAttributeChange = true;
}

void ioLight::SetSpecularColor( const D3DCOLORVALUE &rkSpecular )
{
	m_Specular = rkSpecular;
	m_bLightAttributeChange = true;
}

const D3DCOLORVALUE& ioLight::GetRatedSpecularColor() const
{
	if( m_dwLightStartTime == 0 || m_dwLightDuration == 0 )
		return m_Specular;

	UpdateRatedLightColor();

	return m_RatedSpecular;
}

void ioLight::SetAttenuation( float fRange, float fConst, float fLinear, float fQuadratic )
{
	m_fRange = fRange;
	m_fAttenuationConst  = fConst;
	m_fAttenuationLinear = fLinear;
	m_fAttenuationQuad   = fQuadratic;
	m_bLightAttributeChange = true;
}

void ioLight::SetAttenuationRange( float fRange )
{
	m_fRange = fRange;
	m_bLightAttributeChange = true;
}

D3DXVECTOR4 ioLight::GetAttenuationFactor() const
{
	D3DXVECTOR4 vFactor;
	vFactor.x = m_fAttenuationConst;
	vFactor.y = m_fAttenuationLinear;
	vFactor.z = m_fAttenuationQuad;
	vFactor.w = m_fRange;

	return vFactor;
}

D3DXVECTOR3 ioLight::GetSpotFactor() const
{
	D3DXVECTOR3 vSpotFactor;
	vSpotFactor.x = cosf( DEGtoRAD( m_fSpotOuter * FLOAT05 ) );
	vSpotFactor.y = cosf( DEGtoRAD( m_fSpotInner * FLOAT05 ) ) - vSpotFactor.x;
	vSpotFactor.y = 1.0f / vSpotFactor.y;
	vSpotFactor.z = m_fSpotFallOff;

	return vSpotFactor;
}

void ioLight::UpdateTransform( bool bMustUpdate ) const
{
	if( !m_bNoUpdatePosition || bMustUpdate )
	{
		if( m_pParentNode )
		{
			if( !ioMath::IsEqual( m_pParentNode->GetDerivedOrientation(), m_qtLastParentOrientation ) ||
				!ioMath::IsEqual( m_pParentNode->GetDerivedPosition(), m_vLastParentPosition ) ||
				m_bLocalTransformDirty )
			{
				m_qtLastParentOrientation = m_pParentNode->GetDerivedOrientation();
				m_vLastParentPosition = m_pParentNode->GetDerivedPosition();

				m_vDerivedDirection = m_qtLastParentOrientation * m_vDirection;
				m_vDerivedPosition  = m_qtLastParentOrientation * m_vPosition + m_vLastParentPosition;
			}
		}
		else
		{
			m_vDerivedDirection = m_vDirection;
			m_vDerivedPosition  = m_vPosition;
		}
	}
	
	m_bLocalTransformDirty = false;
}

void ioLight::UpdateD3DLight() const
{
	if( !m_bLightAttributeChange )	return;

	switch( m_LightType )
	{
	case LT_DIRECTIONAL:
		m_D3DLight.Type = D3DLIGHT_DIRECTIONAL;
		break;
	case LT_POINT:
		m_D3DLight.Type = D3DLIGHT_POINT;
		break;
	case LT_SPOT:
		m_D3DLight.Type = D3DLIGHT_SPOT;
		break;
	}

	m_D3DLight.Diffuse   = m_Diffuse;
	m_D3DLight.Specular  = m_Specular;

	m_D3DLight.Ambient.r = 0.0f;
	m_D3DLight.Ambient.g = 0.0f;
	m_D3DLight.Ambient.b = 0.0f;
	m_D3DLight.Ambient.a = 1.0f;

	m_D3DLight.Position  = GetDerivedPosition();
	m_D3DLight.Direction = GetDerivedDirection();

	m_D3DLight.Range	 = m_fRange;
	m_D3DLight.Falloff	 = m_fSpotFallOff;

	m_D3DLight.Attenuation0 = m_fAttenuationConst;
	m_D3DLight.Attenuation1 = m_fAttenuationLinear;
	m_D3DLight.Attenuation2 = m_fAttenuationQuad;

	m_D3DLight.Theta = DEGtoRAD( m_fSpotInner );
	m_D3DLight.Phi	 = DEGtoRAD( m_fSpotOuter );

	m_bLightAttributeChange = false;
}

void ioLight::UpdateRatedLightColor() const
{
	if( m_dwRatedUpdatedFrame == g_FrameTimer.GetFrameCounter() )
		return;

	float fTimeRate = (float)( FRAMEGETTIME() - m_dwLightStartTime ) / (float)m_dwLightDuration;
	fTimeRate = max( 0.0f, min( fTimeRate, 1.0f ) );

	float fFactor = m_LightBrightFactor.GetCurFactor( fTimeRate );
	m_RatedDiffuse  = m_Diffuse * fFactor;
	m_RatedSpecular = m_Specular * fFactor;

	m_dwRatedUpdatedFrame = g_FrameTimer.GetFrameCounter();
}

void ioLight::SetLightTimeDuration( DWORD dwDuration )
{
	m_dwLightStartTime = FRAMEGETTIME();
	m_dwLightDuration  = dwDuration;
}

void ioLight::SetLightNoUpdatePosition( bool bNoUpdate )
{
	m_bNoUpdatePosition = bNoUpdate;
}

void ioLight::SetLightTexture( const ioHashString &rkName )
{
	DESTROY_TEXTURE( m_pLightTexture );

	if( !rkName.IsEmpty() )
	{
		m_pLightTexture = g_TextureMgr.CreateTexture( rkName, 0, 0, TQ_DEFAULT, true );
		m_pLightTexture->SetErasePolicy( EP_MANUAL );
	}
}

void ioLight::SetDecalTexture( const ioHashString &rkName )
{
	DESTROY_TEXTURE( m_pDecalTexture );

	if( !rkName.IsEmpty() )
	{
		m_pDecalTexture = g_TextureMgr.CreateTexture( rkName, 0, 0, TQ_DEFAULT, true );
		m_pDecalTexture->SetErasePolicy( EP_MANUAL );
	}
}

const char* ioLight::GetLightTextureName() const
{
	if( m_pLightTexture )
		return m_pLightTexture->GetName().c_str();

	return "";
}

const char* ioLight::GetDecalTextureName() const
{
	if( m_pDecalTexture )
		return m_pDecalTexture->GetName().c_str();

	return "";
}

void ioLight::NotifyAttached( ioNode *pParent )
{
	ioMovableObject::NotifyAttached( pParent );

	UpdateTransform( true );
}

void ioLight::NotifyCurrentCamera( ioCamera *pCamera )
{
}

void ioLight::UpdateRenderQueue( ioRenderQueue &queue )
{
}

const ioAxisAlignBox& ioLight::GetBoundBox() const
{
	static ioAxisAlignBox sBox;
	return sBox;
}

float ioLight::GetBoundRadius() const
{
	return 0.0f;
}

const D3DXVECTOR3& ioLight::GetDerivedPosition() const
{
	UpdateTransform();

	return m_vDerivedPosition;
}

const D3DXVECTOR3& ioLight::GetDerivedDirection() const
{
	UpdateTransform();

	return m_vDerivedDirection;
}

bool LightLess::operator()( const ioLight *pLhs, const ioLight *pRhs ) const
{
	return pLhs->m_fTempSquareDist < pRhs->m_fTempSquareDist;
}