// libtest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//



#include "stdafx.h"

void ini_file_load();
void ini_file_load_quest();
void SetQuestData( DWORD dwMainIndex, DWORD dwSubIndex, ioINILoader &rkLoader );

int _tmain(int argc, _TCHAR* argv[])
{
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 80 );

	/*for( int i = 0 ; i < 3 ; ++i )
	{
		ini_file_load();
		ini_file_load_quest();
	}
	cout << "end..." << endl;*/
	
	{
		ioINILoader kLoader;
		kLoader.SetFileName( "Gameserverinfo.ini" );

		// 파일 바로 읽기.
		bool boolvalue = kLoader.ReadBool( "security", "on", 0 );
		int intvalue = kLoader.ReadInt( "memorypool", "server_pool", 777 );
		float floatvalue = kLoader.ReadFloat( "classprice", "loadtime", 0 );
		char buff[ 256 ];
		int len = 256;
		kLoader.ReadString( "feature", "name", "", buff, len );
	}


	{
		ioINILoader kLoader2( "Gameserverinfo.ini" );

		// 전체 reload
		//kLoader2.ReloadFile( "Gameserverinfo.ini" );

		// 부분 reload [feature]
		kLoader2.ReloadFile( "Gameserverinfo.ini", "feature" );

		// 
		kLoader2.SetTitle( "feature" );
		int arrayint = kLoader2.LoadInt( "Array", 0 );

		//remove
		//kLoader2.RemoveFile( "Gameserverinfo.ini" );
	}

	int i = 0;
	cin >> i;

	//_CrtDumpMemoryLeaks();

	return 0;
}

void ini_file_load()
{
	DWORD time = GetTickCount();

	int value = 0;
	int temp[ 65535 ] = { 0, };

	// 게임서버 ini파일 로드
	ioINILoader kLoader( "sp2_monster_treasure_card.ini" );

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

	DWORD result = GetTickCount() - time;
	cout << "sp2_monster_treasure_card result : " << result << endl;
}

void ini_file_load_quest()
{
	DWORD time = GetTickCount();

	ioINILoader kLoader( "sp2_quest_info.ini" );
	kLoader.SetTitle( "common" );

	int iMaxQuest = kLoader.LoadInt( "max_quest", 0 );
	for(int i = 0;i < iMaxQuest;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "quest%d", i + 1 );
		kLoader.SetTitle( szTitle );

		char szClassName[MAX_PATH] = "";
		kLoader.LoadString( "class_name", "", szClassName, MAX_PATH );

		DWORD dwMainIndex = kLoader.LoadInt( "main_index", 0 );
		int iMaxSubQuest = kLoader.LoadInt( "max_sub_quest", 0 );			
		for(int j = 0;j < iMaxSubQuest;j++)
		{
			SetQuestData( dwMainIndex, j + 1, kLoader );
		}
	}

	DWORD result = GetTickCount() - time;
	cout << "sp2_quest_info result : " << result << endl;
}

void SetQuestData( DWORD dwMainIndex, DWORD dwSubIndex, ioINILoader &rkLoader )
{
	int value = 0;
	int temp[ 65535 ] = { 0, };

	DWORD m_dwMainIndex = dwMainIndex;
	DWORD m_dwSubIndex  = dwSubIndex;

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	sprintf_s( szKey, "sub%d_title", m_dwSubIndex );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	//m_szTitle = szBuf;

	sprintf_s( szKey, "sub%d_perform_type", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_oneday_style", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_repeat_style", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_guild_style", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_pcroom_style", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_channeling_type", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_occur_value", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_occur_mode", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_occur_room_style", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_complete_value", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_complete_mode", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_complete_room_style", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, -1 );

	sprintf_s( szKey, "sub%d_period_hour", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	//m_vRewardPresent.clear();
	sprintf_s( szKey, "sub%d_max_reward", m_dwSubIndex );
	int iMaxReward = rkLoader.LoadInt( szKey, 0 );
	int i = 0;
	for(;i < iMaxReward;i++)
	{
		sprintf_s( szKey, "sub%d_reward_present%d", m_dwSubIndex, i + 1 );
		temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );
	}

	sprintf_s( szKey, "sub%d_prev_main_index", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_prev_sub_index", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_next_main_index", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_next_sub_index", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_complete_game_alarm", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadBool( szKey, false );

	sprintf_s( szKey, "sub%d_complete_web_alarm", m_dwSubIndex );	
	temp[ value++ ] = rkLoader.LoadBool( szKey, false );

	//m_CustomValue.clear();
	int iDefCustom = -999999999;
	for(i = 0;i < 50;i++)
	{
		sprintf_s( szKey, "sub%d_custom_value%d", m_dwSubIndex, i + 1 );	
		int iCustomValue = rkLoader.LoadInt( szKey, iDefCustom );
		if( iCustomValue == iDefCustom ) 
			break;
		//m_CustomValue.push_back( iCustomValue );
	}

	// 이벤트 퀘스트 시간 로드
	sprintf_s( szKey, "sub%d_start_year", m_dwSubIndex );			
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_start_month", m_dwSubIndex );		
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_start_date", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_start_hour", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_end_year", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_end_month", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_end_date", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );

	sprintf_s( szKey, "sub%d_end_hour", m_dwSubIndex );
	temp[ value++ ] = rkLoader.LoadInt( szKey, 0 );
}
