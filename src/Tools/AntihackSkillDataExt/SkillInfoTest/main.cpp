#include <windows.h>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <tchar.h>

using namespace std;

//전체 공용으로 씀
struct sGrowthInfo
{
	int id;
	int infoNum;
	int iType;

	float fValue1;
	float fValue2;
	float fValue3;

	int iGap1;
	int iGap2;
};

std::vector<sGrowthInfo> g_vecGrowhInfo;
int GetGrowthInfo( sGrowthInfo& growInfo )
{
	//있나 체크
	int nSize = g_vecGrowhInfo.size();
	for( int i =0; i < nSize; ++i )
	{
		if( g_vecGrowhInfo[i].iType == growInfo.iType && g_vecGrowhInfo[i].fValue1 == growInfo.fValue1 && g_vecGrowhInfo[i].fValue2 == growInfo.fValue2 && g_vecGrowhInfo[i].fValue3 == growInfo.fValue3 &&
			g_vecGrowhInfo[i].iGap1 == growInfo.iGap1 && g_vecGrowhInfo[i].iGap2 == growInfo.iGap2 )
		{
			return g_vecGrowhInfo[i].id;
		}
	}

	sGrowthInfo gInfo;
	gInfo.id = nSize;
	gInfo.iType = growInfo.iType;
	gInfo.fValue1 = growInfo.fValue1;
	gInfo.fValue2 = growInfo.fValue2;
	gInfo.fValue3 = growInfo.fValue3;
	gInfo.iGap1 = growInfo.iGap1;
	gInfo.iGap2 = growInfo.iGap2;

	g_vecGrowhInfo.push_back( gInfo );

	return nSize;
}
struct skillinfo
{
	bool bSuccess;

	int id;
	std::string szName;
	int iSkillType;
	int iSkillSubType;	

	int iEquipType;
	int iGrowthValue;	//임시 값
	int iGrowthType;	//실제 나중에 쓸 값	
	
	int iBuffType;
	float fGaugeRate;
	bool bHasGauge;
	float fNeedGauge;
	float fMaxGauge;
	//스킬타입 7의 경우엔 추가 정보 2개 더..
	int iMaxCount;
	int iNeedCount;
	int iTickTime;

	std::string buffSkill;

	bool disableGrowth;
	float fGqugePerTick;
	skillinfo() : bSuccess(false), bHasGauge(false), iGrowthType(-1), iBuffType(-1) {}
};
std::vector<skillinfo> g_vecSkillInfo;

float GetPrivateProfileFloat( char* szAppName, const char* szKey, const char* szPath, float fDefault = -1.f )
{
	float fReturn = fDefault;

	char szName[256]={0,};
	GetPrivateProfileString( szAppName, szKey, "", szName, 256, szPath );
	if( strlen(szName) > 0 )
		fReturn = (float)_tstof(szName);

	return fReturn;
}

void WritePrivateProfileFloat( char* szAppName, const char* szKey, const char* szPath, float fValue )
{
	char szName[256]={0,};
	sprintf_s( szName, "%.1f", fValue );
	WritePrivateProfileString( szAppName, szKey, szName, szPath );
}

void WritePrivateProfileInt( char* szAppName, const char* szKey, const char* szPath, int iValue )
{
	char szName[256]={0,};
	sprintf_s( szName, "%d", iValue );
	WritePrivateProfileString( szAppName, szKey, szName, szPath );
}

