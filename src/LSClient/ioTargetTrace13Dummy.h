#pragma once
#include "ioDummyChar.h"

struct DummyTT13AttackInfo
{
	DWORD			m_dwWeaponIndex;
	DWORD			m_dwFireTime;
	ioHashString	m_stTargetName;
	
	//int				m_FindType;

	float			m_fRangeMin;
	float			m_fRangeMax;
	float			m_fAngle;
	float			m_fHeightMin;
	float			m_fHeightMax;

	//¹«±â
	float			m_fWeaponLookOffSet;
	float			m_fWeaponHeighOffSet;
	float			m_fWeaponSideOffSet;

	float			m_fTargetShootOffHeight;

	DummyTT13AttackInfo()
	{
		m_dwWeaponIndex	= 0;
		m_dwFireTime	= 0;
		//m_FindType = 0;

		m_fRangeMin		= 0.f;
		m_fRangeMax		= 0.f;
		m_fAngle		= 0.f;
		m_fHeightMin	= 0.f;
		m_fHeightMax	= 0.f;

		m_fWeaponLookOffSet		= 0.f;
		m_fWeaponHeighOffSet	= 0.f;
		m_fWeaponSideOffSet		= 0.f;

		m_fTargetShootOffHeight = 0.f;
	}
};
typedef std::vector<DummyTT13AttackInfo> DummyTT13AttackInfoList;


class ioTargetTrace13Dummy :	public ioDummyChar
{
	DummyTT13AttackInfoList m_vAttackFire1Info;
	DummyTT13AttackInfoList m_vAttackFire2Info;
	DummyTT13AttackInfoList m_vAttackFire3Info;

	DummyTT13AttackInfoList m_vAttackFireReserveInfo;

protected:
	enum TraceState
	{
		TS_Start,
		TS_Find_Trace_Target,
		TS_Move,
		TS_End,
	};

	float m_fAddOffSetHeight;
	int	  m_nAttackAttributeIndex;
	DWORD m_dwCreateFireTime;

	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 m_vOffsetPos;
	D3DXVECTOR3 m_vMoveDir;
	bool m_bAutoFollow;
	//////////////////////////////////////////////////////////////////////////
	DWORD m_dwAttack1FiredTime;
	DWORD m_dwAttack2FiredTime;
	DWORD m_dwAttack3FiredTime;

	DWORD m_dwAttack1FireGapTime;
	DWORD m_dwAttack2FireGapTime;
	DWORD m_dwAttack3FireGapTime;

	ioHashString m_szFindTarget;

	float m_fFind_1_Angle;
	float m_fFind_1_Range_min;
	float m_fFind_1_Range_max;
	float m_fFind_1_Height_min;
	float m_fFind_1_Height_max;
	float m_fWeapon_1_LookOffSet;
	float m_fWeapon_1_height_OffSet;
	float m_fWeapon_1_Side_OffSet;
	float m_fTarget_1_ShootOffHeight;

	float m_fFind_2_Angle;
	float m_fFind_2_Range_min;
	float m_fFind_2_Range_max;
	float m_fFind_2_Height_min;
	float m_fFind_2_Height_max;
	float m_fWeapon_2_LookOffSet;
	float m_fWeapon_2_height_OffSet;
	float m_fWeapon_2_Side_OffSet;
	float m_fTarget_2_ShootOffHeight;

	float m_fFind_3_Angle;
	float m_fFind_3_Range_min;
	float m_fFind_3_Range_max;
	float m_fFind_3_Height_min;
	float m_fFind_3_Height_max;
	float m_fWeapon_3_LookOffSet;
	float m_fWeapon_3_height_OffSet;
	float m_fWeapon_3_Side_OffSet;
	float m_fTarget_3_ShootOffHeight;

	//////////////////////////////////////////////////////////////////////////
	float m_fFollowLength;

protected:
	void OnProcessStartState();

	void OnProcessTraceState( float fTimePerSec, DWORD dwPreTime );
	void OnProcessTraceSensing( float fTimePerSec );
	void OnProcessTraceTracing( float fTimePerSec );
	void OnProcessTraceAttack( float fTimePerSec, DWORD dwPreTime );
	void OnProcessTraceJumpAttack( ioPlayStage *pStage, float fTimePerSec );

	ioHashString GetSearchTarget( ioPlayStage *pStage, bool bSecond );

	void CheckTarget( ioPlayStage *pStage );
	ioWeapon* CreateWeapon( ioBaseChar *pOwner, const D3DXVECTOR3& vFireDir, DWORD dwWeaponBaseIndex, const DummyTT13AttackInfo& Info );
	void ProcessPosition( ioBaseChar *pOwner );

	bool FindTarget( ioBaseChar *pOwner, float fMinLength, float fMaxLength, float fAngle, float fCheckHeightGapMin, float fCheckHeightGapMax );
	bool CheckFindedTarget( ioBaseChar *pOwner, float fMinLength, float fMaxLength, float fAngle, float fCheckHeightGapMin, float fCheckHeightGapMax, const ioHashString& stTargetName );

	void SetOffsetPosAndDir( ioBaseChar *pOwner );
	void FollowOwner( ioBaseChar *pOwner, float fTimePerSec );	

	void ProcessWeaponCall( ioBaseChar *pOwner, float fTimePerSec );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void InitDummyCharInfo(	int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale,float fStartAngle=0.0f, bool bNetWork = false );
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );

public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

public:
	ioTargetTrace13Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioTargetTrace13Dummy(void);
};

inline ioTargetTrace13Dummy* ToTargetTrace13Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE13 )
		return NULL;

	return dynamic_cast< ioTargetTrace13Dummy* >( pDummy );
}