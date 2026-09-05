

#include "stdafx.h"

#include "ioPushStructListManager.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "GUI/FishingInventoryWnd.h"

template<> ioPushStructListManager* Singleton< ioPushStructListManager >::ms_Singleton = 0;

ioPushStructListManager::ioPushStructListManager( ioPlayStage *pStage )
{
	m_iLocalIndex = 0;
	m_pPlayStage = pStage;
	m_vPushStructList.clear();

	m_StructLoader.SetFileName_e( "config/sp2_push_struct.ini" );

	LoadBlockInfo();
}

ioPushStructListManager::~ioPushStructListManager()
{
	DestroyStructList();
}

ioPushStructListManager& ioPushStructListManager::GetSingleton()
{
	return Singleton< ioPushStructListManager >::GetSingleton();
}

void ioPushStructListManager::LoadBlockInfo()
{
	//ioINILoader kStructLoader( "config/sp2_push_struct.ini" );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	m_StructLoader.SetTitle_e( "block_info" );

	m_iBlockMaxCnt = m_StructLoader.LoadInt_e( "block_max_cnt", 0 );
	m_iBlockMyRoomMaxCnt = m_StructLoader.LoadInt_e( "block_myroom_max_cnt", 0 );
	m_iBlockEtcMaxCnt = m_StructLoader.LoadInt_e( "block_etcitem_max_cnt", 0 );
	m_dwBlockCreateDelayTime = m_StructLoader.LoadInt_e( "block_create_delay_time", 0 );

	m_StructLoader.LoadString_e( "block_create_loop_ani", "", szBuf, MAX_PATH );
	m_szBlockCreateLoopAni = szBuf;
	m_StructLoader.LoadString_e( "block_create_action_ani", "", szBuf, MAX_PATH );
	m_szBlockCreateActionAni = szBuf;
	
	m_fBlockCreateActionAniRate = m_StructLoader.LoadFloat_e( "block_create_action_ani_rate", FLOAT1 );
	m_fBlockGuidSpeed = m_StructLoader.LoadFloat_e("block_guid_speed", 0.0f );
}

ioPushStruct* ioPushStructListManager::CreatePushStruct( ioHashString szStructName, ioPlayMode *pMode, bool bTestCreate )
{
	DWORD dwStart = timeGetTime();

	ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( szStructName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioPushStructListManager::CreatePushStruct - %s Fail!", szStructName.c_str() );
		return NULL;
	}

	DWORD dwEnd = timeGetTime();

	if( Help::CheckEnableTimeGapUser() )
	{
		DWORD dwGap = dwEnd - dwStart;
		if( dwGap > 100 )
		{
			LOG.PrintTimeAndLog( 0, "FC - PushLoad: %s, %d", szStructName.c_str(), dwGap );
		}
	}

	ioPushStruct* pPush = new ioPushStruct( pGrp, pMode );
	if( !bTestCreate )
	{
		m_iLocalIndex++;
		m_vPushStructList.push_back( pPush );
	}
	else
		pGrp->SetVisible( false );

	return pPush;
}

void ioPushStructListManager::GetPushStructInfo( SP2Packet &rkPacket )
{
	int iPushCnt = m_vPushStructList.size();
	rkPacket << iPushCnt;
	for( int i=0 ; i<iPushCnt ; i++ )
	{
		rkPacket << m_vPushStructList[i]->GetPushArrayIdx();
		rkPacket << m_vPushStructList[i]->GetHP().m_fCurValue;
		rkPacket << m_vPushStructList[i]->GetHP().m_fMaxValue;
		rkPacket << m_vPushStructList[i]->GetWorldPosition();
	}
}

int ioPushStructListManager::FindUserPushStruct( ioBaseChar *pOwner )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0 ; i<iPushCnt ; i++ )
	{
		ioPushStruct *pPush = m_vPushStructList[i];

		if( pPush->IsCanPushed( pOwner ) )
			return pPush->GetPushArrayIdx();
	}

	return -1;
}

ioPushStruct* ioPushStructListManager::FindFishingGround( ioHashString szName, int iStructNum )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() == szName && m_vPushStructList[i]->GetStructNum() == iStructNum && 
			m_vPushStructList[i]->GetStructType() == PSTT_FISHING_GROUND )
			return m_vPushStructList[i];
	}

	return NULL;
}

ioPushStruct* ioPushStructListManager::FindFishingGroundByEtcCode( ioHashString szName, DWORD dwEtcCode )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() == szName && m_vPushStructList[i]->GetCreateEtcCode() == dwEtcCode && 
			m_vPushStructList[i]->GetStructType() == PSTT_FISHING_GROUND )
			return m_vPushStructList[i];
	}

	return NULL;
}

void ioPushStructListManager::DeleteAllFishingGround( ioBaseChar* pChar )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() == pChar->GetCharName() && m_vPushStructList[i]->GetStructType() == PSTT_FISHING_GROUND )
		{
			m_vPushStructList[i]->SetStructDie( pChar, true );
		}
	}
}

