#include "stdafx.h"
#include ".\iochannelingnodeparent.h"
#include "../Local/ioLocalManager.h"

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
		PACKET_GUARD_VOID( rkPacket.Read(iTemp) );
		iItemValueList[0] = iTemp;
		PACKET_GUARD_VOID( rkPacket.Read(iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Read(iItemValueList[2]) );
		PACKET_GUARD_VOID( rkPacket.Read(iItemValueList[3]) );
	}
}

void ioChannelingNodeParent::SetItemValueList( OUT SP2Packet &rkPacket, IN int iType, IN const int iItemValueList[MAX_ITEM_VALUE] )
{
	if( iType == OUTPUT_CASH_SOLDIER )
	{
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[2]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[3]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[4]) );
	}
	else if( iType == OUTPUT_CASH_DECO )
	{
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[2]) );
	}
	else if( iType == OUTPUT_CASH_SOLDIER_EXTEND )
	{
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[2]) );
	}
	else if( iType == OUTPUT_CASH_ETC )
	{
		PACKET_GUARD_VOID( rkPacket.Write((DWORD)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
	}
	else if( iType == OUTPUT_CASH_SOLDIER_CHANGE_PERIOD )
	{
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
	}
	else if( iType == OUTPUT_CASH_EXTRA )
	{
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[0]) );
	}
	else if( iType == OUTPUT_CASH_PRESENT )
	{
		PACKET_GUARD_VOID( rkPacket.Write((short)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[2]) );
		PACKET_GUARD_VOID( rkPacket.Write((DWORD)iItemValueList[3]) );
	}
	else if( iType == OUTPUT_CASH_SUBSCRIPTION )
	{
		PACKET_GUARD_VOID( rkPacket.Write((short)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[2]) );
	}
	else if( iType == OUTPUT_CASH_POPUP )
	{
		PACKET_GUARD_VOID( rkPacket.Write((short)iItemValueList[0]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[1]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[2]) );
		PACKET_GUARD_VOID( rkPacket.Write((int)iItemValueList[3]) );
	}
}

bool ioChannelingNodeParent::UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	// FALSE 이면 ioLocalParent::UpdateOutputCash() 사용
	return false;
}

bool ioChannelingNodeParent::AddReuestSubscriptionRetractCashCheckPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{
	LOG.PrintTimeAndLog(0,"[Channeling :%d][%s] %s Not Supported  ",pUser->GetChannelingType(),pUser->GetPublicID(),__FUNCTION__);
	return false;
}

bool ioChannelingNodeParent::OnReceiveSubscriptionRetractCashCheck( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	LOG.PrintTimeAndLog(0,"[Channeling :%d][%s] %s Not Supported  ",pUser->GetChannelingType(),pUser->GetPublicID(),__FUNCTION__);
	return false;
}

bool ioChannelingNodeParent::AddReuestSubscriptionRetractCashPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{
	LOG.PrintTimeAndLog(0,"[Channeling :%d][%s] %s Not Supported  ",pUser->GetChannelingType(),pUser->GetPublicID(),__FUNCTION__);
	return false;
}

bool ioChannelingNodeParent::OnReceiveSubscriptionRetractCash( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	LOG.PrintTimeAndLog(0,"[Channeling :%d][%s] %s Not Supported  ",pUser->GetChannelingType(),pUser->GetPublicID(),__FUNCTION__);
	return false;
}

bool ioChannelingNodeParent::AddReuestSubscriptionRetractCashFailPacket( IN User *pUser, OUT SP2Packet & rkSendPacket )
{
	LOG.PrintTimeAndLog(0,"[Channeling :%d][%s] %s Not Supported  ",pUser->GetChannelingType(),pUser->GetPublicID(),__FUNCTION__);
	return false;
}

bool ioChannelingNodeParent::OnReceiveSubscriptionRetractCashFail( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	LOG.PrintTimeAndLog(0,"[Channeling :%d][%s] %s Not Supported  ",pUser->GetChannelingType(),pUser->GetPublicID(),__FUNCTION__);
	return false;
}
