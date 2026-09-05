#pragma  once

 
#include <stdint.h>

#define htonll(x) \
	((((x) & 0xff00000000000000LL) >> 56) | \
	(((x) & 0x00ff000000000000LL) >> 40) | \
	(((x) & 0x0000ff0000000000LL) >> 24) | \
	(((x) & 0x000000ff00000000LL) >> 8) | \
	(((x) & 0x00000000ff000000LL) << 8) | \
	(((x) & 0x0000000000ff0000LL) << 24) | \
	(((x) & 0x000000000000ff00LL) << 40) | \
	(((x) & 0x00000000000000ffLL) << 56))
#define ntohll(x) \
	((((x) & 0x00000000000000FF) << 56) | \
	(((x) & 0x000000000000FF00) << 40) | \
	(((x) & 0x0000000000FF0000) << 24) | \
	(((x) & 0x00000000FF000000) << 8)  | \
	(((x) & 0x000000FF00000000) >> 8)  | \
	(((x) & 0x0000FF0000000000) >> 24) | \
	(((x) & 0x00FF000000000000) >> 40) | \
	(((x) & 0xFF00000000000000) >> 56))

/************************************************************************/
/* Nexon Protocol                                                                     */
/************************************************************************/
enum
{
	NEXON_PACKETHEADER	= 0x00AA, //== 0xAA
	NEXON_INITIALIZE	= 41,
	NEXON_LOGIN			= 42,
	NEXON_LOGOUT		= 43,
	NEXON_TERMINATE		= 44,
	NEXON_MESSAGE		= 45,
	NEXON_SYNC			= 46,
	NEXON_ALIVE			= 100,


};

/************************************************************************/
/* nexonUserCode                                                                     */
/************************************************************************/
#define NX_USER_CODE "0;"
#define ORTHER_USER_CODE "15;"

#define NEXON_NAME_MAX 64
#define NEXON_SYNC_TYPE 2
#define NEXON_SYNC_COUNT 100
#define NEXON_USERID_MAX 128
#define NEXON_MESSAGE_MAX 64
#define NEXON_LOGIN_PROPERTY_COUNT 5
#define NEXON_LOGIN_PROPERTY_CHARNAME 1
#define NEXON_LOGIN_PROPERTY_LOCALIP 2
#define NEXON_LOGIN_PROPERTY_POLICYCOUNT 1
#define NEXON_LOGIN_PROPERTY_POLICY_VALUE 10
#define NEXON_LOGIN_PROPERTY_POLICYTYPE 17
#define NEXON_POLICY_DISCIPTION 256
#define NEXON_POLICY_NAME_MAX 64
#define NEXON_DOMAIN 101
#define NEXON_POLICY_SHUTDOWN 19
#define NEXON_LOGIN_REQUEST_PCROOMNO 13
#define NEXOM_LOGIN_PROPERTY_PCROOMNO 7
#define NEXON_LOGIN_PROPERTY_ISPCROOM 8

#pragma pack(1) 
/************************************************************************/
/* Nexon                                                                     */
/************************************************************************/
struct NexonPacketHeader // nexon 패킷 
{
	BYTE header;
	WORD size; //byte
	BYTE PacketType; //필수 
	NexonPacketHeader() : header(0xAA),PacketType(0)
	{
		//header = 0xAA; //kyg 고정값 넥슨문서에 따름 
		size = 0;
	}
	NexonPacketHeader(BYTE cType) : header(0xAA),PacketType(cType){ size = 0;}

	void Init()
	{
		header = 0xAA;
		PacketType = 0;
	}

	void Htonl() { size = htons(  size ); }

	void Ntohl() { size = ntohs( size ); }


};
//inet_addr;

struct NexonFirstInitialize : public NexonPacketHeader
{
	BYTE InitializeType; // Default 1;
	BYTE GameSn;//GameNo
	BYTE DomainSn; //
	BYTE DomainNameLength;
	ioHashString  DomainName;
	BYTE SynchronizeType; //1. Only SessionNo, 2. SerssionNo + CharName
	DWORD SynchronizeCount; // Dafult 100~500

