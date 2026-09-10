#pragma once


#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"

class ioPlayMode;
class ioPlayStage;
class ioEffect;
class ioUIRenderImage;
class ioBaseChar;

class ioFieldRewardItem : public ioPlayEntity, public ioWorldEventReceiver
{
protected:
	enum FieldItemState
	{
		FIS_NORMAL,
		FIS_DROP,
		FIS_HIDE,
		FIS_DESTROY,
		FIS_MOVE,
	};

	enum ReserveNext
	{
		RN_NONE,
		RN_REPOSITION,
		RN_DROP_HIDE,
		RN_HIDE,
	};

protected:
	UniqueObjID m_ItemEffect;
	UniqueObjID m_PickEnableEffect;
	UniqueObjID m_CreateDropEffect;

	FieldItemState m_ItemState;
	ReserveNext    m_ReserveNext;
	D3DXVECTOR3    m_vReservePos;

	float	m_fDropPower;
	float	m_fDropFriction;

	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pItemBack;

	bool	m_bOwnerCharOver;

protected:
	DWORD        m_dwDropTime;
	D3DXVECTOR3  m_vPrePos;
	DWORD		 m_dwCreateTime;

protected:
	float		m_fMoveSpeed;
	D3DXVECTOR3 m_vTargetPos;

public:
	static float m_fItemBackMeshRate;

protected:
	bool m_bDroppingEndHide;

protected:
	ioHashString m_szOwnerName;
	int m_iFieldRewardUniqueIndex;
	int m_iFieldRewardResourceIndex;

//--------- ioGameEntity ---------------------------------------

public:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );

	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
	virtual void TranslateByWorldEntity( const D3DXVECTOR3 &vMove );

	virtual bool IsNeedCheckMapCollision() const;
	virtual void DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt );

protected:
	virtual void UpdateBound() const;
	virtual void UpdateBottomHeight() const;

//--------- ioPlayEntity ---------------------------------------

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint = NULL ) const;

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

//--------------------------------------------------------------

public:
	virtual ioWorldEventReceiver* GetWorldEventReceiver();

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

protected:
	ioPlayStage* GetCreator() const;

	void ProcessNormal();
	void ProcessDropping( float fTimePerSec );
	void ProcessMoving( float fTimePerSec );
	void ProcessHidding();

	bool CheckEnableMove();

public:
	void CreateItemEffect();
	void CreatePickEnableEffect();
	void CreateDropEffect();

	void DestroyDropEffect();
	void DestroyItemEffect();
	void DestroyPickEnableEffect();

	void ReserveHiding();
	void ReserveDropHiding();
	void SetDestroy();

	void LoadResource();
	void RenderItemIcon( float fXPos, float fYPos, float fScale );

public:
	void SetOwnerCharName( const ioHashString &rkOwnerName );
	void SetOwnerCharOver( bool bOver );
	void SetPushTime( DWORD dwPushTime );
	void SetDropState( const D3DXVECTOR3 &vStartPos );
	void SetWorldPositionByServerSide( const D3DXVECTOR3 &vPos );

	void SetMoveDropState( const D3DXVECTOR3 &vStartPos,
						   const D3DXVECTOR3 &vTargetPos,
						   float fMoveSpeed );
	
	void SendFieldItemInfo();

public:
	bool IsNeedDestroy() const;
	bool IsCanPicked() const;
	bool IsDropping() const;

public:
	inline const ioHashString &GetOwnerCharName() const { return m_szOwnerName; }
	inline const int GetRewardUniqueIndex() const { return m_iFieldRewardUniqueIndex; }

public:
	ioFieldRewardItem( ioEntityGroup *pGrp, ioPlayMode *pMode, int iFieldRewardUniqueIndex, int iFieldRewardResourceIndex );
	virtual ~ioFieldRewardItem();
};

inline ioFieldRewardItem* ToFieldRewardItem( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_FIELD_REWARD_ITEM )
		return NULL;

	return dynamic_cast< ioFieldRewardItem* >( pPlay );
}

inline const ioFieldRewardItem* ToFieldRewardItemConst( ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_FIELD_REWARD_ITEM )
		return NULL;

	return dynamic_cast< const ioFieldRewardItem* >( pPlay );
}

