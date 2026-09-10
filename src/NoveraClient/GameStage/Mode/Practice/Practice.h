#pragma once

#include "IoPracticeManager.h"

class ioPracticeMode;
class ioBaseChar;
class ioWeapon;

class Practice
{
protected:
	struct CheckPoint
	{
		int				m_iOrder;
		D3DXVECTOR3		m_vCheckPointPos;
		ioHashString	m_szEffect;
		DWORD			m_dwEffectID;
		float			m_fRange;

		bool			m_bCheck;

		CheckPoint()
		{
			Init();
		}

		void Init()
		{
			m_iOrder = 0;
			m_vCheckPointPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_szEffect.Clear();
			m_dwEffectID = 0;
			m_fRange = 0.f;

			m_bCheck = false;
		}
	};

	typedef std::vector< CheckPoint > CheckPointList;
	CheckPointList m_CheckPointList;

	typedef std::vector< ioMapEffect* > EffectList;
	EffectList m_vEffectList;

public:
	struct PracticeNPC
	{
		//P
		DWORD			m_dwNPCCode;
		DWORD			m_dwMonsterID;
		TeamType		m_eTeamType;
		ioHashString	m_szName;
		float			m_StartXPos;
		float			m_StartZPos;
		bool			m_bPrisoner;

		//A
		bool			m_bCheck;

		PracticeNPC()
		{
			Init();
		}

		void Init()
		{
			m_dwNPCCode = 0;
			m_dwMonsterID = 0;
			m_eTeamType = TEAM_NONE;
			m_szName.Clear();
			m_StartXPos = 0.f;
			m_StartZPos = 0.f;
			m_bPrisoner = false;

			m_bCheck	= false;
		}
	};

	typedef std::vector< PracticeNPC > PracticeNPCList;
	PracticeNPCList m_PracticeNPCList;

protected:
	ioPracticeMode *m_pPracticeMode;
	float m_fExtraRecoveryGaugeRate;
	std::vector<int> AvailableAttackList;

	DWORD m_dwGradeA;
	DWORD m_dwGradeB;
	DWORD m_dwGradeC;

public:
	void Init( ioPracticeMode* pPracticeMode );
	void LoadPracticeINI( ioINILoader &rkLoader );

protected:
	virtual void LoadCheckPoint( ioINILoader &rkLoader );
	virtual void LoadPracticeNPC( ioINILoader &rkLoader );
	virtual void LoadPracticeKit( ioINILoader &rkLoader );

public:
	virtual void UpdateMiniMapSpotPoint();
	
	void ProcessCheckPointCollision();
	float GetExtraRecoveryGaugeRate(){ return m_fExtraRecoveryGaugeRate; }

public:
	virtual int GetPracticeType(){ return ioPracticeManager::E_PRACTICE_NONE; }
	
	virtual bool ApplyWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon );
	virtual void CheckWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage );
	virtual void CheckWeaponDefense( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage );

	virtual void EscapePrisonerChar( ioBaseChar *pChar );

public:
	virtual bool ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade ) = 0; 
	virtual void PracticeProgress( int &iProgress, int &iMaxProgress ) = 0; 

public:
	Practice();
	virtual ~Practice();
};