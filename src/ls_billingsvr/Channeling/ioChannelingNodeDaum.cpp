#include "../stdafx.h"
#include "./iochannelingnodedaum.h"
#include "../NodeInfo/ServerNode.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"
#include "../Util/ioHashString.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MainProcess.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalParent.h"

extern CLog LOG;

ioChannelingNodeDaum::ioChannelingNodeDaum(void)
{
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Channeling" );

	kLoader.LoadString( "DaumGetURL", "", szTemp, sizeof( szTemp ) );
	m_sGetURL = szTemp;

	kLoader.LoadString( "DaumOutputURL", "", szTemp, sizeof( szTemp ) );
	m_sOutputURL = szTemp;

	kLoader.LoadString( "DaumCancelURL", "", szTemp, sizeof( szTemp ) );
	m_sSubscriptionRetractURL = szTemp;

	kLoader.LoadString( "DaumApikey", "", szTemp, sizeof( szTemp ) );
	m_sApikey = szTemp;
}

ioChannelingNodeDaum::~ioChannelingNodeDaum(void)
{
}

ChannelingType ioChannelingNodeDaum::GetType()
{
	return CNT_DAUM;
}

void ioChannelingNodeDaum::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szDaumNo;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szDaumNo; // daum용

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szDaumNo );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeDaum::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	int          iReturnItemPrice = 0;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int			 iBonusCashSize	= 0;

	ioData kData;

	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iBonusCashSize;
	
	for( int i = 0; i < iBonusCashSize; i++ )
	{
		int iIndex		= 0;
		int iBonusCash	= 0;

		rkPacket >> iIndex >> iBonusCash;

		kData.AddBonusCashInfoForConsume(iIndex, iBonusCash);
	}

	rkPacket >> iType; // 공통사항
	
	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	GetItemValueList( rkPacket, iType, iItemValueList );

	ioHashString szDaumNo;
	rkPacket >> szDaumNo; // daum 용 

	
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetExp( iReturnItemPrice );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserIP( szUserIP );
	kData.SetItemPayAmt( iPayAmt );
	kData.SetItemType( iType );
	kData.SetItemValueList( iItemValueList );
	kData.SetUserNo( szDaumNo );
	kData.SetGoodsNo( dwGoodsNo );
	kData.SetGoodsName( rszGoodsName );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_OUTPUT_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeDaum::ThreadGetCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		return;
	}

	long iReturnCash    = 0;
	long iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬
	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s/%s?apiKey=%s" , m_sGetURL.c_str(), rData.GetUserNo().c_str(), m_sApikey.c_str() );

	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	ioXMLDocument xmlDoc;
	//strcpy(szReturnValue,_T("<result>  <header>    <code>0000</code>    <message>정상처리 되었습니다.</message>    <resultCnt>1</resultCnt>    <comment>캐쉬잔액 조회 (권장)</comment>    <requestTime>2013-06-10T19:49:52.520</requestTime>    <responseTime>2013-06-10T19:49:52.530</responseTime>    <isNextData></isNextData>    <nextDataUrl></nextDataUrl>  </header>  <request>    <requestUrl>http://test.bill.daum.net/Alice/REST/lostsaga/Inquiry/CashBalance20100812/9lflB?apiKey=6b7d0746b3829ae8260b1aa4af095002ff5c20a2bdf8ada0fba17f16b7baf2c4856c8359bd33484b66c20865e7b19a01d86dc4ba922fcf0f175413e48cb2325a120b65ff8510b704b50ddf18</requestUrl>    <clientIp>112.218.160.51</clientIp>    <methodType>GET</methodType>    <requestParameters></requestParameters>  </request>  <response>    <itemCnt>1</itemCnt>    <items>      <item>        <cashAmt>0</cashAmt>        <contentsCashAmt>0</contentsCashAmt>        <actualCashAmt>0</actualCashAmt>      </item>    </items>  </response></result> "));

	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "[error][daum]%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	ioHashString sReturnCode;
	ioXMLElement xErrorChildElement = xRootElement.FirstChild( "header" );
	ioXMLElement xErrorGrandChildElement;
	if( !xErrorChildElement.IsEmpty() )
		xErrorGrandChildElement = xErrorChildElement.FirstChild( "code" );
	if( !xErrorGrandChildElement.IsEmpty() && xErrorGrandChildElement.GetText() != NULL )
		sReturnCode = xErrorGrandChildElement.GetText();

	if( sReturnCode != "0000" )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;

		ioHashString sError;
		xErrorGrandChildElement = xErrorChildElement.FirstChild( "message" );
		if( !xErrorGrandChildElement.IsEmpty() && xErrorGrandChildElement.GetText() != NULL )
			sError = xErrorGrandChildElement.GetText();

		char szAnsiError[MAX_PATH*2]="";
		int iReturnAnsiSize = 0;
		UTF8ToAnsi( sError.c_str(), szAnsiError, iReturnAnsiSize, sizeof( szAnsiError ) );

		if( !sError.IsEmpty() )
		{
			kPacket << true;
			kPacket << szAnsiError;
		}

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "%s xml error return. :%d:%s:%s:%s:%s:%d:%s", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), szAnsiError );
		LOG.PrintTimeAndLog(0, "%s Fail SendValue :%s", __FUNCTION__, szFullURL );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

