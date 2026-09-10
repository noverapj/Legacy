#pragma once

#include "ioChannelingNodeParent.h"

class ioChannelingNodeNaver : public ioChannelingNodeParent
{
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
	//virtual DWORD GetCashSendTime() { return 540000; } // ms 
	virtual DWORD GetCashSendTime() { return 15000; } // ms 

protected:
	virtual bool OnRecieveGetCash( SP2Packet &rkReceivePacket );

public:
	ioChannelingNodeNaver(void);
	virtual ~ioChannelingNodeNaver(void);
};

