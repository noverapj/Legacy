

#include "stdafx.h"
#include "ioBaseChar.h"

#include "ioBlowPushPowerBuff.h"

#include "ioGrowthUpInfo.h"

#include "BuffDefine.h"

ioBlowPushPowerBuff::ioBlowPushPowerBuff()
{
	m_fExtraBlowRate = 0.0f;
	m_fExtraPushRate = 0.0f;
}

ioBlowPushPowerBuff::ioBlowPushPowerBuff( const ioBlowPushPowerBuff &rhs )
: ioBuff( rhs ),
m_vPushPowerList( rhs.m_vPushPowerList ),
m_vBlowPowerList( rhs.m_vBlowPowerList ),
m_fPushFriction( rhs.m_fPushFriction ),
m_vPushAngleList( rhs.m_vPushAngleList ),
m_bFrictionLinear( rhs.m_bFrictionLinear ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bCreatorLookDirPush( rhs.m_bCreatorLookDirPush ),
m_fCreatorLookDirPushPower( rhs.m_fCreatorLookDirPushPower )
{
	m_fExtraBlowRate = 0.0f;
	m_fExtraPushRate = 0.0f;
}

ioBlowPushPowerBuff::~ioBlowPushPowerBuff()
{
}

void ioBlowPushPowerBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	// Push
	m_fPushFriction = rkLoader.LoadFloat_e( "push_power_friction", 0.0f );
	m_bFrictionLinear = rkLoader.LoadBool_e( "push_friction_linear", false );

	int iCnt = rkLoader.LoadInt_e( "push_power_cnt", 0 );
	m_vPushPowerList.clear();

	if( iCnt > 0 )
	{
		m_vPushPowerList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szBuf, "push_power%d", i+1 );
			float fPushPower = rkLoader.LoadFloat( szBuf, 0.0f );

			m_vPushPowerList.push_back( fPushPower );
		}
	}

	// Blow
	iCnt = rkLoader.LoadInt_e( "blow_power_cnt", 0 );
	m_vBlowPowerList.clear();

	if( iCnt > 0 )
	{
		m_vBlowPowerList.reserve( iCnt );

		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szBuf, "blow_power%d", j+1 );
			float fBlowPower = rkLoader.LoadFloat( szBuf, 0.0f );

			m_vBlowPowerList.push_back( fBlowPower );
		}
	}

	// Angle
	iCnt = rkLoader.LoadInt_e( "push_angle_cnt", 0 );
	m_vPushAngleList.clear();

	if( iCnt > 0 )
	{
		m_vPushAngleList.reserve( iCnt );

		for( int k=0; k < iCnt; ++k )
		{
			wsprintf_e( szBuf, "push_angle%d", k+1 );
			float fAngle = rkLoader.LoadFloat( szBuf, 0.0f );

			m_vPushAngleList.push_back( fAngle );
		}
	}

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );

	m_bCreatorLookDirPush = rkLoader.LoadBool_e("creator_look_dir_push", false);
	m_fCreatorLookDirPushPower = rkLoader.LoadFloat_e("creator_look_dir_push_power", 0.f);
}

ioBuff* ioBlowPushPowerBuff::Clone()
{
	return new ioBlowPushPowerBuff( *this );
}

void ioBlowPushPowerBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pOwner->GetRandomSeed();
	random.SetRandomSeed( dwSeed );

	// Angle
	float fAngle = 0.0f;
	int iCnt = m_vPushAngleList.size();
	if( iCnt > 0 )
	{
		iRand = random.Random( iCnt );

		if( COMPARE( iRand, 0, iCnt ) )
		{
			fAngle = m_vPushAngleList[iRand];
		}
	}

	D3DXVECTOR3 vMoveDir;
	vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vAxis( 0.0f, FLOAT1, 0.0f );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	vMoveDir = qtRot * vMoveDir;
	//

	ioGrowthBlowPushUpInfo *pUpInfo = ToBlowPushUpInfo(GetGrowthUpInfoForItem(GT_BLOW_PUSH));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		m_fExtraBlowRate = pUpInfo->GetBlowRate(pStage);
		m_fExtraPushRate = pUpInfo->GetPushRate(pStage);
	}

	// Push	
	iCnt = m_vPushPowerList.size();
	if( m_bCreatorLookDirPush )
	{
		pOwner->SetForcePower( GetCreator()->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1), m_fCreatorLookDirPushPower, m_fPushFriction, m_bFrictionLinear );
	}
	if( iCnt > 0 )
	{
		float fPushPower = FLOAT1;
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fPushPower = m_vPushPowerList[iRand];

		fPushPower += m_fExtraPushRate;

		ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
		if( pBuff )
		{
			if( pBuff->CheckTeamAttack() )
			{
				if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
					fPushPower *= pBuff->GetPushPowerRate();
			}
			else
				fPushPower *= pBuff->GetPushPowerRate();
		}


		if( m_vPushAngleList.empty() )  // 지정된 각도가 없으면 현재 방향으로 그냥 밀어낸다
			pOwner->SetForcePower( pOwner->GetForceDir(), fPushPower, m_fPushFriction, m_bFrictionLinear );
		else
			pOwner->SetForcePower( vMoveDir, fPushPower, m_fPushFriction, m_bFrictionLinear );
	}
	

	// Blow
	iCnt = m_vBlowPowerList.size();
	if( iCnt > 0 )
	{
		float fBlowPower = FLOAT1;
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fBlowPower = m_vBlowPowerList[iRand];

		fBlowPower += m_fExtraBlowRate;

		ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
		if( pBuff )
		{
			if( pBuff->CheckTeamAttack() )
			{
				if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
					fBlowPower *= pBuff->GetBlowPowerRate();
			}
			else
				fBlowPower *= pBuff->GetBlowPowerRate();
		}

		pOwner->SetJumpPower( fBlowPower );
	}
}

bool ioBlowPushPowerBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vAxis( 0.0f, FLOAT1, 0.0f );

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pOwner->GetRandomSeed();
	random.SetRandomSeed( dwSeed );

	// Angle
	float fAngle = 0.0f;
	int iCnt = m_vPushAngleList.size();

	if( iCnt > 0 )
	{
		float fPushPower = FLOAT1;
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
		{
			fAngle = m_vPushAngleList[iRand];
		}
	}

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	vMoveDir = qtRot * vMoveDir;

	// Push	
	iCnt = m_vPushPowerList.size();
	if( m_bCreatorLookDirPush )
	{
		pOwner->SetForcePower( GetCreator()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ), m_fCreatorLookDirPushPower, m_fPushFriction, m_bFrictionLinear );
	}
	if( iCnt > 0 )
	{
		float fPushPower = FLOAT1;
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fPushPower = m_vPushPowerList[iRand];

		fPushPower += m_fExtraPushRate;
		
		ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
		if( pBuff )
		{
			if( pBuff->CheckTeamAttack() )
			{
				if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
					fPushPower *= pBuff->GetPushPowerRate();
			}
			else
				fPushPower *= pBuff->GetPushPowerRate();
		}
		
		if( m_vPushAngleList.empty() )  // 지정된 각도가 없으면 현재 방향으로 그냥 밀어낸다
			pOwner->SetForcePower( pOwner->GetForceDir(), fPushPower, m_fPushFriction, m_bFrictionLinear );
		else
			pOwner->SetForcePower( vMoveDir, fPushPower, m_fPushFriction, m_bFrictionLinear );
	}

	// Blow
	iCnt = m_vBlowPowerList.size();
	if( iCnt > 0 )
	{
		float fBlowPower = FLOAT1;
		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
			fBlowPower = m_vBlowPowerList[iRand];

		fBlowPower += m_fExtraBlowRate;

		ioAffectWoundedValueBuff *pBuff = ToAffectWoundedValue( pOwner->GetAffectWoundedValueBuff() );
		if( pBuff )
		{
			if( pBuff->CheckTeamAttack() )
			{
				if( GetOwner() && GetCreator() && GetOwner()->GetTeam() != GetCreator()->GetTeam() )
					fBlowPower *= pBuff->GetBlowPowerRate();
			}
			else
				fBlowPower *= pBuff->GetBlowPowerRate();
		}

		pOwner->SetJumpPower( fBlowPower );
	}

	return true;
}

void ioBlowPushPowerBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
	case OT_DELAY:
	case OT_PASSIVE:
	case OT_ACTIVE:
		{
			if( m_CallBuffList.empty() )
				SetReserveEndBuff();
			else
				CheckCallingProcess();
		}
		break;
	}
}

void ioBlowPushPowerBuff::EndBuff()
{
	ioBuff::EndBuff();
}


void ioBlowPushPowerBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraPushRate;
	rkPacket << m_fExtraBlowRate;
}

void ioBlowPushPowerBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraPushRate;
	rkPacket >> m_fExtraBlowRate;
}

bool ioBlowPushPowerBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}