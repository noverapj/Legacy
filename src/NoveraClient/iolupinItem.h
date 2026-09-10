#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class iolupinItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	
	AttackAttribute m_LupinAttribute;

	AttributeList m_SpecialFirstAttribute;
	AttackAttribute m_SpecialSecondAttribute;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
		CS_SPECIAL_LUPIN_ATTACK,
	};

	enum BuffState
	{
		BS_NONE = CS_SPECIAL_LUPIN_ATTACK + 1,
		BS_HIDE,		// 은신
		BS_CAMOUFLAGE,		// 위장
		BS_MAX,
	};

	enum SpecialState
	{
		SS_NONE = BS_MAX + 1,
		SS_REQ_ACTION,
		SS_PRE_ACTION,
		SS_FIXED,
		SS_JUMP_ACTION,
		SS_LOOP,
		SS_ATTACK,
		SS_FALL,
		SS_END,
		SS_DASH,
		SS_CANCEL_DASH,
		SS_CREATE_DUMMY
	};

	enum SpecialActionBullet
	{
		SAB_FIRST_ACTION,
		SAB_SECOND_ACTION,
		SAB_MAX
	};

	enum AttComboType
	{
		ACT_GROUND_ATT_1,
		ACT_GROUND_ATT_2,
		ACT_AIR_ATT_1,
		ACT_MAX
	};

	struct SummonDummyInfo
	{
		ioHashString	m_DummyCharName;
		IntVec			m_vDummyAttCode;
		CEncrypt<int>	m_iCurDummyCharIndex;

		D3DXVECTOR3  m_vDummyCharOffset;
		ioHashStringVec m_vDestroyDummyCharList;

		std::vector< StTopMoveInfo > m_vDummyMoveInfo;
		StTopMoveInfo	m_DummyMoveInfo_Charge;
		SummonDummyInfo()
		{
			m_iCurDummyCharIndex		= -1;
		}
	};

	ChargeState		m_ChargeState;
	BuffState		m_BuffState;
	SpecialState	m_SpecialState;
	DWORD			m_dwAttackStartTime;
	DWORD			m_dwMotionEndTime;
	int m_iCurCombo;

	bool m_bCharged;
	ioHashStringVec m_vLupinList;

	CEncrypt<bool> m_bSpecialLupinAttackState;
	CEncrypt<bool> m_bBlitzBack;
	CEncrypt<int> m_iMaxLupinCombo;
	CEncrypt<int> m_iCurLupinCombo;

	CEncrypt<float> m_fBlitzRange;
	CEncrypt<float> m_fBlitzMaxAngle;
	CEncrypt<float> m_fBlitzMinAngle;

	CEncrypt<float> m_fMaxChangeGauge;
	CEncrypt<float> m_fNeedChangeGauge;

	CEncrypt<float> m_fIncreaseChangeGauge;
	CEncrypt<float> m_fDecreaseChangeGauge;
	CEncrypt<float> m_fCurChangeGauge;

	float			m_fBulletIncreaseRate;

	int		m_iCurBullet;
	int		m_iMaxBullet;
	int		m_iNeedBulletCnt;
	IntVec	m_vNeedBullet;
	float	m_fTempBullet;


	// 특별 동작 관련
	bool m_bPreDelayProtect;
	ioHashString m_PreDelayAnimation;
	DWORD m_dwPreDelayTime;
	DWORD m_dwAnimationEndDelay;

	DWORD m_dwSpecialFirstAttackMoveTime;

	// 하이드 버프 풀리고 나서 다시 쓰기까지의 내부적인 쿨타임
	DWORD m_dwHideCoolTime;
	DWORD m_dwEnableHideTime;

	DWORD m_dwPreSpecialMoveStartTime;

	ioHashString	m_SpecialAttackMotion;
	float			m_fSpecialAttackAniTimeRate;

	float			m_fSpecialAttackJumpPower;
	float			m_fSpecialAttackGravityRate;

	DWORD			m_dwPreActionEndTime;

	float			m_fScatterAngle;
	int				m_iWeaponWay;
	int				m_iMaxWeaponWay;

	ioHashString	m_SpecialLoopMotion;
	float			m_fSpecialLoopMotionRate;
	// 공중 고정 시작 시간
	//DWORD			m_dwAirFixedStartTime;
	DWORD			m_dwLoopDurationEnd;
	DWORD			m_dwLoopDurationStart;
	// 공중 고정할 시간
	DWORD			m_dwFixedTimeInAir;

	// 마법진 위치
	D3DXVECTOR3		m_vMagicCirclePos;

	// 두번째 공격 모션 끝나는 시간
	DWORD			m_dwSpecialSecAttDurationEnd;
	// 두번째 공격 발사모션부분
	DWORD			m_dwFireMotionEndTime;

	float		  m_fActionJumpPower;

	// 특별 상태 중 폭탄 던진 후 (SS_FALL)상태일때 점프 시 최대 속도
	float		 m_fActionJumpMaxSpeed;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;	
	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwWeaponBaseIndex;

	// 기본 공격
	D3DXVECTOR3 m_vTargetPos;
	D3DXVECTOR3 m_vWeaponOffset;

	// 더미 기본 공격
	std::vector< SummonDummyInfo > m_vSummonDummyInfo;

	bool		m_bNormalAttackCheck;

	// 괴도 무기 스킬 사용 시 발동될 버프 이름
	ioHashString m_StartBuffByWeaponSkill;

	// 장비 복사할 타겟
	ioBaseChar*	m_pTargetChar;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );
	virtual void DecreaseChangeDamage( ioBaseChar *pOwner );
	//void DecreaseChangeDamage( ioBaseChar *pOwner, float fDecreaseGauge );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void	UpdateExtraData( ioBaseChar *pOwner );

	void ClearData();

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual	int GetNeedBullet();

	bool IsEnableLupinFall();
	bool IsEnableLupinJumpAction();
	bool IsLupinFall();

	bool	IsEnoughBullet( SpecialActionBullet eSpecialActionBullet );
	void	DecreaseBullet( SpecialActionBullet eSpecialActionBullet );

	void ChangeToDummyChar( ioBaseChar *pOwner, bool bApplyGravity );
	void CreateDummyCharState( IN ioBaseChar *pOwner, OUT D3DXVECTOR3& vPos, OUT int& iIndex, bool bApplyGravity );
	void CheckDummyCharState( ioBaseChar *pOwner, int iIndex );
	void InitDummyCharIndex( ioBaseChar *pOwner, int iIndex );
	void SetMoveDummyPos( ioBaseChar *pOwner, const StTopMoveInfo& info, bool bGround = true );
	void SetMoveDummyPos( ioBaseChar *pOwner, bool bGround, bool bReceive, bool bDash );
	void MoveDummy( ioBaseChar *pOwner, int iAddIndex );
	bool CheckLupinDummy( DWORD dwAttackCode );
	void CreateDummyImmediatelyAttack( ioBaseChar* pOwner, DWORD dwAttackCode, bool bReceive = false );

	bool CheckOwnerDownState( ioBaseChar *pOwner );

	float GetCurBuffTickTime( ioBaseChar *pOwner );

	bool IsLupinSpecialFixedState();
	bool IsLupinSpecialFallState();

	void ClearBuff( ioBaseChar* pOwner, BuffState eBuffState = BS_NONE );

	virtual void CallItemProcess( ioBaseChar* pOwner );

	bool StartPreDelayAnimation( ioBaseChar *pChar );
	void SetPreActionState( ioBaseChar *pChar );

	void SetLoopState( ioBaseChar *pChar, bool bSend );
	void CheckLoopState( ioBaseChar *pChar );

	bool IsNoDropState();

	void SetEndJumpState( ioBaseChar *pChar );

	void SetJumpActionState( ioBaseChar *pChar, bool bSend );

	void SetFallState( ioBaseChar *pChar );

	void ProcessCancelDashState( ioBaseChar *pOwner );

	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

	void DestroyLupinDummy( ioBaseChar *pOwner );
