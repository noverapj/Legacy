#pragma once

#include "ioSkill.h"
#include "WeaponAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class ioWeapon;

class ioPassiveSkill :public ioSkill
{
public:
	enum PassiveSync
	{
		PS_ZERO_GAUGE,
		PS_ADD_BUFF,
	};

	struct GaugeBuffInfo
	{
		float			m_fMaxPassiveGauge;
		float			m_fMinPassiveGauge;
		float			m_fGradeEndReduce;
		DWORD			m_dwBuffKeepTime;   //특정조건에서 버프를 유지할 시간

		bool			m_bBuffEndGuageClear;		
		bool			m_bCheckBuffActivated;

		BuffInfoList	m_Buff;

		bool			m_bLoadedBuff;
		
		GaugeBuffInfo()
		{
			m_fMaxPassiveGauge = 0.0f;
			m_fMinPassiveGauge = 0.0f;
			m_fGradeEndReduce  = 0.0f;

			m_dwBuffKeepTime = 0;
			m_bBuffEndGuageClear = false;

			m_bLoadedBuff = false;

			m_Buff.clear();
		}
	};
	typedef std::vector<GaugeBuffInfo>	GaugeBuffInfoList;

	struct KeepTimeBuffInfo
	{
		ioHashString m_szBuffName;
		DWORD m_dwBuffEndTime;
		DWORD m_GaugeIdx;
		DWORD m_BuffIdx;
	};
	typedef std::vector<KeepTimeBuffInfo>	KeepTimeBuffInfoList;

	struct GaugeWoundBuffInfo
	{
		float			m_fMaxGauge;
		float			m_fMinGauge;
		BuffInfoList	m_BuffList;

		DWORD			m_dwDuration;
		DWORD			m_dwAirDuration;
		DWORD			m_dwDownDuration;

		GaugeWoundBuffInfo( float fMaxGauge, float fMinGauge, BuffInfoList BuffList )
		{
			m_fMaxGauge = fMaxGauge;
			m_fMinGauge = fMinGauge;
			m_BuffList  = BuffList;
		}

		void SetBuffOnWoundDuration( DWORD dwDuration, DWORD dwAirDuration, DWORD dwDownDuration )
		{
			m_dwDuration     = dwDuration;
			m_dwAirDuration  = dwAirDuration;
			m_dwDownDuration = dwDownDuration;
		}
	};
	typedef std::vector<GaugeWoundBuffInfo>	GaugeWoundBuffInfoList;

protected:
	BuffInfoList m_OwnerBuffList;
	BuffInfoList m_TargetBuffList;
	GaugeBuffInfoList      m_OwnerGaugeBuffInfoList;
	GaugeWoundBuffInfoList m_OwnerGaugeWoundBuffInfoList;
	KeepTimeBuffInfoList   m_vKeepTimeBuff;

	CEncrypt<bool>	m_bUsePassiveGauge;
	CEncrypt<bool>	m_bStopPassiveGaugeIOnSkipBuff;

	CEncrypt<float>	m_fMaxPassiveGauge;
	CEncrypt<float>	m_fDefaultPassiveGauge;
	CEncrypt<float>	m_fCurPassiveGauge;
	CEncrypt<bool>	m_bMaintainPassiveGauge;
	CEncrypt<float>	m_fRecoveryValue;
	CEncrypt<DWORD>	m_dwRecoveryTick;
	CEncrypt<DWORD>	m_dwFCModeRecoveryTick;
	CEncrypt<DWORD>	m_dwRecoveryDelay;
	DWORD			m_dwCheckZeroGaugeTime;
	CEncrypt<bool>	m_bReduceNeedGauge;

	CEncrypt<float>	m_fReducePassiveGaugeOnAttack;
	CEncrypt<float>	m_fReducePassiveGaugeOnWound;
	CEncrypt<float> m_fReducePassiveGaugeOnUse;
	CEncrypt<float> m_fReducePassiveGaugeOnJump;
	CEncrypt<bool>  m_bReduceIgnoreOnDropZone;
	CEncrypt<bool>  m_bReduceIgnoreOnJumpState;
	
