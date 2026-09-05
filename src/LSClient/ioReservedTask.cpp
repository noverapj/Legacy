#include "StdAfx.h"
#include "ioReservedTask.h"

#include "GUI/AttendanceWnd.h"
#include "GUI/TournamentDetailMainWnd.h"

#include "GUI/RollBookWnd.h"
#include "GUI/GuildRewardWnd.h"

#include "GUI/ExPCRoomWnd.h"

template<> ioReservedTask* Singleton< ioReservedTask >::ms_Singleton = 0;
ioReservedTask& ioReservedTask::GetSingleton()
{
	return Singleton< ioReservedTask >::GetSingleton();
}

ioReservedTask::ioReservedTask(void)
{
	Init();
}

ioReservedTask::~ioReservedTask(void)
{
	Destroy();
}

void ioReservedTask::Init()
{
}

void ioReservedTask::Destroy()
{
	for( vReservedTask::iterator it = m_vReservedTask.begin() ; it != m_vReservedTask.end() ; ++it )
	{
		ioTask* pTask = *it;
		if( !pTask )
			continue;
		
		delete pTask;
	}
	m_vReservedTask.clear();
}

void ioReservedTask::AddReservedTask( ioTask* pNewTask )
{
	if( !pNewTask )
	{
		LOG.PrintTimeAndLog(0, "%s - pNewTask == NULL", __FUNCTION__ );
		return;
	}

	// 예약된 작업실행
	for( vReservedTask::iterator it = m_vReservedTask.begin() ; it != m_vReservedTask.end() ; ++it )
	{
		ioTask* pTask = *it;
		if( !pTask )
			continue;

		//같은 예약이면 Add하지 않음
		if( pTask->eType == pNewTask->eType && pTask->eWhen == pNewTask->eWhen )
			return;
	}

	m_vReservedTask.push_back( pNewTask );
}

const ioTask* const ioReservedTask::GetTask( ReservedTaskTypes eTaskType, ReservedTaskWhens eWhen )
{
	for( vReservedTask::iterator it = m_vReservedTask.begin() ; it != m_vReservedTask.end() ; ++it )
	{
		ioTask* pTask = *it;
		if( !pTask )
			continue;

		//같은 예약이면 Add하지 않음
		if( pTask->eType == eTaskType && pTask->eWhen == eWhen )
		{
			return pTask;
		}
	}

	return NULL;
}

void ioReservedTask::ExecuteReservedTask( ReservedTaskWhens eWhen )
{
	// 예약된 작업실행
	for( vReservedTask::iterator it = m_vReservedTask.begin() ; it != m_vReservedTask.end() ; ++it )
	{
		ioTask* pTask = *it;
		if(!pTask)
			continue;

		if( pTask->eWhen == eWhen )
		{
			//현재 하나의 예약작업만 처리함
			pTask->bEraseAfterExecution = OnExecute( pTask );
			break;
		}
	}

	// 예약된 작업삭제
	for( vReservedTask::iterator it = m_vReservedTask.begin() ; it != m_vReservedTask.end() ;  )
	{
		ioTask* pTask = *it;
		if( !pTask ) 
		{
			++it;
			continue;
		}

		if( !pTask->bEraseAfterExecution )
		{
			++it;
			continue;
		}
		
		delete pTask;
		it = m_vReservedTask.erase( it );
	}
}

void ioReservedTask::RemoveTask(const ReservedTaskTypes eType)
{
	for( vReservedTask::iterator it = m_vReservedTask.begin() ; it != m_vReservedTask.end() ; ++it )
	{
		ioTask* pTask = *it;
		if( !pTask )
			continue;
		
		if(pTask->eType == eType)
		{
			delete pTask;
			it = m_vReservedTask.erase(it);
			break;
		}
	}
}

bool ioReservedTask::OnExecute( ioTask* pTask )
{
	switch( pTask->eType )
	{
	case RESERVEDCMD_TYPE_SERVERCHANGE:
		return OnExecuteServerChange( pTask );	
	case RESERVEDCMD_TYPE_ATTENDANCE:
		return OnExecuteRecvAtttendance( pTask );	
	case RESERVEDCMD_TYPE_PCROOOM:
		return OnExecuteRecvPCRoom( pTask );	
	case RESERVEDCMD_TYPE_TOURNAMENT:
		return OnExecuteRecvTournament( pTask );
	case RESERVEDCMD_TYPE_ROLLBOOK:
		return OnExecuteRecvRollBook( pTask );
	case RESERVEDCMD_TYPE_GUILD_RANK_REWARD:
		return OnExecuteRecvGuldRankReward( pTask );
	break;
	}

	return true;
}

