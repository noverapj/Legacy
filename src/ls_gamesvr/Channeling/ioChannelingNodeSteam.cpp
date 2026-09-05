#include "stdafx.h"
#include "../NodeInfo/User.h"
#include ".\ioChannelingNodeSteam.h"
#include "..\Local\ioLocalParent.h"
#include "..\EtcHelpFunc.h"
#include "..\BillingRelayServer\BillingRelayServer.h"
#include "..\MainProcess.h"

ioChannelingNodeSteam::ioChannelingNodeSteam(void)
{

}

ioChannelingNodeSteam::~ioChannelingNodeSteam(void)
{
}

ChannelingType ioChannelingNodeSteam::GetType()
{
	return CNT_STEAM;
}

bool ioChannelingNodeSteam::AddRequestGetCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType )
{
	// pUser가 NULL이니 에러 메시지를 보낼 수 없다.
	// 	if( !pUser )
	// 	{
	// 		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
	// 		return false;
	// 	}
	// 
	// 	rkSendPacket << pUser->GetPublicIP();

	return true;
}

bool ioChannelingNodeSteam::OnRecieveGetCash( User *pUser, SP2Packet &rkRecievePacket, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
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
	bool         bBillingError   = false;
	ioHashString sBillingError;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s GetData Size : %d %d", __FUNCTION__, rkRecievePacket.GetDataSize(),
		rkRecievePacket.GetBufferSize() );

	rkRecievePacket >> dwUserIndex;
	rkRecievePacket >> szBillingGUID;
	rkRecievePacket >> bSetUserMouse;
	rkRecievePacket >> iReturnValue;

	if( iReturnValue != CASH_RESULT_SUCCESS )
	{
		rkRecievePacket >> bBillingError;
		if( bBillingError )
			rkRecievePacket >> sBillingError;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s ReturnValue is Error : %d:%s:%s[%d:%d:%s]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iReturnValue, bBillingError, sBillingError.c_str() );
		pUser->ClearBillingGUID();
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		kReturn << bSetUserMouse;
		kReturn << bBillingError;
		if( bBillingError )
			kReturn << sBillingError;
		pUser->SendMessage( kReturn );
		pUser->SetCash( 0 ); // 초기화
		pUser->SetPurchasedCash( 0 ); // 초기화
		pUser->SetChannelingCash( 0 ); // 초기화
		return false;
	}

	rkRecievePacket >> iReturnCash;
	rkRecievePacket >> iPurchasedCash;

	pUser->SetCash( iReturnCash );

	pUser->SetPurchasedCash( iPurchasedCash );
	pUser->SetChannelingCash( 0 );
	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Success : %d:%s:%s:[%d:%d/%d:%d]", 
		__FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), 
		pUser->GetBillingGUID().c_str(), iReturnCash, iPurchasedCash, 
		pUser->GetCash(), pUser->GetPurchasedCash() );
	
	SP2Packet kPacket( STPK_GET_CASH );
	kPacket << GET_CASH_SUCCESS;
	kPacket << bSetUserMouse;
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	pUser->SendMessage( kPacket );
	pUser->ClearBillingGUID();

	return true;
}

bool ioChannelingNodeSteam::AddRequestOutputCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType, IN const char *szRecvPrivateID, IN const char *szRecvPublicID, IN DWORD dwRecvUserIndex )
{
	// 	if( !pUser )
	// 	{
	// 		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
	// 		return false;
	// 	}
	// 
	// 	rkSendPacket << g_App.GetCSPort();
	// 
	// 	// 선물인경우 받을 유저 정보가 있다
	// 	if( szRecvPrivateID != NULL && 
	// 		szRecvPublicID != NULL )
	// 	{
	// 		ioHashString sRecvPrivateID = szRecvPrivateID; // 패킷에 const char*을 바로 넣을 수 없어서
	// 		ioHashString sRecvPublicID  = szRecvPublicID;  // 패킷에 const char*을 바로 넣을 수 없어서
	// 		rkSendPacket << sRecvPrivateID;
	// 		rkSendPacket << sRecvPublicID;
	// 	}

	return true;
}

