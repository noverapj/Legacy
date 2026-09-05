#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioFlyJumpBuff : public ioBuff
{
protected:
	ioHashStringVec m_vJumpAnimationList;
	DWORD m_dwJumpEnableTime;

	bool m_bEnableRotate;
	bool m_bContinueJump;

	float m_fJumpPower;
	float m_fJumpForce;
	float m_fJumpSpeedRate;
	float m_fGravityRate;

	float m_fEnableHeight;
	float m_fAniRate;

	int m_iMaxJumpCnt;

	bool m_bJumpAttack;

	CEncrypt<bool>	m_bEnableMoveRotate;
	CEncrypt<DWORD>	m_dwMoveRotateSpeed;

	bool m_bReduceGauge;
	CEncrypt<bool>	m_bInvisibleMeshOnFlyJump;
	bool m_bHideMesh;

	CEncrypt<bool> m_bWeaponColSkipOnFly;
	CEncrypt<bool> m_bHideOnFly;
	CEncrypt<float> m_fHideAlphaRate;

	ioHashString m_FlyStartEffect;
	ioHashString m_FlyEndEffect;

	bool m_bSetVisible;

	CEncrypt<int> m_iHideRate;
	CEncrypt<int> m_iWeaponColSkipRate;

	bool m_bHide;
	bool m_bWeaponColSkip;

	float m_fJumpEnableGauge;
	float m_fBuffAirWalkStartGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual bool IsCanJumpAttack() const;

	virtual bool OnReduceGauge() const;

	virtual bool IsCharCollisionSkip() const;
	virtual bool IsCollisionAvailableWeapon() const;

	bool IsHideState() const;

protected:
	void ProcessPassive();

public:
	ioFlyJumpBuff();
	ioFlyJumpBuff( const ioFlyJumpBuff &rhs );
	virtual ~ioFlyJumpBuff();
};

inline ioFlyJumpBuff* ToFlyJumpBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_BUFF_FLY_JUMP )
		return NULL;

	return dynamic_cast< ioFlyJumpBuff* >( pBuff );
}