bool ioReservedTask::OnExecuteServerChange( ioTask* pTask )
{
	ioTaskServerMove* task = dynamic_cast<ioTaskServerMove*>(pTask);
	if( task )
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_MYROOM_SERVER_CHANGE );
		kPacket << task->iServerIndex;
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s - pTask casting FAIL!!", __FUNCTION__ );		
	}

	return true;
}

bool ioReservedTask::OnExecuteRecvAtttendance( ioTask* pTask )
{
	if( !pTask )
	{
		LOG.PrintTimeAndLog(0, "%s - pTask == NULL", __FUNCTION__ );
		return true;
	}

	switch( pTask->eWhen )
	{
	case RESERVEDCMD_WHEN_RECV_ATTENDANCE:
		{
			AttendanceWnd* pWnd = dynamic_cast<AttendanceWnd*>( g_GUIMgr.FindWnd( ATTENDANCE_WND ) );
			if( pWnd )
			{
				pWnd->ShowWnd();
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - %d - ATTENDANCE_WND not find", __FUNCTION__, pTask->eWhen );
			}
			return true;
		}
		break;
	case RESERVEDCMD_WHEN_RECV_ATTENDANCE_REWARD:
		{
			ioTaskAttendanceReward* pReward =  dynamic_cast<ioTaskAttendanceReward*>( pTask );
			if( pReward )
			{
				AttendanceWnd* pWnd = dynamic_cast<AttendanceWnd*>( g_GUIMgr.FindWnd( ATTENDANCE_WND ) );
				if( pWnd )
				{
					pWnd->AddAttendanceRecord( pReward->dwDate );
					pWnd->SetReward( pReward->iTodayPresentType, pReward->iTodayValue1, pReward->iTodayValue2, pReward->iAccruePresentType, pReward->iAccrueValue1, pReward->iAccrueValue2 );					
				}
				else
				{
					LOG.PrintTimeAndLog(0, "%s - %d - ATTENDANCE_WND not find", __FUNCTION__, pTask->eWhen );
				}
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - %d - pTask casting FAIL!!", __FUNCTION__, pTask->eWhen );
			}
			return true;
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog(0, "%s - UNKOWN TYPE", __FUNCTION__ );
		}
	}

	return false;
}

bool ioReservedTask::OnExecuteRecvPCRoom( ioTask* pTask )
{
	//파산 상태에서는 pc방 안내혜택 Off
	if( g_MyInfo.IsBankruptcy() || g_MyInfo.IsCharBankruptcy() )
		return false;
	
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsPCRoomBonusSoldier() )
	{
		if( g_MyInfo.GetPcRoomCharMax() <= 1 )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "경험치+100％ / 페소+50％" );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "PC방 전용 용병 제공" );

			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );
			kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, "낚시/유물탐사 PC방 혜택 제공" );
			g_GUIMgr.SetPrevMsgListBoxWithTitle( g_GUIMgr.FindWnd( PCROOM_SOLDIER_BONUS_WND ), MB_OK, NULL, kPrinter, "넥슨 피씨방 혜택 적용 중", "닫기", "확인", "취소" );
		}
		else
		{
			if( g_MyInfo.GetPCRoomAuthority() ==  FREEDAY_EVENT_CODE )
				g_GUIMgr.ShowWnd( USER_SELECT_HERO_BONUS_WND );
			else if( g_MyInfo.GetPCRoomAuthority() != 0 )
				g_GUIMgr.ShowWnd( EX_PCROOM_AUTHORITY_WND );
			
		}
	}
	else
	{
		g_GUIMgr.ShowWnd( PCROOM_LOCAL_BENEFIT_WND );
	}

	return true;
}

