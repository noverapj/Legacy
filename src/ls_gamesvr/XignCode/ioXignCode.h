#ifndef __ioXignCode_h__
#define __ioXignCode_h__

#ifdef XIGNCODE

#define MAX_XIGNCODE_PACKET_BUF 512

typedef struct tagXignCodePacket
{
	BYTE m_XignCodePacket[MAX_XIGNCODE_PACKET_BUF];

	tagXignCodePacket()
	{
		ZeroMemory( m_XignCodePacket, sizeof( m_XignCodePacket ) );
	}
}XignCodePacket;

class IXigncodeServer;

class ioXignCode
{

protected:
	static ioXignCode *sg_Instance;

protected:
	IXigncodeServer* m_pXignCode;

	bool  m_bUse;
	DWORD m_dwProcessTime;

public:
	bool Start( bool bUse );
	void End();
	void OnAccept( void *pUser );
	void SetUserInformation( void *pUser, const char *szIP, const char *szPrivateID );
	void OnReceive( void *pUser, BYTE *Data ); 
	void OnDisconnect( void *pUser );
	bool IsUse() const { return m_bUse; }

public:
	static ioXignCode &GetInstance();
	static void ReleaseInstance();

private: // Singleton class
	ioXignCode(void);
	virtual ~ioXignCode(void);
};

#define g_ioXignCode ioXignCode::GetInstance()

#endif // XignCode

#endif // __ioXignCode_h__