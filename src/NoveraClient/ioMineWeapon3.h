#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioMineWeapon3 : public ioWeapon
{
public:
	enum MineState
	{
		MS_CHARGE,
		MS_DELAY,
		MS_SENSING,
		MS_WAIT_EXPLOSION, //Fly_tracing 전의 상태에서 폭팔 이벤트를 처리할 State
		//MS_FLY_WAIT,
		//MS_FLY_TRACING,
		MS_EXPLOSION,
		MS_NORMAL_DEAD,
	};

protected:
	ioEntityGroup *m_pMine;

	DWORD m_dwSensingStartTime;

	//추적전에 폭팔 상태로 전환시의 딜레이
	DWORD m_dwTraceDelayTime; // Target 기준 지연 시간(INI)
	DWORD m_dwCurTraceDelayTime;

	//추적시작 준비 딜레이시간
	//DWORD m_dwFlyTraceDealyTime;
	//DWORD m_dwCurFlyTraceDealyTime;

	ioHashString m_szTargetName;

	D3DXVECTOR3 m_vPrePos;
	D3DXVECTOR3 m_vMoveDir;

	CEncrypt<float> m_fPreUpHeight;

	CEncrypt<float> m_fTraceSpeed;
	//CEncrypt<float> m_fTraceReadySpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fTraceRotSpeed;
	CEncrypt<float> m_fTraceEndRange;

	DWORD m_dwTraceDuration;
	DWORD m_dwTraceEndTime;

	CEncrypt<float> m_fTargetTracePosRate;
	CEncrypt<float> m_fDownTargetTracePosRate;;

	float m_fCollisionRange;
	float m_fExplosionRange;
	float m_fMineBoomDist;
	float m_fVisibleRange;
	float m_fMineSensingRange;

	float m_fMineSizeRate;				// 마인의 사이즈 비율

	ioHashString m_BlueMineModel;
	ioHashString m_RedMineModel;
	ioHashString m_BoomEffect;
	ioHashString m_NormalDeadEffect;

	ioHashStringVec m_vTeamModelList;
	ioHashStringVec m_vEnemyModelList;

	ioOrientBox m_WorldCollisionBox;
	ioOrientBox m_LocalAttackBox;
	bool m_bMineVisible;
	bool m_bTeamModel;
	bool m_bWait;

	// Extend
	TeamType m_CurTeamColor;

	bool m_bUseTeamTargetBuff;
	bool m_bNoSencingMine;
	bool m_bNoMineExplosion;
	bool m_bNoMineUp;
	bool m_bNormalDead;

	BuffInfoList m_RedTargetAirBuffList;
	BuffInfoList m_RedTargetBuffList;
	BuffInfoList m_RedTargetDownBuffList;

	MineState m_State;
	MineType m_MineType;

	ioHashString m_szTraceStartEffect;
	ioHashString m_szFireSprayEffect;

	DWORD m_dwFireSprayEffect;
	D3DXVECTOR3 m_vEffectDir;
	float m_fGapAngle;

	ioHashString m_szTraceStartSound;
	UniqueObjID m_dwTraceStartSound;

	//////////////////////////////////////////////////////////////////////////
	int m_nMaxAttackCnt;
	int m_nCurAttackCnt;

	DWORD	m_dwMaxDelayTime;
	DWORD	m_dwRemainDelayTime;
	DWORD   m_dwDelayStartTime;

	DWORD	m_dwMaxAttackChargeTime;
	DWORD   m_dwAttackChargeStartTime;

	ioHashString m_stZoneEffect_Red;
	ioHashString m_stZoneEffect_Blue;
	DWORD	m_dwZoneEffectID;

	WeaponInfoList	m_CallWeaponInfoList;

	float	m_fOffSetY;

	DWORD	m_dwMaxLifeTime;

protected:
	int  m_iConveyerMoveFrameIdx;

	// For Mine Sync
protected:
	int m_iAttributeIndex;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetSensingStartTime( DWORD dwTime ) { m_dwSensingStartTime = dwTime; }

public:
	bool IsCanExplosion() const;
	bool IsCanVisible( ioWeapon *pWeapon ) const;

	virtual const BuffInfoList& GetTargetAirBuffList() const;
	virtual const BuffInfoList& GetTargetBuffList() const;
	virtual const BuffInfoList& GetTargetDownBuffList() const;

public:
	void SetDelay( ioPlayStage *pStage, bool bFirst, bool bSendNetwork );
	void SetWaitExplosionMine( ioPlayStage *pStage, bool bSendNetwork );

	void SetTraceTarget( ioHashString szTarget );

	void TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt );
	void SetNormalDeadMine( ioPlayStage *pStage, bool bSendNetwork );		

	// bWait 값은 동일프레임에서 다른 오브젝트(캐릭터, 포대등)에 의해 미리 폭발했을 경우
	// Process 과정에서 SetWeaponDead()를 시키지 않기 위해 설정하는 값
	void ExplosionMine( ioPlayStage *pStage, bool bWait=false, bool bSendNetwork=true );
	void NormalDeadMine( ioPlayStage *pStage, bool bWait=false );

	bool FindTarget( ioPlayStage *pStage, float fTimePerSec );

protected:
	void OnCharge( ioPlayStage *pStage );
	void OnDelay( ioPlayStage *pStage );
	void OnSensing( ioPlayStage *pStage, float fTimePerSec );
	void OnWaitExplosion( ioPlayStage *pStage );
	void OnExplosion(ioPlayStage *pStage);
	void OnNormalDead();

	bool CheckTargetWoundedState( ioBaseChar *pTarget );
	bool CheckDownTargetState( ioBaseChar *pTarget );
	bool DropZoneCheck( ioPlayStage *pStage );

	void VisibleCheck( ioPlayStage *pStage );
	void UpdateWorldCollisionBox();

	void CallAttribute();

public:
	DWORD GetSensingStartGapTime();

	inline const ioOrientBox& GetWorldCollisionBox() { return m_WorldCollisionBox; }

	inline MineType GetMineType() const { return m_MineType; }
	inline bool IsCanMineSensing() const { return !m_bNoSencingMine; }
	inline bool IsNormalDead() const { return m_bNormalDead; }

	inline MineState GetMineState(){ return m_State; }

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	//virtual void TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage );

	virtual bool IsCanMineExplosion();

	virtual void SetWeaponDead();

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );

public:
	ioMineWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioMineWeapon3();
};

inline ioMineWeapon3* ToMineWeapon3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_MINE3 )
		return NULL;

	return dynamic_cast< ioMineWeapon3* >( pWeapon );
}

