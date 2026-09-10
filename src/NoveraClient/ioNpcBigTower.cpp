#include "StdAfx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ionpcbigtower.h"
#include "ioAI.h"

ioNpcBigTower::ioNpcBigTower( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) 
						: ioNpcChar( pGrp, pCreator, pMode )
{

}

ioNpcBigTower::~ioNpcBigTower(void)
{

}

NpcType ioNpcBigTower::GetNpcType() const
{
	return NT_BIGTOWER;
}

int ioNpcBigTower::DontMoveEntityLevel() const
{
	return 6;
}



//////////////////////////////////////////////////////////////////////////
// 타워 가드다. 
//////////////////////////////////////////////////////////////////////////

ioNpcTwGuard::ioNpcTwGuard( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) 
: ioNpcBigTower( pGrp, pCreator, pMode )
{

}

ioNpcTwGuard::~ioNpcTwGuard(void)
{

}

NpcType ioNpcTwGuard::GetNpcType() const
{
	return NT_TOWERGUARD;
}

int ioNpcTwGuard::DontMoveEntityLevel() const
{
	return 6;
}


//////////////////////////////////////////////////////////////////////////
// 래어다. 
//////////////////////////////////////////////////////////////////////////

ioNpcLair::ioNpcLair( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) 
: ioNpcBigTower( pGrp, pCreator, pMode )
{

}

ioNpcLair::~ioNpcLair(void)
{

}

NpcType ioNpcLair::GetNpcType() const
{
	return NT_LAIR;
}

int ioNpcLair::DontMoveEntityLevel() const
{
	return 6;
}


//////////////////////////////////////////////////////////////////////////
// 래어가드다. 
//////////////////////////////////////////////////////////////////////////

ioNpcLairGuard::ioNpcLairGuard( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) 
: ioNpcBigTower( pGrp, pCreator, pMode )
{

}

ioNpcLairGuard::~ioNpcLairGuard(void)
{

}

NpcType ioNpcLairGuard::GetNpcType() const
{
	return NT_LAIRGUARD;
}

int ioNpcLairGuard::DontMoveEntityLevel() const
{
	return 6;
}


//////////////////////////////////////////////////////////////////////////
// 본진이다. 
//////////////////////////////////////////////////////////////////////////

ioNpcNexus::ioNpcNexus( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) 
: ioNpcBigTower( pGrp, pCreator, pMode )
{

}

ioNpcNexus::~ioNpcNexus(void)
{

}

NpcType ioNpcNexus::GetNpcType() const
{
	return NT_NEXUS;
}

int ioNpcNexus::DontMoveEntityLevel() const
{
	return 6;
}


//////////////////////////////////////////////////////////////////////////
// 본진가드다. 
//////////////////////////////////////////////////////////////////////////

ioNpcNexusGuard::ioNpcNexusGuard( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode ) 
: ioNpcBigTower( pGrp, pCreator, pMode )
{

}

ioNpcNexusGuard::~ioNpcNexusGuard(void)
{

}

NpcType ioNpcNexusGuard::GetNpcType() const
{
	return NT_NEXUSGUARD;
}

int ioNpcNexusGuard::DontMoveEntityLevel() const
{
	return 6;
}




















