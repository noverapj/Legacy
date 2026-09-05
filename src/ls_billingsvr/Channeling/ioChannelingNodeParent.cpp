#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/GoodsManager.h"
#include "./iochannelingnodeparent.h"
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalParent.h"
#include "../MainProcess.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../NodeInfo/MemInfoManager.h"

extern CLog LOG;

ioChannelingNodeParent::ioChannelingNodeParent(void)
{
}

ioChannelingNodeParent::~ioChannelingNodeParent(void)
{
}

ChannelingType ioChannelingNodeParent::GetType()
{
	return CNT_NONE;
}

void ioChannelingNodeParent::OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
 	int            iChannelingType = 0;
	ioHashString   szBillingGUID;
	DWORD          dwUserIndex     = 0;
	ioHashString   szPrivateID;
	ioHashString   szPublicID;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID;


	if( szBillingGUID.IsEmpty() || dwUserIndex == 0 || szPrivateID.IsEmpty() || szPublicID.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "%s Received Data is Wrong:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

	if(g_App.IsTestMode())
	{
		SendTestGetCash(pServerNode,rkPacket);
		LOG.PrintTimeAndLog(0,"Private:%s Testmode GetCash",szPrivateID.c_str());

		return;
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		rkPacket.SetPosBegin(); // Pos to Begin.
		_OnGetCash( pServerNode, rkPacket );
	}
	else
	{
		// Local이 channeling 안에 있다, 개념상으로는 동등하거나 상위이지만 에러 체크 함수를 channeling이 가지고 있어 내부에 추가함.
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			rkPacket.SetPosBegin(); // Pos to Begin.
			pLocal->_OnGetCash( pServerNode, rkPacket );
		}
	}
}