public:
	virtual WeaponSubType GetSubType() const;

public:
	void ClearLupinState( ioBaseChar *pOwner, BuffState eBuffState, bool bSpecialAction = false );
	bool CheckLupinState( ioBaseChar *pOwner );

	ioHashString GetWeaponSkillBuffName() { return m_StartBuffByWeaponSkill; }
protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnBlitzAttack( ioBaseChar *pOwner );
	
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToSpecialLupinAttack( ioBaseChar *pOwner, bool bFirst );		// 위장 상태에서 D꾹하면 점프 후 사방으로 선물 폭턴 투척

	void SetLupinState( ioBaseChar *pOwner, BuffState eBuffState );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	bool CheckBlitzBackTarget( ioBaseChar *pOwner );

public:
	void GetTargetCharByWeaponSkill( ioBaseChar* pOwner, ioBaseChar* pTarget, int iAttributeIndex );
	void ClearTargetCharByWeaponSkill() { m_pTargetChar = NULL; }

	float GetActionJumpMaxSpeed() { return m_fActionJumpMaxSpeed; }

public:
	iolupinItem();
	iolupinItem( const iolupinItem &rhs );
	virtual ~iolupinItem();
};

inline iolupinItem* ToLupinItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LUPIN_ITEM )
		return NULL;

	return dynamic_cast< iolupinItem* >( pItem );
}