std::vector<skillinfo> DoSkillIni( const char* szPath )
{
 	std::vector<skillinfo> vecSkillInfo;
	int nSkillCount = GetPrivateProfileInt( "common", "skill_cnt", 0, szPath );
	if( nSkillCount == 0 )
	{
		printf("file(%s) - skill_cnt size error", szPath );
		assert(0);
	}
	else
	{
		for( int i = 0; i < nSkillCount; ++i )
		{
			skillinfo sInfo;

			char szAppName[32]={0,};
			sprintf_s( szAppName, "skill%d", i+1 );

			char szName[256]={0,};
			GetPrivateProfileString( szAppName, "name", "", szName, 256, szPath );
			if( strlen( szName ) == 0 )
			{
				printf("file(%s) - skillName error", szPath );
				assert(0);
			}
			sInfo.szName = szName;
			if( sInfo.szName.empty() )
				assert(0);

			sInfo.iSkillType = GetPrivateProfileInt( szAppName, "type", -1, szPath );
			if( sInfo.iSkillType == -1 )
				assert(0);
			sInfo.iSkillSubType = GetPrivateProfileInt( szAppName, "sub_type", -1, szPath );
			if( sInfo.iSkillSubType == -1 )
				assert(0);
			sInfo.fNeedGauge = GetPrivateProfileFloat( szAppName, "need_gauge", szPath, -1.f );
			if( sInfo.iSkillType != 7 && sInfo.iSkillType != 4 && sInfo.fNeedGauge == -1.f )
			{
				//예외 처리(왜 얘는 카운트 타입인데 3으로 되어 있는지 ??)
				if( sInfo.szName == "088ES_lswc2k13_as" )
					sInfo.iSkillType = 7;
				else
					assert(0);
			}
			sInfo.fGaugeRate = GetPrivateProfileFloat( szAppName, "skill_gauge_inc_rate", szPath, 1.f );

			GetPrivateProfileString( szAppName, "owner_buff1", "", szName, 256, szPath );
			sInfo.buffSkill = szName;

			// 카운팅  타입일 경우
			if( sInfo.iSkillType == 7 )
			{
				if( sInfo.iSkillSubType == 9 )
				{
					//일단 스킵
					continue;
				}

				/*max_use_count = 99
				need_count    = 30*/
				sInfo.iMaxCount = GetPrivateProfileInt( szAppName, "max_use_count", -1, szPath );
				sInfo.iNeedCount = GetPrivateProfileInt( szAppName, "need_count", -1, szPath );
				sInfo.iTickTime = GetPrivateProfileInt( szAppName, "default_recovery_count_tic", 125, szPath );

				if( sInfo.iMaxCount < 0 )
					assert(0);
				if( sInfo.iNeedCount < 0 )
					assert(0);
				if( sInfo.iTickTime <= 0 )
					assert(0);
			}

			vecSkillInfo.push_back( sInfo );
		}
	}




	return vecSkillInfo;
}

struct sItemInfo
{
	std::string szName;
	int iEquipType;
	int growValue;
	bool bHasSkillGauge;
	float fMaxSkillGauge;

	std::string szChangeName;

};

int GetEquipType( const char* szName )
{
	std::string pName = szName;
	if( pName == "CLOAK" )
		return 3;
	else if( pName == "HELMET" )
		return 2;
	else if( pName == "ARMOR" )
		return 1;
	else if( pName == "WEAPON" )
		return 0;
	else if( pName == "OBJECT" )
		return 5;

	return -1;
}

void GetItemInfo( std::vector<sItemInfo>& vecItemInfos, const char* pIni )
{
	const char* szPath = pIni;
	int nItemCount = GetPrivateProfileInt( "common", "item_cnt", 0, szPath );
	if( nItemCount == 0 )
		return;
	else
	{
		for( int i = 0; i < nItemCount; ++i )
		{
			sItemInfo itemInfo;

			char szAppName[32]={0,};		
			sprintf_s( szAppName, "item%d", i+1 );

			char szName[256]={0,};

			GetPrivateProfileString( szAppName, "item_type", "", szName, 256, szPath );
			itemInfo.iEquipType = GetEquipType( szName );
			if( itemInfo.iEquipType == -1 )
				assert(0);
			if( itemInfo.iEquipType == 5 )
			{
				GetPrivateProfileString( szAppName, "skill0", "", szName, 256, szPath );
				itemInfo.szName = szName;
				vecItemInfos.push_back( itemInfo );
				continue;
			}	

			GetPrivateProfileString( szAppName, "skill0", "", szName, 256, szPath );
			itemInfo.szName = szName;
			if( itemInfo.szName.empty() )
			{
				assert(0);
			}
			else
			{
				//같은 이름 검색해서 있으면 스킵..
				int tSize = vecItemInfos.size();
				for( int j = 0; j < tSize; ++j )
				{
					if( szName == vecItemInfos[j].szName )
						continue;
				}
			}

			//changeskill이 있을경우 이것도..
			GetPrivateProfileString( szAppName, "change_skill_name", "", szName, 256, szPath );
			itemInfo.szChangeName = szName;


			itemInfo.bHasSkillGauge = true; // 0인게 없음
			itemInfo.fMaxSkillGauge = GetPrivateProfileFloat( szAppName, "max_skill_gauge", szPath, -1.f );
			if( itemInfo.fMaxSkillGauge < 0.f )
			{
				assert(0);
			}

			itemInfo.growValue = GetPrivateProfileInt( szAppName, "growth_info_num1", -1, szPath );
			if( itemInfo.growValue == -1 )
			{
				//여기서 강시에 대한 값들 때문에 강시는 예외처리..
				if( itemInfo.szName == "Gangsi_TempSkill" )
					continue;
				else
					assert(0);
			}

			vecItemInfos.push_back( itemInfo );
		}
	}
}

