#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioFireWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSuddenAttackItem : public ioWeaponItem
{
public:
	enum SuddenCharState
	{
		WS_NONE,
		WS_CHARGING,
		WS_ATTACK_FIRE,
		WS_CHANGE_MODE,
		WS_EXTEND2_MODE,
		WS_EXTEND2_AIMED_FIRE,
		WE_END,
	};

	enum Extend2ChargeMode
	{
		ECM_NONE,
		ECM_AIMING,
		ECM_AIMED,
		ECM_ZERO_AIMED,
		ECM_HEAD_AIMED,
	};

	enum SyncSubType
	{
		SST_MARKER,
		SST_AIM_STATE,
		SST_JUMP,
	};

	enum DashAttackType
	{
		DAT_S,
		DAT_D,
	};

	enum ItemMeshType
	{
		IMT_NORMAL,
		IMT_EXTRA1,
		IMT_EXTRA2,
	};

protected:
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_DelayAnimation_extra1;
	ioHashString m_RunAnimation_extra1;
	ioHashString m_DashAnimation_extra1;
	ioHashString m_DefenseAnimation_extra1;

	ioHashString m_DelayAnimation_extra2;
	ioHashString m_RunAnimation_extra2;
	ioHashString m_DashAnimation_extra2;
	ioHashString m_DefenseAnimation_extra2;
	//////////////////////////////////////////////////////////////////////////

	//추가
	ItemMeshType	m_itemMeshType;

	ioHashString	m_NormalAttackMesh;
	ioHashString	m_ExtraAttackMesh1;
	ioHashString	m_ExtraAttackMesh2;

	ioFormalAttackGrp m_FormalAttack_Extend1;

	ioFormalAttackGrp m_FormalAttack_Extend2;
	//ioFormalAttackGrp m_FormalAttack_Extend2_Normal;
	//ioFormalAttackGrp m_FormalAttack_Extend2_Zero;
	//ioFormalAttackGrp m_FormalAttack_Extend2_Head;

	float m_fAutoTargetMinAngle_monk;
	float m_fAutoTargetMaxAngle_monk;
	float m_fAutoTargetRange_monk;

	ioHashString m_JumpReadyAnimationExtend1;
	ioHashString m_JumpReadyAnimationExtend2;
	
	ioHashString m_JumppingAnimationExtend1;
	ioHashString m_JumppingAnimationExtend2;
	
	ioHashString m_JumpEndAnimationExtend1;
	ioHashString m_JumpEndAnimationExtend2;
	
	ioHashString m_ExtendJumppingAnimationExtend1;
	ioHashString m_ExtendJumppingAnimationExtend2;
	
	ioHashString m_ExtendJumpEndAnimationExtend1;
	ioHashString m_ExtendJumpEndAnimationExtend2;
	//////////////////////////////////////////////////////////////////////////

	ioHashStringVec m_AttackReadyAniList;
	ioHashString m_AttackReadyAniList_Extend1;
	ioHashString m_AttackReadyAniList_Extend2;

	AttributeList m_vChargeComboAttList;
	AttributeList m_vChargeComboAttListExtend1;
	AttributeList m_vChargeComboAttListExtend2;
	AttackAttribute m_ExtraAttackAttribute;

	SuddenCharState m_ChargeState;
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

	float			m_fExtend1TargetRange;
	float			m_fExtend2TargetRange;

	DWORD			m_dwFullChargeTime;

	DWORD			m_dwChargeTimeExtend2;
	DWORD			m_dwZeroChargeTimeExtend2;
	DWORD			m_dwHeadChargeTimeExtend2;

	//조준점 이펙트
	ioHashString	m_stReadyAimMarker;
	ioHashString	m_stAimingMaker;
	ioHashString	m_stAimedMarker;
	ioHashString	m_stZeroAimedMarker;
	ioHashString	m_stHeadAimedMarker;

	//재장전
	ioHashString	m_ReloadAnimation;
	ioHashString	m_ReloadMoveAnimation;

	//상하 조준 에니
	ioHashString	m_AimUpExtend1;
	ioHashString	m_AimCenterExtend1;
	ioHashString	m_AimDownExtend1;

	ioHashString	m_FireAimUpExtend1;
	ioHashString	m_FireAimCenterExtend1;
	ioHashString	m_FireAimDownExtend1;

	ioHashString    m_JumpAimUpExtend1;
	ioHashString	m_JumpAimCenterExtend1;
	ioHashString	m_JumpAimDownExtend1;

	ioHashString	m_AimUpExtend2;
	ioHashString	m_AimCenterExtend2;
	ioHashString	m_AimDownExtend2;

	ioHashString	m_FireAimUpExtend2;
	ioHashString	m_FireAimCenterExtend2;
	ioHashString	m_FireAimDownExtend2;

	ioHashString	m_FireAimUpExtend2_Head;
	ioHashString	m_FireAimCenterExtend2_Head;
	ioHashString	m_FireAimDownExtend2_Head;

	D3DXVECTOR3		m_vAimedDir;

	//////////////////////////////////////////////////////////////////////////
	ioHashString	m_AimedTarget;
	CharState		m_OwnerPreCharState;
	float			m_fCurAngle;
	//////////////////////////////////////////////////////////////////////////
	float			m_fAngleExtend1;
	float			m_fRangeExtend1;

	float			m_fAngleExtend2;
	float			m_fRangeExtend2;
	//////////////////////////////////////////////////////////////////////////
	//이속 관련
	float			m_fMoveSpeedRateExtend1;
	float			m_fMoveSpeedRateExtend2;
	float			m_fMoveSpeedRateExtend2_Sniper;
	//////////////////////////////////////////////////////////////////////////
	DWORD			m_dwAimTime;
	DWORD			m_dwTrackingEndTime;
	DWORD			m_dwAimStartTime;
	float			m_fTargetAimRate;
	//////////////////////////////////////////////////////////////////////////
	CEncrypt<int>	m_iMaxBullet;
	CEncrypt<int>	m_iCurBullet;

	float m_fReloadTimeModifyRate;
	float m_fFireTimeModifyRate;

	LevelTimeRate m_LevelTimeRate;

	bool m_bSetReloadMoveAni;

	DWORD m_dwReloadStartTime;
	DWORD m_dwReloadEndTime;
	DWORD m_dwNextReloadTime;
	DWORD m_dwReloadGapTime;

	float m_fReloadTimeRate;
	//////////////////////////////////////////////////////////////////////////
	Extend2ChargeMode	m_Extend2Mode;
	Extend2ChargeMode	m_Extend2FireMode;

	//Extend2 조준후 이동 관련
	ioHashString m_AimMoveFront_Extend2;
	ioHashString m_AimMoveBack_Extend2;
	ioHashString m_AimMoveLeft_Extend2;
	ioHashString m_AimMoveRight_Extend2;

	ioHashString m_AimDelayCenter_Extend2;

	bool m_bFirstAimMoveMotion;
	bool m_bAimMotionSetted;

	float m_fAimMoveAniRate;
	bool m_bChangeDir;

	ioUserKeyInput::DirKeyInput m_AimStartDirKey;
	//////////////////////////////////////////////////////////////////////////
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	DWORD m_dwFireStateEndTime;

	DWORD m_dwAimBipeReturnTime;

	DWORD m_dwExtend1FireTimeGap;
	DWORD m_dwExtend1FireTimeStart;

	ioHashString m_DelayCameraBuffExtend2;

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


	//D~
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

	virtual void Initialize( ioBaseChar *pOwner, ioPlayStage* pStage );
	void SetInitialize( const ioHashString& szName, ioPlayStage* pStage, int nMeshType );

	virtual const AttackAttribute* GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;
	virtual const WeaponInfoList& GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ForceInfoList& GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;
	virtual int GetExtraDashAttackMax() const;

	virtual const ioHashString& GetAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual float GetAttackAniRate( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ioHashString& GetEndAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual float GetAttackEndAniRate( AttackType eType, int iSubIdx = 0 ) const;

	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );
	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );

	virtual float GetAttackSpeedRate() const;	

	virtual bool CheckLanding( ioBaseChar *pChar );

	virtual void ProcessLanding( ioBaseChar *pOwner );

	const ioHashString& GetAimReadyMarker();
	const ioHashString& GetAimingMarker();
	const ioHashString& GetAimedMarker();
	const ioHashString& GetZeroAimedMarker();
	const ioHashString& GetHeadAimedMarker();
	float GetExtend2Range();

