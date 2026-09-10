#pragma once

#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;
class ioUroborusGuideArrow;

class ioUroborusWeapon : public ioWeapon
{
public:
	enum UroborusState
	{
		US_MOVE,
		US_WAIT,
		US_END,
	};

protected:
	UroborusState m_UroborusState;

	ioEffect *m_pFireMesh;
	ioUroborusGuideArrow *m_pGuideArrow;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vPrePos;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	bool m_bFirst;

	CEncrypt<float> m_fCurMoveSpeed;
	CEncrypt<float> m_fCurMoveRange;

	ioHashString m_WaitEffect;

	CEncrypt<DWORD> m_dwUroborusWaitTime;
	DWORD m_dwUroborusWaitEndTime;

	CEncrypt<DWORD> m_dwUroborusJumpTime;
	bool m_bPiercingMove;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );
	
	void OnFloating( float fTimePerSec, ioPlayStage *pStage );
	void OnWaiting( float fTimePerSec, ioPlayStage *pStage );

	float GetMoveSpeed();
	float GetMaxRange();

	bool CheckOwnerState();
	bool CheckOwnerUroborusState();

	void ProcessGuidArrow();

public:
	void SetWaitState( bool bSendNet );
	void SetEndState( ioPlayStage *pStage, bool bSendNet );

	bool IsCanUroborusJump();
	bool IsUroborusMove();
	bool IsUroborusWait();

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );
	virtual void SetWeaponIndex( DWORD dwIndex );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual DWORD GetCollisionEnableTime() const;

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void SetOwnerChargeScaleRate( float fRate );
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioUroborusWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioUroborusWeapon();
};

inline ioUroborusWeapon* ToUroborusWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_UROBORUS )
		return NULL;

	return dynamic_cast< ioUroborusWeapon* >( pWeapon );
}

