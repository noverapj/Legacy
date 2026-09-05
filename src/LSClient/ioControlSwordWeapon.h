#pragma once

#include "ioWeapon.h"
#include "ioUserKeyInput.h"

class ioEffect;

class ioControlSwordWeapon : public ioWeapon
{
public:
	enum ControlState
	{
		CS_PRE,
		CS_FLOATING,
		CS_END,		
	};

	enum StateSync
	{
		SSC_CHANGE_MOVE,
	};

protected:
	ControlState m_State;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;

	ioEffect *m_pMesh;
	ioHashString m_szBoomEffect;
	ioHashString m_szMapCollisionEffect;

	D3DXVECTOR3 m_vStartPos;
	float m_fControlOutRange;

	CEncrypt<float> m_fAccelSpeed;

	CEncrypt<float> m_fMinSpeed;
	CEncrypt<float> m_fMoveSpeed;	
	CEncrypt<float> m_fCurMoveSpeed;

	CEncrypt<DWORD> m_dwKeyInputTime;
	CEncrypt<DWORD> m_dwAccelKeepTime;
	CEncrypt<DWORD> m_dwAccelUpStartTime;
	CEncrypt<DWORD> m_dwAccelDownStartTime;
	CEncrypt<DWORD> m_dwAccelEndTime;
	CEncrypt<DWORD> m_dwAccelUpTime;	
	CEncrypt<DWORD> m_dwKeyInputGapTime;
	CEncrypt<DWORD> m_dwCurKeyInputGapTime;	

	CEncrypt<DWORD> m_dwMapCollisionDelayTime;
	CEncrypt<DWORD> m_dwCurMapCollisionDelayTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bTargetRot;
	CEncrypt<DWORD> m_dwRotateSpeed;

	bool m_bSetUpKey;
	bool m_bSetDownKey;
	float m_fUpDownSpeed;

	CEncrypt<bool> m_bDisableUpDown;

	CEncrypt<bool> m_bPlayWoundSound;
	CEncrypt<bool> m_bDisableMapCollision;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	bool IsFloating() const;

protected:
	bool SetSwordMesh( const ioHashString &szFileName );
	void OnFloating( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage );

	float GetMoveSpeed();

	void CheckMoveKey( float fTimePerSec, DWORD dwCurTime );
	void CheckMoveDir();

	void SetMoveChange( ioUserKeyInput::DirKeyInput pCurDirKey );
	void SetMoveUpDownChange( bool isJumpKey, bool isDefenceKey );

	void CreateMapCollisionEffect( ioPlayStage* pStage );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void PlayWoundedSound( ioPlayEntity *pEntity );

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

	virtual void SetWeaponDead();

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioControlSwordWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioControlSwordWeapon();
};

inline ioControlSwordWeapon* ToControlSwordWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CONTROL_SWORD )
		return NULL;

	return dynamic_cast< ioControlSwordWeapon* >( pWeapon );
}


