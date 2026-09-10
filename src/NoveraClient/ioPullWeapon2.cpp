

#include "stdafx.h"

#include "ioPullWeapon2.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "BuffDefine.h"

#include "WeaponDefine.h"

ActionStopType ioPullWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioPullWeapon2::m_DieActionStopType = AST_NONE;

ioPullWeapon2::ioPullWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pMesh = NULL;
	m_State = CS_PRE;
	
	m_fCurMoveRange = 0;

	m_dwCurMapCollisionDelayTime = 0;
	m_dwPullingEndTime = 0;
	m_dwPullingStartTime = 0;

	m_bArleadyCallWeapon = false;

	m_vStartPos = ioMath::VEC3_ZERO;
}

ioPullWeapon2::~ioPullWeapon2()
{
	if( m_pMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioPullWeapon2::~ioPullWeapon2 - UnReleased Resouce" );
	}
}

bool ioPullWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioPullWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "map_collision_effect", "", szBuf, MAX_PATH );
	m_szMapCollisionEffect = szBuf;
	m_dwMapCollisionDelayTime = rkLoader.LoadFloat_e( "map_collision_effect_delay", 500 );

	m_dwPullingDurationTime = rkLoader.LoadInt_e( "pulling_duration", 1000 );
	m_dwPullingSensingTime = rkLoader.LoadInt_e( "pulling_sensing_time", 100 );

	m_fPullingRange = rkLoader.LoadFloat_e( "pulling_range", FLOAT100 );

	rkLoader.LoadString_e( "pulling_buff", "", szBuf, MAX_PATH );
	m_szPullingBuff = szBuf;
		
	LoadCallAttributeList( rkLoader );
}
void ioPullWeapon2::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}
}

bool ioPullWeapon2::SetMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return false;

	m_pMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pMesh )	
		return false;

	GetParentSceneNode()->AttachObject( m_pMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );
	
	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	m_pMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pMesh->GetCollisionBox();

	AddEffectEvent( m_pMesh );
	return true;
}

void ioPullWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMesh )
	{
		g_EffectFactory.DestroyEffect( m_pMesh );
		m_pMesh = NULL;
	}
}

const ioOrientBox& ioPullWeapon2::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioPullWeapon2::GetType() const
{
	return WT_PULL_WEAPON2;
}

ActionStopType ioPullWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioPullWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioPullWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

// 	D3DXQUATERNION qtRot;
// 	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
// 	SetOrientation( qtRot );

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( vDir, qtRot );
	SetOrientation( qtRot );
}

void ioPullWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pMesh && m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vStartPos = vPos;
}

void ioPullWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case CS_PRE:
		{
			m_State = CS_FLOATING;
		}
		break;
	case CS_FLOATING:
		{
			OnFloating( fTimePerSec, dwCurTime, pStage );
		}
		break;
	case CS_PULLING:
		{
			OnPulling( fTimePerSec, dwCurTime, pStage );
		}
		break;
	case CS_END:
		SetWeaponDead();
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}


void ioPullWeapon2::OnFloating( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage )
{
	
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	if( GetMoveRange() < m_fCurMoveRange )
	{
		m_State = CS_END;
	}

	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioPullWeapon2::SetPullingState()
{
	if( m_State == CS_PULLING )
		return;

	m_dwPullingStartTime = FRAMEGETTIME();
	m_dwPullingEndTime = m_dwPullingStartTime;
	m_dwPullingEndTime += m_dwPullingDurationTime;

	m_State = CS_PULLING;	
}

void ioPullWeapon2::OnPulling( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage )
{
	CallAttribute();

	if( 0 < m_dwPullingEndTime && dwCurTime < m_dwPullingEndTime  )
	{		
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			if( dwCurTime < m_dwPullingStartTime + m_dwPullingSensingTime )
			{
				CheckTargetList( pOwner, pStage );
				SetTargetBuff( pOwner, pStage );
			}
		}
		else
		{
			SetWeaponDead();
		}
	}
	else if( m_dwPullingEndTime < dwCurTime )
	{
		SetWeaponDead();
	}

	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioPullWeapon2::CheckTargetList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner )
		return;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	m_vTargetList.clear();
	
	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !IsEnableTargetState( pOwner, pTarget ) )
			continue;

		if( !pTarget->IsNeedProcess() )
			continue;
		
		D3DXVECTOR3 vDiff = GetPosition() - pTarget->GetWorldPosition();

		float fDistSq = fabsf(D3DXVec3LengthSq( &vDiff ));
		float fRangeSq = m_fPullingRange  *m_fPullingRange;

		if( fDistSq < fRangeSq )
		{
			m_vTargetList.push_back( pTarget->GetCharName() );
		}
	}
}

