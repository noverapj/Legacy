#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;
class ioDummyGradeTurretUnit;

struct AttackInfo
{
	DWORD			m_dwWeaponIndex;
	DWORD			m_dwFireTime;
	D3DXVECTOR3		m_vOffset;
	
	AttackInfo()
	{
		m_dwWeaponIndex	= 0;
		m_dwFireTime	= 0;
		m_vOffset		= ioMath::VEC3_ZERO;
		
	}
};
typedef std::vector<AttackInfo> AttackInfoList;

struct GradeAttackAttribute
{
	AttackInfoList	m_AttackInfoList;
	DWORD			m_dwGapTime;
	float			m_fMaxRange;
	float			m_fMinRange;
	float			m_fHeightMin;

	GradeAttackAttribute()
	{	
		m_dwGapTime		  = 0;
		m_fMaxRange		  = 0.0f;
		m_fMinRange		  = 0.0f;
		m_fHeightMin	  = 0.0f;
	}
};
typedef std::vector<GradeAttackAttribute>		GradeAttackAttributeList;
typedef std::vector<ioDummyGradeTurretUnit*>	DummyGradeTurretUnitList;

class ioDummyGradeTurret : public ioDummyChar
{	
public:
	enum DummyTurretState
	{
		DMS_START,
		DMS_DELAY,
		DMS_END,
	};

	enum GradeState
	{
		GS_FIND_TARGET,
		GS_ROTATE,
	};

	enum SyncState
	{
		SS_FIND_TARGET,
		SS_POS,
		SS_UNIT,

		POS_SYNC_TIME = 1000,
	};

	enum DummyEvent
	{
		DE_NONE,
		DE_UPGRADE,
		DE_MID_SYNC,
	};

	struct TurretSync
	{
		GradeState		eState;		

		D3DXVECTOR3		vTargetDir;
		D3DXVECTOR3		vAttackDir;

		bool			bLeftRot;
		ioHashString	szTargetName;

		TurretSync()
		{
			eState			= GS_FIND_TARGET;
			vTargetDir		= D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
			vAttackDir		= D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
			bLeftRot		= false;
		}
	};

protected:
	DummyTurretState			m_TurretState;
	GradeState					m_GradeState;
	DummyEvent					m_DummyEvent;

	int							m_TurretGrade;

	GradeAttackAttributeList	m_GradeAttackAttributeList;
	DummyGradeTurretUnitList	m_DummyGradeTurretUnitList;
		
	DWORD						m_dwFindTargetRotateSpeed;
	float						m_fFindTargetMinRange;
	float						m_fFindTargetMaxRange;

	ioHashString				m_TargetName;
	D3DXVECTOR3					m_vAttackDir;
	D3DXVECTOR3					m_vTargetDir;
	bool						m_bLeftRot;

	ioHashString m_WeaponFireEffect;
	ioHashString m_WeaponFireSound;

protected:
	ioHashString				m_szExplosionSound;
	ioHashString				m_szUpgradeRangeEffect;
	UniqueObjID					m_UpgradeRangeEffectID;

protected:	
	TurretSync					m_MidTurretSync;
	DWORD						m_dwPosSyncStartTime;

protected:
	D3DXVECTOR3					m_vUpgradePos;
	D3DXVECTOR3					m_vUpgradeAttackDir;
	D3DXQUATERNION				m_qtUpgradeRot;

protected:
	float						m_fCheckColskipRange;
	float						m_fCheckSetUpYRange;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	void LoadAttrbuteProperty( ioINILoader &rkLoader, const char* szTitle );
	void PlayFireEffectNSound();

protected:
	ioDummyGradeTurretUnit* GetUnit( int iIndex );
	void CreateUnit();
	void DestroyUnit();

	void CreateRangeEffect( ioBaseChar* pOwner );
	void UpdateRangeEffect( ioBaseChar* pOwner );
	void DestroyRangeEffect( ioBaseChar* pOwner );

public:
	void SetReserveUpgradeInfo( const D3DXVECTOR3& vPos, const D3DXVECTOR3& vAttackDir, const D3DXQUATERNION& qtRot );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	void ProcessCheckLifeTime( float fTimePerSec );
	bool ProcessMoveDown( ioBaseChar* pOwner, float fTimePerSec );

	void ProcessStartState( float fTimePerSec );
	void ProcessDelayState( float fTimePerSec );
		
	void ProcessFindTarget( ioBaseChar *pOwner );

	bool IsLeftRotate();
	void ProcessRotate( DWORD dwRotateTime );

public:
	ioBaseChar* FindTarget( ioBaseChar *pOwner, float fMinDist, float fLimitDist, float fMinHeight, bool bDownAttack );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );	
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();

public:
	virtual bool IsDummyCharMidSync(){ return true; }
	virtual void FillDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket );
	virtual void ApplyDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;

	virtual int  DontMoveEntityLevel() const;
	virtual void NotifyDontMoveCollisionPushingSkip( ioGameEntity* pColEntity );
	virtual bool IsDummyVsDummyCollisionSkipState( const ioDummyChar *pDummy );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;

public:
	inline void SetAttackDir( D3DXVECTOR3 vDir ){ m_vAttackDir = vDir; }
	const D3DXVECTOR3& GetAttackDir() const { return m_vAttackDir; }
	const ioHashString& GetTargetName() const { return m_TargetName; }

public:
	ioDummyGradeTurret( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyGradeTurret();
};

inline ioDummyGradeTurret* ToDummyGradeTurret( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_GRADE_TURRET )
		return NULL;

	return static_cast< ioDummyGradeTurret* >( pDummy );
}

inline const ioDummyGradeTurret* ToDummyGradeTurretConst( const ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_GRADE_TURRET )
		return NULL;

	return static_cast< const ioDummyGradeTurret* >( pDummy );
}


class ioDummyGradeTurretUnit
{
public:
	enum UnitState
	{
		US_FIND_TARGET,
		US_ATTACK,
	};

	enum SyncUnitState
	{
		SUS_FIND_TARGET,
	};

protected:
	int						m_iUnitIndex;

protected:
	UnitState				m_UnitState;
	GradeAttackAttribute	m_GradeAttackAttribute;
	AttackInfoList			m_CurrAttackList;

	D3DXVECTOR3				m_AttackDir;
	ioHashString			m_TargetName;
	DWORD					m_dwAttackStartTime;
	DWORD					m_dwBaseWeaponIndex;

protected:
	void Init();

public:
	void ProcessState( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret );
	void ProcessFindTarget( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret );
	void ProcessAttackState( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret );

	void ProcessAttackFireList( ioBaseChar *pOwner, ioDummyGradeTurret* pTurret );

protected:
	ioWeapon* AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex );

public:
	void ApplyDummyCharSync( ioDummyGradeTurret* pTurret, SP2Packet &rkPacket );

public:
	ioDummyGradeTurretUnit();
	ioDummyGradeTurretUnit( GradeAttackAttribute Attribute, int iUnitIndex );
	~ioDummyGradeTurretUnit();
};