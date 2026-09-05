

#include "stdafx.h"

#include "ioWorldEntityFactory.h"

#include "ioPlayStage.h"

#include "ioTeleportZone.h"
#include "ioConveyer.h"
#include "ioElevator.h"

#include "ioAffectorZone.h"
#include "ioZoneAffector.h"

ioWorldEntityFactory::ioWorldEntityFactory( ioPlayStage *pCreator )
{
	m_pCreator = pCreator;
	m_bElevationSyncStart = false;
}

ioWorldEntityFactory::~ioWorldEntityFactory()
{
}

ioPlayEntity* ioWorldEntityFactory::CreateWorldEntity( ioINILoader &rkLoader, int iCreateIndex )
{
	int iSubType = rkLoader.LoadInt_e( "entity_type", -1 );
	if( !IsKnownSubType( iSubType ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWorldEntityFactor::CreateWorldEntity - %d Unknown Entity", iSubType );
		return NULL;
	}

	char szFileName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szFileName, MAX_PATH );
	
	ioPlayEntity *pEntity = CreateSubEntity( iSubType, szFileName, iCreateIndex );
	if( pEntity )
	{
		pEntity->LoadProperty( rkLoader );
		m_WorldEntityList.push_back( pEntity );
	}
	
	return pEntity;
}

void ioWorldEntityFactory::RemoveWorldEntity( ioGameEntity *pEntity )
{
	ioPlayEntity *pPlay = ToPlayEntity( pEntity );
	if( pPlay )
	{
		m_WorldEntityList.remove( pPlay );
	}
}

bool ioWorldEntityFactory::IsKnownSubType( int iSubType ) const
{
	switch( iSubType )
	{
	case ioPlayEntity::PST_TELEPORT_ZONE:
	case ioPlayEntity::PST_CONVEYER:
	case ioPlayEntity::PST_ELEVATOR:
	case ioPlayEntity::PST_AFFECTOR_ZONE:
	case ioPlayEntity::PST_WALL_ZONE:
		return true;
	}

	return false;
}

ioPlayEntity* ioWorldEntityFactory::CreateSubEntity( int iSubType, const char *szFileName, int iCreateIndex )
{
	ioEntityGroup *pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szFileName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioWorldEntityFactory::CreateSubEntity - %s Fail!", szFileName );
		return NULL;
	}

	ioPlayMode *pMode = m_pCreator->GetPlayMode();

	ioPlayEntity *pEntity = NULL;
	switch( iSubType )
	{
	case ioPlayEntity::PST_TELEPORT_ZONE:
		pEntity = new ioTeleportZone( pGrp, pMode, iCreateIndex );
		break;
	case ioPlayEntity::PST_CONVEYER:
		pEntity = new ioConveyer( pGrp, pMode, iCreateIndex );
		break;
	case ioPlayEntity::PST_ELEVATOR:
		pEntity = new ioElevator( pGrp, pMode, iCreateIndex );
		break;
	case ioPlayEntity::PST_AFFECTOR_ZONE:
		pEntity = new ioAffectorZone( pGrp, pMode, iCreateIndex );
		break;
	case ioPlayEntity::PST_WALL_ZONE:
		pEntity = new ioWallZone( pGrp, pMode, iCreateIndex );
		break;
	}

	assert( pEntity );	// 사전에 타입체크했기때문에 없을수가 없다.

	return pEntity;
}