void DoItemIni( const char* pExtraItemIni, const char* pItemIni, std::vector<skillinfo>& vecSkillInfo, const char* pPowerItemIni = NULL, const char* pPowerExItemIni = NULL )
{
	std::vector<sItemInfo> vecItemInfos;

	//일단 item.ini에서 먼저 가져오고, 그 후에 extra에서 가져옴..

	GetItemInfo( vecItemInfos, pItemIni );
	GetItemInfo( vecItemInfos, pExtraItemIni );
	if( pPowerItemIni )
		GetItemInfo( vecItemInfos, pPowerItemIni );
	if( pPowerExItemIni )
		GetItemInfo( vecItemInfos, pPowerExItemIni );

	
	int nSize = vecSkillInfo.size();
	int nItemSize = vecItemInfos.size();

	for( int i = 0; i < nSize; ++i )
	{
		for( int j = 0; j < nItemSize; ++j )
		{
			if( vecSkillInfo[i].szName == vecItemInfos[j].szName )
			{
				vecSkillInfo[i].fMaxGauge = vecItemInfos[j].fMaxSkillGauge;
				vecSkillInfo[i].iGrowthValue = vecItemInfos[j].growValue;
				vecSkillInfo[i].iEquipType = vecItemInfos[j].iEquipType;
				vecSkillInfo[i].bHasGauge = true;
				vecSkillInfo[i].bSuccess = true;
				
				break;
			}
		}

		//못찾음
		if( vecSkillInfo[i].bHasGauge == false )
		{

			//못 찾으면 ex 스킬인지 체크해서 ex전에껄로 매칭해줌

			std::string szEXname = vecSkillInfo[i].szName;
			std::string szEXNameOri =vecSkillInfo[i].szName;
			transform(szEXname.begin(), szEXname.end(), szEXname.begin(), toupper);

			int nPos = szEXname.find( "_EX" );
			if( nPos < 0 )
				nPos = szEXname.find( "_HW" );
			if( nPos < 0 )
				nPos = szEXname.find( "_WOLF" );
			if( nPos < 0 )
				nPos = szEXname.find( "_RIDING" );

			if( nPos > 0 )
			{
				std::string exName = szEXNameOri.substr( 0, nPos );
				for( int j = 0; j < nItemSize; ++j )
				{
					if( exName == vecItemInfos[j].szName )
					{
						vecSkillInfo[i].fMaxGauge = vecItemInfos[j].fMaxSkillGauge;
						vecSkillInfo[i].iGrowthValue = vecItemInfos[j].growValue;
						vecSkillInfo[i].bHasGauge = true;
						vecSkillInfo[i].bSuccess = true;
						break;
					}
				}
			}
			else
			{
				for( int j = 0; j < nItemSize; ++j )
				{
					if( vecSkillInfo[i].szName == vecItemInfos[j].szChangeName )
					{
						vecSkillInfo[i].fMaxGauge = vecItemInfos[j].fMaxSkillGauge;
						vecSkillInfo[i].iGrowthValue = vecItemInfos[j].growValue;
						vecSkillInfo[i].bHasGauge = true;
						vecSkillInfo[i].bSuccess = true;
						break;
					}
				}
			}

			//이렇게 되면 실제로 스킬정보는 있는데 아이템정보가 없는 경우이니..기록만
			if( vecSkillInfo[i].bHasGauge == false && vecSkillInfo[i].iEquipType != 5 )
			{
				vecSkillInfo[i].bSuccess = false;
				//assert(0);
				int a = 0;
				a++;
			}
		}
	}

	//여기서 만들어지지 못한것은 삭제..
	auto it = vecSkillInfo.begin();
	while( it != vecSkillInfo.end() )
	{
		if( it->bSuccess == false )
			it = vecSkillInfo.erase( it );
		else
			++it;
	}
}
void DoGrowthIni( const char* pGrowthIni, std::vector<skillinfo>& vecSkillInfo )
{
	std::vector<sGrowthInfo> vecGrowths;

	const char* szPath = pGrowthIni;
	int nItemCount = GetPrivateProfileInt( "common", "growth_info_cnt", 0, szPath );
	if( nItemCount == 0 )
	{
		cout << "file : " << pGrowthIni << " - itemcount zero" << std::endl;
		assert(0);
	}
	else
	{
		for( int i = 0; i < nItemCount; ++i )
		{
			char szAppName[32]={0,};		
			sprintf_s( szAppName, "growth_info_%d", i+1 );

			sGrowthInfo sInfo;
			sInfo.infoNum = GetPrivateProfileInt( szAppName, "growth_info_num", -1, szPath );
			sInfo.iType = GetPrivateProfileInt( szAppName, "type", -1, szPath );
			sInfo.fValue1 = GetPrivateProfileFloat( szAppName, "max_value_section1", szPath, -1.f );
			sInfo.fValue2 = GetPrivateProfileFloat( szAppName, "max_value_section2", szPath, -1.f );
			sInfo.fValue3 = GetPrivateProfileFloat( szAppName, "max_value_section3", szPath, -1.f );

			sInfo.iGap1 = GetPrivateProfileInt( szAppName, "max_value_section1_gap", -1, szPath );
			sInfo.iGap2 = GetPrivateProfileInt( szAppName, "max_value_section2_gap", -1, szPath );

			vecGrowths.push_back( sInfo );
		}
	}

	int nSize = vecSkillInfo.size();
	int nItemSize = vecGrowths.size();

	for( int i = 0; i < nSize; ++i )
	{
		for( int j = 0; j < nItemSize; ++j )
		{
			if( vecSkillInfo[i].iGrowthValue == vecGrowths[j].infoNum )
			{
				vecSkillInfo[i].iGrowthType = GetGrowthInfo( vecGrowths[j] );
				break;
			}
		}
	}

	//없는거 있나 체크..
	for( int i = 0; i < nSize; ++i )
	{
		if( vecSkillInfo[i].iGrowthType == -1 )
		{
			printf("growth.ini error - file(%s) - name(%s) \n", pGrowthIni, vecSkillInfo[i].szName.c_str() );
		}	
	}
}

