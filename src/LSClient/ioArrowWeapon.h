#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioArrowWeapon : public ioWeapon
{
public:
	enum WeaponItemType{ 
		WIT_NONE,
		WIT_MUTANT,
	};

protected:
	ioEffect *m_pFireBall;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	bool	m_bSetGraceClose;
	bool	m_bFirst;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

	CEncrypt<bool>	m_bWaitWeaponDeadForPacket;
	DWORD			m_dwWeaponDeadTime;

	DWORD m_dwAreaWeaponIndex;
	WeaponItemType   m_nWeaponType;

	//추가 2014.07.08
	//movedir이 수직 방향이 아닌 대각선 방향일 경우 weapon이 찌그러지는 현상이 발생
	//이 현상을 막기위해 type 설정.
	enum Orientation_State
	{
		OS_NONE,
		OS_ORIENTATION,
	};

	//추가 2014.07.08
	Orientation_State m_OrientationState;

	ChangeDirInfoList	m_ChangeDirInfoList;
	float				m_fCurMoveSpeed;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	void RotateMoveDir( float fAngle );
	void RotateMoveDir( float fAngleH, float fAngleV );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void DestroyFireBall();

	void SetGraseClose();

	void CheckChangeDir();
public:
	void SetSecondAttribute( SecondAttributeList vSecondList );

	float GetMoveSpeed();
	float GetMaxRange();

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );
	virtual void SetWeaponDead();

	void SetDestroyAreaWeaponIndex( DWORD dwIndex );

	WeaponItemType GetWeaponItemType();

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
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );


public:
	ioArrowWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioArrowWeapon();
};

inline ioArrowWeapon* ToArrowWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ARROW )
		return NULL;

	return dynamic_cast< ioArrowWeapon* >( pWeapon );
}

