

#include "stdafx.h"

#include "ioFieldItem.h"

#include "ioPlayMode.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioFieldItemMgr.h"
#include "ioCloakWearItem.h"

ioFieldItem::ioFieldItem( ioEntityGroup *pGrp, ioPlayMode *pMode, ioItem *pItem )
: ioPlayEntity( pGrp, pMode )
{
	m_ItemEffect = -1;
	m_PickEnableEffect = -1;
	m_CreateDropEffect = -1;

	m_pItem = pItem;

	m_ItemState = FIS_NORMAL;
	m_ReserveNext = RN_NONE;

	m_fDropPower = 300.0f;
	m_fDropFriction = 150.0f;
	m_fCurGravityAmt = 0.0f;

	m_pItemIcon = NULL;
	m_pItemBack = NULL;
	
	m_bOwnerCharOver = false;
	m_bWaitReposition = false;

	m_vPrePos.x = 0.0f;
	m_vPrePos.y = 0.0f;
	m_vPrePos.z = 0.0f;
	m_dwSetTime = 0;
	m_dwDropTime = 0;

	m_fMoveSpeed = 0.0f;
	m_bDroppingEndHide = false;

	m_dwCreateTime = FRAMEGETTIME();
	m_TeamType = TEAM_NONE;
}

ioFieldItem::~ioFieldItem()
{
	DestroyItemEffect();
	DestroyDropEffect();
	DestroyPickEnableEffect();

	SAFEDELETE( m_pItem );
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pItemBack );
}

void ioFieldItem::Update( float fTimePerSec,
						  DWORD dwPreTime,
						  DWORD dwCurTime )
{
	CheckLastPushChar();
	CheckDropChar();

	switch( m_ItemState )
	{
	case FIS_NORMAL:
		ProcessNormal();
		break;
	case FIS_DROP:
		ProcessDropping( fTimePerSec );
		break;
	case FIS_HIDE:
		ProcessHidding();
		break;
	case FIS_MOVE:
		ProcessMoving( fTimePerSec );
		break;
	}
}

void ioFieldItem::UpdateAfter( float fFrameGap )
{
	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	if( pItemEffect )
		pItemEffect->Update( fFrameGap );

	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	if( pPickEnableEffect )
		pPickEnableEffect->Update( fFrameGap );

	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );
	if( pDropEffect )
		pDropEffect->Update( fFrameGap );


	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioGameEntity::UpdateAfter( fFrameGap );
}

bool ioFieldItem::IsNeedCheckMapCollision() const
{
	return true;
}

void ioFieldItem::DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt )
{
	if( !ToBaseChar( pPusher ) && !ToDummyChar( pPusher ) )
	{
		Translate( vMoveAmt );
	}
}

void ioFieldItem::UpdateBound() const
{
	m_WorldColBox = GetCollisionBox();
	m_WorldColBox.Transform( GetWorldPosition(),
							 GetWorldScale(),
							 D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );

	m_WorldAxisBox.SetByOrientBox( m_WorldColBox );
}

void ioFieldItem::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

ApplyWeaponExp ioFieldItem::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioFieldItem::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

bool ioFieldItem::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	return false;
}

ioPlayEntity::PlaySubType ioFieldItem::GetSubType() const
{
	return PST_FIELD_ITEM;
}

ArmorType ioFieldItem::GetArmorType() const
{
	return AT_UNARMOR;
}

ioWorldEventReceiver* ioFieldItem::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

void ioFieldItem::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	SetDropState( vTargetPos );
}

void ioFieldItem::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
	// No Damage..
}

void ioFieldItem::OnPoisonZoneDamage()
{
}

ioPlayStage* ioFieldItem::GetCreator() const
{
	return m_pModeParent->GetCreator();
}

