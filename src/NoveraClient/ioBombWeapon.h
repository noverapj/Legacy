#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioBombWeapon : public ioWeapon
{
protected:
	ioEffect *m_pBombMesh;
	ioHashString m_BoomEffect;

	D3DXVECTOR3 m_vMoveDir;
	ioOrientBox m_LocalAttackBox;

	DWORD m_dwExplosionTime;
	float m_fExplosionRange;
	float m_fFloatingPower;
	
	float m_fMoveSpeed;

	bool m_bDestroyDummy;

protected:
	enum BombState
	{
		BS_FLOATING,
		BS_EXPLOSION_WAIT,
		BS_EXPLOSION
	};
	BombState m_BombState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	typedef struct tagCreateInfo
	{
		ioHashString m_szCreateName;
		D3DXVECTOR3 m_vPos;
		float m_fAngle;
	}CreateInfo;

	typedef std::vector<CreateInfo> CreateInfoList;

protected:
	CreateInfoList m_AreaWeaponInfoList;
	CreateInfoList m_DummyCharInfoList;

protected:
	bool SetBombMesh( const ioHashString &szFileName );

public:
	void RotateMoveDir( float fAngle );
	
	float GetMoveSpeed() const;

	bool IsFloating() const;
	bool IsExplosion() const;

	void Explosion( ioPlayStage *pStage );

	void SetExplosionWait();
	
protected:
	void OnFloating( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );

	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCanMineExplosion();

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void LoadAreaWeaponProperty( ioINILoader &rkLoader );
	void CreateAreaWeaponbyExplosion( ioBaseChar *pOwner );

protected:
	void LoadDummyCharProperty( ioINILoader &rkLoader );
	void CreateDummyCharbyExplosion( ioBaseChar *pOwner );

public:
	ioBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioBombWeapon();
};

inline ioBombWeapon* ToBombWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_BOMB )
		return NULL;

	return dynamic_cast< ioBombWeapon* >( pWeapon );
}