bool ioChannelingNodeSteam::CheckRequestOutputCash( User *pUser, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
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
	}

	if( iCheckCash < iPayAmt )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsBillingTestID( pUser->GetPublicID() ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s IsBillingTestID is true : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
			return true;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Want of Cash : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}

bool ioChannelingNodeSteam::CheckRequestOutputCash( User *pUser, int& iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType, DWORD dwItemCode, IntOfTwoVec& vConsumeInfo )
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
	}

	if( iCheckCash < iPayAmt )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsBillingTestID( pUser->GetPublicID() ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s IsBillingTestID is true : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
			return true;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Want of Cash : %d:%s:%d [%d:%d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPayAmt, dwErrorPacketID, iCheckCash );
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}

bool ioChannelingNodeSteam::CheckReciveOutputCash( User *pUser, int iReturnValue, int iPayAmt, bool bBillingError, const ioHashString &rsBillingError, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
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
		if( iReturnValue == CASH_RESULT_ERROR_EXCESS_BUY )
		{
			if( dwErrorPacketID == STPK_CHAR_CREATE )
				dwErrorPacketType = CREATE_CHAR_BILLING_EXCESS_BUY;
			else if( dwErrorPacketID == STPK_CHAR_DECORATION_BUY )
				dwErrorPacketType = CHAR_DECORATION_BILLING_EXCESS_BUY;
			else if( dwErrorPacketID == STPK_CHAR_EXTEND )
				dwErrorPacketType = CHAR_EXTEND_BILLING_EXCESS_BUY;
			else if( dwErrorPacketID == STPK_ETCITEM_BUY )
				dwErrorPacketType = ETCITEM_BUY_BILLING_EXCESS_BUY;
			else if( dwErrorPacketID == STPK_CHAR_CHANGE_PERIOD )
				dwErrorPacketType = CHAR_CHANGE_PERIOD_BILLING_EXCESS_BUY;
			else if( dwErrorPacketID == STPK_EXTRAITEM_BUY )
				dwErrorPacketType = EXTRAITEM_BUY_BILLING_EXCESS_BUY;
			else if( dwErrorPacketID == STPK_PRESENT_BUY )
				dwErrorPacketType = PRESENT_BUY_BILLING_EXCESS_BUY;
		}
		kReturn << dwErrorPacketType;
		kReturn << bBillingError;
		if( bBillingError )
			kReturn << rsBillingError;
		pUser->SendMessage( kReturn );
		return false;
	}

	if( pUser->GetCash() < iPayAmt )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Want of Cash : %s:%s:%d:%d:%d", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), pUser->GetCash(), iPayAmt, dwErrorPacketID );
		pUser->ClearBillingGUID();
		SP2Packet kReturn( dwErrorPacketID );
		kReturn << dwErrorPacketType;
		pUser->SendMessage( kReturn );
		return false;
	}

	return true;
}


void ioChannelingNodeSteam::SendCancelCash( IN SP2Packet &rkPacket )
{
	if( rkPacket.GetPacketID() != BSTPK_OUTPUT_CASH_RESULT )
		return;

	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int          iReturnValue = 0;
	int          iTransactionID = 0;
	int          iCash = 0;
	int          iPurchasedCash = 0;
	ioHashString szUserIP;
	ioHashString szChargeNo;

	rkPacket >> dwUserIndex;
	rkPacket >> szBillingGUID;
	rkPacket >> iReturnValue;

	if( iReturnValue != CASH_RESULT_SUCCESS )
		return;

	rkPacket >> iType;
	rkPacket >> iPayAmt;
	rkPacket >> iTransactionID;
	int iItemValueList[ioChannelingNodeParent::MAX_ITEM_VALUE];
	for (int i = 0; i < ioChannelingNodeParent::MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );

	// Cancel Step 2
	rkPacket >> iChannelingType;
	rkPacket >> iCash;
	rkPacket >> iPurchasedCash; 
	rkPacket >> szPrivateID;
	rkPacket >> szUserIP;
	rkPacket >> szChargeNo;

	//--------------------------------SEND------------------------------------//
	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	Help::GetGUID( szTempGUID, sizeof(szTempGUID) );
	SP2Packet kBillingPacket( BSTPK_CANCEL_CASH );
	// 공통
	kBillingPacket << iChannelingType;
	kBillingPacket << szTempGUID;
	kBillingPacket << dwUserIndex;

	// Cancel Step 3
	kBillingPacket << szPrivateID;
	kBillingPacket << szUserIP;
	kBillingPacket << szChargeNo;

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GOLD_INFO]%s Send Fail : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GOLD_INFO]%s Send : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
}