	NexonFirstInitialize() { Init(); };

	void Init()
	{
		NexonPacketHeader::Init();
		PacketType = NEXON_INITIALIZE;
		InitializeType = 1;
		SynchronizeType = NEXON_SYNC_TYPE; //2로하면 안대는지 확인 
		SynchronizeCount = NEXON_SYNC_COUNT;
		GameSn = NEXON_DOMAIN;;//kyg 130712 메일 받으면 추가 
		DomainSn =2;//kyg 1~3 암거나 써두댐 
	}

	void SetInfo( const char* szDomainName )
	{
		Init();	
		DomainName = szDomainName;
		DomainNameLength = DomainName.Length() ;
		size = sizeof(NexonFirstInitialize) - sizeof(NexonPacketHeader) + sizeof(PacketType) - sizeof(ioHashString) +  DomainNameLength;  //+1 = nulll //PacketType;
	}

	void Htonl() { NexonPacketHeader::Htonl(); SynchronizeCount = htonl( SynchronizeCount ); }

	void Ntohl() { NexonPacketHeader::Ntohl(); SynchronizeCount = ntohl( SynchronizeCount ); }
};

struct NexonReInitialize : public NexonPacketHeader
{
	//BYTE PacketType; //필수 
	BYTE InitializeType; // Default 2;
	BYTE GameSn;//GameNo //추후 수정 
	BYTE DomainSn; //
	BYTE DomainNameLength;
	ioHashString  DomainName;
	BYTE SynchronizeType; //1. Only SessionNo, 2. SerssionNo + CharName
	BYTE UserSynchronize;
	DWORD SynchronizeCount; // Dafult 100~500

	NexonReInitialize() : NexonPacketHeader(NEXON_INITIALIZE),InitializeType(2)	{ Init(); };

	void Init()
	{
		NexonPacketHeader::Init();

		PacketType = NEXON_INITIALIZE;
		InitializeType = 2; //Re
		GameSn = NEXON_DOMAIN;//kyg 130712 메일 받으면 추가 
		DomainSn = 2;//kyg 130712 Add to After Mail Receive 
		DomainNameLength = 0;
		SynchronizeType = NEXON_SYNC_TYPE;
		SynchronizeCount = NEXON_SYNC_COUNT;
		UserSynchronize = 1;
		size = sizeof(NexonReInitialize) - sizeof(NexonPacketHeader) - sizeof(ioHashString) + DomainName.Length() + 1;//PacketType;
	}

	void SetInfo(const char* szDomainName,
		const BYTE cSynchronizeType,
		const int iSynchronizeCount)
	{

		Init();
		DomainName = szDomainName;
		DomainNameLength = DomainName.Length();
		SynchronizeType = cSynchronizeType;
		SynchronizeCount = iSynchronizeCount;
		 
		size = sizeof(NexonReInitialize) - sizeof(NexonPacketHeader) + sizeof(PacketType) - sizeof(ioHashString) + DomainNameLength;//PacketType;
	}

	void SetInfo(const char* szDomainName)
	{
		Init();
		DomainName = szDomainName;
		DomainNameLength = DomainName.Length();
		size = sizeof(NexonReInitialize) - sizeof(NexonPacketHeader) + sizeof(PacketType) - sizeof(ioHashString) + DomainNameLength;//PacketType;
	}

	void Htonl()
	{
		NexonPacketHeader::Htonl();
		SynchronizeCount = htonl( SynchronizeCount );       
	}

	void Ntohl()
	{
		NexonPacketHeader::Ntohl();
		SynchronizeCount = ntohl( SynchronizeCount );       
	}
};