void ioFieldItem::ProcessNormal()
{
	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	switch( m_ReserveNext )
	{
	case RN_REPOSITION:
		SetWorldPositionByServerSide( m_vReservePos );
		CreateDropEffect();
		CreateItemEffect();
		m_ReserveNext = RN_NONE;
		break;
	case RN_DROP_HIDE:
		// FIS_DROP -> FIS_NORMAL -> 'FIS_HIDE' -> FIS_DESTROY
		ReserveHiding();
		break;
	case RN_HIDE:
		m_ItemState = FIS_HIDE;
		m_ReserveNext = RN_NONE;

		SetTargetAlphaRate( 0 );

		if( pItemEffect )
			pItemEffect->EndEffectForce();

		if( pPickEnableEffect )
			pPickEnableEffect->EndEffectForce();

		if( pDropEffect )
			pDropEffect->EndEffectForce();

		break;
	}

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fRealHeight = GetBottomHeight() + GetCollisionBox().GetExtents(1);

	if( vCurPos.y - fRealHeight > 20.0f )
	{
		SetDropState( vCurPos );
	}
	else
	{
		vCurPos.y = fRealHeight;
		if( m_vPrePos == vCurPos && !m_LastPushChar.IsEmpty() )
		{
			if( m_dwSetTime == 0 )
			{
				m_dwSetTime = FRAMEGETTIME();
			}
		}
		else
		{
			SetWorldPosition( vCurPos );
			DropZoneCheck();

			m_vPrePos = vCurPos;
			m_dwSetTime = 0;
		}
	}
}

void ioFieldItem::ProcessDropping( float fTimePerSec )
{
	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	switch( m_ReserveNext )
	{
	case RN_HIDE:
		m_ItemState = FIS_HIDE;
		m_ReserveNext = RN_NONE;

		SetTargetAlphaRate( 0 );

		if( pItemEffect )
			pItemEffect->EndEffectForce();

		if( pPickEnableEffect )
			pPickEnableEffect->EndEffectForce();

		if( pDropEffect )
			pDropEffect->EndEffectForce();

		D3DXVECTOR3 vCurPos = GetWorldPosition();
		float fRealHeight = GetBottomHeight() + GetCollisionBox().GetExtents(1);

		vCurPos.y = fRealHeight;
		SetWorldPosition( vCurPos );

		m_vPrePos = vCurPos;
		m_dwSetTime = 0;
		return;
	}

	D3DXVECTOR3 vPos = GetWorldPosition();
	vPos.y += m_fDropPower * fTimePerSec;
	m_fDropPower -= m_fDropFriction * fTimePerSec;

	m_pGroup->GetParentNode()->PitchX( fTimePerSec * 900.0f );

	float fMapHeight = GetBottomHeight() + GetCollisionBox().GetExtents(1);

	if( fMapHeight < 0.0f )
		fMapHeight = 0.0f;

	if( fMapHeight > vPos.y )
	{
		vPos.y = fMapHeight;
		SetWorldOrientation( ioMath::QUAT_IDENTITY );

		if( m_iCurAlphaRate != 0 )
		{
			m_ItemState = FIS_NORMAL;
			if( m_ReserveNext == RN_NONE )
			{
				CreateDropEffect();
				CreateItemEffect();
			}
		}
		else
		{
			m_ItemState = FIS_HIDE;
		}
	}

	SetWorldPosition( vPos );

	if( m_bDroppingEndHide )
	{
		if( m_ItemState == FIS_NORMAL )
			ReserveHiding();
	}
}

void ioFieldItem::ProcessHidding()
{
	if( m_iCurAlphaRate != 0 )	return;

	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	if( pItemEffect && pItemEffect->IsLive() )
		return;
	if( pPickEnableEffect && pPickEnableEffect->IsLive() )
		return;
	if( pDropEffect && pDropEffect->IsLive() )
		return;

	m_ItemState = FIS_DESTROY;
}

