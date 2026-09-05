#pragma once
// 기본적인 ZoneEffectWeapon형태를 가져감.
// 추가로 타격시 레슬러의 SwingBuff를 사용할 수 있는 확장 형태의 웨폰.
// 단순히 충돌 대상에게 SwingBuff를 주는 것이 목적이기에 SwingBuff를 설정할 경우 결과적으로 충돌체크는 하지않는다.

#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioZoneEffect2Weapon : public ioWeapon
{
public:
	enum StateControlSync
	{
		STC_SET_SWING_BUFF,
	};

	enum OwnerAttachType
	{
		OAT_NONE,
		OAT_NORMAL,
		OAT_SKILL,
	};

protected:
	ioEffect	*m_pWeaponEffect;

	TargetColType	m_TargetColType;
	TargetWoundType	m_TargetWoundType;

	OwnerAttachType m_OwnerAttachType;

	CEncrypt<float> m_fZoneRange;
	CEncrypt<float>	m_fZoneAngle;

	CEncrypt<float>	m_fUpHeight;
	CEncrypt<float>	m_fUnderHeight;
	CEncrypt<float>	m_fWidth;

	CEncrypt<float>	m_fOffSet;
	CEncrypt<float>	m_fHeightOffSet;

	D3DXVECTOR3 m_vStartOffSet;
	D3DXVECTOR3 m_vCollisionDir;
	ioOrientBox m_LocalAttackBox;

	//Swing Buff
	GrapplingSkillWoundedInfo	m_WoundedSwingInfo;
	ioHashString				m_szWoundedSwingAni;
	CEncrypt<float>				m_fWoundedSwingRate;
	bool						m_bSwinged;

	CEncrypt<bool>	m_bCheckHeight;

	typedef std::list< ioPlayEntity* > BlockEntityList;
	BlockEntityList m_vBlockEntityList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetZoneValue( bool bAttached, bool bSkill );
	void SetZoneValue( float fRange, float fAngle, float fUpHeight, float fUnderHeight, float fWidth, bool bAttached );
	void SetZoneValue( float fRange, bool bAttached );
	void SetCollisionDir( D3DXVECTOR3 &vDir );
	void SetCollisionWoundType( int iColType, int iWoundType );

	bool CheckCollisionSphere( ioPlayEntity *pEntity );
	bool CheckCollisionCylinder( ioPlayEntity *pEntity );
	bool CheckCollisionBox( ioPlayEntity *pEntity );

	bool IsAlreadyBlocked( ioPlayEntity *pEntity ) const;
	void AddBlockEntity( ioPlayEntity *pEntity );

	int GetCollisionType(){ return m_CollisionType; }

protected:
	bool CreateWeaponEffect( const ioHashString &szFileName );

	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual WeaponType GetType() const;
	virtual void SetLiveTime( DWORD dwTime );
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	bool CheckHeight( ioBaseChar *pTarget );
	bool SetSwingBuff( ioBaseChar *pTarget );
	bool UseSwingBuff();
	void CheckSwingBuff( ioPlayEntity *pEntity );

public:
	ioZoneEffect2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioZoneEffect2Weapon();
};

inline ioZoneEffect2Weapon* ToZoneEffect2Weapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ZONE_EFFECT2 )
		return NULL;

	return dynamic_cast< ioZoneEffect2Weapon* >( pWeapon );
}

