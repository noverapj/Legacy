#pragma once

#include <string>

struct NX_InitializeSoap
{
};

struct NX_IsBillingUser
{
	BYTE reason;
	std::string	ID;
	std::string	IP;
};

struct NX_GetPurse
{
	BYTE reason;
	std::string	ID;
	std::string	IP;
};

struct NX_Purchase
{
	short productType;
	std::string amount;
	BYTE reason;
	std::string	ID;
	std::string	IP;
	std::string	transactionID;
	std::string	productCode;
	std::string	gameID;
	std::string	serverID;
	std::string	orderNO;
};

struct NX_Present
{
	short productType;
	BYTE reason;
	std::string	ID;
	std::string	IP;
	std::string	transactionID;
	std::string	productCode;
	std::string	amount;
	std::string	gameID;
	std::string	friendID;
	std::string	serverID;
	std::string	orderNO;
};

struct NX_UsageCancel
{
	short productType;
	int amount;
	std::string	ID;
	std::string	transactionID;
};


struct NX_UsageCancelByUsageSN
{
	std::string	ID;
	int usageSN;
	int amount;
};
