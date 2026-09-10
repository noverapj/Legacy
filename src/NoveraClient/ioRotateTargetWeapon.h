#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioRotateTargetWeapon : public ioWeapon
{
protected:
	enum BallState
	{
		BS_MOVE,
		BS_END_WAIT,
		BS_END,
	};

	enum SyncState
	{
		SS_ADD_TARGET,
		SS_EXCEPTION,
		SS_END,
	};
	struct TargetInfo
	{
		ioHashString	m_TargetName;
		D3DXVECTOR3		m_vDir;
	};
	typedef std::vector<TargetInfo>	TargetInfoList;

protected:
	BallState m_BallState;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vAttackDir;
	ioOrientBox m_LocalAttackBox;

	// Å¸°Ù°ü·Ã
	TargetInfoList	m_TargetInfoList;
	CEncrypt<DWORD>	m_dwDropZoneLiveTime;
	CEncrypt<float>	m_dwRotateSpeed;
	ioHashStringVec	m_TargetEndBuff;
	CEncrypt<float> m_fTargetOffset;
	CEncrypt<float> m_fTargetHeightOffset;
	CEncrypt<float> m_fCollisionRange;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	void SetEndState( ioPlayStage *pStage );
	void CheckTarget( ioPlayStage *pStage );
	bool CheckCollision( ioPlayEntity *pEntity );
	void SendExceptionTarget( ioBaseChar *pTarget );

public:
	ioRotateTargetWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioRotateTargetWeapon();
};

inline ioRotateTargetWeapon* ToRotateTargetWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ROTATE_TARGET_WEAPON )
		return NULL;

	return dynamic_cast< ioRotateTargetWeapon* >( pWeapon );
}

