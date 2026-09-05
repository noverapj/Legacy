#pragma once

#include "ioGameEntity.h"
#include "PlayEntityHelpStruct.h"

class ioWeapon;
class ioEntityGroup;
class ioPlayMode;
class ioOrientBox;
class ioINILoader;
class ioWorldEventReceiver;

class ioSphere;
class ioCylinder;

class ioPlayEntity : public ioGameEntity
{
public:
	enum PlaySubType
	{
		PST_CHAR,

		PST_SYMBOL,
		PST_PUSHSTRUCT,
		PST_FIELD_ITEM,

		PST_TELEPORT_ZONE,
		PST_CONVEYER,
		PST_ELEVATOR,
		PST_AFFECTOR_ZONE,

		PST_BALL,
		PST_MACHINE_STRUCT,

		PST_DUMMY_CHAR,

		PST_WALL_ZONE,

		PST_FIELD_REWARD_ITEM,

		PST_ROULETTE,

		PST_PET,
		PST_HOUSING_BLOCK,
	};

protected:
	ioPlayMode *m_pModeParent;
	CEncrypt<TeamType>    m_TeamType;
	StatusValue m_HP;

	HeadDamageInfoList m_HeadInfoList;

	int		m_iGaugeXPos;
	int		m_iGaugeYPos;
	bool	m_bGaugeVisible;

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon ) = 0;
	
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon ) = 0;
	virtual bool IsCollisionAvailableRange( ioWeapon *pWeapon );

	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsCharCollisionSkipState( TeamType eTeamType, int iState=0 ) const;
	virtual bool IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint = NULL ) const;
	virtual ioOrientBox GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false );

	virtual bool CheckSphereDistance( const D3DXVECTOR3 &vStart, float fRange, float &fDistance ) const;
	virtual bool CheckSpherePoint( const D3DXVECTOR3 &vStart, float fRange, D3DXVECTOR3 &vPoint ) const;
	virtual bool CheckCylinderPoint( const ioCylinder &rkCylinder,
									 const D3DXVECTOR3 &vStart,
									 D3DXVECTOR3 &vPoint ) const;

	virtual PlaySubType GetSubType() const = 0;
	virtual ArmorType GetArmorType() const = 0;

public:
	virtual int DontMoveEntityLevel() const;
	virtual void NotifyDontMoveCollisionPushingSkip( ioGameEntity* pColEntity );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual ioWorldEventReceiver* GetWorldEventReceiver();

public:
	virtual void Revival();
	virtual void UpdateGauge();
	virtual void RenderGauge();
	virtual void FillMaxHP();

protected:
	virtual void ProcessRecovery();

public:
	void SetCurHP( float fValue );
	inline StatusValue& GetHP() { return m_HP; }

public:
	virtual void AddHeadDamageInfo( int iDamage, bool bPlus = false );
	virtual void RenderHeadInfoList( int iXPos, int iYPos );

protected:
	virtual void UpdateHeadInfoList( DWORD dwCurTime );

public:
	virtual void SetTeam( TeamType eType ) { m_TeamType = eType; }
	inline TeamType GetTeam() const { return m_TeamType; }

public:	// overide ioGameEntity
	virtual int GetTypeID() const;

public:
	bool IsZeroHPByMode();	

public:
	ioPlayEntity( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioPlayEntity();
};

inline ioPlayEntity* ToPlayEntity( ioGameEntity *pObj )
{
	if( !pObj || pObj->GetTypeID() != ioGameEntity::GET_PLAY_ENTITY )
		return NULL;

	return dynamic_cast< ioPlayEntity* >( pObj );
}

inline const ioPlayEntity* ToPlayEntityConst( const ioGameEntity *pObj )
{
	if( !pObj || pObj->GetTypeID() != ioGameEntity::GET_PLAY_ENTITY )
		return NULL;

	return dynamic_cast< const ioPlayEntity* >( pObj );
}

