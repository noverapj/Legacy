

#include "stdafx.h"

#include "ioMaterial.h"
#include "ioTechnique.h"
#include "ioPass.h"
#include "ioTextureUnitState.h"
#include "ioMaterialManager.h"

using namespace std;

ioMaterial::ioMaterial( const ioHashString &name )
: m_Name( name )
{
	m_mtl.Diffuse.r = m_mtl.Diffuse.g = m_mtl.Diffuse.b = m_mtl.Diffuse.a = 1.0f;
	m_mtl.Ambient.r = m_mtl.Ambient.g = m_mtl.Ambient.b = m_mtl.Ambient.a = 1.0f;
	m_mtl.Specular.r = m_mtl.Specular.g = m_mtl.Specular.b = m_mtl.Specular.a = 1.0f;
	m_mtl.Emissive.r = m_mtl.Emissive.g = m_mtl.Emissive.b = m_mtl.Emissive.a = 0.0f;
	m_mtl.Power = 0.0f;
	m_fSpecularLv = 0.0f;

	m_TexturePolicy = EP_AUTOMATIC;

	m_bNeedCompile = true;
	m_bLoaded = false;

	m_bLocalGlow = false;
}

ioMaterial::~ioMaterial()
{
	RemoveAllTechniques();
}

ioMaterial& ioMaterial::operator = ( const ioMaterial &rhs )
{
	m_mtl = rhs.m_mtl;
	m_fSpecularLv = rhs.m_fSpecularLv;
	m_bLocalGlow = rhs.m_bLocalGlow;
	m_Name = rhs.m_Name;

	RemoveAllTechniques();

	TechniqueList::const_iterator iter;
	for( iter=rhs.m_TechniqueList.begin() ; iter!=rhs.m_TechniqueList.end() ; ++iter )
	{
		ioTechnique *pTech = CreateTechnique();
		*pTech = *(*iter);

		if( (*iter)->IsSupported() )
		{
			m_SupportedTechniqueList.push_back( pTech );
		}
	}

	m_bNeedCompile = rhs.m_bNeedCompile;

	if( rhs.m_bLoaded )
	{
		Load( rhs.m_TexturePolicy );
	}

	return *this;
}

bool ioMaterial::operator==( const ioMaterial &rhs ) const
{
	if( 0 != memcmp( &m_mtl, &rhs.m_mtl, sizeof(m_mtl) ) )
		return false;

	if( m_fSpecularLv != rhs.m_fSpecularLv )
		return false;

	if( m_bLocalGlow != rhs.m_bLocalGlow )
		return false;

	if( m_TechniqueList.size() != rhs.m_TechniqueList.size() )
		return false;

	int iTechCnt = m_TechniqueList.size();
	for( int i=0 ; i<iTechCnt ; i++ )
	{
		if( *m_TechniqueList[i] != *rhs.m_TechniqueList[i] )
			return false;
	}

	return true;
}

bool ioMaterial::operator!=( const ioMaterial &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

void ioMaterial::Load( ErasePolicy eTextureErasePolicy )
{
	Compile();

	if( !m_bLoaded )
	{
		m_TexturePolicy = eTextureErasePolicy;

		for_each( m_SupportedTechniqueList.begin(),
				  m_SupportedTechniqueList.end(),
				  mem_fun( &ioTechnique::Load ) );

		m_bLoaded = true;
	}
}

bool ioMaterial::IsBestTechLoadDone()
{
	if( !m_bLoaded )
		return false;

	ioTechnique *pTech = GetBestTechnique();
	if( pTech )
		return pTech->IsLoadDone();

	return false;
}

void ioMaterial::Compile()
{
	if( !m_bNeedCompile )	return;

	m_SupportedTechniqueList.clear();

	TechniqueList::iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		ioTechnique *pTech = *iter;
		if( pTech->IsSupported() )
		{
			m_SupportedTechniqueList.push_back( pTech );
		}
	}

	if( m_SupportedTechniqueList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioMaterial::Compile - %s SupportedTechnique Not Exist", m_Name.c_str() );
	}

	m_bNeedCompile = false;
}

