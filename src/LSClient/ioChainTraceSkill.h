#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioChainTraceSkill : public ioAttackSkill
{
public:

	enum SkillState
	{
		SS_NONE,
		SS_PREDELAY,
		SS_START,
		SS_ACTION_HITED,
		SS_WAIT_EXTRA_TIME,
		SS_EXTRA_ACTION,
		SS_WAIT_END_ADD_ACTION,
		SS_AUTOFIRE_SYNC,
		SS_END,
	};

	enum SkillSync
	{
		SSC_TARGET,
	};


protected:
	SkillState m_state;

	ioHashString	m_Target;
	ioHashStringVec	m_vTargetList;

	DWORD			m_dwFireStartTime;
	DWORD			m_dwFireEndTime;
	DWORD			m_dwFirstHitTime;
	DWORD			m_dwHitTime;
	DWORD			m_dwDelayHitTime;
	DWORD			m_dwMaxWaitTime;
	DWORD			m_dwNextAutoFireTime;
	DWORD			m_dwAddAttackFireTime;

	D3DXVECTOR3		m_vPrevHitPosition;

	ioHashString m_CurTargetName;

	float			m_fPrevHitHeight;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetRange2;
	CEncrypt<float> m_fTargetMinRange;
	CEncrypt<float> m_fTargetAngle;

	CEncrypt<int>	m_nMaxHitCnt;
	int				m_nHitCnt;

	ioHashString    m_stHitLoopAni;
	ioHashString	m_stAddAttackAni;
	float			m_fAddAttackAniRate;
	DWORD			m_dwAddAttackAniPreDelay;
	int				m_nAddAttackWeapon;

	DWORD			m_dwProtectTime;
	DWORD			m_dwProtectEndTime;

	std::vector< DWORD > m_vWeaponIndexList;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	ioHashString   m_stOwnerWeaponTargetName;

protected:
	TargetWoundType m_TargetWoundType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	ioUserKeyInput::DirKeyInput GetCurKey();

public:
	void SetTarget( const ioHashString &szTargetName, ioBaseChar* pOwner, ioWeapon *pWeapon );

protected:
	//virtual void AttackSkillFire( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void FindFirstTarget( ioBaseChar *pOwner );
	bool FindAutoTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	void SetStartState( ioBaseChar *pChar );
	void SetHitState( ioBaseChar *pChar );
	void SetWaitExtraActionState( ioBaseChar *pChar );
	void SetExtraAcitonState( ioBaseChar *pChar );
	void SetWaitAddAttackEndState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void AutoFire( ioBaseChar *pChar );

	void RemoveWeapon( ioBaseChar *pChar );

	void ProcessWeaponLine( ioBaseChar *pChar );

	//////////////////////////////////////////////////////////////////////////
	/*void ChangeTrackingUD_Fire( ioBaseChar *pOwner );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void AimedFireToUp( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );*/

public:
	ioChainTraceSkill();
	ioChainTraceSkill( const ioChainTraceSkill &rhs );
	virtual ~ioChainTraceSkill();
};

inline ioChainTraceSkill* ToChainTrace( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CHAIN_TRACE )
		return NULL;

	return dynamic_cast< ioChainTraceSkill* >(pAttack);
}

