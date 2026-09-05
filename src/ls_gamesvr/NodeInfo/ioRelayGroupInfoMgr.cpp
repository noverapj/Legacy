#include "stdafx.h"
#include "ioRelayGroupInfoMgr.h"
#include "ioRelayRoomInfoMgr.h"
#include "SchedulerNode.h"
#ifdef ANTIHACK
#include "../Network/ioSkillInfoMgr.h"
#endif

bool bDetailLog = false;

#ifndef ANTIHACK
ioRelayGroupInfoMgr::ioRelayGroupInfoMgr(void)
{
}

ioRelayGroupInfoMgr::~ioRelayGroupInfoMgr(void)
{
}

void ioRelayGroupInfoMgr::InitData(int iSeedRoom)
{
	for(int i=0 ; i < iSeedRoom ; i++)
	{
		RelayGroup* pRelayGroup = new RelayGroup;
		if(pRelayGroup)
		{
			pRelayGroup->m_dwRoomIndex = -1;
			pRelayGroup->m_RelayUserList.reserve(MAX_PLAYER);
			m_vRelayGroupPool.Push(pRelayGroup);
		}
	}
}

void ioRelayGroupInfoMgr::InsertRoom( RelayHeader* pRelayHeader )
{
	InsertData* pData = reinterpret_cast<InsertData*>(pRelayHeader);
	InsertData &rkData = *pData;

	{
		//LOG.PrintTimeAndLog(0,"ioRelayGroupInfoMgr::InsertRoom Start: %d", GetCurrentThreadId());
		ThreadSync ts(this);
		if(!InsertUser(rkData.m_dwRoomIndex, rkData.m_dwUserIndex, rkData.m_iClientPort, rkData.m_szPublicIP))
		{
			// 생성
			LOG.PrintTimeAndLog( 0, "# %s - 생성 - %d - %d", __FUNCTION__, rkData.m_dwRoomIndex, rkData.m_dwUserIndex );
			CreateRelayGroup(rkData.m_dwRoomIndex, rkData.m_dwUserIndex, rkData.m_iClientPort, rkData.m_szPublicIP);
		}
	}

	//LOG.PrintTimeAndLog(0,"ioRelayGroupInfoMgr::InsertRoom End: %d", GetCurrentThreadId());
	g_RelayRoomInfoMgr->InsertRoomInfo(rkData.m_dwUserIndex, rkData.m_dwRoomIndex);
}

void ioRelayGroupInfoMgr::RemoveRoom( RelayHeader* pRelayHeader )
{
	RemoveData* pData = reinterpret_cast<RemoveData*>(pRelayHeader);
	RemoveData &rkData = *pData;

	if( rkData.m_dwUserIndex == 0 )
	{
		// 룸 통째로 삭제
		//LOG.PrintTimeAndLog(0,"TEST RemoveRoom : %d %d", rkData.m_dwUserIndex, rkData.m_dwRoomIndex);//유영재
		RelayGroup *pRelayGroup = RemoveRelayGroupByRoom( rkData.m_dwRoomIndex );
		if(pRelayGroup)
		{
			LOG.PrintTimeAndLog( 0, "# %s - 통째로 삭제 - %d - %d", __FUNCTION__, rkData.m_dwRoomIndex, rkData.m_dwUserIndex );

			for(DWORD i=0; i<pRelayGroup->m_RelayUserList.size(); i++)
			{
				g_RelayRoomInfoMgr->RemoveRoomInfo(pRelayGroup->m_RelayUserList[i].m_dwUserIndex);
			}

			pRelayGroup->m_dwRoomIndex = 0;
			pRelayGroup->m_RelayUserList.clear();

			m_vRelayGroupPool.Push(pRelayGroup);
			//LOG.PrintTimeAndLog(0,"TEST RemoveRoom m_vRelayGroupPool push");//유영재
		}
	}
	else
	{
		// 유저만 삭제
		//LOG.PrintTimeAndLog(0,"TEST RemoveUser : %d %d", rkData.m_dwUserIndex, rkData.m_dwRoomIndex);
		RemoveUser(rkData.m_dwRoomIndex, rkData.m_dwUserIndex);

		g_RelayRoomInfoMgr->RemoveRoomInfo(rkData.m_dwUserIndex);
		LOG.PrintTimeAndLog( 0, "# %s - 유저만 삭제 - %d - %d", __FUNCTION__, rkData.m_dwRoomIndex, rkData.m_dwUserIndex );
	}
}

BOOL ioRelayGroupInfoMgr::SendRelayPacket( DWORD dwUserIndex, SP2Packet& kPacket )
{
	RelayGroup::RelayGroups vGroupUsers;
	if(!GetRelayUserList( dwUserIndex, vGroupUsers ))
		return FALSE;

	for(unsigned int i=0; i< vGroupUsers.size(); i++)
	{
		UserData& rkUser = vGroupUsers.at(i);
		if(rkUser.m_dwUserIndex != dwUserIndex)
		{
			g_UDPNode.SendMessage(rkUser.m_szPublicIP, rkUser.m_iClientPort, kPacket);
		}
	}
	return TRUE;
}

BOOL ioRelayGroupInfoMgr::CreateRelayGroup(DWORD dwRoomIndex, DWORD dwUserIndex, int iPort, char* szIP)
{
	RelayGroup* pRelayGroup = m_vRelayGroupPool.Pop();
	if( pRelayGroup )
	{
		pRelayGroup->m_dwRoomIndex = dwRoomIndex;
		pRelayGroup->AddUser(dwUserIndex, iPort, szIP);

		m_vRelayGroups.SetAt( dwRoomIndex, pRelayGroup );
		//LOG.PrintTimeAndLog( 0, "Test - RelayGroup Create : %d, %d", (int)dwRoomIndex, (int)dwUserIndex );
		return TRUE;
	}

	LOG.PrintTimeAndLog(0, "CreateRelayGroup failed");
	return FALSE;
}

RelayGroup* ioRelayGroupInfoMgr::RemoveRelayGroupByRoom( const DWORD dwRoomIndex )
{
	ThreadSync ts(this);

	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		m_vRelayGroups.RemoveKey( dwRoomIndex );

		return pRelayGroup;
	}
	return NULL;
}

RelayGroup* ioRelayGroupInfoMgr::GetRelayGroupByRoom( const DWORD dwRoomIndex )
{
	PGROUPRESULT prData = m_vRelayGroups.Lookup(dwRoomIndex);
	if(prData)
	{
		return prData->m_value;
	}
	return NULL;
}

BOOL ioRelayGroupInfoMgr::InsertUser( const DWORD dwRoomIndex, const DWORD dwUserIndex, const int iPort, const char* szIP )
{

	//LOG.PrintTimeAndLog( 0, "Test - RelayGroup InsertUser : %d, %d", (int)dwRoomIndex, (int)dwUserIndex );
	RelayGroup *pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if( pRelayGroup )
	{
		LOG.PrintTimeAndLog( 0, "# %s - 추가 - %d - %d", __FUNCTION__, dwRoomIndex, dwUserIndex );
	//	LOG.PrintTimeAndLog( 0, "Test - RelayGroup InsertUser : AddUser OK : %d", (int)dwUserIndex );
		pRelayGroup->AddUser(dwUserIndex, iPort, szIP);
		return TRUE;
	}
	return FALSE;
}

BOOL ioRelayGroupInfoMgr::RemoveUser( const DWORD dwRoomIndex, const DWORD dwUserIndex )
{
	ThreadSync ts(this);

	RelayGroup *pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		pRelayGroup->RemoveUser(dwUserIndex);
		return TRUE;
	}
	return FALSE;
}

