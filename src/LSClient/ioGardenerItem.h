#pragma once

#include "ioSpecialStateBase.h"

class ioGardenerItem :	public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_ACTION_WALL_DUMMY,
		CS_MAKE_WALL_DUMMY,
		CS_ACTION_TREE_DUMMY_PRE,
		CS_ACTION_TREE_DUMMY,
		CS_MAKE_TREE_DUMMY,
		CS_CREATE_AREA_WEAPON,
		CS_CREATE_AREA_WEAPON_SYNC,
		CS_CREATE_AREA_WEAPON_END,
		CS_HEAL_AREA_WEAPON,
		CS_RETREAT,
	};

protected:
	struct EffectInfo
	{
		DWORD m_dwEffectID;
		int m_iPosIndex;
	};
	typedef std::vector< EffectInfo > EffectInfoVec;
	struct AreaWeaponInfo
	{
		ioHashString m_szName[2];
	};
	typedef std::vector< AreaWeaponInfo > AreaWeaponInfoVec;

protected:
	ChargeState m_ChargeState;

	//Normal Attack
	ioHashStringVec m_AttackReadyAniList;
	int m_iCurCombo;

	DWORD		m_dwMotionStartTime;
	DWORD		m_dwMotionEndTime;
	
	ioHashString m_szGatheringAni;
	float m_fGatheringAniRate;

	ioHashString m_szCreateAreaWeaponLoopAni;
	ioHashString m_szCreateAreaWeaponEndAni;
	ioHashString m_szHealAreaWeaponAni;
	float m_fCreateAreaWeaponAniRate;
	float m_fHealAreaWeaponAniRate;
	DWORD m_dwCreateAreaWeaponTime;
	int m_iAreaWeaponCount;
	AreaWeaponInfoVec m_AreaWeaponNameVec;
	ioHashString m_szHealAreaWeaponName;
	float m_fAreaWeaponOffset;
	float m_fHealAreaWeaponOffset;

	ioUserKeyInput::DirKeyInput m_CurDirKey;	
	DWORD m_dwGatheringTime;

	int m_iWallDummyMaxCount;
	float m_fWallDummyOffset;
	ioHashString m_szWallDummyName;
	ioHashString m_szWallDummyAni;
	float m_fWallDummyAniRate;
	DWORD m_dwDummyEndTime;
	DWORD m_dwDummyCreateTime;

	float m_fTreeDummyOffset;
	ioHashString m_szTreeDummyName;
	ioHashString m_szTreeDummyAni;
	float m_fTreeDummyAniRate;

	bool m_bDefenseMotionSetted;
	bool m_bDefenseFirstMoveMotion;
	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;
	ioHashString m_AniDefenseFrontMove;
	ioHashString m_AniDefenseBackMove;
	ioHashString m_AniDefenseRightMove;
	ioHashString m_AniDefenseLeftMove;

	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatEndTime;
	DWORD m_dwRetreatForceTime;

	ioHashString m_szCurRetreatAni;
	float m_fCurRetreatAniRate;
	D3DXVECTOR3 m_vForceDir;

	ioHashString m_szAreaWeaponEffectName;
	ioHashString m_szAreaWeaponMeshName;
	DWORD m_dwAreaWeaponEffectIndex;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fTreeNeedGauge;
	CEncrypt<float> m_fWallNeedGauge;
	CEncrypt<float> m_fAreaNeedGauge;
	CEncrypt<float> m_fHealNeedGauge;
	CEncrypt<float> m_fAreaDecreaseGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

