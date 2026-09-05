

#ifndef _ioLight_h_
#define _ioLight_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioMovableObject.h"
#include "ioTimeRateFactor.h"

class ioTexture;
struct ioLightSystem;

class __EX ioLight : public ioMovableObject
{
public:
	enum LightAttributeMask
	{
		LAM_VERTEX_COLOR	 = 0x1,
		LAM_REAL_TIME		 = 0x2,
		LAM_EFFECT_LIGHT	 = 0x4,
	};

	float		m_fTempSquareDist;

protected:
	ioHashString	m_Name;
	LightType		m_LightType;
	DWORD			m_dwLightAttribute;
	DWORD			m_dwLightApplyMask;

	D3DXVECTOR3		m_vDirection;
	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vTargetPos;

	D3DCOLORVALUE	m_Diffuse;
	D3DCOLORVALUE	m_Specular;

	ioTexture	*m_pLightTexture;
	ioTexture	*m_pDecalTexture;

	float	m_fSpotInner;
	float	m_fSpotOuter;
	float	m_fSpotFallOff;
	float	m_fRange;
	float	m_fAttenuationConst;
	float	m_fAttenuationLinear;
	float	m_fAttenuationQuad;

	DWORD	m_dwLightStartTime;
	DWORD	m_dwLightDuration;
	ioTimeRateFactor m_LightBrightFactor;

	mutable D3DCOLORVALUE m_RatedDiffuse;
	mutable D3DCOLORVALUE m_RatedSpecular;
	mutable DWORD m_dwRatedUpdatedFrame;

	mutable D3DXVECTOR3 m_vDerivedDirection;
	mutable D3DXVECTOR3	m_vDerivedPosition;

	mutable D3DXQUATERNION m_qtLastParentOrientation;
	mutable D3DXVECTOR3	m_vLastParentPosition;

	mutable D3DLIGHT9 m_D3DLight;
	mutable bool m_bNoUpdatePosition;
	mutable bool m_bLightAttributeChange;
	mutable bool m_bLocalTransformDirty;

public:
	void SetAttribute( DWORD dwMask );
	void AddAttribute( DWORD dwMask );
	void RemoveAttribute( DWORD dwMask );
	bool HasAttribute( DWORD dwMask ) const;

	inline DWORD GetAttributeMask() const { return m_dwLightAttribute; }

public:
	void SetLightApplyMask( DWORD dwMask );
	void AddLightApplyMask( DWORD dwMask );
	void RemoveLightApplyMask( DWORD dwMask );
	bool HasLightApplyMask( DWORD dwMask ) const;

	inline DWORD GetLightApplyMask() const { return m_dwLightApplyMask; }

public:
	bool IsValidLightByReceiveType( LightReceiveType eType );

public:
	void SetType( LightType eType );
	inline LightType GetType() const { return m_LightType; }

	void SetDiffuseColor( float fRed, float fGreen, float fBlue );
	void SetDiffuseColor( const D3DCOLORVALUE &rkDiffuse );

	inline const D3DCOLORVALUE& GetDiffuseColor() const { return m_Diffuse; }
	const D3DCOLORVALUE& GetRatedDiffuseColor() const;

	void SetSpecularColor( float fRed, float fGreen, float fBlue );
	void SetSpecularColor( const D3DCOLORVALUE &rkSpecular );

	inline const D3DCOLORVALUE& GetSpecularColor() const { return m_Specular; }
	const D3DCOLORVALUE& GetRatedSpecularColor() const;

	void SetAttenuation( float fRange, float fConst, float fLinear, float fQuadratic );
	void SetAttenuationRange( float fRange );
	inline float GetAttenuationRange() const { return m_fRange; }
	inline float GetAttenuationConst() const { return m_fAttenuationConst; }
	inline float GetAttenuationLinear() const { return m_fAttenuationLinear; }
	inline float GetAttenuationQuadric() const { return m_fAttenuationQuad; }

	void SetPosition( float fX, float fY, float fZ );
	void SetPosition( const D3DXVECTOR3 &vPos );
	inline const D3DXVECTOR3& GetPosition() const { return m_vPosition; }

	void SetDirection( float fX, float fY, float fZ );
	void SetDirection( const D3DXVECTOR3 &vDirection );
	inline const D3DXVECTOR3& GetDirection() const { return m_vDirection; }

	void SetTargetPos( float fX, float fY, float fZ );
	void SetTargetPos( const D3DXVECTOR3 &vTargetPos );
	inline const D3DXVECTOR3& GetTargetPos() const { return m_vTargetPos; }

	void SetSpotLightRange( float fInnerAngle, float fOuterAngle, float fFallOff = 1.0f );
	inline float GetSpotLightInnerAngle() const { return m_fSpotInner; }
	inline float GetSpotLightOuterAngle() const { return m_fSpotOuter; }
	inline float GetSpotLightFallOff() const { return m_fSpotFallOff; }

	inline float GetSquareRange() const { return m_fRange * m_fRange; }

	D3DXVECTOR4 GetAttenuationFactor() const;
	D3DXVECTOR3 GetSpotFactor() const;

	void SetLightTexture( const ioHashString &rkName );
	void SetDecalTexture( const ioHashString &rkName );

	inline ioTexture* GetLightTexture() const { return m_pLightTexture; }
	inline ioTexture* GetDecalTexture() const { return m_pDecalTexture; }

	const char* GetLightTextureName() const;
	const char* GetDecalTextureName() const;

public:
	void SetD3DLight( const D3DLIGHT9 &kD3DLight );
	const D3DLIGHT9* GetD3DLight() const;

	void SetLightSystem( const ioLightSystem *pSys );
	void SetLightTimeDuration( DWORD dwDuration );
	void SetLightNoUpdatePosition( bool bNoUpdate );

public:
	virtual void NotifyAttached( ioNode *pParent );
	virtual void NotifyCurrentCamera( ioCamera *pCamera );
	virtual void UpdateRenderQueue( ioRenderQueue &queue );

	virtual const ioAxisAlignBox& GetBoundBox() const;
	virtual float GetBoundRadius() const;

	virtual const D3DXVECTOR3& GetDerivedPosition() const;
	virtual const D3DXVECTOR3& GetDerivedDirection() const;

protected:
	void UpdateTransform( bool bMustUpdate = false ) const;
	void UpdateD3DLight() const;
	void UpdateRatedLightColor() const;

public:
	inline const ioHashString& GetName() const { return m_Name; }

public:
	ioLight();
	ioLight( const ioHashString &kName );
	virtual ~ioLight();
};

struct LightLess
{
	__EX bool operator()( const ioLight *pLhs, const ioLight *pRhs ) const;
};

#endif