/*
<result>
	<header> 
		<code> 응답코드 </code> 
		<message> 응답메세지 </message> 
		<resultCnt> 전체 데이터 건수 </resultCnt> 
		<comment> 해당 API 코멘트 </comment> 
		<requestTime> 처리요청시간 </requestTime> 
		<responseTime> 처리응답시간 </responseTime> 
		<isNextData> 다음 데이터 존재 여부 </isNextData> 
		<nextDataUrl> 다음 데이터 URL </nextDataUrl>
	</header> 
	<request> 
		<requestUrl> 요청한 URL </requestUrl> 
		<clientIp> 요청한 Client Ip 주소 </clientIp> 
		<methodType> 요청한 메소드 방식 </methodType> 
		<requestParameters> 요청한 파라메터 (POST 방식인 경우 출력됨) </requestParameters> 
	</request> 
	<response> 
		<itemCnt> 조회된 Item 건수 </itemCnt> 
		<items> 조회된 아이템들 
		<item> 
			<cashAmt> 캐쉬 금액 </cashAmt>
			<contentsCashAmt> 컨텐츠캐쉬 금액 </contentsCashAmt> 
			<actualCashAmt> 실물캐쉬 금액 </actualCashAmt>
		</item> 
		</items> 
	</response> 
</result>

	0000	 정상처리 되었습니다.	
	A002	 do not allow method	 해당 API를 사용하기 위해서는 올바른 Method(GET, POST, PUT, DELETE)를 사용하여 호출하셔야합니다.
	A003	 do not support url	 주소가 잘못되었습니다.
	A004	 invalid format	 파라미터의 포멧이 잘못된 경우
	A005	 *** is null	 파라미터값이 null인 경우
	A006	 해당 되는 서비스를 이용하실 수 없습니다.	 해당 API를 사용하기 위해서는 API등록 요청을 하셔야 합니다.
	A007	 해당 데이터가 없습니다.	 원하는 값이 DB에 없는 경우
	A008	 인증에 실패하였습니다.	 Sign값이 맞지 않는 경우
	A009	 DB 장애입니다.	 각종 DB관련 에러
	A100	 취소가 불가능합니다.	 취소 API 사용시 이미취소되었거나 기타등등의 이유로 취소가 불가능한 경우
	A101	 중복된 거래입니다.	 ClientSeq값으로 중복된 값이 들어온 경우
	A102	 Confirm 처리완료 예비거래 입니다.	confirm 완료된 예비거래에 대해 ConfirmSettlement 가 중복 요청된 경우
	A103	 결제 완료되지 않은 예비거래 입니다.	BI 결제가 완료되지 않은 예비거래에 대해 ConfirmSettlement 가 요청된 경우
	A104	 복합결제건이 아니므로 Confirm 처리할 수 없습니다.	복합결제 건이 아닌 예비거래에 대해 ConfirmComplexSettlement 가 요청된 경우
	A200	 존재하지 않는 상품 입니다.	 itemId 가 잘못된 경우
	A9**	 System error	 각종 시스템 에러로 해당 에러 발생시 문의 바랍니다.
	I010	 Daum캐쉬 자체한도초과입니다.	
	I012	 expired item	 해당 아이템이 유효기간이 만료된 경우
	I013	 해당 CP Item 한도초과입니다.	
	I014	 해당 CP 의 Item 이 아닙니다.	
	I015	 금액이 잘못되었습니다.	
	I017	 다음 캐쉬가 사용정지된 회원입니다.	
	I018	 보유 환금액 보다 환금 요청 금액이 클수 없습니다.	
	I019	 잘못된 payType 입니다.	
	I020	 잘못된 PERIOD(결제주기) 입니다.	
	I021	 잘못된 일자 입니다.	
	I100	 Cancel Error	 취소관련 에러입니다.
	I101	 Cancel Error : PG 취소가 불가능합니다.	
	I110	 무통장입금이 현재 미입금상태입니다.	
	I111	 금액이 상이합니다.	
	I112	 이미 환금정지된 고객입니다.	
	I113	 정지된 PG입니다.	
	I114	 잔액 부족입니다.	
	I115	 캐쉬타입이 잘못되었습니다.	 캐쉬 적립시, 해당 적립 캐쉬의 타입 오류
	I400	 소셜쇼핑 CP 만 등록가능합니다.	 광고주 상품 등록은 소셜쇼핑 CP (shoppinghow, shoppinghow2) 만 가능
	I401	 마커테 등록은 아직 지원하지 않습니다.	 상품 등록시 해당 상품의 광고주가 마케터인 경우 등록 불가
	I402	 이미 존재하는 상품ID 입니다	
	B003	 NO DATA FOUND	 없는 데이터를 조회한 경우
	B004	 이미 등록된 데이터 입니다	
	B005	 잘못된 일자입니다.	
	B006	 0건이 처리되었습니다. 등록은 1건만 처리되어야 합니다.	 insert 시 에러
	B007	 0건이 처리되었습니다. 수정은 1건만 처리되어야 합니다.	 update 시 에러
	B008	 0건이 삭제되었습니다. 삭제는 1건만 처리되어야 합니다.	 delete 시 에러
	B010	 허가되지 않은 PGCATE 코드값입니다	
	B011	 잔액부족입니다.	
	B012	 잔액정보를 가져오지 못했습니다.	
	B015	 Daum캐쉬 사용이 정지되었습니다.	
	B017	 정의 되지 않은 CASH TYPE 입니다	
	B018	 접근 허용 IP 가 아닙니다	
	B019	 등록완료된 API 가 아닙니다	
	B020	 금액이 상이합니다	
	B101	 apiKey는 2개까지만 발급가능합니다.	
	C101	 결제사의 시스템 점검으로 인하여 서비스를 이용할 수 없습니다.	
	C102	 결제사에서 NULL을 리턴하였습니다.	
	C121	 결제사와의 통신 오류입니다.	
	C122	 PG사의 라이브러리 에러입니다.	
	C125	 리턴 데이터가 잘못되었습니다.	
	C201	 결제사의 정보가 부족합니다.	
	C9**	 System error	 각종 시스템 에러로 해당 에러 발생시 문의 바랍니다.

*/

	// xml parsing
	ioXMLElement xChildElement = xRootElement.FirstChild( "response" );
	ioXMLElement xGrandChildElement;
	if( !xChildElement.IsEmpty() )
		xGrandChildElement = xChildElement.FirstChild( "items" );
	ioXMLElement xGreatGrandChildElement;
	if( !xGrandChildElement.IsEmpty() )
		xGreatGrandChildElement = xGrandChildElement.FirstChild( "item" );

	enum { MAX_LOOP = 1, };
	ioXMLElement xCurrentElement;
	if( !xGreatGrandChildElement.IsEmpty() )
	{
		char szCashTitles[MAX_LOOP][MAX_PATH]={ "cashAmt" };
		for (int i = 0; i < MAX_LOOP ; i++)
		{
			xCurrentElement = xGreatGrandChildElement.FirstChild( szCashTitles[i] );
			if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
			{
				iReturnCash += atoi( xCurrentElement.GetText() );	
			}
		}
	}

	// 캐쉬와 실재 구매한 캐쉬가 동일하다.
	iPurchasedCash = iReturnCash;

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetSetUserMouse();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		return;		
	}
	//[%d:%d]
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%s[%d:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), iReturnCash, iPurchasedCash  );
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
}