void ioFieldItem::ProcessMoving( float fTimePerSec )
{
	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	switch( m_ReserveNext )
	{
	case RN_HIDE:
		m_ItemState = FIS_HIDE;
		m_ReserveNext = RN_NONE;

		SetTargetAlphaRate( 0 );

		if( pItemEffect )
			pItemEffect->EndEffectForce();

		if( pPickEnableEffect )
			pPickEnableEffect->EndEffectForce();

		if( pDropEffect )
			pDropEffect->EndEffectForce();

		D3DXVECTOR3 vCurPos = GetWorldPosition();
		float fRealHeight = GetBottomHeight() + GetCollisionBox().GetExtents(1);

		vCurPos.y = fRealHeight;
		SetWorldPosition( vCurPos );

		m_vPrePos = vCurPos;
		m_dwSetTime = 0;
		return;
	}

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fCurSpeed = m_fMoveSpeed * fTimePerSec;
	vPos = vPos + m_vMoveDir * fCurSpeed;

	vPos.y += m_fDropPower * fTimePerSec;

	m_fCurGravityAmt += m_fDropFriction * fTimePerSec;
	vPos.y -= m_fCurGravityAmt * fTimePerSec;

	m_pGroup->GetParentNode()->Rotate( m_vAxis, (1500.0f*fTimePerSec) );

	float fMapHeight = GetBottomHeight() + GetCollisionBox().GetExtents(1);

	if( fMapHeight < 0.0f )
		fMapHeight = 0.0f;

	if( fMapHeight > vPos.y )
	{
		vPos.y = fMapHeight;
		SetWorldOrientation( ioMath::QUAT_IDENTITY );

		if( m_iCurAlphaRate != 0 )
		{
			m_ItemState = FIS_NORMAL;
			if( m_ReserveNext == RN_NONE )
			{
				CreateDropEffect();
				CreateItemEffect();
			}
		}
		else
		{
			m_ItemState = FIS_HIDE;
		}
	}

	SetWorldPosition( vPos );
}

void ioFieldItem::CreateItemEffect()
{
	DestroyItemEffect();
	DestroyPickEnableEffect();

	ioEffect *pItemEffect = NULL;
	if( m_pItem->HasFieldEffect() )
		pItemEffect = g_EffectFactory.CreateEffect( m_pItem->GetFieldEffect() );
	else
		pItemEffect = g_EffectFactory.CreateEffect( g_FieldItemMgr.GetItemEffect( m_TeamType ) );

	if( pItemEffect )
	{
		m_pGroup->GetParentSceneNode()->AttachObject( pItemEffect );
		m_ItemEffect = pItemEffect->GetUniqueID();
	}
}

void ioFieldItem::CreateDropEffect()
{
	DestroyItemEffect();
	DestroyDropEffect();
	DestroyPickEnableEffect();

	ioEffect *pDropEffect = NULL;
	if( m_pItem->HasDropEffect() )
		pDropEffect = g_EffectFactory.CreateEffect( m_pItem->GetDropEffect() );

	if( pDropEffect )
	{
		m_pGroup->GetParentSceneNode()->AttachObject( pDropEffect );
		m_CreateDropEffect = pDropEffect->GetUniqueID();
	}
}

void ioFieldItem::CreatePickEnableEffect()
{
	DestroyItemEffect();
	DestroyPickEnableEffect();

	ioEffect *pPickEnableEffect = NULL;
	if( m_pItem->HasPickEnableEffect() )
		pPickEnableEffect = g_EffectFactory.CreateEffect( m_pItem->GetPickEnableEffect() );
	else
		pPickEnableEffect = g_EffectFactory.CreateEffect( g_FieldItemMgr.GetItemOverEffect( m_TeamType ) );

	if( pPickEnableEffect )
	{
		m_pGroup->GetParentSceneNode()->AttachObject( pPickEnableEffect );
		m_PickEnableEffect = pPickEnableEffect->GetUniqueID();
	}
}

void ioFieldItem::DestroyItemEffect()
{
	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );

	if( pItemEffect )
	{
		m_pGroup->GetParentSceneNode()->DetachObject( pItemEffect );
		g_EffectFactory.DestroyEffect( pItemEffect );
		pItemEffect = NULL;
		m_ItemEffect = -1;
	}
}

void ioFieldItem::DestroyDropEffect()
{
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	if( pDropEffect )
	{
		m_pGroup->GetParentSceneNode()->DetachObject( pDropEffect );
		g_EffectFactory.DestroyEffect( pDropEffect );
		pDropEffect = NULL;
		m_CreateDropEffect = -1;
	}
}

void ioFieldItem::DestroyPickEnableEffect()
{
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );

	if( pPickEnableEffect )
	{
		m_pGroup->GetParentSceneNode()->DetachObject( pPickEnableEffect );
		g_EffectFactory.DestroyEffect( pPickEnableEffect );
		pPickEnableEffect = NULL;
		m_PickEnableEffect = -1;
	}
}

void ioFieldItem::ReserveHiding()
{
	m_ReserveNext = RN_HIDE;
}

void ioFieldItem::ReserveDropHiding()
{
	m_ReserveNext = RN_DROP_HIDE;
}

