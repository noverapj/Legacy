#include "stdafx.h"
#include <stdio.h>
#include <list>
#include <algorithm>
#include "../../LS_HTTP/LS_HTTP/ioHTTP.h"

using namespace std;


int main() 
{ 
	ioHTTP test;

	TCHAR header[512];
	TCHAR request[512];
	TCHAR result[512];
	
	sprintf_s( header, "clientsecret:79e71078976146189f36191d1469a8cd" );
	sprintf_s( request, "{ \"requestKey\" : \"20140124132101_123\", \"nexonSN\" : 123123, \"questKey\" : \"q123\" }" );

	test.GetResultData(_T("https://apis-test.nexon.com/ADS/Playlock/NotifyPCQuestComplete/v1"), request, header, result, 512 );

	printf("------------------------------------------------------\n");
	printf("%s\n", request);
	printf("------------------------------------------------------\n");


	printf("------------------------------------------------------\n");
	printf("%s\n", result);
	printf("------------------------------------------------------\n");

	char c = getchar();
} 



 
