#pragma once

#include "ioNormalSkill.h"


class ioWeaponControlSkill3 : public ioNormalSkill
{
public:
	enum ControlState
	{
		RS_NONE,
		RS_ACTION,
		RS_LOOP,
		RS_END,
		RS_END_ATTACK,
	};

	enum SyncState
	{
		SS_CREATE_AREA,
		SS_END,
		SS_NONE_TARGET,
		SS_FIND_TARGET,
	};

protected:
	ControlState m_ControlState;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	AttackAttribute m_AttackAttribute;

	D3DXVECTOR3 m_vCreatePos;
	DWORD m_dwFireStartTime;

	// Animation
	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniEndTime;

	// HandMesh
	ioHashString m_StartHandMesh;
	ioHashString m_LoopHandMesh;
	ioHashString m_EndHandMesh;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaWeaponIndex;

	WeaponInfoList m_NoneTargetAttackWeaponInfoList;
	float m_fNoneTargetCreateWeaponHeightOffSet;

	ioHashString m_stCheckBuffName;

	bool m_bAttackActive;

	bool m_bNotUseAttackKeyInput;
	AttackAttribute m_EndAttackAttribute;

	bool m_bConveyerDisable;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

	virtual bool UseActiveCount( ioBaseChar *pOwner );

	virtual int GetMaxActiveCount( ioBaseChar *pOwner );
	virtual int GetCurActiveCount( ioBaseChar *pOwner );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar, bool bSendNet = false );

	bool CheckControlWeapon( ioBaseChar *pChar );

	void CreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAreaWeapon();

	void NoneTargetAttack( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos );
	void BuffTargetAttack( ioBaseChar *pChar, ioPlayStage *pStage );
	bool CheckBuff( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool IsWeaponControl( ioBaseChar *pChar );
	const D3DXVECTOR3& GetWeaponPos( ioBaseChar *pChar );

public:
	ioWeaponControlSkill3();
	ioWeaponControlSkill3( const ioWeaponControlSkill3 &rhs );
	virtual ~ioWeaponControlSkill3();
};

inline ioWeaponControlSkill3* ToWeaponControlSkill3( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_WEAPON_CONTROL3 )
		return NULL;

	return dynamic_cast< ioWeaponControlSkill3* >( pSkill );
}

