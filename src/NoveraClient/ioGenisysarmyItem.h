#pragma once

#include "ioSpecialStateBase.h"

class ioGenisysarmyItem : public ioWeaponItem
{
public:
	enum FireState
	{
		FS_NONE,
		FS_NORMAL_ATTACK,
		FS_CHARGING,
		FS_CHARGE_ATTACK,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_JUMP_ATTACK,
		ST_CHARGE_ATTACK,
		ST_CHARGE_ATTACK_COMBO,
	};

protected:
	enum SpecialState
	{
		SS_NONE,
		SS_SPECIAL_DASH,

		// 특별 공격 키 A~
		SS_DUMMY_READY,	
		SS_DUMMY_CHARGING,	
		SS_DUMMY_MAKE,
		SS_DUMMY_MAKE_CREATE,
	};

protected:
	int					m_iAttackMaxComboCnt;					// 기본 공격 카운트
	ioHashString		m_AttackReadyAni;						// 기본 Charge 공격 준비
	int					m_iAttackExtendAttributeCnt;			// 특별 공격 D~ 카운트
	AttackAttribute		m_AttackExtendAttribute;				// 특별 공격 D~	
	FireState			m_FireState;							// 상태 
	DWORD				m_dwAttackStartTime;					// 기본 공격 스타트 시간
	int					m_iCurCombo;							// 현재 콤보 값


	SpecialState		m_SpecialState;							//

	// Cancel Dash
	DWORD				m_dwCancelDashAttackTime;				// 
	DWORD				m_dwMotionEndTime;						// 
	AttackAttribute		m_NormalCancelDashAttribute;			// 기본공격 중 캔슬 대시 


	// 차징 공격
	D3DXVECTOR3			m_vStartDir;							//
	ioHashString		m_AimedTarget;							//
	bool				m_bAimedState;							//
	bool				m_bAimEffect;							//
	ioHashString		m_AimEffect;


	float				m_fMarkerRange;							//
	float				m_fAimAngle;							//
	float				m_fAimRange;							//
	float				m_fMinRange;							//
	float				m_fCurAngle;							//

	// Jump Attack
	ioHashString		m_JumpFireUp;							//
	ioHashString		m_JumpFireCenter;						//
	ioHashString		m_JumpFireDown;							//

	ioHashString		m_AimMoveFront;							//
	ioHashString		m_AimMoveBack;							//
	ioHashString		m_AimMoveLeft;							//
	ioHashString		m_AimMoveRight;							//

	ioHashString		m_FireUp;								//
	ioHashString		m_FireCenter;							//
	ioHashString		m_FireDown;								//

	AttackAttribute		m_JumpChargeAttribute;					//
	D3DXVECTOR3			m_vAimedDir;							//
	DWORD				m_dwRotateStartTime;					//
	DWORD				m_dwRotateEndTime;						//
	ioHashString		m_FireEffect;							//

	// 특별 공격 키 D~
	ioUserKeyInput::DirKeyInput		m_AimStartDirKey;			//
	bool							m_bAimMotionSetted;			//
	bool							m_bFirstAimMoveMotion;		//
	float							m_fAimMoveAniRate;
	bool							m_bEnableDownAim;			// 다운타격 가능 여부 2020-07-16

	// 특별 공격 키 A~
	AttackAttribute					m_DummyChargeAttribute;		//
	GroupDummyInfoList				m_DummyInfoList;			//
	int								m_iDummyMaxCnt;				//

	// 특별 공격 A~ ( 더미 생성 )
	float							m_fDummySetupOffset;		//
	ioHashString					m_DummyMakeAnimation;		//
 	float							m_fDummyMakeAnimationRate;	//
	DWORD							m_dwDummyMakeEndTime;		//

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual WeaponSubType GetSubType() const;

public:	// 기본 공격중 캔슬 대시 기능 함수들....
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual	bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	// ioBaseChar::CheckEnableDashCheck() 함수를 체크 하기 위해서는 m_CurDashType == DT_GENISYSARMY 추가해줘야 함 
	// 또한 ioAttackableItem 기본은 무조건 false 리턴시킨다.
	virtual	bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );	

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

