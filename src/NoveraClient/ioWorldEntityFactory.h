#pragma once


class ioINILoader;
class ioGameEntity;
class ioPlayEntity;

class ioPlayStage;
class SP2Packet;

class ioWorldEntityFactory
{
protected:
	PlayEntityList m_WorldEntityList;
	ioPlayStage *m_pCreator;

	bool m_bElevationSyncStart;

public:
	ioPlayEntity* CreateWorldEntity( ioINILoader &rkLoader, int iCreateIndex );
	void RemoveWorldEntity( ioGameEntity *pEntity );
	bool IsWorldEntity( ioPlayEntity *pPlayEntity, int iCreateIndex );

protected:
	ioPlayEntity* CreateSubEntity( int iSubType, const char *szFileName, int iCreateIndex );
	bool IsKnownSubType( int iSubType ) const;

public:
	void EnableWorldEntityFunctions( bool bEnable );
	void EnableElevationSync( bool bEnable );

public:
	void OnElevatorChanged( SP2Packet &rkPacket );

public:
	void FillElevatorStateInfo( SP2Packet &rkPacket );
	void ApplyElevatorStateInfo( SP2Packet &rkPacket );

public:
	ioWorldEntityFactory( ioPlayStage *pCreator );
	virtual ~ioWorldEntityFactory();
};