void ioChannelingNodeDaum::ThreadOutputCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	char ts[16]="";
	char nonce[41]="";
	char *sign = NULL;

	char szParameters[MAX_PATH*4]="";
	StringCbPrintf( szParameters, sizeof( szParameters ), "SettlementPayCashNormallostsaga%s%s10000", szAgencyNo, rData.GetUserNo().c_str() );

	try 
	{
		getNewTimeStamp(ts); //kyg 
		getNewNonce(nonce);
		sign = getSig( (char*)m_sApikey.c_str(), szParameters, ts, nonce);
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Daum Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	char szPostData[MAX_PATH*2]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "userId=%s&itemId=10000&amt=%d&memo=%d&sign=%s&ts=%s&nonce=%s&clientSeq=%s", rData.GetUserNo().c_str(), rData.GetItemPayAmt(), rData.GetGoodsNo(), sign, ts, nonce, szAgencyNo );
	free(sign); ////////////////////// 반드시 free 필요 DAUM 소스

	ioHTTP Winhttp; //kyg 확인 필요  Get방식 되는지 확인 해야함 
	if( !Winhttp.GetResultData( m_sOutputURL.c_str(), szPostData, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		LOG.PrintTimeAndLog(0, "[error][daum]%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	ioHashString sReturnCode;
	ioXMLElement xErrorChildElement = xRootElement.FirstChild( "header" );
	ioXMLElement xErrorGrandChildElement;
	if( !xErrorChildElement.IsEmpty() )
		xErrorGrandChildElement = xErrorChildElement.FirstChild( "code" );
	if( !xErrorGrandChildElement.IsEmpty() && xErrorGrandChildElement.GetText() != NULL )
		sReturnCode = xErrorGrandChildElement.GetText();

	if( sReturnCode != "0000" )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();

		ioHashString sError;
		xErrorGrandChildElement = xErrorChildElement.FirstChild( "message" );
		if( !xErrorGrandChildElement.IsEmpty() && xErrorGrandChildElement.GetText() != NULL )
			sError = xErrorGrandChildElement.GetText();

		char szAnsiError[MAX_PATH*2]="";
		int iReturnAnsiSize = 0;
		UTF8ToAnsi( sError.c_str(), szAnsiError, iReturnAnsiSize, sizeof( szAnsiError ) );

		if( !sError.IsEmpty() )
		{
			kPacket << true;
			kPacket << szAnsiError;
		}

		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		LOG.PrintTimeAndLog(0, "%s xml error return. :%d:%s:%s:%s:%s:%d[%d:%s:%d:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), szAnsiError );
		LOG.PrintTimeAndLog(0, "%s Fail SendValue :%s?%s", __FUNCTION__, m_sOutputURL.c_str(), szPostData );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}


/*
리턴 코드는 get cash와 동일
<result>
	<header> 
		<code> 응답코드 </code> 
		<message> 응답메세지 </message> 
		<resultCnt> 전체 데이터 건수 </resultCnt> 
		<comment> 해당 API 코멘트 </comment> 
		<requestTime> 처리요청시간 </requestTime> 
		<responseTime> 처리응답시간 </responseTime> 
		<isNextData> 다음 데이터 존재 여부 </isNextData> 
		<nextDataUrl> 다음 데이터 URL </nextDataUrl>
	</header> 
	<request> 
		<requestUrl> 요청한 URL </requestUrl> 
		<clientIp> 요청한 Client Ip 주소 </clientIp> 
		<methodType> 요청한 메소드 방식 </methodType> 
		<requestParameters> 요청한 파라메터 (POST 방식인 경우 출력됨) </requestParameters> 
	</request> 
	<response>
		<itemCnt> 아이템 건수 </itemCnt>
		<items>
			<item>
				<logSeq> 결제된 거래의 해당 거래번호 </logSeq>
				<usedAmt> 결제 금액 </usedAmt>
				<cashAmt> 결제후 남은 캐쉬 </cashAmt>
			</item>
		</items>
	</response>
</result>
*/

	// xml parsing
	int  iReturnCash = 0; 
	ioHashString szReturnBuyNO;

	ioXMLElement xChildElement = xRootElement.FirstChild( "response" );
	ioXMLElement xGrandChildElement;
	if( !xChildElement.IsEmpty() )
		xGrandChildElement = xChildElement.FirstChild( "items" );
	ioXMLElement xGreatGrandChildElement;
	if( !xGrandChildElement.IsEmpty() )
		xGreatGrandChildElement = xGrandChildElement.FirstChild( "item" );

	enum { MAX_LOOP = 1, };
	ioXMLElement xCurrentElement;
	if( !xGreatGrandChildElement.IsEmpty() )
	{
		char szCashTitles[MAX_LOOP][MAX_PATH]={ "cashAmt" };
		for (int i = 0; i < MAX_LOOP ; i++)
		{
			xCurrentElement = xGreatGrandChildElement.FirstChild( szCashTitles[i] );
			if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
				iReturnCash += atoi( xCurrentElement.GetText() );	
		}
	}

	// for log
	xCurrentElement = xGreatGrandChildElement.FirstChild( "logSeq" );
	if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
		szReturnBuyNO = xCurrentElement.GetText();	

	// 캐쉬와 실재 구매한 캐쉬가 동일하다.
	int iPurchasedCash = iReturnCash;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rData.GetExp();
	kPacket << rData.GetItemType();
	kPacket << rData.GetItemPayAmt();
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << szReturnBuyNO; // same szChargeNo
	
	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );
	kPacket << rData.GetChannelingType(); // 공통
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
    //kyg 일단.. 넣어주질 않음 szReturnBuyNO를 
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		return;	
	}
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d:%d:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), //PrivateID %s:%s:%s
		rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), iReturnCash, iPurchasedCash, szReturnBuyNO.c_str() );