struct OnNexonInitialize : public NexonPacketHeader
{
	//BYTE PacketType;
	BYTE InitializeType;
	BYTE Result;
	BYTE DomainSn;
	BYTE MessageLength;
	ioHashString Message;
	BYTE PropertyCount;

	OnNexonInitialize()
	{
		//PacketType = 0;
		InitializeType = 0;
		Result = 0;
		DomainSn = 0;
		PropertyCount = 0;
		size = sizeof(OnNexonInitialize) - sizeof(NexonPacketHeader) + sizeof(BYTE);//PacketType;
	}
};

struct NexonAddInitializePacket //kyg 상속안받음 
{
	uint64_t SessionNo;
	BYTE UserIdLength;
	ioHashString UserId;

	NexonAddInitializePacket()
	{
		SessionNo = 0;
		UserIdLength = 0;
	}

	void Init()
	{
		SessionNo = 0;
		UserIdLength = 0;
		UserId.Clear();
	}

	void SetInfo(const uint64_t iSessionNo, const char* szUserId)
	{
		SessionNo = iSessionNo;
		UserId = szUserId;
		UserIdLength = UserId.Length();
	}

	void Htonl() { SessionNo = htonll( SessionNo ); }

	void Ntohl() { SessionNo = ntohll( SessionNo); }
};

struct NexonLogin : public NexonPacketHeader //메모리풀로 된다면 어케될지 고민중...
{
	//	const BYTE PacketType;//kyg must
	BYTE LoginType;// 1
	BYTE userIdLength;
	ioHashString UserId;//nexonID
	DWORD ClientIP; // inet_addr 
	BYTE PropertyCount; //우리가 쓰는건 3개임 
	//extend property
	WORD CharaterNameType;//1 
	BYTE CharNameLength;
	ioHashString CharName;
	WORD LogcalIPType;//2
	DWORD LocalIP;// PrivateIP inet_addr;.
	WORD AuthType; // 12
	BYTE OnlyAuth;
	WORD PCBangNoType;//13
	BYTE PCBangNo;
	WORD IsPolicyReturnType;//17
	BYTE PolicyReturnCount;//1
	BYTE PolicyType; // 10

	NexonLogin() { Init(); }

	void Init()
	{
		NexonPacketHeader::Init();
		PacketType = NEXON_LOGIN;
		LoginType = 1;
		PropertyCount = NEXON_LOGIN_PROPERTY_COUNT;
		CharaterNameType = 1;
		LogcalIPType = NEXON_LOGIN_PROPERTY_LOCALIP;
		IsPolicyReturnType = NEXON_LOGIN_PROPERTY_POLICYTYPE;
		PolicyReturnCount = NEXON_LOGIN_PROPERTY_POLICYCOUNT;
		PolicyType = NEXON_LOGIN_PROPERTY_POLICY_VALUE;
		AuthType = 12;
		OnlyAuth = 0; // 평소엔 0 무슨일있을땐 1
		userIdLength = 0;
		UserId.Clear();
		ClientIP = 0;
		CharNameLength = 0;
		CharName.Clear();
		int LocalIP = 0;
		PCBangNoType = NEXON_LOGIN_REQUEST_PCROOMNO;
		PCBangNo = 1;
		size = sizeof(NexonLogin) - sizeof(NexonPacketHeader) - sizeof(ioHashString) + CharName.Length()+1 - sizeof(ioHashString) + UserId.Length() +1;//PacketType;
	}

	void SetInfo(const char* szUserId,  const DWORD iClientIP, const char* szCharName, const DWORD iLocalIP, BYTE onlyAuth = 0)
	{
		Init();
		UserId = szUserId;
		userIdLength = UserId.Length();
		ClientIP = iClientIP;
		CharName = szCharName;
		CharNameLength = CharName.Length();
		LocalIP = iLocalIP;
		OnlyAuth = onlyAuth;
		size = sizeof(NexonLogin) - sizeof(NexonPacketHeader) + sizeof(PacketType) - sizeof(ioHashString) + userIdLength  - sizeof(ioHashString) + CharNameLength ;//PacketType;
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
		CharaterNameType = htons( CharaterNameType );
		LogcalIPType = htons( LogcalIPType );
		IsPolicyReturnType = htons( IsPolicyReturnType );
		AuthType = htons(AuthType);
		PCBangNoType = htons(PCBangNoType);
		 
	}

