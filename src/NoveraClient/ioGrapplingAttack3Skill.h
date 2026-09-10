#pragma once


class ioINILoader;
class ioBaseChar;
class ioUroborusGuideArrow;

#include "ioAttackSkill.h"

class ioGrapplingAttack3Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_GRAPPLING,
		SS_CANCEL,
		SS_MOVE_TARGET,
		SS_END,
	};

	enum GrapplingTargetState
	{
		GTS_NONE,
		GTS_STAND_FRONT,
		GTS_STAND_BACK,
	};

	enum GrapplingTargetPosition
	{
		GTP_NONE,
		GTP_AIR,
		GTP_STAND,
		GTP_DOWN,
	};

	enum SkillSync
	{
		SSC_TARGET,
		SSC_GRAPPLING,
		SSC_ATTACK,
		SSC_CANCEL,
	};

protected:
	SkillState				m_SkillState;
	GrapplingTargetState	m_GrapplingTargetState;
	GrapplingTargetPosition	m_GrapplingTargetPosition;

	ioHashString m_GrapplingTargetName;

	WeaponInfoList	m_vWeaponToAirList;
	WeaponInfoList	m_vWeaponToDownList;

	CEncrypt<DWORD>	m_dwSkillProtectTime;

	// 모션 및 공격
	AttributeList	m_vAttackAttributeList;

	// 캔슬 관련
	bool			m_bCancelGrappling;
	CEncrypt<DWORD>	m_dwFastValue;
	CEncrypt<DWORD>	m_dwGrapplingMaxValue;
	DWORD			m_dwGrapplingStartTime;
	DWORD			m_dwGrapplingOrigStartTime;
	DWORD			m_dwCurGrapplingValue;
	CEncrypt<DWORD>	m_dwEnableGrapplingCancelTime;

	ioHashString	m_szCancelMotion;
	ioHashString	m_szTargetCancelMotion;

	ForceInfoList	m_vCancelForce;
	ForceInfoList	m_vTargetCancelForce;

	// 키 연타
	CEncrypt<DWORD>	m_dwEnableKeyTime;
	DWORD			m_dwCheckKeyTime;
	bool			m_bPreLeft;

	// 콤보
	CEncrypt<int>	m_iMaxCombo;
	int				m_iCurCombo;

	bool	m_bSetMotion;
	bool	m_bChangeDirection;

	// 타겟 위치
	D3DXVECTOR3	m_vTargetPosition;

	// 타겟 검색
	ioHashString m_TargetName;
	float m_fTargetRange;
	float m_fTargetMinRange;
	float m_fTargetAngle;

	D3DXVECTOR3 m_vAttackDir;

	// 타겟 이동 관련
	CEncrypt<float>	m_fTargetMoveSpeed;
	CEncrypt<float>	m_fGapRange;
	float			m_fTargetMaxRange;
	float			m_fTargetCurRange;
	D3DXVECTOR3		m_vPrePosition;

	// Guide Effect
	typedef std::vector< UniqueObjID > ArrowList;
	ArrowList m_EffectList;
	ioHashString m_GuideEffect;
	int m_iMaxEffectCount;
	float m_fMaxEffectRange;

	// target end buff
	ioHashString m_TargetEndBuff;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader );

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

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void FindTarget( ioBaseChar *pOwner );
	void CheckTargetState( ioHashString &szTargetName );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void SetSkillCancelState( ioBaseChar *pChar, bool bSendNet );
	void SetMoveTargetState( ioBaseChar *pChar );

	void GrapplingAttackFire( ioBaseChar *pChar );
	void CheckSkillGrapplingCancel( ioBaseChar *pChar );
	void ProcessWoundGrapplingState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool isEnableCancel();
	void SetSkillEndState();

// Guide Effect
protected:
	void ProcessGuideEffect( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos );
	void DestroyArrow();
	bool CheckArrowCreated( ioBaseChar *pOwner );
	void ArrangeArrows( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos );

public:
	ioGrapplingAttack3Skill();
	ioGrapplingAttack3Skill( const ioGrapplingAttack3Skill &rhs );
	virtual ~ioGrapplingAttack3Skill();
};

inline ioGrapplingAttack3Skill* ToGrapplingAttack3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_ATTACK3 )
		return NULL;

	return dynamic_cast< ioGrapplingAttack3Skill* >( pAttack );
}