void ioChannelingNodeParent::OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	int          iReturnItemPrice = 0;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	ioHashString szCountry;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int			 iBonusCashSize	= 0;

	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP;
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN )	
		rkPacket >> szCountry;	
	rkPacket >> iPayAmt >> iBonusCashSize;
		
	for( int i = 0; i < iBonusCashSize; i++ )
	{
		int iIndex	= 0;
		int iPay	= 0;

		rkPacket >>	iIndex >> iPay;
	}
		
	rkPacket >>	iType;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

	// error check
	if( szBillingGUID.IsEmpty() || dwUserIndex == 0 || szPublicID.IsEmpty() || szPrivateID.IsEmpty() || szUserIP.IsEmpty() || iPayAmt <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Received Data is Wrong:%s:%d:%s:%s:%s:%d", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szUserIP.c_str(), iPayAmt  );

		if( dwUserIndex != 0 && !szBillingGUID.IsEmpty() )
		{
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
			kPacket << dwUserIndex;
			kPacket << szBillingGUID;
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << iReturnItemPrice;
			kPacket << iType;
			pServerNode->SendMessage( kPacket );
		}
		return;
	}

	DWORD dwGoodsNo = 0;
	ioHashString szGoodsName;

	ItemInfo kBuyInfo;
	GetItemInfo( rkPacket, iType, kBuyInfo );

	bool bError      = false;
	if( iType == OUTPUT_CASH_SOLDIER )
	{
		if( !g_GoodsMgr.GetGoodsInfoSoldier( kBuyInfo.m_iClassType, kBuyInfo.m_iLimitSecond, kBuyInfo.m_iPeriodType, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(Soldier):%s:%s:%d:%d:%d:%d:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iClassType, kBuyInfo.m_iKindred, kBuyInfo.m_iSex, kBuyInfo.m_iLimitSecond, kBuyInfo.m_iPeriodType );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_DECO )
	{
		if( !g_GoodsMgr.GetGoodsInfoDeco( kBuyInfo.m_iDecoType, kBuyInfo.m_iDecoCode, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(Deco):%s:%s:%d:%d:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iDecoType, kBuyInfo.m_iDecoCode, kBuyInfo.m_iCharArray );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_SOLDIER_EXTEND )
	{
#ifdef DATE_CHAMP
		int nPeriodTime = GoodsManager::CPT_TIME;

		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN )
			nPeriodTime = GoodsManager::CPT_DATE;

		if( !g_GoodsMgr.GetGoodsInfoSoldier( kBuyInfo.m_iClassType, kBuyInfo.m_iLimitSecond, nPeriodTime, dwGoodsNo, szGoodsName ) )
#else
		if( !g_GoodsMgr.GetGoodsInfoSoldier( kBuyInfo.m_iClassType, kBuyInfo.m_iLimitSecond,GoodsManager::CPT_TIME, dwGoodsNo, szGoodsName ) )
#endif
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(SoldierExtend):%s:%s:%d:%d:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iCharArray, kBuyInfo.m_iClassType, kBuyInfo.m_iLimitSecond );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_ETC )
	{
		if( !g_GoodsMgr.GetGoodsInfoEtc( kBuyInfo.m_dwType, kBuyInfo.m_iArray, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(Etc):%s:%s:%d:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_dwType, kBuyInfo.m_iArray );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_SOLDIER_CHANGE_PERIOD )
	{
		if( !g_GoodsMgr.GetGoodsInfoSoldier( kBuyInfo.m_iClassType, 0, GoodsManager::CPT_MORTMAIN, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(SoldierChangePeriod):%s:%s:%d:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iCharArray, kBuyInfo.m_iClassType );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_EXTRA )
	{
		if( !g_GoodsMgr.GetGoodsInfoExtraBox( kBuyInfo.m_iMachineCode, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(Extra):%s:%s:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iMachineCode );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_PRESENT )
	{
		if( !g_GoodsMgr.GetGoodsInfoPresent( kBuyInfo.m_iPresentType, kBuyInfo.m_iBuyValue1, kBuyInfo.m_iBuyValue2, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(Present):%s:%s[%d:%d:%d]", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iPresentType, kBuyInfo.m_iBuyValue1, kBuyInfo.m_iBuyValue2 );
			bError = true;
		}
	}
	else if ( iType == OUTPUT_CASH_SUBSCRIPTION )
	{
		//청약 철회 상품 ㄱ 
		//로그를 남겨야하는지 고민 kyg 
		//kyg 만약 청약철회전용 아이템이 따로 만들어진다면.. ini파일 셋팅이 필요함 
		if( !g_GoodsMgr.GetGoodsInfoSubcription( kBuyInfo.m_iPresentType, kBuyInfo.m_iBuyValue1, kBuyInfo.m_iBuyValue2, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "%s Goods info Error(SubsCription):%s:%s[%d:%d:%d]", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iPresentType, kBuyInfo.m_iBuyValue1, kBuyInfo.m_iBuyValue2 );
			bError = true;
		}
	}
	else if( iType == OUTPUT_CASH_POPUP )
	{
		if( !g_GoodsMgr.GetGoodsInfoPopup( kBuyInfo.m_iPresentType, kBuyInfo.m_iBuyValue3, dwGoodsNo, szGoodsName ) )
		{
			LOG.PrintTimeAndLog(0, "[popup]%s Goods info Error(Present):%s:%s[%d:%d:%d]", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), kBuyInfo.m_iPresentType, kBuyInfo.m_iBuyValue1, kBuyInfo.m_iBuyValue2 );
			bError = true;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Type is %d:%s:%d:%s:%s:%s:%d", __FUNCTION__, iType, szBillingGUID.c_str(), dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szUserIP.c_str(), iPayAmt  );
		bError = true;
	}

	// 한국 테스트일때만 에러 없이 처리
	if( g_App.IsTestMode())
	{
		{
			bError = false;
			rkPacket.SetPosBegin();
			SendTestOutputCash(pServerNode,rkPacket);
			LOG.PrintTimeAndLog(0,"Private:%s Testmode SendTestOutputCash",szPrivateID.c_str());
			return;


		}
	}

	if( bError )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		return;
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		rkPacket.SetPosBegin(); // Pos to Begin.
		_OnOutputCash( pServerNode, rkPacket, dwGoodsNo, szGoodsName );
	}
	else
	{
		// Local이 channeling 안에 있다, 개념상으로는 동등하거나 상위이지만 에러 체크 함수를 channeling이 가지고 있어 내부에 추가함.
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			rkPacket.SetPosBegin(); // Pos to Begin.
			pLocal->_OnOutputCash( pServerNode, rkPacket, dwGoodsNo, szGoodsName );
		}
	}
}

void ioChannelingNodeParent::OnPCRoom( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int            iChannelingType = 0;
	ioHashString   szBillingGUID;
	DWORD          dwUserIndex     = 0;
	ioHashString   szPrivateID;
	ioHashString   szPublicID;
	ioHashString   szPublicIP;
	int            iPort           = 0;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> szPublicIP >> iPort;  // 공통 사항

	if( szBillingGUID.IsEmpty() || dwUserIndex == 0 || szPrivateID.IsEmpty() || szPublicID.IsEmpty() || szPublicIP.IsEmpty() || iPort == 0 )
	{
		LOG.PrintTimeAndLog(0, "%s Received Data is Wrong:%s:%d:%s:%s:%s:%d", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str(), iPort );
		return;
	}

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s:%s:%d", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str(), iPort );
		return;
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		rkPacket.SetPosBegin(); // Pos to Begin.
		_OnPCRoom( pServerNode, rkPacket );
	}
	else
	{
		// Local이 channeling 안에 있다, 개념상으로는 동등하거나 상위이지만 에러 체크 함수를 channeling이 가지고 있어 내부에 추가함.
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			rkPacket.SetPosBegin(); // Pos to Begin.
			//pLocal->_OnPCRoom( pServerNode, rkPacket );
		}
	}
}