void ioPushStructListManager::DeleteSelectFishingGround( ioBaseChar* pChar, DWORD dwEtcItemCode )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() == pChar->GetCharName() && m_vPushStructList[i]->GetCreateEtcCode() == dwEtcItemCode
			&& m_vPushStructList[i]->GetStructType() == PSTT_FISHING_GROUND )
		{
			m_vPushStructList[i]->SetStructDie( pChar, true );
		}
	}
}

void ioPushStructListManager::ApplyPushStructInfo( SP2Packet &rkPacket )
{
	int iPushCnt;
	rkPacket >> iPushCnt;

	for( int i=0 ; i<iPushCnt ; i++ )
	{
		int iIndex;
		float fCurHP, fMaxHP;
		D3DXVECTOR3 vPos;
		rkPacket >> iIndex >> fCurHP >> fMaxHP >> vPos;

		int iListSize = m_vPushStructList.size();
		for( int j=0; j<iListSize; j++ )
		{
			ioPushStruct *pPush = m_vPushStructList[j];
			if( pPush->GetPushArrayIdx() == iIndex )
			{
				pPush->SetHP( fCurHP, fMaxHP );

				pPush->ClearMountEntity();
				pPush->SetWorldPosition( vPos );
				break;
			}
		}
	}
}

void ioPushStructListManager::ClearDeadStructList()
{
	PushList::iterator iter = m_vPushStructList.begin();
	while( iter != m_vPushStructList.end() )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( pPush && pPush->GetState() == ioPushStruct::PS_DIE )
		{
			iter = m_vPushStructList.erase( iter );
		}
		else
			++iter;
	}
}

void ioPushStructListManager::ClearStructListByOwner( ioBaseChar *pChar )
{
	if( !pChar ) return;

	bool bExist = false;
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() == pChar->GetCharName() )
		{
			m_vPushStructList[i]->SetKillerChar( pChar->GetCharName() );
			m_vPushStructList[i]->SetStructDieByOwnerClear( pChar, true );

			bExist = true;
		}
	}

	if( bExist && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CTPK_PUSHSTRUCT_OWNER_CLEAR );
		kPacket << pChar->GetCharName();
		TCPNetwork::SendToServer( kPacket );

		// 낚시 가방 UI에서 
		FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
		if( pWnd )
			pWnd->InitCheckUsingFishingGround();
	}
}

ioPushStruct* ioPushStructListManager::CheckMaxStructCnt( ioBaseChar *pOwner, int iStructNum, int iMaxCnt )
{
	if( !pOwner->IsNeedProcess() ) return NULL;
	if( iMaxCnt <= 0 ) return NULL;

	//ioINILoader_e kLoader( "config/sp2_push_struct.ini" );

	char szTitle[MAX_PATH];
	wsprintf_e( szTitle, "push_struct%d", iStructNum );
	m_StructLoader.SetTitle( szTitle );

	PushStructType eType = (PushStructType)m_StructLoader.LoadInt_e( "struct_type", PSTT_NOMAL );
	//

	PushList vOwnerStructList;

	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( (m_vPushStructList[i]->GetOwnerName() == pOwner->GetCharName()) &&
			(m_vPushStructList[i]->GetStructType() == eType ) )
		{
			vOwnerStructList.push_back( m_vPushStructList[i] );
		}
	}

	if( vOwnerStructList.empty() || (int)vOwnerStructList.size() < iMaxCnt )
		return NULL;

	ioPushStruct *pDelete = vOwnerStructList.front();

	int iListCnt = vOwnerStructList.size();
	for( i=1; i < iListCnt; i++ )
	{
		if( pDelete->GetCreateTime() > vOwnerStructList[i]->GetCreateTime() )
		{
			pDelete = vOwnerStructList[i];
		}
	}

	if( pDelete )
	{
		pDelete->SetKillerChar( pOwner->GetCharName() );
		return pDelete;
	}

	return NULL;
}

ioPushStruct* ioPushStructListManager::CheckMaxStructCntByNum( ioBaseChar *pOwner, int iStructNum, int iMaxCnt )
{
	if( !pOwner->IsNeedProcess() ) return NULL;
	if( iMaxCnt <= 0 ) return NULL;

	PushList vOwnerStructList;
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( (m_vPushStructList[i]->GetOwnerName() == pOwner->GetCharName()) &&
			(m_vPushStructList[i]->GetStructNum() == iStructNum ) )
		{
			vOwnerStructList.push_back( m_vPushStructList[i] );
		}
	}

	if( vOwnerStructList.empty() || (int)vOwnerStructList.size() < iMaxCnt )
		return NULL;

	ioPushStruct *pDelete = vOwnerStructList.front();

	int iListCnt = vOwnerStructList.size();
	for( i=1; i < iListCnt; i++ )
	{
		if( pDelete->GetCreateTime() > vOwnerStructList[i]->GetCreateTime() )
		{
			pDelete = vOwnerStructList[i];
		}
	}

	if( pDelete )
	{
		pDelete->SetKillerChar( pOwner->GetCharName() );
		return pDelete;
	}

	return NULL;
}

