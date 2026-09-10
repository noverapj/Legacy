#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;
class ioWeapon;

class ioMultiLightAttack : public ioAttackSkill
{
private:

	enum SkillState
	{
		SS_PRE,
		SS_ACTION,
		SS_LOOP,
		SS_WEAPON_PULL,
		SS_TELEPORT,
		SS_END,
	};
	
	enum SkillSync
	{
		SSC_PULL,
		SSC_TELEPORT,
	};

	SkillState m_State;
	BuffInfoList m_OwnerBuffList;

	CEncrypt<int> m_iMaxWeaponWay;
	CEncrypt<int> m_iCurWeaponWay;

	typedef std::vector< float > FireAngleList;
	FireAngleList m_FireAngleList;

	ioHashString m_SkillLoopMotion;
	CEncrypt<float> m_fSkillLoopMotionRate;
	CEncrypt<DWORD> m_dwLoopDuration;

	//skill end auto target
	CEncrypt<bool> m_bEndUseAutoTargetValue;
	CEncrypt<bool> m_bEndAutoTargetDownState;
	CEncrypt<float> m_fEndAutoTargetMinAngle;
	CEncrypt<float> m_fEndAutoTargetMaxAngle;
	CEncrypt<float> m_fEndAutoTargetRange;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_Circle;	
	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;		

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	// 스킬 시작
	CEncrypt<DWORD> m_dwTeleportEndTime;
	CEncrypt<DWORD> m_dwCheckPullStateMaxTime;
	DWORD m_dwCurCheckPullStateStartTime;

	DWORD m_dwCurTeleportEndTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	typedef std::vector<DWORD> ActiveWeaponList;
	ActiveWeaponList m_vActiveWeapon;

	ioHashString m_SkillMapEffect;
	UniqueObjID m_dwSkillMapEffect;

	DWORD m_dwPullTime;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void ClearData();
	void LoadOwnerBuffList( ioINILoader &rkLoader );

	
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();
	virtual bool IsNoDropState() const;
	
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	void RemoveWeapon( ioBaseChar* pChar );

	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetPullState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend );
	void SetTeleport( ioBaseChar *pChar, bool bSend );

	bool CheckPullState( ioBaseChar* pChar );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	
public:
	ioMultiLightAttack();
	ioMultiLightAttack( const ioMultiLightAttack &rhs );
	virtual ~ioMultiLightAttack();
};

inline ioMultiLightAttack* ToMultiLightAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MULTI_LIGHT )
		return NULL;

	return dynamic_cast< ioMultiLightAttack* >(pAttack);
}

