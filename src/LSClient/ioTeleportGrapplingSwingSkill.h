#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioTeleportGrapplingSwingSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_TELEPORT,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_TELEPORT,
		SSC_ACTION,
		SSC_END,
		SSC_KEY,
	};


	enum TargetSearchState
	{
		TSS_COLLISION,
		TSS_RANGE,
	};

protected:
	SkillState					m_SkillState;
	TargetSearchState			m_TargetSearchState;
	ioHashString				m_GrapplingTargetName;

	CEncrypt<float>				m_fFindTargetRange;
	CEncrypt<float>				m_fFindTargetWidth;
	CEncrypt<float>				m_fFindTargetHeight;
	CEncrypt<float>				m_fFindTargetBackOffset;

	CEncrypt<float>				m_fBackRange;
	CEncrypt<float>				m_fHeightRange;

	ioHashString				m_SkillLoopMotion;
	CEncrypt<float>				m_fSkillLoopMotionRate;

	ioHashString				m_SkillSwingMotion;
	CEncrypt<float>				m_fSkillSwingMotionRate;

	ioHashString				m_SkillSwingedMotion;
	CEncrypt<float>				m_fSkillSwingedMotionRate;

	DWORD						m_dwSkillLoopStartTime;
	CEncrypt<DWORD>				m_dwSkillLoopDuration;
	CEncrypt<DWORD>				m_dwSkillLoopProtectTime;

	CEncrypt<DWORD>				m_dwLoopRotateTime;
	CEncrypt<DWORD>				m_dwEnableSwingTime;

	bool						m_bReverseLoopRotate;

	CEncrypt<float>				m_fTargetLoopGapRange;
	CEncrypt<float>				m_fCharHeightRate;

	//
	CEncrypt<bool>				m_bEnableSwingMove;
	CEncrypt<float>				m_fMoveSpeed;
	CEncrypt<DWORD>				m_dwMoveRotateTime;

	bool						m_bTargetRot;
	D3DXVECTOR3					m_vCurMoveDir;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float>	m_fFindTargetAngle;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void ProcessLoopDir( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );
	void SetTeleport( ioBaseChar *pOwner, ioPlayStage *pStage);//, float fRange, float fWidth, float fHeight );
protected:
	ioBaseChar* FindTargetByWoundedCollision( ioBaseChar *pOwner, ioPlayStage *pStage );
	ioBaseChar* FindTargetByRange( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioTeleportGrapplingSwingSkill();
	ioTeleportGrapplingSwingSkill( const ioTeleportGrapplingSwingSkill &rhs );
	virtual ~ioTeleportGrapplingSwingSkill();
};

inline ioTeleportGrapplingSwingSkill* ToTeleportGrapplingSwing( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TELEPORT_GRAPPLING_SWING )
		return NULL;

	return dynamic_cast< ioTeleportGrapplingSwingSkill* >( pAttack );
}