bool ioChannelingNodeSteam::AddRequestFillCashUrlPacket( IN User *pUser, OUT SP2Packet &rkSendPacket )
{
	if( !pUser )
	{
	 	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
	 	return false;
	}
	 
	rkSendPacket << pUser->GetPrivateID() << pUser->GetPublicID();
		
	return true;
}

bool ioChannelingNodeSteam::AddReuestSubscriptionRetractCashCheckPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{ //kyg 있어야하는지 고민 
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	rkSendPacket << pUser->GetPublicIP();

	return true;
}

bool ioChannelingNodeSteam::OnReceiveSubscriptionRetractCashCheck( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	DWORD		 dwUserIndex = 0;
	ioHashString szBillingGUID;
	DWORD		 dwIndex = 0;
	ioHashString szSubscriptionID;
	int			 iReturnCode;
	DWORD		 dwChargedAmt = 0; // 전체 사용 금액 
	DWORD		 dwRealChargedAmt = 0; 
	DWORD		 dwBonusChargedAmt = 0;
	DWORD		 dwCanceledAmt = 0; // 중요 청약 철회시 복원 가능 전체금액
	DWORD		 dwRealCash = 0; // dwCanceledAmt 에서 진짜 캐쉬 
	DWORD		 dwBonusCash = 0; // dwCAncncledAmt 에서 무료 충전 캐쉬 

	PACKET_GUARD_bool( rReceivePacket.Read(dwUserIndex) );
	PACKET_GUARD_bool( rReceivePacket.Read(szBillingGUID) );
	PACKET_GUARD_bool( rReceivePacket.Read(dwIndex) );
	PACKET_GUARD_bool( rReceivePacket.Read(szSubscriptionID) );
	PACKET_GUARD_bool( rReceivePacket.Read(iReturnCode) );

	// 청약 철회 조회 완료 //Index, SubscriptionID, SubscriptionGold, RetractGold
	if( iReturnCode == BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS )
	{
		PACKET_GUARD_bool( rReceivePacket.Read(dwChargedAmt) );
		PACKET_GUARD_bool( rReceivePacket.Read(dwRealChargedAmt) );
		PACKET_GUARD_bool( rReceivePacket.Read(dwBonusChargedAmt) );
		PACKET_GUARD_bool( rReceivePacket.Read(dwCanceledAmt) );
		PACKET_GUARD_bool( rReceivePacket.Read(dwRealCash) );
		PACKET_GUARD_bool( rReceivePacket.Read(dwBonusCash) );

		int iRetractGold = dwRealCash+dwBonusCash;
		pUser->SetRetractGold( dwIndex, szSubscriptionID, iRetractGold );

		SP2Packet kPacket( STPK_SUBSCRIPTION_RETR_CHECK );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_CHECK_OK) );
		PACKET_GUARD_bool( kPacket.Write(dwIndex) );
		PACKET_GUARD_bool( kPacket.Write(szSubscriptionID) );
		PACKET_GUARD_bool( kPacket.Write(dwRealChargedAmt) );
		PACKET_GUARD_bool( kPacket.Write(dwRealCash) );

		pUser->ClearBillingGUID();

		return pUser->SendMessage( kPacket );
	}
	else if ( iReturnCode == BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL )
	{
		SP2Packet kPacket( STPK_SUBSCRIPTION_RETR_CHECK );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_CHECK_EXCEPTION) );
		PACKET_GUARD_bool( kPacket.Write(9) );

		pUser->ClearBillingGUID();

		pUser->SendMessage( kPacket );

		ioHashString errorMsg;

		PACKET_GUARD_bool( rReceivePacket.Read(errorMsg) );

		LOG.PrintTimeAndLog(0, "%s Fail WeMadeyBuyServer Rtn ErrCode (%s)",__FUNCTION__,errorMsg.c_str());	
	}
	else
	{
		SP2Packet kPacket( STPK_SUBSCRIPTION_RETR_CHECK );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_CHECK_EXCEPTION) );
		PACKET_GUARD_bool( kPacket.Write(9) );

		pUser->ClearBillingGUID();

		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog(0,"%s Fail BillingRelayServer Rtn WrongCode(%d)",__FUNCTION__,iReturnCode);
	}
	return true;
}