void ioFieldItem::ReserveRePosition( const D3DXVECTOR3 &vPos )
{
	m_ReserveNext = RN_REPOSITION;
	m_vReservePos = vPos;
	m_bWaitReposition = false;
}

void ioFieldItem::SetDropState( const D3DXVECTOR3 &vStartPos )
{
	if( m_ItemState == FIS_DROP || m_ItemState == FIS_HIDE || m_ItemState == FIS_DESTROY )
		return;

	SetWorldPosition( vStartPos );

	m_ItemState = FIS_DROP;
	m_fDropPower = 800.0f;
	m_fDropFriction = 3000.0f;

	m_dwDropTime = FRAMEGETTIME();

	DestroyItemEffect();
	DestroyPickEnableEffect();
}

void ioFieldItem::SetMoveDropState( const ioHashString &szAttacker,
								    const ioHashString &szSkillName,
								    const D3DXVECTOR3 &vStartPos,
								    const D3DXVECTOR3 &vTargetPos,
								    float fMoveSpeed )
{
	if( m_ItemState == FIS_DROP || m_ItemState == FIS_HIDE || m_ItemState == FIS_DESTROY )
		return;

	m_szAttackChar = szAttacker;
	m_szMoveSkillName = szSkillName;

	CheckMoveDropTime( vStartPos, vTargetPos );

	m_ItemState = FIS_MOVE;
	m_dwDropTime = FRAMEGETTIME();

	DestroyItemEffect();
	DestroyPickEnableEffect();
}

void ioFieldItem::CheckMoveDropTime( const D3DXVECTOR3 &vStartPos, const D3DXVECTOR3 &vTargetPos )
{
	m_fCurGravityAmt = 0.0f;
	m_fMoveSpeed = 0.0f;

	D3DXVECTOR3 vDiff = vTargetPos - vStartPos;
	vDiff.y = 0.0f;
	
	float fDistance = D3DXVec3Length( &vDiff );
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	m_fDropFriction = g_FieldItemMgr.GetItemGravityAmt();
	m_fDropPower = g_FieldItemMgr.GetItemJumpAmt();

	fTopTime = m_fDropPower / m_fDropFriction;
	fTopHeight = (m_fDropPower*fTopTime) - (0.5f*m_fDropFriction*fTopTime*fTopTime);

	float fResult = 2 * fTopTime;
	if( fResult > 0.0f )
		m_fMoveSpeed = fDistance / fResult;

	D3DXVec3Normalize( &m_vMoveDir, &vDiff );

	if( m_vMoveDir == ioMath::VEC3_ZERO )
	{
		m_vMoveDir = ioMath::UNIT_X;
		m_fMoveSpeed = 0.0f;
	}

	D3DXVec3Cross( &m_vAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );

	SetWorldPosition( vStartPos );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetWorldOrientation( qtRot );
}

ioItem* ioFieldItem::ReleaseItem()
{
	ioItem *pItem = m_pItem;

	m_pItem = NULL;
	m_ItemState = FIS_DESTROY;

	return pItem;
}

bool ioFieldItem::IsNeedDestroy() const
{
	if( !m_pItem || m_ItemState == FIS_DESTROY )
		return true;

	return false;
}

bool ioFieldItem::IsNeedDestroyByTime() const
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwCreateTime;

	if( dwGapTime > 60000 )
		return true;

	return false;
}

bool ioFieldItem::IsCanPicked() const
{
	if( !m_pItem )	return false;
	if( m_ItemState != FIS_NORMAL )	return false;
	if( m_ReserveNext != RN_NONE )	return false;

	return true;
}

bool ioFieldItem::IsDropping() const
{
	if( m_ItemState == FIS_DROP )
		return true;

	return false;
}

void ioFieldItem::SetOwnerCharOver( bool bOver )
{
	if( m_bOwnerCharOver != bOver )
	{
		m_bOwnerCharOver = bOver;

		if( m_bOwnerCharOver )
		{
			CreatePickEnableEffect();
		}
		else
		{
			CreateItemEffect();
		}
	}
}

void ioFieldItem::SetWorldPositionByServerSide( const D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3 vNewPos = vPos;

	if( GetCrownItemType() != ioItem::MCT_NONE || vPos.y == 0.0f )
	{
		vNewPos.y = m_pModeParent->GetMapHeight( vPos.x, vPos.z, this, false );
	}

	vNewPos.y += GetCollisionBox().GetExtents(1);
	SetWorldPosition( vNewPos );
}

