#ifndef __ioXtrap_h__
#define __ioXtrap_h__

#ifdef XTRAP

#pragma comment(lib, "Xtrap/XTrap_Unipher_mtDll.lib" ) // multi thread dll
#pragma comment(lib, "Xtrap/XTrap4Client_ClientPart_mtDll.lib" )
#pragma comment(lib, "Xtrap/XTrap4Client_mtDll.lib" )
#pragma comment(lib, "Xtrap/XTrap4Client_ServerPart_mtDll.lib" )
#pragma comment(lib, "Xtrap/XTrap4ClientExt1_mtdll.lib" )

	
class ioXtrap 
{
public:
	enum 
	{
		MAX_PACKET_BUF        = 128,
		MAX_PROCESS_MS        = 120000,
	};

protected:
	static ioXtrap *sg_Instance;

	DWORD m_dwMethod;
	bool  m_bPayment;
	DWORD m_dwPreTime;
		
public:
	bool Start();
	void KeepAlive();
	void SetUserInfo( IN LPCSTR lpUserID, IN LPCSTR lpServerName, IN LPCSTR lpCharacterName, IN LPCSTR lpCharacterClass, IN DWORD dwCharacterLevel );
	bool Step2( IN BYTE *pPacketBuf, OUT BYTE *pOutPacketBuf );
	bool StartPayment();
	bool EndPayment();
	void Process();

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