

#ifndef _ioPass_h_
#define _ioPass_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioVectorIterator.h"
#include "ioShaderParameter.h"
#include "ioShaderDefine.h"

class ioShader;
class ioShaderGroup;
class ioTechnique;
class ioTextureUnitState;
class ioAutoShaderParamSource;

class __EX ioPass
{
public:
	enum PassProperty
	{
		PP_SHADOW_CAST_PASS = 0x1,
		PP_SHADOW_RECV_PASS = 0x2,
		PP_LIGHT_ITERATE_PASS = 0x4,
	};

	enum PassLightType
	{
		PLT_DIRECTIONAL = 0x1,
		PLT_POINT = 0x2,
		PLT_SPOT = 0x4,
	};

private:
	ioTechnique *m_pParent;
	ioShaderGroup *m_pShaderGrp;

	ioShaderCustomParameters m_CustomParameters;
	mutable D3DCOLORVALUE m_CachedAmbientEmissiveColor;

	int		m_iPassIndex;
	DWORD	m_dwPassHash;
	DWORD	m_dwPassProperty;
	DWORD	m_dwPassLightType;

	D3DBLENDOP	m_BlendOp;
	D3DBLEND	m_SourceBlend;
	D3DBLEND	m_DestBlend;

	DWORD		m_dwTextureFactor;
	bool		m_bUseTextureFactor;

	D3DCMPFUNC	m_DepthFunc;
	float		m_fDepthBias;
	float		m_fBiasSlope;

	D3DCMPFUNC	m_AlphaTestFunc;
	int			m_iAlphaTestRef;

	CullingMode m_CullMode;
	ShadeType	m_ShadeType;
	FillMode	m_FillMode;

	ioHashString m_RenderTarget;
	int m_iRTWidth;
	int m_iRTHeight;

	typedef std::vector< ioTextureUnitState* > TextureUnitStateList;
	TextureUnitStateList m_TextureUnitStateList;

	float		m_fAmbientXformRate;
	float		m_fDiffuseXformRate;
	float		m_fSpecularXformRate;
	float		m_fEmissiveXformRate;
	bool		m_bColorXformed;

	bool		m_bDepthCheck;
	bool		m_bDepthWrite;
	bool		m_bLightingEnable;
	bool		m_bColorWrite;

	bool		m_bUseQuadBuffer;
	bool		m_bDisableShader;
	bool		m_bLoadDone;
	bool		m_bEnablePass;

	mutable bool m_bNeedUpdatedAmbientEmissiveCache;

public:
	void Load();
	bool IsLoaded() const;	// 이전에 로딩함수가 호출된적이 있는가..
	bool IsLoadDone();		// 스레드로딩이 모두 완료되었는가.

	ErasePolicy GetTextureErasePolicy() const;

public:
	void SetShaderGroup( const ioHashString &name );
	const ioHashString* GetShaderGroupName() const;
	ShaderGroupType GetShaderGroupType() const;

public:
	ioShader* BindActiveShader( const ioLight *pLight, bool bGenShadow, bool bRecvShadow );
	ioShader* BindActiveShader( const ioLightList &rkLightList, bool bGenShadow, bool bRecvShadow );

public:
	inline void AddCustomParameter( const ioShaderCustomArg &rkArg )
	{
		m_CustomParameters.SetCustomArg( rkArg );
	}

	inline ioShaderCustomArg* GetCustomArg( const ioHashString &szName ) { return m_CustomParameters.GetArg( szName ); }
	inline ioShaderCustomArg* GetCustomArg( int iIndex ) { return m_CustomParameters.GetArg( iIndex ); }
	inline const ioShaderCustomParameters& GetCustomParameters() const { return m_CustomParameters; }
	inline int GetCustomParameterCnt() const { return m_CustomParameters.GetCustomArgCount(); }

public:
	void ConvertShaderStaticToVertexColor();
	void ConvertShaderAnimateToStatic();
	void ConvertShaderStaticToAnimate();

public:
	void SetRenderTarget( const ioHashString &szName, int iWidth, int iHeight );
	void ClearRenderTarget();