	void Ntohl()
	{ 
		NexonPacketHeader::Ntohl();
		CharaterNameType = ntohs( CharaterNameType );
		LogcalIPType = ntohs( LogcalIPType );
		IsPolicyReturnType = ntohs( IsPolicyReturnType );
		AuthType = ntohs(AuthType);
		PCBangNoType = ntohs(PCBangNoType);
	}
};

struct OtherLogin : public NexonPacketHeader //메모리풀로 된다면 어케될지 고민중...
{
	//	const BYTE PacketType;//kyg must
	BYTE LoginType;// 1
	BYTE userIdLength;
	ioHashString UserId;//nexonID
	DWORD ClientIP; // inet_addr 
	BYTE PropertyCount; //우리가 쓰는건 3개임 
	//extend property
	WORD CharaterNameType;//1 
	BYTE CharNameLength;
	ioHashString CharName;
	WORD LogcalIPType;//2
	DWORD LocalIP;// PrivateIP inet_addr;.
	WORD AuthType; // 12
	BYTE OnlyAuth;
    WORD PCBangNoType;
	BYTE PCBangNo;

	OtherLogin() { Init(); }

	void Init()
	{
		NexonPacketHeader::Init();
		PacketType = NEXON_LOGIN;
		LoginType = 1;
		PropertyCount = NEXON_LOGIN_PROPERTY_COUNT -1;
		CharaterNameType = 1;
		LogcalIPType = NEXON_LOGIN_PROPERTY_LOCALIP;
		AuthType = 12;
		OnlyAuth = 0; // 평소엔 0 무슨일있을땐 1
		userIdLength = 0;
		UserId.Clear();
		ClientIP = 0;
		CharNameLength = 0;
		CharName.Clear();
		int LocalIP = 0;
		PCBangNoType = NEXON_LOGIN_REQUEST_PCROOMNO;
		PCBangNo = 1;
		size = sizeof(OtherLogin) - sizeof(NexonPacketHeader) - sizeof(ioHashString) + CharName.Length()+1 - sizeof(ioHashString) + UserId.Length() +1;//PacketType;
	}

	void SetInfo(const char* szUserId,  const DWORD iClientIP, const char* szCharName, const DWORD iLocalIP, BYTE onlyAuth = 0)
	{
		Init();
		UserId = szUserId;
		userIdLength = UserId.Length();
		ClientIP = iClientIP;
		CharName = szCharName;
		CharNameLength = CharName.Length();
		LocalIP = iLocalIP;
		OnlyAuth = onlyAuth;
		size = sizeof(OtherLogin) - sizeof(NexonPacketHeader) + sizeof(PacketType) - sizeof(ioHashString) + userIdLength  - sizeof(ioHashString) + CharNameLength ;//PacketType;
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
		CharaterNameType = htons( CharaterNameType );
		LogcalIPType = htons( LogcalIPType );
		AuthType = htons(AuthType);
		PCBangNoType = htons(PCBangNoType);
	}

	void Ntohl()
	{ 
		NexonPacketHeader::Ntohl();
		CharaterNameType = ntohs( CharaterNameType );
		LogcalIPType = ntohs( LogcalIPType );
		AuthType = ntohs(AuthType);
		PCBangNoType = ntohs(PCBangNoType);
	}
};

