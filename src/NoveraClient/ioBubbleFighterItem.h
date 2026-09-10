#pragma once

#include "ioSpecialStateBase.h"

class ioBubbleFighterItem :	public ioWeaponItem
{
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_ATTACK,
		CS_RETREAT_MOVE,
		CS_CRUSH_BUBBLE_ATTACK,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_CHARGE_ATTACK,
		ST_CHARGE_ATTACK_COMBO,
		ST_CRUSH_BUBBLE_ATTACK,
		ST_RETREAT_MOVE,

		ST_JUMP_ATTACK,
	};

private:
	ChargeState						m_ChargeState;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;

	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Bullet
	int								m_iMaxBullet;
	int								m_iCurBullet;

	//Reload
	ioHashString					m_ReloadAnimation;
	ioHashString					m_ReloadMoveAnimation;
	ioHashString					m_ReloadEffect;
	float							m_fReloadTimeRate;
	std::vector<float>				m_LevelTimeRate;
	float							m_fReloadTimeModifyRate;
	bool							m_bSetReloadMoveAni;
	DWORD							m_dwReloadEndTime;
	DWORD							m_dwNextReloadTime;
	DWORD							m_dwReloadGapTime;

	//Mark
	float							m_fMarkerRange;

	//Charge
	ioUserKeyInput::DirKeyInput		m_AimStartDirKey;
	bool							m_bAimMotionSetted;
	bool							m_bFirstAimMoveMotion;

	D3DXVECTOR3						m_vStartDir;
	ioHashString					m_AimedTarget;
	bool							m_bAimedState;
	bool							m_bAimEffect;
	ioHashString					m_AimEffect;

	float							m_fAimAngle;
	float							m_fAimRange;
	float							m_fMinRange;
	bool							m_bEnableDownAim;

	struct FireMotion
	{
		ioHashString m_FireUp;
		ioHashString m_FireCenter;
		ioHashString m_FireDown;
	};
	typedef std::vector< FireMotion > FireMotionList;

	struct MotionInfo
	{
		FireMotion	m_FireMotion;

		ioHashString m_AimMoveFront;
		ioHashString m_AimMoveBack;
		ioHashString m_AimMoveLeft;
		ioHashString m_AimMoveRight;
	};

	typedef std::vector< MotionInfo > MotionInfoList;
	MotionInfoList					m_MotionInfoList;
	float							m_fAimMoveAniRate;
	AttributeList					m_vExtendAttributeList;
	D3DXVECTOR3						m_vAimedDir;
	DWORD							m_dwRotateStartTime;
	DWORD							m_dwRotateEndTime;
	ioHashStringVec					m_FireEffectList;

	//Retreat
	ioHashString					m_szRetreatMoveFr;
	ioHashString					m_szRetreatMoveBk;
	ioHashString					m_szRetreatMoveRt;
	ioHashString					m_szRetreatMoveLt;

	float							m_fRetreatMoveRateFr;
	float							m_fRetreatMoveRateBk;
	float							m_fRetreatMoveRateRt;
	float							m_fRetreatMoveRateLt;

	float							m_fRetreatMoveForce;
	float							m_fRetreatMoveFriction;

	bool							m_bChargeRetreateMove;

	DWORD							m_dwMoveForceTime;
	D3DXVECTOR3						m_vMoveForceDir;

	float							m_fCurAngle;

private:
//jump
	FireMotionList					m_JumpFireMotionList;
	AttributeList					m_vJumpAttributeList;

private:
//Crush Bubble
	AttackAttribute					m_CrushBubbleAttack;
	ioHashStringVec					m_vCheckCrushBubbleBuff;
	float							m_fCrushBubbleBuffCheckRange;
	D3DXVECTOR3						m_CrushBubbleAttackTargetDir;
private:
	void ClearData();

	void LoadChargeAttack( ioINILoader &rkLoader );
	void LoadRetreat( ioINILoader &rkLoader );
	void LoadReload( ioINILoader &rkLoader );
	void LoadJumpAttack( ioINILoader &rkLoader );
	void LoadCrushBubbleAttack( ioINILoader &rkLoader );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToRetreatMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir, bool bChargeMove );
	void ChangeToCrushBubbleAttack( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessChargeAttack( ioBaseChar *pOwner );
	void ProcessRetreatMove( ioBaseChar *pOwner );
	void ProcessCrushBubbleAttack( ioBaseChar *pOwner );

	//Charge
	void CheckAimMoveAniState( ioBaseChar *pOwner );
	bool CompareAimedTarget( ioBaseChar *pOwner );
	bool SetChangeFireMotion( ioBaseChar *pOwner );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void AimedFireToUp( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bJump,
		bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bJump,
		bool bHold );
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );
	void SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir );

	void CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	
	bool CheckEnableCrushBubbleAttack( ioBaseChar *pOwner );
public:

	void ChangeToChargeAttackState( ioBaseChar *pOwner );

	void SetBubbleFighterJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, OUT ioHashString& szTarget, OUT bool& bAimedBall );
	void ApplyBubbleFighterJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, IN const ioHashString& szTarget, IN const bool& bAimedBall );
	void ProcessBubbleFighterJumpAttack( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime );
private:
	bool SetChangeJumpFireMotion( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime );
	void UpdateAimDir( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	//Reload
public:
	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );
	virtual void OnEndExtendDefense( ioBaseChar *pOwner );

	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	//Bullet
	void WasteBullet();
	bool EnableBulletAttack();
	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void SetCurBullet( int iCurBullet );

	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
public:
	ioBubbleFighterItem(void);
	ioBubbleFighterItem( const ioBubbleFighterItem &rhs );
	virtual ~ioBubbleFighterItem(void);
};

inline ioBubbleFighterItem* ToBubbleFighterItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BUBBLE_FIGHTER )
		return NULL;

	return dynamic_cast< ioBubbleFighterItem* >( pItem );
}