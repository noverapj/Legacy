#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;
class ioBaseChar;
class ioEffect;

class ioGraffitiWeapon : public ioWeapon
{
public:
	enum WeaponState
	{
		TS_NONE,
		TS_EXPLOSION,
	};

protected:
	ioEffect *m_pFireMesh;

	WeaponState m_State;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	ioHashString m_BoomEffect;

	UniqueObjID m_EffectID;

	D3DXVECTOR3 m_vMoveDir;
	ioOrientBox m_LocalAttackBox;

	DWORD m_dwExplosionStart;
	DWORD m_dwExplosionDuration;

	D3DXVECTOR3 m_vStartOffSet;

protected:
	TargetColType	m_TargetColType;
	TargetWoundType	m_TargetWoundType;

	CEncrypt<float> m_fZoneRange;
	CEncrypt<float> m_fZoneMinRange;
	CEncrypt<float> m_fZoneMaxRange;
	CEncrypt<float>	m_fZoneInnerRange;
	CEncrypt<float>	m_fZoneAngle;

	CEncrypt<float>	m_fUpHeight;
	CEncrypt<float>	m_fUnderHeight;
	CEncrypt<float>	m_fWidth;
	CEncrypt<float>	m_fOffSet;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void Explosion( ioPlayStage *pStage );
	bool IsExplosion() const;
	void TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	
public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual DWORD GetCollisionEnableTime() const;

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

protected:
	bool SetFireMesh( const ioHashString &szFileName );

	void OnNoneState( float fTimePerSec, ioPlayStage *pStage );
	void OnExplosionState( float fTimePerSec, ioPlayStage *pStage );

	bool CheckCollisionSphere( ioPlayEntity *pEntity );
	bool CheckCollisionCylinder( ioPlayEntity *pEntity );
	bool CheckCollisionBox( ioPlayEntity *pEntity );
	bool CheckCollisionDonut( ioPlayEntity *pEntity );
	bool CheckCollisionCone( ioPlayEntity *pEntity );

public:
	ioGraffitiWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioGraffitiWeapon();
};

inline ioGraffitiWeapon* ToGraffitiWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_GRAFFITI )
		return NULL;

	return dynamic_cast< ioGraffitiWeapon* >( pWeapon );
}

