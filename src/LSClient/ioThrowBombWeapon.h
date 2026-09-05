#pragma once

#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioThrowBombWeapon : public ioWeapon
{
protected:
	enum ThrowState
	{
		TS_NONE,
		TS_MOVE,
		TS_PINGPONG,
		TS_EXPLOSION,
	};

	ThrowState m_State;

protected:
	ioEffect *m_pFireMesh;
	ioHashString m_BoomEffect;
	ioHashString m_ExplosionDelayEffect;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vOrigMoveDir;
	D3DXVECTOR3 m_vPreDir;
	D3DXVECTOR3 m_vRollAxis;

	ioOrientBox m_LocalAttackBox;

	DWORD m_dwDuration;
	DWORD m_dwThrowDuration;
	DWORD m_dwDropDuration;

	DWORD m_dwExplosionStart;
	DWORD m_dwExplosionDuration;

	DWORD m_dwAutoReserveExplosiontTime;
	DWORD m_dwResreveExplosionTime;
	DWORD m_dwResreveExplosionStartTime;
	bool m_bReserveExplosion;

	float m_fDirectlyCharExplosionRange;
	float m_fExplosionRange;
	float m_fJumpFloatingAngle;

	float m_fFloatingPower;
	float m_fDropFloatingPower;

	float m_fMoveSpeed;
	float m_fDropMoveSpeed;

	float m_fExtentsOffSet;
	float m_fPingPongPower;
	float m_fCurPingPongPower;
	int m_iMaxPingPongCnt;
	int m_iCurPingPongCnt;

	float m_fDefaultAngle;

	CEncrypt<bool>	m_bDisableRollOnMove;
	CEncrypt<bool>	m_bUseMeshLink;

	CEncrypt<bool> m_bVisbleSameTeam;
	
	ioHashString m_szReserveSound;

	ioHashString m_szWeaponEffect_Blue;
	ioHashString m_szWeaponEffect_Red;

	//¥ìa¢¬¡¾¢Ò¡× A©¬¡Æ¢®¥ìE(by A¡¾AAA¢Ò)
	float m_fOffSet_Look;
	float m_fOffSet_Y;
	float m_fOffSet_X;

protected:
	bool m_bObjectCollision;
	CEncrypt<bool> m_bDirectlyBoom;
	CEncrypt<int>  m_iDirectlyBoomToChar;
	bool m_bDropState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	ioHashString m_szTargetName;

protected:
	//Call Attribute
	WeaponFireInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

public:
	void RotateMoveDir( float fAngle );
	void SetBombDrop();
	void SetJumpThrow();

	void SetBoomEffect( const ioHashString &rkName );
	void SetWallCollision( const D3DXVECTOR3 &vFaceNrm );

	void SetExplosionState( ioPlayStage *pStage );
	void SetReserveExplosion( ioPlayStage *pStage );
	bool IsExplosion() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	
public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetFloatPower( float fFloatPower );
	
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual DWORD GetCollisionEnableTime() const;

public:
	bool IsOneTargetCollision( ioPlayEntity *pEntity, bool bMapCollision );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;
	virtual bool IsCanMineExplosion();

protected:
	void SetCurMoveDir( const D3DXVECTOR3 &vDir );

	void OnNoneState( float fTimePerSec, ioPlayStage *pStage );
	void OnMoveState( float fTimePerSec, ioPlayStage *pStage );
	void OnTargetState( float fTimePerSec, ioPlayStage *pStage );
	void OnPingPongState( float fTimePerSec, ioPlayStage *pStage );
	void OnExplosionState( float fTimePerSec, ioPlayStage *pStage );

protected:
	void CheckReserveExplosion( ioPlayStage *pStage );
	void VisibleCheck( ioPlayStage *pStage );

protected:
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();
	D3DXVECTOR3 CalcRotateMoveDir( float fAngle );

public:
	ioThrowBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioThrowBombWeapon();
};

inline ioThrowBombWeapon* ToThrowBombWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_THROWBOMB )
		return NULL;

	return dynamic_cast< ioThrowBombWeapon* >( pWeapon );
}


