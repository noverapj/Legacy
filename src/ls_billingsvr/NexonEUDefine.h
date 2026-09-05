#pragma  once

 
#include <stdint.h>

/************************************************************************/
/* packet header[packetID 1byte + packet length 4byte] + packet No + packet Type + Data Field
/* Nexon Protocol                                                       */
/************************************************************************/
#define	NEXON_PACKETHEADER				 0xAF	//packet header[packetID + packet length]
#define	NEXON_EU_INITIALIZE				 0x01
#define NEXON_EU_ALIVE					 0x02
#define NEXON_EU_OUTPUT_BALANCE				 0x11	//북미와 유럽에서 사용하는 패킷
#define NEXON_EU_BALANCE				 0x12	//북미와 유럽에서 사용하는 패킷
#define NEXON_EU_PURCHARE_AMOUNT		 0x15
#define NEXON_EU_PURCHASE_ITEM			 0x21
#define NEXON_EU_PURCHASE_GIFT			 0x22
#define NEXON_EU_PRODUCT_INQUIRY		 0x51
#define NEXON_EU_AUTH_INITIALIZE_REQUEST 131
#define NEXON_EU_AUTH_INITIALIZE_REPLY   132
#define NEXON_EU_AUTH_HELLO				 101
#define NEXON_EU_AUTH_SESSION4_REQUEST	 125
#define NEXON_EU_AUTH_SESSION4_REPLY	 126

#pragma pack(1)
typedef struct NexonEUPacketHeader // nexon 패킷 
{
	//header
	BYTE	packetID;	//1byte
	DWORD	size;		//Packet Length: PacketHeader를 제외한 Packet No + Packet Type + Data Field들의 길이 총합, 4byte

	DWORD	packetNo;	//4byte
	BYTE	packetType;	//1byte
	
	

	NexonEUPacketHeader() : packetID(0xAF)
	{
		size = 0;
		packetNo = 0;
	}
	
	void Init()
	{
		packetID = 0xAF;	//미리정의된 상수, 0xAF , 1byte
	}

	void Htonl() 
	{ 
		size = htonl( size ); 
		packetNo = htonl( packetNo );
	}
	void Ntohl() 
	{ 
		size = ntohl( size ); 
		packetNo = ntohl( packetNo );
	}
}EU_HEADER;

typedef struct NexonEUInitialize : public NexonEUPacketHeader
{
	WORD			serviceCodeLength;	//length는 2byte --
	ioHashString	serviceCode;
	BYTE			serverNo;			//1byte
	
	
	void Init()
	{
		packetType = NEXON_EU_INITIALIZE;
		serviceCodeLength = 0;
		packetNo = 1;
		serverNo = 99;
	}	
	void SetInfo(const char* szServiceCode)
	{
		Init();
		serviceCode = szServiceCode;
		serviceCodeLength = serviceCode.Length();
		size = sizeof( NexonEUInitialize ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo ) - sizeof( ioHashString ) + serviceCodeLength;	//ioHashString빼는이유, 길이값만 담으니까
		
	}
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		serviceCodeLength = htons( serviceCodeLength );

	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Htonl();
		serviceCodeLength = ntohs( serviceCodeLength );
	}
}EU_INITIALIZE;


typedef struct NexonProductInquiry : public NexonEUPacketHeader
{
	DWORD	pageIndex;
	DWORD	rowPerPage;
	DWORD	queryType;
	
	void Init()
	{
		packetType	= NEXON_EU_PRODUCT_INQUIRY;
		packetNo	= 0;
		pageIndex	= 0;
		rowPerPage	= 0;
		queryType	= 0;	
	}	
	void SetInfo(const DWORD dwPageIndex, const DWORD dwRowPerPage , const DWORD dwQueryType)
	{
		Init();
		
		pageIndex	= dwPageIndex;
		rowPerPage	= dwRowPerPage;
		queryType	= dwQueryType;
		size = sizeof( NexonProductInquiry ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo );
	}
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		pageIndex	= htonl( pageIndex );
		rowPerPage	= htonl(rowPerPage );
		queryType	= htonl( queryType );
	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Ntohl();
		pageIndex	= ntohl( pageIndex );
		rowPerPage	= ntohl( rowPerPage );
		queryType	= ntohl( queryType );
	}
}EU_PRODUCT_INQUIRY;

