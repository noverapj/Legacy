#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingSkill3 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_TELEPORT,
		SS_ACTION,
		SS_LOOP,
		SS_AUTO_COMBO,
		SS_COMBO,
		SS_SWINGLOOP,
		SS_END,
		SS_SWINGEND,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_COMBO,
		SSC_END,
		SSC_DIR,
		SSC_TELEPORT,
		SSC_SWINGEND,
	};

	enum AutoComboType
	{
		ACT_NONE,
		ACT_NORMAL,
		ACT_LOOP,
	};

	enum EndOtherAttackType
	{
		EOAT_LINEAR,
		EOAT_SWING,
	};

	enum SkillLoopType
	{
		SLT_STOP,
		SLT_NORMAL,
	};
	
protected:
	SkillState	m_SkillState;

	// loop
	ioHashString	m_SkillLoopMotion;
	float			m_fSkillLoopMotionRate;

	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopDuration;
	DWORD m_dwSkillLoopEndTime;

	DWORD m_dwEnableComboTime;

	DWORD m_dwLoopRotateTime;
	CEncrypt<float> m_fTargetLoopGapRange;

	ioHashString m_GrapplingTargetName;

	float m_fCharJumpPower;
	float m_fCharHeightRate;

	// combo
	AutoComboType m_AutoComboType;

	AttributeList m_vComboList;
	ioHashStringVec m_vComboWoundList;

	int m_iCurLinearCombo;
	DWORD m_dwNextComboTime;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	// end other attack
	bool m_bUseEndOtherAttack;
	bool m_bUsedEndOtherAttack;

	AttackAttribute m_EndOtherAttack;
	ioHashString m_EndOtherWoundAni;

	//
	CEncrypt<bool> m_bEnableLoopMove;

	//
	CEncrypt<float> m_fRunSpeed;
	CEncrypt<DWORD> m_dwRotateTime;

	bool m_bTargetRot;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// target teleport
	CEncrypt<bool> m_bUseTelelportTarget;

	TargetColType m_TeleportTargetColType;
	TargetWoundType m_TeleportTargetWoundType;

	CEncrypt<float> m_fTeleportTargetRange;
	CEncrypt<float> m_fTeleportTargetAngle;
	CEncrypt<float> m_fTeleportTargetOffSet;
	CEncrypt<float> m_fTeleportTargetHeightOffSet;

	CEncrypt<float> m_fTeleportTargetUpHeight;
	CEncrypt<float> m_fTeleportTargetUnderHeight;
	CEncrypt<float> m_fTeleportTargetWidth;

	CEncrypt<float> m_fTeleportOffSet;

	ioHashString m_TeleportTargetName;
	D3DXVECTOR3 m_vTeleportTargetPos;
	D3DXQUATERNION m_qtTeleportTargetRot;

	//추가 yong jun
	//자이언트 스윙 관련
	EndOtherAttackType m_EndOtherAttackType;
	SkillLoopType m_SkillLoopType;
	DWORD m_dwSkillSwingLoopStartTime;
	DWORD m_dwSkillSwingLoopDuration;
	DWORD m_dwEnableSwingTime;
	DWORD m_dwSwingLoopRotateTime;
	bool  m_bReverseSwingLoopRotate;
	bool  m_bEnableSwingLoopMove;
	ioHashString  m_SkillSwingEndMotion;
	float		  m_fSkillSwingEndMotionRate;
	ioHashString  m_SkillSwingedMotion;
	float		  m_fSkillSwingedMotionRate;
	D3DXVECTOR3 m_vCurMoveDir;
	GrapplingSkillWoundedInfo	m_SkillSwingGrapplingInfo;
	float	m_fTargetSwingLoopGapRange;
	float   m_fCharSwingHeightRate;
	//스윙에 붙는 웨폰 정보
	int		m_iRotateWeaponCurCnt;	
	BulletInfoList m_RotateWeaponInfoList;
	DWORDVec			m_vecRotateWeaponIndex;

	BuffInfoList		m_SkillEndTargetKillingBuffList;
	//
protected:
	void ClearData();

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
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsJumpState() const;

	virtual bool CheckUseItemMesh();

	bool IsComboState();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillAutoComboState( ioBaseChar *pChar );

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bOther, bool bSendNet=true );

	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

	//추가 yong jun
	void SetSwingState( ioBaseChar *pChar );
	void ProcessSwingLoopRotate( ioBaseChar *pChar );
	void SetSwingSkillEndState( ioBaseChar *pChar, bool bSendNet );
	void CheckSwingMove( ioBaseChar *pChar );
	void DestroyWeapon( ioBaseChar *pChar );
	//

protected:
	void SetActionSkill( ioBaseChar *pOwner );

	void CheckTeleportTarget( ioBaseChar *pOwner );

public:
	ioGrapplingSkill3();
	ioGrapplingSkill3( const ioGrapplingSkill3 &rhs );
	virtual ~ioGrapplingSkill3();
};

inline ioGrapplingSkill3* ToGrappling3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING3 )
		return NULL;

	return dynamic_cast< ioGrapplingSkill3* >( pAttack );
}

