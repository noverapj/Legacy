#pragma once

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

struct ME2DummyAttackInfo
{
	DWORD			m_dwWeaponIndex;
	DWORD			m_dwFireTime;
	D3DXVECTOR3		m_vOffset;

	ME2DummyAttackInfo()
	{
		m_dwWeaponIndex	= 0;
		m_dwFireTime	= 0;
		m_vOffset		= ioMath::VEC3_ZERO;		
	}
};
typedef std::vector<ME2DummyAttackInfo>		ME2DummyAttackInfoList;

class ioMoveAttack4Dummy : public ioDummyChar
{
public:
	enum OwnerControlSync
	{
		SYNC_ATTACK_KEY    = 1000,
	};

	enum MoveState
	{
		MS_WAIT,
		MS_MOVE_ATTACK,
		MS_FIRE,
	};

protected:
	MoveState  m_MoveState;

	D3DXVECTOR3 m_vTargetPos;
	float		m_fNeedMoveLength;
	float		m_fMoveSpeed;
	bool		m_bCreateWeapon;
	DWORD		m_dwFireStartTime;
	ME2DummyAttackInfoList m_MEDummyAttackInfoList;
	ME2DummyAttackInfoList m_CurrAttackList;

	bool		m_bGravity;

	// 더미 공격 횟수
	int			m_iCurCombo;

	// 더미가 공격할때 적용될 중력 값
	float		m_fAttackStateGravityAmt;
	// 더미 공격 후 사라질 att idx
	IntVec		m_vDisappearAttackIdx;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadAttrbuteProperty( ioINILoader &rkLoader, const char* szTitle );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessPos( float fTimePerSec );

	void SetWaitState();
	void SetFireState();

public:
	virtual bool IsActivateWeapon();

	void SetMovePosition( const D3DXVECTOR3& vTargetPos );
	
	void ApplyGravity( bool bGravity ) { m_bGravity = bGravity; }

	void SetState( DummyCharState eCharState );// { m_State = eCharState; }

	void IncreaseComboCount();
public:
	ioMoveAttack4Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMoveAttack4Dummy();
};

inline ioMoveAttack4Dummy* ToMoveAttack4Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_MOVE_ATTACK4 )
		return NULL;

	return dynamic_cast< ioMoveAttack4Dummy* >( pDummy );
}