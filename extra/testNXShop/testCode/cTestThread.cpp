#include "stdafx.h"
#include "cTestThread.h"
#include "MemPooler.h"
#include <algorithm>
#include "../../LS_NXSoap/LS_NXSoap/cNEXON.h"

using namespace std;

extern MemPooler<cNEXON> G_NEXON;


BOOL NexonInitialize(cNEXON& soap, char* IP, int& error);
BOOL NexonIsBillingUser(cNEXON& soap, char* ID, char* IP);
BOOL NexonGetPurse(cNEXON& soap, char* ID, char* IP);
BOOL NexonPurchase(cNEXON& soap, char* ID, char* IP, int amount, int& UsageSN);
BOOL NexonPresent(cNEXON& soap, char* ID, char* friendID, int age, int amount, char* IP, int& UsageSN);
BOOL NexonUsageCancelByUsageSN(cNEXON& soap, char* ID, int usageSN, int refundAmount);


cTestThread::cTestThread(void)
{
	Init();
}

cTestThread::~cTestThread(void)
{
	Destroy();
}

void cTestThread::Init()
{
}

void cTestThread::Destroy()
{
}


BOOL cTestThread::InitializeSoap(DWORD& time)
{
	DWORD begin = GetTickCount();

	char IP[64] = "112.218.160.51";
	char ID[64] = "lostsg002", friendID[64] = "lostsg003";
	int usageSN = 0;
	int amount = 300;
	int error = 0;

	cNEXON* test = G_NEXON.Pop();
	if(test)
	{
		if(NexonInitialize( *test, IP, error ))
		{
			NexonGetPurse( *test, ID, IP );
		}
		G_NEXON.Push( test );
		time = GetTickCount() - begin;
		return TRUE;
	}
	time = 0;
	return FALSE;
}

void cTestThread::Process()
{
	DWORD total = 0, count = 0;
	while(TRUE)
	{
		DWORD time = 0;
		if(InitializeSoap(time))
		{
			++count;
			total += time;

			if(total >= 1000)
			{
				printf("------------------ time : %lu, count : %lu\n", total, count);
				count = 0;
				total = 0;
			}
		}
		//Sleep(1);
	}
}