///데이터 추가함

typedef struct NexonProductInquiryResponse : public NexonEUPacketHeader
{	/*
	BYTE			bonusProductCount;

	WORD			productIDLength;
	WORD			extendLength;					
	WORD			productStatus;
    WORD			productGUIDLength;
	
	DWORD			result;
	DWORD			totalProductCount;
	DWORD			productArrayLength;
	DWORD			productNo;
	DWORD			relationProductNo;
	DWORD			productExpire;
	DWORD			productPieces;
	DWORD			productType;
	DWORD			salePrice;
	DWORD			paymentType;
	DWORD			categoryNo;
	uint64_t		releaseTicks;
	ioHashString	productID;
	ioHashString	productGUID;

	ioHashString	extend;
	*/
	DWORD			resultXMLLength;	//length는 2byte --
	DWORD			result;
	DWORD			totalProductCount;
	DWORD			remainProductCount;
	int64_t			releaseTicks;
	ioHashString	resultXML;
	
	
	void Init()
	{
		packetType			= NEXON_EU_PRODUCT_INQUIRY;
		result				= 0;
		totalProductCount	= 0;
		remainProductCount	= 0;
		resultXML.Clear();
		resultXMLLength		= 0;
		/*
		releaseTicks		= 0;
		bonusProductCount	= 0;
		productIDLength		= 0;
		extendLength		= 0;		
		productGUIDLength	= 0;
		productArrayLength  = 0;
		productNo			= 0;
		relationProductNo	= 0;
		productExpire		= 0;
		productPieces		= 0;
		productType			= 0;
		salePrice			= 0;
		productNo			= 0;
		paymentType			= 0;
		categoryNo			= 0;
		productStatus		= 0;

		productID.Clear();
		productGUID.Clear();
		extend.Clear();
		*/
		
	}	
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		
		result				= htonl( result );
		totalProductCount	= htonl( totalProductCount );
		remainProductCount	= htonl( remainProductCount );
		releaseTicks		= htonll( releaseTicks ); 
		resultXMLLength		= htonl( resultXMLLength );
		

		/*
		productIDLength		= htons( productIDLength );
		extendLength		= htons( extendLength );		
		productGUIDLength	= htons( productGUIDLength );
		productStatus		= htons( productStatus );

		result				= htonl( result );
		totalProductCount	= htonl( totalProductCount );
		productArrayLength	= htonl( productArrayLength );
		productNo			= htonl( productNo );
		relationProductNo	= htonl( relationProductNo );
		productExpire		= htonl( productExpire );
		productPieces		= htonl( productPieces );
		productType			= htonl( productType );
		salePrice			= htonl( salePrice );
		productNo			= htonl( productNo );
		paymentType			= htonl( paymentType );
		categoryNo			= htonl( categoryNo );
		
		releaseTicks		= htonll( releaseTicks ); 
		*/
		
	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Ntohl();
		
		result				= ntohl( result );
		totalProductCount	= ntohl( totalProductCount );
		remainProductCount	= ntohl( remainProductCount );
		releaseTicks		= ntohll( releaseTicks ); 
		resultXMLLength		= ntohs( resultXMLLength );

		/*
		productIDLength		= ntohs( productIDLength );
		extendLength		= ntohs( extendLength );		
		productGUIDLength	= ntohs( productGUIDLength );
		productStatus		= ntohs( productStatus );

		result				= ntohl( result );
		totalProductCount	= ntohl( totalProductCount );
		productArrayLength	= ntohl( productArrayLength );
		productNo			= ntohl( productNo );
		relationProductNo	= ntohl( relationProductNo );
		productExpire		= ntohl( productExpire );
		productPieces		= ntohl( productPieces );
		productType			= ntohl( productType );
		salePrice			= ntohl( salePrice );
		productNo			= ntohl( productNo );
		paymentType			= ntohl( paymentType );
		categoryNo			= ntohl( categoryNo );
		
		releaseTicks		= ntohll( releaseTicks ); 
		*/
	}
}EU_PRODUCT_INQUIRY_RESPONSE;

