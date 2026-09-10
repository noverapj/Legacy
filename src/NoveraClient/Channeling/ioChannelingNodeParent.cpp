#include "stdafx.h"
#include "..\SP2Packet.h"
#include ".\iochannelingnodeparent.h"


ioChannelingNodeParent::ioChannelingNodeParent(void)
{
}

ioChannelingNodeParent::~ioChannelingNodeParent(void)
{
}

bool ioChannelingNodeParent::IsBillingTestID( const ioHashString &rsPublicID )
{
	// 아래 "금벌", "금벌인" 아이디 수정시 반드시 대윤씨에게 문의해주세요.
	// 금벌 : 일반, 금벌인 : 엠게임, 실리콘베이브 : 네이버 (재우씨ID)
	if( rsPublicID == "금벌" || rsPublicID == "금벌인" || rsPublicID == "실리콘베이브" ) //Except Extracting Hangeul
		return true;

	return false;
}

const char * ioChannelingNodeParent::GetBillingErrorMent(ioHashString &sServerError)
{
	return "Billing Error : ";
}