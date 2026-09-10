#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

struct TimeLineWeapon
{
	WeaponInfo m_pWeapon;
	DWORD m_dwFireTime;
};

typedef std::vector<TimeLineWeapon> TimeLineWeaponList;

class ioFlashTransformationAttackSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SS_ROTATE,
		SS_ATTACK,
	};

protected:
	CEncrypt<int> m_State;

	TimeLineWeaponList m_pWeaponList;

	// Attack
	CEncrypt<DWORD> m_dwOrgLoopDuration;
	CEncrypt<DWORD> m_dwSkillProtectedTime;
	CEncrypt<DWORD> m_dwKeyReserveTime;
		
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;
	DWORD m_dwAttackKeyReserveTime;
	DWORD m_dwKeyInputGapTime;
	DWORD m_dwCurKeyInputGapTime;
	DWORD m_dwAttackKeyGapTime;
	DWORD m_dwFireIndex;

	DWORD m_dwCurWeaponCount;

	ioHashString m_szSkillLoopEffect;
	UniqueObjID m_dwSkillLoopEffect;

	float m_fSkillForceMove;
	float m_fSkillForceFriction;
	float m_fSkillForceTime;

	float m_fSkillForceCurTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );	
	virtual ioSkill* Clone();

	void InitSkillData();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;

protected:

	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );	
	void SetFrictionMove( ioBaseChar* pChar, float fExtraForceRate = FLOAT1 );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetFire( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckKeyInput( ioBaseChar* pChar, ioPlayStage *pStage );


protected:	

	void CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioFlashTransformationAttackSkill();
	ioFlashTransformationAttackSkill( const ioFlashTransformationAttackSkill &rhs );
	virtual ~ioFlashTransformationAttackSkill();
};

inline ioFlashTransformationAttackSkill* ToFlashTransformationAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_FLASH_TRANSFORMATION )
		return NULL;

	return dynamic_cast< ioFlashTransformationAttackSkill* >( pSkill );
}