	inline bool HasRenderTarget() const { return !m_RenderTarget.IsEmpty(); }
	inline const ioHashString& GetRenderTarget() const { return m_RenderTarget; }
	inline int GetRenderTargetWidth() const { return m_iRTWidth; }
	inline int GetRenderTargetHeight() const { return m_iRTHeight; }

public:
	inline void SetUseQuadBuffer( bool bUse ) { m_bUseQuadBuffer = bUse; }
	inline bool UseQuadBuffer() const { return m_bUseQuadBuffer; }

public:
	ioPass& operator=( const ioPass &rhs );

	bool operator==( const ioPass &rhs ) const;
	bool operator!=( const ioPass &rhs ) const;

public:
	void CopyExceptTextureUnitStates( const ioPass &rhs );

public:
	inline ioTechnique* GetParent() const { return m_pParent; }

	void SetPassIndex( int iIndex );
	inline int GetPassIndex() const { return m_iPassIndex; }

	bool HasShader() const;
	bool IsTransparent() const;
	bool IsCanCastShadow() const;
	bool HasRenderShadowShader() const;

public:
	inline void SetPassProperty( DWORD dwProperty ) { m_dwPassProperty = dwProperty; }
	inline void AddPassProperty( PassProperty eProperty ) { m_dwPassProperty |= eProperty; }
	inline void RemovePassProperty( PassProperty eProperty ) { m_dwPassProperty &= ~eProperty; }
	inline bool HasProperty( PassProperty eProperty ) const
	{
		if( m_dwPassProperty & eProperty )
			return true;

		return false;
	}

public:
	inline void SetPassEnable( bool bEnable ) { m_bEnablePass = bEnable; }
	inline bool IsPassEnable() const { return m_bEnablePass; }

	inline void SetDisableShader( bool bDisable ) { m_bDisableShader = bDisable; }
	inline bool IsDisableShader() const { return m_bDisableShader; }

	inline void SetPassLightType( DWORD dwLightType ) { m_dwPassLightType = dwLightType; }
	inline void AddPassLightType( PassLightType eType ) { m_dwPassLightType |= eType; }
	inline void RemovePassLightType( PassLightType eType ) { m_dwPassLightType &= ~eType; }

	bool IsLightAvailable( ioLight *pLight ) const;

	inline bool IsLightAvailableType( PassLightType eType ) const
	{
		if( m_dwPassLightType & eType )
			return true;

		return false;
	}

public:
	D3DMATERIAL9  GetDXMaterial() const;
	D3DCOLORVALUE GetAmbient() const;
	D3DCOLORVALUE GetDiffuse() const;
	D3DCOLORVALUE GetSpecular() const;
	D3DCOLORVALUE GetEmissive() const;
	D3DCOLORVALUE GetAmbientEmissiveColor( const D3DCOLORVALUE &rkAmbientLight, bool bNeedUpdate ) const;
	D3DXVECTOR4 GetSpecularParam() const;

	inline void NeedUpdateAmbientEmissiveCache() { m_bNeedUpdatedAmbientEmissiveCache = true; }

public:
	ioTextureUnitState* CreateTextureUnitState();
	ioTextureUnitState* CreateTextureUnitState( const ioHashString &szTexName );

	void AddTextureUnitState( ioTextureUnitState *pState );
	void SwapTextureUnitStateOrder( int iSrc, int iDest );
	ioTextureUnitState* GetTextureUnitState( int iIndex ) const;

	typedef ioVectorIterator< TextureUnitStateList > TextureUnitStateIterator;
	TextureUnitStateIterator GetTextureUnitStateIterator();

	void RemoveTextureUnitState( int iIndex );
	void RemoveAllTextureUnitStates();