public:
	virtual WeaponSubType GetSubType() const;
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnEquiped( ioBaseChar *pOwner );

public:
	void ChangeAttackType( ioBaseChar *pOwner, ItemMeshType meshtype, bool bMustSend = false, bool bFirst = false, bool bMustChange = false );

	void ChangeExtend2AimingMode( ioBaseChar *pOwner, bool bSendNet = true );
	void ChangeExtend2AimedMode( ioBaseChar *pOwner, bool bSendNet = true );
	void ChangeExtend2ZeroAimedMode( ioBaseChar *pOwner );
	void ChangeExtend2HeadAimedMode( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeEnd( ioBaseChar *pOwner );

	void ChangeToExtend2AimedFire( ioBaseChar *pOwner );


	void OnAimingExtend2( ioBaseChar *pOwner );
	void OnAimedExtend2( ioBaseChar *pOwner );
	void OnZeroAimedExtend2( ioBaseChar *pOwner );
	void OnHeadAimedExtend2( ioBaseChar *pOwner );
	void OnFireStateExtend2( ioBaseChar *pOwner );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindDashTarget( ioBaseChar *pOwner );
	void FindExtendAttackTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void CheckDashMoveDir( ioBaseChar *pOwner );

	void KeyReserve( ioBaseChar *pOwner );

	void FindAttackTarget( ioBaseChar *pOwner, ItemMeshType eType  );
	bool CheckTargetValidate( ioBaseChar *pOwner, ItemMeshType eType );
	void CompareTarget( ioBaseChar *pOwner );

	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	void CheckExtend2AimMoveAniState( ioBaseChar *pOwner );
	void AimedFireToUp_Extend2( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );

	void AimedFireToDown_Extend2( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );

public:
	ioSuddenAttackItem();
	ioSuddenAttackItem( const ioSuddenAttackItem &rhs );
	virtual ~ioSuddenAttackItem();

public:
	const ioHashString& GetDelayCameraBuffExtend2() const;
	bool HasDelayAnimation() const;
	bool HasRunAnimation() const;
	bool HasDashRunAnimation() const;
	const ioHashString& GetDelayAnimation() const;
	const ioHashString& GetRunAnimation() const;
	const ioHashString& GetDashRunAnimation() const;

	virtual bool HasJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const;
	virtual const ioHashString& GetJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const;

	ioHashString GetDefenseAnimation() const;

	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );

	virtual int GetMaxCombo() const;
	//int GetMaxMonkCombo() const;
	int GetExtend_1_Combo() const;
	int GetExtend_2_Combo() const;

	virtual const ioHashString& GetJumppingAnimation() const;
	virtual const ioHashString& GetExtendJumppingAnimation() const;

	const ItemMeshType GetMeshType() const { return m_itemMeshType; }

	//bool IsCanChangeMode();

	void ShowWeapon( ioBaseChar *pOwner );
	void HideWeapon( ioBaseChar *pOwner );

	virtual void SetVisible( ioBaseChar *pOwner, bool bVisible );

