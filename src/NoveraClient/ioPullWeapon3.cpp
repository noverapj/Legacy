
#include "stdafx.h"

#include "ioPullWeapon3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "BuffDefine.h"

#include "WeaponDefine.h"

ActionStopType ioPullWeapon3::m_ActionStopType = AST_NONE;
ActionStopType ioPullWeapon3::m_DieActionStopType = AST_NONE;

ioPullWeapon3::ioPullWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pMesh = NULL;
	m_State = CS_PRE;

	m_fCurMoveRange = 0;

	m_dwCurMapCollisionDelayTime = 0;

	m_bArleadyCallWeapon = false;

	m_vStartPos = ioMath::VEC3_ZERO;
	m_vTargetList.clear();
}

ioPullWeapon3::~ioPullWeapon3()
{
	if( m_pMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioPullWeapon3::~ioPullWeapon3 - UnReleased Resouce" );
	}
}

bool ioPullWeapon3::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioPullWeapon3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "map_collision_effect", "", szBuf, MAX_PATH );
	m_szMapCollisionEffect = szBuf;
	m_dwMapCollisionDelayTime = rkLoader.LoadFloat_e( "map_collision_effect_delay", 500 );

	m_fExtraOffsetX = rkLoader.LoadFloat_e( "extra_offset_x", 0.0f );
	m_fExtraOffsetY = rkLoader.LoadFloat_e( "extra_offset_y", 0.0f );
	m_fExtraOffsetZ = rkLoader.LoadFloat_e( "extra_offset_z", 0.0f );

	m_fAngleH = rkLoader.LoadFloat_e( "angle_h", 0.0f );
	m_fAngleV = rkLoader.LoadFloat_e( "angle_v", 0.0f );
	
	// 끝까지 끌려간 후 걸어줄 버프
	int iCnt = rkLoader.LoadInt_e( "target_wound_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i)
	{
		wsprintf_e( szKey, "target_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szTargetWoundBuff.push_back( szBuf );
	}

	LoadCallAttributeList( rkLoader );
}
void ioPullWeapon3::LoadCallAttributeList( ioINILoader &rkLoader )
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

bool ioPullWeapon3::SetMesh( const ioHashString &szFileName )
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

void ioPullWeapon3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMesh )
	{
		g_EffectFactory.DestroyEffect( m_pMesh );
		m_pMesh = NULL;
	}
}

const ioOrientBox& ioPullWeapon3::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioPullWeapon3::GetType() const
{
	return WT_PULL_WEAPON3;
}

ActionStopType ioPullWeapon3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioPullWeapon3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioPullWeapon3::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	RotateMoveDir( m_fAngleH, m_fAngleV );

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRot );
	SetOrientation( qtRot );
}

void ioPullWeapon3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pMesh && m_pMesh->GetSkeletonName().IsEmpty() && m_pMesh->HasLinkedSystem() )
		return;

	D3DXVECTOR3 vNewPos = vPos;
	if( m_fExtraOffsetX != 0.0f || m_fExtraOffsetY != 0.0f || m_fExtraOffsetZ != 0.0f )
	{
		ioBaseChar *pOwner = GetOwner();
		if( m_pMesh && pOwner )
		{
			vNewPos = pOwner->GetWorldPosition();
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();

			D3DXVECTOR3 vZDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vZDir, &vZDir );

			D3DXVECTOR3 vXDir;
			D3DXVec3Cross( &vXDir, &vZDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vXDir, &vXDir );

			D3DXVECTOR3 vYDir;
			D3DXVec3Cross( &vYDir, &vXDir, &vZDir );
			D3DXVec3Normalize( &vYDir, &vYDir );

			vNewPos += m_fExtraOffsetX * vXDir;
			vNewPos += m_fExtraOffsetY * vYDir;
			vNewPos += m_fExtraOffsetZ * vZDir;
		}
	}

	SetPosition( vNewPos );
	m_vStartPos = GetPosition();
}

void ioPullWeapon3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case CS_PRE:
		{
			m_State = CS_MOVE;
		}
		break;
	case CS_MOVE:
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

			UpdateWorldAttackBox( fTimePerSec, false );
		}
		break;
	case CS_END:
		SetTargetBuff( pStage );
		SetWeaponDead();
	}

	CallAttribute();
}

bool ioPullWeapon3::IsEnableTargetState( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pTarget )
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

void ioPullWeapon3::SetTargetBuff( ioPlayStage* pStage )
{
	if( !pStage )
		return;

	int iCharCnt = (int)m_vTargetList.size();
	for( int i=0; i < iCharCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_vTargetList[i] );
		if( !pTarget ) continue;

		int iBuffCnt = (int)m_szTargetWoundBuff.size();
		for( int j=0; j < iBuffCnt; ++j )
		{
			pTarget->AddNewBuff( m_szTargetWoundBuff[j], GetOwner()->GetCharName(), "", NULL );
		}
	}
}

float ioPullWeapon3::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioPullWeapon3::GetMoveRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

bool ioPullWeapon3::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
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

void ioPullWeapon3::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioPullWeapon3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if ( m_State == CS_PRE )
		return false;

	if( !pEntity )
		return false;

	if( !ToBaseChar(pEntity) )
		return false;

	if( !ToBaseChar(pEntity)->IsNeedProcess() )
		return false;



	bool bCollision = false;
	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if ( !pTarget )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if ( !IsEnableTargetState( pOwner, pTarget ) )
			return false;
	}	

	CheckPiercing( bCollision );

	if( bCollision )
	{
		m_vTargetList.push_back( pTarget->GetCharName() );

		if( pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << pTarget->GetCharName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return true;
		
	}

	return bCollision;
}

void ioPullWeapon3::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{

}

void ioPullWeapon3::CreateMapCollisionEffect( ioPlayStage* pStage, D3DXVECTOR3 vColPoint )
{
	pStage->CreateMapEffect( m_szMapCollisionEffect, vColPoint, GetScale() );
}

void ioPullWeapon3::CallAttribute()
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

void ioPullWeapon3::RotateMoveDir( float fAngleH, float fAngleV )
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

	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	//ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	//SetOrientation( qtRot );
}

D3DXVECTOR3 ioPullWeapon3::GetMoveDir()
{
	return m_vMoveDir;
}