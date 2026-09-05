#pragma once
#include "iodummychar.h"
class ioPappyDummy :public ioDummyChar
{
private:
	enum PappyState
	{
		PS_START,
		PS_DELAY,
		PS_MOVE,
		PS_RUN,
		PS_JUMP,
		PS_ATTACK,
		PS_RETURN,
		PS_DIE,
	};

	enum PappyAttackState
	{
		PAS_NONE,
		PAS_START,
		PAS_RUSH,
		PAS_RUSH2,
		PAS_BITE,
		PAS_TELEPOLT,
		PAS_HIDE,
	};

private:
	PappyState			m_PappyState;
	PappyAttackState	m_PappyAttackState;
	JumpState			m_OwnerJumpState;

	ioHashString		m_RunAnimation;
	float				m_fRunAniRate;

	ioHashString		m_szJumppingAni;
	float				m_fJumppingAniRate;

	ioHashString		m_szJumpLandingAni;
	float				m_fJumpLandingAniRate;

	D3DXVECTOR3			m_vBaseOffset;
	DWORDVec			m_vWeaponIndexList;

	ioHashString		m_szRushAttackStartAni;
	float				m_fRushAttackStartAniRate;

	ioHashString		m_szRush2AttackStartAni;
	float				m_fRush2AttackStartAniRate;

	ioHashString		m_szTeleportAttackStartAni;
	float				m_fTeleportAttackStartAniRate;

	ioHashString		m_szHideAttackStartAni;
	float				m_fHideAttackStartAniRate;
	ioHashStringVec		m_vHidePappySkillNameList;

	DWORD m_dwAttackStartAniEndTime;
	void SetAttackStartState( ioBaseChar *pOwner );
	void SetLoopAni( const ioHashString& szName, const float& fRate );
	void SetActionAni( const ioHashString& szName, const float& fRate );
	void SetStartAni( const ioHashString& szName, const float& fRate );
	void ProcessAttackStartState( ioBaseChar *pOwner );
private:
	float m_fPappyAttackMoveSpeed;
	float m_fPappyAttackAngleH;
	float m_fPappyAttackAngleV;
	float m_fPappyAttackDuration;
	
	D3DXVECTOR3 m_vPappyMoveDir;
	D3DXVECTOR3 m_vPappyTargetPos;
	float		m_fCurMoveMaxRange;
	float		m_fCurMoveRange;

	//Bite
private:
	ioHashString m_szBiteTargetName;
	ioHashString m_szBiteBuffName;

	//Teleport
	float m_fReplicaTargetAngle;
	float m_fReplicaTargetRange;
	D3DXVECTOR3					m_vReplicaTargetOffset;
	D3DXVECTOR3					m_vReplicaNoneTargetOffset;
	float						m_fMaxReplicaAttackRange;
	float						m_fLimitReplicaAttackOwnerRange;

	//Over Heads
	DWORD						m_dwHideEndTime;
	DWORD						m_dwHideDuration;

	//Return
	float m_fReturnLimitRange;
	float m_fReturnMoveSpeed;

	
private:
	ioHashString m_szReturnAnimation;
	float		m_fReturnAniRate;
	

//public:
	//bool CheckEnableUseCmd( int iType );
private:
	void SetDelayState( ioBaseChar *pOwner, bool bNet );
	void SetRunState();
	void SetJumpState( JumpState OwnerJumpState );
	void SetAttackState( ioBaseChar *pOwner );
	void SetRushState( ioBaseChar *pOwner, const AttackAttribute& rkAttack );
	void SetRush2State( ioBaseChar *pOwner, const AttackAttribute& rkAttack );
	void SetBiteState( ioHashString szTargetName );
	void SetTeleportAttackState( ioBaseChar *pOwner, const AttackAttribute& rkAttack );
	void SetHideState( ioBaseChar *pOwner );
	void SetReturnState( ioBaseChar *pOwner );


	void ProcessStartState( ioBaseChar *pOwner );
	void ProcessDelayState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessRunState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessMoveState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessJumpState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessPos( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessPappyAttackState( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );
	void ProcessAttackState( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );
	void ProcessBiteState( ioBaseChar *pOwner );
	void ProcessHideState( ioBaseChar *pOwner );
	void ProcessTeleprotAttackState( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );

	D3DXVECTOR3 RotateMoveDir( const D3DXQUATERNION& qtOri, float fAngleH, float fAngleV );

	void SetAttackAni( ioBaseChar *pOwner, const AttackAttribute& rkCurAttribute, bool bAction );

	void ClearWeaponIndexList();

	void CheckPappyVisible( ioBaseChar *pOwner );
	bool CheckPappyOwnerVisible( ioBaseChar *pOwner );

	ioHashString FindAttTarget( ioBaseChar *pOwner, float fAngle, float fRange );
public:
	bool CheckEnableUseCmd();
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	
	virtual void SetMoveState();
	virtual void OnPoisonZoneDamage();
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsNeedCheckMapCollision() const;
	virtual void DropZoneCheck();
protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );

	
public:
	ioPappyDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioPappyDummy(void);
};

inline ioPappyDummy* ToPappyDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_PAPPY )
		return NULL;

	return dynamic_cast< ioPappyDummy* >( pDummy );
}