struct sBuffInfo
{
	int iType;
	std::string buffName;
	int iTickTime;
};

void DoBuffIni( const char* pBuffIni, std::vector<skillinfo>& vecSkillInfo )
{
	std::vector<sBuffInfo> vecBuff;

	const char* szPath = pBuffIni;
	int nItemCount = GetPrivateProfileInt( "common", "buff_cnt", 0, szPath );
	if( nItemCount == 0 )
		assert(0);
	else
	{
		for( int i = 0; i < nItemCount; ++i )
		{
			sBuffInfo sBuff;

			char szAppName[32]={0,};		
			sprintf_s( szAppName, "buff%d", i+1 );
			sBuff.iType = GetPrivateProfileInt( szAppName, "type", -1, szPath );

			char szTemp[64]={0,};
			GetPrivateProfileString( szAppName, "name", "", szTemp, 64, szPath );
			sBuff.buffName = szTemp;

			sBuff.iTickTime = GetPrivateProfileInt( szAppName, "tick_time", -1, szPath );
		}
	}

	int nSize = vecSkillInfo.size();
	for( int i = 0; i < nSize; ++i )
	{
		if( !vecSkillInfo[i].buffSkill.empty() )
		{

			int jSize = vecBuff.size();
			for( int j = 0; j < jSize; ++j )
			{
				if( vecSkillInfo[i].buffSkill == vecBuff[j].buffName )
				{
					vecSkillInfo[i].iBuffType = vecBuff[j].iType;
					vecSkillInfo[i].iTickTime = vecBuff[j].iTickTime;
					break;
				}
			}
		}

	}
}