BOOL ioRelayGroupInfoMgr::GetRelayUserList(const DWORD dwUserIndex, RelayGroup::RelayGroups& vGroupUsers)
{
	DWORD dwRoomIndex = g_RelayRoomInfoMgr->GetRoomIndexByUser(dwUserIndex);
	if(dwRoomIndex != 0)
	{
		ThreadSync ts(this);

		RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
		if(pRelayGroup) 
		{
			pRelayGroup->GetUserLists(vGroupUsers);
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
#else


#include "../Network/ioSkillInfoMgr.h"
extern CLog CheatLOG;

//테스트용
bool bDetailSkillLog = false;

ioRelayGroupInfoMgr::ioRelayGroupInfoMgr(void)
{
	m_bRoomLog = m_bIsWrite = false;
	m_dwAntiWaitTime = 1000;
	m_fAntiErrorRate = 0.8f;
	m_dwPenguinCount = 8;
	m_dwKickCount = 50;
	m_iTimeDecrease = 30;

	m_dwSkillPenguinCount = 5;
	m_dwSkillKickCount = 15;
	m_iSkillTimeDecrease = 30000;
}

ioRelayGroupInfoMgr::~ioRelayGroupInfoMgr(void)
{
}

void ioRelayGroupInfoMgr::InitData(int iSeedRoom, DWORD dwAntiWaitTime, float fAntiErrorRate, int iPenguinCount, int iKickCount, int iTimeDecrease, int iSkillHackCount, int iSkillKickCount, int iSkillTimeDrease, std::vector<int>& vecExceptList )
{
	m_dwAntiWaitTime = dwAntiWaitTime;
	m_fAntiErrorRate = fAntiErrorRate;

	m_dwPenguinCount = iPenguinCount;
	m_dwKickCount = iKickCount;
	m_iTimeDecrease = iTimeDecrease;

	m_dwSkillPenguinCount = iSkillHackCount;
	m_dwSkillKickCount = iSkillKickCount;
	m_iSkillTimeDecrease = iSkillTimeDrease;

	m_vecExceptID = vecExceptList;

	for(int i=0 ; i < iSeedRoom ; i++)
	{
		RelayGroup* pRelayGroup = new RelayGroup;
		if(pRelayGroup)
		{
			pRelayGroup->m_dwRoomIndex = -1;
			pRelayGroup->m_bWriteLog = false;
			pRelayGroup->m_RelayUserList.reserve(MAX_PLAYER);
			m_vRelayGroupPool.Push(pRelayGroup);
		}
	}
}

void ioRelayGroupInfoMgr::InsertRoom( RelayHeader* pRelayHeader )
{
	InsertData* pData = reinterpret_cast<InsertData*>(pRelayHeader);
	InsertData &rkData = *pData;

	if(!InsertUser(rkData))
	{
		// 생성
		CreateRelayGroup(rkData);
	}

	g_RelayRoomInfoMgr->InsertRoomInfo(rkData.m_dwUserIndex, rkData.m_dwRoomIndex);
}

void ioRelayGroupInfoMgr::RemoveRoom( RelayHeader* pRelayHeader )
{
	RemoveData* pData = reinterpret_cast<RemoveData*>(pRelayHeader);
	RemoveData &rkData = *pData;

	if( rkData.m_dwUserIndex == 0 )
	{
		// 룸 통째로 삭제
		RelayGroup *pRelayGroup = RemoveRelayGroupByRoom( rkData.m_dwRoomIndex );
		if(pRelayGroup)
		{
			for(DWORD i=0; i<pRelayGroup->m_RelayUserList.size(); i++)
			{
				g_RelayRoomInfoMgr->RemoveRoomInfo(pRelayGroup->m_RelayUserList[i].m_dwUserIndex);
			}

			pRelayGroup->InitData();

			m_vRelayGroupPool.Push(pRelayGroup);
		}
	}
	else
	{
		// 유저만 삭제
		RemoveUser(rkData.m_dwRoomIndex, rkData.m_dwUserIndex);

		g_RelayRoomInfoMgr->RemoveRoomInfo(rkData.m_dwUserIndex);
	}
}

BOOL ioRelayGroupInfoMgr::CreateRelayGroup( InsertData& rkData )
{
	RelayGroup* pRelayGroup = m_vRelayGroupPool.Pop();
	if( pRelayGroup )
	{
		if( IsRoomLog() )
		{
			if( IsWriteLog() == false )
			{
				SetWriteLog( true );
				pRelayGroup->SetWriteLog( true );
			}
			else
				pRelayGroup->SetWriteLog( false );
		}
		else
			pRelayGroup->SetWriteLog( false );

		pRelayGroup->m_dwRoomIndex = rkData.m_dwRoomIndex;
		pRelayGroup->m_iCoolType = rkData.m_iCoolType;
		pRelayGroup->m_iModeType = rkData.m_iModeType;
		pRelayGroup->m_iRoomStyle = rkData.m_iRoomStyle;
		pRelayGroup->AddUser(rkData.m_dwUserIndex, rkData.m_iClientPort, rkData.m_szPublicIP, rkData.m_dwUserSeed, rkData.m_dwNPCSeed, rkData.m_iTeamType );

		pRelayGroup->InitMode();

		m_vRelayGroups.SetAt( rkData.m_dwRoomIndex, pRelayGroup );

		//LOG.PrintTimeAndLog( 0, "Test - RelayGroup Create : %d, %d", (int)dwRoomIndex, (int)dwUserIndex );
		return TRUE;
	}

	LOG.PrintTimeAndLog(0, "[relay] CreateRelayGroup failed");
	return FALSE;
}

RelayGroup* ioRelayGroupInfoMgr::RemoveRelayGroupByRoom( const DWORD dwRoomIndex )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		if( IsRoomLog() )
		{
			if( IsWriteLog() )
			{
				if( pRelayGroup->IsWriteLog() )
				{
					pRelayGroup->SetWriteLog( false );
					SetWriteLog( false );
				}
			}
		}


		m_vRelayGroups.RemoveKey( dwRoomIndex );

		return pRelayGroup;
	}
	return NULL;
}

RelayGroup* ioRelayGroupInfoMgr::GetRelayGroupByRoom( const DWORD dwRoomIndex )
{
	PGROUPRESULT prData = m_vRelayGroups.Lookup(dwRoomIndex);
	if(prData)
	{
		return prData->m_value;
	}
	return NULL;
}

BOOL ioRelayGroupInfoMgr::InsertUser( InsertData& rkData )
{
	RelayGroup *pRelayGroup = GetRelayGroupByRoom( rkData.m_dwRoomIndex );
	if( pRelayGroup )
	{
		if( pRelayGroup->m_iCoolType != rkData.m_iCoolType ||
			pRelayGroup->m_iModeType != rkData.m_iModeType || 
			pRelayGroup->m_iRoomStyle != rkData.m_iRoomStyle )
			LOG.PrintTimeAndLog( 0, "[relay] TEST - InsertUser CoolTypeCheck Error roomIndex(%u), uIdx(%u), CoolType(%d/%d),ModeType(%d/%d),RoomStyle(%d/%d)",
			pRelayGroup->m_dwRoomIndex, rkData.m_dwRoomIndex, rkData.m_dwUserIndex, pRelayGroup->m_iCoolType, rkData.m_iCoolType, pRelayGroup->m_iModeType, rkData.m_iModeType, pRelayGroup->m_iRoomStyle, rkData.m_iRoomStyle );

		pRelayGroup->AddUser(rkData.m_dwUserIndex, rkData.m_iClientPort, rkData.m_szPublicIP, rkData.m_dwUserSeed, rkData.m_dwNPCSeed, rkData.m_iTeamType );
		return TRUE;
	}
	return FALSE;
}

BOOL ioRelayGroupInfoMgr::RemoveUser( const DWORD dwRoomIndex, const DWORD dwUserIndex )
{
	RelayGroup *pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		UserData* pUserData = pRelayGroup->GetUserData(dwUserIndex);
		if( pUserData )
		{
			DWORD dwTotalCnt = pUserData->m_dwUserSeed - pUserData->m_dwUserSeedOri;
			dwTotalCnt += pUserData->m_dwNPCSeed - pUserData->m_dwNPCSeedOri;
			if( dwTotalCnt != 0 )
			{
				CheatLOG.PrintTimeAndLog( 0, "[RUDP][INFO] RudpInfo uid:%u, RoomIndex:%u, modeType:%d, total:%u, req:%u, ack:%u, dup:%u, err:%u, loss:%u, del:%u",
					dwUserIndex, dwRoomIndex, (int)pRelayGroup->m_iModeType, dwTotalCnt,
					pUserData->m_dwReqCount, pUserData->m_dwAckCount, pUserData->m_dwAckCountDupl, pUserData->m_dwErrCount, pUserData->m_dwLossCount, pUserData->m_dwEraseCount );

				CheatLOG.PrintTimeAndLog( 0, "[ANTI][INFO] AntiInfo - uid:%u, RoomIndex:%u, modeType:%d, gamePlayValue(%u), Hit Total(%u), Hit Penalty(%u), Skill Total(%u), Skill Penalty(%u)",
					dwUserIndex, dwRoomIndex, (int)pRelayGroup->m_iModeType, dwTotalCnt, pUserData->m_dwAntiHitCount, pUserData->m_dwAntiHitCount2, pUserData->m_dwAntiSkillCount, pUserData->m_dwAntiSkillCount2 );
			}			
		}

		if( IsRoomLog() )
		{
			if( IsWriteLog() )
			{
				if( pRelayGroup->IsWriteLog() )
				{
					pRelayGroup->SetWriteLog( false );
					SetWriteLog( false );
				}
			}
		}

		pRelayGroup->RemoveUser(dwUserIndex);

		if( pRelayGroup->GetUserCount() == 0 )
		{
			m_vRelayGroups.RemoveKey( dwRoomIndex );

			pRelayGroup->InitData();

			m_vRelayGroupPool.Push(pRelayGroup);
		}
		return TRUE;
	}
	return FALSE;
}

