#pragma once

struct AnnounceInfo
{
	ioHashString szAnnounce;
	int          iMsgType;
	ioHashString szUserID;
	WORD         wYear;
	WORD         wMonth;
	WORD         wDay;
	WORD         wHour;
	WORD         wMinute;
	DWORD        dwEndTime;

	AnnounceInfo()
	{
		iMsgType  = ANNOUNCE_TYPE_ALL;
		wYear     = 0;
		wMonth    = 0;
		wDay      = 0;
		wHour     = 0;
		wMinute   = 0;
		dwEndTime = 0;
	}
};

typedef std::vector<AnnounceInfo> vAnnounceInfo;

class CAnnounceMgr
{
protected:
	static CAnnounceMgr *sg_Instance;

protected:
	vAnnounceInfo m_vAnnounceInfo;
	DWORD m_current_timer;

public:
	static CAnnounceMgr &GetInstance();
	static void ReleaseInstance();

public:
	void ProcessSendReservedAnnounce();
	void AddAnnouce(const AnnounceInfo &rAInfo);

private:     	/* Singleton Class */
	CAnnounceMgr(void);
	virtual ~CAnnounceMgr(void);
};

#define g_AnnounceManager CAnnounceMgr::GetInstance()