bool ioReservedTask::OnExecuteRecvTournament( ioTask* pTask )
{
	switch( pTask->eWhen )
	{
	case RESERVEDCMD_WHEN_RECV_TOURNAEMNT_REWARD:
		{
			ioTaskTournamentReward* pReward =  dynamic_cast<ioTaskTournamentReward*>( pTask );
			if( pReward )
			{
				// 보상 UI 표시만한다. 선물함에 이미 들어있음
				TournamentAlarmMainRewardWnd* pWnd = dynamic_cast<TournamentAlarmMainRewardWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_REWARD_ALARM_WND ) );
				if( pWnd )
				{
					pWnd->SetRewardInfo( pReward->AttainTourPos,
										 pReward->dwStartDate,
										 pReward->iMyCampPos,
										 pReward->iWinCampPos,
										 pReward->iLadderBonusPeso,
										 pReward->iLadderRank,
										 pReward->iLadderPoint,
										 pReward->dwCheerPeso,
										 pReward->szWinnerTeamName );
					pWnd->ShowWnd();

					if( pReward->iLadderPoint > 0 )
					{
						g_QuestMgr.QuestCompleteTerm( QC_CAMP_SEASON_REWARD, RESERVEDCMD_TYPE_TOURNAMENT, RESERVEDCMD_WHEN_RECV_TOURNAEMNT_REWARD );
						g_QuestMgr.QuestOccurTerm( QO_CAMP_SEASON_REWARD );						
					}
					LOG.PrintTimeAndLog( 0 , "%s : Show Tournament wnd", __FUNCTION__ );
				}
			}
		}
		break;
	}

	return true;
}

bool ioReservedTask::OnExecuteRecvRollBook( ioTask* pTask )
{
	if( !pTask )
	{
		LOG.PrintTimeAndLog(0, "%s - pTask == NULL", __FUNCTION__ );
		return true;
	}

	switch( pTask->eWhen )
	{
	case RESERVEDCMD_WHEN_ROLLBOOK:
		{
			ioTaskRollBook* pRollBookTask =  dynamic_cast<ioTaskRollBook*>( pTask );
			if( pRollBookTask )
			{
				RollBookWnd* pWnd = dynamic_cast<RollBookWnd*>( g_GUIMgr.FindWnd( ROLLBOOK_WND ) );
				if( pWnd )
				{
					pWnd->SettingRollBookByPacket( pRollBookTask->dwReceiveTime, pRollBookTask->iState, pRollBookTask->iTablePage, pRollBookTask->iPosition );
				}
				else
				{
					LOG.PrintTimeAndLog(0, "%s - %d - ATTENDANCE_WND not find", __FUNCTION__, pTask->eWhen );
				}
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - %d - pTask casting FAIL!!", __FUNCTION__, pTask->eWhen );
			}
			return true;
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog(0, "%s - UNKOWN TYPE", __FUNCTION__ );
		}
	}

	return false;
}

bool ioReservedTask::OnExecuteRecvGuldRankReward( ioTask* pTask )
{
	if( !pTask )
	{
		LOG.PrintTimeAndLog(0, "%s - pTask == NULL", __FUNCTION__ );
		return true;
	}

	switch( pTask->eWhen )
	{
	case RESERVEDCMD_WHEN_RECV_GUILD_RANK_REWARD:
		{
			ioTaskGuildRankReward* pGulildRankRewardTask =  dynamic_cast<ioTaskGuildRankReward*>( pTask );
			if( pGulildRankRewardTask )
			{
				GuildRankRewardWnd *pGuildRankRewardWnd = dynamic_cast< GuildRankRewardWnd* >( g_GUIMgr.FindWnd( GUILD_RANK_REWARD_WND ) );
				if( pGuildRankRewardWnd )
					pGuildRankRewardWnd->SettingRewardInfo( g_GuildInfo.GetMyGuildLevel(), 
						(short)pGulildRankRewardTask->iPresentType, pGulildRankRewardTask->iPresentValue1, pGulildRankRewardTask->iPresentValue2 );

				LOG.PrintTimeAndLog( 0 , "%s : Show Guild Rank Reward wnd ", __FUNCTION__ );
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - %d - pTask casting FAIL!!", __FUNCTION__, pTask->eWhen );
			}
			return true;
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog(0, "%s - UNKOWN TYPE", __FUNCTION__ );
		}
	}

	return false;
}