RelayGroup::RelayGroups* ioRelayGroupInfoMgr::GetRelayUserList( const DWORD dwUserIndex )
{
	DWORD dwRoomIndex = g_RelayRoomInfoMgr->GetRoomIndexByUser(dwUserIndex);
	if(dwRoomIndex != 0)
	{
		RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
		if(pRelayGroup) 
		{
			return pRelayGroup->GetUsers();
		}
	}

	return NULL;
}

BOOL ioRelayGroupInfoMgr::BroadcastPacket( DWORD dwUserIndex, SP2Packet& kPacket )
{
	RelayGroup::RelayGroups* pUsers = GetRelayUserList( dwUserIndex );

	if( pUsers == NULL )
		return FALSE;

	for(unsigned int i=0; i< pUsers->size(); i++)
	{
		kPacket.SetPosBegin();
		UserData& rkUser = pUsers->at(i);
		if(rkUser.m_dwUserIndex != dwUserIndex)
		{
			g_UDPNode.SendMessage(rkUser.m_szPublicIP, rkUser.m_iClientPort, kPacket);
		}
	}
	return TRUE;
}

bool ioRelayGroupInfoMgr::CheckControlSeed( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwSeed, DWORD dwHostTime, bool bUser )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( pRelayGroup )
	{
		UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
		if( pUserData )
		{
			DWORD& dwUpdateSeed = (bUser == true) ? pUserData->m_dwUserSeed : pUserData->m_dwNPCSeed;
			std::vector<DWORD>& dwVecID = (bUser == true) ? pUserData->m_vecRUDP : pUserData->m_vecRUDPNPC;
			std::vector<DWORD>& dwVecIDErase = bUser == true ? pUserData->m_vecRUDPErase : pUserData->m_vecRUDPEraseNPC;
			std::vector<DWORD>& dwVecIDTest = (bUser == true) ? pUserData->m_vecRUDPTest : pUserData->m_vecRUDPTestNPC;

			//min 체크는 rudp seed 올 수 있는 허용 갯수정도, max 체크는 실제 유실된 후 패킷이 올 예상량 정도
			DWORD dwSeedCheckValue = (bUser == true) ? MAX_PASS_SEED_VALUE_USER : MAX_PASS_SEED_VALUE_NPC;
			if( dwSeed < (dwUpdateSeed-dwSeedCheckValue) ||
				(dwUpdateSeed+(dwSeedCheckValue/2)) < dwSeed )
			{
				// 이상 시드 값
				//CheatLOG.PrintTimeAndLog( 0, "[RUDP][seed] user seed check err userIndex(%u), seed(%u,%u)", pUserData->m_dwUserIndex, dwUpdateSeed, dwSeed );
				dwUpdateSeed = dwSeed;
				return false;
			}

			bool bSend = false;
			if( dwUpdateSeed < dwSeed )
			{
				for( DWORD i = dwUpdateSeed + 1; i < dwSeed; ++i )
				{
					bSend = true;
					dwVecID.push_back( i );
					pUserData->m_dwReqCount++;
				}

				//시드 갱신
				if( dwUpdateSeed < dwSeed )
					dwUpdateSeed = dwSeed;

				//유저 아니면 pass
				if( bUser == false )
					return true;

				//사이즈가 너무 커지면..
				if( dwVecID.size() > MAX_SEED_CHECK_COUNT )
				{
					int vSize = dwVecID.size();
					int delSize = vSize - MAX_SEED_CHECK_COUNT;

					//지우기 전에 정보 넣고
					for( int i = 0; i < delSize; ++i )
						dwVecIDErase.push_back( dwVecID[i] );

					dwVecID.erase( dwVecID.begin(), dwVecID.begin() + delSize );
					pUserData->m_dwLossCount += delSize;
				}

				if( dwVecIDErase.size() > MAX_SEED_CHECK_COUNT )
				{
					int vSize = dwVecIDErase.size();
					int delSize = vSize - MAX_SEED_CHECK_COUNT;
					dwVecIDErase.erase( dwVecIDErase.begin(), dwVecIDErase.begin() + delSize );
				}

				//인덱스가 너무 많이 차이나도 삭제 해야함..
				DWORD dwSeedLiveCnt = bUser == true ? MAX_PASS_SEED_VALUE_USER : MAX_PASS_SEED_VALUE_NPC;
				if( !dwVecID.empty() )
				{
					//첫번째 들은값이 제일 오래된거인데, 이거 차이 나는 순간 싹 체크.
					if( (dwUpdateSeed-dwVecID[0]) > dwSeedLiveCnt )
					{
						auto it = dwVecID.begin();
						while( it != dwVecID.end() )
						{
							if( (dwUpdateSeed - *it) > dwSeedLiveCnt )
							{
								it = dwVecID.erase(it);
								pUserData->m_dwLossCount++;
							}
							else
								break;
						}

					}
				}

				//요청, 요번 seed에 빠진게 있을때만 보내는데.. NPC는 요청 안 하게 수정
				if( !dwVecID.empty() && bSend )
				{
					SP2Packet kPacket( SUPK_RUDP );
					//현재는 유저만..
					//PACKET_GUARD_BOOL( kPacket.W
					DWORD dwSize = dwVecID.size();
					PACKET_GUARD_BOOL( kPacket.Write( dwSize ));
					for( DWORD i = 0; i < dwSize; ++i )
						PACKET_GUARD_BOOL( kPacket.Write( dwVecID[i] ) );

					g_UDPNode.SendMessage( pUserData->m_szPublicIP, pUserData->m_iClientPort, kPacket);
				}
			}
			//역전일경우
			else
			{
				auto it = std::find( dwVecID.begin(), dwVecID.end(), dwSeed );
				if( it != dwVecID.end() )
				{
					pUserData->m_dwAckCount++;
					dwVecID.erase( it );
					dwVecIDTest.push_back( dwSeed );
					return true;
				}
				else
				{
					//재전송 패킷이 두번이상 올경우..
					auto it = std::find( dwVecIDTest.begin(), dwVecIDTest.end(), dwSeed );
					if( it != dwVecIDTest.end() )
					{
						pUserData->m_dwAckCountDupl++;
					}
					//지운 뒤에 올경우인데 체크용..
					it = std::find( dwVecIDErase.begin(), dwVecIDErase.end(), dwSeed );
					if( it != dwVecIDErase.end() )
					{
						pUserData->m_dwEraseCount++;
					}
					else
					{
						pUserData->m_dwErrCount++;
					}
					return false;
				}
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "[relay][seed] TEST - checkcontrolseed user index error - room(%u), user(%u)", dwRoomIndex, dwUserIndex );
			return false;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "[relay][seed] TEST - checkcontrolseed room index error - room(%u), user(%u)", dwRoomIndex, dwUserIndex );
		return false;
	}
	return true;
}