public:	// 특별 GenisysarmyspecialState 관리 함수들...
	void ProcessSpecialState( ioBaseChar *pOwner );	
	void ClearAttackState( ioBaseChar *pOwner );

protected: // 특별 GenisysarmyspecialState 관리 함수들...
	void ProcessSpecialDash( ioBaseChar* pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );


public:	// 점프 D~
	void SetComboChargingJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, OUT ioHashString& szTarget, OUT bool& bAimedBall );
	void ApplyJComboChargingJumpAttack( ioBaseChar *pOwner, OUT DWORD& dwInput_Dash_S_StartTime, IN const ioHashString& szTarget, IN const bool& bAimedBall );
	void ProcessComboChargingJumpAttack( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime );

private:	// 기본, 점프 D~ Charge
	bool		CompareAimedTarget( ioBaseChar *pOwner );
	bool		SetChangeJumpFireMotion( ioBaseChar *pOwner, DWORD& dwInput_Dash_S_StartTime );
	float		GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void		AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold );
	void		AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold );
	bool		CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void		UpdateAimDir( ioBaseChar *pOwner );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );
	void		SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir );

	// 특별 공격 D~
	void		CheckAimMoveAniState( ioBaseChar *pOwner );	// 키 이동 상태
	bool		SetChangeFireMotion( ioBaseChar *pOwner );
	void		ProcessChargeAttack( ioBaseChar *pOwner );

public:	// 특별 공격 D~
	void		ChangeToChargeAttackState( ioBaseChar *pOwner );

public:	// 특별 공격 A~ (기본이 점프 길게 누르면 A~)
	virtual	void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	void			SetDummyReadyState( ioBaseChar* pOwner );
	void			SetDummyChargeState( ioBaseChar* pOwner );
	void			ChangeToDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner, int iCurIndex );
	void			SetDummyMakeState( ioBaseChar* pOwner );

protected: // 특별 공격 A~ ( 더미 생성 )
	ioDummyChar*	CreateDummyChar( const ioHashString& szName, ioPlayStage *pStage, ioBaseChar *pOwner, int iCurIndex );
	float			GetDummyNeedGauge( const ioHashString& szDummyName );
	float			GetTotalDummyGauge( ioBaseChar* pOwner );
	void			OnDummyMake( ioBaseChar *pOwner );

	const GroupDummyInfo* const	GetGroupDummyInfo( const ioHashString& szDummyName );

public:
	ioGenisysarmyItem();
	ioGenisysarmyItem( const ioGenisysarmyItem &rhs );
	virtual ~ioGenisysarmyItem();

};

inline ioGenisysarmyItem* ToGenisysarmyItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GENISYSARMY_ITEM )
	{
		return NULL;
	}

	return dynamic_cast< ioGenisysarmyItem* >( pItem );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioGenisysarmySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );

	// ioBaseChar::SetState 함수에서 상태 체크, 이전 특별 상태 삭제할때 호출
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GENISYSARMY_SPECIAL"; }
	virtual const CharState GetState(){ return CS_GENISYSARMY_SPECIAL; }

public:
	// ioBaseChar::ProcessState() 함수에서 디폴트로 함수 계속 호출한다.
	virtual void CheckSpecialState( ioBaseChar* pOwner );	

	// 특별 상태가 지속되는지 검사해서 아니면 false로 리턴하여 ioBaseChar::ProcessMove()를 실행
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );	

public:
	ioGenisysarmySpecialState();
	virtual ~ioGenisysarmySpecialState();
};

inline ioGenisysarmySpecialState* ToGenisysarmySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GENISYSARMY_SPECIAL )
	{
		return NULL;
	}

	return dynamic_cast< ioGenisysarmySpecialState* >( pState );
}