bool ioPullWeapon2::IsEnableTargetState( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pOwner->GetTeam() == pTarget->GetTeam() )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )
		return false;

	if( pTarget->HasBuff( BT_CLOSEORDER ) )
		return false;

	if( pTarget->HasBuff( BT_SKIP_STUN ) )
		return false;
	
	if( pTarget->IsProtectState() )
		return false;
	
	ioPlayStage *pStage = pTarget->GetCreator();
	if( pStage )
	{
		ioPlayMode *pMode = pStage->GetPlayMode();
		if( pMode && pMode->GetModeType() == MT_FOOTBALL )
			return false;
	}
	else
	{
		return false;
	}

	return true;
}

void ioPullWeapon2::SetTargetBuff( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( m_vTargetList.empty() ) 
		return;

	if( !pOwner )
		return;

	ioHashStringVec::iterator iter;
	for( iter = m_vTargetList.begin(); iter != m_vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter) );
		if( !pTarget )
			continue;

		if( !pTarget->IsNeedProcess() )
			continue;

		ioBuff* pBuff = pTarget->AddNewBuff( m_szPullingBuff, pOwner->GetCharName(), pTarget->GetCharName(), NULL );
			
		ioCloseOrderBuff *pCloseOrder = ToCloseOrderBuff(pBuff);
		if( pCloseOrder && pCloseOrder->GetCloseOrderType() == ioCloseOrderBuff::COT_AREA_TRACE )
		{
			pCloseOrder->SetTargetWeapon( GetWeaponIndex() );
		}

 		if( pTarget->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff6 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pTarget->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << GetCreateItem();
			kPacket << pTarget->GetRandomSeed();
			kPacket << false;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << 0.0f;
			kPacket << 0.0f;

			kPacket << COWT_WEAPON;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << GetOwnerName();
			kPacket << GetWeaponIndex();		//타입에 따라 AreaWeapon 혹은 Weapon 인덱스 사용

			kPacket << 1;				
			kPacket << m_szPullingBuff;
			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

float ioPullWeapon2::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioPullWeapon2::GetMoveRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

bool ioPullWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCurMapCollisionDelayTime < dwCurTime )
	{
		if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
		{						
			D3DXVECTOR3 vColPoint;
			ioRay vRay( m_vStartPos, m_vMoveDir );		
			pShape->TestIntersection( NULL, vRay, NULL, &vColPoint );

			//벽 통과 이펙트 생성
			CreateMapCollisionEffect( pStage, vColPoint );
			m_dwCurMapCollisionDelayTime = dwCurTime + m_dwMapCollisionDelayTime;
			return true;
		}
	}

	return false;
}

void ioPullWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

bool ioPullWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	ioBaseChar* pTarget = ToBaseChar(pEntity);
	if( !pTarget )
		return false;
	
	if( m_State != CS_FLOATING || !pTarget->IsNeedProcess() )
		return false;

	bool bCollision = false;
	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	if( bCollision )
	{
		SetPullingState();

		if( pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bCollision;
}

void ioPullWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	SetPullingState();
}

void ioPullWeapon2::CreateMapCollisionEffect( ioPlayStage* pStage, D3DXVECTOR3 vColPoint )
{
	pStage->CreateMapEffect( m_szMapCollisionEffect, vColPoint, GetScale() );
}

void ioPullWeapon2::CallAttribute()
{	
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;

	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			pInfo,
			GetPosition(),
			D3DXVECTOR3(0.0f, FLOAT1, 0.0f),
			dwCurGenerateTime + pInfo.m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;	
}