void ioRelayGroupInfoMgr::NotUseControlSeed( DWORD dwRoomIndex, DWORD dwUserIndex, SP2Packet& rkPacket )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if(pRelayGroup == NULL)
		return;
	UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
	if( !pUserData )
		return;

	//일단 유저만
	bool bUser = true;
	DWORD dwSize;
	rkPacket >> dwSize;
	MAX_GUARD(dwSize, 100);
	std::vector<DWORD>& dwVecID = (bUser == true) ? pUserData->m_vecRUDP : pUserData->m_vecRUDPNPC;
	for( DWORD i = 0; i < dwSize; ++i )
	{
		DWORD dwID;
		rkPacket >> dwID;

		auto it = std::find( dwVecID.begin(), dwVecID.end(), dwID );
		if( it != dwVecID.end() )
		{
			dwVecID.erase( it );
			pUserData->m_dwLossCount++;
		}
	}
}

float GetCoolTypebyRoomOption( int iCoolType )
{
	float fResult = 1.f;
	switch( iCoolType )
	{
	case 4 :
		fResult = 1.f;
		break;
	case 0 :
		fResult = 0.f;
		break;
	case 1 :
		fResult = 0.25f;
		break;
	case 2 :
		fResult = 0.5f;
		break;
	case 3 :
		fResult = 0.75f;
		break;
	case 5 :
		fResult = 1.25f;
		break;
	case 6 :
		fResult = 1.5f;
		break;
	case 7 :
		fResult = 2.f;
		break;
	case 8 :
		fResult = 3.f;
		break;
	case 9:
		fResult = 5.f;
		break;
	}
	return fResult;
}

void ioRelayGroupInfoMgr::OnSkillUse( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwHostTime, ioHashString& hsSkillName, int iLevel, float fGauge )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( !pRelayGroup )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pRelayGroup NULL" );
		return;
	}
	UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
	if( !pUserData )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pUserData NULL" );
		return;
	}
	ioSkillInfoMgr* pSkillMgr = g_SkillInfoMgr;
	if( !pSkillMgr )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pSkillMgr NULL" );
		return;
	}

	SkillInfo* pSkillInfo = pSkillMgr->GetSkillInfo( hsSkillName );
	if( !pSkillInfo )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pSkillInfo NULL, SkillName(%s)", hsSkillName.c_str() );
		return;
	}

	if( pSkillInfo->iEquipType < 0 || pSkillInfo->iEquipType > 3 )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - iEquipType error %d", pSkillInfo->iEquipType );
		return;
	}

	//except 리스트 검색!!
	if( IsExceptID(pSkillInfo->iSkillID) )
		return;

	if( bDetailSkillLog )
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - OnSkillUse iSlot(%d) room/user(%u/%u)",pSkillInfo->iEquipType, dwRoomIndex, dwUserIndex );

	if( IsCheckSkillTime( pUserData, pSkillInfo )  )	
		CheckSkillTime( pUserData, pRelayGroup, pSkillInfo, iLevel, dwHostTime );

	OnAfterSkill( pUserData, pSkillInfo, dwHostTime );
}

void ioRelayGroupInfoMgr::UpdateRelayGroupWin( DWORD dwRoomIndex, int iRedTeamWinCnt, int iBlueTeamWindCnt )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		pRelayGroup->m_iWinCntRed = iRedTeamWinCnt;
		pRelayGroup->m_iWinCntBlue = iBlueTeamWindCnt;
		pRelayGroup->CalculateTeamWinCnt();
	}
	else
		;//CheatLOG.PrintTimeAndLog( 0, "[relay] TEST - UpdateRelayGroupWin RoomIndex(%u) not exist", dwRoomIndex );
}

void ioRelayGroupInfoMgr::UpdateRelayGroupScore( DWORD dwRoomIndex, int iTeamType )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		// 죽은 유저팀이 넘어옴
		// RED = 1, BLUE= 2
		if( iTeamType == 1 )
			pRelayGroup->IncTeamScoreBlue();
		else if( iTeamType == 2 )
			pRelayGroup->IncTeamScoreRed();

		pRelayGroup->CalculateTeamScore();
	}
	else
		;//CheatLOG.PrintTimeAndLog( 0, "[relay] TEST - UpdateRelayGroupScore RoomIndex(%u) not exist", dwRoomIndex );
}

void ioRelayGroupInfoMgr::OnSkillAstRandomGenerate( UserData* pUser, SkillInfo* pSkillInfo, float fRate, float fUseTimeDiff )
{
	//기본적으로 체크는 사용하고 바로 취소하였을경우로 체크함
	// 블리자드는 시작과 동시에 취소 가능해서 최소 사용 시간만큼만인데..차지할 경우 1.5배 빨리지니 과하지만!
	fRate *= 1.5f;

	// 최저 시전시간을 200ms로
	float fLowestTime = 0.2f;
	float fUsingTime = pSkillInfo->fNeedGauge + fLowestTime + 1.f;	//사용시 바로 1.f 달음
	fUsingTime /= fRate;
	fUsingTime *= 1000.f;
	// 이 계산은 시작과 동시에 취소하고 바로 풀차지 할 경우 인데, 사실상 불가능함.. 좀더 엄격하게 해도 될듯?
	// fUsingTime 쓴만큼 채워야 함
	if( fUsingTime > fUseTimeDiff ) 
	{
		AddPenaltySkill( pUser, pSkillInfo->hsSkillName, fUseTimeDiff, fUsingTime );
// 		CheatLOG.PrintTimeAndLog( 0, "[skill][hack] Check - Uidx(%u), SkillName(%s), fCheckTiem(%f), fUseTimeDiff(%f)"	
// 			, pUser->m_dwUserIndex, hsSkillName.c_str(), fUsingTime, fUseTimeDiff );
	}
}

void ioRelayGroupInfoMgr::OnSkillBasic( UserData* pUser, ioHashString& hsSkillName, float fIncMaxCoolTime, float fUseTimeDiff, float fErrorRate )
{
	if( fIncMaxCoolTime > fUseTimeDiff )
	{
		// 20프로 정도의 오차면 허용
		if( fIncMaxCoolTime * fErrorRate > fIncMaxCoolTime - fUseTimeDiff )
		{
			/*// 허용하지만 로그..
			CheatLOG.PrintTimeAndLog( 0, "[skill][warn] Check - Uidx(%u), SkillName(%s), SkillUseTimeDiff(%f), fAllowTime(%f)"	
				, pUser->m_dwUserIndex, hsSkillName.c_str(), fUseTimeDiff, fIncMaxCoolTime * fErrorRate );*/
		}
		else
		{
			//치팅 의심
			AddPenaltySkill( pUser, hsSkillName, fUseTimeDiff, fIncMaxCoolTime );			
// 			CheatLOG.PrintTimeAndLog( 0, "[skill][hack] Check - Uidx(%u), SkillName(%s), SkillUseTimeDiff(%f), fAllowTime(%f)"	
// 				, pUser->m_dwUserIndex, hsSkillName.c_str(), fUseTimeDiff, fIncMaxCoolTime * fErrorRate );
		}
	}
}

