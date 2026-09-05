

#include "stdafx.h"

#include "ioMath.h"
#include "ioStringConverter.h"
#include "ioStringInterface.h"
#include "ioRenderable.h"
#include "ioMovableObject.h"

#include "ioOrientBox.h"

#include "ioParticleEmitter.h"
#include "ioEmitPointGenerator.h"

#include "ioParticle.h"
#include "ioParticleAffector.h"
#include "ioLinearForceAffector.h"
#include "ioScaleAffector.h"
#include "ioTexRotationAffector.h"
#include "ioEmitAffector.h"
#include "ioRotationAffector.h"
#include "ioBipedTrailAffector.h"
#include "ioParticleSystem.h"

#include "ioModelParticle.h"
#include "ioModelParticleAffector.h"
#include "ioModelLinearForceAffector.h"
#include "ioModelRotateAffector.h"
#include "ioModelScaleAffector.h"
#include "ioModelEmitAffector.h"
#include "ioModelBipedTrailAffector.h"
#include "ioModelEmitter.h"
#include "ioModelParticleSystem.h"

#include "ioLightSystem.h"

#include "ioAxisAlignBox.h"
#include "ioNode.h"
#include "ioEntity.h"
#include "ioRenderQueue.h"
#include "ioSceneNode.h"
#include "ioSceneManager.h"

#include "ioEffect.h"
#include "ioEffectFactory.h"
#include "ioEffectBufferManager.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ioFrameTimer.h"

using namespace std;

template<> ioEffectFactory* Singleton<ioEffectFactory>::ms_Singleton = 0;

ioEffectFactory::ioEffectFactory() : m_StaticSwitch( SATICS_OFF )
{
	strcpy( m_szStartDir, "Effect" );

	m_pEffectBuffer = new ioEffectBufferManager;
}

ioEffectFactory::~ioEffectFactory()
{
	if( !m_EffectMap.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioEffectFactory : Not Erase Effect : %d", m_EffectMap.size() );

		EffectMap::iterator iter;
		for( iter=m_EffectMap.begin() ; iter!=m_EffectMap.end() ; ++iter )
		{
			delete iter->second;
		}
		m_EffectMap.clear();
	}

	DestroyTemplate();

	SAFEDELETE( m_pEffectBuffer );
}

ioEffectFactory& ioEffectFactory::GetSingleton()
{
	return Singleton<ioEffectFactory>::GetSingleton();
}

void ioEffectFactory::DestroyTemplate()
{
	if( !m_EffectMap.empty() )
	{
		LOG.PrintTimeAndLog( 0, "ioEffectFactory::DestroyTemlate - %d Effect Exist", m_EffectMap.size() );
	}

	EffectTempleteMap::iterator iter = m_EffectTempleteMap.begin();
	for( ; iter != m_EffectTempleteMap.end() ; ++iter )
	{
		RemoveMem( iter->second->GetFileName() );
		delete iter->second;
	}
	m_EffectTempleteMap.clear();
	m_EffectTempleteTimeMap.clear();
	
}

void ioEffectFactory::DestroyTemplateEffect( const ioHashString& szFileName )
{
	EffectTempleteMap::iterator iter = m_EffectTempleteMap.find( szFileName );
	if( iter != m_EffectTempleteMap.end() )
	{
		RemoveMem( szFileName );
		ioEffect* pEffect = iter->second;

		delete pEffect;
		m_EffectTempleteMap.erase( iter );

	}
}

void ioEffectFactory::DestroyTimeTemplate( const ioHashString& szFileName )
{
	EffectTempleteTimeMap::iterator iter = m_EffectTempleteTimeMap.find( szFileName );
	if( iter != m_EffectTempleteTimeMap.end() )
		m_EffectTempleteTimeMap.erase( iter );
}

void ioEffectFactory::ReleaseTimeTemplate( const ioHashString& szFileName )
{
		EffectTempleteTimeMap::iterator iter = m_EffectTempleteTimeMap.find( szFileName );
		if( iter != m_EffectTempleteTimeMap.end() )
		{
			EffectTimeStamp& Stamp = iter->second;
			Stamp.iRefCount--;
	}
}

void ioEffectFactory::AddMem( const ioHashString& KeyName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		m_MemoryMap.insert( MemoryMap::value_type( KeyName, dwMem ) );
	}
}

void ioEffectFactory::CloneMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{		
			m_CloneMemoryMap.insert( MemoryMap::value_type( iter->first, iter->second ) );
		}
	}
}

void ioEffectFactory::RemoveMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{
			m_MemoryMap.erase( iter );
		}
	}
}

