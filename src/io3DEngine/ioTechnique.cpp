

#include "stdafx.h"

#include "ioPass.h"
#include "ioTechnique.h"
#include "ioMaterial.h"

using namespace std;

ioTechnique::ioTechnique( ioMaterial *pParent )
{
	m_pParent = pParent;
	m_bSupported = true;
	m_bLoadDone = false;
	m_bCustomTex = false;
}

ioTechnique::ioTechnique( ioMaterial *pParent, const ioTechnique &rhs )
{
	m_pParent = pParent;
	*this = rhs;

	m_bLoadDone = false;
	m_bCustomTex = false;
}

ioTechnique::~ioTechnique()
{
	RemoveAllPass();
}

void ioTechnique::Load()
{
	m_bLoadDone = false;

	for_each( m_PassList.begin(), m_PassList.end(), mem_fun(&ioPass::Load) );
}

bool ioTechnique::IsLoaded() const
{
	return m_pParent->IsLoaded();
}

bool ioTechnique::IsLoadDone()
{
	if( m_bLoadDone )
		return true;

	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		if( !(*iter)->IsLoadDone() )
		{
			m_bLoadDone = false;
			return false;
		}
	}

	m_bLoadDone = true;
	return true;
}

ErasePolicy ioTechnique::GetTextureErasePolicy() const
{
	return m_pParent->GetTextureErasePolicy();
}

ioPass* ioTechnique::CreatePass()
{
	ioPass *pNewPass = new ioPass( this, m_PassList.size() );
	m_PassList.push_back( pNewPass );

	return pNewPass;
}

ioPass* ioTechnique::CreatePassInFront()
{
	PassList eTempList( m_PassList );
	m_PassList.erase( m_PassList.begin(), m_PassList.end() );

	ioPass *pNewPass = new ioPass( this, 0 );
	m_PassList.push_back( pNewPass );

	int iPassCnt = eTempList.size();
	for( int iPass = 0 ; iPass < iPassCnt ; iPass++ )
	{
		ioPass *pPrePass = eTempList[iPass];
		pPrePass->SetPassIndex( iPass + 1 );
		m_PassList.push_back( pPrePass );
	}

	return pNewPass;
}

ioPass* ioTechnique::GetPass( int iIndex )
{
	if( !COMPARE( iIndex, 0, GetNumPasses() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioTechnique::GetPass - overflow" );
		return NULL;
	}

	return m_PassList[ iIndex ];
}

ioPass* ioTechnique::GetPassByShaderGrp( ShaderGroupType eType )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		if( (*iter)->GetShaderGroupType() == eType )
			return *iter;
	}

	return NULL;
}

int ioTechnique::GetPassCntByShaderGrp( ShaderGroupType eType ) const
{
	int iPassCnt = 0;

	PassList::const_iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		if( (*iter)->GetShaderGroupType() == eType )
		{
			iPassCnt++;
		}
	}

	return iPassCnt;
}

int ioTechnique::RemovePass( int iIndex )
{
	if( COMPARE( iIndex, 0, GetNumPasses() ) )
	{
		PassList::iterator iter = m_PassList.begin() + iIndex;
		delete *iter;
		m_PassList.erase( iter );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioTechnique::RemovePass - overflow" );
	}

	return GetNumPasses();
}

void ioTechnique::RemoveAllPass()
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		delete *iter;
	}

	m_PassList.clear();
}

void ioTechnique::ReArrangePassIndex()
{
	int iPassIndex = 0;
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetPassIndex( iPassIndex );
		iPassIndex++;
	}
}

const ioTechnique::PassIterator ioTechnique::GetPassIterator()
{
	return PassIterator( m_PassList.begin(), m_PassList.end() );
}

ioTechnique& ioTechnique::operator = ( const ioTechnique &rhs )
{
	m_bSupported = rhs.m_bSupported;

	RemoveAllPass();

	PassList::const_iterator iter;
	for( iter=rhs.m_PassList.begin() ; iter!=rhs.m_PassList.end() ; ++iter )
	{
		ioPass *pPass = new ioPass( this, (*iter)->GetPassIndex(), *(*iter) );
		m_PassList.push_back( pPass );
	}

	return *this;
}

bool ioTechnique::operator==( const ioTechnique &rhs ) const
{
	if( m_PassList.size() != rhs.m_PassList.size() )
		return false;

	int iPassCnt = m_PassList.size();
	for( int i=0 ; i<iPassCnt ; i++ )
	{
		if( *m_PassList[i] != *rhs.m_PassList[i] )
			return false;
	}

	return true;
}

