#pragma once
#include "iodummychar.h"
class ioNoneTraceDummy2 :
	public ioDummyChar
{
protected:
	D3DXVECTOR3 m_vCirclePos;
	WeaponMoveInfo m_vWeaponMoveInfo;

	/////////////////////////////////
	float m_fWeaponGravityRate;
	float m_fWeaponFloatPower;
	float m_fWeaponFloatMinPower;
	float m_fWeaponMinSpeed;
	float m_fWeaponGapDistance;
	float m_fRevisionRate;
	float m_fUpLimitRange;
	float m_fUpLimitHeight;
	float m_fCircleRange;
	float m_fFireHightOffset;
	float m_fFireFrontOffset;
	////////////////////////////////

	int	  m_iNormalAttackCode;
protected:
	void CheckWeaponFloatInfo();
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );
public:
	inline void SetCirclePos( D3DXVECTOR3 vCirclePos ) { m_vCirclePos = vCirclePos; }
	virtual void SetAttackState( int iCode );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetAttackState();
protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );
public:
	ioNoneTraceDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioNoneTraceDummy2(void);
};

inline ioNoneTraceDummy2* ToNoneTraceDummy2( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_NONE_TRACE2 )
		return NULL;

	return dynamic_cast< ioNoneTraceDummy2* >( pDummy );
}