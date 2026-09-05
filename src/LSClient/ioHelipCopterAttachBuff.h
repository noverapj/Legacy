#pragma once

#include "ioBuff.h"
#include "ioSpecialStateBase.h"

class ioBaseChar;
class ioINILoader;

class ioHelipCopterAttachBuff : public ioBuff
{
protected:
	enum BuffState
	{
		BS_None,
		BS_Move,
		BS_End,
		BS_CreateWeapon,
		BS_StartGauge,
		BS_Attack,
	};

	typedef struct tagFireWeaponInfo
	{
		int m_nWeaponType;
		D3DXVECTOR3 m_vWeaponOffset;
		DWORD m_dwCheckAttackTime;
	}FireWeaponInfo;

protected:
	BuffState m_BuffState;
	D3DXVECTOR3 m_vCirclePos;
	DWORD m_dwAttackTime;
	bool m_bSetEndJump;

protected:
	ioHashStringVec m_OwnerBuffList;

	//카메라
	DWORD m_dwCameraEvent;
	ioHashString m_szCameraBuff;

	bool m_bEnableWeaponSkill;
	bool m_bEnableArmorSkill;
	bool m_bEnableHelmetSkill;
	bool m_bEnableCloakSkill;

	//마법진 설정
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	ioHashString m_ReadyCircle;

	FireWeaponInfo m_CurFireWeaponInfo;

	ioHashString m_szDummyName;
	D3DXVECTOR3 m_vPosOffset;

	ioHashStringVec m_CheckBuffList;
	ioHashString m_szGaugeBuff;

	float m_fEndJumpPower;
	bool  m_bRotateCircle;
	bool  m_bEnableEndJumpByJumpKey;
	bool  m_bDestoryDummy;

	ioHashString m_szLoopAni;
	float		 m_fLoopAniRate;
	ioHashString m_szAttackAni;
	float		 m_fAttackAniRate;
	DWORD		 m_dwFireStartTime;
	DWORD		 m_dwMotionEndTime;

	float m_fWeaponMoveSpeed;
	float m_fWeaponFloatPower;
	float m_fWeaponFloatMinPower;

	CharState m_CheckCreatorState;
	int		  m_iGaugeSlot;
	bool	  m_bDontCamCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );
	void LoadCheckBuffList( ioINILoader &rkLoader );
	
protected:
	void SetCameraBuff( ioBaseChar *pOwner );
	void EndCameraBuff( ioBaseChar *pOwner );
	void SetCameraEvent( ioBaseChar *pOwner );
	void EndCameraEvent( ioBaseChar *pOwner );

protected:
	void ClearData();

protected:
	void SetMoveState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner, bool bEndJump = false );
	void SetAttackState( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );

protected:
	bool CheckAllBuff( ioBaseChar *pOwner );
	bool CheckMyBuff( ioBaseChar *pOwner );
	bool CheckCreatorState();
	void SetLoopAni( ioBaseChar *pOwner, const ioHashString& szAniName, float fRate );
	bool SetPos( ioBaseChar *pOwner );

public:
	bool CheckEnableUseSkill( int iSlot );
	bool IsCamCheck() { return !m_bDontCamCheck; }

protected:
	void CreateMagicCircle( ioBaseChar *pOwner );
	void UpdateMagicCircle( ioBaseChar *pOwner );

protected:
	void CheckInputKey( ioBaseChar *pOwner );

protected:
	void SetGaugeStart( ioBaseChar *pOwner );
	void CreateWeapon( ioBaseChar *pOwner );
protected:
	bool CheckGauge( ioBaseChar *pOwner );
protected:
	bool CheckWeaponFloatInfo( ioBaseChar *pOwner, ioWeapon *pWeapon, ioDummyChar *pDummy );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	
public:
	ioHelipCopterAttachBuff();
	ioHelipCopterAttachBuff( const ioHelipCopterAttachBuff &rhs );
	virtual ~ioHelipCopterAttachBuff();
};

inline ioHelipCopterAttachBuff* ToHelipCopterAttachBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HELICOPTER_ATTACH )
		return NULL;

	return dynamic_cast< ioHelipCopterAttachBuff* >( pBuff );
}

//-----------------------------------------------------------------------------------------------------------

class ioHelipCopterAttachSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_HELICOPTER_ATTACH"; }
	virtual const CharState GetState(){ return CS_HELICOPTER_ATTACH; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioHelipCopterAttachSpecialState();
	virtual ~ioHelipCopterAttachSpecialState();
};

inline ioHelipCopterAttachSpecialState* ToHelipCopterAttachSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_HELICOPTER_ATTACH )
		return NULL;

	return dynamic_cast< ioHelipCopterAttachSpecialState* >( pState );
}