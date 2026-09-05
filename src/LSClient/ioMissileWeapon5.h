#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioMissileWeapon5 : public ioWeapon
{
	enum MissileMoveType
	{
		NONE_MOVE,
		START_MOVE,
		POSITION_MOVE,
	};

	struct stMovePositionInfo
	{
		D3DXVECTOR3 vPos;
		float		fRange;

		stMovePositionInfo()
		{
			fRange = 0.f;
			vPos = ioMath::VEC3_ZERO;
		}
	};

protected:
	ioEffect *m_pFireBall;

	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	D3DXVECTOR3 m_vStartOffSet;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	ChangeDirInfoList m_ChangeDirInfoList;

	bool	m_bSetGraceClose;
	bool	m_bFirst;
	bool m_bDisableOwnerScale;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

	WeaponInfo	m_CallWeaponInfo;
	ChangeDirInfoList m_CallAttFireList;
	bool m_bArleadyCallWeapon;

	WeaponInfo m_CallShadowWeapon;
	DWORD	   m_dwCallShadowWeaponDuration;
	DWORD      m_dwNextCallShadowWeaponTime;
	float	   m_fAddShadowWeaponHeight;

	//Vector3Vec		m_vWeaponPosList;
	std::vector< stMovePositionInfo > m_vMoveInfoList;
	MissileMoveType m_MoveState;
	float	m_fMoveSpeed;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

	void SetSecondAttribute( SecondAttributeList vSecondList );

	bool ChangeWeaponEffect( const ioHashString &szEffectName );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void SetGraceClose();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetWeaponDead();

	void AddMovePosition( const D3DXVECTOR3& vPos );

public:
	void LoadCallAttribute( ioINILoader &rkLoader );
	void LoadCallAttributeFireDirectionList( ioINILoader &rkLoader );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	//virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	//virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	bool isCallAttribute();
	void CallAttribute( ioPlayEntity *pEntity );

//protected:
	//bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	virtual bool IsIgnoreTestMapCollision() const;

public:
	ioMissileWeapon5( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioMissileWeapon5();
};

inline ioMissileWeapon5* ToMissileWeapon5( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_MISSILE5 )
		return NULL;

	return dynamic_cast< ioMissileWeapon5* >( pWeapon );
}

