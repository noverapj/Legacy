

#include "stdafx.h"

#include "ioChainWeapon.h"

#include "ioBaseChar.h"

ActionStopType ioChainWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioChainWeapon::m_DieActionStopType = AST_NONE;

ioChainWeapon::ioChainWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;

	m_ChainState = CWS_NONE;
	m_fCurMoveSpeed = 0.0f;
	m_fCurChainRange = 0.0f;
	m_iCurChainCnt = 0;

	m_fCurMoveDistance = 0.0f;
	m_dwCollisionTime = 0;

	m_vChainInfoList.clear();

	m_bFirst = true;
	m_vMoveDir = ioMath::VEC3_ZERO;
}

ioChainWeapon::~ioChainWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioMissileWeapon::~ioMissileWeapon - UnReleased Resouce" );
	}

	m_vChainInfoList.clear();
}

bool ioChainWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioChainWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_bDisableDownTarget = rkLoader.LoadBool_e( "disable_down_target", false );
	m_fChainAngle = rkLoader.LoadFloat_e( "chain_angle", 0.0f );

	m_iMaxChainCnt = (int)rkLoader.LoadInt_e( "max_chain_cnt", 0 );

	if( m_iMaxChainCnt <= 0 ) return;

	m_vChainInfoList.clear();
	m_vChainInfoList.reserve( m_iMaxChainCnt );

	for( int i=0; i < m_iMaxChainCnt; ++i )
	{
		wsprintf_e( szBuf, "chain_speed%d", i+1 );
		float fSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "chain_range%d", i+1 );
		float fRange = rkLoader.LoadFloat( szBuf, 0.0f );

		ChainInfo kInfo;
		kInfo.m_fRange = fRange;
		kInfo.m_fSpeed = fSpeed;

		m_vChainInfoList.push_back( kInfo );
	}

	int iCnt = rkLoader.LoadInt_e( "ignore_chain_count", 0 );
	m_iIgnoreCountList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "ignore_chain%d", i+1 );
		int iChainCount = rkLoader.LoadInt( szKey, -1 );
		m_iIgnoreCountList.push_back( iChainCount-1 );
	}
	m_fLimitLengthToOwner = rkLoader.LoadFloat_e( "limit_length_to_owner", 0.0f );
	m_bWeaponInvisible = rkLoader.LoadBool_e( "invisible_weapon", false );
}

bool ioChainWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_StartPos = GetPosition();
	
	if( !m_vChainInfoList.empty() )
	{
		m_fCurMoveSpeed = m_vChainInfoList[0].m_fSpeed;
		m_fCurChainRange = m_vChainInfoList[0].m_fRange;
	}
	else
	{
		m_fCurMoveSpeed = 0.0f;
		m_fCurChainRange = 0.0f;
	}

	m_ChainState = CWS_MOVE;
	AddEffectEvent( m_pFireBall );
	if( m_bWeaponInvisible )
		m_pFireBall->SetVisible( false );

	return true;
}

void ioChainWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioChainWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioChainWeapon::ChangeChainWeapon( int iCurCnt, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vMoveDir )
{
	if( !COMPARE( iCurCnt, 0, m_iMaxChainCnt ) )
	{
		m_ChainState = CWS_NONE;
		SetWeaponDead();
		return;
	}

	int iCnt = m_vChainInfoList.size();
	if( !COMPARE( iCurCnt, 0, iCnt ) )
	{
		m_ChainState = CWS_NONE;
		SetWeaponDead();
		return;
	}

	m_iCurChainCnt = iCurCnt;
	m_fCurMoveSpeed = m_vChainInfoList[iCurCnt].m_fSpeed;
	m_fCurChainRange = m_vChainInfoList[iCurCnt].m_fRange;

	m_StartPos = vPos;
	SetPosition( m_StartPos );
	SetMoveDir( vMoveDir );

	m_ChainState = CWS_MOVE;
	m_fCurMoveDistance = 0.0f;
}

void ioChainWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_iCurChainCnt == 0 )
	{
		SetPosition( vPos );
		m_StartPos = GetPosition();
	}
}

void ioChainWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	if( GetOwner() && m_fLimitLengthToOwner > 0.0f )
	{
		D3DXVECTOR3 vOwnerPos = GetOwner()->GetMidPositionByRate();
		D3DXVECTOR3 vTargetPos = GetPosition();
		D3DXVECTOR3 vDiff = vTargetPos - vOwnerPos;
		float fLength = D3DXVec3Length( &vDiff );
		if( fLength > m_fLimitLengthToOwner )
		{
			SetWeaponDead();
		}
	}

	if( !m_bFirst )
	{
		float fMoveDist = GetMoveSpeed() * fTimePerSec;

		if( m_ChainState == CWS_MOVE )
		{
			m_fCurMoveDistance += fMoveDist;
			GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
			UpdateWorldAttackBox( fTimePerSec, false );
		}
		else
		{
			UpdateWorldAttackBox( fTimePerSec, true );

			if( m_ChainState == CWS_COLLISION )
			{
				if( m_dwCollisionTime+200 < FRAMEGETTIME() )
				{
					SetWaitState();
				}
			}

			if( !m_pFireBall || !m_pFireBall->IsLive() )
			{
				SetWeaponDead();
			}
		}
	}
	else
	{
		UpdateWorldAttackBox( fTimePerSec, false );
	}

	if( IsLive() && m_fCurMoveDistance >= m_fCurChainRange )
	{
		SetWaitState();
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioChainWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioChainWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioChainWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioChainWeapon::GetType() const
{
	return WT_CHAIN;
}

ActionStopType ioChainWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioChainWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioChainWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioChainWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(),
									GetScale(),
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  GetScale(),
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

bool ioChainWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_ChainState != CWS_MOVE )
	{
		return false;
	}

	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
	{
		bCollision = CheckCollisionLine( pEntity, vColPoint );
	}

	CheckPiercing( bCollision );

	if( bCollision )
	{
		m_ChainState = CWS_COLLISION;
		m_dwCollisionTime = FRAMEGETTIME();
		m_fCurMoveDistance = 0.0f;
	}

	if( bCollision )
	{
		ioBaseChar *pTarget = ToBaseChar( pEntity );
		if( pTarget )
		{
			bool bCheck = false;
			int iCnt = m_iIgnoreCountList.size();
			for( int i=0; i<iCnt; ++i )
			{
				if( m_iIgnoreCountList[i] != -1 && m_iCurChainCnt == m_iIgnoreCountList[i] )
					bCheck = true;
			}
			
			if( bCheck )
			{			
				AddCollisionEntity( pEntity );
				pTarget->CheckChainWeapon( this );
				bCollision = false;
			}
		}
	}

	return bCollision;
}

bool ioChainWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( m_StartPos, vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

float ioChainWeapon::GetNextChainRange()
{
	int iNextCnt = m_iCurChainCnt + 1;

	if( !COMPARE( iNextCnt, 0, m_iMaxChainCnt ) )
		return 0.0f;

	int iCnt = m_vChainInfoList.size();
	if( !COMPARE( iNextCnt, 0, iCnt ) )
		return 0.0f;

	return m_vChainInfoList[iNextCnt].m_fRange;
}

void ioChainWeapon::SetWaitState()
{
	if( m_ChainState == CWS_WAIT )
		return;

	m_ChainState = CWS_WAIT;

	if( m_pFireBall )
	{
		if( m_bEndEffectGrace )
			m_pFireBall->EndEffectGrace();
		else
			m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
}
