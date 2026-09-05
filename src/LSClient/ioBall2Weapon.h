#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioBall2Weapon : public ioWeapon
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
		SS_END,
		SS_CULLING,
	};
	struct TargetInfo
	{
		ioHashString	m_TargetName;
		D3DXVECTOR3		m_vDir;
		float			m_fLength;
		DWORD			m_AttachTime;
	};
	typedef std::vector<TargetInfo>	TargetInfoList;

protected:
	ioEffect *m_pFireMesh;

	BallState m_BallState;

	D3DXVECTOR3 m_vPrePos;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vAttackDir;
	D3DXVECTOR3 m_vRollAxis;
	ioOrientBox m_LocalAttackBox;

	bool m_bCheckCollision;
	int m_iCollisionCnt;

	// Å¸°Ù°ü·Ã
	TargetInfoList	m_TargetInfoList;
	CEncrypt<DWORD>	m_dwDropZoneLiveTime;
	DWORD			m_dwDropZoneCheckStartTime;
	CEncrypt<float>	m_dwRotateSpeed;
	ioHashStringVec	m_TargetEndBuff;
	ioHashStringVec	m_TargetCullingBuff;

	CEncrypt<bool> m_bRotateXAxis;
	CEncrypt<float> m_fOffsetByRotateX;
	CEncrypt<float> m_fRotateAngle;

	bool			m_bOnlyAttack;
	bool			m_bOffsetXDisable;
	bool			m_bMustChangeDir;

	//
	int				m_iTargetMaxCount;
	DWORD			m_dwTargetCullingTime;

protected:
	ChangeDirInfoList m_ChangeDirInfoList;
	
	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	bool m_bCheckPacket;

	D3DXQUATERNION m_qtOriginal;

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
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

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

protected:
	void SetEndWaitState();
	void SetEndState( ioPlayStage *pStage );
	void CullingTarget( ioPlayStage *pStage );
	void RemoveTarget( ioPlayStage *pStage, const ioHashString& szTarget );

	void CheckTarget( ioPlayStage *pStage );
	void SendTargetPacket( ioPlayStage *pStage );

public:
	ioBall2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioBall2Weapon();
};

inline ioBall2Weapon* ToBall2Weapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_BALL2_WEAPON )
		return NULL;

	return dynamic_cast< ioBall2Weapon* >( pWeapon );
}

