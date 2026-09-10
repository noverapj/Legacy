#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;
class ioPlayStage;

class ioTsunamiWeapon : public ioWeapon
{
public:
	enum FlameState
	{
		FS_FALL,
		FS_EXPLOSION
	};

	enum TsunamiType
	{
		TT_NORMAL,
		TT_DIRECT,
	};

	enum TsunamiMoveType
	{
		TMT_NORMAL	= 1,
		TMT_REVERSE	= 2,
	};

protected:
	TsunamiType m_TsunamiType;
	TsunamiMoveType m_TsunamiMoveType;

	FlameState m_FlameState;

	ioEffect *m_pMeteor;

	ioHashString m_BoomEffect;
	ioHashString m_TsunamiEffect;
	ioHashString m_TsunamiEndEffect;

	ioOrientBox m_LocalAttackBox;

	CEncrypt<float> m_fExplosionRange;
	CEncrypt<float> m_fExplosionInRange;
	CEncrypt<float> m_fTsunamiSpeed;
	CEncrypt<float> m_fTsunamiAngle;

	CEncrypt<float> m_fCurExpRange;
	CEncrypt<float> m_fCurExpInRange;

	DWORD m_dwExplosionStartTime;

	CEncrypt<float> m_fStartHeight;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;

	UniqueObjID m_dwCurMapEffect;

	D3DXVECTOR3 m_StartPos;
	D3DXQUATERNION m_qtStartRot;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	float GetMoveSpeed();

public:
	bool IsFalling() const;
	void Explosion( ioPlayStage *pStage );

protected:
	bool CreateMeteor( const ioHashString &szFileName );

	void OnFalling( float fTimePerSec, ioPlayStage *pStage );
	void OnExplosion( float fTimePerSec, ioPlayStage *pStage );

	void CheckTestCollisionBox();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioTsunamiWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioTsunamiWeapon();
};

inline ioTsunamiWeapon* ToTsunamiWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_TSUNAMI )
		return NULL;

	return dynamic_cast< ioTsunamiWeapon* >( pWeapon );
}