void ioEffectFactory::RemoveCloneMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_CloneMemoryMap.find( KeyName );
		if( iter != m_CloneMemoryMap.end() )
		{
			m_CloneMemoryMap.erase( iter );
		}
	}
}

void ioEffectFactory::GetMemStatics( DWORD& dwTempleteCount, DWORD& dwTempleteSize, DWORD& dwCloneCount, DWORD& dwCloneSize )
{
	dwTempleteCount	= 0;
	dwTempleteSize	= 0;
	dwCloneCount	= 0;
	dwCloneSize		= 0;

	for( MemoryMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwTempleteSize	+= iter->second;
		dwTempleteCount++;
	}

	for( MemoryMap::iterator iter = m_CloneMemoryMap.begin(); iter != m_CloneMemoryMap.end(); ++iter )
	{
		dwCloneSize	+= iter->second;
		dwCloneCount++;
	}
}

void ioEffectFactory::InitMemoryPoolSize( int iParticle, int iModelParticle )
{
	ioParticle::GetSharedPool().InitPreserve( iParticle );
	ioModelParticle::GetSharedPool().InitPreserve( iModelParticle );
}

void ioEffectFactory::SetStartDir( const char *szPath )
{
	strcpy( m_szStartDir, szPath );
}

void ioEffectFactory::SetDefaultIterationInterval( float fInterval )
{
	ioParticleSystem::SetDefaultIterationInterval( fInterval );
	ioModelParticleSystem::SetDefaultIterationInterval( fInterval );
}

void ioEffectFactory::SetLimitMaxParticleQuota( bool bLimit, float fLimitRate )
{
	ioParticleSystem::SetLimitMaxParticleQuota( bLimit, fLimitRate );
	ioModelParticleSystem::SetLimitMaxParticleQuota( bLimit, fLimitRate );
}

void ioEffectFactory::PrintCurEffectList() const
{
	EffectMap::const_iterator iter;
	for( iter=m_EffectMap.begin() ; iter!=m_EffectMap.end() ; ++iter )
	{
		LOG.PrintLog( 0, "%s", iter->second->GetFileName().c_str() );
	}
}

ioEffect* ioEffectFactory::CreateEffect( const ioHashString &kFileName )
{
	if( kFileName.IsEmpty() )
		return NULL;

	EffectTempleteMap::iterator iter = m_EffectTempleteMap.find( kFileName );
	if( iter != m_EffectTempleteMap.end() )
		return CloneEffect( iter->second );

	ioEffect *pNewTemplete = new ioEffect;

	if( !LoadEffect( kFileName, pNewTemplete ) )
	{
		SAFEDELETE( pNewTemplete );
		DestroyTimeTemplate( kFileName );

		return NULL;
	}

	m_EffectTempleteMap.insert( EffectTempleteMap::value_type( kFileName, pNewTemplete ) );	
	return CloneEffect( pNewTemplete );
}

ioEffect* ioEffectFactory::GetEffect( UniqueObjID eObjID )
{
	EffectMap::iterator iter = m_EffectMap.find( eObjID );
	if( iter!=m_EffectMap.end() )
		return iter->second;

	return NULL;
}

void ioEffectFactory::DestroyEffect( UniqueObjID eObjID )
{
	EffectMap::iterator iter = m_EffectMap.find( eObjID );
	if( iter != m_EffectMap.end() )
	{
		ioEffect* pEffect = iter->second;
		if( pEffect )
		{
			RemoveCloneMem( pEffect->GetFileName() );
			ReleaseTimeTemplate( pEffect->GetFileName() );
		}

		delete iter->second;
		m_EffectMap.erase(iter);
	}
}

void ioEffectFactory::DestroyEffect( ioEffect *pEffect )
{
	if( pEffect )
	{
		DestroyEffect( pEffect->GetUniqueID() );
	}
}

const ioEffect* ioEffectFactory::GetScript( const ioHashString &kFileName )
{
	EffectTempleteMap::const_iterator iter = m_EffectTempleteMap.find( kFileName );
	if( iter != m_EffectTempleteMap.end() )
		return iter->second;

	return NULL;
}

void ioEffectFactory::ReLoadEffectScript( const ioHashString &kFileName )
{
	EffectTempleteMap::iterator iter = m_EffectTempleteMap.find( kFileName );
	if( iter == m_EffectTempleteMap.end() )
		return;	// 기존에 로드된게 없어서 갱신할 필요가 없다

	ioEffect *pNewScript = new ioEffect;

	if( !LoadEffect( kFileName, pNewScript ) )
	{
		SAFEDELETE( pNewScript );
		return;
	}

	delete iter->second;	// 기존 스크립트 삭제
	iter->second = pNewScript;
}

