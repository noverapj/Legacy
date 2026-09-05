#pragma once

#include "ioSpecialStateBase.h"

class ioMagneticItem : public ioWeaponItem
{
	enum FireState
	{
		FS_NONE,			
		FS_CHARGING,		
		FS_NORMAL_ATTACK,	

		FS_CHANGE_BUFF,		
		FS_CHANGE_MESH,		
		FS_CHANGE_BUFF_REMOVE,	
		FS_CHANGE_DEFENSE_BUFF, 
	};

	enum WeaponType
	{
		WT_RED_N,	// N극(빨강)
		WT_BLUE_S,	// S극(파랑)
	};

	enum WeaponAttributeBuffType
	{
		WABT_RED_N,		// N극(빨강) 버프 속성
		WABT_BLUE_S,	// S극(파랑) 버프 속성
	};

protected:
	enum SpecialState
	{
		SS_NONE,
		SS_SPECIAL_DASH,		
	};

protected:
	ioHashStringVec			m_AttackRedNReadyAniList;	// 기본 N극 빨강 공격 준비
	AttributeList			m_NormalAttackRedNList;

	ioHashStringVec			m_AttackBlueSReadyAniList;	// 기본 S 공격 준비
	AttributeList			m_NormalAttackBlueSList;


	AttributeList			m_vExtendAttributeList;		// 특별공격 준비
	FireState				m_FireState;				// 상태 
	DWORD					m_dwAttackStartTime;		// 기본 공격 스타트 시간
	int						m_iCurCombo;				// 현재 콤보 값
	SpecialState			m_SpecialState;				//

	// Cancel Dash
	DWORD					m_dwCancelDashAttackTime;			// 
	DWORD					m_dwMotionEndTime;					// 
	AttackAttribute			m_NormalRedNCancelDashAttribute;	// 기본공격 중 캔슬 대시 N극(빨강)
	AttackAttribute			m_NormalBlueSCancelDashAttribute;	// 기본공격 중 캔슬 대시 S극(파랑)

	// Change
	float					m_dwChargeBuffStartTime;	// 
	ioHashString			m_ChargeRedNDelayAnimation;	//	N극(빨강)
	ioHashString			m_ChargeBlueSDelayAnimation;//	S극(파랑)
	ioHashStringVec			m_vChargeBuff;				//
	WeaponAttributeBuffType	m_WeaponAttributeBuffType;	// N극(빨강) 버프, S극(파랑) 버프 속성
	bool					m_bChangeBuff;				// 변경 상태

	// Gauge
	float					m_fMaxBullet;
	float					m_fCurBullet;
	float					m_fNeedBullet;

	float					m_fRecoverGaugeDelay;
	float					m_fRecoverGaugeRun;
	float					m_fRecoverGaugeDash;
	float					m_fRecoverGaugeWound;
	float					m_fReduceGaugeRate;

	float					m_fDecreaseChangeDamage;	// 게이지 감소 값 
	bool					m_bBulletBuffInit;			// 용병 교체 시 버프 해제 사용 유무

	// 무기
	WeaponType				m_WeaponType;				// S극(파랑), N극(빨강)
	int						m_iWeaponRedNMeshCount;		//
	ioHashStringVec			m_WeaponRedNMeshList;		// N극(빨강)

	int						m_iWeaponBlueSMeshCount;	//
	ioHashStringVec			m_WeaponBlueSMeshList;		// S극(파랑)

	// 무기 스킬 이름
	ioHashString			m_strWeaponSkillName;		// 무기 스킬 이름 
	ioHashString			m_strWeaponRedNABName;		// 무기 N극(빨강) 버프
	ioHashString			m_strWeaponBlueSABName;		// 무기 S극(파랑) 버프
	bool					m_bWeaponAttributeBuff;		// 무기 속성 변경 상태