void ioChannelingNodeParent::OnQuestComplete(ServerNode *pServerNode,  SP2Packet &rkPacket )
{
	int            iChannelingType = 0;
	ioHashString   szBillingGUID;
	DWORD          dwUserIndex     = 0;
	ioHashString   szPrivateID;
	ioHashString   szPublicID;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID;

	if( szBillingGUID.IsEmpty() || dwUserIndex == 0 || szPrivateID.IsEmpty() || szPublicID.IsEmpty() )
	{
		LOG.PrintTimeAndLog(0, "%s Received Data is Wrong:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}
}

void ioChannelingNodeParent::GetItemInfo( IN SP2Packet &rkPacket, IN int iType, OUT ItemInfo &rkBuyInfo )
{
	if( iType == OUTPUT_CASH_SOLDIER )
	{
		rkPacket >> rkBuyInfo.m_iClassType;
		rkPacket >> rkBuyInfo.m_iKindred;
		rkPacket >> rkBuyInfo.m_iSex;
		rkPacket >> rkBuyInfo.m_iLimitSecond;
		rkPacket >> rkBuyInfo.m_iPeriodType;
	}
	else if( iType == OUTPUT_CASH_DECO )
	{
		rkPacket >> rkBuyInfo.m_iDecoType;
		rkPacket >> rkBuyInfo.m_iDecoCode;
		rkPacket >> rkBuyInfo.m_iCharArray;
	}
	else if( iType == OUTPUT_CASH_SOLDIER_EXTEND )
	{
		rkPacket >> rkBuyInfo.m_iCharArray;
		rkPacket >> rkBuyInfo.m_iClassType;
		rkPacket >> rkBuyInfo.m_iLimitSecond;
	}
	else if( iType == OUTPUT_CASH_ETC )
	{
		rkPacket >> rkBuyInfo.m_dwType;
		rkPacket >> rkBuyInfo.m_iArray;
	}
	else if( iType == OUTPUT_CASH_SOLDIER_CHANGE_PERIOD )
	{
		rkPacket >> rkBuyInfo.m_iCharArray;
		rkPacket >> rkBuyInfo.m_iClassType;
	}
	else if( iType == OUTPUT_CASH_EXTRA )
	{
		rkPacket >> rkBuyInfo.m_iMachineCode;
	}
	else if( iType == OUTPUT_CASH_PRESENT )
	{
		rkPacket >> rkBuyInfo.m_iPresentType;
		rkPacket >> rkBuyInfo.m_iBuyValue1;
		rkPacket >> rkBuyInfo.m_iBuyValue2;
		rkPacket >> rkBuyInfo.m_dwRecvUserIndex;
	}
	else if( iType == OUTPUT_CASH_SUBSCRIPTION )
	{
		rkPacket >> rkBuyInfo.m_iPresentType;
		rkPacket >> rkBuyInfo.m_iBuyValue1;
		rkPacket >> rkBuyInfo.m_iBuyValue2;
	}
	else if( iType == OUTPUT_CASH_POPUP )
	{
		rkPacket >> rkBuyInfo.m_iPresentType;
		rkPacket >> rkBuyInfo.m_iBuyValue1;
		rkPacket >> rkBuyInfo.m_iBuyValue2;
		rkPacket >> rkBuyInfo.m_iBuyValue3;	// popupindex로 사용함..
	}
}

void ioChannelingNodeParent::SetItemInfo( OUT SP2Packet &rkPacket, IN int iType, IN const ItemInfo &rkBuyInfo )
{
	if( iType == OUTPUT_CASH_SOLDIER )
	{
		rkPacket << rkBuyInfo.m_iClassType;
		rkPacket << rkBuyInfo.m_iKindred;
		rkPacket << rkBuyInfo.m_iSex;
		rkPacket << rkBuyInfo.m_iLimitSecond;
		rkPacket << rkBuyInfo.m_iPeriodType;
	}
	else if( iType == OUTPUT_CASH_DECO )
	{
		rkPacket << rkBuyInfo.m_iDecoType;
		rkPacket << rkBuyInfo.m_iDecoCode;
		rkPacket << rkBuyInfo.m_iCharArray;
	}
	else if( iType == OUTPUT_CASH_SOLDIER_EXTEND )
	{
		rkPacket << rkBuyInfo.m_iCharArray;
		rkPacket << rkBuyInfo.m_iClassType;
		rkPacket << rkBuyInfo.m_iLimitSecond;
	}
	else if( iType == OUTPUT_CASH_ETC )
	{
		rkPacket << rkBuyInfo.m_dwType;
		rkPacket << rkBuyInfo.m_iArray;
	}
	else if( iType == OUTPUT_CASH_SOLDIER_CHANGE_PERIOD )
	{
		rkPacket << rkBuyInfo.m_iCharArray;
		rkPacket << rkBuyInfo.m_iClassType;
	}
	else if( iType == OUTPUT_CASH_EXTRA )
	{
		rkPacket << rkBuyInfo.m_iMachineCode;
	}
	else if( iType == OUTPUT_CASH_PRESENT )
	{
		rkPacket << rkBuyInfo.m_iPresentType;
		rkPacket << rkBuyInfo.m_iBuyValue1;
		rkPacket << rkBuyInfo.m_iBuyValue2;
		rkPacket << rkBuyInfo.m_dwRecvUserIndex;
	}
	else if( iType == OUTPUT_CASH_SUBSCRIPTION )
	{
 		rkPacket << rkBuyInfo.m_iPresentType;
 		rkPacket << rkBuyInfo.m_iBuyValue1;
 		rkPacket << rkBuyInfo.m_iBuyValue2;
	}
	else if( iType == OUTPUT_CASH_POPUP )
	{
		rkPacket << rkBuyInfo.m_iPresentType;
		rkPacket << rkBuyInfo.m_iBuyValue1;
		rkPacket << rkBuyInfo.m_iBuyValue2;
		rkPacket << rkBuyInfo.m_iBuyValue3;		
	}
}

void ioChannelingNodeParent::GetItemValueList( IN SP2Packet &rkPacket, IN int iType, OUT int iItemValueList[MAX_ITEM_VALUE] )
{
	if( iType == OUTPUT_CASH_SOLDIER )
	{
		rkPacket >> iItemValueList[0];
		rkPacket >> iItemValueList[1];
		rkPacket >> iItemValueList[2];
		rkPacket >> iItemValueList[3];
		rkPacket >> iItemValueList[4];
	}
	else if( iType == OUTPUT_CASH_DECO )
	{
		rkPacket >> iItemValueList[0];
		rkPacket >> iItemValueList[1];
		rkPacket >> iItemValueList[2];
	}
	else if( iType == OUTPUT_CASH_SOLDIER_EXTEND )
	{
		rkPacket >> iItemValueList[0];
		rkPacket >> iItemValueList[1];
		rkPacket >> iItemValueList[2];
	}
	else if( iType == OUTPUT_CASH_ETC )
	{
		rkPacket >> iItemValueList[0];
		rkPacket >> iItemValueList[1];
	}
	else if( iType == OUTPUT_CASH_SOLDIER_CHANGE_PERIOD )
	{
		rkPacket >> iItemValueList[0];
		rkPacket >> iItemValueList[1];
	}
	else if( iType == OUTPUT_CASH_EXTRA )
	{
		rkPacket >> iItemValueList[0];
	}
	else if( iType == OUTPUT_CASH_PRESENT )
	{
		short iTemp = 0;
		rkPacket >> iTemp;
		iItemValueList[0] = iTemp;
		rkPacket >> iItemValueList[1];
		rkPacket >> iItemValueList[2];
		rkPacket >> iItemValueList[3];
	}
	else if( iType == OUTPUT_CASH_SUBSCRIPTION )
	{
		short iTemp = 0;
		rkPacket >> iTemp;
		iItemValueList[0] = iTemp;
		rkPacket >> iItemValueList[1];
		rkPacket >> iItemValueList[2];
	}
	else if( iType == OUTPUT_CASH_POPUP )
	{
		short iTemp = 0;
		rkPacket >> iTemp;
		iItemValueList[0] = iTemp;
		rkPacket >> iItemValueList[1];
		rkPacket >> iItemValueList[2];
		rkPacket >> iItemValueList[3];
	}
}

void ioChannelingNodeParent::SetItemValueList( OUT SP2Packet &rkPacket, IN int iType, IN const int iItemValueList[MAX_ITEM_VALUE] )
{
	if( iType == OUTPUT_CASH_SOLDIER )
	{
		rkPacket << (int)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
		rkPacket << (int)iItemValueList[2];
		rkPacket << (int)iItemValueList[3];
		rkPacket << (int)iItemValueList[4];
	}
	else if( iType == OUTPUT_CASH_DECO )
	{
		rkPacket << (int)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
		rkPacket << (int)iItemValueList[2];
	}
	else if( iType == OUTPUT_CASH_SOLDIER_EXTEND )
	{
		rkPacket << (int)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
		rkPacket << (int)iItemValueList[2];
	}
	else if( iType == OUTPUT_CASH_ETC )
	{
		rkPacket << (DWORD)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
	}
	else if( iType == OUTPUT_CASH_SOLDIER_CHANGE_PERIOD )
	{
		rkPacket << (int)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
	}
	else if( iType == OUTPUT_CASH_EXTRA )
	{
		rkPacket << (int)iItemValueList[0];
	}
	else if( iType == OUTPUT_CASH_PRESENT )
	{
		rkPacket << (short)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
		rkPacket << (int)iItemValueList[2];
		rkPacket << (DWORD)iItemValueList[3];
	}
	else if( iType == OUTPUT_CASH_SUBSCRIPTION )
	{
		rkPacket << (short)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
		rkPacket << (int)iItemValueList[2];
	}
	else if( iType == OUTPUT_CASH_POPUP )
	{
		rkPacket << (short)iItemValueList[0];
		rkPacket << (int)iItemValueList[1];
		rkPacket << (int)iItemValueList[2];
		rkPacket << (int)iItemValueList[3];
	}
}

void ioChannelingNodeParent::OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szUserIdentify;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> dwIndex >> szChargeNo >> szUserIdentify;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

	// error check
	if( szBillingGUID.IsEmpty() || dwUserIndex == 0 || szPublicID.IsEmpty() || szPrivateID.IsEmpty() || szUserIdentify.IsEmpty() || szChargeNo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Received Data is Wrong:%s:%d:%s:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szUserIdentify.c_str(), szChargeNo.c_str() );

		if( dwUserIndex != 0 && !szBillingGUID.IsEmpty() )
		{
			SendSubscriptionRetractErrorPacket(pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID);
		}
		return;
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		rkPacket.SetPosBegin(); // Pos to Begin.
		_OnSubscriptionRetractCheck( pServerNode, rkPacket );
	}
	else
	{
		// Local이 channeling 안에 있다, 개념상으로는 동등하거나 상위이지만 에러 체크 함수를 channeling이 가지고 있어 내부에 추가함. //아직 추가 안함 해외는. kyg 130701
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			rkPacket.SetPosBegin(); // Pos to Begin.
			//pLocal->_
		}
	}
}

