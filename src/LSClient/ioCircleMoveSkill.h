#pragma once


#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum AreaWeaponState
	{
		AS_DELAY,
		AS_MOVE,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_UPDATE,
		SSS_DUMMY,
	};

	enum GaugeUseType
	{
		GUT_ON_KEY_INPUT,
		GUT_ON_SKILL_USE,
		GUT_BOTH,
	};

	enum CreateWeaponType
	{
		CWT_NONE,
		CWT_AFTER,
	};

protected:
	CEncrypt<int> m_State;
	AreaWeaponState m_AreaWeaponState;

	ioHashString m_AreaWeaponName;

	DWORD m_dwAreaWeaponIndex;
	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

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

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwLoopProtectDuration;
	DWORD           m_dwProtectTime;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	bool m_bEnableControl;
	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeed;
	float m_fMoveRange;
	
	CreateWeaponType	m_createWeaponTpye;
	bool				m_bCreatedWeapon;

// Struct
protected:
	bool m_bCreateStruct;
	CEncrypt<int> m_iStructNum;
	CEncrypt<float> m_fStructOffset;
	CEncrypt<float> m_fStructHeightOffset;

// Attribute
	bool m_bUseAttack;

	CEncrypt<float> m_fAttackOffset;
	CEncrypt<float> m_fAttackHeightOffset;

	D3DXVECTOR3 m_vAttackDir;
	DWORD m_dwAttackCheckTime;
	CallAttributeList m_CallAttributeList;

// dummychar
protected:
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

protected:
	CEncrypt<DWORD> m_dwCircleCheckDuration;
	DWORD m_dwCircleCheckTime;

	GaugeUseType m_GaugeUseType;

	CEncrypt<bool> m_bHandMeshOnDelayAni;

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
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CheckCallingAttribute( ioBaseChar *pChar );

	void CheckCreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAreaWeapon();

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void CheckControl( ioBaseChar *pChar );
	void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	void CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool CheckAreaWeapon();
	const D3DXVECTOR3& GetAreaWeaponPos();
	const D3DXVECTOR3& GetAreaWeaponPrePos();

public:
	ioCircleMoveSkill();
	ioCircleMoveSkill( const ioCircleMoveSkill &rhs );
	virtual ~ioCircleMoveSkill();
};

inline ioCircleMoveSkill* ToCircleMoveSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE )
		return NULL;

	return dynamic_cast< ioCircleMoveSkill* >( pSkill );
}