void ioRelayGroupInfoMgr::OnSkillBuff( UserData* pUser, SkillInfo* pSkillInfo, float fIncMaxCoolTime, float fUseTimeDiff, float fErrorRate )
{	
	if( fIncMaxCoolTime > fUseTimeDiff )
	{
		// 스킬 사용 직후 게이지가 소모되는 형태들..
		// 사용하자마자 바로 취소되었다고 가정하고..

		float fRemainGauge = pSkillInfo->fMaxGauge - pSkillInfo->fNeedGauge;
		float fRemainIncTime = fIncMaxCoolTime - (fRemainGauge / pSkillInfo->fMaxGauge) * fIncMaxCoolTime;
		float fChecktime = fRemainIncTime;
		if( fChecktime > fUseTimeDiff )
		{
			//여기서부턴 일단 의심.
			fChecktime = fChecktime * (1.f - fErrorRate);
			float fDiffTime = fRemainIncTime - fUseTimeDiff;
			float fAllowTime = fIncMaxCoolTime * fErrorRate;
			if( fChecktime > fDiffTime )
			{
				/*// 일단 허용하는걸로
				CheatLOG.PrintTimeAndLog( 0, "[skill][warn] TEST - WARN %s, fChecktime(%f), fDiffTime(%f)", pSkillInfo->hsSkillName.c_str(), fChecktime, fDiffTime );*/
			}
			else
			{
				AddPenaltySkill( pUser, pSkillInfo->hsSkillName, fUseTimeDiff, fChecktime );
				// 치팅 유저 일 수 있으니 기록!
// 				CheatLOG.PrintTimeAndLog( 0, "[skill][hack] Check - Uidx(%u), SkillName(%s), fChecktime(%f), fDiffTime(%f)"
// 					, pUser->m_dwUserIndex, pSkillInfo->hsSkillName.c_str(), fChecktime, fDiffTime );
			}
		}
	}
}

void ioRelayGroupInfoMgr::OnSkillCount( UserData* pUser, SkillInfo* pSkillInfo, DWORD dwCurTime )
{
	// 정확한 체크가 불가능함. 그냥 심플하게 단위시간당 올 수 있는 갯수 + 연속으로 올 수 없는 시간 2개를 체크함.
	std::vector<DWORD>& vecTimer = pUser->m_dwVecMultiCountSkill[pSkillInfo->iEquipType];
	vecTimer.push_back( dwCurTime );

	//일정 시간 이상된건 삭제
	int nMaxCount = pSkillInfo->iMaxCount / pSkillInfo->iNeedCount;
	float fNeedTime = pSkillInfo->fNeedGauge * pSkillInfo->iTickTime;
	float fNotCheckTime = fNeedTime * nMaxCount;

	auto it = vecTimer.begin();
	while( it != vecTimer.end() )
	{
		if( (dwCurTime - *it) > fNotCheckTime )
		{
			it = vecTimer.erase( it );
		}
		else
			break;
	}

	//연속으로 올 수 있는 갯수			
	int nCurSize = vecTimer.size();
	if( nCurSize <= nMaxCount )
	{
		//일단 처음 3개는 무조건 받게
	}
	else
	{
		// 2가지 케이스 검사.
		// 1. 최대 휴식후에 3번 사용이후 바로 바로 사용할 경우 3-4 구간에서 최소 모으는 시간이 가능해야함
		// 2. 1번과 유사하지만 걸린 시간대비 사용가능횟수 체크..조금 부정확할지도?

		const float fLowestAnitime = 150.f;
		float fLastedTime = dwCurTime - vecTimer[0];	
		float fCheckTime = (nCurSize-nMaxCount) * fNeedTime;
		if( fLastedTime < fCheckTime )
		{
			AddPenaltySkill( pUser, pSkillInfo->hsSkillName, fLastedTime * 0.001f, fCheckTime * 0.001f );
			/*CheatLOG.PrintTimeAndLog( 0, "[relay] hack Check Count Skill - Uidx(%u), SkillName(%s), LastTime(%.2f), fCheckTime(%.2f)",
				pUser->m_dwUserIndex, pSkillInfo->hsSkillName.c_str(), fLastedTime, fCheckTime );*/
		}
	}
}

void ioRelayGroupInfoMgr::OnAntiHackHit( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwHitUserIndex, DWORD dwWeaponIndex, BYTE eAttackType )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( !pRelayGroup )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[anti] TEST - pRelayGroup NULL" );
		return;
	}
	UserData* pUserData = pRelayGroup->GetUserData( dwHitUserIndex );
	if( !pUserData )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[anti] TEST - pUserData NULL" );
		return;
	}

	DWORD dwTarget = dwNPCIndex != 0 ? dwNPCIndex : dwUserIndex;

	/*if( bDetailLog )
		CheatLOG.PrintTimeAndLog( 0, "Test OnAntiHackHit - send(%u) -  %u >> %u", dwUserIndex, dwTarget, dwHitUserIndex );*/

	pUserData->AddHitData( dwTarget, dwWeaponIndex/*, pRelayGroup->IsWriteLog()*/ );
}

void ioRelayGroupInfoMgr::OnAntiHackWounded( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwAttackerIndex, DWORD dwWeaponIndex, BYTE eAttackType )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( !pRelayGroup )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[anti] TEST - pRelayGroup NULL" );
		return;
	}
	UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
	if( !pUserData )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[anti] TEST - pUserData NULL" );
		return;
	}

	/*if( bDetailLog )
		CheatLOG.PrintTimeAndLog( 0, "Test - OnAntiHackWounded send(%u) -  %u << %u", dwUserIndex, dwUserIndex, dwAttackerIndex );*/

	pUserData->AddWoundedData( dwAttackerIndex, dwWeaponIndex/*, pRelayGroup->IsWriteLog()*/ );
}

void ioRelayGroupInfoMgr::OnAntiHackDefence( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwAttackerIndex, DWORD dwWeaponIndex, BYTE eAttackType )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( !pRelayGroup )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[anti] TEST - pRelayGroup NULL" );
		return;
	}
	UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
	if( !pUserData )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[anti] TEST - pUserData NULL" );
		return;
	}

	pUserData->AddWoundedData( dwAttackerIndex, dwWeaponIndex/*, pRelayGroup->IsWriteLog()*/ );
}

void ioRelayGroupInfoMgr::CheckAntiHackData()
{
	DWORD dwCurTime = TIMEGETTIME();

	POSITION pos = m_vRelayGroups.GetStartPosition();
	while(pos)
	{
		RelayGroup* pRelayGroup = m_vRelayGroups.GetValueAt(pos);
		if( pRelayGroup )
		{
			RelayGroup::RelayGroups* pRelayGroups = pRelayGroup->GetUsers();
			if( pRelayGroups )
			{
				for( DWORD i = 0; i < pRelayGroups->size(); ++i )
				{
					UserData& rUserData = pRelayGroups->at(i);

					int iHackCount = 0;
					int nSize = rUserData.m_vecAntiData.size();
					int nCheckSize = 0;

					auto it = rUserData.m_vecAntiData.begin();
					while( it != rUserData.m_vecAntiData.end() )
					{
						if( dwCurTime-it->dwLastTime >= m_dwAntiWaitTime )
						{
							++nCheckSize;

							if( it->dwHitCount > it->dwWDCount )
								iHackCount++;

							it = rUserData.m_vecAntiData.erase( it );
						}
						else
							break;
					}			

					if( nCheckSize > 0 && ((float)iHackCount/(float)nCheckSize >= m_fAntiErrorRate) )
					{
						int iPenaltyPoint = iHackCount * iHackCount / nCheckSize;
						if( iPenaltyPoint < 1 ) iPenaltyPoint = 1;
						//펭귄.
						AddPenaltyPoint( &rUserData, iPenaltyPoint );
					}
				}
			}
		}

		m_vRelayGroups.GetNext(pos);
	}
}

