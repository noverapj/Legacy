#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioIceMissileWeapon : public ioWeapon
{
protected:
	enum IceState
	{
		IS_MOVE,
		IS_EXPLOSION,
	};
	
	IceState m_IceState;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pIceMesh;

	ioHashString m_BoomEffect;

	bool m_bFirst;
	bool m_bEndExplosion;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	ChangeDirInfoList m_ChangeDirInfoList;

	D3DXVECTOR3 m_vPrePos;
	float m_fDefaultGapHeight;
	float m_fEnableMoveGap;

	bool m_bSetGraceClose;

	bool m_bWeaponSync;
	DWORD	m_PreSyncTime;
	DWORD	m_SyncTerm;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetIceMesh( const ioHashString &szFileName );
	void SetGraceClose();

	void OnFloating( float fTimePerSec, ioPlayStage *pStage );
	bool CheckCollisionLine( ioPlayStage *pStage );

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

	bool IsFloating() const;
	bool IsExplosion() const;

	void Explosion( ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual D3DXVECTOR3 GetMoveDir();
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void SetOwnerChargeScaleRate( float fRate );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	void SetWeaponSync( bool bSync )	{ m_bWeaponSync = bSync; };

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioIceMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioIceMissileWeapon();
};

inline ioIceMissileWeapon* ToIceMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ICE_MISSILE )
		return NULL;

	return dynamic_cast< ioIceMissileWeapon* >( pWeapon );
}

