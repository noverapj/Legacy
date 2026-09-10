#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioAdhesiveMissileWeapon4 : public ioWeapon
{
public:
	
	enum WeaponState
	{
		WS_INIT,
		WS_ATTACH_MOVE,
		WS_CHOP,
	};

	enum StateControlSync
	{
		STC_CHAR_COLLISION,
		STC_TEST_MAP_COLLISION,
	};

	struct TargetInfo
	{
		ioHashString m_szTargetName;
		float m_fLength;
		float m_fHeightGap;
	};
	typedef std::vector<TargetInfo> TargetInfoList;

protected:
	WeaponState m_State;
	
	TargetInfoList m_TargetInfoList;

	ioEffect *m_pFireBall;
	ioOrientBox	m_CollisionBox;

	ioHashString m_BoomEffect;		
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vPreAttackDir;
	bool m_bSetGraceClose;

	bool m_bMapCollision;
	
	ioHashStringVec m_szTargetMapCollisionCancelBuff;
	ioHashStringVec m_szOwnerWoundedBuff;

	CEncrypt<float> m_fLimitAngle;

	bool m_bUseCol;
	bool m_bCollisionMap;
	bool m_bCollisionMapForSkill;

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
	void ProcessChop( float fTimePerSec, ioPlayStage *pStage  );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	void SetUseColCheck( bool bCheck );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	void SetTargetMapCollisionCancelBuff();
	void SetOwnerWoundedBuff();

	void SetChopFirstDirection();
	void SetState( WeaponState State );
	void Explosion( ioPlayStage* pStage );

public:
	void SebCollisionMapForSkill( bool bColl ){ m_bCollisionMapForSkill = bColl; }

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool IsProtectState( ioBaseChar* pChar );

public:
	ioAdhesiveMissileWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAdhesiveMissileWeapon4();
};

inline ioAdhesiveMissileWeapon4* ToAdhesiveMissileWeapon4( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ADHESIVE_MISSILE4 )
		return NULL;

	return dynamic_cast< ioAdhesiveMissileWeapon4* >( pWeapon );
}

