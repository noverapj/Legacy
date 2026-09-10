#pragma once
#include "ioSpecialStateBase.h"

typedef std::vector<float> LevelTimeRate;

class ioRocketLauncherItem : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_ATTACK_START,
		CS_CHARGE_ATTACK_LOOP,
		CS_CHARGE_ATTACK_END,
	};
	enum SyncType
	{
		ST_NONE,
		ST_NORMAL_ATTACK,
		ST_CHARGE_ATTACK_START,
		ST_CHARGE_ATTACK_LOOP,
		ST_CHARGE_ATTACK_FIRE_START,
		ST_CHARGE_ATTACK_FIRE_END,
		ST_CHARGE_ATTACK_END,
		ST_DIR,
		ST_RELOAD,
	};

	enum SpecialAttackAimStep
	{
		SAAS_ZERO_LEVEL,
		SAAS_FIRST_LEVEL = 0,
		SAAS_SECOND_LEVEL,
		SAAS_THIRD_LEVEL,
		SAAS_MAX_LEVEL
	};

protected:
	ChargeState						m_ChargeState;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;
	//Normal Attack
	//ioHashStringVec					m_AttackReadyAniList;

	// Special Attack
	DWORD							m_dwSpecialAttackStartTime;
	DWORD							m_dwSpecialAttackEndTime;

	int								m_iCurCombo;

	// 처음 차징 액션 타임
	DWORD m_dwChargeFirstActionTime;

	DWORD m_dwFireTime;
	DWORD m_dwFireGapTime;
	int m_iFireCount;

	DWORD m_dwFireGapMaxTime;
	DWORD m_dwFireGapMinTime;
	DWORD m_dwFireGapReduceTime;
	int m_iFireGapReduceCount;

	//Charge
	AttackAttribute	m_ChargeAttackStart;

	AttackAttribute	m_ChargeAttackLoop;

	// 구체 더미 
	int	m_iDummyIndex;
	D3DXVECTOR3 m_vCreatePos;

	ioHashString m_DummyCharName;
	D3DXVECTOR3	m_vDummyCharOffset;

	ioHashStringVec m_vDestroyDummyCharList;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;
	CEncrypt<int> m_iNeedBullet;

	ioHashString m_ReloadAnimation;
	ioHashString m_ReloadMoveAnimation;
	ioHashString m_ReloadEffect;

	CEncrypt<float> m_fReloadTimeRate;
	float m_fReloadTimeModifyRate;
	bool m_bSetReloadMoveAni;

	LevelTimeRate m_LevelTimeRate;
	CEncrypt<DWORD> m_dwReloadStartTime;
	CEncrypt<DWORD> m_dwReloadEndTime;
	CEncrypt<DWORD> m_dwNextReloadTime;
	CEncrypt<DWORD> m_dwReloadGapTime;

	DWORD m_dwWeaponBaseIndex;

	// D꾹 시 더미 생성 후 릴리즈 시 타겟 있을때 발사되는 것을 방지하는 플래그
	bool			m_bReleaseBlock;

	// 특별 상태에서 D키 누를때 미사일 발사
	AttackAttribute	m_ChargeSpecialAttack;
	// 마지막 발사(애니 때문에 추가함)
	AttackAttribute	m_ChargeSpecialLastAttack;
	// 특별상태 미사일 발사 시작 시간
	DWORD			m_dwFireStartTime;
	// 더미와 충돌된 용병이 시간이 오래 있을 수록 오르는 단계 에임 이펙트
	ioHashStringVec	m_vAttactStepAimEffectList;
	ioHashString	m_AimedTarget;

	// D 꾹 발사 시작 ~ 발사 끝까지 True로 되는 플래그
	bool			m_bSpecialAttackFire;

	// 특별상태 해제를 위한 D키 처음 누른 시간 (D꾹이어야 해제)
	DWORD			m_dwSpecialStateReleaseStartTime;
	DWORD			m_dwSpecialStateReleaseEndime;
	DWORD			m_dwChargeTimeForDummyRelease;

	// 특별 상태 공격 단계(조준 구체에 적이 오래있을수록 공격 레벨이 오름)
	int				m_iSpecialAttackLevel;
	// 공격 레벨에 따라서 m_ChargeSpecialAttack을 여러번 사용할때 후 딜레이
	DWORD			m_dwSpecialAttackDelayByLv;			// 딜레이 값
	DWORD			m_dwSpecialAttackDelayTime;

	// 용병의 바라보는 방향의 회전 값 저장 ( 패킷 동기화할때 이 값과 실제 값 차이가 있을 경우에만 패킷 보내도록 함 )
	D3DXQUATERNION	m_qtTargetRot;

protected:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );	
	void ChangeToChargeAttackStart( ioBaseChar *pOwner );
	void ChangeToChargeAttackLoop( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessChargeAttackStart( ioBaseChar *pOwner );
	void ProcessChargeAttackLoop( ioBaseChar *pOwner );
	void ProcessChargeAttackEnd( ioBaseChar *pOwner );
	void ProcessExtendAttack( ioBaseChar *pOwner );

	void ProcessLookatCircleDummy( ioBaseChar *pOwner );

public:
	void InitChargeState( ioBaseChar* pOwner, bool bInitTargetMark, bool bSend );

	void AttachDummyAimEffect( ioBaseChar* pOwner );
	void DeleteSpecialAttackAim( ioBaseChar *pOwner );
	void DeleteSpecialAttackAim( ioBaseChar *pOwner, ioBaseChar *pTargetChar );
	void SetDummyAimTarget( ioBaseChar* pOwner, int iAttackStep, ioHashString stTargetName, bool bSync = false );

	void WasteBullet( ioBaseChar* pOwner );

	void ChangeToExtendAttack( ioBaseChar *pOwner );

	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

	bool IsDummyCharControl( ioBaseChar *pOwner );
	D3DXVECTOR3 GetDummyCharControlPos( ioBaseChar *pOwner );

	void ExtendAttackFire( ioBaseChar *pOwner, bool bLastFire );

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetNeedBullet() { return m_iNeedBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void SetCurBullet( int iCurBullet );

	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void OnEndExtendDefense( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );
	virtual float GetAttackSpeedRate() const;

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	int		GetAttackLevel() { return m_iSpecialAttackLevel; }
	ioHashString GetDummyAimTarget() { return m_AimedTarget; }

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

	virtual void OnReleased( ioBaseChar *pOwner );

public:
	ioRocketLauncherItem(void);
	ioRocketLauncherItem( const ioRocketLauncherItem &rhs );
	~ioRocketLauncherItem(void);
};

inline ioRocketLauncherItem * ToRocketLauncherItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ROCKET_LAUNCHER_ITEM )
		return NULL;

	return dynamic_cast< ioRocketLauncherItem * >( pItem );
}