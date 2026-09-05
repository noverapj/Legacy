

#include "stdafx.h"

#include "ioPlayMode.h"
#include "ioPlayStage.h"
#include "ioFieldRewardItem.h"
#include "ioBaseChar.h"

ioFieldRewardItem::ioFieldRewardItem( ioEntityGroup *pGrp, ioPlayMode *pMode, int iFieldRewardUniqueIndex, int iFieldRewardResourceIndex ) : ioPlayEntity( pGrp, pMode )
{
	m_ItemEffect = -1;
	m_PickEnableEffect = -1;
	m_CreateDropEffect = -1;

	m_ItemState = FIS_NORMAL;
	m_ReserveNext = RN_NONE;

	m_fDropPower = 300.0f;
	m_fDropFriction = 150.0f;

	m_pItemIcon = NULL;
	m_pItemBack = NULL;
	
	m_bOwnerCharOver = false;

	m_vPrePos.x = 0.0f;
	m_vPrePos.y = 0.0f;
	m_vPrePos.z = 0.0f;
	m_dwDropTime = 0;

	m_fMoveSpeed = 0.0f;
	m_bDroppingEndHide = false;

	m_iFieldRewardUniqueIndex   = iFieldRewardUniqueIndex;
	m_iFieldRewardResourceIndex = iFieldRewardResourceIndex;

	m_dwCreateTime = FRAMEGETTIME();
}

ioFieldRewardItem::~ioFieldRewardItem()
{
	DestroyItemEffect();
	DestroyDropEffect();
	DestroyPickEnableEffect();

	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pItemBack );
}

void ioFieldRewardItem::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
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

void ioFieldRewardItem::UpdateAfter( float fFrameGap )
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

bool ioFieldRewardItem::IsNeedCheckMapCollision() const
{
	return true;
}

void ioFieldRewardItem::DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt )
{
	if( !ToBaseChar( pPusher ) )
	{
		Translate( vMoveAmt );
	}
}

void ioFieldRewardItem::UpdateBound() const
{
	m_WorldColBox = GetCollisionBox();
	m_WorldColBox.Transform( GetWorldPosition(),
							 GetWorldScale(),
							 D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );

	m_WorldAxisBox.SetByOrientBox( m_WorldColBox );
}

void ioFieldRewardItem::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

ApplyWeaponExp ioFieldRewardItem::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioFieldRewardItem::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

bool ioFieldRewardItem::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	return false;
}

ioPlayEntity::PlaySubType ioFieldRewardItem::GetSubType() const
{
	return PST_FIELD_REWARD_ITEM;
}

ArmorType ioFieldRewardItem::GetArmorType() const
{
	return AT_UNARMOR;
}

ioWorldEventReceiver* ioFieldRewardItem::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

void ioFieldRewardItem::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	SetDropState( vTargetPos );
}

void ioFieldRewardItem::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
	// No Damage..
}

void ioFieldRewardItem::OnPoisonZoneDamage()
{
}

ioPlayStage* ioFieldRewardItem::GetCreator() const
{
	return m_pModeParent->GetCreator();
}

void ioFieldRewardItem::ProcessNormal()
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
		SetWorldPosition( vCurPos );
		m_vPrePos = vCurPos;
	}
}

void ioFieldRewardItem::ProcessDropping( float fTimePerSec )
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

void ioFieldRewardItem::ProcessHidding()
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

void ioFieldRewardItem::ProcessMoving( float fTimePerSec )
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
		return;
	}

	if( !CheckEnableMove() )
	{
		m_ItemState = FIS_DROP;
		m_fDropPower = 0.0f;
		m_fDropFriction = FLOAT1000;

		return;
	}

	D3DXVECTOR3 vPos = GetWorldPosition();
	D3DXVECTOR3 vDir = m_vTargetPos - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurSpeed = m_fMoveSpeed * fTimePerSec;
	vPos = vPos + vDir * fCurSpeed;

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
}

void ioFieldRewardItem::CreateItemEffect()
{
	DestroyItemEffect();
	DestroyPickEnableEffect();

	// 획득 가능할 경우에만 표시
	if( GetOwnerCharName().IsEmpty() || GetOwnerCharName() == g_MyInfo.GetPublicID() )
	{
		ioEffect *pItemEffect = g_EffectFactory.CreateEffect( "sp2_default_field_item.txt" );
		if( pItemEffect )
		{
			m_pGroup->GetParentSceneNode()->AttachObject( pItemEffect );
			m_ItemEffect = pItemEffect->GetUniqueID();
		}
	}
}