#if(0) //kyg Test영역
	ioData testData;
	testData.SetBillingGUID(rData.GetBillingGUID());
	testData.SetUserIndex(rData.GetUserIndex());
	testData.SetChargeNo(szReturnBuyNO);
	testData.SetEmpty(false);
	testData.SetUserNo(rData.GetUserNo());
	ThreadSubscriptionRetract(testData);
#endif
}

void ioChannelingNodeDaum::ThreadSubscriptionRetract( const ioData& rData )
{
	ioHashString sError = "UnKnownError";

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;		
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	char ts[16]="";
	char nonce[41]="";
	char *sign = NULL;

	char szParameters[MAX_PATH*4]="";

	
	StringCbPrintf( szParameters, sizeof( szParameters ), "CancelCancellationlostsaga%s%s", szAgencyNo, rData.GetChargeNo().c_str() );

	try 
	{
		getNewTimeStamp(ts); //kyg 
		getNewNonce(nonce);
		sign = getSig( (char*)m_sApikey.c_str(), szParameters, ts, nonce);
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Daum Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"ThreadSubscriptionRetract::Billing exception daum." );

		return;
	}

	char szPostData[MAX_PATH*4]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "logSeq=%s&sign=%s&ts=%s&nonce=%s&clientSeq=%s",rData.GetChargeNo().c_str(), sign, ts, nonce, szAgencyNo );
	free(sign); ////////////////////// 반드시 free 필요 DAUM 소스

	ioHTTP Winhttp; //kyg 확인 필요  Get방식 되는지 확인 해야함 

	if( !Winhttp.GetResultData( m_sSubscriptionRetractURL.c_str(), szPostData, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s http fail %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"ThreadSubscriptionRetract::Billing exception daum." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		LOG.PrintTimeAndLog(0, "[error][daum]%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	ioHashString sReturnCode;
	ioXMLElement xErrorChildElement = xRootElement.FirstChild( "header" );
	ioXMLElement xErrorGrandChildElement;
	if( !xErrorChildElement.IsEmpty() )
		xErrorGrandChildElement = xErrorChildElement.FirstChild( "code" );
	if( !xErrorGrandChildElement.IsEmpty() && xErrorGrandChildElement.GetText() != NULL )
		sReturnCode = xErrorGrandChildElement.GetText();

	if( sReturnCode != "0000" )
	{
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;
				
		xErrorGrandChildElement = xErrorChildElement.FirstChild( "message" );
		if( !xErrorGrandChildElement.IsEmpty() && xErrorGrandChildElement.GetText() != NULL )
			sError = xErrorGrandChildElement.GetText();

		char szAnsiError[MAX_PATH*2]="";
		int iReturnAnsiSize = 0;
		UTF8ToAnsi( sError.c_str(), szAnsiError, iReturnAnsiSize, sizeof( szAnsiError ) );

		if( !sError.IsEmpty() )
		{
			kPacket << true;
			kPacket << szAnsiError;
		}

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		LOG.PrintTimeAndLog(0, "%s xml error return. :%d:%s:%s:%s:%s:%d[%d:%s:%d:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), szAnsiError );
		LOG.PrintTimeAndLog(0, "%s Fail SendValue :%s?%s", __FUNCTION__, m_sOutputURL.c_str(), szPostData );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}
/*
리턴 코드는 get cash와 동일
<response>
<itemCnt>1</itemCnt>
<items>
<item>
<cancelTypeCode>5</cancelTypeCode>
<logSeq>123</logSeq>
<cancelAmt>123</cancelAmt>
<remainAmt>0</remainAmt>
</item>
</items>
</response>
*/

	// xml parsing
	int cancelType = 0;
	ioHashString szLogSeq;
	int cancelAmt = 0;
	int remainAmt = 0;

	ioXMLElement xChildElement = xRootElement.FirstChild( "response" );
	ioXMLElement xGrandChildElement;
	if( !xChildElement.IsEmpty() )
		xGrandChildElement = xChildElement.FirstChild( "items" );
	ioXMLElement xGreatGrandChildElement;
	if( !xGrandChildElement.IsEmpty() )
		xGreatGrandChildElement = xGrandChildElement.FirstChild( "item" );

	enum { MAX_LOOP = 1, };
	ioXMLElement xCurrentElement;

	xCurrentElement = xGreatGrandChildElement.FirstChild("cancelTypeCode");
	if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
		cancelType = atoi( xCurrentElement.GetText() );

	// for log
	xCurrentElement = xGreatGrandChildElement.FirstChild( "logSeq" );
	if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
		szLogSeq = xCurrentElement.GetText();	

	xCurrentElement = xGreatGrandChildElement.FirstChild( "cancelAmt" );
	if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
		cancelAmt = atoi( xCurrentElement.GetText() ); //취소된 금액 

	xCurrentElement = xGreatGrandChildElement.FirstChild( "remainAmt" );
	if( !xCurrentElement.IsEmpty() && xCurrentElement.GetText() != NULL )
		remainAmt = atoi( xCurrentElement.GetText() ); //원거래 금액 - 취소된 금액 

	// 캐쉬와 실재 구매한 캐쉬가 동일하다.
	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
	
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetIndex();
	kPacket << rData.GetChargeNo();
	kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	kPacket << cancelAmt;
	kPacket << remainAmt; 
	 
	//kyg 일단.. 넣어주질 않음 szReturnBuyNO를 
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		return;
	}
	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(),rData.GetChargeNo().c_str() );
}

void ioChannelingNodeDaum::UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize )
{
	if( szUTF8 == NULL )
		return;

	WCHAR szUni[MAX_PATH*2];
	ZeroMemory( szUni, sizeof(szUni) );

	int iUnisize = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1 , szUni, MAX_PATH*2 );
	riReturnAnsiSize = WideCharToMultiByte(CP_ACP, 0 , szUni, iUnisize, szAnsi, iAnsiBufferSize ,NULL ,NULL );
}