//////////////////////////////////////////////////////////////////////////
	virtual void CallItemProcess( ioBaseChar* pOwner );

	void WasteBullet();

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );

	void CheckAimTarget( ioBaseChar *pOwner );
	void UpdateAimDir( ioBaseChar *pOwner );

	void ProcessTargetTracking_Extend_Delay( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown );
	void ProcessTargetTracking_Extend1_Attack( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown );

	const ioHashString& GetAimedTarget();
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;


	float m_fChargingCamDistanceExtend2;
	float m_fChargingCamHeightExtend2;
	float m_fChargingCamFovExtend2;
	DWORD m_dwChargingCamZoomTimeExtend2;

	float m_fCamHeightExtend2;
	float m_fCamDistanceExtend2;
	float m_fCamFovExtend2;
	float m_dwCamZoomTimeExtend2;

	float m_fRunCamHeightExtend2;
	float m_fRunCamDistanceExtend2;
	float m_fRunCamFovExtend2;
	DWORD m_dwRunCamZoomTimeExtend2;

	inline float GetChargingCamHeightExtend2() const { return m_fChargingCamHeightExtend2; }
	inline float GetChargingCamDistanceExtend2() const { return m_fChargingCamDistanceExtend2; }
	inline float GetChargingCamFovExtend2() const { return m_fChargingCamFovExtend2; }
	inline DWORD GetChargingCamZoomTimeExtend2() const { return m_dwChargingCamZoomTimeExtend2; }

	inline float GetItemCamHeightExtend2() const { return m_fCamHeightExtend2; }
	inline float GetItemCamDistanceExtend2() const { return m_fCamDistanceExtend2; }
	inline float GetItemCamFovExtend2() const { return m_fCamFovExtend2; }
	inline DWORD GetItemCamZoomTimeExtend2() const { return m_dwCamZoomTimeExtend2; }

	inline float GetRunCamHeightExtend2() const { return m_fRunCamHeightExtend2; }
	inline float GetRunCamDistanceExtend2() const { return m_fRunCamDistanceExtend2; }
	inline float GetRunCamFovExtend2() const { return m_fRunCamFovExtend2; }
	inline DWORD GetRunCamZoomTimeExtend2() const { return m_dwRunCamZoomTimeExtend2; }

	void ClearAimedTarget();

private:

	void ItemEffectSet( ioBaseChar *pOwner );

};

inline ioSuddenAttackItem* ToSuddenAttackItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SUDDEN_ATTACK_ITEM )
		return NULL;

	return dynamic_cast< ioSuddenAttackItem* >( pItem );
}

