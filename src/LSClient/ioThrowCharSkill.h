#pragma once


#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioThrowCharSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_LOOP,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	struct ThrowTargetInfo
	{
		ioHashString	m_Name;
		D3DXVECTOR3		m_vTargetPos;
		D3DXVECTOR3		m_vCenterPos;
		D3DXVECTOR3		m_vAxis;
		float			m_fAngle;
		ThrowTargetInfo()
		{
			Init();
		}
		void Init()
		{
			m_Name.Clear();
			m_vTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_vCenterPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_vAxis = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_fAngle = 0.0f;
		}
	};
	typedef std::vector<ThrowTargetInfo>	ThrowTargetInfoList;

protected:
	SkillState	m_SkillState;
	ThrowTargetInfoList m_ThrowTargetInfoList;

	ioHashStringVec m_TargetNameList;

	// Weapon
	int				m_iWeaponCount;
	int				m_iCurWeaponCount;
	WeaponInfoList	m_vWeaponInfoList;
	DWORD			m_dwFireStartTime;

	// Loop Motion
	ioHashString	m_SkillLoopMotion;
	CEncrypt<DWORD>	m_dwSkillLoopDuration;
	CEncrypt<float>	m_fSkillLoopAniRate;
	DWORD			m_dwSkillLoopStartTime;
	CEncrypt<DWORD>	m_dwSkillLoopProtectTime;

	// Action Motion
	ioHashString	m_SkillActionMotion;
	CEncrypt<float>	m_fSkillActionAniRate;

	// End Motion
	ioHashString	m_SkillEndMotion;
	CEncrypt<float>	m_fSkillEndAniRate;

	CEncrypt<DWORD>	m_SkillProtectTime;

	CEncrypt<DWORD>	m_dwThrowSpeed;
	CEncrypt<float>	m_fCheckTargetHeight;
	CEncrypt<float> m_fThrowBackOffset;
	float m_fCurAngle;

	// Buff
	ioHashStringVec	m_CancelBuffList;

	// Search target..
	ioHashStringVec m_TargetNameListForWeapon;
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float>	m_fTargetUpHeight;
	CEncrypt<float>	m_fTargetDownHeight;
	TargetWoundType	m_TargetWoundType;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );

protected:
	void FindTarget( ioBaseChar *pChar );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );

	void CheckThrowInfo( ioBaseChar *pChar );
	void CheckTargetChar( ioBaseChar *pChar );
	bool CheckLanding( const ioHashString &szTargetName );

protected:
	void AddCancelBuffToTarget( ioBaseChar *pOwner, ioBaseChar *pTarget );

public:
	ioThrowCharSkill();
	ioThrowCharSkill( const ioThrowCharSkill &rhs );
	virtual ~ioThrowCharSkill();
};

inline ioThrowCharSkill* ToThrowCharSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_THROW_CHAR )
		return NULL;

	return dynamic_cast< ioThrowCharSkill* >(pAttack);
}

