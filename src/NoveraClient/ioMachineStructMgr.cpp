

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMachineStructMgr.h"
#include "ioMachineStruct.h"
#include "ioMachineCannon.h"
#include "ioMachineCannon2.h"

#include "SP2Packet.h"
#include "NetworkWrappingFunc.h"

template<> ioMachineStructMgr* Singleton< ioMachineStructMgr >::ms_Singleton = 0;

ioMachineStructMgr::ioMachineStructMgr( ioPlayStage *pStage )
{
	m_iLocalIndex = 0;
	m_pPlayStage = pStage;
	m_vMachineStructList.clear();

	m_IniLoader.SetFileName_e( "config/sp2_machine_struct.ini" );
}

ioMachineStructMgr::~ioMachineStructMgr()
{
	DestroyMachineStructList();
}

ioMachineStructMgr& ioMachineStructMgr::GetSingleton()
{
	return Singleton< ioMachineStructMgr >::GetSingleton();
}

ioMachineStruct* ioMachineStructMgr::CreateMachineStruct( int iMachineNum, ioPlayMode *pMode )
{
	char szTitle[MAX_PATH], szKey[MAX_PATH];

	wsprintf_e( szTitle, "machine_struct%d", iMachineNum );
	m_IniLoader.SetTitle( szTitle );
	m_IniLoader.LoadString_e( "machine_struct", "", szKey, MAX_PATH );

	MachineStructType eType = (MachineStructType)m_IniLoader.LoadInt_e( "machine_type", MCT_NONE );

	ioHashString szStructName;
	szStructName = szKey;

	ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( szStructName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioMachineStructMgr::CreateMachineStruct - %s Fail!", szStructName.c_str() );
		return NULL;
	}

	ioMachineStruct* pMachine = NULL;
	switch( eType )
	{
	case MCT_CANNON:
		pMachine = new ioMachineCannon( pGrp, pMode );
		break;
	case MCT_CANNON2:
		pMachine = new ioMachineCannon2( pGrp, pMode );
		break;
	default:
		pMachine = new ioMachineStruct( pGrp, pMode );
		break;
	}

	m_iLocalIndex++;
	m_vMachineStructList.push_back( pMachine );

	return pMachine;
}

void ioMachineStructMgr::ClearDeadStructList()
{
	vMachineList::iterator iter = m_vMachineStructList.begin();

	while( iter != m_vMachineStructList.end() )
	{
		if( !(*iter)->IsLive() )
		{
			iter = m_vMachineStructList.erase( iter );
			continue;
		}

		++iter;
	}
}

void ioMachineStructMgr::GetMachineStructInfo( SP2Packet &rkPacket )
{
	int iMachineCnt = m_vMachineStructList.size();
	rkPacket << iMachineCnt;
	for( int i=0; i < iMachineCnt; i++ )
	{
		rkPacket << m_vMachineStructList[i]->GetMachineArrayIdx();
		rkPacket << m_vMachineStructList[i]->GetHP().m_fCurValue;
		rkPacket << m_vMachineStructList[i]->GetHP().m_fMaxValue;
		rkPacket << m_vMachineStructList[i]->GetWorldPosition();
		rkPacket << m_vMachineStructList[i]->GetWorldOrientation();
	}
}

void ioMachineStructMgr::ApplyMachineStructInfo( SP2Packet &rkPacket )
{
	int iMachineCnt;
	rkPacket >> iMachineCnt;

	for( int i=0; i < iMachineCnt; i++ )
	{
		int iArray;
		rkPacket >> iArray;

		float fCurHP, fMaxHP;
		D3DXVECTOR3 vPos;
		D3DXQUATERNION qtRot;
		rkPacket >> fCurHP >> fMaxHP >> vPos >> qtRot;

		ioMachineStruct *pMachine = FindMachineStruct( iArray );
		if( pMachine )
		{
			pMachine->SetHP( fCurHP, fMaxHP );
			pMachine->SetWorldPosition( vPos );
			pMachine->SetWorldOrientation( qtRot );
		}
	}
}

ioMachineStruct* ioMachineStructMgr::FindMachineStruct( int iArray )
{
	int iMachineCnt = m_vMachineStructList.size();
	for( int i=0; i < iMachineCnt; i++ )
	{
		if( m_vMachineStructList[i]->GetMachineArrayIdx() == iArray )
			return m_vMachineStructList[i];
	}

	return NULL;
}

ioMachineStruct* ioMachineStructMgr::GetMachineStruct( int iIndex )
{
	if( (int)m_vMachineStructList.size() > iIndex )
		return m_vMachineStructList[iIndex];

	return NULL;
}

void ioMachineStructMgr::DestroyMachineStructList()
{
	m_vMachineStructList.clear();
	m_pPlayStage->DestroyAllMachineStruct();
}

ioMachineStruct* ioMachineStructMgr::FindTakeEnableMachine( ioBaseChar *pChar )
{
	if( !pChar ) return NULL;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	ioMachineStruct *pMachine = NULL;
	float fCurDistance = std::numeric_limits<float>::infinity();
	for( int i=0; i < (int)m_vMachineStructList.size(); ++i )
	{
		ioMachineStruct *pCheck = m_vMachineStructList[i];
		if( !pCheck ) continue;

		if( !pCheck->IsTakeEnableState() )
			continue;

		D3DXVECTOR3 vDiff = vPos - pCheck->GetWorldPosition();
		float fGapDistance = D3DXVec3LengthSq( &vDiff );

		float fTakeRangeSq = pCheck->GetTakeEnableRange() * pCheck->GetTakeEnableRange();
		if( fGapDistance < fTakeRangeSq && fGapDistance < fCurDistance )
		{
			pMachine = pCheck;
			fCurDistance = fGapDistance;
		}
	}

	return pMachine;
}

ioPlayStage* ioMachineStructMgr::GetPlayStage()
{
	return m_pPlayStage;
}

