#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;

class ioFlamewaveWeapon : public ioWeapon
{
public:
	enum FlameMoveType
	{
		FMT_NORMAL,
		FMT_OWNER_RL,
		FMT_CUSTOM_RANGE,//ini를 이용하지 않고 사용자가 외부에서 이동거리를 셋팅하면서 height값을 처음 셋팅값으로 한다.
	};

protected:
	ioEffect *m_pFireBall;

	ioHashString	m_szMoveDelayEffect;
	DWORD			m_dwMoveDelayEffectID;

	D3DXVECTOR3 m_vFirstMoveDir;
	D3DXVECTOR3 m_vMoveDir;

	float	m_fCurMoveRange;

	bool m_bSetGraceClose;

	bool m_bFirst;
	bool m_bLink;
	bool m_bYRot;

	DWORD m_dwEndEffectID;

	//
	FlameMoveType m_FlameMoveType;
	DWORD m_dwRotateTime;

	D3DXVECTOR3 m_vStartOffSet;

	bool m_bUseCustomRange;
	float m_fCustomMaxRange;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngle );
	void SetCustomRange(bool b, float fLen = -FLOAT1);

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void SetGraceClose();

	void CheckOwnerRLMoveDir(  float fTimePerSec );
	void ProcessMove( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	ioFlamewaveWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioFlamewaveWeapon();
};

inline ioFlamewaveWeapon* ToFlamewaveWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_FLAME_WAVE )
		return NULL;
	return dynamic_cast< ioFlamewaveWeapon* >( pWeapon );
}

