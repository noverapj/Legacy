#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;

class ioControlReturnWeapon : public ioWeapon
{
public:
	enum ThrowReturnState
	{
		TRS_NONE,
		TRS_RETRUN,
	};

	enum ControlSycState
	{
		CSS_RETURN	= 1,
		CSS_DIE		= 2,
	};

	enum ControlMoveType
	{
		CMT_NO1	= 1,
		CMT_NO2	= 2,
	};

protected:
	ThrowReturnState m_ReturnState;
	ControlMoveType m_MoveType;

	ioEffect *m_pThrowMesh;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vCurMoveDir;
	D3DXVECTOR3 m_vFirstMoveDir;
	D3DXVECTOR3 m_vRollAxis;

	DWORD m_dwStartReturnTime;
	DWORD m_dwReturnDuration;

	float m_fCurMoveRange;
	float m_fCurMoveDistance;
	ioOrientBox m_LocalAttackBox;

	float m_fDefaultHeightGap;
	float m_fControlSpeed;
	float m_fReturnControlSpeed;
	D3DXVECTOR3 m_vCrossVec;

	float m_fControlValue;
	float m_fControlValue2;
	float m_fReturnHeightMoveValue;

	float m_fEndRange;

	D3DXVECTOR3 m_vPrePos;

	bool m_bRotateMoveDir;

	DWORD m_dwReturnHeightMoveTime;
	float m_fReturnHeightMoveUpSpeed;
	float m_fReturnHeightMoveDownSpeed;

protected:
	bool m_bIgnoreCharMove;
	float m_fStartAngle;
	float m_fTargetOffset;
	float m_fTargetOffset2;

	bool m_bWallCollision;

private:
	ChangeDirInfoList m_ChangeDirInfoList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();

	void SetReturnWay( ioPlayStage *pStage );

public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual void PlayExplosionSound();
	virtual void PlayWoundedSound( ioPlayEntity *pEntity );
	virtual void PlayDefensedSound( ioPlayEntity *pEntity );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

protected:
	bool SetThrowMesh( const ioHashString &szFileName );
	bool CheckMaxRange( float fTimePerSec );

	bool CheckOwnerControl1( float fTimePerSec );
	bool CheckOwnerControl2( float fTimePerSec );
	bool SetWallCollision( const D3DXVECTOR3 &vFaceNrm, ioPlayStage *pStage );

public:
	void SetDieState( ioPlayStage *pStage, bool bSendNet );
	
public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

private:
	void CheckChangeDir();

public:
	ioControlReturnWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioControlReturnWeapon();
};

inline ioControlReturnWeapon* ToControlReturnWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CONTROL_RETURN )
		return NULL;

	return dynamic_cast< ioControlReturnWeapon* >( pWeapon );
}