ioEffect* ioEffectFactory::CloneEffect( ioEffect *pEffect )
{
	ioEffect *pClone = new ioEffect;
	*pClone = *pEffect;
	pClone->InitResource();

	m_EffectMap.insert( EffectMap::value_type( pClone->GetUniqueID(), pClone ) );
	CloneMem( pClone->GetFileName() );

	EffectTempleteTimeMap::iterator iter = m_EffectTempleteTimeMap.find( pEffect->GetFileName() );
	if( iter != m_EffectTempleteTimeMap.end() )	
	{
		EffectTimeStamp& Stamp = iter->second;
		Stamp.dwTime = FRAMEGETTIME();
		Stamp.iRefCount++;
	}
	else
	{
		EffectTimeStamp Stamp;
		Stamp.dwTime = FRAMEGETTIME();
		Stamp.iRefCount++;
		m_EffectTempleteTimeMap.insert( EffectTempleteTimeMap::value_type( pEffect->GetFileName(), Stamp ) );
	}


	return pClone;
}

bool ioEffectFactory::LoadEffect( const ioHashString &kFileName, ioEffect *pEffect )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "%s/%s", m_szStartDir, kFileName.c_str() );
	
	ioTextStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEffectFactory::LoadEffect - %s Load Failed", szFileName );
		return false;
	}

	pEffect->SetFileName( kFileName );
	ParseScript( pEffect, kStream );
	AddMem( pEffect->GetFileName(), kStream.GetSize() );

	return true;
}


void ioEffectFactory::ParseScript( ioEffect *pEffect, ioTextStream &rkStream )
{
	string line, tagName;

	int iParticleSystemCnt = 0;

	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;

		tagName = ioStringConverter::SplitFirstOnly( line, "\t " );
		if( tagName == "info" )
		{
			rkStream.SkipToNextOpenBrace();
			ParseEffectInfo( rkStream, pEffect );
		}
		else if( tagName == "particle" )
		{
			rkStream.SkipToNextOpenBrace();
			ParseNewParticle( rkStream, pEffect, iParticleSystemCnt );
			iParticleSystemCnt++;
		}
		else if( tagName == "model" )
		{
			rkStream.SkipToNextOpenBrace();
			ParseNewModel( rkStream, pEffect );
		}
		else if( tagName == "light" )
		{
			rkStream.SkipToNextOpenBrace();
			ParseNewLight( rkStream, pEffect );
		}
	}
}

ioParticleEmitter* ioEffectFactory::CreateEmitter( const ioHashString &kType )
{
	ioEmitPointGenerator *pGenerator = CreateEmitPointGenerator( kType );
	if( pGenerator )
		return new ioParticleEmitter( pGenerator );

	return NULL;
}

ioParticleAffector* ioEffectFactory::CreateAffector( const ioHashString &kType )
{
	if( kType == "Linear" )
		return new ioLinearForceAffector;

	if( kType == "Scale" )
		return new ioScaleAffector;

	if( kType == "TexRotation")
		return new ioTexRotationAffector;
	
	if( kType == "Emit" )
		return new ioEmitAffector;
	
	if( kType == "Rotation" )
		return new ioRotationAffector;

	if( kType == "Trail" )
		return new ioBipedTrailAffector;

	LOG.PrintTimeAndLog( 0, "ioEffectFactory::CreateAffector - Unknown Type : %s",
							kType.c_str() );

	return NULL;
}

ioModelEmitter* ioEffectFactory::CreateModelEmitter( const ioHashString &kType )
{
	ioEmitPointGenerator *pGenerator = CreateEmitPointGenerator( kType );
	if( pGenerator )
		return new ioModelEmitter( pGenerator );

	return NULL;
}

ioModelParticleAffector* ioEffectFactory::CreateModelAffector( const ioHashString &kType )
{
	if( kType == "Linear" )
		return new ioModelLinearForceAffector;

	if( kType == "Rotation" )
		return new ioModelRotateAffector;

	if( kType == "Scale" )
		return new ioModelScaleAffector;

	if( kType == "Emit")
		return new ioModelEmitAffector;

	if( kType == "Trail" )
		return new ioModelBipedTrailAffector;

	LOG.PrintTimeAndLog( 0, "ioEffectFactory::CreateModelAffector - Unknown Type : %s",
							 kType.c_str() );

	return NULL;
}

