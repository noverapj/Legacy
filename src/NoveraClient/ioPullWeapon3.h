#pragma once

#include "ioWeapon.h"
#include "ioUserKeyInput.h"

class ioEffect;

class ioPullWeapon3 : public ioWeapon
{
public:
	enum ControlState
	{
		CS_PRE,
		CS_MOVE,
		CS_PULLING,
		CS_END,
	};

protected:
	ControlState m_State;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vStartPos;

	ioEffect *m_pMesh;
	ioHashString m_szMapCollisionEffect;

	CEncrypt<DWORD> m_dwMapCollisionDelayTime;
	CEncrypt<DWORD> m_dwCurMapCollisionDelayTime;

	CEncrypt<float> m_fCurMoveRange;

	ioHashStringVec m_vTargetList;
	ioHashStringVec m_szTargetWoundBuff;

	WeaponInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

	CEncrypt<float> m_fExtraOffsetX;
	CEncrypt<float> m_fExtraOffsetY;
	CEncrypt<float> m_fExtraOffsetZ;

	float m_fAngleH;
	float m_fAngleV;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetMesh( const ioHashString &szFileName );	
	void CreateMapCollisionEffect( ioPlayStage* pStage, D3DXVECTOR3 vColPoint );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void LoadCallAttributeList( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual D3DXVECTOR3 GetMoveDir();

public:
	bool IsEnableTargetState( ioBaseChar *pOwner, ioBaseChar *pTarget );
	void SetTargetBuff( ioPlayStage *pStage );

	void CallAttribute();	

public:
	float GetMoveSpeed();
	float GetMoveRange();

	void RotateMoveDir( float fAngleH, float fAngleV );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioPullWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioPullWeapon3();
};

inline ioPullWeapon3* ToPullWeapon3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_PULL_WEAPON3 )
		return NULL;

	return dynamic_cast< ioPullWeapon3* >( pWeapon );
}

