#pragma once

#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;
class ioUroborusGuideArrow;

class ioGuide2Weapon : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_MOVE,
		WS_TRACING,
		WS_AFTER_COLLISION,
		WS_END,
	};

protected:
	WeaponState m_WeaponState;

	ioEffect *m_pFireMesh;
	ioUroborusGuideArrow *m_pGuideArrow;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vPrePos;

	// Tracing..
	ioHashString m_TargetName;
	CEncrypt<float> m_fTargetTracePosRate;
	CEncrypt<float> m_fDownTargetTracePosRate;
	DWORD m_dwTraceEnableTime;
	CEncrypt<float> m_fTraceSpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fTraceRotSpeed;
	CEncrypt<float> m_fTraceEndRange;
	CEncrypt<DWORD> m_dwTraceDuration;
	DWORD m_dwTraceEndTime;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	bool m_bFirst;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	CEncrypt<bool>	m_bAttachTarget;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

	void OnFloating( float fTimePerSec, ioPlayStage *pStage );
	void OnTracing( float fTimePerSec, ioPlayStage *pStage );
	void OnAfterCollision( float fTimePerSec, ioPlayStage *pStage );

	float GetMoveSpeed();
	bool CheckOwnerState();
	bool CheckDownTargetState( ioBaseChar *pTarget );

	void RotateMoveDir( float fAngleH, float fAngleV );

	void ProcessGuidArrow();

public:
	void SetTraceState( ioPlayStage *pStage );
	void SetAfterCollisionState( bool bSendNet );
	void SetEndState( bool bSendNet );

	void CheckAfterCollisionState( ioBaseChar *pChar, bool bApplyCol );

	void SetTraceTarget( ioHashString &szName );

	float GetMaxRange();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void SetWeaponDead();

	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );
	virtual void SetWeaponIndex( DWORD dwIndex );

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
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

protected:
	void SetSkillInfo( ioBaseChar *pOwner );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioGuide2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioGuide2Weapon();
};

inline ioGuide2Weapon* ToGuide2Weapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_GUIDE2 )
		return NULL;

	return dynamic_cast< ioGuide2Weapon* >( pWeapon );
}

