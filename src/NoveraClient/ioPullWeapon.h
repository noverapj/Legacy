#pragma once
#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioPullWeapon : public ioWeapon
{
protected:

	enum WeaponState
	{
		WS_INIT,
		WS_MOVE,
		WS_WAIT,
	};

	enum SyncState
	{
		SS_NONE,
		SS_COLLISION,
	};

	WeaponState m_State;

	TargetColType	m_TargetColType;
	TargetWoundType	m_TargetWoundType;
	CEncrypt<bool>	m_bDisableColGrap;

	CEncrypt<float> m_fZoneRange;
	CEncrypt<float> m_fZoneMinRange;
	CEncrypt<float> m_fZoneMaxRange;
	CEncrypt<float>	m_fZoneInnerRange;
	CEncrypt<float>	m_fZoneAngle;
	float			m_fCurRange;

	CEncrypt<float>	m_fUpHeight;
	CEncrypt<float>	m_fUnderHeight;
	CEncrypt<float>	m_fWidth;
	CEncrypt<float>	m_fMaxWidth;
	float			m_fCurWidth;

	CEncrypt<float>	m_fOffSet;
	CEncrypt<float>	m_fHeightOffSet;
	CEncrypt<bool> m_bEnableProtectCol;

	D3DXVECTOR3 m_vStartOffSet;
	D3DXVECTOR3 m_vCollisionDir;
	D3DXVECTOR3 m_vAttackDir;
	ioOrientBox m_LocalAttackBox;

	ioHashStringVec m_szTargetsName;
	ioHashStringVec m_szTargetWoundBuff;
	ioHashStringVec m_szTargetCancelBuff;

	D3DXVECTOR3 m_vMoveDir;

	float m_fCurMoveRange;
	float m_fCurMoveRate;

	DWORD m_dwPullCnt;
	WeaponInfoList	m_CallWeaponInfoList;

	bool m_bArleadyCallWeapon;
	bool m_bSetOwnerOrderWeaponDead;
	bool m_bMoveComplete;

	DWORD m_dwPreStartTime;
	DWORD m_dwPreDuration;
	DWORD m_dwStartTime;
	D3DXVECTOR3 m_vCallStartPosition;

protected:
	ioHashString	m_szRemoveSkipStunBuffName;
public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetCollisionDir( D3DXVECTOR3 &vDir );
	void SetCollisionWoundType( int iWoundType );
	bool CheckCollisionBox( ioPlayEntity *pEntity );

	int GetCollisionType(){ return m_CollisionType; }
	inline const ioHashString& GetRemoveSkipStunBuffName() { return m_szRemoveSkipStunBuffName; }
protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual void SetWeaponDead();

public:
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );
	virtual void ApplyExtraReserveWeaponByWeaponInfo( ioWeapon* pWeapon );

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );

public:
	void SetAttackDir(  D3DXVECTOR3& vDir );
	void SetCallStartPosition( const D3DXVECTOR3& vPos );

	float GetMoveSpeed();
	float GetMaxRange();

	void ClearData();
	void CallAttribute();

	void SetTargetBuff( ioPlayStage* pStage );

	ioHashStringVec GetTargetList();
	bool isMoveComplete();

	void SetWeaponPreInfo( DWORD dwPreTime, DWORD dwDuration );

public:
	ioPullWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioPullWeapon();
};

inline ioPullWeapon* ToPullWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_PULL_WEAPON )
		return NULL;

	return dynamic_cast< ioPullWeapon* >( pWeapon );
}