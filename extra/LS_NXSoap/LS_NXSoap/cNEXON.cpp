#include "stdafx.h"
#include "cNEXON.h"
#include "Nexon.h"
#include "MD5.h"
#include "import/soapNexonCashSoapProxy.h"
#include "import/gsoap/NexonCashSoap.nsmap"

using namespace std;


NexonCashSoapProxy g_proxy;

cNEXON::cNEXON(void)
{
	Init();
}

cNEXON::~cNEXON(void)
{
	Destroy();
}

void cNEXON::Init()
{
}

void cNEXON::Destroy()
{
	DestroyHeader();
}

BOOL cNEXON::InitializeSoap(
	const char* TS, 
	const char* IP, 
	const char* KEY,
	int& error)
{
	error = 0;
	return InitializeSoapSEH( TS, IP, KEY, error );
	//__try
	//{
	//	return InitializeSoapSEH( TS, IP, KEY, error );
	//}
	//__except(1)
	//{
	//	error = 3;
	//	return FALSE;
	//}
	//error = 4;
	//return FALSE;
}

 BOOL cNEXON::InitializeSoapSEH(
	const char* TS, 
	const char* IP, 
	const char* KEY,
	int& error)
{
	error = 0;

	if(CreateHeader())
	{
		GenerateHeader(TS, IP, KEY);

		_ns1__InitializeSoap request;
		_ns1__InitializeSoapResponse response;

		int ret = g_proxy.InitializeSoap(&request, &response);
		if(SOAP_OK == ret)
			return TRUE;

		error = 2;
		return FALSE;
	}
	error = 1;
	return FALSE;
}

BOOL cNEXON::IsBillingUser(
		const char* ID,
		BYTE reason,
		const char* IP,
		int& error,
		time_t& lastDate)
{
	_ns1__ISBillingUser request;
	_ns1__ISBillingUserResponse response;

	m_isbillinguser.ID		= ID;
	m_isbillinguser.IP		= IP;
	m_isbillinguser.reason	= reason;

	request.nexonID			= &m_isbillinguser.ID;
	request.actionUserIP	= &m_isbillinguser.IP;
	request.reason			= reason;

	int ret = g_proxy.ISBillingUser(&request, &response);
	if(SOAP_OK == ret)
	{
		// 호출성공
		error		= response.errorCode;
		lastDate	= response.dtLastPaymentDate;
		return TRUE;
	}

	return FALSE;
}

 BOOL cNEXON::GetPurse(
	const char* ID, 
	const BYTE reason, 
	const char* IP, 
	int& error, 
	INT64& balance)
{
	_ns1__GetPurse request;
	_ns1__GetPurseResponse response;

	m_getpurse.ID		= ID;
	m_getpurse.IP		= IP;
	m_getpurse.reason	= reason;

	request.nexonID			= &m_getpurse.ID;
	request.actionUserIP	= &m_getpurse.IP;
	request.reason			= reason;

	int ret = g_proxy.GetPurse(&request, &response);
	if(SOAP_OK == ret)
	{
		// 호출성공
		error	= response.errorCode;
		balance = _atoi64(response.GetPurseResult.c_str());
		return TRUE;
	}

	return FALSE;
}
	
BOOL cNEXON::Purchase(
	const char* ID, 
	const char* transactionID, 
	short productType, 
	const char* productCode, 
	const char* amount,
	const char* gameID, 
	const char* serverID, 
	const char* orderNO, 
	BYTE reason, 
	const char* IP, 
	int& error,
	BOOL& result)
{
	_ns1__Purchase request;
	_ns1__PurchaseResponse response;
	
	m_purchase.ID				= ID;
	m_purchase.transactionID	= transactionID;
	m_purchase.productType		= productType;
	m_purchase.productCode		= productCode;
	m_purchase.amount			= amount;
	m_purchase.gameID			= gameID;
	m_purchase.serverID			= serverID;
	m_purchase.orderNO			= orderNO;
	m_purchase.reason			= reason;
	m_purchase.IP				= IP;

	request.nexonID			= &m_purchase.ID;
	request.transactionID	= &m_purchase.transactionID;
	request.productType		= productType;
	request.productCode		= &m_purchase.productCode;
	request.amount			= m_purchase.amount;
	request.gameID			= &m_purchase.gameID;
	request.data2			= &m_purchase.serverID;
	request.data3			= &m_purchase.orderNO;
	request.reason			= reason;
	request.actionUserIP	= &m_purchase.IP;

	int ret = g_proxy.Purchase(&request, &response);
	if(SOAP_OK == ret)
	{
		// 호출성공
		error	= response.errorCode;
		result	= response.PurchaseResult ? TRUE : FALSE;
		return TRUE;
	}
	return FALSE;
}


