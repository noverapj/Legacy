#pragma once
#include "ioAttackSkill.h"

class ioSpaceMasterAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_ATTACK,
		SS_LOOP,
		SS_TARGET_THROW,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_ATTACK,
		SSS_LOOP,
		SSS_TARGET_THROW,
		SSS_REMOVE_WEAPON,
		SSS_END,
	};
	typedef std::vector<DWORD> ActiveWeaponList;

protected:
	SkillState			m_SkillState;
	SkillEndJumpType	m_SkillEndJumpType;

	DWORD				m_dwSkillProtectTime;
	DWORD				m_dwSkillJumpPower;

protected:
	float				m_fJumpAutoTargetRange;
	float				m_fJumpAutoTargetMinAngle;
	float				m_fJumpAutoTargetMaxAngle;
	
	D3DXVECTOR3			m_vAutoTargetDir;
	ioHashString		m_TargetName;

protected:
	AttackAttribute		m_SpaceAttack;
	DWORDVec			m_SpaceAttackList;

protected:
	ioHashString		m_LoopAnimation;
	float				m_fLoopAnimationRate;
		
	DWORD				m_dwLoopStartTime;
	DWORD				m_dwLoopDuration;
	DWORD				m_dwCharRotateTime;
	DWORD				m_dwCharAttachWeaponIdx;
	float				m_fCharAttachWeaponOffsetZ;
	float				m_fCharAttachWeaponOffsetY;
		
	DWORD				m_dwSelectWeaponIdx;
	DWORDVec			m_MissileWeaponList;

protected:
	int					m_nTargetThrowCount;
	int					m_nTargetThrowMaxCount;

protected:
	ioHashString		m_TargetCatchDelayAnimation;
	float				m_fTargetCatchDelayAnimationRate;
	float				m_fEndTargetThrowMoveRange;
	DWORD				m_dwTargetCatchDelayStartTime;
	DWORD				m_dwTargetCatchDelayStartDuration;

protected:
	ioHashString		m_TargetBuff;
	ioHashStringVec		m_TargetCharNameList;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEndByWounded( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	ioWeapon* ExtendAttackFire( ioBaseChar *pChar, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex );

protected:
	void CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckTargetThrowState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetSkillActionState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetSkillLoopState( ioBaseChar* pChar, ioPlayStage* pStage, bool bCallSpaceWeapon );
	void SetTargetThrowState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetEndState( ioBaseChar* pChar, ioPlayStage* pStage, bool bThrow, bool bSendNetWork );

	void SetWeaponDir( ioBaseChar* pChar, ioPlayStage* pStage, float fRange = 0.0f );

protected:
	void ProcessCharRotate( ioBaseChar *pChar, ioPlayStage *pStage );	
	void ProcessWeaponPos( ioBaseChar* pChar );

	void RemoveWeapon( ioBaseChar* pChar );
	bool CheckAliveWeapon( ioBaseChar* pChar );
	
protected:
	void AddTarget( ioBaseChar* pTarget );

public:
	virtual void GetAutoTargetValue( float &fRange, float &fMinAngle, float &fMaxAngle );
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );

public:
	ioSpaceMasterAttack();
	ioSpaceMasterAttack( const ioSpaceMasterAttack &rhs );
	virtual ~ioSpaceMasterAttack();
};

inline ioSpaceMasterAttack* ToSpaceMasterAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_SPACE_MASTER_ATTACK )
		return NULL;

	return dynamic_cast< ioSpaceMasterAttack* >(pAttack);
}

