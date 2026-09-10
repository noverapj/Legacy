#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;
class ioPlayStage;

class ioFlameWeapon : public ioWeapon
{
protected:
	enum FlameState
	{
		FS_FALL,
		FS_EXPLOSION
	};
	FlameState m_FlameState;

	ioEffect *m_pMeteor;

	ioHashString m_BoomEffect;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vStartPos;

	float	m_fExplosionRange;
	float	m_fStartHeight;

	ioHashString m_szRedEffect;
	ioHashString m_szBlueEffect;

	UniqueObjID		 m_CircleMapEffectID;
	

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	float GetMoveSpeed();

protected:
	bool CreateMeteor( const ioHashString &szFileName );
	void CreateCircleMapEffect();
	void DeleteCircleMapEffect();
public:
	void SetExplosionRange( float fRange );

	bool IsFalling() const;
	void Explosion( ioPlayStage *pStage );

protected:
	void OnFalling( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

	virtual void SetWeaponDead();
public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioFlameWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioFlameWeapon();
};

inline ioFlameWeapon* ToFlameWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_FLAME )
		return NULL;

	return dynamic_cast< ioFlameWeapon* >( pWeapon );
}