void ioMaterial::NotifyNeedRecompile()
{
	m_bNeedCompile = true;
}

ioTechnique* ioMaterial::CreateTechnique()
{
	ioTechnique *t = new ioTechnique( this );
	m_TechniqueList.push_back( t );

	m_bNeedCompile = true;

	return t;
}

void ioMaterial::RemoveTechnique( int iIdx )
{
	if( COMPARE( iIdx, 0, GetNumTechniques() ) )
	{
		TechniqueList::iterator iter = m_TechniqueList.begin() + iIdx;
		delete *iter;
		m_TechniqueList.erase( iter );

		m_SupportedTechniqueList.erase( m_SupportedTechniqueList.begin(), m_SupportedTechniqueList.end() );
		m_bNeedCompile = true;
	}
}

void ioMaterial::RemoveAllTechniques()
{
	TechniqueList::iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		delete *iter;
	}
	
	m_TechniqueList.erase( m_TechniqueList.begin(), m_TechniqueList.end() );
	m_SupportedTechniqueList.erase( m_SupportedTechniqueList.begin(), m_SupportedTechniqueList.end() );
	m_bNeedCompile = true;
}

ioMaterial::TechniqueIterator ioMaterial::GetTechniqueIterator()
{
	return TechniqueIterator( m_TechniqueList.begin(), m_TechniqueList.end() );
}

ioMaterial::TechniqueIterator ioMaterial::GetSupportedTechniqueIterator()
{
	Compile();

	return TechniqueIterator( m_SupportedTechniqueList.begin(), m_SupportedTechniqueList.end() );
}

ioTechnique* ioMaterial::GetSupportedTechnique( int iIdx )
{
	Compile();

	assert( COMPARE( iIdx, 0, GetNumSupportedTechniques() ) );

	return m_SupportedTechniqueList[iIdx];
}

int ioMaterial::GetNumSupportedTechniques()
{
	Compile();

	return static_cast<int>( m_SupportedTechniqueList.size() );
}

void ioMaterial::NeedUpdateAmbientEmissiveCache()
{
	for_each( m_SupportedTechniqueList.begin(),
			  m_SupportedTechniqueList.end(),
			  mem_fun( &ioTechnique::NeedUpdateAmbientEmissiveCache ) );
}

ioTechnique* ioMaterial::GetBestTechnique()
{
	Compile();

	if( !m_SupportedTechniqueList.empty() )
		return m_SupportedTechniqueList.front();

	return NULL;
}

bool ioMaterial::IsTransparent() const
{
	TechniqueList::const_iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		if( (*iter)->IsTransparent() )
			return true;
	}

	return false;
}

void ioMaterial::SetAmbient( const D3DCOLORVALUE &rkColor )
{
	SetAmbient( rkColor.r, rkColor.g, rkColor.b, rkColor.a );
}

void ioMaterial::SetAmbient( float fRed, float fGreen, float fBlue, float fAlpha )
{
	m_mtl.Ambient.r = fRed;
	m_mtl.Ambient.g = fGreen;
	m_mtl.Ambient.b = fBlue;
	m_mtl.Ambient.a = fAlpha;

	NeedUpdateAmbientEmissiveCache();
}

void ioMaterial::SetDiffuse( const D3DCOLORVALUE &rkColor )
{
	SetDiffuse( rkColor.r, rkColor.g, rkColor.b, rkColor.a );
}

void ioMaterial::SetDiffuse( float fRed, float fGreen, float fBlue, float fAlpha )
{
	m_mtl.Diffuse.r = fRed;
	m_mtl.Diffuse.g = fGreen;
	m_mtl.Diffuse.b = fBlue;
	m_mtl.Diffuse.a = fAlpha;
}

void ioMaterial::SetEmissive( const D3DCOLORVALUE &rkColor )
{
	SetEmissive( rkColor.r, rkColor.g, rkColor.b, rkColor.a );
}