struct OnNexonLogin : public NexonPacketHeader
{
	//BYTE PacketType;
	BYTE LoginType;
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString UserId;
	char AuthorizeResult;
	char AuthorizeType;
	char ChargeType;
	BYTE Option;
	DWORD Argument;
	BYTE IsCharged;
	BYTE PropertyCount;
	//char ProPertyData[2048]; // 순서가 바뀌어올 확률도 있음으모 받아서 파싱하는 식으로 처리 하는게 좋을거같음 

	OnNexonLogin()
	{
		//PacketType = 0;
		LoginType = 0;
		SessionNo = 0;
		UserId.Clear();
		AuthorizeResult = 0;
		AuthorizeType = 0;
		ChargeType = 0;
		Option = 0;
		Argument = 0;
		IsCharged = 0;
		PropertyCount = 0;
		//ZeroMemory(ProPertyData,2048);
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
		SessionNo= htonll( SessionNo ); //kyg 이거는 안해도 될듯함 
		Argument = htonl( Argument );
		
	}

	void Ntohl()
	{ 
		NexonPacketHeader::Ntohl();
		SessionNo = ntohll( SessionNo ); //kyg 이거는 안해도 될듯함 
		Argument = ntohl( Argument );
	}

};

struct NexonPolicyResult
{
	BYTE PolicyNo;
	BYTE PolicyNameLength;
	ioHashString PolicyName;
	BYTE PolicyResult;//1 Active
	BYTE PolicyDisLength;
	ioHashString PolicyDisciption;
	DWORD PCRoomNo;
};

struct NexonLogOut : public NexonPacketHeader
{
	//const BYTE PacketType;
	BYTE LogoutType;
	BYTE userIdLength;
	ioHashString UserId;
	uint64_t SessionNo;

	NexonLogOut() :NexonPacketHeader(NEXON_LOGOUT),LogoutType(1)
	{
		Init();
	}
	void Init()
	{
		PacketType = NEXON_LOGOUT;
		userIdLength = 0;
		UserId.Clear();
		SessionNo = 0;
		LogoutType = 1;
	}

	void SetInfo(const char* szUserId,const uint64_t iSessionNo )
	{
		Init();
		PacketType = NEXON_LOGOUT;
		UserId = szUserId;
		userIdLength = UserId.Length();
		SessionNo = iSessionNo;

		size = sizeof(NexonLogOut) - sizeof(NexonPacketHeader)  + sizeof(PacketType) - sizeof(ioHashString) + userIdLength; //NULL
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
		SessionNo= htonll( SessionNo ); //kyg 이거는 안해도 될듯함 
	}

	void Ntohl()
	{ 
		NexonPacketHeader::Ntohl();
		SessionNo = ntohll( SessionNo ); //kyg 이거는 안해도 될듯함 
	}
};

struct OnNexonTerminate : public NexonPacketHeader
{
	//BYTE PacketType;
	BYTE TerminateType;
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString UserId;
	BYTE CharNameLength;
	ioHashString CharName;
	BYTE Option;
	BYTE PropertyCount;
	//char ProPer tyData[2048]; // 순서가 바뀌어올 확률도 있음으모 받아서 파싱하는 식으로 처리 하는게 좋을거같음 

	OnNexonTerminate()
	{
		TerminateType = 0;
		SessionNo = 0;
		userIdLength = 0;
		UserId.Clear();
		CharNameLength = 0;
		CharName.Clear();
		Option = 0;
		PropertyCount = 0;
		//	ZeroMemory(ProPertyData,2048);
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();

		//PropertyCount = htonl(PropertyCount);
		SessionNo= htonll( SessionNo ); //kyg 이거는 안해도 될듯함 

	}

	void Ntohl()
	{ 
		NexonPacketHeader::Ntohl();
	
		//PropertyCount = ntohl(PropertyCount);
		SessionNo = ntohll( SessionNo ); //kyg 이거는 안해도 될듯함 
	}
};

struct OnNexonSynchronize : public NexonPacketHeader
{
	BYTE IsMonitoring;
	DWORD Count;