//------------------------------------------------------------------------------------------------DAUM
#include "openssl/sha.h"
#include "openssl/hmac.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 4554)
#pragma warning (disable : 4018)

void ioChannelingNodeDaum::decode(unsigned char* s, unsigned char* ret)
{
	int i=0;
	for(; i < strlen((const char*)s); i += 2)
	{
		unsigned char c = s[i];
		unsigned char c1 = s[i + 1];
		int j = i / 2;
		if(c < 'a')
			ret[j] = (unsigned char)(c - 48 << 4);
		else
			ret[j] = (unsigned char)((c - 97) + 10 << 4);
		if(c1 < 'a')
			ret[j] += (unsigned char)(c1 - 48);
		else
			ret[j] += (unsigned char)((c1 - 97) + 10);
	}
	ret[strlen((const char*)s)/2] = '\0';
}

char *ioChannelingNodeDaum::pt(unsigned char *md, char* buf)
{
	int i;

	for (i=0; i<SHA_DIGEST_LENGTH; i++)
		sprintf(&(buf[i*2]),"%02x",md[i]);
	return(buf);
}

char *ioChannelingNodeDaum::getSig(char *apiKey, char* data, char* ts, char* nonce) 
{
	int orgLen = strlen(data) + strlen(ts) + strlen(nonce) + 1;
	char *orgStr = (char*)malloc(orgLen);
	char *digestStr = (char*)malloc(SHA_DIGEST_LENGTH*2 + 1);
	char decodeApiKey[80];
	char *md;
	char tmp[80];
	char c[80];

	strncpy_s(orgStr,orgLen, data, strlen(data)+1);
	strncat_s(orgStr,orgLen, ts, strlen(ts)+1);
	strncat_s(orgStr,orgLen, nonce, strlen(nonce)+1);	

	decode((unsigned char*)apiKey, (unsigned char*)decodeApiKey);

	md =(char*) HMAC(EVP_sha1(),
		decodeApiKey, strlen(decodeApiKey),
		(const unsigned char*)orgStr, strlen(orgStr),
		(unsigned char*)c, NULL);

	strncpy_s(digestStr,SHA_DIGEST_LENGTH*2 + 1, pt((unsigned char *)md, tmp), SHA_DIGEST_LENGTH*2);
	digestStr[SHA_DIGEST_LENGTH*2] = '\0';

	free(orgStr);

	return digestStr;

}

