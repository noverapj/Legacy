#pragma once
//////////////////////////////////////////////////////////////////////////
// 포로 구출
#include "Practice.h"

class Practice_2 : public Practice
{
public:
	virtual int GetPracticeType(){ return ioPracticeManager::E_PRACTICE_2; }
	virtual bool ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade );
	virtual void PracticeProgress( int &iProgress, int &iMaxProgress );
	virtual void EscapePrisonerChar( ioBaseChar *pChar );

public:
	Practice_2();
	virtual ~Practice_2();
};