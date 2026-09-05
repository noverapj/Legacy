#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioAngleFire3Skill : public ioAttackSkill
{
protected:

	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_ACTION,	
		SS_END,
	};

	enum SkillSync
	{	
		SSC_LOOP,
		SSC_ACTION,		
	};

	SkillState m_State;

	//Loop
	ioHashString m_SkillLoopMotion;
	CEncrypt<float> m_fSkillLoopMotionRate;

	CEncrypt<DWORD> m_dwSkillLoopDuration;
	CEncrypt<DWORD> m_dwSkillLoopProtectTime;
	CEncrypt<DWORD> m_dwSkillLoopProtectEndTime;

	CEncrypt<DWORD> m_dwLoopStartTime;
	CEncrypt<DWORD> m_dwLoopEndTime;

	//Angle
	CEncrypt<DWORD> m_dwAngleTime;

	CEncrypt<float> m_fNormalMaxAngle;
	CEncrypt<float> m_fNormalMinAngle;

	CEncrypt<float> m_fJumpMaxAngle;
	CEncrypt<float> m_fJumpMinAngle;

	CEncrypt<float> m_fJumpHeight;

	CEncrypt<float> m_fCurAngle;
	CEncrypt<float> m_fCurMaxAngle;
	CEncrypt<float> m_fCurMinAngle;

	ioHashString m_AngleGuidEffect;
	UniqueObjID m_dwAngleGuidEffect;

	D3DXVECTOR3 m_vAttackDir;

	//Key
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	CEncrypt<DWORD> m_dwSkillLoopRotateSpeed;

	DWORD m_dwInvisibleCloakID;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;

	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	void ClearData();

	void SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend = true );
	bool CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar* pChar, ioPlayStage* pStage );

	void CheckAngle( ioBaseChar* pChar, float fMinAngle, float fMaxAngle );
	void CheckDir( ioBaseChar *pChar );
	void UpdateAngleGuidEffect( ioBaseChar *pChar, ioPlayStage *pStage );

	void ProcessRotate( ioBaseChar *pChar, float fRotateSpeed );
	void CheckKeyInput( ioBaseChar *pChar );

public:
	ioAngleFire3Skill();
	ioAngleFire3Skill( const ioAngleFire3Skill &rhs );
	virtual ~ioAngleFire3Skill();
};

inline ioAngleFire3Skill* ToAngleFire3Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_ANGLE_FIRE3 )
		return NULL;

	return dynamic_cast< ioAngleFire3Skill* >(pAttack);
}