	bool					m_bShowWeaponEquipItem;		// 기본 무기 활성화/비활성화 상태 확인 2020-12-21

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	//virtual bool CheckLanding( ioBaseChar *pChar );
	virtual void SetLandingState( ioBaseChar *pOwner );
	virtual	void UpdateExtraData( ioBaseChar *pOwner );
	virtual bool CheckLanding( ioBaseChar *pChar );

	virtual WeaponSubType GetSubType() const;
	
	virtual void Initialize( ioBaseChar *pOwner, ioPlayStage* pStage );

public:	// 캔슬 대시
	virtual bool	ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual	bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	void	SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

public:	// 특별 ioEvaItem 관리 함수들...
	void ProcessSpecialState( ioBaseChar *pOwner, float fHeightGap = 0.0f );	
	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialDash( ioBaseChar* pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

	// 무기스킬
	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );

public:	// 서브 웨폰 관전 OR 입장 관련 함수
	void FillMagneticWeaponInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetMagneticWeaponInfoList( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

	// D~ 자신에게 붙는 마그네틱 필드 생성
	void OnChargeBuff( ioBaseChar* pOwner );
	void RemoveChargeBuff( ioBaseChar* pOwner );
	void ChangeToChargeBuff( ioBaseChar* pOwner );

	bool IsChargeBuffCheck( ioBaseChar* pOwner );

	// 무기 사용시 버프 체크
	void RemoveWeaponAttributeBuff( ioBaseChar* pOwner );

public: // 무기
	void ChangeMesh( ioBaseChar* pOwner );	
	void ReleaseMesh( ioBaseChar* pOwner );

public:
	virtual void	CallItemProcess( ioBaseChar* pOwner );	// 버프

public:	// 게이지
	virtual	int		GetNeedBullet();
	virtual	int		GetMaxBullet();
	virtual	int		GetCurBullet();
	virtual	void	SetCurBullet( int iCurBullet );

	virtual void	DecreaseChangeDamage( ioBaseChar *pOwner );

	// 무기 타입에 따라 버프 이펙트 표시
	WeaponType		GetWeaponType()	{	return m_WeaponType;	}
	bool			IsGetWeaponTypeRedN();
	bool			IsGetWeaponTypeBlueS();

	// 무기 스킬 버프 사용시
	void	SetWeaponAttributeBuff( bool bWeaponAttributeBuff );
	bool	IsWeaponAttributeBuff( ioBaseChar* pOwner );
	void	SetChangeWeaponAttributeBuffType();	// 함수 호출시 무기 타입 변경함 
	bool	IsGetWeaponAttributeBuffTypeRedN();
	bool	IsGetWeaponAttributeBuffTypeBlueS();


	

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	ioMagneticItem();
	ioMagneticItem( const ioMagneticItem &rhs );
	virtual ~ioMagneticItem();
};

inline ioMagneticItem* ToMagneticItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGNETIC_ITEM )
	{
		return NULL;
	}

	return dynamic_cast< ioMagneticItem* >( pItem );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioMagneticSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );

	// ioBaseChar::SetState 함수에서 상태 체크, 이전 특별 상태 삭제할때 호출
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MAGNETIC_SPECIAL"; }
	virtual const CharState GetState(){ return CS_MAGNETIC_SPECIAL; }

public:
	// ioBaseChar::ProcessState() 함수에서 디폴트로 함수 계속 호출한다.
	virtual void CheckSpecialState( ioBaseChar* pOwner );	
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

	// 특별 상태가 지속되는지 검사해서 아니면 false로 리턴하여 ioBaseChar::ProcessMove()를 실행
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );	

	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	// 2020-12-23

public:
	ioMagneticSpecialState();
	virtual ~ioMagneticSpecialState();
};

inline ioMagneticSpecialState* ToMagneticSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MAGNETIC_SPECIAL )
	{
		return NULL;
	}

	return dynamic_cast< ioMagneticSpecialState* >( pState );
}