protected:
	void ClearData();
	void ClearState();

	void CreateAreaWeaponEffect( ioBaseChar *pChar );
	void DestroyAreaWeaponEffect( ioBaseChar *pChar );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnDummyMake( ioBaseChar *pOwner );
	void OnDummyWallAction( ioBaseChar *pOwner );
	void OnDummyTreeAction( ioBaseChar *pOwner );
	void OnCreateAreaWeapon( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToCraeteAreaWeaponState( ioBaseChar *pOwner );
	void ChangeToHealAreaWeaponState( ioBaseChar *pOwner );
	void ChangeToCraeteAreaWeaponEndState( ioBaseChar *pOwner );
	void ChangeToWallDummyAction( ioBaseChar *pOwner );
	void ChangeToWallDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner );
	void ChangeToTreeDummyActionPre( ioBaseChar *pOwner );
	void ChangeToTreeDummyAction( ioBaseChar *pOwner );
	void ChangeToTreeDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	bool IsEnableMakeTreeDummy( ioBaseChar *pOwner );

	void EndDefense( ioBaseChar *pOwner );
	void CheckDefenseMoveAniDir( ioBaseChar *pOwner );

	bool CheckRetreat( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

protected:
	void ChangeGatheringState( ioBaseChar *pOwner );

public:
	void CreateAreaWeapon( ioBaseChar *pChar, bool bSendNetwork );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge( float fNeedGauge );
	void CheckDecreaseGauge( float fNeedGauge );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual WeaponSubType GetSubType() const;
	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle,	AutoTargetType eType );

	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );
	virtual void CheckRetreatState( ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnReleasedByEndGame( ioBaseChar *pOwner );

protected:
	ioDummyGroup* GetDummyGroup( ioBaseChar* pOwner );
	int SetMakeDummyActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate );

public:
	ioGardenerItem(void);
	ioGardenerItem( const ioGardenerItem &rhs );
	virtual ~ioGardenerItem(void);
};

inline ioGardenerItem* ToGardenerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GARDENER_ITEM )
		return NULL;

	return dynamic_cast< ioGardenerItem* >( pItem );
}

class ioGardenerDelaySpecialState : public ioSpecialStateBase
{
private:
	enum GardenerState
	{
		GS_START,
		GS_DELAY,
		GS_SPECIAL,
	};

	ioHashStringVec m_OwnerBuffList;
	GardenerState m_State;
	DWORD m_dwMotionEndTime;

	ioHashString m_szStartAni;
	ioHashString m_szDelayAni;
	float m_fStartAniRate;
	float m_fDelayAniRate;
	float m_fEndJumpPower;
	float m_fDefenseEndJumpPower;

protected:
	void ChangeDelayState( ioBaseChar *pOwner );
	void ChangeGardenerAttackSpecial( ioBaseChar *pOwner );

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GARDENER_DELAY_SPECIAL"; }
	virtual const CharState GetState(){ return CS_GARDENER_DELAY_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	ioGardenerDelaySpecialState();
	virtual ~ioGardenerDelaySpecialState();
};

inline ioGardenerDelaySpecialState* ToGardenerDelaySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GARDENER_DELAY_SPECIAL )
		return NULL;

	return dynamic_cast< ioGardenerDelaySpecialState* >( pState );
}

class ioGardenerAttackSpecialState : public ioSpecialStateBase
{
private:
	enum GardenerAttackState
	{
		GAS_DELAY,
		GAS_MOVE,
		GAS_END,
	};

	GardenerAttackState m_State;

	ioHashString m_szStartAni;
	ioHashString m_szEndAni;
	float m_fStartAniRate;
	float m_fEndAniRate;

	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;
	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vCircleMoveDir;
	ioHashString m_ReadyCircle;

	float m_fFlightDuration;
	float m_fCharMoveSpeed;
	float m_fCurCharMoveSpeed;
	float m_fCharJumpPower;
	float m_fSkillGravityRate;
	D3DXVECTOR3 m_vCharMoveDir;
	DWORD m_dwActionStartTime;
	DWORD m_dwMotionEndTime;

	WeaponInfoList m_vWeaponInfoList;

protected:
	float GetFlightDuration( float fCurGravity );

	void CheckControl( ioBaseChar *pChar );
	void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	void CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

	bool CheckLanding( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void ProcessEnd( ioBaseChar *pChar );

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GARDENER_ATTACK_SPECIAL"; }
	virtual const CharState GetState(){ return CS_GARDENER_ATTACK_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	ioGardenerAttackSpecialState();
	virtual ~ioGardenerAttackSpecialState();
};

inline ioGardenerAttackSpecialState* ToGardenerAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GARDENER_ATTACK_SPECIAL )
		return NULL;

	return dynamic_cast< ioGardenerAttackSpecialState* >( pState );
}