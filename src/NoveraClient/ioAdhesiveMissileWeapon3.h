#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioAdhesiveMissileWeapon3 : public ioWeapon
{
public:
	enum StateControlSync
	{
		STC_TEST_MAP_COLLISION,
	};

protected:
	ioEffect *m_pFireBall;
	ioOrientBox	m_CollisionBox;

	ioHashString m_BoomEffect;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;

	CEncrypt<float>	m_fStartOffset_x;
	CEncrypt<float>	m_fStartOffset_y;
	CEncrypt<float>	m_fStartOffset_z;

	float m_fCurMoveRange;
	float m_fCurMoveRate;

	bool m_bSetGraceClose;
	bool m_bFirst;

	bool m_bCollisionMap;

	//
	bool m_bOwnerAttach;
	bool m_bCharColState;

	BuffInfoList m_FirstColBuffList;

protected:
	WeaponInfo			m_CallWeaponInfo;
	ChangeDirInfoList	m_CallAttFireList;
	bool				m_bMoveWeapon;
	BOOL				m_bArleadyCallWeapon;

protected:
	void ClearData();

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void DestroyFireBall();

	void SetForceClose();

	float GetMoveSpeed();
	float GetMaxRange();

	void RotateMoveDir( float fAngle );

	void ProcessAttachMove( float fTimePerSec, ioPlayStage *pStage );
	void ProcessNormalMove( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

public:
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );
	virtual void SetWeaponDead();

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual const BuffInfoList& GetTargetAirBuffList() const;
	virtual const BuffInfoList& GetTargetBuffList() const;
	virtual const BuffInfoList& GetTargetDownBuffList() const;

	virtual void CheckWeaponCollision();

	void CheckOwnerAttach( bool bAttach );
	void Explosion( ioPlayStage* pStage );

	inline bool IsOwnerAttach() const { return m_bOwnerAttach; }

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool IsProtectState( ioBaseChar* pChar );
	void CallAttribute();

public:
	ioAdhesiveMissileWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAdhesiveMissileWeapon3();
};

inline ioAdhesiveMissileWeapon3* ToAdhesiveMissileWeapon3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ADHESIVE_MISSILE3 )
		return NULL;

	return dynamic_cast< ioAdhesiveMissileWeapon3* >( pWeapon );
}

