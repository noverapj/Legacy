#ifndef __LicenseManager_h__
#define __LicenseManager_h__

#define SHUTDOWN_KEY       "$Deidk#$0dls;w-3304-dGR;l3-3-2;dgjrkrRe"
#define LICENSE_SERVERIP   "210.118.58.224"
#define LICENSE_SERVERPORT 14009

#include "../Util/Singleton.h"

class LicenseManager : public Singleton< LicenseManager >
{
protected: 
	DWORD m_dwShutDownTime;
	DWORD m_dwSendTime;
	DWORD m_dwMinSendMinutes;
	DWORD m_dwMaxSendMinutes;

protected:
	void ProcessSend();
	void ProcessShutDown();

public:
	static LicenseManager& GetSingleton();

public:
	void OnLicense( sockaddr_in sender_addr,  SP2Packet &rkPacket );
	void Pocess();

public:
	LicenseManager(void);
	virtual ~LicenseManager(void);
};

#define g_LicenseMgr LicenseManager::GetSingleton()

#endif // __LicenseManager_h__