ioEmitPointGenerator* ioEffectFactory::CreateEmitPointGenerator( const ioHashString &kType )
{
	if( kType == "Point" )
		return new ioPointEmitGenerator;
	else if( kType == "Box" )
		return new ioBoxEmitGenerator;
	else if( kType == "Ellipse" )
		return new ioEllipseEmitGenerator;
	else if( kType == "Cylinder" )
		return new ioCylinderEmitGenerator;
	else if( kType == "Ellipsoid" )
		return new ioEllipsoidEmitGenerator;
	else if( kType == "HollowEllipsoid" )
		return new ioHollowEllipsoidEmitGenerator;

	LOG.PrintTimeAndLog( 0, "ioEffectFactory::CreateEmitPointGenerator - Unknown Type:%s",
							kType.c_str() );

	return NULL;
}

void ioEffectFactory::ParseEffectInfo( ioTextStream &rkStream, ioEffect *pEffect )
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
		if( tagName == "collision_box" )
		{
			ParseAttribute( line, pEffect );
		}
		else if( tagName == "skeleton_name" )
		{
			ParseAttribute( line, pEffect );
		}
		else if( tagName == "animation_name" )
		{
			ParseAttribute( line, pEffect );
		}
		else if( tagName == "direction" )
		{
			ParseAttribute( line, pEffect );
		}
		else if( tagName == "cull_each_system" )
		{
			ParseAttribute( line, pEffect );
		}
		else if( tagName == "update_always" )
		{
			ParseAttribute( line, pEffect );
		}
	}
}

void ioEffectFactory::ParseNewParticle( ioTextStream &rkStream,
										ioEffect *pEffect,
										int iIndex )
{
	ioParticleSystem *pSys = new ioParticleSystem;
	pEffect->AddParticleSys( pSys );

	string line, tagName;

	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;

		if( line[0] == '}' )
			break;

		tagName = ioStringConverter::SplitFirstOnly( line, "\t " );
		if( tagName == "emitter" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, "\t " );
			if( vecparams.size() < 2 )
			{
				LOG.PrintTimeAndLog( 0, "ioEffectFactory::ParseNewParticle - Error %s in %s",
									    line.c_str(), pEffect->GetFileName().c_str() );

				rkStream.SkipToNextCloseBrace();
			}

			rkStream.SkipToNextOpenBrace();

			ParseNewEmitter( ioHashString( vecparams[1].c_str() ), rkStream, pSys );
		}
		else if( tagName == "affector" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, "\t " );
			if( vecparams.size() < 2 )
			{
				LOG.PrintTimeAndLog( 0, "ioEffectFactory::ParseNewParticle - Error %s in %s",
									    line.c_str(), pEffect->GetFileName().c_str() );

				rkStream.SkipToNextCloseBrace();
			}

			rkStream.SkipToNextOpenBrace();

			ParseNewAffector( ioHashString( vecparams[1].c_str() ), rkStream, pSys );
		}
		else
		{
			ParseAttribute( line, pSys );
		}
	}
}

