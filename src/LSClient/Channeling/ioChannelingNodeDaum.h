#pragma once

#include "ioChannelingNodeParent.h"

class ioChannelingNodeDaum : public ioChannelingNodeParent
{
public:
	virtual ChannelingType GetType();
	virtual const char *GetININame();
	virtual const char *GetBillingErrorMent(ioHashString &sServerError);
	
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
	virtual bool IsSubscriptionRetract() { return true; }
	virtual bool IsSendFillCashURL() { return true; }
public:
	ioChannelingNodeDaum(void);
	virtual ~ioChannelingNodeDaum(void);
};

