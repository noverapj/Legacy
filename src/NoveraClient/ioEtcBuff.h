#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

// CatchBuff
class ioCatchBuff : public ioBuff
{
protected:
	DWORD m_dwExtraRopeDuration;

	ioHashString m_CatchEffect;
	ioHashString m_CatchStateEffect;
	ioHashString m_CatchEscapeEffect;
	ioHashString m_CatchRope;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

public:
	ioCatchBuff();
	ioCatchBuff( const ioCatchBuff &rhs );
	virtual ~ioCatchBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// GaugeClearBuff
class ioGaugeClearBuff : public ioBuff
{
public:
	enum GaugeClearType
	{
		GCT_NORMAL,
		GCT_HIGH,
		GCT_FULL_HIGHT,
		GCT_PERCENT,
	};

protected:
	GaugeClearType	m_GaugeClearType;
	EquipSlot		m_eGaugeClearSlot;
	float			m_fClearActiveGaugePercent;
	float			m_fClearPassiveGaugePercent;

	ioHashString	m_ClearBuffEffect;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioGaugeClearBuff();
	ioGaugeClearBuff( const ioGaugeClearBuff &rhs );
	virtual ~ioGaugeClearBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// FearBuff
class ioFearBuff : public ioBuff
{
protected:
	ioHashString m_FearAnimation;
	int m_iEtcStateCnt;

	CEncrypt<bool> m_bMovingDrop;
	CEncrypt<float> m_fMovingDropRate;
	CEncrypt<float> m_fMovingDropNormalRate;

	float m_fMovingSpeed;
	float m_fMovingOffSet;

	ioHashString m_MovingSound;

	bool m_bOnlyArmorDrop;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

public:
	ioFearBuff();
	ioFearBuff( const ioFearBuff &rhs );
	virtual ~ioFearBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// DoubleJump Buff
class ioDoubleJumpBuff : public ioBuff
{
public:
	struct JumpTypeInfo
	{
		JumpType m_eJumpType;
		DWORD m_dwJumpLevel;
	};

	struct ExcpetMercenaryInfo
	{
		DWORD m_dwMercenaryWeaponType;		
		DWORD m_dwJumpLevel;
	};

protected:
	ioHashStringVec m_vJumpAnimationList;
	DWORD m_dwJumpEnableTime;

	bool m_bEnableRotate;

	CEncrypt<float> m_fJumpPower;
	CEncrypt<float> m_fJumpForce;
	CEncrypt<float> m_fJumpSpeedRate;
	CEncrypt<float> m_fGravityRate;

	CEncrypt<float> m_fEnableHeight;
	CEncrypt<float> m_fAniRate;
	CEncrypt<float> m_fLandingRate;

	CEncrypt<int> m_iMaxJumpCnt;

	bool m_bJumpAttack;

	typedef std::vector<JumpTypeInfo> JumpTypeLevelList;
	JumpTypeLevelList m_pJumpTypeLevelList;

	typedef std::vector<ExcpetMercenaryInfo> ExcpetMercenaryList;
	ExcpetMercenaryList m_pExcpetMercenaryList;

	CEncrypt<float> m_fBuffOnJumpingEnableGauge;
	ioHashStringVec m_vBuffOnJumping;
	int m_iRandomSeed;
	int m_iOwnerRandomSeed;

	CEncrypt<bool> m_bInvisibleWeaponMeshOnJump;
	bool           m_bHideMesh;

	float		m_fJumpMaxHeight;
	float		m_fJumpDropZoneMaxHeight;
	float		m_fJumpEnableGauge;
	float		m_fGaugePerTickByDropZone;

	float m_fBuffAirWalkStartGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual bool IsCanJumpAttack() const;

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

public:
	DWORD CheckJumpTypeLevel( DWORD dwWeaponItemCode, JumpType eCheckType );

protected:
	void ProcessPassive();

public:
	void CheckRandomSeed( DWORD dwRandomSeed );
	int GetRandomSeed() { return m_iOwnerRandomSeed + m_iRandomSeed; }
	float GetBuffOnJumpingEnableGauge() { return m_fBuffOnJumpingEnableGauge; }

public:
	ioDoubleJumpBuff();
	ioDoubleJumpBuff( const ioDoubleJumpBuff &rhs );
	virtual ~ioDoubleJumpBuff();
};

inline ioDoubleJumpBuff* ToDoubleJumpBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DOUBLE_JUMP )
		return NULL;

