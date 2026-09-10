#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioLimitAttachWeapon2 : public ioWeapon
{
protected:
	ioHashString	m_szReduceColByBuffName;
	ioHashString	m_szSkillSoundName;
	UniqueObjID m_SkillSoundID;

	ioHashString	m_szWeaponEffectStand;
	ioHashString	m_szWeaponEffectMove;
	ioHashString	m_szWeaponEffectJump;
	ioHashString	m_szWeaponEffectEnd;

	DWORD	m_dwWeaponEffectStand;
	DWORD	m_dwWeaponEffectMove;
	DWORD	m_dwWeaponEffectJump;
	DWORD	m_dwWeaponEffectEnd;

	bool m_bIgnoreColOnDelay;
	bool m_bIgnoreColOnMove;
	bool m_bIgnoreColOnJump;

	D3DXVECTOR3 m_vCurPosition;
	D3DXVECTOR3 m_vPrePosition;

	D3DXVECTOR3 m_vPreScale;
	ioEffect *m_pFireBall;

	D3DXVECTOR3 m_vMoveDir;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	void Init();
protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void UpdateCurPosition( ioBaseChar *pOwner );
	void UpdateWeaponEffect( ioBaseChar *pOwner );
	bool IsJumpStateOnBuff();
	bool IsMoveStateOnBuff();
	bool IsDelayStateOnBuff();
	void WeaponEndEffect( ioPlayStage *pStage );

public:
	ioLimitAttachWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioLimitAttachWeapon2();
};

inline ioLimitAttachWeapon2* ToLimitAttachWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_LIMIT_ATTACH2 )
		return NULL;

	return dynamic_cast< ioLimitAttachWeapon2* >( pWeapon );
}

