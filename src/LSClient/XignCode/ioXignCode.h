#ifndef __ioXignCode_h__
#define __ioXignCode_h__

#ifdef XIGNCODE
#include "zwave_sdk_client.h"

#define MAX_XIGNCODE_PACKET_BUF 512

typedef struct tagXignCodePacket
{
	BYTE m_XignCodePacket[MAX_XIGNCODE_PACKET_BUF];

	tagXignCodePacket()
	{
		ZeroMemory( m_XignCodePacket, sizeof( m_XignCodePacket ) );
	}
}XignCodePacket;

class ioXignCode
{
public:
	enum CallBackType
	{
		CBT_NONE       = 0,
		CBT_SEND_CHECK = 3,
	};

	enum 
	{
		MAX_PACKET_BUF = 512,
	};
protected:
	static ioXignCode *sg_Instance;

protected:
	XignCodePacket m_XignCodePacket;
	bool           m_bRunCallBack;
	CallBackType   m_eCallBackType;
	
public:
	static ioXignCode &GetInstance();
	static void ReleaseInstance();

public:
	bool Start();
	void OnConnect();
	void OnDisconnect();
	void Cleanup();


	void SetPacket( const XignCodePacket &rkPacket );
	void SetRunCallBack(bool bRunCallBack) { m_bRunCallBack = bRunCallBack; }
	void SetCallBackType( CallBackType eCallBackType) { m_eCallBackType = eCallBackType; }
	void Process();
	void OnRecieveCheck( XignCodePacket &rkPacket );

private: // Singleton class
	ioXignCode(void);
	virtual ~ioXignCode(void);
};

#define g_ioXignCode ioXignCode::GetInstance()


#endif // XIGNCODE

#endif // __ioXignCode_h__