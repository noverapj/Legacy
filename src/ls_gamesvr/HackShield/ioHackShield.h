#ifndef __ioHackShield_h__
#define __ioHackShield_h__

#ifdef HACKSHIELD

#include "../HackShield/AntiCpXSvr.h"
#pragma comment(lib, "HackShield/AntiCpXSvr.lib" )

class ioHackShield
{
public:
	enum 
	{
		CHECK_TIME       = 120000,         // 1∫– ±«¿Â
	};
protected:
	static ioHackShield *sg_Instance;

protected:
	bool  m_bUse;
	DWORD m_dwProcessTime;

	AHNHS_SERVER_HANDLE m_hServer;

public:
	bool Start( bool bUse );
	void End();

	AHNHS_CLIENT_HANDLE CreateClient();
	void CloseClient( AHNHS_CLIENT_HANDLE hClient );
	
	bool MakeRequest( IN AHNHS_CLIENT_HANDLE hClient, OUT AHNHS_TRANS_BUFFER &rkBuf );
	bool VerifyRespose( IN AHNHS_CLIENT_HANDLE hClient, IN AHNHS_TRANS_BUFFER &rkBuf );

	bool IsUse() const { return m_bUse; }

public:
	static ioHackShield &GetInstance();
	static void ReleaseInstance();

private: // Singleton class
	ioHackShield(void);
	virtual ~ioHackShield(void);
};

#define g_ioHackShield ioHackShield::GetInstance()

#endif // HACKSHIELD

#endif // __ioHackShield_h__