	inline int GetNumTextureUnitStates() const { return m_TextureUnitStateList.size(); }

public:
	void SetColorTransformRate( ColorTransformType ctType, float fRate );
	void ClearColorTransformRate();

public:
	void SetSceneBlending( SceneBlendMode eMode );
	void SetSceneBlending( D3DBLENDOP eOp, D3DBLEND eSource, D3DBLEND eDest );
	void SetSceneBlendOp( D3DBLENDOP eOp );
	void SetSceneBlendSource( D3DBLEND eSource );
	void SetSceneBlendDest( D3DBLEND eDest );

	inline D3DBLENDOP GetBlendOp() const { return m_BlendOp; }
	inline D3DBLEND GetSourceBlendFactor() const { return m_SourceBlend; }
	inline D3DBLEND GetDestBlendFactor() const { return m_DestBlend; }

	D3DBLENDOP GetAlphaBlendOp() const;
	D3DBLEND GetAlphaSourceBlendFactor() const;
	D3DBLEND GetAlphaDestBlendFactor() const;

public:
	void SetTextureFactor( DWORD dwFactor );
	inline DWORD GetTextureFactor() const { return m_dwTextureFactor; }
	inline bool IsUseTextureFactor() const { return m_bUseTextureFactor; }

public:
	inline void SetDepthCheckEnable( bool bEnable ) { m_bDepthCheck = bEnable; }
	inline void SetDepthWriteEnable( bool bEnable ) { m_bDepthWrite = bEnable; }
	inline void SetDepthFunction( D3DCMPFUNC eFunction ) { m_DepthFunc = eFunction; }

	inline bool GetDepthCheckEnable() const { return m_bDepthCheck; }
	inline bool GetDepthWriteEnable() const { return m_bDepthWrite; }
	inline D3DCMPFUNC GetDepthFunction() const { return m_DepthFunc; }

	inline void SetColorWriteEnable( bool bEnable ) { m_bColorWrite = bEnable; }
	inline bool GetColorWriteEnable() const { return m_bColorWrite; }

	inline void SetCullingMode( CullingMode eMode ) { m_CullMode = eMode; }
	inline CullingMode GetCullingMode() const { return m_CullMode; }

	inline void SetFillMode( FillMode eMode ) { m_FillMode = eMode; }
	inline FillMode GetFillMode() const { return m_FillMode; }

	inline void SetLightingEnable( bool bEnable ) { m_bLightingEnable = bEnable; }
	inline bool IsLightingEnable() const { return m_bLightingEnable; }

	inline void SetShadeType( ShadeType eType ) { m_ShadeType = eType; }
	inline ShadeType GetShadeType() const { return m_ShadeType; }

	void SetDepthBias( float fDepthBias, float fBiasSlope );
	inline float GetDepthBias() const { return m_fDepthBias; }
	inline float GetBiasSlope() const { return m_fBiasSlope; }

	void SetAlphaTestSetting( D3DCMPFUNC eFunc, int iRef );
	inline void SetAlphaTestFunction( D3DCMPFUNC eFunc ) { m_AlphaTestFunc = eFunc; }
	inline void SetAlphaTestRef( int iRef ) { m_iAlphaTestRef = iRef; }

	inline D3DCMPFUNC GetAlphaTestFunction() const { return m_AlphaTestFunc; }
	inline int GetAlphaTestRef() const { return m_iAlphaTestRef; }

	void SetTextureFiltering( TextureFilterOption filter );
	void SetTextureMipMapBias( float fBias );
	void SetTextureAnisotropy( int iMaxAniso );

	void SetDefaultSetting();
	void StopAllTextureEffect( bool bStop );

public:
	inline DWORD GetHash() const { return m_dwPassHash; }
	void ReCalculateHash();

public:
	ioPass( ioTechnique *pParent, int iIndex );
	ioPass( ioTechnique *pParent, int iIndex, const ioPass &rhs );
	~ioPass();
};

#endif
