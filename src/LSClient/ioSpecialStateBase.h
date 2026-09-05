#pragma once

class ioBaseChar;

class ioSpecialStateBase
{
protected:
	enum 
	{
		STATE_FREEZING_VALUE = 500,
	};

	ioBaseChar* m_pOwner;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );					//초기화(SetState에 의해서 현재 State를 생성할때)
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );	//마무리(SetState에 의해서 다른 State를 생성하여 현재 State를 소멸시킬때)
	virtual void LoadProperty( ioINILoader &rkLoader ){}

public:
	virtual const char* GetStateName() PURE;
	virtual const CharState GetState() PURE;

public:
	//공격속도 보정함수
	virtual float GetAttackSpeedRate( ioBaseChar* pOwner, float fMaxSpeed ){ return fMaxSpeed; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner ){}
	virtual void ProcessState( IN ioBaseChar* pOwner ){}
	virtual void ProcessMoveSpeed( ioBaseChar* pOwner ){}

	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

public:
	virtual bool IsCanNoWound( ioBaseChar* pOwner ) { return false; };
	virtual bool IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon ) { return true; }
	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const { return false; }
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
										DWORD dwStartTime,
										DWORD dwEndTime,
										DWORD dwSkipType,
										bool bTeamOnly,
										bool bDefense,
										TeamType eOwnerTeam,
										TeamType eOtherTeam ) const;

	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return true; }	
	virtual bool IsIgnoreAttackOverDelay(){ return false; }
	virtual bool IsLandingEnable(){ return true; }
	virtual bool IsFallStateCheck(){ return true; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return true; }
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return false; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	virtual bool IsCanChangeJumppingState( ioBaseChar* pOwner ) { return false; }

	virtual bool IsEnableDashState( ioBaseChar* pOwner ){ return false; }
	virtual bool IsCanKeyMove( ioBaseChar* pOwner ) { return false; }
	virtual bool IsCanKeySpeedMove( ioBaseChar* pOwner ) { return false; }
	virtual bool IsEnableMoveState( const ioBaseChar* pOwner ) const { return false; }

	//특별상태가 지속되는지 검사하는 함수	
	virtual bool IsStateEnd( ioBaseChar* pOwner ){ return false; }
	virtual bool IsProcessFlyMove(){ return false; }	
	
	virtual bool IsFloatingState( const ioBaseChar * const pOwner ) const { return false; }	
	virtual bool IsAttackState()		{ return true; }

public:
	// 베이스 캐릭터에서 SetState()로 상태 클래가 생성 됬을떄 동기화 선택(중간 입장시 상태를 동기화 하기 위한)
	virtual bool IsInitializeSync(){ return false; }

	// 중간 입장 발생시 동기화 선택
	virtual bool IsJoinSync(){ return true; }

	//중간 입장 동기화
	virtual void FillJoinSync(  SP2Packet &rkPacket, ioBaseChar* pOwner  );
	virtual void ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

	//스테이트 내부 동기화
	virtual void ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioSpecialStateBase();
	virtual ~ioSpecialStateBase();
};