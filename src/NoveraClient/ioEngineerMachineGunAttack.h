#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;
class ioEngineerGatlingDummy;
class ioWeapon;
class ioSimpleBoxWeapon;

class ioEngineerMachineGunAttack : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_CHARGING,
		SS_GATLING_ATTACK,
		SS_BEAM_ATTACK,
		SS_SHOCK_ATTACK,
		SS_AIR_TRACE_ATTACK,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_DIR_KEY,
		SSS_CREATE_DUMMY,
		SSS_LOOP,
		SSS_CHARGING,
		SSS_GATLING_ATTACK,
		SSS_BEAM_ATTACK,
		SSS_SHOCK_ATTACK,
		SSS_AIR_TRACE_ATTACK,
	};

	enum LoopInit
	{
		LI_NONE,
		LI_START,
	};

	typedef ioUserKeyInput::DirKeyInput DirKey;

	struct AttackWeaponInfo
	{
		WeaponInfo		m_WeaponInfo;
		DWORD			m_dwFireTime;
		D3DXVECTOR3		m_vOffset;

		AttackWeaponInfo()
		{
			m_dwFireTime	= 0;
			m_vOffset		= ioMath::VEC3_ZERO;
		}
	};
	typedef std::vector<AttackWeaponInfo> AttackWeaponInfoList;
	typedef std::vector< ioWeapon* > FireWeaponList;

protected:
	SkillState				m_SkillState;
	DWORD					m_dwSkillProtectTime;

protected:
	int						m_iMaxBullet;
	int						m_iCurrBullet;
	int						m_iGatlingAttackNeedBullet;
	int						m_iBeamAttackNeedBullet;
	int						m_iShockAttackNeedBullet;
	int						m_iAirTraceAttackNeedBullet;

protected:
	AttackWeaponInfoList	m_CurrAttackWeaponInfoList;
	D3DXVECTOR3				m_SimpleWeaponOffset;
	DWORD					m_dwWeaponBaseIndex;

protected:
	ioHashString			m_szDummyName;
	int						m_iDummyCharIndex;

protected:
	ioHashString			m_szLoopAnimation;	
	float					m_fLoopAnimationRate;

	DWORD					m_dwLoopDuration;
	DWORD					m_dwLoopStartTime;

protected:
	ioHashString			m_szChargingAnimation;	
	float					m_fChargingAnimationRate;

	DWORD					m_dwChargingTime;
	DWORD					m_dwChargingStartTime;

protected:	
	ioHashString			m_szGatlingAttackAnimation;
	float					m_fGatlingAttackAnimationRate;
	DWORD					m_dwGatlingAttackStartTime;
	DWORD					m_dwGatlingAttackEndTime;
		
	AttackWeaponInfoList	m_GatlingAttackList;


protected:
	ioHashString			m_szBeamAttackAnimation;
	float					m_fBeamAttackAnimationRate;

	DWORD					m_dwBeamAattackBulletUseTime;
	DWORD					m_dwBeamAattackBulletUseStartTime;

	AttackWeaponInfoList	m_BeamAttackList;
	IntVec					m_BeamWeaponIndexList;

protected:	
	ioHashString			m_szShockAttackAnimation;
	float					m_fShockAttackAnimationRate;
	DWORD					m_dShockAttackStartTime;
	DWORD					m_dShockAttackEndTime;

	AttackWeaponInfoList	m_ShockAttackList;

protected:
	ioHashString			m_szAirTraceAttackAnimation;
	float					m_fAirTraceAttackAnimationRate;
	DWORD					m_dwAirTraceAttackStartTime;
	DWORD					m_dwAirTraceAttackEndTime;

	AttackWeaponInfoList	m_AirTraceAttackList;

protected:
	DirKey					m_CurDirKey;
	float					m_fRotateSpeed;
	float					m_fBeamRotateSpeed;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void LoadAttackWeaponInfo( ioINILoader &rkLoader, const char *szTtitle, AttackWeaponInfoList& List );

protected:
	void CheckWeaponItemMesh( ioBaseChar* pChar );

protected:
	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, OUT DWORD& dwStartTime, DWORD dwPreDelay );
	int SetLoopAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate );

protected:
	void AttackFire( ioBaseChar* pOwner, DWORD dwStartTime, DWORD dwIndexBase );

protected:
	void CreateDummy( ioBaseChar* pOwner, ioPlayStage *pStage, int iCurIndex );	
	void CheckDummy( ioBaseChar *pOwner, ioPlayStage *pStage );
	void DestroyDummy( ioBaseChar* pOwner );
	ioEngineerGatlingDummy* GetDummy( ioBaseChar* pOwner );

public:
	virtual bool HasSkillMotion() const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:	
	void SetLoopState( ioBaseChar* pOwner, ioPlayStage* pStage, LoopInit eInit = LI_START );
	void SetChargingState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetGatlingAttackState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetBeamAttackState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetShockAttackState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetAirTraceAttackState( ioBaseChar* pOwner, ioPlayStage* pStage );

protected:	
	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckChargingState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckGatlingAttacktState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckBeamAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckShockAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckAirTraceAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	void CheckWeapon( ioBaseChar *pOwner );
	void RemoveWeapon( ioBaseChar *pOwner );


protected:
	bool IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey );

	void ProcessRotate( ioBaseChar *pOwner, float fRotateTime );
	void CheckKeyInput( ioBaseChar* pOwner );

public:	
	inline int GetCurBullet(){ return m_iCurrBullet; }
	inline int GetMaxBullet(){ return m_iMaxBullet; }
	inline EquipSlot GetOwerItemEquipSlot(){ return ES_WEAPON; }

public:
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	ioEngineerMachineGunAttack();
	ioEngineerMachineGunAttack( const ioEngineerMachineGunAttack &rhs );
	virtual ~ioEngineerMachineGunAttack();
};

inline ioEngineerMachineGunAttack* ToEngineerMachineGunAttack( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill(	pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_ENGINEER_MACHINE_GUN )
		return NULL;

	return dynamic_cast< ioEngineerMachineGunAttack* >( pSkill );
}