ioPushStruct* ioPushStructListManager::FindPushStruct( int iArray )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetPushArrayIdx() == iArray )
			return m_vPushStructList[i];
	}

	return NULL;
}

ioPushStruct* ioPushStructListManager::GetPushStruct( int iIndex )
{
	if( (int)m_vPushStructList.size() > iIndex )
		return m_vPushStructList[iIndex];

	return NULL;
}

void ioPushStructListManager::DestroyStructList()
{
	m_vPushStructList.clear();
	m_pPlayStage->DestroyAllPushStruct();
}

int ioPushStructListManager::GetCurPushStructCntByType( PushStructType eType )
{
	int iCurCnt = 0;
	PushList::iterator iter = m_vPushStructList.begin();
	while( iter != m_vPushStructList.end() )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( pPush && pPush->GetStructType() == eType )
			++iCurCnt;

		++iter;
	}

	return iCurCnt;
}

int ioPushStructListManager::GetCurPushStructCntByEtcCode( ioBaseChar *pChar, DWORD dwEtcCode )
{
	if( !pChar ) return 0;

	ioHashString szOwnerName = pChar->GetCharName();
	if( szOwnerName.IsEmpty() )
		return 0;

	int iCurCnt = 0;

	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() != szOwnerName )
			continue;

		if( m_vPushStructList[i]->GetCreateEtcCode() == dwEtcCode )
			++iCurCnt;
	}

	return iCurCnt;
}

bool ioPushStructListManager::CheckMaxStructCntForRoom()
{
	int iBlockCnt = GetCurPushStructCntByType( PSTT_BLOCK );

	if( iBlockCnt < m_iBlockMaxCnt )
		return true;
	
	return false;
}

bool ioPushStructListManager::CheckMaxStructCntForMyRoom()
{
	int iBlockCnt = GetCurPushStructCntByType( PSTT_BLOCK );

	if( iBlockCnt < m_iBlockMyRoomMaxCnt )
		return true;

	return false;
}

const ioHashString& ioPushStructListManager::GetBlockCreateLoopAni()
{
	return m_szBlockCreateLoopAni;
}

const ioHashString& ioPushStructListManager::GetBlockCreateActionAni()
{
	return m_szBlockCreateActionAni;
}

float ioPushStructListManager::GetBlockCreateActionAniRate()
{
	return m_fBlockCreateActionAniRate;
}

float ioPushStructListManager::GetBlockGuidSpeed()
{
	return m_fBlockGuidSpeed;
}

DWORD ioPushStructListManager::GetBlockCreateDelayTime()
{
	return m_dwBlockCreateDelayTime;
}

bool ioPushStructListManager::CheckMaxStructCntForEtcCode( ioBaseChar *pChar, DWORD dwEtcCode )
{
	if( !pChar ) return false;

	ioHashString szOwnerName = pChar->GetCharName();
	if( szOwnerName.IsEmpty() )
		return false;

	int iCurCnt = 0;

	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() != szOwnerName )
			continue;

		if( m_vPushStructList[i]->GetCreateEtcCode() == dwEtcCode )
			++iCurCnt;
	}

	if( iCurCnt < m_iBlockEtcMaxCnt )
		return true;

	return false;
}

void ioPushStructListManager::ApplyStructListByOwner( const ioHashString &szName )
{
	if( szName.IsEmpty() ) return;
	m_pPlayStage->GetOwnerChar();

	int iPushCnt = m_vPushStructList.size();
	for( int i=0; i<iPushCnt; i++ )
	{
		if( m_vPushStructList[i]->GetOwnerName() == szName )
		{
			bool bUsing = false;
			if( m_vPushStructList[i]->GetStructType() == PSTT_FISHING_GROUND )
			{
				// 낚시터 삭제 시 내가 낚시하던 곳이었으면 낚시 상태 해제
				ioBaseChar* pMyChar = m_pPlayStage->GetOwnerChar();
				if( pMyChar && pMyChar->GetState() == CS_FISHING && pMyChar->GetUseFishGroundOwner() == szName )
				{
					pMyChar->InitUseFishGroundOwner();
					pMyChar->SetFishingEndState();
					bUsing = true;
				}
			}
			m_vPushStructList[i]->StructDie( true, false, bUsing );
		}
	}
}

bool ioPushStructListManager::GeDisablePosition( D3DXVECTOR3 vPos, float fRadius )
{
	int iPushCnt = m_vPushStructList.size();
	for( int i=0 ; i<iPushCnt ; i++ )
	{
		ioPushStruct *pPush = m_vPushStructList[i];
		if( !pPush )
			continue;

		D3DXVECTOR3 vPoint;
		if( pPush->CheckSpherePoint( vPos, fRadius, vPoint ) )		
			return true;
	}

	return false;
}