void ioChannelingNodeParent::OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szUserIdentify;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> dwIndex >> szChargeNo >> szUserIdentify;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

	// error check
	if( szBillingGUID.IsEmpty() || dwUserIndex == 0 || szPublicID.IsEmpty() || szPrivateID.IsEmpty() || szUserIdentify.IsEmpty() || szChargeNo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Received Data is Wrong:%s:%d:%s:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szUserIdentify.c_str(), szChargeNo.c_str() );

		if( dwUserIndex != 0 && !szBillingGUID.IsEmpty() )
		{
			SendSubscriptionRetractErrorPacket(pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT,dwUserIndex,szBillingGUID);
		}
		return;
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		rkPacket.SetPosBegin(); // Pos to Begin.
		_OnSubscriptionRetract( pServerNode, rkPacket );
	}
	else
	{
		// Local이 channeling 안에 있다, 개념상으로는 동등하거나 상위이지만 에러 체크 함수를 channeling이 가지고 있어 내부에 추가함. //아직 추가 안함 해외는. kyg 130701
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			rkPacket.SetPosBegin(); // Pos to Begin.
			//pLocal->_
		}
	}
}

void ioChannelingNodeParent::OnSubscriptionRetractFail( ServerNode* pServerNode, SP2Packet& rkPakcet )
{

}