///데이터 추가함

typedef struct NexonEUInitializeResponse : public NexonEUPacketHeader
{
	WORD			serviceCodeLength;	//length는 2byte 
	ioHashString	serviceCode;
	BYTE			serverNo;			//1byte
	DWORD			result;
	
	void Init()
	{
		serviceCodeLength	= 0;
		serverNo			= 0;
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		serviceCodeLength = htons( serviceCodeLength );
		result			  = htonl( result );
	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Ntohl();
		serviceCodeLength	= ntohs( serviceCodeLength );
		result				= ntohl( result );
	}
}EU_INITIALIZE_RESPONSE;

typedef struct NexonEUHeartBeat : public NexonEUPacketHeader
{
	uint64_t productReleaseTicks;	
	//8byte로 서기 1년 1월 1일 자정100나노초 간격수
	
	void Init()
	{
		packetType			= NEXON_EU_ALIVE;
		productReleaseTicks = 0;
	}

	void SetInfo( DWORD dwTime, const uint64_t ticks )
	{
		Init();
		packetNo = dwTime;
		productReleaseTicks = ticks;
		size = sizeof( NexonEUHeartBeat ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo );
		
	}
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		productReleaseTicks= htonll( productReleaseTicks ); 

	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Ntohl();
		productReleaseTicks= ntohll( productReleaseTicks ); 
	}

}EU_ALIVE;

typedef struct NexonEUHeartBeatResponse : public NexonEUPacketHeader
{
	DWORD result;	

	void Init()
	{
		result = 0;
		
		//size = sizeof( NexonEUHeartBeatResponse ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo );
	}
	
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		result	= htonl( result ); 

	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Htonl();
		result= ntohl( result ); 
	}
	/*	
		1 = 정상
		0 = 오류
		17 = 상품정보 업데이트 – 상품정보가 변경되었으니 Get Products패킷을 재 요청하여 Client의 상품정보를 갱신하라는 의미입니다. NISMS로부터 상품목록을 받아가지 않는 경우엔 Result값을 무시해도 됩니다.
		255 = Maintenance
	*/
}EU_ALIVE_RESPONSE;

typedef struct NexonEUCheckCash : public NexonEUPacketHeader
{
	WORD			nexonIDLength;
	ioHashString	nexonID;
	
	void Init()
	{
		//packetType		= NEXON_EU_BALANCE;
		nexonIDLength	= 0;
	}

	void SetInfo( BYTE dwPacketType, const char* ID)
	{
		Init();
		packetType = dwPacketType;
		nexonID = ID;
		nexonIDLength = nexonID.Length();
		size = sizeof( NexonEUCheckCash ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo ) - sizeof( ioHashString ) + nexonIDLength;
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		//nexonIDLength	= htonl( nexonIDLength ); 
		nexonIDLength	= htons( nexonIDLength ); 

	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Htonl();
		//nexonIDLength= ntohl( nexonIDLength ); 
		nexonIDLength= ntohs( nexonIDLength ); 
	}

}EU_GETCASH;


