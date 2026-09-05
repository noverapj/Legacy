

#ifndef _ioMaterial_h_
#define _ioMaterial_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioVectorIterator.h"

class ioTechnique;

class __EX ioMaterial
{
protected:
	ioHashString m_Name;

	D3DMATERIAL9 m_mtl;
	float m_fSpecularLv;
	ErasePolicy m_TexturePolicy;

	typedef std::vector< ioTechnique* > TechniqueList;
	TechniqueList m_TechniqueList;
	TechniqueList m_SupportedTechniqueList;
	bool m_bNeedCompile;
	bool m_bLoaded;

	bool m_bLocalGlow;

public:
	inline const ioHashString& GetName() const { return m_Name; }

public:
	void Load( ErasePolicy eTextureErasePolicy );
	inline bool IsLoaded() const { return m_bLoaded; }
	inline ErasePolicy GetTextureErasePolicy() const { return m_TexturePolicy; }

	bool IsBestTechLoadDone();

public:
	ioTechnique* CreateTechnique();
	inline ioTechnique* GetTechnique( int iIdx ) { return m_TechniqueList[iIdx]; }
	inline int GetNumTechniques() const { return m_TechniqueList.size(); }

	void RemoveTechnique( int iIdx );
	void RemoveAllTechniques();

	typedef ioVectorIterator< TechniqueList > TechniqueIterator;
	TechniqueIterator GetTechniqueIterator();
	TechniqueIterator GetSupportedTechniqueIterator();

	ioTechnique* GetBestTechnique();
	ioTechnique* GetSupportedTechnique( int iIdx );
	int GetNumSupportedTechniques();

protected:
	void NeedUpdateAmbientEmissiveCache();

public:
	void Compile();
	void NotifyNeedRecompile();

	void ConvertShaderStaticToVertexColor();
	void ConvertShaderAnimateToStatic();
	void ConvertShaderStaticToAnimate();

public:
	void SetAmbient( const D3DCOLORVALUE &rkColor );
	void SetAmbient( float fRed, float fGreen, float fBlue, float fAlpha = 1.0f );

	void SetDiffuse( const D3DCOLORVALUE &rkColor );
	void SetDiffuse( float fRed, float fGreen, float fBlue, float fAlpha = 1.0f );

	void SetEmissive( const D3DCOLORVALUE &rkColor );
	void SetEmissive( float fRed, float fGreen, float fBlue, float fAlpha = 1.0f );

	void SetSpecular( const D3DCOLORVALUE &rkColor );
	void SetSpecular( float fRed, float fGreen, float fBlue, float fAlpha = 1.0f );
	void SetSpecularPower( float fPower );
	void SetSpecularLv( float fSpecularLv );

	void SetLocalGlow( bool b);

	inline const D3DCOLORVALUE& GetAmbient() const { return m_mtl.Ambient; }
	inline const D3DCOLORVALUE& GetDiffuse() const { return m_mtl.Diffuse; }
	inline const D3DCOLORVALUE& GetEmissive() const { return m_mtl.Emissive; }
	inline const D3DCOLORVALUE& GetSpecular() const { return m_mtl.Specular; }
	inline float GetSpecularPower() const { return m_mtl.Power; }
	inline float GetSpecularLv() const { return m_fSpecularLv; }
	inline D3DXVECTOR4 GetSpecularParam() const { return D3DXVECTOR4( m_mtl.Power, m_fSpecularLv, 0.0f, 0.0f ); }
	inline bool GetLocalGlow(){ return m_bLocalGlow;}

public:
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

	bool IsTransparent() const;

public:
	ioMaterial& operator=( const ioMaterial &rhs );
	bool operator==( const ioMaterial &rhs ) const;
	bool operator!=( const ioMaterial &rhs ) const;

public:
	ioMaterial( const ioHashString &name );
	virtual ~ioMaterial();
};

#endif
