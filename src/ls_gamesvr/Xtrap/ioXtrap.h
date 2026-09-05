#ifndef __ioXtrap_h__
#define __ioXtrap_h__

#ifdef XTRAP

class ioXtrap 
{
public:
	enum 
	{
		MAX_TIMEOUT_SEC       = 600,
		MAX_CS3FILE_COUNT     = 5,
		MAX_CS3FILE_DATA_SIZE = 13000,
		MAX_SESSION_BUF       = 320,
		MAX_PACKET_BUF        = 128,
		MAX_SERVER_IP         = 30,
		CHECK_TIME            = 30000, // ms  : xtrap 권장 20초, 30초까지는 가능, 30초 이상이면 체크시간이 보통 곱하기 4 소요되므로 문제 될 수 있음
		SIZE_ONE_READ         = 200,   // Byte
	};

protected:
	static ioXtrap *sg_Instance;

	static BYTE  m_byCS3Data[MAX_CS3FILE_COUNT][MAX_CS3FILE_DATA_SIZE];

	int   m_iCS3FileVersion[MAX_CS3FILE_COUNT];
	DWORD m_dwMethod;
	char  m_szServerIP[MAX_SERVER_IP];

	BYTE   m_byTempCS3Data[MAX_CS3FILE_DATA_SIZE];
	HANDLE m_hFile;
	DWORD  m_dwReadSize;

	bool                m_bUse;
	CRITICAL_SECTION	m_CriticalSection;
	ioHashString        m_MontoringGUID;
	int                 m_iOpenVersion;
	int                 m_iLastDeletVersion;
	int                 m_iChangeArray;

protected:
	int  GetCS3FileVersion( HANDLE hFile ); 
	void SafeRename( int iMapFileArray );

public:
	bool LoadDll( bool bUse );
	bool FreeDll();
	bool Start( const char *pServerIP );
	bool Init( BYTE *pSessionBuf, IN const char *szPublicID, IN const char *szPublicIP, IN int iLevel ); 
	bool Step1( BYTE *pSessionBuf , OUT BYTE *pPacketBuf, IN const char *szPublicID, IN const char *szPublicIP, IN int iLevel );
	bool Step3( BYTE *pSessionBuf, IN BYTE * pPacketBuf, IN const char *szPublicID, IN const char *szPublicIP, IN int iLevel ); // step2는 클라이언트 존재함. 

	bool LoadCS3File();
	bool LoadOneCS3File( const char *szPath, int iArray );

	// 2개가 한세트
	void OpenCS3File( const ioHashString &rszGUID, int iVersion, int iChange );
	void DivideLoadCS3File();
	//
	bool IsUse() const { return m_bUse; }

	void SendCS3Version( const ioHashString &rszGUID );
	void GetTextCS3Version( OUT char *szText, IN int iTextSize );

private: // Singleton class
	ioXtrap(void);
	virtual ~ioXtrap(void);
public:
	static ioXtrap &GetInstance();
	static void ReleaseInstance();
};

#define g_ioXtrap ioXtrap::GetInstance()

typedef struct tagXTrapPacket
{
	BYTE m_XTrapPacket[ioXtrap::MAX_PACKET_BUF];

	tagXTrapPacket()
	{
		ZeroMemory( m_XTrapPacket, sizeof( m_XTrapPacket ) );
	}
}XtrapPacket;

#endif // XTRAP

#endif // __ioXtrap_h__