bool ioTechnique::operator!=( const ioTechnique &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

const D3DCOLORVALUE& ioTechnique::GetAmbient() const
{
	return m_pParent->GetAmbient();
}

const D3DCOLORVALUE& ioTechnique::GetDiffuse() const
{
	return m_pParent->GetDiffuse();
}

const D3DCOLORVALUE& ioTechnique::GetEmissive() const
{
	return m_pParent->GetEmissive();
}

const D3DCOLORVALUE& ioTechnique::GetSpecular() const
{
	return m_pParent->GetSpecular();
}

float ioTechnique::GetSpecularPower() const
{
	return m_pParent->GetSpecularPower();
}

float ioTechnique::GetSpecularLv() const
{
	return m_pParent->GetSpecularLv();
}

D3DXVECTOR4 ioTechnique::GetSpecularParam() const
{
	return m_pParent->GetSpecularParam();
}

void ioTechnique::NeedUpdateAmbientEmissiveCache()
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  mem_fun( &ioPass::NeedUpdateAmbientEmissiveCache ) );
}

void ioTechnique::SetDepthCheckEnable( bool bEnable )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetDepthCheckEnable( bEnable );
	}
}

void ioTechnique::SetDepthWriteEnable( bool bEnable )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetDepthWriteEnable( bEnable );
	}
}

void ioTechnique::SetDepthFunction( D3DCMPFUNC eFunction )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetDepthFunction( eFunction );
	}
}

void ioTechnique::SetColorWriteEnable( bool bEnable )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetColorWriteEnable( bEnable );
	}
}

void ioTechnique::SetCullingMode( CullingMode eMode )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetCullingMode( eMode );
	}
}

void ioTechnique::SetLightingEnable( bool bEnable )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetLightingEnable( bEnable );
	}
}

void ioTechnique::SetShadeType( ShadeType eType )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetShadeType( eType );
	}
}

void ioTechnique::SetFillMode( FillMode eMode )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetFillMode( eMode );
	}
}

void ioTechnique::SetDepthBias( float fDepthBias, float fBiasSlope )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetDepthBias( fDepthBias, fBiasSlope );
	}
}

void ioTechnique::SetTextureFiltering( TextureFilterOption filter )
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  bind2nd( mem_fun(&ioPass::SetTextureFiltering), filter ) );
}

void ioTechnique::SetTextureMipMapBias( float fBias )
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  bind2nd( mem_fun(&ioPass::SetTextureMipMapBias), fBias ) );
}

void ioTechnique::SetTextureAnisotropy( int iMaxAniso )
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  bind2nd( mem_fun(&ioPass::SetTextureAnisotropy), iMaxAniso ) );
}

void ioTechnique::SetSceneBlending( SceneBlendMode eMode )
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  bind2nd( mem_fun(&ioPass::SetSceneBlending), eMode ) );
}

void ioTechnique::SetSceneBlending( D3DBLENDOP eOp, D3DBLEND eSource, D3DBLEND eDest )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetSceneBlending( eOp, eSource, eDest );
	}
}

void ioTechnique::SetShaderGroup( const ioHashString &szName )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->SetShaderGroup( szName );
	}
}

void ioTechnique::StopAllTextureEffect( bool bStop )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		(*iter)->StopAllTextureEffect( bStop );
	}
}

bool ioTechnique::IsDepthWriteEnabled() const
{
	if( !m_PassList.empty() )
		return m_PassList.front()->GetDepthWriteEnable();

	return false;
}

bool ioTechnique::IsDepthCheckEnabled() const
{
	if( !m_PassList.empty() )
		return m_PassList.front()->GetDepthCheckEnable();

	return false;
}

bool ioTechnique::IsTransparent() const
{
	if( !m_PassList.empty() )
		return m_PassList.front()->IsTransparent();

	return false;
}

bool ioTechnique::HasShaderGroupPass( ShaderGroupType eType )
{
	PassList::iterator iter;
	for( iter=m_PassList.begin() ; iter!=m_PassList.end() ; ++iter )
	{
		if( (*iter)->GetShaderGroupType() == eType )
			return true;
	}

	return false;
}

void ioTechnique::ConvertShaderStaticToVertexColor()
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  mem_fun( &ioPass::ConvertShaderStaticToVertexColor ) );
}

void ioTechnique::ConvertShaderAnimateToStatic()
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  mem_fun( &ioPass::ConvertShaderAnimateToStatic ) );
}

void ioTechnique::ConvertShaderStaticToAnimate()
{
	for_each( m_PassList.begin(),
			  m_PassList.end(),
			  mem_fun( &ioPass::ConvertShaderStaticToAnimate ) );
}