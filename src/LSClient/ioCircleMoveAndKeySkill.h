#pragma once


#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAndKeySkill : public ioNormalSkill
{
public:
	enum PreDelayType
	{
		PDT_WAIT,
		PDT_NORMAL,
	};

	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_ACTION2,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_ACTION2,
		SSS_UPDATE,
	};

protected:
	CEncrypt<int> m_State;
	PreDelayType m_PreDelayType;

	ioHashString m_AreaWeaponName;

	DWORD m_dwAreaWeaponIndex;
	DWORD m_dwFireStartTime;

	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	ioHashString m_SkillMotion2;
	float m_fSkillMotion2Rate;

	CEncrypt<DWORD> m_dwCheckDeadTime;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

// Struct
protected:
	bool m_bCreateStruct;
	CEncrypt<int> m_iStructNum;
	CEncrypt<float> m_fStructOffset;
	CEncrypt<float> m_fStructHeightOffset;

// Attribute
	WeaponInfo m_WeaponInfo;
	AttackAttribute m_AttackAttribute;

// key
	bool m_bAddKey;
	DWORD m_dwAddDuration;
	DWORD m_dwEnableKeyTime;
	DWORD m_dwCheckStartTime;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );

	int GetSkillState();

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetCreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAreaWeapon();

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetActionState2( ioBaseChar *pChar, ioPlayStage *pStage );

	void CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioCircleMoveAndKeySkill();
	ioCircleMoveAndKeySkill( const ioCircleMoveAndKeySkill &rhs );
	virtual ~ioCircleMoveAndKeySkill();
};

inline ioCircleMoveAndKeySkill* ToCircleMoveAndKeySkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_KEY )
		return NULL;

	return dynamic_cast< ioCircleMoveAndKeySkill* >( pSkill );
}