bool ioChannelingNodeSteam::AddReuestSubscriptionRetractCashPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	rkSendPacket << pUser->GetPublicIP();

	return true;
}

bool ioChannelingNodeSteam::OnReceiveSubscriptionRetractCash( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	DWORD        dwUserIndex     = 0;
	ioHashString szBillingGUID;
	DWORD		 dwIndex;
	ioHashString szSubscriptionID;
	int			 iReturnCode = 0;
	int			 iCanceledCashAmt = 0;
	int			 iRealCash = 0;
	int 		 iBonusCash = 0;//이름 바꿀것 

	PACKET_GUARD_bool( rReceivePacket.Read(dwUserIndex) );
	PACKET_GUARD_bool( rReceivePacket.Read(szBillingGUID) );
	PACKET_GUARD_bool( rReceivePacket.Read(dwIndex) );
	PACKET_GUARD_bool( rReceivePacket.Read(szSubscriptionID) );
	PACKET_GUARD_bool( rReceivePacket.Read(iReturnCode) );


	if( iReturnCode == BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS )
	{
		//청약 철회 패킷 (성공)
		PACKET_GUARD_bool( rReceivePacket.Read(iCanceledCashAmt) );
		PACKET_GUARD_bool( rReceivePacket.Read(iRealCash) );
		PACKET_GUARD_bool( rReceivePacket.Read(iBonusCash) );

		int iResultCancelCash = pUser->GetRetractGold( dwIndex, szSubscriptionID );

		SP2Packet kReturnPacket( STPK_SUBSCRIPTION_RETR );
		pUser->SetSubscriptionRetract( dwIndex, szSubscriptionID, iResultCancelCash, kReturnPacket );

		//kyg 여기에 패킷 추가할것은 도형씨와 협의 필요 130704
		pUser->ClearBillingGUID();

		return pUser->SendMessage(kReturnPacket);
	}
	else if ( iReturnCode == BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL)
	{
		ioHashString errCode;

		PACKET_GUARD_bool( rReceivePacket.Read(errCode) );

		SP2Packet kPacket( STPK_SUBSCRIPTION_RETR );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_EXCEPTION) );
		PACKET_GUARD_bool( kPacket.Write(9) );

		pUser->SendMessage( kPacket );

		pUser->ClearBillingGUID();

		LOG.PrintTimeAndLog(0,"%s Error WemadeBuyServer Rtn ErrorCode(%d:%s)",__FUNCTION__,iReturnCode,errCode.c_str());
	}
	else
	{
		//error 상황임 

		SP2Packet kPacket( STPK_SUBSCRIPTION_RETR );

		PACKET_GUARD_bool( kPacket.Write(SUBSCRIPTION_RETR_EXCEPTION) );
		PACKET_GUARD_bool( kPacket.Write(9) );

		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog(0,"%s Error BillingRelayServer  Rtn WrongCode(%d)",__FUNCTION__,iReturnCode);
	}
	return true;
}

bool ioChannelingNodeSteam::AddReuestSubscriptionRetractCashFailPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{
	return true;
}

bool ioChannelingNodeSteam::OnReceiveSubscriptionRetractCashFail( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	//
	return true;
}
