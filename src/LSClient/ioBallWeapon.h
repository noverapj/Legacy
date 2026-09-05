#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioBallWeapon : public ioWeapon
{
protected:
	enum BombState
	{
		BS_FLOATING,
		BS_EXPLOSION,
		BS_EXPLOSION_END,
	};
	enum ExplosionType
	{
		ET_RANGE,
		ET_AXIS,
	};

	ioEffect *m_pFireMesh;

	D3DXVECTOR3 m_vPrePos;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vAttackDir;
	D3DXVECTOR3 m_vRollAxis;
	ioOrientBox m_LocalAttackBox;

	bool m_bCheckCollision;
	int m_iCollisionCnt;

	bool m_bEndExplosion;
	bool m_bCallExplosion;
	float m_fExplosionRange;

	ioHashString m_BoomEffect;
	D3DXVECTOR3 m_vExplosionAxis;
	ExplosionType	  m_ExplosionType;
	BombState m_BombState;

protected:
	typedef struct tagCreateInfo
	{
		ioHashString m_szCreateName;
		D3DXVECTOR3 m_vPos;
		float m_fAngle;
	}CreateInfo;

	typedef std::vector<CreateInfo> CreateInfoList;
	CreateInfoList m_DummyCharInfoList;

	WeaponInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

protected:
	ChangeDirInfoList m_ChangeDirInfoList;
	
	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:
	bool SetWallCollision( const D3DXVECTOR3 &vFaceNrm, bool bMap, bool bOnlyAttack );
	D3DXVECTOR3 GetMoveDir();
	void ChangeBallWeapon( int iCurCnt, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vMoveDir, const D3DXVECTOR3 &vAttackDir );
	
	inline int GetCollisionCnt() const { return m_iCollisionCnt; }

	void Explosion( ioPlayStage *pStage );
	bool IsExplosion() const;
	bool IsEndExplosion()	{ return m_bEndExplosion; }
	virtual bool IsCanMineExplosion();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual DWORD GetCollisionEnableTime() const;

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	void OnMoveState( float fTimePerSec );
	void OnDownState( float fTimePerSec, ioPlayStage *pStage );
	void OnPingPongState( float fTimePerSec, ioPlayStage *pStage );

	void RollWeapon( float fTimePerSec );

protected:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );

	float GetMoveSpeed();
	float GetMaxRange();

	void OnFloating( float fTimePerSec, ioPlayStage *pStage );
	void CreateDummyCharbyExplosion( ioBaseChar *pOwner );
	void LoadDummyCharProperty( ioINILoader &rkLoader );
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();

public:
	ioBallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioBallWeapon();
};

inline ioBallWeapon* ToBallWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_BALL_WEAPON )
		return NULL;

	return dynamic_cast< ioBallWeapon* >( pWeapon );
}

