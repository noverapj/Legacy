#pragma once

#include "ioChannelingNodeParent.h"

class ioChannelingNodeHappyTalk : public ioChannelingNodeParent
{
public:
	virtual ChannelingType GetType();
	virtual const char *GetININame();
	virtual const char *GetLogoSetName();
	virtual const char *GetBillingErrorMent(ioHashString &sServerError);

	virtual void GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL );
	virtual void GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey );

	virtual int GetCashPageWidth();
	virtual int GetCashPageHeight();

	virtual void OnCashPageHide();

	virtual bool IsCreateProcessFillCashWeb();
	virtual bool IsSendFillCashURL() { return false; }

public:
	virtual bool OnRecieveGetCash( SP2Packet &rkReceivePacket );
	virtual bool OnRecieveAddCash( SP2Packet &rkReceivePacket );

public:
	virtual bool IsSubscriptionRetractCheck() { return true; }
	virtual bool IsSubscriptionRetract() { return true; }

public:
	ioChannelingNodeHappyTalk(void);
	virtual ~ioChannelingNodeHappyTalk(void);
};