/*typedef struct NexonEUCheckCashResponse : public NexonEUPacketHeader
{
	DWORD result;
	DWORD balance;
	DWORD notRefundableBalance;	//환불 불가능한 캐시ㅣ 잔액

	void Init()
	{
		result = 0;
		balance = 0;
		notRefundableBalance = 0;
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		result					= htonl( result ); 
		balance					= htonl( balance ); 
		notRefundableBalance	= htonl( notRefundableBalance ); 

	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Htonl();
		result					= ntohl( result ); 
		balance					= ntohl( balance ); 
		notRefundableBalance	= ntohl( notRefundableBalance ); 
	}

	
}EU_GETCASH_RESPONSE;*/

typedef struct NexonEUCheckCashResponse : public NexonEUPacketHeader
{
	DWORD result;
	DWORD balance;
	DWORD notRefundableBalance;	//환불 불가능한 캐시ㅣ 잔액

	void Init()
	{
		result = 0;
		balance = 0;
		notRefundableBalance = 0;
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		result					= htonl( result ); 
		balance					= htonl( balance ); 
		notRefundableBalance	= htonl( notRefundableBalance ); 

	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Htonl();
		result					= ntohl( result ); 
		balance					= ntohl( balance ); 
		notRefundableBalance	= ntohl( notRefundableBalance ); 
	}

	/*
	1 = 정상
	0 = 오류
	12001 = 블록된 유저
	12002 = 회원정보를 찾을 수 없음
	99 = DB 오류
	255 = Maintenance
	*/
}EU_GETCASH_RESPONSE;

typedef struct NexonEUCheckCashAmountResponse : public NexonEUPacketHeader
{
	DWORD result;
	DWORD balance;
	
	void Init()
	{
		result = 0;
		balance = 0;
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		result					= htonl( result ); 
		balance					= htonl( balance ); 
	
	}
	void Ntohl() 
	{ 
		NexonEUPacketHeader::Htonl();
		result					= ntohl( result ); 
		balance					= ntohl( balance ); 
	}
	/*
	1 = 정상
	0 = 오류
	12001 = 블록된 유저
	12002 = 회원정보를 찾을 수 없음
	99 = DB 오류
	255 = Maintenance
	*/
}EU_GETCASH_AMOUNT_RESPONSE;

typedef struct NexonEUGetPurchaseAmount : public NexonEUPacketHeader
{
	DWORD orderNo;		//NISMS 가 발급한 주문승인번호
	DWORD productNo;	//상품번호

	void Init()
	{
		packetType = NEXON_EU_PURCHARE_AMOUNT;
		orderNo = 0;
		productNo = 0;
	}

	void SetInfo( const DWORD iorderNo, const DWORD iproductNo )
	{
		Init();
		orderNo = iorderNo;
		productNo = iproductNo;
	    size = sizeof( NexonEUGetPurchaseAmount ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo );
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		orderNo					= htonl( orderNo ); 
		productNo				= htonl( productNo ); 
	}

	void Ntohl()
	{
		NexonEUPacketHeader::Htonl();
		orderNo					= ntohl( orderNo ); 
		productNo				= ntohl( productNo ); 
	}
}EU_AMOUNT;

typedef struct NexonEUGetPurchaseAmountResponse : public NexonEUPacketHeader
{
	BYTE	arrayLength;
	BYTE	type;	// 2 유료, 6 무료, 17 1회성유료(선물받은)
	DWORD	value;	// 4 차감금액
	DWORD	result;

	void Init()
	{
		result			= 0;
		arrayLength		= 0;
		type			= 0;
		value			= 0;
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		arrayLength				= htons( arrayLength );
		type					= htons( type );
		value					= htonl( value ); 
		result					= htonl( result ); 
	}

	void Ntohl()
	{
		NexonEUPacketHeader::Htonl();
		arrayLength				= ntohs( arrayLength );
		type					= ntohs( type );
		value					= ntohl( value ); 		
		result					= ntohl( result ); 
		
	}
}EU_AMOUNT_RESPONSE;