void ioFieldRewardItem::CreateDropEffect()
{
	DestroyItemEffect();
	DestroyDropEffect();
	DestroyPickEnableEffect();

	// 필요 없음.	
}

void ioFieldRewardItem::CreatePickEnableEffect()
{
	DestroyItemEffect();
	DestroyPickEnableEffect();

	// 획득 가능할 경우에만 표시
	if( GetOwnerCharName().IsEmpty() || GetOwnerCharName() == g_MyInfo.GetPublicID() )
	{
		ioEffect *pPickEnableEffect = g_EffectFactory.CreateEffect( "sp2_default_field_item_over.txt" );
		if( pPickEnableEffect )
		{
			m_pGroup->GetParentSceneNode()->AttachObject( pPickEnableEffect );
			m_PickEnableEffect = pPickEnableEffect->GetUniqueID();
		}
	}
}

void ioFieldRewardItem::DestroyItemEffect()
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

void ioFieldRewardItem::DestroyDropEffect()
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

void ioFieldRewardItem::DestroyPickEnableEffect()
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

void ioFieldRewardItem::ReserveHiding()
{
	m_ReserveNext = RN_HIDE;
}

void ioFieldRewardItem::ReserveDropHiding()
{
	m_ReserveNext = RN_DROP_HIDE;
}

void ioFieldRewardItem::SetDestroy()
{
	m_ItemState = FIS_DESTROY;
}

void ioFieldRewardItem::SetDropState( const D3DXVECTOR3 &vStartPos )
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

void ioFieldRewardItem::SetMoveDropState( const D3DXVECTOR3 &vStartPos, const D3DXVECTOR3 &vTargetPos, float fMoveSpeed )
{
	if( m_ItemState == FIS_DROP || m_ItemState == FIS_HIDE || m_ItemState == FIS_DESTROY )
		return;

	m_vTargetPos = vTargetPos;
	m_fMoveSpeed = fMoveSpeed;

	SetWorldPosition( vStartPos );

	m_ItemState = FIS_MOVE;
	m_dwDropTime = FRAMEGETTIME();

	DestroyItemEffect();
	DestroyPickEnableEffect();
}

bool ioFieldRewardItem::IsNeedDestroy() const
{
	if( m_ItemState == FIS_DESTROY )
		return true;

	return false;
}

bool ioFieldRewardItem::IsCanPicked() const
{
	if( m_ItemState != FIS_NORMAL )	return false;
	if( m_ReserveNext != RN_NONE )	return false;

	return true;
}

bool ioFieldRewardItem::IsDropping() const
{
	if( m_ItemState == FIS_DROP )
		return true;

	return false;
}

void ioFieldRewardItem::SetOwnerCharName( const ioHashString &rkOwnerName )
{
	m_szOwnerName = rkOwnerName;
	if( m_bOwnerCharOver )
	{
		CreatePickEnableEffect();
	}
	else
	{
		CreateItemEffect();
	}
}

void ioFieldRewardItem::SetOwnerCharOver( bool bOver )
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

void ioFieldRewardItem::SetWorldPositionByServerSide( const D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3 vNewPos = vPos;

	if( vPos.y == 0.0f )
	{
		vNewPos.y = m_pModeParent->GetMapHeight( vPos.x, vPos.z, this, false );
	}

	vNewPos.y += GetCollisionBox().GetExtents(1);
	SetWorldPosition( vNewPos );
}

void ioFieldRewardItem::LoadResource()
{
	SAFEDELETE( m_pItemIcon );
	SAFEDELETE( m_pItemBack );

	m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName( g_FieldRewardItemMgr.GetIconName( m_iFieldRewardResourceIndex ) );	// 보상 아이콘
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

void ioFieldRewardItem::RenderItemIcon( float fXPos, float fYPos, float fScale )
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

void ioFieldRewardItem::TranslateByWorldEntity( const D3DXVECTOR3 &vMove )
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

void ioFieldRewardItem::TranslateByConveyer( const D3DXVECTOR3 &vMove )
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

bool ioFieldRewardItem::CheckEnableMove()
{
	ioPlayStage *pStage = GetCreator();
	if( !pStage ) return false;

	// Position Check
	D3DXVECTOR3 vPos = GetWorldPosition();
	bool bCheck = pStage->CheckCollisionLine( vPos, m_vTargetPos );

	return bCheck;
}