void ioMaterial::SetEmissive( float fRed, float fGreen, float fBlue, float fAlpha )
{
	m_mtl.Emissive.r = fRed;
	m_mtl.Emissive.g = fGreen;
	m_mtl.Emissive.b = fBlue;
	m_mtl.Emissive.a = fAlpha;

	NeedUpdateAmbientEmissiveCache();
}

void ioMaterial::SetSpecular( const D3DCOLORVALUE &rkColor )
{
	SetSpecular( rkColor.r, rkColor.g, rkColor.b, rkColor.a );
}

void ioMaterial::SetSpecular( float fRed, float fGreen, float fBlue, float fAlpha )
{
	m_mtl.Specular.r = fRed;
	m_mtl.Specular.g = fGreen;
	m_mtl.Specular.b = fBlue;
	m_mtl.Specular.a = fAlpha;
}

void ioMaterial::SetSpecularPower( float fPower )
{
	m_mtl.Power = fPower;
}

void ioMaterial::SetSpecularLv( float fSpecularLv )
{
	m_fSpecularLv = fSpecularLv;
}

void ioMaterial::SetDepthCheckEnable( bool bEnable )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetDepthCheckEnable ), bEnable ) );
}

void ioMaterial::SetDepthWriteEnable( bool bEnable )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetDepthWriteEnable ), bEnable ) );
}

void ioMaterial::SetDepthFunction( D3DCMPFUNC eFunction )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetDepthFunction ), eFunction ) );
}

void ioMaterial::SetColorWriteEnable( bool bEnable )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetColorWriteEnable ), bEnable ) );
}

void ioMaterial::SetCullingMode( CullingMode eMode )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetCullingMode ), eMode ) );
}

void ioMaterial::SetLightingEnable( bool bEnable )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetLightingEnable ), bEnable ) );
}

void ioMaterial::SetShadeType( ShadeType eType )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetShadeType ), eType ) );
}

void ioMaterial::SetFillMode( FillMode eMode )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetFillMode ), eMode ) );
}

void ioMaterial::SetDepthBias( float fDepthBias, float fBiasSlope )
{
	TechniqueList::iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		(*iter)->SetDepthBias( fDepthBias, fBiasSlope );
	}
}

void ioMaterial::SetTextureFiltering( TextureFilterOption filter )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetTextureFiltering ), filter ) );
}

void ioMaterial::SetTextureMipMapBias( float fBias )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetTextureMipMapBias ), fBias ) );
}

void ioMaterial::SetTextureAnisotropy( int iMaxAniso )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetTextureAnisotropy ), iMaxAniso ) );
}

void ioMaterial::SetSceneBlending( SceneBlendMode eMode )
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  bind2nd( mem_fun( &ioTechnique::SetSceneBlending ), eMode ) );
}

void ioMaterial::SetSceneBlending( D3DBLENDOP eOp, D3DBLEND eSource, D3DBLEND eDest )
{
	TechniqueList::iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		(*iter)->SetSceneBlending( eOp, eSource, eDest );
	}
}

void ioMaterial::SetShaderGroup( const ioHashString &szName )
{
	TechniqueList::iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		(*iter)->SetShaderGroup( szName );
	}
}

void ioMaterial::StopAllTextureEffect( bool bStop )
{
	TechniqueList::iterator iter;
	for( iter=m_TechniqueList.begin() ; iter!=m_TechniqueList.end() ; ++iter )
	{
		(*iter)->StopAllTextureEffect( bStop );
	}
}

void ioMaterial::SetLocalGlow( bool b )
{
	m_bLocalGlow = b;
}

void ioMaterial::ConvertShaderStaticToVertexColor()
{
	for_each( m_TechniqueList.begin(),
		      m_TechniqueList.end(),
			  mem_fun(&ioTechnique::ConvertShaderStaticToVertexColor) );
}

void ioMaterial::ConvertShaderAnimateToStatic()
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  mem_fun(&ioTechnique::ConvertShaderAnimateToStatic) );
}

void ioMaterial::ConvertShaderStaticToAnimate()
{
	for_each( m_TechniqueList.begin(),
			  m_TechniqueList.end(),
			  mem_fun(&ioTechnique::ConvertShaderStaticToAnimate) );
}