#pragma once


#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;

class ioAreaEffectWeapon : public ioWeapon
{
	struct stWantCheckList
	{
		ioHashString m_stName;
		bool		 m_bTested;

		stWantCheckList()
		{
			m_bTested = false;
		}
	};
protected:
	std::vector< stWantCheckList > m_vWantCheckList;

	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	ioHashStringVec m_vNewTargetList;

	CEncrypt<float> m_fCollisionRange;
	CEncrypt<float> m_fCollisionInRange;
	CEncrypt<float> m_fAreaAngle;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<float> m_fWidth;

	D3DXVECTOR3 m_vCollisionDir;
	ioOrientBox m_LocalAttackBox;

	typedef std::list< ioPlayEntity* > BlockEntityList;
	BlockEntityList m_vBlockEntityList;
	bool			m_bEndAreaAttack;

	//////////////////////////////////////////////////////////////////////////
	//CCAI¨¡O SD¢Ò¡× A©¬¡Æ¢®
	bool m_bCheckCollinsionOnlyOne;
	//int  m_nCheckedCnt;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void SetCollisionWoundType( int iColType, int iWoundType );
	void SetAreaValue( float fRange, float fInRange, float fAngle, float fUpHeight, float fUnderHeight, float fWidth );
	void SetCollisionDir( D3DXVECTOR3 &vDir );

	void UpdatePosition( const D3DXVECTOR3 &vPos );
	void UpdateRange( float fRange, float fInRange );
	void UpdateCollisionList( BaseCharList &rkCharList, bool bRefresh );

	bool CheckCollisionSphere( ioPlayEntity *pEntity );
	bool CheckCollisionCylinder( ioPlayEntity *pEntity );
	bool CheckCollisionBox( ioPlayEntity *pEntity );

	bool IsAlreadyBlocked( ioPlayEntity *pEntity ) const;
	void AddBlockEntity( ioPlayEntity *pEntity );

protected:
	bool IsExistTarget( const ioHashString &szName );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

public:
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	ioAreaEffectWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAreaEffectWeapon();
};

inline ioAreaEffectWeapon* ToAreaEffectWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_AREA_EFFECT )
		return NULL;

	return dynamic_cast< ioAreaEffectWeapon* >( pWeapon );
}

