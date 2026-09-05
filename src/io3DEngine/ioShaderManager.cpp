

#include "stdafx.h"

#include "ioResource.h"
#include "ioShader.h"
#include "ioShaderGroup.h"
#include "ioShaderManager.h"
#include "ioStringConverter.h"
#include "ioRenderSystem.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ErrorReport.h"

using namespace std;

template<> ioShaderManager* Singleton< ioShaderManager >::ms_Singleton = 0;

ioShaderManager::ioShaderManager( IDirect3DDevice9 *pDevice )
{
	m_pD3Device = pDevice;
	m_pD3Device->AddRef();

	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Shader" );
}

ioShaderManager::~ioShaderManager()
{
	DestroyAll();
	SAFERELEASE( m_pD3Device );
}

void ioShaderManager::DestroyAll()
{
	ShaderGroupMap::iterator iter;
	for( iter=m_ShaderGroupMap.begin() ; iter!=m_ShaderGroupMap.end() ; ++iter )
	{
		ioShaderGroup *pGrp = iter->second;
		if( pGrp->PeekRef() > 0 )
		{
			LOG.PrintTimeAndLog( 0, "%s Resource UnReleased(%d) !!", pGrp->GetName().c_str(),
																	 pGrp->PeekRef() );
		}
		
		delete pGrp;
	}

	m_ShaderGroupMap.clear();

	ioResourceManager::DestroyAll();
}

ioResource* ioShaderManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ID3DXEffect *pEffect = LoadEffect( name );

	if( pEffect )
	{
		ioShader *pShader = new ioShader( name );
		pShader->InitShader( pEffect );
		return pShader;
	}

	return NULL;
}

void ioShaderManager::ReLoadImpl( const ioHashString &name )
{
}

ioShader* ioShaderManager::CreateShader( const ioHashString &name )
{
	return dynamic_cast< ioShader* >( Create( name ) );
}

ioShader* ioShaderManager::GetShaderByName( const ioHashString &name )
{
	return dynamic_cast< ioShader* >( GetByName( name ) );
}

ID3DXEffect* ioShaderManager::LoadEffect( const ioHashString &name )
{
	char szPath[MAX_PATH];
	wsprintf( szPath, "%s/%s", m_szStartDir, name.c_str() );

	HRESULT	hr;
	ID3DXEffect *pEffect = NULL;
	
	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szPath, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioShaderManager::LoadEffect - %s Load Failed", szPath );
		return NULL;
	}
		
	hr = D3DXCreateEffect( m_pD3Device,
						   kStream.GetPtr(),
						   kStream.GetSize(),
						   NULL,
						   NULL,
						   0,
						   NULL,
						   &pEffect,
						   NULL );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "LoadEffect: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioShaderManager::LoadEffect - %s Create Failed(%X)", szPath, hr );
		return NULL;
	}

	AddMem( name, kStream.GetSize() );

	return pEffect;
}

ioShaderGroup* ioShaderManager::LoadShaderGroup( const ioHashString &name )
{
	ioShaderGroup *pGrp = GetShaderGroup( name );
	if( !pGrp )
	{
		pGrp = new ioShaderGroup(name);
		m_ShaderGroupMap.insert( ShaderGroupMap::value_type( pGrp->GetName(), pGrp ) );

		LoadGroup( pGrp );
	}

	pGrp->AddRef();

	return pGrp;
}

ioShaderGroup* ioShaderManager::GetShaderGroup( const ioHashString &name )
{
	ShaderGroupMap::iterator iter=m_ShaderGroupMap.find( name );
	if( iter != m_ShaderGroupMap.end() )
		return iter->second;

	return NULL;
}

void ioShaderManager::UnLoadShaderGroup( ioShaderGroup *pGrp )
{
	if( pGrp )
	{
		pGrp->Release();
	}
}

void ioShaderManager::OnLostDevice()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioShader *pShader = dynamic_cast< ioShader* >( iter->second );
		if( pShader )
		{
			pShader->OnLostDevice();
		}
	}
}

