#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioShaolinItem : public ioWeaponItem
{
public:
	enum MonkCharState
	{
		WS_NONE,
		WS_CHARGING,
		WS_DASH_WAIT,
		WS_DASH,
		WS_DASH_ENDING,
		WS_ATTACK_FIRE,
		WS_COMBO,
		WS_RETREAT,
		WS_DASH_EXTEND_ATTACK,
		WS_DASH_JUMP,
		WS_DASH_DEFENSE,
		WS_CHANGE_MODE,
		WE_END,
	};

	enum DashAttackType
	{
		DAT_S,
		DAT_D,
	};

	enum ItemChangeType
	{
		ICT_HAND,
		ICT_STICK,
	};

	enum ItemMeshType
	{
		IMT_NORMAL,
		IMT_EXTRA,
	};

protected:
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_DelayAnimation_extra;
	ioHashString m_RunAnimation_extra;
	ioHashString m_DashAnimation_extra;
	ioHashString m_DefenseAnimation_extra;

	//추가
	ItemMeshType	m_itemMeshType;

	ioHashString	m_NormalAttackMesh;
	ioHashString	m_ExtraAttackMesh;

	ioFormalAttackGrp m_FormalAttack_monk;

	float m_fAutoTargetMinAngle_monk;
	float m_fAutoTargetMaxAngle_monk;
	float m_fAutoTargetRange_monk;

	ioHashString m_JumpReadyAnimation_monk;
	ioHashString m_JumppingAnimation_monk;
	ioHashString m_JumpEndAnimation_monk;
	ioHashString m_ExtendJumppingAnimation_monk;
	ioHashString m_ExtendJumpEndAnimation_monk;

	//////////////////////////////////////////////////////////////////////////

	ioHashStringVec m_AttackReadyAniList;
	ioHashStringVec m_AttackReadyAniListMonk;
	AttributeList m_vChargeComboAttList;
	AttributeList m_vChargeComboAttListMonk;
	AttackAttribute m_ExtraAttackAttribute;

	MonkCharState m_ChargeState;
	DWORD m_dwAttackStartTime;

	CEncrypt<bool> m_bUsedExtraAttack;
	CEncrypt<int> m_iNormalCombo;
	CEncrypt<int> m_iChargeCombo;

	CEncrypt<DWORD> m_dwMaxComboDuration;
	CEncrypt<DWORD> m_dwStartComboTime;
	CEncrypt<DWORD> m_dwComboAttackTimeEnd;

	//
	CEncrypt<float> m_fDodgeForce;
	CEncrypt<float> m_fDodgeFrictionRate;

	CEncrypt<float> m_fDodgeAniRate;
	ioHashStringVec m_vDodgeAniList;

	DWORD m_dwDodgeDefenseEnd;

	//
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;


	//////////////////////////////////////////////////////////////////////////
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fCurGauge;
	CEncrypt<float> m_fIncreaseGauge;

	bool			m_bKeyReserved;
	bool			m_bAttackKeyReserved;
	bool			m_bDefenseKeyReserved;
	bool			m_bJumpKeyReserved;

	ioHashString	m_stDashTarget;
	float			m_fTargetAngle;
	float			m_fTargetRange;
	float			m_fTargetUpHeight;
	float			m_fTargetDownHeight;

	float			m_fExtendTargetAngle;
	float			m_fExtendTargetRange;
	float			m_fExtendTargetUpHeight;
	float			m_fExtendTargetDownHeight;

	AttackAttribute m_DashAttackMove;
	//AttackAttribute m_FullGatherAttack;
	float			m_DashAttackEndJumpAmt;

	DWORD			m_dwMotionEndTime;

	CEncrypt<DWORD> m_dwGatheringTime;
	float			m_fGatherAniRate;
	ioHashString	m_GatherAnimation;

	DWORD			m_dwDashWaitStartTime;
	DWORD			m_dwDashWaitMaxTime;

	ioHashString	m_DashAnimation;
	float			m_fDashAniRate;
	float			m_fDashSpeed;
	float			m_fDashRange;
	D3DXVECTOR3		m_vDashMoveDir;
	float			m_fDashMovedLenth;

	ioHashString	m_DashEndAnimation;
	float			m_fDashEndAniRate;

	float			m_fDashJumpAmt;//대쉬중에 점프전환
	ioHashStringVec m_vChargeBuffList;
	//ioHashString	m_stChargeBuffAni;
	//float			m_fChargeBuffRate;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );


	//D~
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

	void DecreaseGauge();
	//virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	virtual void Initialize( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ChangeAttackType( ioBaseChar *pOwner, ItemMeshType meshtype, bool bMustSend = false, bool bFirst = false );
	void ChangeToggleAttckType( ioBaseChar *pOwner );

	virtual const AttackAttribute* GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;
	virtual const WeaponInfoList& GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ForceInfoList& GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;
	virtual int GetExtraDashAttackMax() const;

	virtual const ioHashString& GetAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual float GetAttackAniRate( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ioHashString& GetEndAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual float GetAttackEndAniRate( AttackType eType, int iSubIdx = 0 ) const;

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;
	virtual void OnReleased( ioBaseChar *pOwner );


	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();
	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

	virtual void OnEquiped( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	//void OnGathering( ioBaseChar *pOwner );
	//void OnFullGather( ioBaseChar *pOwner );
	void OnDash( ioBaseChar *pOwner );
	void OnDashEnding( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	//void ChangeGatheringState( ioBaseChar *pOwner );
	//void ChangeToFullGatherState( ioBaseChar *pOwner );

	void ChangeToDashState( ioBaseChar *pOwner );
	void ChangeToDashEnding( ioBaseChar *pOwner );

	void ChangeDefenseState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToComboState( ioBaseChar *pOwner );
	bool ChangeToRetreatAttack( ioBaseChar *pOwner );

	bool ChangeDashExtendAttack( ioBaseChar *pOwner, int nDashAttackType );
	void ChangeDashEndJump( ioBaseChar *pOwner );

	void ChangeEnd( ioBaseChar *pOwner );

	void SetNextComboState( ioBaseChar *pOwner );
	void SetExtraComboState( ioBaseChar *pOwner );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindDashTarget( ioBaseChar *pOwner );
	void FindExtendAttackTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void CheckDashMoveDir( ioBaseChar *pOwner );

	void KeyReserve( ioBaseChar *pOwner );

public:
	ioShaolinItem();
	ioShaolinItem( const ioShaolinItem &rhs );
	virtual ~ioShaolinItem();


public:
	bool HasDelayAnimation() const;
	bool HasRunAnimation() const;
	bool HasDashRunAnimation() const;
	const ioHashString& GetDelayAnimation() const;
	const ioHashString& GetRunAnimation() const;
	const ioHashString& GetDashRunAnimation() const;

	ioHashString GetDefenseAnimation() const;

	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );

	virtual int GetMaxCombo() const;
	int GetMaxMonkCombo() const;

	virtual const ioHashString& GetJumppingAnimation() const;
	virtual const ioHashString& GetExtendJumppingAnimation() const;

	const ItemMeshType GetMeshType() const { return m_itemMeshType; }

	bool IsCanChangeMode();

	void ShowWeapon( ioBaseChar *pOwner );
	void HideWeapon( ioBaseChar *pOwner );

	virtual void SetVisible( ioBaseChar *pOwner, bool bVisible );

private:

	void ItemEffectSet( ioBaseChar *pOwner );

};

inline ioShaolinItem* ToShaolinItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SHAOLIN )
		return NULL;

	return dynamic_cast< ioShaolinItem* >( pItem );
}

