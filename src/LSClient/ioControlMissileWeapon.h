#pragma once

#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;

class ioControlMissileWeapon : public ioWeapon
{
public:
	enum ControlState
	{
		CTS_IN_RANGE,
		CTS_OUT_RANGE,
		CTS_RETURN,
	};

	enum StateControlSync
	{
		STC_SYNC	= 1,
		STC_IN		= 2,
		STC_OUT		= 3,
		STC_DIE		= 4,
		STC_RETURN	= 5,
	};

protected:
	ControlState m_ControlState;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pMissileMesh;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	D3DXVECTOR3 m_vPrePos;
	float m_fDefaultGapHeight;
	float m_fEnableMoveGap;

	float m_fCurMoveSpeed;

	DWORD m_dwRotateTime;
	DWORD m_dwRotateStartTime;
	DWORD m_dwRotateEndTime;

	DWORD m_dwSyncTime;
	bool m_bSetGraceClose;

	DWORD m_dwReturnStartTime;
	DWORD m_dwReturnDuration;
	float m_fReturnEndRange;

	bool m_bEnableCollisionReturn;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;


protected:
	bool SetMesh( const ioHashString &szFileName );

	float GetMoveSpeed();
	float GetMaxRange();

	void CheckOwnerRotateDir( float fTimePerSec );
	void CheckOwnerReturnDir( float fTimePerSec );

	bool CheckInRange();
	void ChangeToInRange();
	void ChangeToOutRange();
	void ChangeToReturnState();

	void OnInRangeState( float fTimePerSec, ioPlayStage *pStage );
	void OnOutRangeState( float fTimePerSec, ioPlayStage *pStage );
	void OnReturnState( float fTimePerSec, ioPlayStage *pStage );

	void SetGraceClose();

	DWORD GetCurRotateEndTime();

	bool CheckReturnEndRange();
	bool CheckReturnEndTime();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetLiveTime( DWORD dwTime );
	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );
	virtual void SetWeaponIndex( DWORD dwIndex );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioControlMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioControlMissileWeapon();
};

inline ioControlMissileWeapon* ToControlMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CONTROL_MISSILE )
		return NULL;

	return dynamic_cast< ioControlMissileWeapon* >( pWeapon );
}

