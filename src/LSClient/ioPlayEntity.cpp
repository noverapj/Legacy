

#include "stdafx.h"

#include "ioPlayEntity.h"
#include "ioZoneEffectWeapon.h"

ioPlayEntity::ioPlayEntity( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioGameEntity( pGrp )
{
	m_pModeParent = pMode;
	m_TeamType = TEAM_NONE;

	m_iGaugeXPos = 0;
	m_iGaugeYPos = 0;
	m_bGaugeVisible = false;
}

ioPlayEntity::~ioPlayEntity()
{
}

bool ioPlayEntity::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return false;
}

bool ioPlayEntity::IsCharCollisionSkipState( TeamType eTeamType, int iState ) const
{
	return false;
}

int ioPlayEntity::DontMoveEntityLevel() const
{
	return 0;
}

void ioPlayEntity::NotifyDontMoveCollisionPushingSkip( ioGameEntity* pColEntity )
{

}

void ioPlayEntity::LoadProperty( ioINILoader &rkLoader )
{
}

ioWorldEventReceiver* ioPlayEntity::GetWorldEventReceiver()
{
	return NULL;
}

bool ioPlayEntity::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	const ioOrientBox &rkWounded = GetWorldCollisionBox();
	if( ioMath::SphereTestLightly( rkAttackBox, rkWounded ) )
	{
		if( ioMath::TestIntersection( rkAttackBox, rkWounded, NULL ) )
			return true;
	}

	return false;
}

bool ioPlayEntity::CheckSphereDistance( const D3DXVECTOR3 &vStart, float fRange, float &fDistance ) const
{
	fDistance = -FLOAT1;

	ioSphere kRangeSphere( vStart, fRange );
	if( !ioMath::SphereTestLightly( GetWorldCollisionBox(), kRangeSphere ) )
		return false;

	ioSegment kColSegment( vStart, GetWorldCollisionBox().GetCenter() );

	int iQuantity = 0;
	D3DXVECTOR3 vPoints[2];
	if( ioMath::FindIntersection( kColSegment, GetWorldCollisionBox(), iQuantity, vPoints ) )
	{
		if( iQuantity > 0 )
		{
			D3DXVECTOR3 vDiff = vStart - vPoints[0];
			float fDistanceSq = D3DXVec3LengthSq( &vDiff );

			if( fDistanceSq <= fRange * fRange )
			{
				fDistance = sqrt( fDistanceSq );
				return true;
			}
		}
	}

	return false;
}

bool ioPlayEntity::CheckSpherePoint( const D3DXVECTOR3 &vStart, float fRange, D3DXVECTOR3 &vPoint ) const
{
	ioSphere kRangeSphere( vStart, fRange );
	if( !ioMath::SphereTestLightly( GetWorldCollisionBox(), kRangeSphere ) )
		return false;

	ioSegment kColSegment( vStart, GetWorldCollisionBox().GetCenter() );

	int iQuantity = 0;
	D3DXVECTOR3 vContact[2];
	if( ioMath::FindIntersection( kColSegment, GetWorldCollisionBox(), iQuantity, vContact ) )
	{
		if( iQuantity > 0 )
		{
			D3DXVECTOR3 vDiff = vStart - vContact[0];
			float fDistanceSq = D3DXVec3LengthSq( &vDiff );

			if( fDistanceSq <= fRange * fRange )
			{
				vPoint = vContact[0];
				return true;
			}
		}
	}

	return false;
}

bool ioPlayEntity::CheckCylinderPoint( const ioCylinder &rkCylinder,
									   const D3DXVECTOR3 &vStart,
									   D3DXVECTOR3 &vPoint ) const
{
	ioSegment kColSegment( vStart, GetWorldCollisionBox().GetCenter() );

	int iQuantity = 0;
	D3DXVECTOR3 vContact[2];
	if( ioMath::FindIntersection( kColSegment, GetWorldCollisionBox(), iQuantity, vContact ) )
	{
		if( iQuantity > 0 )
		{
			vPoint = vContact[0];
			return true;
		}
	}

	return false;
}

ioOrientBox ioPlayEntity::GetAttackCollisionBox( AniCollisionType eType, bool bNoScale )
{
	return GetWorldCollisionBox();
}