void ioChannelingNodeParent::SendSubscriptionRetractErrorPacket( ServerNode* pServerNode, unsigned int errorId,const DWORD dwUserIndex, const ioHashString& szBillingGUID )
{
	if(pServerNode)
	{
		ioHashString szError = "Error";
		SP2Packet rkPacket( errorId );

		rkPacket << dwUserIndex;
		rkPacket << szBillingGUID;
		rkPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		rkPacket << (DWORD) 0;
		rkPacket << szError;
		rkPacket << -1;

		pServerNode->SendMessage(rkPacket);
	}
}

void ioChannelingNodeParent::_OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket )
{
	int iChannelingType = 0;
	rkPacket >> iChannelingType;
	LOG.PrintTimeAndLog(0,"[Channeling :%d] %s Not Supported  ",iChannelingType,__FUNCTION__);
}

void ioChannelingNodeParent::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket )
{
	int iChannelingType = 0;
	rkPacket >> iChannelingType;
	LOG.PrintTimeAndLog(0,"[Channeling :%d] %s Not Supported  ",iChannelingType,__FUNCTION__);
}

void ioChannelingNodeParent::_OnSubscriptionRetractFail( ServerNode* pServerNode, SP2Packet& rkPacket )
{
	int iChannelingType = 0;
	rkPacket >> iChannelingType;
	LOG.PrintTimeAndLog(0,"[Channeling :%d] %s Not Supported  ",iChannelingType,__FUNCTION__);
}

