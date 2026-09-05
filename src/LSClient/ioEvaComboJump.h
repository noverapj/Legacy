#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioEvaComboJump : public ioExtendJump
{
public:
	enum EvaComboJumpState
	{
		ECJS_NONE,
		ECJS_CHANGE_JUMP_ATTACK,	// 특별 D~ 누른 상태에서 점프 했을 경우에
		ECJS_CHARGE,
		ECJS_NORMAL_ATTACK,
		ECJS_CHANGE_AIM,	// D~
		ECJS_AIM,
		ECJS_FIRE,
	};

protected:
	struct JumpMotionInfo
	{
		ioHashString m_FireUp;
		ioHashString m_FireCenter;
		ioHashString m_FireDown;
	};
	typedef std::vector< JumpMotionInfo > JumpMotionInfoList;
	JumpMotionInfoList m_JumpMotionInfoList;


	EvaComboJumpState	m_EvaComboJumpState;		// 기본 차지 점프 상태
	DWORD				m_dwChargingTime;			// 기본 공격 차징 시간
	DWORD				m_dwChargingStartTime;		//

	// 기본 점프 공격
	bool				m_bReserveJumpAttack;		//
	int					m_iMaxComboCnt;				// 점프 공격 카운트
	int					m_iCurComboCnt;				//
	DWORD				m_dwEnableComboTime_S;		//
	DWORD				m_dwEnableComboTime_E;		//
	AttributeList		m_AttackAttributeList;		// 기본 점프 공격

protected: // Charge Combo
	ioHashString		m_AimedTarget;
	ioHashString		m_FirstAimedTarget;			// 점프후 처음 타겟
	bool				m_bCheckFirstTarget;		//

	ioHashString		m_JumpAimUp;				//
	ioHashString		m_JumpAimCenter;			//
	ioHashString		m_JumpAimDown;				//

	ioHashString		m_stFireUpAni;				//	
	ioHashString		m_stFireCenterAni;			//
	ioHashString		m_stFireDownAni;			//

	float				m_fMarkerRange;				//

	DWORD				m_dwKeyReserveTime;			//
	DWORD				m_dwFireStartTime;			//
	DWORD				m_dwFireEndTime;			//

	AttributeList		m_vChargeComboAttList;		//
	D3DXVECTOR3			m_vAimedDir;				//
	bool				m_bFindJumpTarget;			//
	int					m_iChargeCombo;				//		
	bool				m_bFirstAimBullet;			//

	DWORD				m_dwJumpChargeTime;			// 특별 D~ 누른 상태에서 차징 시간


protected:	// +D , D~
	void	ChangeToJumpAttack( ioBaseChar *pOwner );	
	void	ChangeToAimState( ioBaseChar *pOwner );

	void	ProcessAim( ioBaseChar *pOwner );

	void	SetChangeFireMotion( ioBaseChar *pOwner);	// 빔 발사
	float	GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void	AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold );
	void	AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bJump, bool bHold );


	// +D 
public:	
	void	SetComboJumpAttack( ioBaseChar *pOwner );
	void	ProcessComboJumpAttack( ioBaseChar *pOwner );

public:
	virtual	void LoadProperty( ioINILoader &rkLoader );
	virtual	ioExtendJump* Clone();
	virtual	JumpType GetType() const;


	// 점프 하는 순간 CheckComboJumpAttack() 함수를 호출하여 사용함
	bool IsJumpAttackReserve();

public:	// 기본 점프 함수
	virtual void InitJump();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	// 특별 D~ 누른 상태에서 차징 시간
	virtual DWORD GetChargingTime( ioBaseChar *pChar );

public:
	ioEvaComboJump();
	ioEvaComboJump( const ioEvaComboJump &rhs );
	virtual ~ioEvaComboJump();
};

inline ioEvaComboJump* ToEvaComboJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_EVACOMBO_JUMP )
	{
		return NULL;
	}

	return dynamic_cast< ioEvaComboJump* > (pJump);
}