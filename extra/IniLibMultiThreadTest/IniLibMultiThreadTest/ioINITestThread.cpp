#include "StdAfx.h"
#include "ioINITestThread.h"


ioINITestThread::ioINITestThread(void)
{
}


ioINITestThread::~ioINITestThread(void)
{
}

void ioINITestThread::Run()
{
	DWORD time = GetTickCount();

	int value = 0;
	int temp[ 65535 ] = { 0, };

	// 게임서버 ini파일 로드
	ioINILoader kLoader;
	if( kLoader.LoadFile( "config/sp2_monster_treasure_card.ini" ) == false )
	//if( kLoader.LoadFile( "config/test.ini" ) == false )
	{
		cout << "Ini File Read Error... ThreadID : " << GetCurrentThreadId() << endl;
	}
	else
	{
		cout << "success.. " << endl;
	}

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	kLoader.SetTitle( "common" );

	// 선물 리스트
	int iMaxTable = kLoader.LoadInt( "max_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s( szKey, "table%d", i + 1 );
		kLoader.SetTitle( szKey );
		
		// 공용
		kLoader.LoadString( "present_name", "", szBuf, MAX_PATH );

		temp[ value++ ] = kLoader.LoadInt( "present_state", 0 );
		temp[ value++ ] = kLoader.LoadInt( "present_ment", 0 );
		temp[ value++ ] = kLoader.LoadInt( "present_period", 0 );	

		int iMaxPresent = kLoader.LoadInt( "max_present", 0 );
		for(int k = 0;k < iMaxPresent;k++)
		{
			sprintf_s( szKey, "present%d_rand", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );			

			sprintf_s( szKey, "present%d_type", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "present%d_value1", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "present%d_value2", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );
		}
	}
	
	// 장비 선물의 기간+강화값 랜덤 리스트
	kLoader.SetTitle( "common" );
	iMaxTable = kLoader.LoadInt( "max_extraitem_rand_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s( szKey, "extraitem_rand_table%d", i + 1 );
		kLoader.SetTitle( szKey );

		int k = 0;
		int iMaxLimitDate = kLoader.LoadInt( "max_limit_date", 0 );
		for(k = 0;k < iMaxLimitDate;k++)
		{
			sprintf_s( szKey, "limit_date%d_rand", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "limit_date%d_value", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );
		}

		int iMaxReinforceDate = kLoader.LoadInt( "max_reinforce", 0 );
		for(k = 0;k < iMaxReinforceDate;k++)
		{
			sprintf_s( szKey, "reinforce%d_rand", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "reinforce%d_value", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );
		}
	}
	
	cout << "ThreadID : " << GetCurrentThreadId() << " ";
	cout << "sp2_monster_treasure_card result : " << GetTickCount() - time << endl;

	cout << endl;
}

void ioINITestThread::Test()
{
	DWORD time = GetTickCount();

	int value = 0;
	int temp[ 65535 ] = { 0, };

	// 게임서버 ini파일 로드
	ioINILoader kLoader;
	if( kLoader.LoadFile( "config/sp2_monster_treasure_card.ini" ) == false )
	//if( kLoader.LoadFile( "config/test.ini" ) == false )
	{
		cout << "Ini File Read Error... ThreadID : " << GetCurrentThreadId() << endl;
	}
	else
	{
		cout << "success.. " << endl;
	}

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	kLoader.SetTitle( "common" );

	// 선물 리스트
	int iMaxTable = kLoader.LoadInt( "max_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s( szKey, "table%d", i + 1 );
		kLoader.SetTitle( szKey );
		
		// 공용
		kLoader.LoadString( "present_name", "", szBuf, MAX_PATH );

		temp[ value++ ] = kLoader.LoadInt( "present_state", 0 );
		temp[ value++ ] = kLoader.LoadInt( "present_ment", 0 );
		temp[ value++ ] = kLoader.LoadInt( "present_period", 0 );	

		int iMaxPresent = kLoader.LoadInt( "max_present", 0 );
		for(int k = 0;k < iMaxPresent;k++)
		{
			sprintf_s( szKey, "present%d_rand", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );			

			sprintf_s( szKey, "present%d_type", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "present%d_value1", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "present%d_value2", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );
		}
	}
	
	// 장비 선물의 기간+강화값 랜덤 리스트
	kLoader.SetTitle( "common" );
	iMaxTable = kLoader.LoadInt( "max_extraitem_rand_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s( szKey, "extraitem_rand_table%d", i + 1 );
		kLoader.SetTitle( szKey );

		int k = 0;
		int iMaxLimitDate = kLoader.LoadInt( "max_limit_date", 0 );
		for(k = 0;k < iMaxLimitDate;k++)
		{
			sprintf_s( szKey, "limit_date%d_rand", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "limit_date%d_value", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );
		}

		int iMaxReinforceDate = kLoader.LoadInt( "max_reinforce", 0 );
		for(k = 0;k < iMaxReinforceDate;k++)
		{
			sprintf_s( szKey, "reinforce%d_rand", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "reinforce%d_value", k + 1 );
			temp[ value++ ] = kLoader.LoadInt( szKey, 0 );
		}
	}
	
	cout << "ThreadID : " << GetCurrentThreadId() << " ";
	cout << "sp2_monster_treasure_card result : " << GetTickCount() - time << endl;

	cout << endl;
}
