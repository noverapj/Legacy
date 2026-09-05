

#ifndef _ioLightSystem_h_
#define _ioLightSystem_h_

#include "io3DCommon.h"
#include "ioTimeRateFactor.h"

struct __EX ioLightSystem
{
	ioHashString	szName;
	ioHashString	szLightTexture;
	ioHashString	szDecalTexture;

	LightType		Light_Type;

	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vDirection;
	D3DXVECTOR3		m_vTargetPos;

	D3DCOLORVALUE	m_Diffuse;
	D3DCOLORVALUE	m_Specular;

	float	m_fSpotInner;
	float	m_fSpotOuter;
	float	m_fSpotFallOff;

	float	m_fRange;
	float	m_fAttenuationConst;
	float	m_fAttenuationLinear;
	float	m_fAttenuationQuad;

	DWORD	dwLightAttribute;
	DWORD	dwLight_s;
	DWORD	dwLight_e;

	ioTimeRateFactor m_BrightRate;
	bool	m_bNoUpdatePosition;

public:
	bool operator==( const ioLightSystem &rhs ) const;
	bool operator!=( const ioLightSystem &rhs ) const;

public:
	ioLightSystem();
	~ioLightSystem();
};

#endif