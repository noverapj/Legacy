#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioLobeliaWeapon : public ioWeapon
{
protected:
	enum ThrowState
	{
		TS_NONE,
		TS_MOVE,
		TS_PINGPONG,
		TS_GROUND,
		TS_EXPLOSION,
	};

	ThrowState m_State;

protected:
	ioEffect *m_pFireMesh;

	ioHashString m_GroundEffect;
	ioHashString m_BoomEffect;

	UniqueObjID m_GroundID;
	UniqueObjID m_EffectID;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vRollAxis;

	ioOrientBox m_LocalAttackBox;

	DWORD m_dwDuration;
	DWORD m_dwThrowDuration;
	DWORD m_dwDropDuration;

	DWORD m_dwGroundStart;
	DWORD m_dwGroundDuration;

	DWORD m_dwExplosionStart;
	DWORD m_dwExplosionDuration;

	float m_fExplosionRange;
	float m_fJumpFloatingAngle;

	float m_fFloatingPower;
	float m_fDropFloatingPower;

	float m_fMoveSpeed;
	float m_fDropMoveSpeed;

	float m_fPingPongPower;
	float m_fExtentsOffSet;

protected:
	bool m_bObjectCollision;
	bool m_bDirectlyBoom;
	bool m_bDropState;

protected:
	TargetColType	m_TargetColType;
	TargetWoundType	m_TargetWoundType;

	CEncrypt<float> m_fZoneRange;
	CEncrypt<float> m_fZoneMinRange;
	CEncrypt<float> m_fZoneMaxRange;
	CEncrypt<float>	m_fZoneInnerRange;
	CEncrypt<float>	m_fZoneAngle;

	CEncrypt<float>	m_fUpHeight;
	CEncrypt<float>	m_fUnderHeight;
	CEncrypt<float>	m_fWidth;
	CEncrypt<float>	m_fOffSet;

	BuffInfoList m_LobeliaBuffList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:
	void RotateMoveDir( float fAngle );
	void SetBombDrop();
	void SetJumpThrow();

	void SetBoomEffect( const ioHashString &rkName );

	void SetGroundState( ioPlayStage *pStage );

	void Explosion( ioPlayStage *pStage );
	void SetWallCollision( const D3DXVECTOR3 &vFaceNrm );

	bool IsExplosion() const;
	bool IsGroundState() const;

	void TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	
public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

	virtual const BuffInfoList& GetTargetAirBuffList() const;
	virtual const BuffInfoList& GetTargetBuffList() const;
	virtual const BuffInfoList& GetTargetDownBuffList() const;

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;
	virtual bool IsCanMineExplosion();

protected:
	void OnNoneState( float fTimePerSec, ioPlayStage *pStage );
	void OnMoveState( float fTimePerSec, ioPlayStage *pStage );
	void OnPingPongState( float fTimePerSec, ioPlayStage *pStage );
	void OnGroundState( float fTimePerSec, ioPlayStage *pStage );
	void OnExplosionState( float fTimePerSec, ioPlayStage *pStage );

protected:
	bool CheckCollisionSphere( ioPlayEntity *pEntity );
	bool CheckCollisionCylinder( ioPlayEntity *pEntity );
	bool CheckCollisionBox( ioPlayEntity *pEntity );
	bool CheckCollisionDonut( ioPlayEntity *pEntity );
	bool CheckCollisionCone( ioPlayEntity *pEntity );

public:
	ioLobeliaWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioLobeliaWeapon();
};

inline ioLobeliaWeapon* ToLobeliaWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_LOBELIA )
		return NULL;

	return dynamic_cast< ioLobeliaWeapon* >( pWeapon );
}