void ioChannelingNodeParent::ThreadSubscriptionRetract( const ioData &rData )
{
	LOG.PrintTimeAndLog(0,"[Channeling: %d] %s Not Supported",rData.GetChannelingType(),__FUNCTION__);
}

void ioChannelingNodeParent::ThreadSubscriptionRetractCheck( const ioData &rData )
{
	LOG.PrintTimeAndLog(0,"[Channeling: %d] %s Not Supported",rData.GetChannelingType(),__FUNCTION__);
}

void ioChannelingNodeParent::SendTestGetCash( ServerNode *pServerNode,SP2Packet& rkPacket)
{
	if(g_App.IsTestMode())
	{
		if(pServerNode ==NULL)
			return;

		rkPacket.SetPosBegin();
		int            iChannelingType = 0;
		ioHashString   szBillingGUID;
		DWORD          dwUserIndex     = 0;
		ioHashString   szPrivateID;
		ioHashString   szPublicID;

		rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID;

		int iReturnCash    = 2000000;
		int iPurchasedCash = 2000000;

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << true;			//Mouse Busy -> false 에서 변경
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iReturnCash;
		kPacket << iPurchasedCash;
		pServerNode->SendMessage(kPacket);

	}

}

void ioChannelingNodeParent::SendTestOutputCash( ServerNode *pServerNode, SP2Packet& rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	int          iReturnItemPrice = 0;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int			 iBonusCashSize	= 0;

	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iBonusCashSize;

	for( int i = 0; i < iBonusCashSize; i++ )
	{
		int iIndex		= 0;
		int iBonusCash	= 0;

		rkPacket >> iIndex >> iBonusCash;
	}

	rkPacket >> iType; // 공통사항	

	
	int iReturnCash    =2000000;
	int iPurchasedCash = 2000000;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnItemPrice;
	kPacket << iType;
	kPacket << iPayAmt;
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << szUserIP;

	ItemInfo kItemInfo;
	ItemInfo kBuyInfo;
	GetItemInfo( rkPacket, iType, kBuyInfo );
 
	SetItemInfo( kPacket, iType, kBuyInfo );

	kPacket << iChannelingType;  // 공통
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;

	// Cancel Step 1
	static TwoOfINTVec vInfo;
	vInfo.clear();
	int iiBonusCashSize	= vInfo.size();

	kPacket << iiBonusCashSize	;

	for( int i = 0; i < iiBonusCashSize	; i++ )
	{
		kPacket << vInfo[i].value1 << vInfo[i].value2;
	}

	kPacket << szPrivateID;
	kPacket << szUserIP;
	kPacket << 0;

	//kPacket << iChannelingType;  // 공통
	//kPacket << iReturnCash;
	//kPacket << iPurchasedCash;
	//kPacket << 0;
	
	if( pServerNode->SendMessage(kPacket) )
	{
		//	LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
	
		//	m_BillInfoMgr.Delete( rkResult.UserID  );
		return;
	}

}