	return dynamic_cast< ioDoubleJumpBuff* >( pBuff );
}
/////////////////////////////////////////////////////////////////////////////////////////////
// AbsoluteProtection Buff
class ioAbsoluteProtectionBuff : public ioBuff
{
protected:
	ioHashString m_StartEffect;
	bool m_bSetLoopEffect;

public:
	virtual ioBuff* Clone();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioAbsoluteProtectionBuff();
	ioAbsoluteProtectionBuff( const ioAbsoluteProtectionBuff &rhs );
	virtual ~ioAbsoluteProtectionBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// Eletric Buff
class ioEletricBuff : public ioBuff
{
protected:
	ioHashString m_EletricAnimation;
	ioHashString m_EletricAnimationDown;
	ioHashString m_EletricAnimationDownBack;

	ioHashString m_EletricCatchAnimation;
	ioHashString m_EletricCatchAnimationDown;
	ioHashString m_EletricCatchAnimationDownBack;

	ioHashString m_AddEntity;

	int m_iEtcStateCnt;
	bool m_bBlowWounded;

	bool m_bRseveSetEtcState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

public:
	ioEletricBuff();
	ioEletricBuff( const ioEletricBuff &rhs );
	virtual ~ioEletricBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// ManaShield Buff
class ioManaShieldBuff : public ioBuff
{
protected:
	float m_fStartGauge;
	float m_fPreGauge;
	bool m_bDisableDownState;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckOwnerStateCheck();

public:
	ioManaShieldBuff();
	ioManaShieldBuff( const ioManaShieldBuff &rhs );
	virtual ~ioManaShieldBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// WarpState Buff
class ioWarpStateBuff : public ioBuff
{
public:
	enum PositionType
	{
		PT_START,
		PT_CREATOR,
		PT_STARTING_POINT,
		PT_DIR,
		PT_ENEMY_STARTING_POINT,
		PT_ENEMY_NEAR_CASTER_POINT,
	};

protected:
	ioHashString m_WarpAnimation;
	ioHashString m_WarpCatchAnimation;
	
	PositionType m_PositionType;
	D3DXVECTOR3 m_vCreatorPos;

	int m_iEnablePositionCnt;
	float m_fEnablePositionRange;
	float m_fEnableMaxPositionRange;

	float m_fExtraHeight;
	float m_fExtraHeightMax;
	
	bool m_bSetCreatorPos;
	bool m_bWarpStateCancel;
	bool m_bCheckDropZone;

	bool m_bCheckHeight;
	float m_fCheckLimitHeight;

	// 임시 : 더미상태로 만들기 위한 옵션
	bool m_bChangeDummy;
	bool m_bEnableEndJump;

	bool m_bSkillStartPos;

	// 내 가까이에 적 용병 워프 시킬때의 오프셋 PT_ENEMY_NEAR_CASTER_POINT
	float m_fWarpOffsetNearMyChar;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckWarpPosition();

public:
	ioWarpStateBuff();
	ioWarpStateBuff( const ioWarpStateBuff &rhs );
	virtual ~ioWarpStateBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// CloseOrder Buff
class ioCloseOrderBuff : public ioBuff
{
public:
	enum CloseOrderType
	{
		COT_NONE       = 0,
		COT_NORMAL     = 1, 
		COT_TRACE      = 2,
		COT_IMITATIION = 3,
		COT_AREA_TRACE = 4,
	};

	enum CloseOrderTargetType
	{
		COTT_NONE,
		COTT_AIR,
	};

protected:
	CloseOrderWeaponType m_eCloseOrderWeaponType;

	bool m_bOnlyMoveDir;
	DWORD m_dwDelayTime;
	float m_fDecreaseSpeed;
	float m_fAniRate;

	bool m_bMustBlowWound;

	DWORD m_dwCloseOrderMinTime;
	DWORD m_dwCloseOrderMaxTime;

	DWORD m_dwCloseOrderDuration;

	ioHashString m_PreDelayAnimation;
	ioHashString m_MoveAnimation;

	ioHashString m_PreDelayCatchAnimation;
	ioHashString m_MoveCatchAnimation;

	typedef std::vector< float > FloatVec;
	FloatVec m_AngleList;

	CloseOrderType m_eCloseOrderType;
	CloseOrderTargetType m_eCloseOrderSubType;

	float m_fCarefulStepHeight;
	DWORD m_dwTargetAreaWeaponIdx;
	DWORD m_dwTargetWeaponIdx;
	
	D3DXVECTOR3 m_vTargetPos;

	bool m_bEndCloseOrder;

