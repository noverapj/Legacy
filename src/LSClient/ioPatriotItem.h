#pragma once
class ioPatriotItem : public ioWeaponItem
{
private:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_FIND_TARGET,
		CS_TARGET_ATTACK,
		CS_FAIL,

		//Sync
		CS_UPDATE_TARGET,
		CS_EXTEND_ATTACK_FIRE,
	};

	typedef std::map<ioHashString,DWORD> TargetList;
private:
	//State
	ChargeState		m_ChargeState;

	//Normal Attack
	ioHashStringVec	m_AttackReadyAniList;
	int				m_iCurCombo;

	//Ani
	DWORD			m_dwMotionStartTime;
	DWORD			m_dwMotionEndTime;
	DWORD			m_dwFireStartTime;
	/////////////////////////////
	//D~
	//Speed Rate
	float			m_fGatherMoveSpeedRate;
	//Ani
	ioHashString	m_szGatherLoopMotion;
	ioHashString	m_szGatherMoveLoopMotion;
	float			m_fGatherAniRate;
	bool			m_bGatherMoveAni;
	//Effect
	ioHashString	m_GatherOwnerAuraEffect;
	ioHashString	m_GatherAuraEffect;
	float			m_fHeightOffSet;
	float			m_fGatheringScaleRate;
	float			m_fGatheringScaleMaxRate;
	ioHashString	m_AimEffect;

	UniqueObjID		m_dwGatherAuraEffectID;
	float			m_fCurrScaleRate;

	//Target
	float			m_fTargetAngle;
	float			m_fTargetRange;
	float			m_fEnableTargetHeightGap;
	float			m_fTargetCurrRange;
	float			m_fTargetRangeRate;
	TargetList		m_TargetList;
	//Tic타임이 지나면 새로 타겟을 찾는다.
	DWORD			m_dwFindTargetTic;
	DWORD			m_dwFindTargetTime;

	AttackAttribute	m_ExtendAttack;

	//Fail
	ioHashString	m_szFailAni;
	float			m_fFailAniRate;
	/////////////////////////////


	//Gauge
	float			m_fMaxBullet;
	float			m_fCurBullet;
	float			m_fIncreseGaugeTic;
	float			m_fDecreaseTargetFindTic;
	float			m_fEnableExtendAttackGauge;
	float			m_fDecreaseExtendAttackGauge;

	//For Female
	ioHashString	m_szGatherLoopFemaleMotion;
	ioHashString	m_szGatherMoveLoopFemaleMotion;
	ioHashString	m_szExtendFemaleAttack;
protected:
	void ClearData();
	void ClearState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessTargetFind( ioBaseChar *pOwner );
	void ProcessTargeting( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ProcessExtendAttack( ioBaseChar *pOwner );
	void ProcessFailState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar* pOwner );
	void ChangeToTargetFind( ioBaseChar* pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner );
	void ChangeToExtendFail( ioBaseChar* pOwner );

	void CheckGatherMoveState( ioBaseChar *pOwner );

	void ExtendAttackFire( ioBaseChar *pOwner );

	DWORD AddTargetList( ioBaseChar *pOwner, ioPlayStage* pStage, ioBaseChar* pTarget );
	void RemoveTarget( ioBaseChar *pOwner, ioPlayStage* pStage, ioHashString szTargetName );
	void AllRemoveTargetList( ioBaseChar *pOwner );
	void UpdateTargetMaker( ioBaseChar *pOwner );
	void UpdateGatheringEffect( ioBaseChar *pOwner );

	void RemoveAuraEffect( ioBaseChar *pOwner );

	void DecreaseGague( float fDecraseGauge );
public:
	virtual int GetNeedBullet();
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual float GetAttackSpeedRate() const;

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	ioPatriotItem(void);
	ioPatriotItem( const ioPatriotItem &rhs );
	~ioPatriotItem(void);
};

inline ioPatriotItem* ToPatriotItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PATRIOT_ITEM )
		return NULL;

	return dynamic_cast< ioPatriotItem* >( pItem );
}