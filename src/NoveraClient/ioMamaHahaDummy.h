#pragma once

#include "ioDummyChar.h"

class ioEntityGroup;
class ioPlayStage;
class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;

class ioMamaHahaDummy : public ioDummyChar
{
public:
	enum MamaHahaState
	{
		MHS_NONE,
		MHS_OWNER_ATTACH,	// 특별
		MHS_FIRE_ATTACK,	// 레라슈
		MHS_FLYING1,		// 시치카푸 에투	: 날아가기
		MHS_RETURN1,		// 시치카푸 에투	: 되돌아오기
		MHS_FLYING2,		// 시치카푸 암	: 날아가기
		MHS_RETURN2,		// 시치카푸 암	: 되돌아오기
		MHS_ENEMY_ATTACH,	// 시치카푸 암	: 적 잡기
	};

protected:
	MamaHahaState m_MamaHahaState;
	D3DXVECTOR3 m_vMamaHahaBaseOffSet;

	D3DXVECTOR3 m_vMamaHahaOwnerAttachOffSet;
	ioHashString m_MamaHahaOwnerAttachAni;
	float m_fMamaHahaOwnerAttachAniRate;

	ioHashString m_MamaHahaEnemyAttachAni;
	float m_fMamaHahaEnemyAttachAniRate;

	ioHashString m_MamaHahaEnemyAttachBuff;
	ioHashString m_MamaHahaTargetName;
	D3DXVECTOR3 m_vMamaHahaEnemyAttachOffSet;

protected:
	CEncrypt<float> m_fShichikapuMoveSpeed;
	CEncrypt<float> m_fShichikapuMoveAngle;
	CEncrypt<float> m_fShichikapuReturnSpeed;
	CEncrypt<float> m_fShichikapuLimitRange;

	CEncrypt<DWORD> m_dwShichikapuMoveDuration;

	D3DXVECTOR3 m_vShichikapuMoveTargetPos;
	D3DXVECTOR3 m_vShichikapuMoveDir;

	float m_fShichikapuMoveMaxRange;
	float m_fShichikapuMoveCurRange;
	
	IntVec m_vMamaHahaWeaponList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );

public:
	virtual void SetDelayState( bool bNet );
	virtual void SetAttackState();
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );

protected:
	void ProcessMamaHahaState( float fTimePerSec, DWORD dwPreTime );
	void ProcessEnemyAttachState( float fTimePerSec );

public:
	void SetMamaHahaOwnerAttach();
	void ReleaseMamaHahaOwnerAttach();

	bool CheckEnableOwnerAttach();
	bool CheckEnableUseCmd( int iType );

protected:
	void SetMamaHahaState( MamaHahaState eType );

	void SetNormalAttackState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType );
	void SetReraShuState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType );
	void SetShichikapuEtuState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType );
	void SetShichikapuArmState( const AttackAttribute &rkCurAttribute, DummySubUseType eSubType );

	void SetMamaHahaEnemyAttach( const ioHashString &szName );
	void ReleaseMamaHahaEnemyAttach();

	bool CheckMamaHahaOwnerVisible();
	void CheckMamaHahaVisible();

	void SetShichikapuReturn();

	void ClearMamaHahaWeaponList();

public:
	ioMamaHahaDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMamaHahaDummy();
};

inline ioMamaHahaDummy* ToMamaHahaDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_MAMAHAHA )
		return NULL;

	return dynamic_cast< ioMamaHahaDummy* >( pDummy );
}