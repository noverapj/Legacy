#pragma once

#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"
#include "ioItem.h"

class ioPlayMode;
class ioPlayStage;
class ioEffect;
class ioUIRenderImage;
class ioBaseChar;

class ioFieldItem : public ioPlayEntity, public ioWorldEventReceiver
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

	ioItem *m_pItem;

	FieldItemState m_ItemState;
	ReserveNext m_ReserveNext;
	D3DXVECTOR3 m_vReservePos;

	float	m_fDropPower;
	float	m_fDropFriction;

	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pItemBack;
	bool	m_bOwnerCharOver;
	bool	m_bWaitReposition;

protected:
	ioHashString m_DropChar;
	DWORD m_dwDropTime;

	ioHashString m_LastPushChar;
	D3DXVECTOR3 m_vPrePos;
	DWORD m_dwSetTime;

	DWORD m_dwCreateTime;
	TeamType m_TeamType;

protected:
	ioHashString m_szAttackChar;
	ioHashString m_szMoveSkillName;
	float m_fMoveSpeed;
	float m_fCurGravityAmt;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vAxis;

public:
	static float m_fItemBackMeshRate;

protected:
	bool m_bDroppingEndHide;

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

protected:
	void CheckLastPushChar();
	void CheckDropChar();
	//void DropZoneCheck();

	void CheckMoveDropTime( const D3DXVECTOR3 &vStartPos, const D3DXVECTOR3 &vTargetPos );

public:
	void DropZoneCheck();
	void CreateItemEffect();
	void CreatePickEnableEffect();
	void CreateDropEffect();

	void DestroyDropEffect();
	void DestroyItemEffect();
	void DestroyPickEnableEffect();

	void ReserveHiding();
	void ReserveDropHiding();
	void ReserveRePosition( const D3DXVECTOR3 &vPos );

	void LoadResource();
	void RenderItemIcon( float fXPos, float fYPos, float fScale );

public:
	bool HasOwnerName() const;
	void ClearItemOwnerByOutUser( const ioHashString &rkName );
	void ReleaseFieldItemByIndex( int itemIndex );

	ioItem* ReleaseItem();

public:
	void SetOwnerCharOver( bool bOver );
	void SetPushTime( DWORD dwPushTime );
	void SetDropState( const D3DXVECTOR3 &vStartPos );
	void SetWorldPositionByServerSide( const D3DXVECTOR3 &vPos );

	void SetMoveDropState( const ioHashString &szAttacker,
						   const ioHashString &szSkillName,
						   const D3DXVECTOR3 &vStartPos,
						   const D3DXVECTOR3 &vTargetPos,
						   float fMoveSpeed );
	
	void SendFieldItemInfo();

public:
	int GetItemCreateIndex() const;
	ioItem::ItemType GetItemType() const;
	const ioHashString& GetItemName() const;

public:
	bool IsNeedDestroy() const;
	bool IsNeedDestroyByTime() const;
	bool IsCanPicked() const;	
	bool IsDropping() const;
	bool IsObjectItem() const;
	bool IsFlagItem() const;
	bool MaintainItemWhileCharChange() const;

	int GetCrownItemType() const;
	TeamType GetItemTeamType();

	bool CheckTeamType( TeamType eTeam );
	void SetTeamType( TeamType eTeam );
	ioHashString GetDeleteFieldItemAni();

public:
	void SetLastPushChar( const ioHashString &szCharName );
	void SetDropChar( const ioHashString &szCharName );

	int GetCharOnlySexType();

public:
	inline DWORD GetPushTime() const { return m_dwSetTime; }
	inline bool  IsOwnerCharOver() const { return m_bOwnerCharOver; }
	inline const ioHashString& GetLastPushChar() { return m_LastPushChar; }
	inline void SetDroppingEndHide(){ m_bDroppingEndHide = true; }

	DWORD GetEventGapTime();

public:
	ioFieldItem( ioEntityGroup *pGrp, ioPlayMode *pMode, ioItem *pItem );
	virtual ~ioFieldItem();
};

inline ioFieldItem* ToFieldItem( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_FIELD_ITEM )
		return NULL;

	return dynamic_cast< ioFieldItem* >( pPlay );
}

inline const ioFieldItem* ToFieldItemConst( ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_FIELD_ITEM )
		return NULL;

	return dynamic_cast< const ioFieldItem* >( pPlay );
}

typedef std::vector<ioFieldItem*> ioFieldItemVec;