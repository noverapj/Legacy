#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioSylphidMissileWeapon : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_NONE,
		WS_FLOATING,
		WS_PINGPONG,
		WS_GROUND,
		WS_MOVE,
		WS_EXPLOSION,
	};

	enum WeaponMoveType
	{
		WMT_AIR,
		WMT_GROUND,
	};

protected:
	WeaponState m_WeaponState;
	WeaponMoveType m_WeaponMoveType;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pWeaponMesh;
	ioEffect *m_pSylphidEffect;

	ioHashString m_BoomEffect;
	ioHashString m_SylphidEffect;
	
	CEncrypt<float> m_fCurMoveSpeed;
	CEncrypt<float> m_fVerticalSpeed;

	// move
	D3DXVECTOR3 m_vPrePos;
	float m_fDefaultGapHeight;
	float m_fEnableMoveGap;
	CEncrypt<float> m_fCurGravityAmt;

	// sylphid
	CEncrypt<bool> m_bDisableSylphidOwnerRot;
	CEncrypt<float> m_fSylphidSpeedRate;

	CEncrypt<DWORD> m_dwSylphidDuration;
	CEncrypt<DWORD> m_dwSylphidStartTime;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetWeaponMesh( const ioHashString &szFileName );
	void OnMove( float fTimePerSec, ioPlayStage *pStage );
	
	void CheckOwnerRotate();

	bool CheckCollisionLine( ioPlayStage *pStage );

public:
	float GetMoveSpeed();
	bool IsFloating() const;

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual int SetSylphid();
	virtual void SetWeaponDead();

	virtual const ioOrientBox& GetLocalAttackBox() const;
	
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioSylphidMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioSylphidMissileWeapon();
};

inline ioSylphidMissileWeapon* ToSylphidMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_SYLPHID_MISSILE )
		return NULL;

	return dynamic_cast< ioSylphidMissileWeapon* >( pWeapon );
}

