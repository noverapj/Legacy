#pragma once

#include "ioChannelingNodeParent.h"

class ioChannelingNodeMgame : public ioChannelingNodeParent
{
protected:
	ioHashString m_szCurrentDirectory; // 충전페이지를 호출하면 current directory 경로가 변경되어 경로를 복원하도록 함.

public:
	virtual ChannelingType GetType();
	virtual const char *GetININame();
	virtual const char *GetLogoSetName();

	virtual void GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL );
	virtual void GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey );

	virtual int GetCashPageWidth();
	virtual int GetCashPageHeight();

	virtual void OnCashPageHide();

	virtual bool IsCreateProcessFillCashWeb();

protected:
	virtual bool OnRecieveGetCash( SP2Packet &rkReceivePacket );

public:
	ioChannelingNodeMgame(void);
	virtual ~ioChannelingNodeMgame(void);
};