int ioRelayGroupInfoMgr::GetModeType( DWORD dwRoomIndex )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if( pRelayGroup )
		return pRelayGroup->GetModeType();

	return 0;
}

int ioRelayGroupInfoMgr::GetModeStyle( DWORD dwRoomIndex )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if( pRelayGroup )
		return pRelayGroup->GetModeStyle();

	return 0;
}

void ioRelayGroupInfoMgr::AddPenaltySkill( UserData* pUser, ioHashString& hsSkillName, float fTime1, float fTime2 )
{
	DWORD dwCurTime = TIMEGETTIME();
	pUser->m_dwPenaltySkill++;
	if( pUser->m_dwLastSkillPenaltyTime != 0 )
	{
		int iPenaltyDecreaseTime = 1000 * m_iSkillTimeDecrease;

		DWORD dwPastTime = dwCurTime - pUser->m_dwLastSkillPenaltyTime;
		DWORD dwCnt = dwPastTime / iPenaltyDecreaseTime;
		if( pUser->m_dwPenaltySkill < dwCnt )
		{
			// clean penalty
			pUser->m_dwPenaltySkill = 0;
		}
		else
		{
			pUser->m_dwPenaltySkill -= dwCnt;
		}
	}
	pUser->m_dwLastSkillPenaltyTime = dwCurTime;

	if( pUser->m_dwPenaltySkill < m_dwSkillKickCount )
	{
		if( pUser->m_dwPenaltySkill > m_dwSkillPenguinCount )
		{
			pUser->m_dwAntiSkillCount++;

			SP2Packet kPacket(SUPK_CLIENT_MSG);
			PACKET_GUARD_VOID( kPacket.Write((BYTE)1) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->m_dwPenaltySkill) );
			g_UDPNode.SendMessage( pUser->m_szPublicIP, pUser->m_iClientPort, kPacket );
		}
	}
	else
	{
		pUser->m_dwAntiSkillCount2 = 1;
		// Kick..
		SP2Packet iPacket( ITPK_UDP_ANTIHACK_PENALTY );
		PACKET_GUARD_VOID( iPacket.Write(pUser->m_dwUserIndex) );
		g_InternalNode->ReceivePacket( iPacket );
		LOG.PrintTimeAndLog( 0, "[anti][Kick] User Skill Penalty over - Uidx(%u), SkillName(%s)", pUser->m_dwUserIndex, hsSkillName.c_str() );
	}

	

	/*SP2Packet kPacket(SUPK_CLIENT_MSG);
	PACKET_GUARD_VOID( kPacket.Write((BYTE)0) );
	PACKET_GUARD_VOID( kPacket.Write(hsSkillName) );
	PACKET_GUARD_VOID( kPacket.Write(fTime1) );
	PACKET_GUARD_VOID( kPacket.Write(fTime2) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->m_dwPenaltySkill) );
	g_UDPNode.SendMessage( pUser->m_szPublicIP, pUser->m_iClientPort, kPacket );

	CheatLOG.PrintTimeAndLog( 0, "[skill][penalty] User Penalty - Uidx(%u), SkillName(%s), Penalty(%u)"	
		, pUser->m_dwUserIndex, hsSkillName.c_str(), pUser->m_dwPenaltySkill );*/
}

void ioRelayGroupInfoMgr::AddPenaltyPoint( UserData* pUser, int iPenaltyPoint )
{
	//pUser->m_dwAntiHitCount++;
#ifdef SRC_ID
	return;
#endif
	//페널티
	DWORD dwCurTime = TIMEGETTIME();
	pUser->m_dwPenaltyCount += iPenaltyPoint;
	if( pUser->m_dwLastPenaltyTime != 0 )
	{
		//특정 시간마다 페널티 감소를 해줌 일단 1분에 1씩 줄여줌
		int iPenaltyDecreaseTime = 1000 * m_iTimeDecrease;

		DWORD dwPastTime = dwCurTime - pUser->m_dwLastPenaltyTime;
		DWORD dwCnt = dwPastTime / iPenaltyDecreaseTime;
		if( pUser->m_dwPenaltyCount < dwCnt )
		{
			// clean penalty
			pUser->m_dwPenaltyCount = 0;
		}
		else
		{
			pUser->m_dwPenaltyCount -= dwCnt;
		}
	}
	pUser->m_dwLastPenaltyTime = dwCurTime;

	if( pUser->m_dwPenaltyCount < m_dwKickCount )
	{
		if( pUser->m_dwPenaltyCount > m_dwPenguinCount )
		{
			pUser->m_dwAntiHitCount++;

			SP2Packet kPacket(SUPK_CLIENT_MSG);
			PACKET_GUARD_VOID( kPacket.Write((BYTE)2) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->m_dwPenaltyCount) );
			g_UDPNode.SendMessage( pUser->m_szPublicIP, pUser->m_iClientPort, kPacket );

			
		}
	}
	else
	{
		pUser->m_dwAntiHitCount2 = 1;
		// Kick..
		SP2Packet iPacket( ITPK_UDP_ANTIHACK_PENALTY );
		PACKET_GUARD_VOID( iPacket.Write(pUser->m_dwUserIndex) );
		g_InternalNode->ReceivePacket( iPacket );
		//CheatLOG.PrintTimeAndLog( 0, "[anti][Kick] User Hit Penalty over - Uidx(%u)", pUser->m_dwUserIndex );
	}

	/*// 기본 페널티 점수를 유저에게 알려준다
	SP2Packet kPacket(SUPK_CLIENT_MSG);
	PACKET_GUARD_VOID( kPacket.Write((BYTE)2) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->m_dwPenaltyCount) );
	g_UDPNode.SendMessage( pUser->m_szPublicIP, pUser->m_iClientPort, kPacket );*/
}

DWORD ioRelayGroupInfoMgr::GetLastSkillTime( UserData* pUserData, SkillInfo* pSkillInfo )
{
	if( pSkillInfo->iSkillType == ST_COUNT )
		return 0;

	if( pSkillInfo->iSkillSubType == AST_RANDOM_GENERATE )
		return pUserData->m_dwSkillUseTime2[pSkillInfo->iEquipType];

	return pUserData->m_dwSkillUseTime[pSkillInfo->iEquipType];
}

bool ioRelayGroupInfoMgr::IsCheckSkillTime( UserData* pUserData, SkillInfo* pSkillInfo )
{
	DWORD dwLastSkillTime = GetLastSkillTime( pUserData, pSkillInfo );
	if( dwLastSkillTime == 0 )
		return false;

	return true;
}

