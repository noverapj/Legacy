#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioMultiFlyJumpBuff : public ioBuff
{
protected:
	ioHashStringVec m_vJumpAnimationList;
	ioHashStringVec m_vJumpExtraAnimationList;
	DWORD m_dwJumpEnableTime;

	bool m_bEnableRotate;
	bool m_bContinueJump;

	float m_fJumpPower;
	float m_fJumpForce;
	float m_fJumpSpeedRate;
	float m_fGravityRate;

	float m_fJumpExtraPower;
	float m_fJumpExtraForce;
	float m_fExtraGravityRate;

	float m_fEnableHeight;
	float m_fAniRate;

	int m_iMaxJumpCnt;

	bool m_bJumpAttack;

	CEncrypt<bool>	m_bEnableMoveRotate;
	CEncrypt<DWORD>	m_dwMoveRotateSpeed;

	bool m_bReduceGauge;
	CEncrypt<bool>	m_bInvisibleMeshOnFlyJump;
	bool m_bHideMesh;

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

protected:
	void ProcessPassive();

public:
	ioMultiFlyJumpBuff();
	ioMultiFlyJumpBuff( const ioMultiFlyJumpBuff &rhs );
	virtual ~ioMultiFlyJumpBuff();
};