typedef struct NexonEUPurchaseItem : public NexonEUPacketHeader
{
	DWORD			clientIP;
	BYTE			reason;
	WORD			gameIDLength;
	ioHashString	gameID;	
	WORD			nexonIDLengh;
	ioHashString	nexonID;
	DWORD			nexonOID;
	WORD			useNameLength;
	ioHashString	userName;
	BYTE			userAge;
	WORD			orderIDLength;
	ioHashString	orderID;
	DWORD			paymentType;
	DWORD			totalAmount;
	BYTE			productArrayLength;
	DWORD			productNo;
	WORD			orderQuantity;
	
	void Init()
	{
		packetType			= NEXON_EU_PURCHASE_ITEM;		
		reason				= 0;	
		userAge				= 0;
		productArrayLength	= 0;
		orderQuantity		= 0;
		gameIDLength		= 0;
		nexonIDLengh		= 0;
		useNameLength		= 0;
		orderIDLength		= 0;
		clientIP			= 0;
		nexonOID			= 0;
		paymentType			= 0;		
		totalAmount			= 0;
		productNo			= 0;
		
		gameID.Clear();
		nexonID.Clear();
		orderID.Clear();
		userName.Clear();
		
	}

	void SetInfo( const DWORD iclientIP, const BYTE ireason, const char* szgameID, const char* sznexonID, const DWORD dwnexonOID, const char* szuserName, const BYTE iuserAge, const char* szorderID, const DWORD ipaymentType, const DWORD itotalAmount, const BYTE iproductArrayLength, const DWORD iproductNo, const WORD iorderQuantity )
	{
		Init();
		clientIP			= iclientIP;
		reason				= ireason;
		gameID				= szgameID;
		gameIDLength		= gameID.Length();
		nexonID				= sznexonID;
		nexonIDLengh		= nexonID.Length();
		nexonOID			= dwnexonOID;
		
		userName			= szuserName;
		useNameLength		= userName.Length();
		userAge				= iuserAge;
		orderID				= szorderID;
		orderIDLength		= orderID.Length();
		paymentType			= ipaymentType;
		totalAmount			= itotalAmount;
		productArrayLength	= iproductArrayLength;
		productNo			= iproductNo;
		orderQuantity		= iorderQuantity;
		/*
		nexonIDLength = nexonID.Length();
		size = sizeof( NexonEUCheckCash ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo ) - sizeof( ioHashString ) + nexonIDLength;
		*/
		size = sizeof( NexonEUPurchaseItem ) - sizeof( NexonEUPacketHeader ) + sizeof( packetType ) + sizeof( packetNo ) - sizeof( ioHashString) + gameIDLength -sizeof( ioHashString ) + nexonIDLengh - sizeof( ioHashString ) + useNameLength - sizeof( ioHashString ) + orderIDLength;
	}
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		
		orderQuantity	= htons( orderQuantity );
		gameIDLength	= htons( gameIDLength );
		nexonIDLengh	= htons( nexonIDLengh );
		useNameLength	= htons( useNameLength );
		orderIDLength	= htons( orderIDLength );

		clientIP		= htonl( clientIP );
		nexonOID		= htonl( nexonOID );
		paymentType		= htonl( paymentType );
		totalAmount		= htonl( totalAmount );
		productNo		= htonl( productNo );
		
	}

	void Ntohl()
	{
		NexonEUPacketHeader::Ntohl();

		orderQuantity	= ntohs( orderQuantity );
		gameIDLength	= ntohs( gameIDLength );
		nexonIDLengh	= ntohs( nexonIDLengh );
		useNameLength	= ntohs( useNameLength );
		orderIDLength	= ntohs( orderIDLength );

		clientIP		= ntohl( clientIP );
		nexonOID		= ntohl( nexonOID );
		paymentType		= ntohl( paymentType );
		totalAmount		= ntohl( totalAmount );
		productNo		= ntohl( productNo );
	}
}EU_PURCHASEITEM;

