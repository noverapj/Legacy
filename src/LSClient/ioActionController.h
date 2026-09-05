#pragma once


class ioActionController
{
protected:
	struct ActionSkillData
	{
		DWORD m_dwRandSeed;
		ioHashString m_szSkillName;	

		ActionSkillData()
		{
			m_dwRandSeed = 0;
		}
	};
	typedef std::vector< ActionSkillData > ActionSkillDataVec;

	struct ActionData
	{
		WORD	m_wDefenseKey;
		WORD	m_wJumpKey;
		WORD	m_wAttackKey;
		DWORD   m_dwDelayTime;
		ioHashString m_szChatMent;
		ActionData()
		{
			m_wDefenseKey = m_wJumpKey = m_wAttackKey = 0;
			m_dwDelayTime = 0;
		}
	};
	typedef std::vector< ActionData > vActionData;
	struct ActionList
	{
		vActionData		m_Action;
		float			m_fActionRangeMin;
		float			m_fActionRangeMax;
		float			m_fLimitHPRateMin;
		float			m_fLimitHPRateMax;
		float			m_fTargetAngle;
		float			m_fTargetDirection;
		DWORDVec		m_vMyState;
		DWORDVec		m_vTargetState;
		ioHashStringVec m_vMyBuff;
		ioHashStringVec m_vTargetBuff;
		DWORD			m_dwActiveRand;
		DWORD           m_dwActiveDelayCheckTime;
		DWORD			m_dwActiveDelayTime;
		DWORD           m_dwActiveDelayTimeRand;
		DWORD			m_dwCurActiveTimer;
		ActionSkillDataVec	m_RandomSkllList;
		DWORD               m_RandomSkilSeed;
		ioHashString		m_szCurrentSkill;
		int                 m_iActiveCount;

		ActionList()
		{
			m_iActiveCount = 0;
			m_fActionRangeMin = m_fActionRangeMax = m_fLimitHPRateMin = m_fLimitHPRateMax = m_fTargetDirection = 0.0f;
			m_dwActiveRand = m_dwActiveDelayCheckTime = m_dwActiveDelayTime = m_dwActiveDelayTimeRand = m_dwCurActiveTimer = m_RandomSkilSeed = 0;		
			m_fTargetAngle =FLOAT90;
		}

		void CreateActiveCheckTime()
		{
			m_dwCurActiveTimer = FRAMEGETTIME();
			IORandom	ioRand;
			ioRand.Randomize();
			m_dwActiveDelayCheckTime = m_dwActiveDelayTime + ( ioRand.Random(max( 1, m_dwActiveDelayTimeRand )) );
		}
	};
	typedef std::map< DWORD, ActionList > ActionMap;
	ActionMap  m_ActionMap;

protected:
	ActionList m_CurAction;
	DWORD      m_dwCurrentTime;

protected:
	ioActionController::ActionList &FindActionData( DWORD dwID );
	void AttachActionData( ActionList &rkList, ioNpcChar *pOwner, ioBaseChar *pTarget );
	bool CheckState( DWORDVec &rkState, DWORD dwState );
	bool CheckBuff( ioHashStringVec &rkBuffList, ioBaseChar *pChar );

public:
	void LoadController( ioINILoader &rkLoader );
	void ClearCurrentData();

public:
	bool Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fDistSq, float fTargetAngle );
	
public:
	bool OnAttackAction( ioNpcChar *pOwner, ioBaseChar *pTarget );

public:
	bool IsAction();

public:
	ioActionController();
	virtual ~ioActionController();
};
