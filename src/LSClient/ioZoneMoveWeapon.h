#pragma once
#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioZoneMoveWeapon : public ioWeapon
{
protected:

	enum WeaponState
	{
		WS_INIT,
		WS_MOVE,
		WS_RETURN,
		WS_END,
	};

	enum WeaponRetrunType
	{
		WRT_NONE,
		WRT_ROTATE,
	};

	WeaponState m_State;

	TargetColType	m_TargetColType;
	TargetWoundType	m_TargetWoundType;
	CEncrypt<bool>	m_bDisableColGrap;

	CEncrypt<float> m_fZoneRange;
	CEncrypt<float> m_fZoneMinRange;
	CEncrypt<float> m_fZoneMaxRange;
	CEncrypt<float>	m_fZoneInnerRange;
	CEncrypt<float>	m_fZoneAngle;
	float			m_fCurRange;

	CEncrypt<float>	m_fUpHeight;
	CEncrypt<float>	m_fUnderHeight;
	CEncrypt<float>	m_fWidth;
	CEncrypt<float>	m_fMaxWidth;
	float			m_fCurWidth;

	D3DXVECTOR3 m_vStartOffSet;
	D3DXVECTOR3 m_vCollisionDir;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vPosition;

	ioOrientBox m_LocalAttackBox;

	float m_fGoMaxRange;
	float m_fBackMaxRange;
	float m_fCurMoveRange;

	float m_fStartSpeed;
	float m_fMaxSpeed;
	float m_fAccelSpeed;
	float m_fCurveRange;

	DWORD m_dwCharCallTime;
	DWORD m_dwCurCharCallTime;
	
	bool m_bCharWallCol;
	ioHashString m_szWallEffect;
	ioHashString m_szWallCollEffect;

	UniqueObjID m_WallEffectID;
	UniqueObjID m_WallCollEfffectID;

	WeaponRetrunType m_eWeaponRetrunType;
	float		m_fRotateY;
	float		m_fRotateX;

	ioHashString m_szWeaponEffect;
	DWORD        m_dwWeaponEffectID;

	bool m_bRotationBox;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void CreateWall( float fTimePerSec, ioPlayStage *pStage );
	void CreateWallColl( float fTimePerSec, ioPlayStage *pStage );

protected:
	bool CheckCollisionBox( ioPlayEntity *pEntity );
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:	
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	void ProcessMove( float fTimePerSec, ioPlayStage *pStage );

public:

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
public:
	virtual void SetWeaponDead();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );	
	virtual D3DXVECTOR3 GetMoveDir();

public:
	float CheckCurSpeed( float fTimePerSec );

public:
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	D3DXVECTOR3 GetAttchPosition();

public:
	void ClearData();

public:
	ioZoneMoveWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioZoneMoveWeapon();
};

inline ioZoneMoveWeapon* ToZoneMoveWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ZONE_MOVE_WEAPON )
		return NULL;

	return dynamic_cast< ioZoneMoveWeapon* >( pWeapon );
}