int ioFieldItem::GetItemCreateIndex() const
{
	if( m_pItem )
		return m_pItem->GetItemCreateIndex();

	return 0;
}

const ioHashString& ioFieldItem::GetItemName() const
{
	return m_pItem->GetName();
}

ioItem::ItemType ioFieldItem::GetItemType() const
{
	if( m_pItem )
		return m_pItem->GetType();

	return ioItem::IT_NONE;
}

bool ioFieldItem::HasOwnerName() const
{
	if( m_pItem )
		return m_pItem->HasOwnerName();

	return false;
}

void ioFieldItem::ClearItemOwnerByOutUser( const ioHashString &rkName )
{
	if( m_pItem )
	{
		m_pItem->ClearIfOwnerName( rkName );
	}
}

void ioFieldItem::ReleaseFieldItemByIndex( int itemIndex )
{
	if( m_pItem )
	{
		if( m_pItem->GetItemCreateIndex() == itemIndex )
		{
			ioItem *pItem = ReleaseItem();
			SAFEDELETE( pItem );
		}
	}
}

void ioFieldItem::LoadResource()
{
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pItemBack );

	if( !m_pItem || m_pItem->GetItemLargeIconName().IsEmpty() )
		return;

	m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName( m_pItem->GetItemLargeIconName() );
	if( m_pItemIcon )
	{
		m_pItemIcon->SetScale( FLOAT05 );
	}

	m_pItemBack = g_UIImageSetMgr.CreateImage( "UIIconPack1", "item_icon_back" );
	if( m_pItemBack )
	{
		m_pItemBack->SetScale( FLOAT05 );
	}
}

void ioFieldItem::RenderItemIcon( float fXPos, float fYPos, float fScale )
{
	fXPos = (float)(int)fXPos;
	fYPos = (float)(int)fYPos;
	
	if( m_pItemBack )
	{
		m_pItemBack->Render( fXPos - 26.0f, fYPos - 26.0f );
	}

	if( m_pItemIcon )
	{
		m_pItemIcon->Render( fXPos - 26.0f, fYPos - 26.0f );
	}
}

void ioFieldItem::SetLastPushChar( const ioHashString &szCharName )
{
	if( !szCharName.IsEmpty() && m_LastPushChar != szCharName )
	{
		m_LastPushChar = szCharName;
	}
}

void ioFieldItem::SetDropChar( const ioHashString &szCharName )
{
	if( !szCharName.IsEmpty() && m_DropChar != szCharName )
	{
		m_DropChar = szCharName;
	}
}

void ioFieldItem::DropZoneCheck()
{
	if( m_bWaitReposition )	return;

	if( !P2PNetwork::IsNetworkPlaying() || !m_pModeParent->IsNetworkMode() )
		return;

	if( m_pModeParent )
	{
		ioPlayStage *pStage = GetCreator();
		if( pStage && m_pModeParent->GetModeType() == MT_FIGHT_CLUB )
		{
			D3DXVECTOR3 vPos = GetWorldPosition();
			if( !pStage->CheckAxisLimit( vPos ) && GetBottomHeight() > 0.0f )
				return;
		}
		else
		{
			if( !IsFlagItem() && GetCrownItemType() == ioItem::MCT_NONE && m_pModeParent->GetModeType() != MT_BATTLE )
				return;
			
			if( GetBottomHeight() > 0.0f )
				return;
		}
	}

	if( !m_LastPushChar.IsEmpty() )
	{
		if( m_pModeParent->GetOwnerName() != m_LastPushChar )
			return;
	}
	else
	{
		if( !m_DropChar.IsEmpty() )
		{
			if( m_DropChar != m_pModeParent->GetOwnerName() )
				return;
		}
		else
		{
			if( m_LastPushChar.IsEmpty() && !P2PNetwork::IsHost() )
				return;
		}
	}

	SP2Packet kPacket( CTPK_REPOSITION_FIELDITEM );
	kPacket << GetItemCreateIndex();
	TCPNetwork::SendToServer( kPacket );
	m_bWaitReposition = true;
}