	OnNexonSynchronize()
	{
		//		PacketType = 0;
		IsMonitoring = 0;
		Count = 0;
//		SessionNo = 0;
	//	userIdLength = 0;
	//	userId.c_str();
	};

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
	//	SessionNo = htonll( SessionNo );  
		Count = htonl( Count );  
	}								    

	void Ntohl()					    
	{ 								    
		NexonPacketHeader::Ntohl();	    
		Count = ntohl( Count ); 
		//SessionNo = ntohll( SessionNo ); 
	}								    
};

struct OnNexonSyncUser
{
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString userId;

	OnNexonSyncUser() { Init();	}

	void Init()
	{
		SessionNo = 0;
		userIdLength = 0;
		userId.Clear();
	}

	void Htonl()
	{		 
		SessionNo = htonll( SessionNo );  
	}								    

	void Ntohl()					    
	{ 								    
		SessionNo = ntohll( SessionNo );  
	}	

};

struct OnNexonMessage : public NexonPacketHeader
{
	BYTE MessageType;
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString UserId;
	BYTE charNameLegnth;
	ioHashString CharName;
	BYTE Option;
	DWORD Argument;
	DWORD SessionCount;

	OnNexonMessage()
	{
		MessageType = 0;
		SessionNo = 0;

		//ZeroMemory(UserId,NEXON_USERID_MAX);
		//ZeroMemory(CharName,NEXON_USERID_MAX);
		Option = 0;
		Argument = 0;
		SessionCount = 0;
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
		SessionNo = htonll( SessionNo );  
		Argument = htonl( Argument );  
		SessionCount = htonl( SessionCount );  
	}								    

	void Ntohl()					    
	{ 								    
		NexonPacketHeader::Ntohl();	    
		SessionNo = ntohll( SessionNo ); 
		Argument = ntohl( Argument ); 
		SessionCount = ntohl( SessionCount ); 
	}			
};
struct NexonSynchronize : public NexonPacketHeader
{
	//	const BYTE PacketType;
	BYTE IsMonitoring;//받은값 그대로 전송해야함 
	DWORD Count;

	NexonSynchronize() : NexonPacketHeader(NEXON_SYNC)
	{
		Init();
	}

	void Init()
	{
		PacketType = NEXON_SYNC;
		IsMonitoring = 0;
		Count = 0;
 
	}
	void SetInfo(const BYTE cIsMonitor, const int iCount)
	{
		PacketType = NEXON_SYNC;
		IsMonitoring = cIsMonitor;
		Count = iCount;
 
		size = sizeof(NexonSynchronize) - sizeof(NexonPacketHeader)  + sizeof(PacketType);
	}

	void Htonl()
	{		
		NexonPacketHeader::Htonl();
 
		Count = htonl( Count );  
	}								    

	void Ntohl()					    
	{ 								    
		NexonPacketHeader::Ntohl();	    
 
		Count = ntohl( Count ); 
	}
};

struct NexonSendSyncUser
{
	uint64_t SessionNo;
	BYTE SessionAlived;
	
	void SetInfo(const uint64_t iSessionNo, const BYTE iSessionState)
	{
		SessionNo = iSessionNo;
		SessionAlived = iSessionState;
	}
	void Htonl() { SessionNo = htonll( SessionNo ); }								    

	void Ntohl() { SessionNo = ntohll( SessionNo ); }

	int GetSize() { return sizeof(NexonSendSyncUser); }

};

struct NexonAlive : public NexonPacketHeader
{
	//const BYTE PacketType;

	NexonAlive() : NexonPacketHeader(NEXON_ALIVE) { };

	void Init()
	{
		PacketType = NEXON_ALIVE;
		size = sizeof(NexonAlive) - sizeof(NexonPacketHeader)  + sizeof(PacketType);
	}
	void Htonl()
	{
		NexonPacketHeader::Htonl();
	}
	void Ntohl()
	{
		NexonPacketHeader::Ntohl();
	}

};

#pragma pack()