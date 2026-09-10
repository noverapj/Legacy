#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioRotateAttachCharWeapon : public ioWeapon
{
protected:
	enum BallState
	{
		BS_MOVE,
		BS_END,
	};

	enum SyncState
	{
		SS_ADD_TARGET,
	};

	struct TargetInfo
	{
		ioHashString	m_TargetName;
		D3DXVECTOR3		m_vDir;
		float			m_fLength;
	};

	typedef std::vector<TargetInfo>	TargetInfoList;

protected:
	// Å¸°Ù°ü·Ã
	TargetInfoList	m_TargetInfoList;

	ioEffect *m_pFireMesh;
	BallState m_BallState;

	D3DXVECTOR3 m_vPrePos;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vAttackDir;
	D3DXVECTOR3 m_vRollAxis;
	ioOrientBox m_LocalAttackBox;
	
	D3DXQUATERNION m_qtOriginal;

	bool m_bAttachChar;

	DWORD m_dwCheckFireTime;
	ioHashStringVec	m_TargetEndBuff;
	ioHashStringVec m_TargetEndRemoveBuff;

protected:
	CEncrypt<DWORD>	m_dwDropZoneLiveTime;
	DWORD			m_dwDropZoneCheckStartTime;
	CEncrypt<float>	m_dwRotateSpeed;

	CEncrypt<bool> m_bRotateXAxis;
	CEncrypt<float> m_fOffsetByRotateX;
	CEncrypt<float> m_fRotateAngle;

	D3DXVECTOR3 m_vAttachOffset;

	DWORD m_dwFireLiveTime;

	bool m_bCollisionMap;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;
	
public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	virtual void SetLiveTime( DWORD dwTime );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

public:
	D3DXVECTOR3 GetMoveDir();
	void SetAttachChar( bool bAttachChar );

protected:
	bool SetFireMesh( const ioHashString &szFileName );

protected:
	bool IsDropZone( ioPlayStage *pStage );
	void CheckMove( float fTimePerSec );
	float GetMoveSpeed();
	void CheckTarget( ioPlayStage *pStage );

protected:
	void SetEndState( ioPlayStage *pStage );

public:
	ioRotateAttachCharWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioRotateAttachCharWeapon();
};

inline ioRotateAttachCharWeapon* ToRotateAttachCharWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ROTATE_ATTACH_CHAR )
		return NULL;

	return dynamic_cast< ioRotateAttachCharWeapon* >( pWeapon );
}

