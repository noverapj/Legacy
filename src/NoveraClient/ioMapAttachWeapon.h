#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioMapAttachWeapon : public ioWeapon
{
protected:
	enum AttachState
	{
		TS_NONE,
		TS_DELAY,
		TS_DROP,
		TS_EXPLOSION,
	};

	AttachState m_State;

protected:
	ioEffect *m_pFireMesh;
	ioHashString m_BoomEffect;

	ioOrientBox m_LocalAttackBox;

	DWORD m_dwDelayDuration;

	DWORD m_dwExplosionStart;
	DWORD m_dwExplosionDuration;

	float m_fExplosionRange;
	float m_fExtentsOffSet;

	float m_fDirOffset;
	int m_iConveyerMoveFrameIdx;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:
	void Explosion( ioPlayStage *pStage );
	bool IsExplosion() const;

	void TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual DWORD GetCollisionEnableTime() const;

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;
	virtual bool IsCanMineExplosion();

protected:
	void OnDelayState( float fTimePerSec, ioPlayStage *pStage );
	void OnDropState( float fTimePerSec, ioPlayStage *pStage );
	void OnExplosionState( float fTimePerSec, ioPlayStage *pStage );

public:
	ioMapAttachWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioMapAttachWeapon();
};

inline ioMapAttachWeapon* ToMapAttachWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_MAP_ATTACH )
		return NULL;

	return dynamic_cast< ioMapAttachWeapon* >( pWeapon );
}