void DoSkillInfo( std::string& strPath )
{
	/*
	001에서 가져올 파일은 6개..
	일단 스킬파일에서 먼저 	
	*/

	char szCurPath[256]={0,};
	std::string szNewPath;
	GetCurrentDirectory( 256, szCurPath );
	szNewPath = szCurPath;
	szNewPath += "\\";
	szNewPath += strPath;

	SetCurrentDirectory( szNewPath.c_str() );

	std::string szSkillIni;
	std::string szBuffIni;
	std::string szExtraIni;
	std::string szGrowIni;
	std::string szItemIni;

	std::string szPowerItemIni;
	std::string szPowerExItemIni;

	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;	
	hFindFile = FindFirstFile( "*", &FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				std::string strPath = FindFileData.cFileName;
				if( strPath.size() > 2 )
				{
					//여기서 attack.ini 등의 파일에 대한 파싱을..
					char firstCh = strPath[4];

					switch( firstCh )
					{
// 					case 'a':
// 						break;
					case 'b':
						szBuffIni = FindFileData.cFileName;
						break;
					case 'e':
						szExtraIni = FindFileData.cFileName;
						break;
					case 'g':
						szGrowIni = FindFileData.cFileName;
						break;
					case 'i':
						szItemIni = FindFileData.cFileName;
						break;
					case 's':
						szSkillIni = FindFileData.cFileName;
						break;
					case 'p':
						{
							//power item
							if( strPath[12] == 'e' )
								szPowerExItemIni = FindFileData.cFileName;
							else if( strPath[12] == 'i' )
								szPowerItemIni = FindFileData.cFileName;
						}
						break;
					}

				}
			}
		} while(FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}

	if( szSkillIni.empty() || (szItemIni.empty() && szExtraIni.empty()) || szGrowIni.empty() || szBuffIni.empty() )
	{
		assert(0);
		return;
	}

	// 1. skill.ini
	//갯수만큼 만들어주고 이하 작업으로 채워준후에 완성되면 넣어줌..
	// id는 완성하고 1번부터 인덱싱해줌

	std::string dotPath = ".\\";
	std::string rsPath = dotPath;
	std::string rsPath2 = dotPath;
	std::string rsPath3 = dotPath;
	std::string rsPath4 = dotPath;

	rsPath += szSkillIni.c_str();
	std::vector<skillinfo> vecSkills = DoSkillIni( rsPath.c_str() );

	// 2. item.ini 와 extraitem.ini  >>  여기서 item과extraitem이 대부분 같고 아이콘과 mesh 정도만 조금 다른듯..
	// extraitem에서 가져오고, item과 비교해보는 정도로만..

	rsPath = dotPath;
	rsPath2 = dotPath;
	rsPath += szExtraIni.c_str();
	rsPath2 += szItemIni.c_str();


	if( !szPowerItemIni.empty() )
	{
		rsPath3 += szPowerItemIni.c_str();

		if( !szPowerExItemIni.empty() )
			rsPath4 += szPowerExItemIni.c_str();

		DoItemIni( rsPath.c_str(), rsPath2.c_str(), vecSkills, rsPath3.c_str(), rsPath4.c_str() );
	}
	else
		DoItemIni( rsPath.c_str(), rsPath2.c_str(), vecSkills );

	// 3.growth.ini 에서 가져옴
	rsPath = dotPath;
	rsPath += szGrowIni.c_str();
	DoGrowthIni( rsPath.c_str(), vecSkills );

	// 4. buff.ini 에서 가져옴
	rsPath = dotPath;
	rsPath += szBuffIni.c_str();
	DoBuffIni( rsPath.c_str(), vecSkills );

	int nSize = vecSkills.size();
	for( int i = 0; i < nSize; ++i )
		g_vecSkillInfo.push_back( std::move(vecSkills[i]));

	SetCurrentDirectory( szCurPath );
};