void ioRelayGroupInfoMgr::CheckSkillTime( UserData* pUserData, RelayGroup* pRelayGroup, SkillInfo* pSkillInfo,  int iLevel, DWORD dwHostTime )
{
	ioSkillInfoMgr* pSkillMgr = g_SkillInfoMgr;
	if( !pSkillMgr )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pSkillMgr NULL" );
		return;
	}

	//육성치가 적용된 쿨타임
	float fMaxCoolTime = pSkillMgr->GetMaxSkillCoolTime( pSkillInfo->hsSkillName, iLevel );
	if( fMaxCoolTime == 0 )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - Skill Max CoolTime error. SkillName(%s)", pSkillInfo->hsSkillName.c_str() );
		return;
	}

	DWORD dwLastTime = 0;
	DWORD dwCurTime = 0;
	//지난 시간과 현재 시간의 차이로 계산..
	if( pSkillInfo->iSkillType != ST_COUNT )
	{
		dwLastTime = GetLastSkillTime( pUserData, pSkillInfo );
		dwCurTime = dwHostTime;
		if( dwLastTime > dwCurTime )
		{
			// 버그 상황이거나 프로토콜 조작 의심..
			//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - SkillTime Cheat Doubt Last/Cur(%u/%u)", dwLastTime, dwCurTime );
			//실제로 상당히 많이 나와서 여기서도 페널티 더해줌
			AddPenaltySkill( pUserData, pSkillInfo->hsSkillName, 0.f, 0.f );
			return;
		}
	}

	float fErrorRate = pSkillMgr->GetErrorRate();

	// 스킬 간격
	DWORD dwUseDiffTime = dwCurTime - dwLastTime;
	float fUseDifftime = dwUseDiffTime;// * 0.1f;

	// 각종 옵션이 반영된 계산값
	//여기서 방옵션, 모드, 기여도 등등의 종합 평가로 쿨타임 계산해줘야 함..
	float fRate = 1.f;

	//룸옵션
	fRate *= GetCoolTypebyRoomOption(pRelayGroup->m_iCoolType);
	// 인원수
	// 스코어
	// 모드별 점수 보정
	if( pUserData->m_iTeamType == TEAM_RED )
	{
		fRate *= pRelayGroup->GetTeamBalanceRed();
		fRate *= pRelayGroup->GetWinBalanceRed();
		fRate *= pRelayGroup->GetModeBalanceRed();
	}
	else if( pUserData->m_iTeamType == TEAM_BLUE )
	{
		fRate *= pRelayGroup->GetTeamBalanceBlue();
		fRate *= pRelayGroup->GetWinBalanceBlue();
		fRate *= pRelayGroup->GetModeBalanceBlue();
	}
	else
	{
		//개인전
		fRate *= pRelayGroup->GetModeBalanceRed();
	}
	//////////////////////////////////////////////////////////////////////////
	//미적용부분			
	//파이트 클럽일시 1~1.3f * 1~1.3f
	//소극적플레이 일시 0~0.5f?
	//////////////////////////////////////////////////////////////////////////

	// 맥스 쿨타임에 증가속도를 더해서 사용 가능한지
	float fIncMaxCoolTime = fMaxCoolTime / fRate;
	fIncMaxCoolTime *= 1000.f;
	switch( pSkillInfo->CheckSkillType() )
	{
	case SCT_BASIC:
		OnSkillBasic( pUserData, pSkillInfo->hsSkillName, fIncMaxCoolTime, fUseDifftime, fErrorRate );
		break;
	case SCT_NEED:
		OnSkillBuff( pUserData, pSkillInfo, fIncMaxCoolTime, fUseDifftime, fErrorRate );
		break;
	case SCT_COUNT:
		OnSkillCount( pUserData, pSkillInfo, dwCurTime );
		break;
	case SCT_AST:
		OnSkillAstRandomGenerate( pUserData, pSkillInfo, fRate, fUseDifftime );
		break;
	case SCT_RETURN:
		//무시
		break;
	}
}

void ioRelayGroupInfoMgr::OnAfterSkill( UserData* pUserData, SkillInfo* pSkillInfo, DWORD dwHostTime )
{
	// 시간 기록
	SetLastSkillTime( pUserData, pSkillInfo, dwHostTime );
	// 회복스킬인 경우..CUPK_ANTIHACK_SKILL 스킬로 추가 정보 입수
	if( pSkillInfo->bRecoverGauge )
	{
		Debug( "OnAfterSkill Start- SkillIdx(%d), Time(%u)", pSkillInfo->iSkillID, dwHostTime );
		pUserData->OnRecvoerSkill( dwHostTime );
	}
	else if( pSkillInfo->bReloadBullet )
	{
		//현재 유저가 장착한 장비를 알 수 있나?
		UserData::sBulletData* pBulletData = pUserData->GetBulletData( pSkillInfo->dwExtraValue );
		pBulletData->Clear();
	}
	//주사위 스킬은 예외코딩
}

void ioRelayGroupInfoMgr::SetLastSkillTime( UserData* pUserData, SkillInfo* pSkillInfo, DWORD dwHostTime )
{
	if( pSkillInfo->iSkillType == ST_COUNT )
		return;

	if( pSkillInfo->iSkillSubType == AST_RANDOM_GENERATE )
		pUserData->m_dwSkillUseTime2[pSkillInfo->iEquipType] = dwHostTime;
	else
		pUserData->m_dwSkillUseTime[pSkillInfo->iEquipType] = dwHostTime;
}

void ioRelayGroupInfoMgr::OnSkillAntihack( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwHostTime, int nSize, int* iSlots, DWORD* dwLatedTime )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( !pRelayGroup )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pRelayGroup NULL" );
		return;
	}
	UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
	if( !pUserData )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pUserData NULL" );
		return;
	}

	Debug( "OnSkillAntihack - uidx(%u), Time(%u)", dwUserIndex, dwHostTime );
	
	pUserData->OnRecoverSkillExtraInfo( dwHostTime, nSize, iSlots, dwLatedTime );
}

void ioRelayGroupInfoMgr::OnSkillExtraInfo( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwHostTime, int iSize, DWORD* dwUserIndexs )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom(dwRoomIndex);
	if( !pRelayGroup )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pRelayGroup NULL" );
		return;
	}
	UserData* pUserData = pRelayGroup->GetUserData( dwUserIndex );
	if( !pUserData )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] TEST - pUserData NULL" );
		return;
	}

	Debug( "OnSkillExtraInfo - uidx(%u), Time(%u)", dwUserIndex, dwHostTime );

	// 해당 유저에게 리커버리 가능하다고 신호를 줌
	for( int i = 0; i < iSize; ++i )
	{
		UserData* pTargetUser = pRelayGroup->GetUserData( dwUserIndexs[i] );
		if( pTargetUser )
		{
			pTargetUser->OnRecvoerSkill( dwHostTime );
		}
	}
}

void ioRelayGroupInfoMgr::ReloadAntiHack( float fAntiErrorRate, int iAntiWaitTime, int iPenguinCount, int iKickCount, int iTimeDecrease, int iSkillHackCount, int iSkillKickCount, int iSkillTimeDecrease, int* iArray )
{
	m_dwAntiWaitTime = iAntiWaitTime;
	m_fAntiErrorRate = fAntiErrorRate;

	m_dwPenguinCount = iPenguinCount;
	m_dwKickCount = iKickCount;
	m_iTimeDecrease = iTimeDecrease;

	m_dwSkillPenguinCount = iSkillHackCount;
	m_dwSkillKickCount = iSkillKickCount;
	m_iSkillTimeDecrease = iSkillTimeDecrease;

	int nSize = m_vecExceptID.size();

	for( int i = 0; i < 10; ++i )
	{
		if( iArray[i] == 0 )
			break;

		int iExceptid = iArray[i];

		for( int j = 0; j < nSize; ++j )
		{
			if( m_vecExceptID[j] == iExceptid )
				break;
		}

		m_vecExceptID.push_back( iExceptid );
	}
}

bool ioRelayGroupInfoMgr::IsExceptID( int iSkillid )
{
	int nSize = m_vecExceptID.size();
	for( int i = 0; i < nSize; ++i )
		if( m_vecExceptID[i] == iSkillid )
			return true;
	return false;
}

void ioRelayGroupInfoMgr::UpdateUserSPPotion( DWORD dwRoomIndex, DWORD dwUserIndex )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		DWORD dwCurTime = TIMEGETTIME();
		UserData* pUser = pRelayGroup->GetUserData( dwUserIndex );
		if( pUser )
			pUser->OnRecvoerSkill( dwCurTime );
	}
}

void ioRelayGroupInfoMgr::OnAnthHackBulletInfo( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwItemCode, DWORD dwWasteCount, DWORD dwWasteTime )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		UserData* pUser = pRelayGroup->GetUserData( dwUserIndex );
		if( pUser )
		{
			UserData::sBulletData* pBulletData = pUser->GetBulletData( dwItemCode );
			if( !pBulletData )
			{	
				sWeaponInfo* pWeapon = GetWeaponInfo( dwItemCode );
				if( !pWeapon )
					return;
				pBulletData = pUser->InsertBulletData( dwItemCode, pWeapon->dwMaxCount );
				if( !pBulletData )
					return;
			}
			pBulletData->WasteBullet( dwWasteCount, dwWasteTime, dwHostTime );
			CheckBulletData( pUser, dwItemCode );
		}
	}
}