void ioFieldItem::CheckDropChar()
{
	if( m_DropChar.IsEmpty() ) return;

	ioBaseChar *pChar = GetCreator()->GetBaseChar( m_DropChar );
	if( pChar )
	{
		if( m_dwDropTime != 0 && FRAMEGETTIME()-m_dwDropTime > 3000 )
		{
			m_DropChar.Clear();
			m_dwDropTime = 0;
		}
	}
	else
	{
		m_DropChar.Clear();
		m_dwDropTime = 0;
	}
}

void ioFieldItem::CheckLastPushChar()
{
	if( m_LastPushChar.IsEmpty() )
		return;

	ioBaseChar *pChar = GetCreator()->GetBaseChar( m_LastPushChar );
	if( !pChar )
	{
		m_LastPushChar.Clear();
	}
	else
	{
		if( m_ItemState == FIS_NORMAL )
		{
			if( m_dwSetTime != 0 && (FRAMEGETTIME() - m_dwSetTime) > 10000 )
			{
				m_LastPushChar.Clear();
				m_dwSetTime = 0;
			}
		}
	}
}

void ioFieldItem::SendFieldItemInfo()
{
	SP2Packet kPacket( CUPK_FIELDITEM_INFO );
	kPacket << GetItemCreateIndex();
	kPacket << GetWorldPosition();
	P2PNetwork::SendToAllPlayingUser( kPacket );

	GetCreator()->NotifyPushStructSendInfo();
}

void ioFieldItem::SetPushTime( DWORD dwPushTime )
{
	m_dwSetTime = dwPushTime;
}

int ioFieldItem::GetCrownItemType() const
{
	if( !m_pItem )
		return false;

	return m_pItem->GetCrownType();
}

TeamType ioFieldItem::GetItemTeamType()
{
	if( !m_pItem )
		return TEAM_NONE;

	return m_pItem->GetItemTeamType();
}

bool ioFieldItem::IsObjectItem() const
{
	if( ToObjectItem(m_pItem) )
		return true;

	return false;
}

bool ioFieldItem::IsFlagItem() const
{
	ioCloakWearItem *pCloakWear = ToCloakWearItem( m_pItem );
	if( !pCloakWear )
		return false;

	if( pCloakWear->GetCloakWearItemType() == ioCloakWearItem::CWT_FLAG )
		return true;

	return false;
}

bool ioFieldItem::MaintainItemWhileCharChange() const
{
	ioWearItem *pWearItem = ToWearItem( m_pItem );
	if( !pWearItem )
		return false;

	return pWearItem->CharCanChange();
}

DWORD ioFieldItem::GetEventGapTime()
{
	return (FRAMEGETTIME() - m_dwCreateTime);
}

void ioFieldItem::TranslateByWorldEntity( const D3DXVECTOR3 &vMove )
{
	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();

	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	if( pItemEffect )
		pItemEffect->TranslateParticle( vMove );

	if( pPickEnableEffect )
		pPickEnableEffect->TranslateParticle( vMove );

	if( pDropEffect )
		pDropEffect->TranslateParticle( vMove );
}

void ioFieldItem::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();

	ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_ItemEffect );
	ioEffect *pPickEnableEffect = g_EffectFactory.GetEffect( m_PickEnableEffect );
	ioEffect *pDropEffect = g_EffectFactory.GetEffect( m_CreateDropEffect );

	if( pItemEffect )
		pItemEffect->TranslateParticle( vMove );

	if( pPickEnableEffect )
		pPickEnableEffect->TranslateParticle( vMove );

	if( pDropEffect )
		pDropEffect->TranslateParticle( vMove );
}

int ioFieldItem::GetCharOnlySexType()
{
	int iOnlySexType = ioSetItemInfo::CST_ALL;
	
	if( m_pItem )
	{
		iOnlySexType = m_pItem->GetCharOnlySexType();
	}

	return iOnlySexType;
}

bool ioFieldItem::CheckTeamType( TeamType eTeam )
{
	if( m_TeamType == TEAM_NONE || m_TeamType == eTeam )
		return true;

	return false;
}

void ioFieldItem::SetTeamType( TeamType eTeam )
{
	m_TeamType = eTeam;
}

ioHashString ioFieldItem::GetDeleteFieldItemAni()
{
	if( !m_pItem || !IsObjectItem() )
		return ioHashString();

	return ToObjectItem( m_pItem )->GetDeleteFieldItemAni();
}