void ioShaderManager::OnResetDevice()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioShader *pShader = dynamic_cast< ioShader* >( iter->second );
		if( pShader )
		{
			pShader->OnResetDevice();
		}
	}
}

void ioShaderManager::LoadGroup( ioShaderGroup *pGrp )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "%s/%s/Group.txt", m_szStartDir, pGrp->GetName().c_str() );

	ioTextStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioShaderManager::LoadGroup - %s Load Failed", szFileName );
		return;
	}

	ParseScript( pGrp, kStream );
}

void ioShaderManager::ParseScript( ioShaderGroup *pGrp, ioTextStream &rkStream )
{
	string line, tagName;

	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;
		
		tagName = ioStringConverter::SplitFirstOnly( line, "\t " );
		if( tagName == "property" )
		{
			rkStream.SkipToNextOpenBrace();
			ParseProperty( pGrp, rkStream );
		}
		else if( tagName == "program" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, " \t" );
			if( vecparams.size() == 2 )
			{
				char szShaderName[MAX_PATH];
				wsprintf( szShaderName, "%s/%s", pGrp->GetName().c_str(), vecparams[1].c_str() );

				ioShader *pShader = CreateShader( szShaderName );
				if( pShader )
				{
					rkStream.SkipToNextOpenBrace();
					ParseProgram( pShader, rkStream );

					if( RenderSystem().GetShadowMapType() == SMT_R32F_TEX )
					{
						if( pShader->GetShaderType() == ST_GEN_SHADOW_SHADER ||
							pShader->GetShaderType() == ST_RENDER_SHADOW_SHADER )
						{
							pShader->ValidateTechnique( 1 );
						}
					}

					pGrp->AddShader( pShader );
				}
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioShaderManager::ParseScript - %s Error", line.c_str() );
				rkStream.SkipToNextCloseBrace();
			}
		}
	}
}

void ioShaderManager::ParseProperty( ioShaderGroup *pGrp, ioTextStream &rkStream )
{
	string line;

	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;

		if( line[0] == '}' )
		{
			break;
		}
		else if( line.substr(0, 10) == "group_type" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, " \t" );
			if( vecparams.size() == 2 )
			{
				pGrp->SetGroupType( ConvertShaderGroupType( vecparams[1] ) );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioShaderManager::ParseProperty - %s wrong parameter"
										"(expect - 1)", line.c_str() );
			}
		}
	}
}

void ioShaderManager::ParseProgram( ioShader *pShader, ioTextStream &rkStream )
{
	string line, tagName;
	
	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;

		if( line[0] == '}' )
			break;

		tagName = ioStringConverter::SplitFirstOnly( line, "\t " );
		if( tagName == "profile" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, " \t" );
			if( vecparams.size() == 3 )
			{
				pShader->SetVertexShaderProfile( ConvertVertexShaderProfile( vecparams[1] ) );
				pShader->SetPixelShaderProfile( ConvertPixelShaderProfile( vecparams[2] ) );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioShaderManager::ParseProgram - %s wrong parameter"
										"(expect - 2)", line.c_str() );
			}
		}
		else if( tagName == "light" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, " \t" );
			if( vecparams.size() == 2 )
			{
				pShader->SetLightCombination( ConvertLightCombination( vecparams[1] ) );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioShaderManager::ParseProgram - %s wrong parameter"
										"(expect - 1)", line.c_str() );
			}
		}
		else if( tagName == "type" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, " \t" );
			if( vecparams.size() == 2 )
			{
				pShader->SetShaderType( ConvertShaderType( vecparams[1] ) );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioShaderManager::ParseProgram - %s wrong parameter"
										"(expect - 1)", line.c_str() );
			}			
		}
	}
}

ioShaderManager& ioShaderManager::GetSingleton()
{
	return Singleton<ioShaderManager>::GetSingleton();
}