	CEncrypt<float>	m_fMoveRangeToStop;

protected:
	float GetDegOwnerCreator();
	void RotateToCreator();
	void RotateToWeapon();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

public:
	inline bool IsMustBlowWound() const { return m_bMustBlowWound; }
	inline float GetCarefulStepHeight() const { return m_fCarefulStepHeight; }

	inline CloseOrderType GetCloseOrderType() const { return m_eCloseOrderType; }

	void SetTargetAreaWeapon( DWORD dwIndex );
	void SetTargetWeapon( DWORD dwIndex );

public:
	ioCloseOrderBuff();
	ioCloseOrderBuff( const ioCloseOrderBuff &rhs );
	virtual ~ioCloseOrderBuff();
};

inline ioCloseOrderBuff* ToCloseOrderBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CLOSEORDER )
		return NULL;

	return dynamic_cast< ioCloseOrderBuff* >( pBuff );
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Float Buff
class ioFloatBuff : public ioBuff
{
public:
	enum CancelType
	{
		FCT_NONE,
		FCT_SEMI_CANCEL,
		FCT_ALL_CANCEL,
		FCT_NO_CANCEL,
	};

	enum MoveType
	{
		MVT_NONE,
		MVT_NORMAL,
	};

	enum LandEndType
	{
		LET_NORMAL,
		LET_NO_END,
	};

	enum WeaponAttachType
	{
		WAT_NONE,
		WAT_XZ,
		WAT_ALL,
		WAT_AREA_XZ,
	};

protected:
	FloatBuffInfo m_FloatBuffInfo;

	float m_fFloatOffHeight;

	ioHashString m_FloatAnimation;
	ioHashString m_FloatCatchAnimation;

	WeaponAttachType m_WeaponAttachType;
	float m_fFloatMoveSpeed;
	D3DXVECTOR3 m_vFloatMoveDir;
	bool m_bNoColCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	void SetMoveDir( D3DXVECTOR3 vDir )		{ m_vFloatMoveDir = vDir; }

protected:
	bool CheckWeaponAttach();
	bool CheckAreaWeaponAttach();
	void CheckWeaponAttachEnd();

public:
	ioFloatBuff();
	ioFloatBuff( const ioFloatBuff &rhs );
	virtual ~ioFloatBuff();
};

inline ioFloatBuff* ToFloatBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FLOAT_BUFF )
		return NULL;

	return dynamic_cast< ioFloatBuff* > ( pBuff );
}

// Float Mode Event Buff
class ioFloatModeEventBuff : public ioFloatBuff
{
public:
	virtual ioBuff* Clone();

public:
	virtual void EndBuff();

public:
	ioFloatModeEventBuff();
	ioFloatModeEventBuff( const ioFloatModeEventBuff &rhs );
	virtual ~ioFloatModeEventBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// JumpPowerRate Buff
class ioJumpPowerRateBuff : public ioBuff
{
protected:
	float m_fJumpPowerRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioJumpPowerRateBuff();
	ioJumpPowerRateBuff( const ioJumpPowerRateBuff &rhs );
	virtual ~ioJumpPowerRateBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// LandingRate Buff
class ioLandingRateBuff : public ioBuff
{
protected:
	float m_fLandingRate;
	float m_fExtendLandingRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioLandingRateBuff();
	ioLandingRateBuff( const ioLandingRateBuff &rhs );
	virtual ~ioLandingRateBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// ScreenBlind Buff
class ioScreenBlindBuff : public ioBuff
{
protected:
	ioHashString m_szBlindName;
	ScreenBlindTime  m_BlindTime;

	int m_iMaxAlpha;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void EndBuff();

public:
	ioScreenBlindBuff();
	ioScreenBlindBuff( const ioScreenBlindBuff &rhs );
	virtual ~ioScreenBlindBuff();
};
/////////////////////////////////////////////////////////////////////////////////////////////
// PushBlowEnhance Buff
class ioPushBlowEnhanceBuff : public ioBuff
{
protected:
	float m_fPushRate;
	float m_fAirPushRate;
	float m_fBlowRate;
	float m_fAirBlowRate;

	DWORD m_dwPushID;
	DWORD m_dwBlowID;
	DWORD m_dwAirPushID;
	DWORD m_dwAirBlowID;

	bool m_bDisableDownState;
	bool m_bWoundEnhance;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckOwnerStateCheck();

public:
	ioPushBlowEnhanceBuff();
	ioPushBlowEnhanceBuff( const ioPushBlowEnhanceBuff &rhs );
	virtual ~ioPushBlowEnhanceBuff();
};

