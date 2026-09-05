#pragma once

#include "../DataHeaders/LSC_Practice.h"

class PracticeManager
{
public:
	enum PracticeType
	{
		E_PRACTICE_NONE = 0,
		E_PRACTICE_1 = 1, 		// 이동 & 점프
		E_PRACTICE_2 = 2, 		// 포로 구출
		E_PRACTICE_3 = 3, 		// 마법 사용
	};

	enum PracticeGrade
	{
		E_GRADE_NONE = 0,
		E_GRADE_C = 1,
		E_GRADE_B = 2,
		E_GRADE_A = 3,
	};

	

protected:
	LSC_Practice_Manager* m_pPracticeMgr;

public:
	void Init();
	int GetRegularSoldierCloak( int iIndex );
	int GetRegularSoldierHelmet( int iIndex );
	int GetRegularSoldierArmor( int iIndex );
	int GetRegularSoldierWeapon( int iIndex );

	int GetRegularSoldierClass( int iIndex );
	BYTE GetRegularSoldierGender( int iIndex );
	int GetRegularSoldierUnderwear( int iIndex );
	int GetRegularSoldierHair( int iIndex );
	int GetRegularSoldierHairColor( int iIndex );
	int GetRegularSoldierFace( int iIndex );
	int GetRegularSoldierSkinColor( int iIndex );
	int GetPracticeMap( int iIndex );
	int GetPracticeFreeAdmission( int iIndex );
	DWORD GetLimitTime( int iIndex );

	DWORD GetPracticeGradeA( int iIndex );
	DWORD GetPracticeGradeB( int iIndex );
	DWORD GetPracticeGradeC( int iIndex );
	DWORD GetPracticeRewardA( int iIndex );
	DWORD GetPracticeRewardB( int iIndex );
	DWORD GetPracticeRewardC( int iIndex );

	PracticeType GetPracticeType( int iIndex );

	LSC_Practice* GetLSCPractice( int iIndex );

	void GetPracticeReward( int iIndex, int iGrade, DWORDVec& rvGrade, DWORDVec& rvReward );

public:
	static PracticeManager& GetInstance();
	static void ReleaseInstance();

public:
	PracticeManager();
	virtual ~PracticeManager();
};

#define g_PracticeMgr PracticeManager::GetInstance()

