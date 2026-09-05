

#include "stdafx.h"

#include "ioMath.h"
#include "ioLightSystem.h"

ioLightSystem::ioLightSystem()
{
	Light_Type = LT_POINT;

	m_vPosition  = ioMath::VEC3_ZERO;
	m_vDirection = ioMath::UNIT_Y;
	m_vTargetPos = ioMath::UNIT_Y;

	m_Diffuse.r = m_Diffuse.g = m_Diffuse.b = m_Diffuse.a = 1.0f;
	m_Specular.r = m_Specular.g = m_Specular.b = m_Specular.a = 1.0f;

	m_fSpotInner = m_fSpotOuter = m_fSpotFallOff = 0.0f;

	m_fRange = 100.0f;
	m_fAttenuationConst  = 0.0f;
	m_fAttenuationLinear = 1.0f;
	m_fAttenuationQuad   = 0.0f;

	dwLightAttribute = 0;
	dwLight_s = 0;
	dwLight_e = 0;

	m_bNoUpdatePosition = false;
}

ioLightSystem::~ioLightSystem()
{

}

bool ioLightSystem::operator==( const ioLightSystem &rhs ) const
{
	if( szLightTexture != rhs.szLightTexture )
		return false;

	if( szDecalTexture != rhs.szDecalTexture )
		return false;

	if( Light_Type != rhs.Light_Type )
		return false;

	if( !ioMath::IsEqual( m_vPosition, rhs.m_vPosition ) )
		return false;

	if( !ioMath::IsEqual( m_vDirection, rhs.m_vDirection ) )
		return false;

	if( !ioMath::IsEqual( m_vTargetPos, rhs.m_vTargetPos ) )
		return false;

	if( memcmp( &m_Diffuse, &rhs.m_Diffuse, sizeof(D3DCOLORVALUE) ) )
		return false;

	if( memcmp( &m_Specular, &rhs.m_Specular, sizeof(D3DCOLORVALUE) ) )
		return false;

	if( m_fSpotInner != rhs.m_fSpotInner )
		return false;

	if( m_fSpotOuter != rhs.m_fSpotOuter )
		return false;

	if( m_fSpotFallOff != rhs.m_fSpotFallOff )
		return false;

	if( m_fRange != rhs.m_fRange )
		return false;

	if( m_fAttenuationConst != rhs.m_fAttenuationConst )
		return false;

	if( m_fAttenuationLinear != rhs.m_fAttenuationLinear )
		return false;

	if( m_fAttenuationQuad != rhs.m_fAttenuationQuad )
		return false;

	if( dwLightAttribute != rhs.dwLightAttribute )
		return false;

	if( dwLight_s != rhs.dwLight_s )
		return false;

	if( dwLight_e != rhs.dwLight_e )
		return false;

	if( m_BrightRate != rhs.m_BrightRate )
		return false;

	if( m_bNoUpdatePosition != rhs.m_bNoUpdatePosition )
		return false;

	return true;
}

bool ioLightSystem::operator!=( const ioLightSystem &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}