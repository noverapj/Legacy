#pragma once
class ioHackerItem  : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_GATHERING,
		CS_CHARGE_ATTACK,

		//Dash
		CS_HACKER_DASH_ATTACK,
		CS_HACKER_DASH_MOVE,
		CS_HACKER_DASH_ADD_ATTACK,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_GATHERING,
		ST_CIRCLE,
		ST_CHARGE_END,
		ST_EFFECT_CHANGE,
		ST_CHARGE_ATTACK,
		ST_CHARGE_ATTACK_FIRE,
		ST_HACKER_DASH_ATTACK,
		ST_CREATE_DASH_DUMMY,
		ST_HACKER_DASH_MOVE,
		ST_HACKER_DASH_ADD_ATTACK,
	};
protected:
	ChargeState						m_ChargeState;
	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;
	DWORD							m_dwFireStartTime;

	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//////////////////////////////////////////////////////////////////////////
	//Charge Attack
	//Gather
	ioHashString					m_szGatherLoopAni;
	float							m_fGatherLoopAniRate;

	//Circle Effect
	ioHashStringVec					m_vRedReadyCircleList;
	ioHashStringVec					m_vBlueReadyCircleList;

	//Dummy
	ioHashString					m_MagicCircleDummyName;
	D3DXVECTOR3						m_vMagicCircleDummyOffset;
	DWORD							m_MagicCircleDummyCharIndex;

	//Dash Dummy
	ioHashString					m_szDashDummyName;
	D3DXVECTOR3						m_vDashDummyOffset;
	std::list<DWORD>				m_vDashDummyList;
	int								m_iMaxDashDummyCount;

	//charge Attack
	AttributeList					m_vChargeAttackList;
	D3DXVECTOR3						m_vCirclePos;
	WeaponInfoList					m_vChargeAttackWeaponInfoList;
	//////////////////////////////////////////////////////////////////////////

	//Gauge
	int								m_iCurSpecialAttCnt;
	int								m_iMaxSpecialAttCnt;
	int								m_iNeedSpecialAttCnt;
	FloatVec						m_ExtraGaugeInc;

	//////////////////////////////////////////////////////////////////////////
	//Dash Attack
	AttackAttribute					m_HackerExtendDashAttack; 
	AttackAttribute					m_HackerExtendDashAddAttack;

	//Dash Move
	ioHashString					m_szHackerDashMoveFr;
	ioHashString					m_szHackerDashMoveBk;
	ioHashString					m_szHackerDashMoveRt;
	ioHashString					m_szHackerDashMoveLt;

	float							m_fHackerDashMoveRateFr;
	float							m_fHackerDashMoveRateBk;
	float							m_fHackerDashMoveRateRt;
	float							m_fHackerDashMoveRateLt;

	float							m_fHackerDashMoveForce;
	float							m_fHackerDashMoveFriction;

	bool							m_bChargeHackerDasheMove;

	DWORD							m_dwMoveForceTime;
	D3DXVECTOR3						m_vMoveForceDir;
	//////////////////////////////////////////////////////////////////////////

protected:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner, int iIndex  );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	void ChangeToDashMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir );
	void ChangeToExtendDashAddAttackState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessGatheringState( ioBaseChar *pOwner );
	void ProcessLookatCircleDummy( ioBaseChar *pOwner );
	void ProcessChargeAttackState( ioBaseChar *pOwner );
	void ProcessExtendDashAttakState( ioBaseChar *pOwner );
	void ProcessDashMoveState( ioBaseChar *pOwner );
	void ProcessExtendDashAddAttackState( ioBaseChar *pOwner );

	void CreateCircleDummy( ioBaseChar *pOwner, int iIndex );
	void CreateDashDummy( ioBaseChar *pOwner );
	void DestroyCircleDummy( ioBaseChar* pOwner );
	void DestroyAllDashDummy( ioBaseChar* pOwner );
	void ChangeDummyEffectByGauge( ioBaseChar* pOwner, bool bNet );

	void ChargeAttackFire( ioBaseChar* pOwner );
	void CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
public:
	bool IsDummyCharControl( ioBaseChar *pOwner );
	D3DXVECTOR3 GetDummyCharControlPos( ioBaseChar *pOwner );

	void SetHackerDashAttack( ioBaseChar *pOwner );

	inline const D3DXVECTOR3& GetHackerCirclePos( ioBaseChar *pOwner ) { return m_vCirclePos; }

//Gauge
public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	virtual int GetNeedBullet();
	float GetBaseInc();
protected:
	void DecreaseAllGauge();
public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	ioHackerItem(void);
	ioHackerItem( const ioHackerItem &rhs );
	~ioHackerItem(void);
};

inline ioHackerItem* ToHackerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HACKER_ITEM )
		return NULL;

	return dynamic_cast< ioHackerItem* >( pItem );
}