void ioEffectFactory::ParseNewLight( ioTextStream &rkStream, ioEffect *pEffect )
{
	ioLightSystem *pLightSys = pEffect->CreateLightSystem();
	pLightSys->dwLightAttribute = ioLight::LAM_REAL_TIME | ioLight::LAM_EFFECT_LIGHT;

	string line;
	
	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;

		if( line[0] == '}' )
			break;

		const StringVector &vecparams = ioStringConverter::Split( line, "\t ", 1 );
		if( vecparams.size() == 2 )
		{
			if( vecparams[0] == "light_texture" )
			{
				pLightSys->szLightTexture = vecparams[1].c_str();
			}
			else if( vecparams[0] == "decal_texture" )
			{
				pLightSys->szDecalTexture = vecparams[1].c_str();
			}
			else if( vecparams[0] == "type" )
			{
				if( vecparams[1] == "point" )
					pLightSys->Light_Type = LT_POINT;
				else if( vecparams[1] == "spot" )
					pLightSys->Light_Type = LT_SPOT;
				else if( vecparams[1] == "directional" )
					pLightSys->Light_Type = LT_DIRECTIONAL;
			}
			else if( vecparams[0] == "position" )
			{
				pLightSys->m_vPosition = ioStringConverter::ParseVector3( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "direction" )
			{
				pLightSys->m_vDirection = ioStringConverter::ParseVector3( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "targetpos" )
				pLightSys->m_vTargetPos = ioStringConverter::ParseVector3( vecparams[1].c_str() );
			else if( vecparams[0] == "diffuse" )
			{
				pLightSys->m_Diffuse = ioStringConverter::ParseColorValue( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "specular" )
			{
				pLightSys->m_Specular = ioStringConverter::ParseColorValue( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "attenuation" )
			{
				D3DXVECTOR4 vAttenuation = ioStringConverter::ParseVector4( vecparams[1].c_str() );
				pLightSys->m_fRange				= vAttenuation.x;
				pLightSys->m_fAttenuationConst  = vAttenuation.y;
				pLightSys->m_fAttenuationLinear = vAttenuation.z;
				pLightSys->m_fAttenuationQuad   = vAttenuation.w;
			}
			else if( vecparams[0] == "light_start" )
			{
				pLightSys->dwLight_s = ioStringConverter::ParseUnsignedInt( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "light_end" )
			{
				pLightSys->dwLight_e = ioStringConverter::ParseUnsignedInt( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "spot_theta" )
			{
				pLightSys->m_fSpotInner = ioStringConverter::ParseFloat( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "spot_phi" )
			{
				pLightSys->m_fSpotOuter = ioStringConverter::ParseFloat( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "spot_falloff" )
			{
				pLightSys->m_fSpotFallOff = ioStringConverter::ParseFloat( vecparams[1].c_str() );
			}
			else if( vecparams[0] == "bright_rate" )
			{
				std::vector< D3DXVECTOR2 > vRateList;
				ioStringConverter::ParseVector2Vec( vecparams[1].c_str(), vRateList );

				pLightSys->m_BrightRate.RemoveAllFactor();
				for( int i=0 ; i<(int)vRateList.size() ; i++ )
				{
					pLightSys->m_BrightRate.AddTimeFactor( vRateList[i].x, vRateList[i].y );
				}
			}
			else if( vecparams[0] == "no_update_pos" )
			{
				pLightSys->m_bNoUpdatePosition = ioStringConverter::ParseBool( vecparams[1].c_str() );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ParseAttribute - %s is wrong, (expected 2)", line.c_str() );
		}
	}
}

void ioEffectFactory::ParseNewEmitter( const ioHashString &kType,
									   ioTextStream &rkStream,
									   ioParticleSystem *pSys )
{
	ioParticleEmitter *pEmit = pSys->SetEmitter( kType );

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
		else
		{
			ParseAttribute( line, pEmit );
		}
	}

	pEmit->BuildColorTable();
}

void ioEffectFactory::ParseNewAffector( const ioHashString &kType,
									    ioTextStream &rkStream,
									    ioParticleSystem *pSys )
{
	ioParticleAffector *pAff = pSys->AddAffector( kType );

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
		else
		{
			ParseAttribute( line, pAff );
		}
	}
}

void ioEffectFactory::ParseNewModel( ioTextStream &rkStream, ioEffect *pEffect )
{
	ioModelParticleSystem *pSys = new ioModelParticleSystem;
	pEffect->AddModelSystem( pSys );

	string line, tagName;

	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );
	
		if( line.length() == 0 || line.substr(0, 2) == "//" )
			continue;

		if( line[0] == '}' )
			break;

		tagName = ioStringConverter::SplitFirstOnly( line, "\t " );
		if( tagName == "emitter" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, "\t " );
			if( vecparams.size() < 2 )
			{
				LOG.PrintTimeAndLog( 0, "ioEffectFactory::ParseNewModel - Error %s in %s",
									    line.c_str(), pEffect->GetFileName().c_str() );
				rkStream.SkipToNextCloseBrace();
			}

			rkStream.SkipToNextOpenBrace();

			ParseNewModelEmitter( ioHashString( vecparams[1].c_str() ), rkStream, pSys );
		}
		else if( tagName == "affector" )
		{
			const StringVector &vecparams = ioStringConverter::Split( line, "\t " );
			if( vecparams.size() < 2 )
			{
				LOG.PrintTimeAndLog( 0, "ioEffectFactory::ParseNewModel - Error %s in %s",
									     line.c_str(), pEffect->GetFileName().c_str() );

				rkStream.SkipToNextCloseBrace();
			}

			rkStream.SkipToNextOpenBrace();

			ParseNewModelAffector( ioHashString( vecparams[1].c_str() ),
								   rkStream,
								   pSys );
		}
		else
		{
			ParseAttribute( line, pSys );
		}
	}
}

void ioEffectFactory::ParseNewModelEmitter( const ioHashString &kType,
										    ioTextStream &rkStream,
										    ioModelParticleSystem *pSys )
{
	ioModelEmitter *pEmit = pSys->SetEmitter( kType );

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
		else
		{
			ParseAttribute( line, pEmit );
		}
	}
}

void ioEffectFactory::ParseNewModelAffector( const ioHashString &kType,
											 ioTextStream &rkStream,
											 ioModelParticleSystem *pSys )
{
	ioModelParticleAffector *pAff = pSys->AddAffector( kType );

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
		else
		{
			ParseAttribute( line, pAff );
		}
	}
}

void ioEffectFactory::ParseAttribute( const std::string &line, ioStringInterface *pInter )
{
	const StringVector &vecparams = ioStringConverter::Split( line, "\t ", 1 );
	if( vecparams.size() == 2 )
	{
		if( !pInter->SetParameter( vecparams[0].c_str(), vecparams[1].c_str() ) )
		{
			LOG.PrintTimeAndLog( 0, "ParseAttribute - Error %s line", line.c_str() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ParseAttribute - %s is wrong, (expected 2)", line.c_str() );
	}
}

void ioEffectFactory::SaveEffect( const char *szStartPath, ioEffect *pEffect )
{
	if( !pEffect )	return;

	char szFilePath[MAX_PATH];
	wsprintf( szFilePath, "%s%s", szStartPath, pEffect->GetFileName().c_str() );

	FILE *fp = fopen( szFilePath, "wt" );

	SaveEffectInfo( fp, pEffect );

	int i, iParticleCnt;

	ioParticleSystem *pSys = NULL;
	iParticleCnt = pEffect->GetParticleSystemCnt();
	for( i=0 ; i<iParticleCnt ; i++ )
	{
		pSys = pEffect->GetParticleSys(i);
		SaveParticle( fp, pSys );
	}

	ioModelParticleSystem *pMdlSys = NULL;
	iParticleCnt = pEffect->GetModelSystemCnt();
	for( i=0 ; i<iParticleCnt ; i++ )
	{
		pMdlSys = pEffect->GetModelSys(i);
		SaveModel( fp, pMdlSys );
	}

	ioLightSystem *pLightSys = pEffect->GetLightSys();
	if( pLightSys )
	{
		SaveLight( fp, pLightSys );
	}

	fclose( fp );
}

void ioEffectFactory::SaveEffectInfo( FILE *fp, ioEffect *pEffect )
{
	std::string szTemp;

	fprintf( fp, "info\n" );
	fprintf( fp, "{\n" );

	if( !pEffect->GetCollisionBox().IsNull() )
	{
		szTemp = pEffect->GetParameter( "collision_box" );
		fprintf( fp, "\tcollision_box %s\n", szTemp.c_str() );
	}

	szTemp = pEffect->GetParameter( "skeleton_name" );
	if( !szTemp.empty() )
	{
		fprintf( fp, "\tskeleton_name %s\n", szTemp.c_str() );
	}

	szTemp = pEffect->GetParameter( "animation_name" );
	if( !szTemp.empty() )
	{
		fprintf( fp, "\tanimation_name %s\n", szTemp.c_str() );
	}

	szTemp = pEffect->GetParameter( "direction" );
	if( !szTemp.empty() )
	{
		fprintf( fp, "\tdirection %s\n", szTemp.c_str() );
	}

	if( pEffect->IsCullEachSystem() )
	{
		fprintf( fp, "\tcull_each_system true\n" );
	}

	if( pEffect->IsAlwaysUpdateSystem() )
	{
		fprintf( fp, "\tupdate_always true\n" );
	}

	fprintf( fp, "}\n" );
}

void ioEffectFactory::SaveParticle( FILE *fp, ioParticleSystem *pSys )
{
	fprintf( fp, "particle\n" );
	fprintf( fp, "{\n" );

	std::string szTemp;

	const ParameterList &kList = pSys->GetParameters();

	ParameterList::const_iterator iter;
	for( iter=kList.begin() ; iter!=kList.end() ; ++iter )
	{
		const ioParamCommand *pCmd = pSys->GetParameterCmd( iter->m_Name );
		if( pCmd && !pCmd->IsSkipSave( pSys ) )
		{
			szTemp = pSys->GetParameter( iter->m_Name );
			fprintf( fp, "\t%s %s\n", iter->m_Name.c_str(), szTemp.c_str() );
		}
	}

	fprintf( fp, "\n" );

	SaveEmitter( fp, pSys->GetEmitter() );

	int iAffCnt = pSys->GetNumAffector();
	for( int i=0 ; i<iAffCnt ; i++ )
	{
		SaveAffector( fp, pSys->GetAffector(i) );
	}

	fprintf( fp, "}\n" );	// particle end
}

void ioEffectFactory::SaveLight( FILE *fp, ioLightSystem *pSys )
{
	fprintf( fp, "light\n" );
	fprintf( fp, "{\n" );

	if( !pSys->szLightTexture.IsEmpty() )
	{
		fprintf( fp, "\tlight_texture %s\n", pSys->szLightTexture.c_str() );
	}

	if( !pSys->szDecalTexture.IsEmpty() )
	{
		fprintf( fp, "\tdecal_texture %s\n", pSys->szDecalTexture.c_str() );
	}

	switch( pSys->Light_Type )
	{
	case LT_POINT:
		fprintf( fp, "\ttype point\n" );
		break;
	case LT_DIRECTIONAL:
		fprintf( fp, "\ttype directional\n" );
		break;
	case LT_SPOT:
		fprintf( fp, "\ttype spot\n" );
		break;
	}

	fprintf( fp, "\tdirection %.2f %.2f %.2f\n", pSys->m_vDirection.x,
												 pSys->m_vDirection.y,
												 pSys->m_vDirection.z );

	fprintf( fp, "\ttargetpos %.2f %.2f %.2f\n", pSys->m_vTargetPos.x,
												 pSys->m_vTargetPos.y,
												 pSys->m_vTargetPos.z );

	fprintf( fp, "\tposition %.2f %.2f %.2f\n", pSys->m_vPosition.x,
												pSys->m_vPosition.y,
												pSys->m_vPosition.z );

	fprintf( fp, "\tdiffuse %.2f %.2f %.2f %.2f\n", pSys->m_Diffuse.r,
													pSys->m_Diffuse.g,
													pSys->m_Diffuse.b,
													pSys->m_Diffuse.a );

	fprintf( fp, "\tspecular %.2f %.2f %.2f %.2f\n", pSys->m_Specular.r,
													 pSys->m_Specular.g,
													 pSys->m_Specular.b,
													 pSys->m_Specular.a );

	fprintf( fp, "\tattenuation %.2f %.2f %.2f %.2f\n", pSys->m_fRange,
														pSys->m_fAttenuationConst,
														pSys->m_fAttenuationLinear,
														pSys->m_fAttenuationQuad );

	fprintf( fp, "\tspot_theta %.2f\n", pSys->m_fSpotInner );
	fprintf( fp, "\tspot_phi %.2f\n", pSys->m_fSpotOuter );
	fprintf( fp, "\tspot_falloff %.2f\n", pSys->m_fSpotFallOff );

	fprintf( fp, "\tlight_start %d\n", pSys->dwLight_s );
	fprintf( fp, "\tlight_end %d\n", pSys->dwLight_e );

	Vector2Vec vRateList;
	pSys->m_BrightRate.GetTimeFactorList( vRateList );
	std::string szBrightList = ioStringConverter::toString( vRateList );

	fprintf( fp, "\tbright_rate %s\n", szBrightList.c_str() );
	fprintf( fp, "}\n");

	if( pSys->m_bNoUpdatePosition )
	{
		fprintf( fp, "\no_update_pos true\n" );
	}
}

void ioEffectFactory::SaveEmitter( FILE *fp, ioParticleEmitter *pEmit )
{
	if( !pEmit )	return;
	
	fprintf( fp, "\temitter %s\n", pEmit->GetGeneratorType().c_str() );
	fprintf( fp, "\t{\n" );

	std::string szTemp;

	const ParameterList &rkEmitList = pEmit->GetParameters();
	ParameterList::const_iterator iter;
	for( iter = rkEmitList.begin() ; iter!=rkEmitList.end() ; ++iter )
	{
		szTemp = pEmit->GetParameter( iter->m_Name );
		fprintf( fp, "\t\t%s\t%s\n", iter->m_Name.c_str(), szTemp.c_str() );
	}

	ioEmitPointGenerator *pGenerator = pEmit->GetGenerator();
	const ParameterList &rkGenList = pGenerator->GetParameters();
	for( iter = rkGenList.begin() ; iter!=rkGenList.end() ; ++iter )
	{
		szTemp = pGenerator->GetParameter( iter->m_Name );

		fprintf( fp, "\t\t%s\t%s\n", iter->m_Name.c_str(), szTemp.c_str() );
	}

	fprintf( fp, "\t}\n\n" );	// emit end
}

void ioEffectFactory::SaveAffector( FILE *fp, ioParticleAffector *pAff )
{
	if( !pAff )	return;

	fprintf( fp, "\taffector %s\n", pAff->GetType().c_str() );
	fprintf( fp, "\t{\n");

	std::string szTemp;

	const ParameterList &kList = pAff->GetParameters();
	ParameterList::const_iterator iter;
	for( iter=kList.begin() ; iter!=kList.end() ; ++iter )
	{
		szTemp = pAff->GetParameter( iter->m_Name );
		fprintf( fp, "\t\t%s\t%s\n", iter->m_Name.c_str(), szTemp.c_str() );
	}

	fprintf( fp, "\t}\n\n");
}

void ioEffectFactory::SaveModel( FILE *fp, ioModelParticleSystem *pSys )
{
	fprintf( fp, "model\n" );
	fprintf( fp, "{\n" );

	std::string szTemp;

	const ParameterList &kList = pSys->GetParameters();
	ParameterList::const_iterator iter;
	for( iter=kList.begin() ; iter!=kList.end() ; ++iter )
	{
		const ioParamCommand *pCmd = pSys->GetParameterCmd( iter->m_Name );
		if( pCmd && !pCmd->IsSkipSave( pSys ) )
		{
			szTemp = pSys->GetParameter( iter->m_Name );
			fprintf( fp, "\t%s %s\n", iter->m_Name.c_str(), szTemp.c_str() );
		}
	}

	fprintf( fp, "\n" );

	SaveMdlEmitter( fp, pSys->GetEmitter() );

	int iAffCnt = pSys->GetNumAffector();
	for( int i=0 ; i<iAffCnt ; i++ )
	{
		SaveMdlAffector( fp, pSys->GetAffector(i) );
	}

	fprintf( fp, "}\n" );	// model end
}

void ioEffectFactory::SaveMdlEmitter( FILE *fp, ioModelEmitter *pEmit )
{
	if( !pEmit )	return;
	
	fprintf( fp, "\temitter %s\n", pEmit->GetGeneratorType().c_str() );
	fprintf( fp, "\t{\n" );

	std::string szTemp;

	const ParameterList &kList = pEmit->GetParameters();
	ParameterList::const_iterator iter;
	for( iter = kList.begin() ; iter!=kList.end() ; ++iter )
	{
		szTemp = pEmit->GetParameter( iter->m_Name );
		fprintf( fp, "\t\t%s\t%s\n", iter->m_Name.c_str(), szTemp.c_str() );
	}

	ioEmitPointGenerator *pGenerator = pEmit->GetGenerator();
	const ParameterList &rkGenList = pGenerator->GetParameters();
	for( iter = rkGenList.begin() ; iter!=rkGenList.end() ; ++iter )
	{
		szTemp = pGenerator->GetParameter( iter->m_Name );
		fprintf( fp, "\t\t%s\t%s\n", iter->m_Name.c_str(), szTemp.c_str() );
	}


	fprintf( fp, "\t}\n\n" );	// emit end
}

void ioEffectFactory::SaveMdlAffector( FILE *fp, ioModelParticleAffector *pAff )
{
	if( !pAff )	return;

	fprintf( fp, "\taffector %s\n", pAff->GetType().c_str() );
	fprintf( fp, "\t{\n");

	std::string szTemp;

	const ParameterList &kList = pAff->GetParameters();
	ParameterList::const_iterator iter;
	for( iter=kList.begin() ; iter!=kList.end() ; ++iter )
	{
		szTemp = pAff->GetParameter( iter->m_Name );
		fprintf( fp, "\t\t%s\t%s\n", iter->m_Name.c_str(), szTemp.c_str() );
	}

	fprintf( fp, "\t}\n\n");
}

void ioEffectFactory::ClearWorldEffectEventOwner( ioEntityEvent *pEntityEventOwner )
{
	if( !m_EffectMap.empty() )
	{
		EffectMap::iterator iter;
		for( iter=m_EffectMap.begin() ; iter!=m_EffectMap.end() ; ++iter )
		{
			ioEffect *pEffect = iter->second;
			if( pEffect )
			{
				if( pEffect->GetWorldEffectEventOwner() == pEntityEventOwner )
					pEffect->SetWorldEffectEventOwner( NULL );
			}
		}
	}
}

void ioEffectFactory::ProcerssTimeTempleteCheck()
{
	for( EffectTempleteTimeMap::iterator iter = m_EffectTempleteTimeMap.begin(); iter != m_EffectTempleteTimeMap.end();  )
	{
		const EffectTimeStamp& Stamp = iter->second;

		if( Stamp.iRefCount <= 0 && Stamp.dwTime + 600000 < FRAMEGETTIME() )
		{				
			DestroyTemplateEffect( iter->first );
			iter = m_EffectTempleteTimeMap.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}