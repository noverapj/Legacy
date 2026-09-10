#pragma once
#include "StdAfx.h"

namespace TournamentPacketParser
{
	bool ProcessTCPPacket( SP2Packet &rkPacket );
	
	void OnTournamentRegularInfo( SP2Packet &rkPacket );
	void OnTournamentMainInfo( SP2Packet &rkPacket );
	void OnTournamentTeamList( SP2Packet &rkPacket );
	void OnTournamentTeamInfo( SP2Packet &rkPacket );
	void OnTournamentTeamUserList( SP2Packet &rkPacket );
	void OnTournamentScheduleInfo( SP2Packet &rkPacket );
	void OnTournamentRoundTeamData( SP2Packet &rkPacket );
	void OnTournamentListRequest( SP2Packet &rkPacket );	
	void OnTournamentCreateTeam( SP2Packet &rkPacket );
	void OnTournamentTeamInvited( SP2Packet &rkPacket );
	void OnTournamentTeamEntryDelayMember( SP2Packet &rkPacket );
	void OnTournamentTeamEntryApp( SP2Packet &rkPacket );
	void OnTournamentTeamEntryAgree( SP2Packet &rkPacket );
	void OnTournamentTeamJoin( SP2Packet &rkPacket );
	void OnTournamentTeamLeave( SP2Packet &rkPacket );
	void OnTournamentEndProcess( SP2Packet &rkPacket );
	void OnTournamentBattleInvite( SP2Packet &rkPacket );
	void OnTournamentTeamPosition( SP2Packet &rkPacket );
	void OnTournamentRewardData( SP2Packet &rkPacket );
	void OnTournamentRoomList( SP2Packet &rkPacket );
	void OnTournamentCustomCreate( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateList( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateData( SP2Packet &rkPacket );
	void OnTournamentTeamJoinConfirmCheck( SP2Packet &rkPacket );
	void OnTournamentTeamJoinConfirmRequest( SP2Packet &rkPacket );
	void OnTournamentTeamJoinConfirmList( SP2Packet &rkPacket );
	void OnTournamentAnnounceChange( SP2Packet &rkPacket );
	void OnTournamentTotalTeamList( SP2Packet &rkPacket );
	void OnTournamentTeamJoinConfirmCommand(SP2Packet &rkPacket );
	void OnTournamentCustomStateStart(SP2Packet &rkPacket );
	void OnTournamentCustomRewardList( SP2Packet &rkPacket );
	void OnTournamentCustomRewardBuy( SP2Packet &rkPacket );
	void OnTournamentCustomRewardData( SP2Packet &rkPacket );
	void OnTournamentEndProcess( SP2Packet &rkPacket );

	void OnTournamentCheerDecision( SP2Packet &rkPacket );
	void OnTournamentCheerList( SP2Packet &rkPacket );
}