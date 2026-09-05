#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioNoCollisionWeapon : public ioWeapon
{
protected:
	enum
	{
		WS_MOVE,
		WS_SENCING,
	};

	struct TargetInfo
	{
		ioHashString name;
		int fire_cnt;
		DWORD next_fire_time;
		bool is_fire_end;

		TargetInfo( ioHashString _name )
		{
			name = _name;
			fire_cnt = 0;
			next_fire_time = 0;
			is_fire_end = false;
		}
	};
	typedef std::vector<TargetInfo> vTargetInfo;
	vTargetInfo m_vTargetInfo;

	ioEffect *m_pFireBall;

	int m_WeaponState;

	D3DXVECTOR3 m_vMoveDir;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;
	float	m_fCurMoveSpeed;

	CEncrypt<float> m_fMaxMoveRange;
	CEncrypt<float> m_fSencingRange;
	CEncrypt<DWORD> m_dwSencingTime;

	CEncrypt<int> m_iMaxTargetCnt;
	CEncrypt<int> m_iMaxFireCnt;
	CEncrypt<DWORD> m_dwFireTime;

	DWORD m_dwSencingEndTime;

	TargetWoundType m_TargetWoundType;

	WeaponInfo m_WeaponInfo;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void DestroyFireBall();

	void SetGraceClose();

public:
	float GetMoveSpeed();
	float GetMaxRange();

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );
	virtual void SetWeaponDead();

public:
	virtual bool InitResource();

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

private:
	void CheckTarget();
	void ProcessFire();
	void FireWeapon( ioHashString szTargetName );
	bool IsAlreadyTarget( ioHashString szName );
	bool IsEnableWeaponDie();
	bool CheckDownState( ioBaseChar *pTarget );

public:
	ioNoCollisionWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioNoCollisionWeapon();
};

inline ioNoCollisionWeapon* ToNoCollisionWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_NO_COLLISION_WEAPON )
		return NULL;

	return dynamic_cast< ioNoCollisionWeapon* >( pWeapon );
}

