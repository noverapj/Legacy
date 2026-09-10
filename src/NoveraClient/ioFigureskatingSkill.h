#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioFigureskatingSkill : public ioAttackSkill
{
public:
	enum RunningState
	{
		RST_NONE,
		RST_READY,
		RST_RUN,
		RST_RUN_END,
		RST_LOOP,
		RST_ATTACK,
		RST_END,
	};

	enum
	{
		SST_ROTATE,
		SST_LOOP,
		SST_ATTACK,
	};

protected:
	// 스킬 게이지 감소
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	bool m_bUseFireEnd;
	bool m_bDirection;

	RunningState m_RunningState;

	ioHashString m_RunningReadyAni;
	float m_fRunningReadyAniRate;
	DWORD m_dwRunningReadyEndTime;

	DWORD m_dwLoopTime;
	DWORD m_dwRunningEndTime;

	DWORD m_dwRunningStartTime;

	ioHashString m_RunningEndAni;
	float m_fRunningEndAniRate;
	DWORD m_dwRunningEndEndTime;

	CEncrypt<float> m_fRunSpeed;
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;

	DWORD m_dwFireDuration;

	bool m_bDisableRunColSkip;
protected:
	CEncrypt<int> m_iMaxWeaponWay;
	int m_iWeaponWay;
	BulletInfoList m_BulletInfoList;

protected:
	//Loop Animation
	ioHashString	m_szLoopAnimation;
	CEncrypt<float>	m_fLoopAniRate;
	CEncrypt<DWORD>	m_dwLoopDuration;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;
	ioHashString	m_szLoopEffect;

	WeaponInfoList	m_LoopWeaponInfoList;

	//Second Attack
	AttackAttribute	m_SecondAttackAttribute;

	typedef std::vector<DWORD> ActiveWeaponList;
	ActiveWeaponList m_vActiveWeapon;
	
	BuffInfoList m_OwnerBuffList;
public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	void LoadLoopWeaponList( ioINILoader &rkLoader );
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	void SetReadyState( ioBaseChar *pChar );
	void SetRunState( ioBaseChar *pChar );
	void SetRunEndState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );
	void SetAttackState( ioBaseChar *pChar );
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearData();

	void SetOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
public:
	ioFigureskatingSkill();
	ioFigureskatingSkill( const ioFigureskatingSkill &rhs );
	virtual ~ioFigureskatingSkill();
};

inline ioFigureskatingSkill* ToFigureskatingSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIGURESKATING )
		return NULL;

	return dynamic_cast< ioFigureskatingSkill* >( pAttack );
}

