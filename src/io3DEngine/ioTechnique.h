

#ifndef _ioTechnique_h_
#define _ioTechnique_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioVectorIterator.h"
#include "ioShaderDefine.h"

class ioMaterial;
class ioPass;

class __EX ioTechnique
{
private:
	typedef std::vector< ioPass* > PassList;
	PassList m_PassList;

	ioMaterial *m_pParent;
	bool m_bSupported;
	bool m_bLoadDone;
	bool m_bCustomTex;

public:
	void Load();
	bool IsLoaded() const;	// 이전에 로딩함수가 호출된적이 있는가..
	bool IsLoadDone();		// 스레드로딩이 모두 완료되었는가.

	ErasePolicy GetTextureErasePolicy() const;

public:
	inline bool IsCustomTex(){ return m_bCustomTex; }
	inline void SetCustomTex( bool bCustomTex ){ m_bCustomTex = bCustomTex; }

public:
	void ConvertShaderStaticToVertexColor();
	void ConvertShaderAnimateToStatic();
	void ConvertShaderStaticToAnimate();

public:
	ioPass* CreatePass();
	ioPass* CreatePassInFront();
	ioPass* GetPass( int iIndex );
	
	ioPass* GetPassByShaderGrp( ShaderGroupType eType );
	int GetPassCntByShaderGrp( ShaderGroupType eType ) const;

	int RemovePass( int iIndex );
	void RemoveAllPass();
	void ReArrangePassIndex();

	inline int GetNumPasses() const { return m_PassList.size(); }

	typedef ioVectorIterator< PassList > PassIterator;
	const PassIterator GetPassIterator();

public:
	inline ioMaterial* GetParent() const { return m_pParent; }
	inline bool IsSupported() const { return m_bSupported; }

	ioTechnique& operator=( const ioTechnique &rhs );
	bool operator==( const ioTechnique &rhs ) const;
	bool operator!=( const ioTechnique &rhs ) const;

public:
	const D3DCOLORVALUE& GetAmbient() const;
	const D3DCOLORVALUE& GetDiffuse() const;
	const D3DCOLORVALUE& GetEmissive() const;
	const D3DCOLORVALUE& GetSpecular() const;
	float GetSpecularPower() const;
	float GetSpecularLv() const;
	D3DXVECTOR4 GetSpecularParam() const;

	void NeedUpdateAmbientEmissiveCache();

public:	// Set All Passes
	void SetDepthCheckEnable( bool bEnable );
	void SetDepthWriteEnable( bool bEnable );
	void SetDepthFunction( D3DCMPFUNC eFunction );

	void SetColorWriteEnable( bool bEnable );

	void SetCullingMode( CullingMode eMode );
	void SetLightingEnable( bool bEnable );
	void SetShadeType( ShadeType eType );
	void SetFillMode( FillMode eMode );

	void SetDepthBias( float fDepthBias, float fBiasSlope );

	void SetTextureFiltering( TextureFilterOption filter );
	void SetTextureMipMapBias( float fBias );
	void SetTextureAnisotropy( int iMaxAniso );

	void SetSceneBlending( SceneBlendMode eMode );
	void SetSceneBlending( D3DBLENDOP eOp, D3DBLEND eSource, D3DBLEND eDest );
	void SetShaderGroup( const ioHashString &szName );
	void StopAllTextureEffect( bool bStop );

	bool IsDepthWriteEnabled() const;
    bool IsDepthCheckEnabled() const;

	bool HasShaderGroupPass( ShaderGroupType eType );

	bool IsTransparent() const;

public:
	ioTechnique( ioMaterial *pParent );
	ioTechnique( ioMaterial *pParent, const ioTechnique &rhs );
	~ioTechnique();
};

#endif
