#pragma once
//////////////////////////////////////////////////////////////////////////
// 목적지 도달
#include "Practice.h"

class Practice_1 : public Practice
{
public:
	virtual int GetPracticeType(){ return ioPracticeManager::E_PRACTICE_1; }
	virtual bool ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade );
	virtual void PracticeProgress( int &iProgress, int &iMaxProgress ){}

public:
	Practice_1();
	virtual ~Practice_1();
};