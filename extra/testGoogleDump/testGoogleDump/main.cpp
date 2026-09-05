#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <Windows.h>
#include "Abstract.h"
#include "GoogleDump.h"

void failApi()
{
	printf(NULL);
	printf("나는야 나는야 \n");
}

void failNull()
{
	int *a = NULL;
	*a = 1;
}

void failPureFunction()
{
	google_breakpad::Derived derive;
}

void CallMe(char buf[8])
{
	char call[8];
	strcpy(call, buf);
	CallMe(call);
}

int main()
{
	
	GoogleDump::Begin(_T("DumpTest"));
	//failApi();
	failNull();

	//failPureFunction();

	//char buf[8] = "cacacac";
	//CallMe(buf);

	GoogleDump::End();
}