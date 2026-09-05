#pragma once

#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"

class ioPlayMode;
class ioINILoader;
class ioWeapon;

class ioRoulette : public ioPlayEntity, public ioWorldEventReceiver
{
protected:
	int m_iRouletteType;
	int m_iSubIndex;

	bool m_bReserveDestroy;

protected:
	ioPlayStage *m_pPlayStage;
	DWORD m_dwProcessTime;

	ArmorType m_ArmorType;
	D3DXVECTOR3 m_vRollAxis;

// OutLine
protected:
	OutLineRecordList m_OutLineChangeList;
	D3DCOLORVALUE m_OutLineColor;
	float m_fOutLineSize;
	bool  m_bOutLineModified;

public:
	void InitRouletteInfo( ioPlayStage *pStage, int iType, int iSubIndex, D3DXVECTOR3 vPos, D3DXVECTOR3 vScale );

	void SetRouletteType( int iType ) { m_iRouletteType = iType; }
	int  GetRouletteType()            { return m_iRouletteType; }

	void SetSubIndex( int iIndex ) { m_iSubIndex = iIndex; }
	int  GetSubIndex()             { return m_iSubIndex; }

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

public:
	virtual bool IsNeedCheckMapCollision() const;
	virtual bool IsMountAvailable() const;		// 자신의 위에 다른 오브젝트를 Mount 할수 있는 객체인가?
	virtual bool IsMountingAvailable() const;	// 다른 오브젝트에 쌓여질수 있는 객체인가? Mounted가 될수 있나?
	virtual bool IsNowMountEnable() const;		// 현재 Mount가 가능한 상태인가?

	virtual int  DontMoveEntityLevel() const;

public:
	virtual ioWorldEventReceiver* GetWorldEventReceiver();

protected:
	virtual void UpdateBottomHeight() const;

public:
	ioEffect* AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner = NULL, bool bDependence=true );
	void EndEffect( UniqueObjID eObjID, bool bGrace = true );

protected:
	void ProcessState( float fTimerPerSec );

public:
	void SetReserveDestroy() { m_bReserveDestroy = true; }
	bool IsReserveDestroy() { return m_bReserveDestroy; }

public:
	ioRoulette( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioRoulette();
};

inline ioRoulette* ToRoulette( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_ROULETTE )
		return NULL;

	return dynamic_cast< ioRoulette* >( pPlay );
}

inline const ioRoulette* ToRouletteConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_ROULETTE )
		return NULL;

	return dynamic_cast< const ioRoulette* >( pPlay );
}