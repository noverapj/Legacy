#pragma once
//////////////////////////////////////////////////////////////////////////
// Áö·Ú Á¦°Å
#include "Practice.h"

class Practice_4 : public Practice
{
public:
	virtual int GetPracticeType(){ return ioPracticeManager::E_PRACTICE_4; }
	virtual bool ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade );
	virtual void PracticeProgress( int &iProgress, int &iMaxProgress );

	virtual void CheckWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage );
	virtual void CheckWeaponDefense( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage );

	virtual void LoadPracticeKit( ioINILoader &rkLoader );
	virtual void UpdateMiniMapSpotPoint();

protected:
	DWORD m_dwCount;

public:
	Practice_4();
	virtual ~Practice_4();
};