typedef struct NexonEUPurchaseItemResponse : public NexonEUPacketHeader
{
	BYTE			productArrayLength;	

	WORD			extendValuelength;
	WORD			orderIDlength;
	WORD			orderQuantity;

	DWORD			result;
	DWORD			orderNo;
	DWORD			productNo;
	
	ioHashString	extendValue;
	ioHashString	orderID;
	

	void Init()
	{	
		orderIDlength		= 0;
		result				= 0;
		orderNo				= 0;
		productArrayLength	= 0;
		productNo			= 0;
		orderQuantity		= 0;
		extendValuelength		= 0;
		extendValue.Clear();
		orderID.Clear();
	}
	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		
		extendValuelength	= htons( orderQuantity );
		orderIDlength		= htons( orderIDlength );
		orderQuantity		= htons( orderQuantity );
		
		result				= htonl( result );
		orderNo				= htonl( orderNo );
		productNo			= htonl( productNo );
		
	}

	void Ntohl()
	{
		NexonEUPacketHeader::Ntohl();

		extendValuelength	= ntohs( extendValuelength );
		orderIDlength		= ntohs( orderIDlength );
		orderQuantity		= ntohs( orderQuantity );
		
		result				= ntohl( result );
		orderNo				= ntohl( orderNo );
		productNo			= ntohl( productNo );
	}

}EU_PURCAHSEITEM_RESPONSE;

typedef struct NexonEUPurchaseGift : public NexonEUPacketHeader
{
	BYTE			reason;
	
	WORD			userAge;
	WORD			serverNo;

	DWORD			remoteIP;
	DWORD			senderUserOID;
	
	ioHashString	senderGameID;
	ioHashString	senderUserID;
	ioHashString	senderUserName;
	void Init()
	{
		packetType = NEXON_EU_PURCHASE_GIFT;  
	}

	void Htonl()
	{
		NexonEUPacketHeader::Htonl();
		
		userAge			= htons( userAge );
		serverNo		= htons( serverNo );
		
		remoteIP		= htonl( remoteIP );
		senderUserOID	= htonl( senderUserOID );
		
	}

	void Ntohl()
	{
		NexonEUPacketHeader::Ntohl();

		userAge			= ntohs( userAge );
		serverNo		= ntohs( serverNo );
		
		remoteIP		= ntohl( remoteIP );
		senderUserOID	= ntohl( senderUserOID );
		
	}

}EU_GIFT;

typedef struct NexonEUSessionHeader
{
	WORD size;	//2byte, 이 필드 포함 전체 패킷 크기
	WORD code;	//2byte, 패킷의 종류를 구분하는 코드값

	void Init()
	{
		code = 0;  
		size = 0;
	}
	
	void Ntohl()
	{
	
		size		= ntohs( size );
		code		= ntohs( code );
	}

}EU_SESSION_HEADER;

typedef struct NexonEUSessionInitialize : public NexonEUSessionHeader
{
	UINT gameCode;
	UINT serverCode;	//연결을 시도한 게임서버의 서버 코드 , 정보성 데이터 이므로 입력하지 않아도됨
	UINT codePage;

	void Init()
	{
		code		= NEXON_EU_AUTH_INITIALIZE_REQUEST;
		size		= 0;
		gameCode	= 0;
		serverCode	= 0;
		codePage	= 0;

	}	
	void SetInfo(const UINT ugameCode, const UINT userverCode )
	{
		Init();
		gameCode	= ugameCode;
		serverCode	= userverCode;
		codePage	= 949;
		size		= sizeof( NexonEUSessionInitialize );
	}

	void Ntohl()
	{
		NexonEUSessionHeader::Ntohl();

		gameCode			= ntohl( gameCode );
		serverCode			= ntohl( serverCode );
		codePage			= ntohl( codePage );
	}

}EU_SESSION_INITIALIZE;

