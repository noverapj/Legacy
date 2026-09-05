#include "stdafx.h"
#include "../NodeInfo/User.h"
#include ".\ioChannelingNodeNaver.h"
#include "..\Local\ioLocalParent.h"
#include "../NodeInfo/ioEtcItem.h"

ioChannelingNodeNaver::ioChannelingNodeNaver(void)
{
	
}

ioChannelingNodeNaver::~ioChannelingNodeNaver(void)
{
}

ChannelingType ioChannelingNodeNaver::GetType()
{
	return CNT_NAVER;
}

bool ioChannelingNodeNaver::AddRequestGetCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType )
{
	// pUser가 NULL이니 에러 메시지를 보낼 수 없다.
		if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	PACKET_GUARD_bool( rkSendPacket.Write(pUser->GetChannelingUserNo()) );
	return true;
}

bool ioChannelingNodeNaver::OnRecieveGetCash( User *pUser, SP2Packet &rkRecievePacket, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}


	DWORD        dwUserIndex     = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse   = false;
	int          iReturnValue    = 0;
	int          iReturnCash     = 0; 
	int          iPurchasedCash  = 0; 

	rkRecievePacket >> dwUserIndex;
	rkRecievePacket >> szBillingGUID;
	rkRecievePacket >> bSetUserMouse;
	rkRecievePacket >> iReturnValue;

	if( iReturnValue != CASH_RESULT_SUCCESS )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s ReturnValue is Error : %d:%s:%s[%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iReturnValue );
		pUser->ClearBillingGUID();
		SP2Packet kReturn( dwErrorPacketID );
		PACKET_GUARD_bool( kReturn.Write(dwErrorPacketType) );
		PACKET_GUARD_bool( kReturn.Write(bSetUserMouse) );
		pUser->SendMessage( kReturn );
		pUser->SetCash( 0 ); // 초기화
		pUser->SetPurchasedCash( 0 ); // 초기화
		pUser->SetChannelingCash( 0 ); // 초기화
		return false;
	}

	rkRecievePacket >> iReturnCash;
	rkRecievePacket >> iPurchasedCash;
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][cash]Channeling naver get cash success : [%d] [%s] [%d] [%d] [%d] [%d]", pUser->GetUserIndex(), pUser->GetBillingGUID().c_str(), iReturnCash, iPurchasedCash, pUser->GetCash(), pUser->GetChannelingCash() );
	pUser->SetCash( iReturnCash );
	pUser->SetPurchasedCash( iPurchasedCash ); // 캐쉬와 실재 구매한 캐쉬가 동일하다.
	pUser->SetChannelingCash( 0 );

	SP2Packet kPacket( STPK_GET_CASH );
	PACKET_GUARD_bool( kPacket.Write(GET_CASH_SUCCESS) );
	PACKET_GUARD_bool( kPacket.Write(bSetUserMouse) );
	PACKET_GUARD_bool( kPacket.Write(iReturnCash) );
	PACKET_GUARD_bool( kPacket.Write(iPurchasedCash) );
	pUser->SendMessage( kPacket );
	pUser->ClearBillingGUID();

	//Title check
	UserTitleInven* pInven = pUser->GetUserTitleInfo();
	if( pInven )
	{
		pInven->CheckTitleValue(TITLE_HAVE_GOLD, iReturnCash);
	}

	return true;
}

bool ioChannelingNodeNaver::AddRequestOutputCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType, IN const char *szRecvPrivateID, IN const char *szRecvPublicID, IN DWORD dwRecvUserIndex )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	PACKET_GUARD_bool( rkSendPacket.Write(pUser->GetChannelingUserNo()) );

	return true;
}

bool ioChannelingNodeNaver::CheckRequestOutputCash( User *pUser, int& iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType, DWORD dwItemCode, IntOfTwoVec& vConsumeInfo )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	int iCheckCash = pUser->GetCash();

	switch( dwErrorPacketID )
	{
	case STPK_PRESENT_BUY:
		iCheckCash = pUser->GetPurchasedCash();
		break;
	case STPK_ETCITEM_BUY:
		{
			if( ioEtcItem::EIT_ETC_TIME_CASH1 == dwItemCode || ioEtcItem::EIT_ETC_TIME_CASH2 == dwItemCode )
				iCheckCash = pUser->GetPurchasedCash();
		}
		break;
	}

	if( iCheckCash < iPayAmt )
	{
		if( IsPossibleToUseBonusCash(dwErrorPacketID) )
		{
			if( GetBonusCashInfoForOutputCash(pUser, iCheckCash, iPayAmt, vConsumeInfo) )
				return true;
		}

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsBillingTestID( pUser->GetPublicID() ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s IsBillingTestID is true : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
			return true;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Want of Cash : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
		SP2Packet kReturn( dwErrorPacketID );
		PACKET_GUARD_bool( kReturn.Write(dwErrorPacketType) );
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}

bool ioChannelingNodeNaver::CheckReciveOutputCash( User *pUser, int iReturnValue, int iPayAmt, bool bBillingError, const ioHashString &rsBillingError, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	if( iReturnValue != CASH_RESULT_SUCCESS )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s ReturnValue is Error : %d:%s:%s:%d", __FUNCTION__, iReturnValue, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), dwErrorPacketID );
		pUser->ClearBillingGUID();
		SP2Packet kReturn( dwErrorPacketID );
		PACKET_GUARD_bool( kReturn.Write(dwErrorPacketType) );
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}