void main()
{
	g_vecSkillInfo.reserve( 820 );
	// 작업 디렉토리 mercenary 폴더 기준으로
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;
	hFindFile = FindFirstFile("*", &FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 000~1xx 까지 돌림
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string strPath = FindFileData.cFileName;
				// . 이랑 .. 빼려고
				if( strPath.size() > 2 )
				{
					//000만 안 하니깐 이것만 안 하게 하드코딩
					int iPos = strPath.find( "000" );
					if( iPos < 0 )
					{
						DoSkillInfo(strPath);
					}
				}
			}
		} while(FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}

	//스킬 정보 파싱이 끝났으면 새로운 스킬.ini 파일로
	const char* szINI = ".\\ls_skill.ini";
	
	// common
	WritePrivateProfileFloat( "default", "default_recovery_gauge", szINI, 0.1f );
	WritePrivateProfileInt( "default", "default_recovery_gauge_tic", szINI, 100 );	
	int nGrowthSize = g_vecGrowhInfo.size();
	WritePrivateProfileInt( "default", "max_value_info_cnt", szINI, nGrowthSize );
	int nSkillSize = g_vecSkillInfo.size();
	WritePrivateProfileInt( "default", "skill_cnt", szINI, nSkillSize );
	WritePrivateProfileFloat( "default", "error_rate", szINI, 0.2f );

	for( int i = 0; i < nGrowthSize; ++i )
	{
		char szTitle[32] = {0,};
		sprintf_s( szTitle, "max_value_info%d", i+1 );

		WritePrivateProfileInt( szTitle, "maxvalueid", szINI, g_vecGrowhInfo[i].id );
		WritePrivateProfileInt( szTitle, "maxvalueType", szINI, g_vecGrowhInfo[i].iType );

		WritePrivateProfileFloat( szTitle, "max_value_section1", szINI, g_vecGrowhInfo[i].fValue1 );
		WritePrivateProfileFloat( szTitle, "max_value_section2", szINI, g_vecGrowhInfo[i].fValue2 );
		WritePrivateProfileFloat( szTitle, "max_value_section3", szINI, g_vecGrowhInfo[i].fValue3 );

		WritePrivateProfileInt( szTitle, "max_value_section1_gap", szINI, g_vecGrowhInfo[i].iGap1 );
		WritePrivateProfileInt( szTitle, "max_value_section2_gap", szINI, g_vecGrowhInfo[i].iGap2 );
	}
	
	for( int i = 0; i < nSkillSize; ++i )
	{
		g_vecSkillInfo[i].id = i+1;

		char szTitle[32] = {0,};
		sprintf_s( szTitle, "skill_%d", i+1 );

		WritePrivateProfileInt( szTitle, "skillid", szINI, g_vecSkillInfo[i].id );
		WritePrivateProfileString( szTitle, "skillname", g_vecSkillInfo[i].szName.c_str(), szINI );
		WritePrivateProfileInt( szTitle, "skilltype", szINI, g_vecSkillInfo[i].iSkillType );
		WritePrivateProfileInt( szTitle, "sub_type", szINI, g_vecSkillInfo[i].iSkillSubType );
		WritePrivateProfileInt( szTitle, "equiptype", szINI, g_vecSkillInfo[i].iEquipType );
		WritePrivateProfileInt( szTitle, "bufftype", szINI, g_vecSkillInfo[i].iBuffType );
		
		WritePrivateProfileInt( szTitle, "max_value_type", szINI, g_vecSkillInfo[i].iGrowthType );

		WritePrivateProfileFloat( szTitle, "gaugerate", szINI, g_vecSkillInfo[i].fGaugeRate );


		if( g_vecSkillInfo[i].iSkillType == 7 )
		{
			WritePrivateProfileInt( szTitle, "needgauge", szINI, g_vecSkillInfo[i].iNeedCount );
			WritePrivateProfileInt( szTitle, "gaugerate", szINI, g_vecSkillInfo[i].iMaxCount );
			WritePrivateProfileInt( szTitle, "ticktime", szINI, g_vecSkillInfo[i].iTickTime );
			
		}
		else
		{
			WritePrivateProfileFloat( szTitle, "needgauge", szINI, g_vecSkillInfo[i].fNeedGauge );
			WritePrivateProfileFloat( szTitle, "maxgauge", szINI, g_vecSkillInfo[i].fMaxGauge );
		}
	}
}