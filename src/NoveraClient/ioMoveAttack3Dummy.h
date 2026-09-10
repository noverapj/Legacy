#pragma once

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

struct MEDummyAttackInfo
{
	DWORD			m_dwWeaponIndex;
	DWORD			m_dwFireTime;
	D3DXVECTOR3		m_vOffset;
	
	MEDummyAttackInfo()
	{
		m_dwWeaponIndex	= 0;
		m_dwFireTime	= 0;
		m_vOffset		= ioMath::VEC3_ZERO;		
	}
};
typedef std::vector<MEDummyAttackInfo>		MEDummyAttackInfoList;

class ioMoveAttack3Dummy : public ioDummyChar
{
public:
	enum OwnerControlSync
	{
		SYNC_ATTACK_KEY    = 1000,
	};

	enum MoveState
	{
		MS_WAIT,
		MS_MOVE,
		MS_FIRE,
	};

protected:
	MoveState  m_MoveState;

	D3DXVECTOR3 m_vTargetPos;
	float		m_fNeedMoveLength;
	float		m_fMoveSpeed;
	bool		m_bCreateWeapon;
	DWORD		m_dwFireStartTime;
	MEDummyAttackInfoList m_MEDummyAttackInfoList;
	MEDummyAttackInfoList m_CurrAttackList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadAttrbuteProperty( ioINILoader &rkLoader, const char* szTitle );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessMoveState( float fTimePerSec );

	void SetWaitState();
	void SetFireState();

public:
	virtual bool IsActivateWeapon();

	void SetMovePosition( const D3DXVECTOR3& vTargetPos );
	
public:
	ioMoveAttack3Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMoveAttack3Dummy();
};

inline ioMoveAttack3Dummy* ToioMoveAttack3Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_MOVE_ATTACK3 )
		return NULL;

	return dynamic_cast< ioMoveAttack3Dummy* >( pDummy );
}