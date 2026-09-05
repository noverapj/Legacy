#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;
class ioEngineerGatlingDummy;
class ioWeapon;
class ioSimpleBoxWeapon;

class ioCircleMineSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};
	
	enum SkillSyncState
	{
		SSS_ATTACK_A,
		SSS_ATTACK_S,
		SSS_ATTACK_D,
	};

protected:
	SkillState				m_SkillState;
	DWORD					m_dwSkillProtectTime;

protected:
	int						m_nMaxMineCnt;
	int						m_nUseMineCnt;
	int						m_nCurrUseMineCnt;
	int						m_eMineType;

protected:
	ioHashString			m_szLoopAnimation;	
	float					m_fLoopAnimationRate;

	DWORD					m_dwLoopDuration;
	DWORD					m_dwLoopStartTime;

protected:	
	float				m_fCircleRange;
	float				m_fCircleMoveSpeed;
	float				m_fCircleHeightGap;
	float				m_fCircleOffSet;
	bool				m_bCircleOn;

	D3DXVECTOR3			m_vCircleStartPos;

	ioHashString		m_szOnCircle;
	ioHashString		m_szOffCircle;
	ioHashString		m_szCurCircle;

	ioHashString		m_szRedGuideCircle;
	ioHashString		m_szBlueGuideCircle;

protected:
	DWORD				m_dwBuryEndTime;
	D3DXVECTOR3			m_vMinePos;

	//////////////////////////////////////////////////////////////////////////
	bool				m_bUseFullAttack;
	AttackAttribute     m_A_Attiribute;
	AttackAttribute     m_S_Attiribute;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void CheckWeaponItemMesh( ioBaseChar* pChar );
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

protected:	
	int SetLoopAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:	
	void SetLoopState( ioBaseChar* pOwner, ioPlayStage* pStage, bool bFirst );

	void SetActionState_A( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetActionState_S( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetActionState_D( ioBaseChar* pOwner, ioPlayStage* pStage );

protected:	
	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckActionState( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	void CheckCircle( ioBaseChar *pOwner, ioPlayStage *pStage );
	const ioHashString& GetMagicGuide( ioBaseChar *pOwner );

public:
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	ioCircleMineSkill();
	ioCircleMineSkill( const ioCircleMineSkill &rhs );
	virtual ~ioCircleMineSkill();
};

inline ioCircleMineSkill* ToCircleMineSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill(	pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CIRCLE_MINE )
		return NULL;

	return dynamic_cast< ioCircleMineSkill* >( pSkill );
}

