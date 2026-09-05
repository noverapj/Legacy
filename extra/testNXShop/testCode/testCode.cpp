#include "stdafx.h"
#include "cTestThread.h"
#include "MemPooler.h"
#include <stdio.h>
#include <list>
#include <algorithm>
#include "../../LS_NXSoap/LS_NXSoap/cNEXON.h"

using namespace std;

SYSTEMTIME systime;

void PrintError(cNEXON& soap, const char* file)
{
	char buffer[4096];
	soap.GetError(buffer, 2048);

	FILE * fp;
	fp = fopen(file, "w+");
	if(fp)
	{
		fprintf(fp, "%s", buffer);
		fclose(fp);
	}
}

BOOL NexonInitialize(cNEXON& soap, char* IP, int& error)
{
	GetLocalTime(&systime);

	char TS[64];
	char hash[512] = "{82CFDDC1-E25F-41DD-8E2E-E93A4BF2C576}";

	sprintf_s(TS, sizeof(TS), "%04d%02d%02d%02d%02d%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

	BOOL ret = soap.InitializeSoap(TS, IP, hash, error);
	if(ret == FALSE)
	{
		printf("-초기화 실패\n");
		PrintError( soap, "initializesoap.txt" );
		return FALSE;
	}

	//printf("-초기화 성공\n");
	return TRUE;
}

BOOL NexonIsBillingUser(cNEXON& soap, char* ID, char* IP)
{
	int error = 0;

	time_t lastDate;
	BOOL ret = soap.IsBillingUser(ID, 1, IP, error, lastDate);
	if(ret == FALSE)
	{
		printf("-빌링유저 실패\n");
		PrintError( soap, "IsBilling.txt" );
		return FALSE;
	}

	printf("-빌링유저 성공 : %s, %I64d\n", ID, lastDate);
	return TRUE;
}

BOOL NexonGetPurse(cNEXON& soap, char* ID, char* IP)
{
	INT64 balance;
	int error = 0;
	BOOL ret = soap.GetPurse(ID, 1, IP, error, balance);
	if(ret == FALSE)
	{
		printf("-잔액조회 실패\n");
		PrintError( soap, "getpurse.txt" );
		return FALSE;
	}

	//printf("-잔액조회 성공 : %s, %I64d\n", ID, balance);
	return TRUE;
}

BOOL NexonPurchase(cNEXON& soap, char* ID, char* IP, int amount, int& UsageSN)
{
	char transactionID[512] = {};
	int productType = 112;
	char productCode[64] = "100020067";
	char serverID[64] = "1-게임서버1";
	char amountsz[64];

	sprintf_s(amountsz, sizeof(amountsz), "%d", amount);

	int error=0, result=0;

	GetLocalTime(&systime);
	sprintf_s(transactionID, sizeof(transactionID), "%04d%02d%02d%02d%02d%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

	BOOL ret = soap.Purchase(ID, transactionID, productType, productCode, amountsz, ID, serverID, transactionID, 1, IP, error, result);
	if(ret == FALSE)
	{
		PrintError( soap, "purchase.txt" );
		printf("-구매 실패\n");
		return FALSE;
	}

	UsageSN = error;
	printf("-구매 성공 : %s - %s, %d\n", productCode, ID, result);
	return TRUE;
}

BOOL NexonPresent(cNEXON& soap, char* ID, char* friendID, int age, int amount, char* IP, int& UsageSN)
{
	char transactionID[512] = {};
	int productType = 112;
	char productCode[64] = "100020067";
	char serverID[64] = "1-게임서버1";
	char amountsz[64];

	sprintf_s(amountsz, sizeof(amountsz), "%d", amount);

	int error=0, result=0;

	GetLocalTime(&systime);
	sprintf_s(transactionID, sizeof(transactionID), "%04d%02d%02d%02d%02d%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

	BOOL ret = soap.Present(ID, transactionID, productType, productCode, amountsz, ID, friendID, age, serverID, transactionID, 1, IP, error, result);
	if(ret == FALSE)
	{
		PrintError( soap, "purchase.txt" );
		printf("- 선물 실패\n");
		return FALSE;
	}

	UsageSN = error;
	printf("- 선물 성공 : %s - %s(to %s), %d\n", productCode, ID, friendID, result);
	return TRUE;
}

BOOL NexonUsageCancelByUsageSN(cNEXON& soap, char* ID, int usageSN, int refundAmount)
{
	int result = 0;

	BOOL ret = soap.UsageCancelByUsageSN(ID, usageSN, refundAmount, result);
	if(ret == FALSE)
	{
		PrintError( soap, "usagecancel.txt" );
		printf("-청약철회 실패\n");
		return FALSE;
	}

	printf("-청약철회 성공 : %s - %d(%d), %d\n", ID, usageSN, refundAmount, result);
	return TRUE;
}

const int G_COUNT = 1;
cTestThread G_THREAD[G_COUNT];
MemPooler<cNEXON> G_NEXON;

int main() 
{ 
	G_NEXON.CreatePool(G_COUNT, G_COUNT, TRUE);

	char IP[64] = "112.218.160.51";
	char ID[64] = "lostsg002", friendID[64] = "lostsg003";
	int usageSN = 0;
	int amount = 300;
	int error = 0;

	for(int i = 0; i < G_COUNT ; i++)
	{
		G_THREAD[i].StartThread();
	}
	
	//cNEXON test;
	////for(int i = 0 ; i < 1000 ; i++)
	//{
	//	NexonInitialize( test, IP, error );
	//	NexonGetPurse( test, ID, IP );

	//	//NexonInitialize( test, IP );
	//	//NexonPurchase( test, ID, IP, amount, usageSN);
	//	//NexonInitialize( test, IP );
	//	//NexonGetPurse( test, ID, IP );

	//	NexonInitialize( test, IP, error );
	//	NexonPresent( test, ID, friendID, 20, amount, IP, usageSN );

	//	NexonInitialize( test, IP, error );
	//	NexonGetPurse( test, ID, IP );

	//	NexonInitialize( test, IP, error );
	//	NexonUsageCancelByUsageSN( test, ID, usageSN, amount);

	//	NexonInitialize( test, IP, error );
	//	NexonGetPurse( test, ID, IP );
	//	//NexonInitialize( test, IP );
	//	//NexonIsBillingUser( test, ID, IP );
	//	//NexonPurchase( test, ID, IP );

	//}

	printf("-완료\n");
	char c = getchar();
} 



 
