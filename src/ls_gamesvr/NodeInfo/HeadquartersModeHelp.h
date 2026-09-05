
#ifndef _HeadquartersModeHelp_h_
#define _HeadquartersModeHelp_h_

#include "User.h"

struct HeadquartersRecord : public ModeRecord
{
	int  iMonsterSyncCount;                               // 자신이 동기화를 맡고 있는 몬스터 갯수
	HeadquartersRecord()
	{
		iMonsterSyncCount = 0;
	}
};
typedef std::vector< HeadquartersRecord > HeadquartersRecordList;


struct HeadquartersCharRecord  
{
	// 일반 데이터
	DWORD        dwCode; 
	ioHashString szName;
	float        fStartXPos;
	float        fStartZPos;
	DWORD        dwStartTime;
	RecordState  eState;
	ioHashString szSyncUser;
	TeamType     eTeam;
	DWORD        dwCurDieTime;
	bool         bAI;

	// 캐릭터 치장 - 성장 - 장비 - 메달
	int         dwCharIndex;
	int         iClassLevel;
	CHARACTER   kCharInfo;
	ITEM_DATA   kEquipItem[MAX_CHAR_DBITEM_SLOT];
	IntVec      vEquipMedal;
	//IntOfTwoVec vEquipCustomMedal;  // index, code
	IntOfStatVec vEquipCustomMedalData;  // index, code
	BYTE        kCharGrowth[MAX_CHAR_GROWTH];
	BYTE        kItemGrowth[MAX_ITEM_GROWTH];

	int        kCustomStat[CustomMedal::MEDAL_STAT];
	

	// 진열 캐릭터
	bool        bDisplayCharacter;
	DWORD       dwDisplayEtcMotion;

	HeadquartersCharRecord& operator = ( const HeadquartersCharRecord &rhs )
	{
		// 일반 데이터
		dwCode		= rhs.dwCode;
		szName		= rhs.szName;
		fStartXPos	= rhs.fStartXPos;
		fStartZPos	= rhs.fStartZPos;
		dwStartTime = rhs.dwStartTime;
		eState		= rhs.eState;
		szSyncUser	= rhs.szSyncUser;
		eTeam		= rhs.eTeam;
		dwCurDieTime= rhs.dwCurDieTime;
		bAI         = rhs.bAI;

		// 캐릭터 치장 - 성장 - 장비 - 메달
		dwCharIndex = rhs.dwCharIndex;
		iClassLevel = rhs.iClassLevel;
		kCharInfo   = rhs.kCharInfo;
	
		int i = 0;
		for(i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
			kEquipItem[i] = rhs.kEquipItem[i];
		
		vEquipMedal.clear();
		for(i = 0;i < (int)rhs.vEquipMedal.size();i++)
			vEquipMedal.push_back( rhs.vEquipMedal[i] );
		
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			kCharGrowth[i] = rhs.kCharGrowth[i];

		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			kItemGrowth[i] = rhs.kItemGrowth[i];

		vEquipCustomMedalData.clear();
		for(i = 0;i < (int)rhs.vEquipCustomMedalData.size();i++)
			vEquipCustomMedalData.push_back( rhs.vEquipCustomMedalData[i] );

		for(i = 0;i < CustomMedal::MEDAL_STAT; i++)
			kCustomStat[i] = rhs.kCustomStat[i];

		bDisplayCharacter = rhs.bDisplayCharacter;
		dwDisplayEtcMotion= rhs.dwDisplayEtcMotion;

		return *this;
	}

	HeadquartersCharRecord()
	{
		// 일반 데이터
		dwCode		= 0;
		fStartXPos  = 0.0f;
		fStartZPos  = 0.0f;
		dwStartTime = 0;
		eState      = RS_LOADING;
		eTeam       = TEAM_RED;
		dwCurDieTime= 0;
		bAI         = false;

		// 캐릭터 치장 - 성장 - 장비 - 메달
		dwCharIndex = 0;
		iClassLevel = 0;
		
		int i = 0;
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			kCharGrowth[i] = 0;

		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			kItemGrowth[i] = 0;

		for(i = 0;i < CustomMedal::MEDAL_STAT; i++)
			kCustomStat[i] = 0;

		bDisplayCharacter = false;
		dwDisplayEtcMotion= 0;
	}	
};
typedef std::vector< HeadquartersCharRecord > HeadquartersCharRecordList;

#endif