bool ioWorldEntityFactory::IsWorldEntity( ioPlayEntity *pPlayEntity, int iCreateIndex )
{
	if( pPlayEntity == NULL )
		return false;

	switch( pPlayEntity->GetSubType() )
	{
	case ioPlayEntity::PST_TELEPORT_ZONE:
		{
			ioTeleportZone *pEntity = ToTeleportZone( pPlayEntity );
			if( pEntity && pEntity->GetCreateIndex() == iCreateIndex )
				return true;
		}
		break;
	case ioPlayEntity::PST_CONVEYER:
		{
			ioConveyer *pEntity = ToConveyer( pPlayEntity );
			if( pEntity && pEntity->GetCreateIndex() == iCreateIndex )
				return true;
		}
		break;
	case ioPlayEntity::PST_ELEVATOR:
		{
			ioElevator *pEntity = ToElevator( pPlayEntity );
			if( pEntity && pEntity->GetCreateIndex() == iCreateIndex )
				return true;
		}
		break;
	case ioPlayEntity::PST_AFFECTOR_ZONE:
		{
			ioAffectorZone *pEntity = ToAffectorZone( pPlayEntity );
			if( pEntity && pEntity->GetCreateIndex() == iCreateIndex )
				return true;
		}
		break;
	case ioPlayEntity::PST_WALL_ZONE:
		{
			ioWallZone *pEntity = ToWallZone( pPlayEntity );
			if( pEntity && pEntity->GetCreateIndex() == iCreateIndex )
				return true;
		}
		break;
	}
	return false;
}

void ioWorldEntityFactory::EnableWorldEntityFunctions( bool bEnable )
{
	PlayEntityList::iterator iter = m_WorldEntityList.begin();
	for( ; iter!=m_WorldEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = *iter;

		ioConveyer *pConveyer = ToConveyer( pEntity );
		if( pConveyer )
		{
			pConveyer->EnableWorldFunction( bEnable );
		}
		else
		{
			ioElevator *pElevator = ToElevator( pEntity );
			if( pElevator )
			{
				pElevator->EnableWorldFunction( bEnable );
			}
		}
	}

	EnableElevationSync( true );
}

void ioWorldEntityFactory::EnableElevationSync( bool bEnable )
{
	m_bElevationSyncStart = bEnable;
}

void ioWorldEntityFactory::OnElevatorChanged( SP2Packet &rkPacket )
{
	if( !m_bElevationSyncStart )
		return;

	DWORD dwCreateIndex = 0;
	rkPacket >> dwCreateIndex;

	PlayEntityList::iterator iter = m_WorldEntityList.begin();
	for( ; iter!=m_WorldEntityList.end() ; ++iter )
	{
		ioElevator *pElevator = ToElevator( *iter );
		if( pElevator && pElevator->GetCreateIndex() == dwCreateIndex )
		{
			pElevator->OnElevatorChanged( rkPacket );
			break;
		}
	}
}

void ioWorldEntityFactory::FillElevatorStateInfo( SP2Packet &rkPacket )
{
	PlayEntityList vElevatorList;

	PlayEntityList::iterator iter = m_WorldEntityList.begin();
	for( ; iter!=m_WorldEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = *iter;

		ioElevator *pElevator = ToElevator( pEntity );
		if( pElevator )
			vElevatorList.push_back( pEntity );
	}

	int iCnt = vElevatorList.size();
	rkPacket << iCnt;

	iter = vElevatorList.begin();
	while ( iter != vElevatorList.end() )
	{
		ioPlayEntity *pEntity = *iter;
		ioElevator *pElevator = ToElevator( pEntity );
		if( pElevator )
		{
			pElevator->FillCurElevatorInfo( rkPacket );
		}
		else
		{
			rkPacket << 0;
			rkPacket << 0;
			rkPacket << D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			rkPacket << 0;
			rkPacket << 0;
		}

		++iter;
	}
}

void ioWorldEntityFactory::ApplyElevatorStateInfo( SP2Packet &rkPacket )
{
	int iCnt;
	rkPacket >> iCnt;

	if( iCnt == 0 ) return;

	for( int i=0; i < iCnt; ++i )
	{
		DWORD dwIndex = 0;

		rkPacket >> dwIndex;

		bool bFind = false;
		PlayEntityList::iterator iter = m_WorldEntityList.begin();
		for( ; iter!=m_WorldEntityList.end() ; ++iter )
		{
			ioElevator *pElevator = ToElevator( *iter );
			if( pElevator && pElevator->GetCreateIndex() == dwIndex )
			{
				pElevator->ApplyCurElevatorInfo( rkPacket );
				bFind = true;
				break;
			}
		}

		if( !bFind )
		{
			int iState, iTime1, iTime2;
			D3DXVECTOR3 vPos;

			rkPacket >> iState;
			rkPacket >> vPos;
			rkPacket >> iTime1 >> iTime2;
		}
	}
}