	ioHashStringVec m_ReduceIgnoreBuffList;
	ioHashStringVec m_RecoveryIgnoreBuffList;

	CEncrypt<float> m_fRecoveryPassiveGaugeRateOnDelayRun;
	CEncrypt<float> m_fRecoveryPassiveGaugeRateOnDash;
	CEncrypt<float> m_fRecoveryPassiveGaugeRateOnJump;
	CEncrypt<float> m_fRecoveryPassiveGaugeRateOnEtc;
	CEncrypt<float> m_fRecoveryPassiveGaugeRateOnJumpAttack;
	CEncrypt<float> m_fRecoveryPassiveGaugeRateOnAttack;

	bool m_bSetUseActiveCnt;
	
	DWORD m_dwCurStopPassiveSkillID;
	DWORDVec m_vStopPassiveSkillID;

	bool m_bSetKeepGaugeBuff;

	bool m_bClearWoundKeepTimeList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	//virtual bool IsCanUseSkillWithCrown( ioBaseChar *pChar );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	
	virtual bool HasBuff( int iBuffType ) const;
	virtual bool HasBuff( const ioHashString& szBuffName ) const;

	virtual void CheckAttackProcess( ioBaseChar *pOwner, ioBaseChar *pAttacker );
	virtual void CheckWoundProcess( ioBaseChar *pOwner, ioBaseChar *pAttacker );	
	virtual void CheckUseProcess( ioBaseChar *pOwner );
	virtual void CheckJumpProcess( ioBaseChar *pOwner );

protected:
	void LoadBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );
	void AddTargetBuff( ioBaseChar *pChar );

	void CheckKeepTimeAllBuff( ioBaseChar *pChar );
	bool CheckKeepTimeBuff( ioBaseChar *pChar, ioHashString szBuffName, DWORD dwGaugeIdx, DWORD dwBuffIdx );

public:
	void CheckReducePassiveSkillGrade( ioBaseChar* pChar, ioBaseChar* pAttacker );

	bool CheckReducePassiveSkillGaugeBuff( ioBaseChar* pChar, int iBuffType );
	bool CheckReducePassiveSkillGaugeBuff( ioBaseChar* pChar, int iBuffType, ioHashString& szBuffName );

	void RemoveSkillBuff( ioBaseChar *pChar );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	DWORD StopPassiveSkill( ioBaseChar *pChar );
	void RestorePassiveSkill( ioBaseChar *pChar, const DWORD &dwID );
	void CheckPassiveSkillBuff( ioBaseChar *pChar );

public:
	virtual void UpdatePassiveGauge( ioBaseChar *pChar );
	virtual float ReduceCurPassiveGauge( float fUseGauge );
	void SetCurPassiveGauge( float fValue );
	float GetMaxPassiveGauge() { return m_fMaxPassiveGauge; }
	float GetCurPassiveGauge() { return m_fCurPassiveGauge; }
	bool GetMaintainPassiveGauge() { return m_bMaintainPassiveGauge; }

	virtual bool UsePassiveGauge() const;
	virtual bool IsBuffEndGaugeClear( const ioHashString& rkName ) const;

	const ioHashStringVec GetOwnerGaugeBuffName( ioBaseChar *pOwner ) const;

protected:
	void UpdatePassiveGaugeBuff( ioBaseChar *pChar );
	void UpdatePassiveGaugeBuff2( ioBaseChar *pChar );
	virtual void RemoveAllPassiveGaugeBuff( ioBaseChar *pChar );
	virtual void UpdateRecoveryPassiveGauge( ioBaseChar *pChar );
	virtual void CheckOwnerGaugeWoundBuff( ioBaseChar *pOwner );

	virtual bool CheckUseActiveCount();

	//패시브 게이지 퍼센트 확인 jch
public:
	float GetCurPassiveGaugeRate();

public:
	ioPassiveSkill();
	ioPassiveSkill( const ioPassiveSkill &rhs );
	virtual ~ioPassiveSkill();
};

inline ioPassiveSkill* ToPassiveSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_PASSIVE )
		return NULL;
	
	return dynamic_cast< ioPassiveSkill* >( pSkill );
}