typedef struct NexonEUSessionResponse : public NexonEUSessionHeader
{
	DWORD initializeResult;

	void Init()
	{
		code				= NEXON_EU_AUTH_INITIALIZE_REPLY;
		size				= 0; 
		initializeResult	= 0;
	}
}EU_SESSION_RESPONSE;

typedef struct NexonEUSessionCheck : public NexonEUSessionHeader
{
	unsigned int serial;
	void Init()
	{
		code	= NEXON_EU_AUTH_HELLO;
		size	= 0;
		serial	= 0;
	}

	void SetInfo( const UINT dwSerial )
	{
		Init();
		serial = dwSerial;
		size   = sizeof( NexonEUSessionCheck );
	}

}EU_SESSION_CHECK;

typedef struct NexonEUSession4Request : public NexonEUSessionHeader
{
	UINT serial;

	WORD			passPortLength;
	ioHashString	passPort;
  
	WORD			userIPLength;
	ioHashString	userIP;
  	
	WORD			userHwidLength;
	ioHashString	userHwid;

	UINT			gameCode;
	
	void Init()
	{
		code = NEXON_EU_AUTH_SESSION4_REQUEST;
		size = 0;
		serial = 0;
		passPortLength = 0;
		passPort.Clear();
		
		userIPLength = 0;
		userIP.Clear();

		userHwidLength = 0;
		userHwid.Clear();

		gameCode = 0;
	}

	void SetInfo( const UINT uSerial, const char* szPassPort, const char* szUserIP, const char* szUserHwid, UINT ugameCode )
	{
		Init();
		serial = uSerial;

		passPort = szPassPort;
		passPortLength = passPort.Length();

		userIP = szUserIP;
		userIPLength = userIP.Length();

		userHwid = szUserHwid;
		userHwidLength = userHwid.Length();

		gameCode = ugameCode;

		size = sizeof( NexonEUSession4Request ) - sizeof( ioHashString ) + passPortLength - sizeof( ioHashString ) + userIPLength - sizeof( ioHashString ) + userHwidLength;
	}

}EU_SESSION4_REQUEST;

typedef struct NexonEUSession4Reply : public NexonEUSessionHeader
{
	UINT		 serial;
	DWORD		 resultCode;

	WORD		 nexonIDLength;
	ioHashString nexonID;

	WORD		 clientIPLength;
	ioHashString clientIP;

	INT64		 nexonSN;
	DWORD	     gender;
	
	DWORD		 age;

	WORD		 nationCodeLength;
	ioHashString nationCode;

	WORD		 metaDataLength;
	ioHashString metaData;

	BYTE		 secureCode;
	BYTE		 channelCode;

	WORD		 channelUIDLength;
	ioHashString channelUID;

	BYTE		 memberShip;
	BYTE		 mainAuthLevel;
	BYTE		 subAuthLevel;

	void Init()
	{
		code				= NEXON_EU_AUTH_SESSION4_REPLY;
		size				= 0;

		serial				= 0;
		resultCode			= 0;
		nexonIDLength		= 0;
		nexonID.Clear();
		clientIPLength		= 0;
		clientIP.Clear();
		nexonSN				= 0;
		gender				= 0;
		age					= 0;
		nationCodeLength	= 0;
		nationCode.Clear();
		metaDataLength		= 0;
		metaData.Clear();
		secureCode			= 0;	
		channelCode         = 0;

		channelUIDLength	= 0;
	    channelUID.Clear();

		memberShip			= 0;
		mainAuthLevel		= 0;
		subAuthLevel		= 0;


	}

}EU_SESSION4_REPLY;

typedef struct NISMS_LIST
{
	DWORD goodsNo;
	DWORD salesPrice;
	
	void Init()
	{
		goodsNo		= 0;
		salesPrice	= 0;
	}

	void SetData( int dwGoodsNo, int dwSalesPrice )
	{
		Init();
		goodsNo		= dwGoodsNo;
		salesPrice	= dwSalesPrice;
	}
}NISMS_LIST;