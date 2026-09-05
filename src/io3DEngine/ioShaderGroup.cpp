

#include "stdafx.h"

#include "ioShader.h"
#include "ioShaderGroup.h"
#include "ioShaderManager.h"
#include "ioLight.h"

ioShaderGroup::ioShaderGroup( const ioHashString &name ) : ioResource( name )
{
	m_GroupType = SGT_DEFAULT;

	for( int i=0 ; i<LC_MAX_COMBINATION ; i++ )
	{
		m_ShaderTable[i] = NULL;
	}

	m_pGenShadow = NULL;
	m_pRenderShadow = NULL;
}

ioShaderGroup::~ioShaderGroup()
{
	for( int i=0 ; i<LC_MAX_COMBINATION ; i++ )
	{
		UnLoadShader( m_ShaderTable[i] );
	}

	UnLoadShader( m_pGenShadow );
	UnLoadShader( m_pRenderShadow );
}

void ioShaderGroup::SetGroupType( ShaderGroupType eType )
{
	m_GroupType = eType;
}

void ioShaderGroup::AddShader( ioShader *pShader )
{
	ShaderType eType = pShader->GetShaderType();
	if( eType == ST_NORMAL_SHADER )
	{
		LightCombination eCombo = pShader->GetLightCombination();

		assert( COMPARE( eCombo, LC_NOLIGHT, LC_MAX_COMBINATION ) );

		UnLoadShader( m_ShaderTable[eCombo] );
		m_ShaderTable[eCombo] = pShader;
	}
	else if( eType == ST_GEN_SHADOW_SHADER )
	{
		UnLoadShader( m_pGenShadow );
		m_pGenShadow = pShader;
	}
	else if( eType == ST_RENDER_SHADOW_SHADER )
	{
		UnLoadShader( m_pRenderShadow );
		m_pRenderShadow = pShader;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioShaderGroup::AddShader - %s UnKnwon Type(%d) Shader"
							  , pShader->GetName().c_str(), eType );

		UnLoadShader( pShader );
	}
}

ioShader* ioShaderGroup::GetShader( LightCombination eLight )
{
	return m_ShaderTable[eLight];
}

ioShader* ioShaderGroup::GetShader( const ioLight *pLight )
{
	ioShader *pShader = NULL;
	if( pLight )
	{
		switch( pLight->GetType() )
		{
		case LT_DIRECTIONAL:
			pShader = m_ShaderTable[LC_DIR];
			break;
		case LT_POINT:
			pShader = m_ShaderTable[LC_POINT];
			break;
		case LT_SPOT:
			pShader = m_ShaderTable[LC_SPOT];
			break;
		}
	}

	if( !pShader )
		pShader = m_ShaderTable[LC_NOLIGHT];

	return pShader;
}

ioShader* ioShaderGroup::GetShader( const ioLightList &rkLightList )
{
	if( rkLightList.empty() )
		return m_ShaderTable[LC_NOLIGHT];

	ioShader *pShader = NULL;
	if( rkLightList.size() == 1 )
	{
		const ioLight *pFirst = rkLightList.front();

		switch( pFirst->GetType() )
		{
		case LT_DIRECTIONAL:
			pShader = m_ShaderTable[LC_DIR];
			break;
		case LT_POINT:
			pShader = m_ShaderTable[LC_POINT];
			break;
		case LT_SPOT:
			pShader = m_ShaderTable[LC_SPOT];
			break;
		}
	}
	else
	{
		const ioLight *pSecond = rkLightList[1];

		switch( pSecond->GetType() )
		{
		case LT_POINT:
			if( m_ShaderTable[LC_DIR_POINT] )
				pShader = m_ShaderTable[LC_DIR_POINT];
			break;
		case LT_SPOT:
			if( m_ShaderTable[LC_DIR_SPOT] )
				pShader = m_ShaderTable[LC_DIR_SPOT];
			break;
		}

		if( !pShader )
			pShader = m_ShaderTable[LC_DIR];
	}

	if( !pShader )
		pShader = m_ShaderTable[LC_NOLIGHT];

	return pShader;
}

void ioShaderGroup::UnLoadShader( ioShader* &pShader )
{
	if( pShader )
	{
		ioShaderManager::GetSingleton().Destroy( pShader );
		pShader = NULL;
	}
}
