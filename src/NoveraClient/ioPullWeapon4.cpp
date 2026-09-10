

#include "stdafx.h"

#include "ioPullWeapon4.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "BuffDefine.h"

#include "WeaponDefine.h"

ActionStopType ioPullWeapon4::m_ActionStopType		= AST_NONE;
ActionStopType ioPullWeapon4::m_DieActionStopType	= AST_NONE;

ioPullWeapon4::ioPullWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pMesh = NULL;
	m_State = CS_PRE;

	m_fCurMoveRange	= 0.0f;
	m_fCurMoveSpeed	= 0.0f;

	m_dwCurMapCollisionDelayTime = 0;
	m_bArleadyCallWeapon = false;

	m_vStartPos = ioMath::VEC3_ZERO;
}

ioPullWeapon4::~ioPullWeapon4()
{
	if( m_pMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioPullWeapon2::~ioPullWeapon2 - UnReleased Resouce" );
	}
}

bool ioPullWeapon4::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioPullWeapon4::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "map_collision_effect", "", szBuf, MAX_PATH );

	m_szMapCollisionEffect		= szBuf;
	m_dwMapCollisionDelayTime	= rkLoader.LoadFloat_e( "map_collision_effect_delay", 500 );
	m_fPullingRange				= rkLoader.LoadFloat_e( "pulling_range", FLOAT100 );
	m_bCollisionMap				= rkLoader.LoadBool_e( "check_collision_map", false );

	rkLoader.LoadString_e( "pulling_buff", "", szBuf, MAX_PATH );
	m_szPullingBuff = szBuf;

	LoadChangeDirection( rkLoader );
	LoadCallAttributeList( rkLoader );
}

void ioPullWeapon4::LoadChangeDirection( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

	m_ChangeDirInfoList.clear();
	m_ChangeDirInfoList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		ChangeDirInfo kInfo;

		wsprintf_e( szBuf, "change_range%d", i+1 );
		kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_speed%d", i+1 );
		kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_angle_h%d", i+1 );
		kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_angle_v%d", i+1 );
		kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

		m_ChangeDirInfoList.push_back( kInfo );
	}

	std::sort( m_ChangeDirInfoList.begin(), m_ChangeDirInfoList.end(), ChangeDirSort() );
}


void ioPullWeapon4::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

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

bool ioPullWeapon4::SetMesh( const ioHashString &szFileName )
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



void ioPullWeapon4::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMesh )
	{
		g_EffectFactory.DestroyEffect( m_pMesh );
		m_pMesh = NULL;
	}
}

const ioOrientBox& ioPullWeapon4::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioPullWeapon4::GetType() const
{
	return WT_PULL_WEAPON2;
}

ActionStopType ioPullWeapon4::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioPullWeapon4::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioPullWeapon4::SetMoveSpeed( float fSpeed )
{
	m_fCurMoveSpeed = fSpeed;
}

void ioPullWeapon4::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( vDir, qtRot );
	SetOrientation( qtRot );
}

void ioPullWeapon4::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pMesh && m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vStartPos = vPos;
}

void ioPullWeapon4::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() )
		return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioPullWeapon4::Process( float fTimePerSec, ioPlayStage *pStage )
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
	case CS_END:
		SetWeaponDead();
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioPullWeapon4::OnFloating( float fTimePerSec, DWORD dwCurTime, ioPlayStage *pStage )
{
	CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	if( GetMoveRange() < m_fCurMoveRange )
	{
		m_State = CS_END;
		CallAttribute();
	}

	if( m_pMesh )
	{
		m_pMesh->Update( fTimePerSec * FLOAT1000 );
	}

	CheckTargetList( GetOwner(), pStage );
	SetTargetBuff( GetOwner(), pStage );
}

void ioPullWeapon4::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	m_vMoveDir = qtRot * m_vMoveDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	m_vMoveDir = qtRot * m_vMoveDir;

	D3DXQUATERNION qtRotDir;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRotDir );
	SetOrientation( qtRotDir );
}

void ioPullWeapon4::CheckTargetList( ioBaseChar *pOwner, ioPlayStage *pStage )
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

bool ioPullWeapon4::IsEnableTargetState( ioBaseChar *pOwner, ioBaseChar *pTarget )
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

void ioPullWeapon4::SetTargetBuff( ioBaseChar *pOwner, ioPlayStage *pStage )
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

float ioPullWeapon4::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioPullWeapon4::GetMoveRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

bool ioPullWeapon4::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
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

			if( m_bCollisionMap )
			{
				CallAttribute();
				SetWeaponDead();
			}
			return true;
		}
	}

	return false;
}

void ioPullWeapon4::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioPullWeapon4::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	return false;
}

void ioPullWeapon4::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
}

void ioPullWeapon4::CreateMapCollisionEffect( ioPlayStage* pStage, D3DXVECTOR3 vColPoint )
{
	pStage->CreateMapEffect( m_szMapCollisionEffect, vColPoint, GetScale() );
}

void ioPullWeapon4::CallAttribute()
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