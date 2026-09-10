#pragma once

#include "..\CharacterInfo.h"

class SP2Packet;

class ioChannelingNodeParent
{
public:
	static bool  IsBillingTestID( const ioHashString &rsPublicID );

public:
	virtual ChannelingType GetType() = 0;
	virtual const char *GetININame() = 0;
	virtual const char *GetLogoSetName() = 0;
	virtual const char *GetBillingErrorMent(ioHashString &sServerError);

	virtual void GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL ) = 0;
	virtual void GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey ) = 0;

	virtual int GetCashPageWidth() = 0;
	virtual int GetCashPageHeight() = 0;

	virtual void OnCashPageHide() = 0;

	virtual bool IsCreateProcessFillCashWeb() = 0;
	virtual bool IsSendFillCashURL() { return false; }

public:
	virtual bool OnRecieveGetCash( SP2Packet &rkReceivePacket ) = 0;
	virtual DWORD GetCashSendTime() { return 15000; } // ms
	virtual bool OnRecieveAddCash( SP2Packet &rkReceivePacket ){ return true; }

public:
	virtual bool IsSubscriptionRetractCheck() { return false; }
	virtual bool IsSubscriptionRetract() { return false; }

public:
	ioChannelingNodeParent(void);
	virtual ~ioChannelingNodeParent(void);
};