void ioPlayEntity::Revival()
{
	FillMaxHP();
}

void ioPlayEntity::UpdateGauge()
{
}

void ioPlayEntity::RenderGauge()
{
}

void ioPlayEntity::ProcessRecovery()
{
	if( m_HP.m_fRecovery == 0.0f )
		return;

	if( m_HP.m_dwRecoveryTick == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iTickGap = (int)( dwCurTime - m_HP.m_dwPreRecoveryTime );
	if( iTickGap >= (int)m_HP.m_dwRecoveryTick )
	{
		m_HP.m_dwPreRecoveryTime = dwCurTime - ( iTickGap - m_HP.m_dwRecoveryTick );
		m_HP.m_fCurValue = min( m_HP.m_fCurValue + m_HP.m_fRecovery, m_HP.m_fMaxValue );
	}
}

void ioPlayEntity::FillMaxHP()
{
	m_HP.m_fCurValue = m_HP.m_fMaxValue;
	m_HP.m_dwPreRecoveryTime = FRAMEGETTIME();
}

void ioPlayEntity::SetCurHP( float fValue )
{
	m_HP.m_fCurValue = min( fValue, m_HP.m_fMaxValue );
}

void ioPlayEntity::AddHeadDamageInfo( int iDamage, bool bPlus )
{
	HeadDamageInfo kInfo;
	kInfo.dwStartTime = FRAMEGETTIME();
	kInfo.iDamage = iDamage;
	kInfo.bPlus = bPlus;

	m_HeadInfoList.push_back( kInfo );
}

void ioPlayEntity::UpdateHeadInfoList( DWORD dwCurTime )
{
	HeadDamageInfoList::iterator iter = m_HeadInfoList.begin();
	while( iter != m_HeadInfoList.end() )
	{
		if( dwCurTime - iter->dwStartTime < 1500 )
		{
			++iter;
		}
		else
		{
			iter = m_HeadInfoList.erase( iter );
		}
	}
}

void ioPlayEntity::RenderHeadInfoList( int iXPos, int iYPos )
{
	DWORD dwCurTime = FRAMEGETTIME();
	UpdateHeadInfoList( dwCurTime );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE );

	HeadDamageInfoList::iterator iter = m_HeadInfoList.begin();
	while( iter != m_HeadInfoList.end() )
	{
		const HeadDamageInfo &rkInfo = *iter;

		float fScale = FLOAT1;
		float fUpRate = (float)(dwCurTime - rkInfo.dwStartTime) / FLOAT1000;
		float fCosR = D3DX_PI * fUpRate * 3.0f;

		if( COMPARE( fCosR, 0.0f, D3DX_PI ) )
		{
			fScale += cosf( fCosR );
			fScale *= (float)rkInfo.iDamage / FLOAT10;
			fScale = max( fScale, FLOAT1 );
		}

		if( !rkInfo.bPlus )
		{
			g_FontMgr.SetTextColor( 255, 127, 127 );
			g_FontMgr.PrintText( iXPos, iYPos - 70.0f, fScale, STR(1), rkInfo.iDamage );
		}
		else
		{
			g_FontMgr.SetTextColor( 127, 255, 127 );
			g_FontMgr.PrintText( iXPos, iYPos - 70.0f, fScale, STR(2), rkInfo.iDamage );
		}

		++iter;
	}
}

int ioPlayEntity::GetTypeID() const
{
	return GET_PLAY_ENTITY;
}

bool ioPlayEntity::IsCollisionAvailableRange( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	D3DXVECTOR3 vDiff = GetWorldPosition() - pWeapon->GetPosition();
	float fLength = D3DXVec3Length( &vDiff );

	//yunga (웨폰 거리체크 하는...)

	ioZoneEffectWeapon* pZoneEffectWeapon = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffectWeapon )
	{
		if( fLength >= 3500.0f )
			return false;
	}
	else
	if( fLength >= FLOAT1000 )
		return false;

	return true;
}

bool ioPlayEntity::IsZeroHPByMode()
{
	if( m_pModeParent && m_pModeParent->IsZeroHP() )
		return true;

	return false;
}