void ioChannelingNodeParent::MultiToUni(const char* multiStr, wchar_t* uniStr, int maxLen)
{
	int len = MultiByteToWideChar(	CP_ACP,
                                    0,
									multiStr,
                                    strlen(multiStr),
                                    uniStr,
                                    maxLen);
   if(len < 0)
	   len = 0;

   if(len < maxLen)
	   uniStr[len] = NULL;
   else
	   uniStr[0] = NULL;

}

void ioChannelingNodeParent::UniToUTF8(const wchar_t* uniStr, char* utf8String, int maxLen)
{
	int len = WideCharToMultiByte(	CP_UTF8,
                                    0,
									uniStr,
                                    lstrlenW(uniStr),
                                    utf8String,
                                    maxLen,
									NULL,
									NULL);
   if(len < 0)
	   len = 0;

   if(len < maxLen)
	   utf8String[len] = NULL;
   else
	   utf8String[0] = NULL;
}

void ioChannelingNodeParent::MultiToUTF8(IN const char* szMultiStr, OUT char* szUTF8Str)
{
	if( !szMultiStr || !szUTF8Str )
		return;

	wchar_t szUnicodeStr[USER_ID_NUM] = {0,};
	MultiToUni(szMultiStr,szUnicodeStr, USER_ID_NUM);
	UniToUTF8(szUnicodeStr,szUTF8Str,USER_ID_NUM);
}