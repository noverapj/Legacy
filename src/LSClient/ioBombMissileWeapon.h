#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioBombMissileWeapon : public ioWeapon
{
protected:
	enum BombState
	{
		BS_FLOATING,
		BS_EXPLOSION,
		BS_EXPLOSION_END,
	};

	//추가 2014.05.28
	//movedir이 수직 방향이 아닌 대각선 방향일 경우 weapon이 찌그러지는 현상이 발생
	//이 현상을 막기위해 type 설정.
	enum Orientation_State
	{
		OS_NONE,
		OS_ORIENTATION,
	};

	enum ExplosionType
	{
		ET_RANGE,
		ET_AXIS,
	};
	
	BombState m_BombState;
	//추가 2014.05.28
	Orientation_State m_OrientationState;
	ExplosionType	  m_ExplosionType;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pBombMesh;
	ioHashString m_BoomEffect;
	float m_fExplosionRange;

	D3DXVECTOR3 m_vExplosionAxis;

	D3DXVECTOR3 m_vStartOffSet;


	bool m_bFirst;
	bool m_bEndExplosion;
	bool m_bCallExplosion;
	bool m_bDisableMapCollision;
	bool m_bEnableMapCollision;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	ChangeDirInfoList m_ChangeDirInfoList;

	//
	OwnerRotType m_OwnerRotate;
	DWORD m_dwRotateTime;
	float m_fRotateEndRange;

	//
	float m_fStartSpeed;
	float m_fMaxSpeed;
	float m_fAccelSpeed;

	//Call Attribute
	WeaponInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

	CEncrypt<bool> m_bPiercingStruct;

	DWORD m_dwAreaWeaponIndex;

protected:
	typedef struct tagCreateInfo
	{
		ioHashString m_szCreateName;
		D3DXVECTOR3 m_vPos;
		float m_fAngle;
	}CreateInfo;

	typedef std::vector<CreateInfo> CreateInfoList;
	//Call Dummy
	CreateInfoList m_DummyCharInfoList;

protected:
	ioHashString	m_PointEffect;
	UniqueObjID		m_PointEffectID;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetBombMesh( const ioHashString &szFileName );
	void OnFloating( float fTimePerSec, ioPlayStage *pStage );

	void CheckOwnerRotateDir( float fTimePerSec );
	float CheckCurSpeed( float fTimePerSec );
public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

	bool IsFloating() const;
	bool IsExplosion() const;

	void Explosion( ioPlayStage *pStage );
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();

	void SetDestroyAreaWeaponIndex( DWORD dwIndex );
protected:
	void LoadDummyCharProperty( ioINILoader &rkLoader );
	void CreateDummyCharbyExplosion( ioBaseChar *pOwner );
public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCanMineExplosion();
	virtual void SetWeaponDead();

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

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void CreateMapEffect( ioBaseChar *pOwner, ioPlayStage* pStage );

public:
	ioBombMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioBombMissileWeapon();
};

inline ioBombMissileWeapon* ToBombMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_BOMB_MISSILE )
		return NULL;

	return dynamic_cast< ioBombMissileWeapon* >( pWeapon );
}