void ioRelayGroupInfoMgr::OnAnthHackReloadBullet( DWORD dwRoomIndex, DWORD dwUserIndex, DWORD dwNPCIndex, DWORD dwHostTime, DWORD dwItemCode )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		UserData* pUser = pRelayGroup->GetUserData( dwUserIndex );
		if( pUser )
		{
			UserData::sBulletData* pBulletData = pUser->GetBulletData( dwItemCode );
			{
				if( pBulletData )
				{
					sWeaponInfo* pWeapon = GetWeaponInfo( dwItemCode );
					if( pWeapon )
					{
						//reload 시간체크
						DWORD dwReloadInterval = dwHostTime-pBulletData->dwReloadTime;
						//20프로 정도 봐줌
						if( dwReloadInterval * 1.2 < pWeapon->dwReloadTime )
						{
							CheatLOG.PrintTimeAndLog( 0, "[hack][bullet] OnAnthHackReloadBullet -  bullet reload time check over time(%u/%u) interval(%d)", pBulletData->dwReloadTime, dwHostTime, dwReloadInterval );
							AddPenaltyPoint( pUser, 5 );
							pBulletData->Clear();
						}
					}
					pBulletData->Clear();
					pBulletData->dwReloadTime = dwHostTime;
				}
				else
				{
					//CheatLOG.PrintTimeAndLog( 0, "[hack][bullet] OnAnthHackReloadBullet  time(%u) interval pBulletData is NULL", dwHostTime );
				}
				
			}
		}
	}
}

void ioRelayGroupInfoMgr::CheckBulletData( UserData* pUser, DWORD dwItemCode )
{
	if( !pUser )
		return;

	UserData::sBulletData* pData = pUser->GetBulletData( dwItemCode );
	if( !pData )
		return;

	int iSize = pData->vecRecvTime.size();
	if( iSize < 3 )
		return;

	sWeaponInfo* pWeapon = GetWeaponInfo( pData->dwItemCode );
	if( !pWeapon ||  pWeapon->dwMaxCount == 0 )
		return;

	//2가지 체크
	//1. 수량 ( 일단 20프로정도 더 가능하게 )
	if( pData->dwCurCount > pData->dwMaxCount + (pWeapon->dwMaxCount*0.2)  )
	{
		pData->dwCurCount = 0;
		AddPenaltyPoint( pUser, 5 );
		CheatLOG.PrintTimeAndLog( 0, "[hack][bullet] CheckBulletData - BulletCount check over count(%u/%u)", pData->dwCurCount, pData->dwMaxCount );
		return;
	}

	//2.발사 속도 검사
	int iCheatCnt = 0;
	for( int i = 0; i < iSize; ++i )
	{
		DWORD dwWasteCount = pData->vecWasteBullet[i];
		if( dwWasteCount == 1 )
			continue;
		DWORD dwWasteTime = pData->vecWasteTime[i];
		DWORD dwWasteInterval = dwWasteTime / dwWasteCount;
		if( dwWasteInterval < pWeapon->dwIntervalBullet )
		{
			iCheatCnt++;
			CheatLOG.PrintTimeAndLog( 0, "[hack][bullet] CheckBulletData - BulletWasteTime check over time(%u/%u)", dwWasteInterval, pWeapon->dwIntervalBullet );
		}
	}
	if( iCheatCnt > 0 )
	{	
		int iDefaultValue = 10;
		int iPenalty = (iDefaultValue * iCheatCnt) / iSize;
		AddPenaltyPoint( pUser, iPenalty );
	}

	
	//3.발사 간격 검사
	iCheatCnt = 0;
	for( int i = 1; i < iSize; ++i )
	{
		DWORD dwPrevTime = pData->vecRecvTime[i-1];
		DWORD dwNextTime = pData->vecRecvTime[i];
		DWORD dwInterVal = dwNextTime - dwPrevTime;
		if( dwInterVal < pWeapon->dwIntervalMotion )
		{
			iCheatCnt++;
			CheatLOG.PrintTimeAndLog( 0, "[hack][bullet] CheckBulletData - BulletRecvTime check over time(%u/%u)", dwInterVal, pWeapon->dwIntervalMotion );
		}
	}
	if( iCheatCnt > 0 )
	{	
		int iDefaultValue = 10;
		int iPenalty = (iDefaultValue * iCheatCnt) / iSize;
		AddPenaltyPoint( pUser, iPenalty );
	}
}

sWeaponInfo* ioRelayGroupInfoMgr::GetWeaponInfo( DWORD dwItemCode )
{
	static sWeaponInfo sWeapon;

	auto it = m_mapWeaponInfo.find( dwItemCode );
	if( it == m_mapWeaponInfo.end() )
	{
		//싱글톤에서 가져옴
		sWeaponInfo* pWeapon = g_SkillInfoMgr->GetWeaponInfo( dwItemCode );
		m_mapWeaponInfo.insert( std::map<DWORD,sWeaponInfo*>::value_type( dwItemCode, pWeapon ) );
	}
	
	it = m_mapWeaponInfo.find( dwItemCode );
	if( it == m_mapWeaponInfo.end() )
		return &sWeapon;

	return it->second;
}

void ioRelayGroupInfoMgr::UpdateDieState( DWORD dwRoomIndex, DWORD dwUserIndex, BOOL bState )
{
	RelayGroup* pRelayGroup = GetRelayGroupByRoom( dwRoomIndex );
	if(pRelayGroup)
	{
		UserData* pUser = pRelayGroup->GetUserData(dwUserIndex);
		if( pUser )
		{
			pUser->ClearBulletData();
		}
	}
}

#endif

//buff 이전타입계산
/*
void ioRelayGroupInfoMgr::OnSkillBuff( UserData* pUser, SkillInfo* pSkillInfo, float fIncMaxCoolTime, float fUseTimeDiff, float fErrorRate )
{	
if( fIncMaxCoolTime > fUseTimeDiff )
{
// 스킬 사용 직후 게이지가 소모되는 형태들..
// 사용하자마자 바로 취소되었다고 가정하고..

float fRemainGauge = pSkillInfo->fMaxGauge - pSkillInfo->fNeedGauge;
float fLowestUsingTime = 0.2f;
int nCnt = fLowestUsingTime / pSkillInfo->iTickTime;
//파이트클럽만 예외임..
if( iModeType == MT_FIGHT_CLUB )
nCnt = fLowestUsingTime / pSkillInfo->iFCTickTime;

float fLowestTime = pSkillInfo->fGaugePerTick * nCnt;					
float fRemainIncTime = fIncMaxCoolTime - (fRemainGauge / pSkillInfo->fMaxGauge) * fIncMaxCoolTime;
float fChecktime = fRemainIncTime - fLowestTime;
if( fChecktime > fUseTimeDiff )
{
//여기서부턴 일단 의심.
fChecktime = fChecktime * (1.f - fErrorRate);
float fDiffTime = fRemainIncTime - fUseTimeDiff;
float fAllowTime = fIncMaxCoolTime * fErrorRate;
if( fChecktime > fDiffTime )
{
// 일단 허용하는걸로
CheatLOG.PrintTimeAndLog( 0, "[skill][warn] TEST - WARN %s, fChecktime(%f), fDiffTime(%f)", pSkillInfo->hsSkillName.c_str(), fChecktime, fDiffTime );
}
else
{
AddPenaltySkill( pUser, pSkillInfo->hsSkillName, fUseTimeDiff, fChecktime );
// 치팅 유저 일 수 있으니 기록!
CheatLOG.PrintTimeAndLog( 0, "[skill][hack] Check - Uidx(%u), SkillName(%s), fChecktime(%f), fDiffTime(%f)"
, pUser->m_dwUserIndex, pSkillInfo->hsSkillName.c_str(), fChecktime, fDiffTime );

return;
}
}
}

return true;
}
*/