BOOL cNEXON::Present(
	const char* ID, 
	const char* transactionID, 
	short productType, 
	const char* productCode, 
	const char* amount,
	const char *gameID, 
	const char *friendID, 
	BYTE age,
	const char* serverID, 
	const char* orderNO, 
	BYTE reason, 
	const char* IP, 
	int& error,
	BOOL& result)
{
	_ns1__Present request;
	_ns1__PresentResponse response;

	m_present.ID				= ID;
	m_present.transactionID		= transactionID;
	m_present.productType		= productType;
	m_present.productCode		= productCode;
	m_present.amount			= amount;
	m_present.gameID			= gameID;
	m_present.friendID			= friendID;
	m_present.serverID			= serverID;
	m_present.orderNO			= orderNO;
	m_present.reason			= reason;
	m_present.IP				= IP;

	request.nexonID			= &m_present.ID;
	request.transactionID	= &m_present.transactionID;
	request.productType		= productType;
	request.productCode		= &m_present.productCode;
	request.amount			= m_present.amount;
	request.gameID			= &m_present.gameID;
	request.giftGameID		= &m_present.friendID;
	request.age				= age;
	request.data2			= &m_present.serverID;
	request.data3			= &m_present.orderNO;
	request.reason			= reason;
	request.actionUserIP	= &m_present.IP;

	int ret = g_proxy.Present(&request, &response);
	if(SOAP_OK == ret)
	{
		// 호출성공
		error	= response.errorCode;
		result	= response.PresentResult ? TRUE : FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL cNEXON::UsageCancel(
	const char* ID, 
	const char* transactionID, 
	short productType, 
	int amount,
	int& result)
{
	_ns1__UsageCancel request;
	_ns1__UsageCancelResponse response;

	m_usagecancel.ID				= ID;
	m_usagecancel.transactionID		= transactionID;
	m_usagecancel.productType		= productType;
	m_usagecancel.amount			= amount;

	request.nexonID			= &m_usagecancel.ID;
	request.transactionID	= &m_usagecancel.transactionID;
	request.productType		= m_usagecancel.productType;
	request.refundAmount	= amount;

	int ret = g_proxy.UsageCancel(&request, &response);
	if(SOAP_OK == ret)
	{
		// 호출성공
		result = response.UsageCancelResult;
		return TRUE;
	}

	return FALSE;
}

BOOL cNEXON::UsageCancelByUsageSN(
		const char* ID,
		int usageSN,
		int refundAmount,
		int& result)
{
	_ns1__UsageCancelByUsageSn request;
	_ns1__UsageCancelByUsageSnResponse response;

	m_usagecancelbyusagesn.ID = ID;

	request.id				= &m_usagecancelbyusagesn.ID;
	request.usageSn			= usageSN;
	request.refundAmount	= refundAmount;

	int ret = g_proxy.UsageCancelByUsageSn(&request, &response);
	if(SOAP_OK == ret)
	{
		// 호출성공
		result = response.UsageCancelByUsageSnResult;
		return TRUE;
	}

	return FALSE;
}


void cNEXON::GetError(char* buffer, int length)
{
	ZeroMemory(buffer, length);
	
	length = (length > ((*(soap*)(&g_proxy))).buflen) ? ((*(soap*)(&g_proxy))).buflen : length;
	CopyMemory(buffer, ((*(soap*)(&g_proxy))).buf, length);
}

//-------------------------------------------------------------------------------------------------
// protected operations
//-------------------------------------------------------------------------------------------------

BOOL cNEXON::CreateHeader()
{
	// create soap header
	if(!g_proxy.header)
	{
		g_proxy.header = SOAP_NEW(struct SOAP_ENV__Header);
		g_proxy.header->ns1__Header_			= SOAP_NEW(ns1__Header);
		g_proxy.header->ns1__SecurityHeader_	= SOAP_NEW(ns1__SecurityHeader);
		return TRUE;
	}
	else
	{
		if(g_proxy.header->ns1__Header_)
		{
			SOAP_DELETE(g_proxy.header->ns1__Header_);
		}
		if(g_proxy.header->ns1__SecurityHeader_)
		{
			SOAP_DELETE(g_proxy.header->ns1__SecurityHeader_);
		}

		SOAP_DELETE(g_proxy.header);

		g_proxy.header = SOAP_NEW(struct SOAP_ENV__Header);
		g_proxy.header->ns1__Header_			= SOAP_NEW(ns1__Header);
		g_proxy.header->ns1__SecurityHeader_	= SOAP_NEW(ns1__SecurityHeader);
	}
	return TRUE;
}

void cNEXON::DestroyHeader()
{
	// SOAP API를 호출하면 내부에서 자동으로 삭제함

	//if(g_proxy.header->ns1__Header_)
	//{
	//	SOAP_DELETE(g_proxy.header->ns1__Header_);
	//	g_proxy.header->ns1__Header_ = NULL;
	//}
	//if(g_proxy.header->ns1__SecurityHeader_)
	//{
	//	SOAP_DELETE(g_proxy.header->ns1__SecurityHeader_);
	//	g_proxy.header->ns1__SecurityHeader_ = NULL;
	//}
	//if(g_proxy.header)
	//{
	//	SOAP_DELETE(g_proxy.header);
	//	g_proxy.header = NULL;
	//}

}

void cNEXON::GenerateHeader(const char* TS, const char* IP, const char* hashKey)
{
	// TS			현재시간(yyyyMMddHHmmss)
	// IPAddress	유저IP
	// MD5Hash		IP + HashKey

	m_TS	= TS;
	m_IP	= IP;

	std::string key = m_IP + hashKey;

	CMD5 MD5;
	MD5.Generate((BYTE*)key.c_str(), key.size());

	m_Hash	= MD5.GetDigestHEX();

	g_proxy.header->ns1__Header_->TS			= &m_TS;
	g_proxy.header->ns1__Header_->IPAddress	= &m_IP;
	g_proxy.header->ns1__Header_->MD5Hash		= &m_Hash;
}


