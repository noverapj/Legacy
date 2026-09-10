#pragma once
//////////////////////////////////////////////////////////////////////////
// 스킬 사용
#include "Practice.h"

class Practice_3 : public Practice
{
public:
	virtual int GetPracticeType(){ return ioPracticeManager::E_PRACTICE_3; }
	virtual bool ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade );
	virtual void PracticeProgress( int &iProgress, int &iMaxProgress );

	virtual bool ApplyWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon );

public:
	Practice_3();
	virtual ~Practice_3();
};