void ioChannelingNodeDaum::getNewNonce(char *nonce) 
{
	int i, tmp;

	memset(nonce, '0', SHA_DIGEST_LENGTH*2);
	srand(time(NULL));
	for(i=0; i<40; i+=6) {
		tmp = rand();
		sprintf(&(nonce[i]),"%02x", tmp);
	}
	nonce[40] = '\0';
}

void ioChannelingNodeDaum::getNewTimeStamp(char *ts) 
{
	time_t          cal;
	struct tm       *lctm;

	if ((cal = time(NULL)) < 0)  {
		return;
	}

	lctm = localtime(&cal);

	strftime(ts, 15, "%Y%m%d%H%M%S", lctm);
	ts[15] = '\0'; //kyg 변수가 15짜린데 15에 널값어줌.. 
}

void ioChannelingNodeDaum::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket ) //kyg 이루틴 타는걸로도 테스트 해봐야함 
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szDaumNo;

	rkPacket >> iChannelingType;
	rkPacket >> szBillingGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szPublicID;
	rkPacket >> szPrivateID;
	rkPacket >> dwIndex; 
	//위에까지 공통인자 
	rkPacket >> szChargeNo;
	rkPacket >> szDaumNo;
 


	ioData kData;

	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserNo( szDaumNo );
	kData.SetChargeNo( szChargeNo );
	kData.SetIndex( dwIndex );
 
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_SUBSCRIPTION_RETRACT );

	g_ThreadPool.SetData( kData );
}
