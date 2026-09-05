// testCompress.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Encoder.h"
#include <string.h>



///////////////////////////////////////////////////////////////////////////////////
char G_INPUT[512] = "";
char G_OUTPUT[512] = {};
char G_TEXT[512] = {};
int N_INPUT = 0;
unsigned int N_OUTPUT = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CEncoder G_ENCODER;

void Compress(int count)
{
	for(int i = 0 ; i < count ; i++)
	{
		BOOL result = G_ENCODER.Pack((uint8*)G_INPUT, N_INPUT, (uint8*)G_OUTPUT, N_OUTPUT);
	}
}

void Decompress(int count)
{
	for(int i = 0 ; i < count ; i++)
	{
		unsigned int output;
		BOOL result = G_ENCODER.Unpack((uint8*)G_OUTPUT, N_OUTPUT, (uint8*)G_TEXT, output);
	}
}

int BitPacking(int count)
{
	DWORD current = GetTickCount();
	Compress(count);
	Decompress(count);
	int elapse = GetTickCount() - current;

	//printf("elapse : %lu\n", elapse);
	return elapse;
}

void lzo_compress(int count)
{
	for(int i = 0 ; i < count ; i++)
	{
		BOOL result = G_ENCODER.Compress((uint8*)G_INPUT, N_INPUT, (uint8*)G_OUTPUT, N_OUTPUT);
	}
}

void lzo_decompress(int count)
{
	for(int i = 0 ; i < count ; i++)
	{
		unsigned int output = 0;
		BOOL result = G_ENCODER.Decompress((uint8*)G_OUTPUT, N_OUTPUT, (uint8*)G_TEXT, output);
	}
}

int MiniLZO(int count)
{
 	DWORD current = GetTickCount();

	lzo_compress( count );
	lzo_decompress( count );

	int elapse = GetTickCount() - current;

	//printf("elapse : %lu\n", elapse);
    return elapse;
}


void MakeData(int size)
{
	char *temp = "1";
	N_INPUT =size;

	CopyMemory(G_INPUT, temp, N_INPUT);
	//printf("input : %d\n", N_INPUT );
}

int _tmain(int argc, _TCHAR* argv[])
{
	G_ENCODER.Startup();

	while(TRUE)
	{
		int input = 0;
		scanf_s("%d", &input);

		if(input == 0)
			break;

		int loopCount = 100;
		int testCount = 10000;
	
		int bp = 0, ml = 0;

		for(int i = 1 ; i <= loopCount ; i++)
		{
			MakeData(input);

			bp += BitPacking( testCount );
			ml += MiniLZO( testCount );

			//printf("loop %d end\n", i );
		}

		printf("Bitpacking elapse : %lu\n", bp);
		printf("MiniLZO elapse : %lu\n", ml);

		char c = getchar();
	}

	return 0;
}

