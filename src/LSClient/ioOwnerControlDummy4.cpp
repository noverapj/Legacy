
#include "stdafx.h"

#include "ioOwnerControlDummy4.h"
#include "ioDummyAttachBuff3.h"

ioOwnerControlDummy4::ioOwnerControlDummy4( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bSetUpKey	= false;
	m_bSetDownKey = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_ControlState = CS_Start;
	dwCheckAttackTime = 0;
	m_nAttackCnt = 0;
}

ioOwnerControlDummy4::~ioOwnerControlDummy4()
{
	m_HeadInfoList.clear();
}

void ioOwnerControlDummy4::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "control_move_ani", "", szBuf, MAX_PATH );
	m_szControlMoveAni = szBuf;

	m_fControlMoveAniRate = rkLoader.LoadFloat_e( "control_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "control_check_buff", "", szBuf, MAX_PATH );
	m_szControlCheckBuff = szBuf;

	rkLoader.LoadString_e( "attack_check_buff", "", szBuf, MAX_PATH );
	m_szAttackCheckBuff = szBuf;

	m_fAttackRange = rkLoader.LoadFloat_e( "attack_range", 0.0f );
	m_fAttackAngle = rkLoader.LoadFloat_e( "attack_angle", 0.0f );
	m_fAttackUpHeight = rkLoader.LoadFloat_e( "attack_up_height", 0.0f );
	m_fAttackUnderHeight = rkLoader.LoadFloat_e( "attack_under_height", 0.0f );

	dwAttackTime = rkLoader.LoadInt_e( "attack_time", 0 );
	m_nWeaponType = rkLoader.LoadInt_e( "weapon_type", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "return_move_ani", "", szBuf, MAX_PATH );
	m_szReturnMoveAni = szBuf;

	m_fReturnMoveAniRate = rkLoader.LoadFloat_e( "return_move_ani_rate", FLOAT1 );
	m_fReturnMoveSpeed = rkLoader.LoadFloat_e( "return_move_speed", 0.0f );
	m_fReturnDist = rkLoader.LoadFloat_e( "return_dist", 0.0f );

	m_HeadInfoList.clear();
	int nMaxHealBuffCnt = rkLoader.LoadInt_e( "heal_info_cnt", 0 );
	for ( int i=0; i<nMaxHealBuffCnt; i++ )
	{
		OwnerHealInfo sOwnerHealInfo;
		wsprintf_e( szKey, "heal_info%d_min_cnt", i+1 );
		sOwnerHealInfo.m_MinAttackCnt = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "heal_info%d_max_cnt", i+1 );
		sOwnerHealInfo.m_MaxAttackCnt = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "heal_info%d_buff", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sOwnerHealInfo.m_szHealBuff = szBuf;

		m_OwnerHealInfoList.push_back( sOwnerHealInfo );
	}

	rkLoader.LoadString_e( "heal_other_buff", "", szBuf, MAX_PATH );
	m_szHealOtherBuff = szBuf;	
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "check_max_attack", 0 );
}

void ioOwnerControlDummy4::ProcessState( float fTimePerSec, DWORD dwPreTime )
{	
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ControlState )
	{
	case CS_Start:
		if ( m_dwStartAniEndTime < dwCurTime )
			SetControlMoveState();
		break;
	case CS_Control_Move:
		{
			CheckControlKey( GetOwner() );
			ProcessControlMove( GetOwner(), fTimePerSec );

			CreateWeapon( GetOwner() );
			CheckReturnMove( GetOwner() );
		}
		break;
	case CS_Return_Move:
		{
			ProcessReturnMove( GetOwner(), fTimePerSec );
			CheckReturnEnd( GetOwner() );
		}
		break;
	case CS_End:
		{
			ioBaseChar *pOwner = GetOwner();
			if ( pOwner )
				pOwner->SetReserveEndBuff( m_szControlCheckBuff );

			SetDieState( true );
			HealOwnerBuff( GetOwner() );
		}
		break;
	}
}

void ioOwnerControlDummy4::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;	

	int nType;
	rkPacket >> nType;	
	
	switch ( nType )
	{
	case CS_Control_Move:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			SetWorldPosition( vPos );
			SetWorldOrientation( qtRot );
		}
		break;
	case CS_Return_Move:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			SetWorldPosition( vPos );
			SetWorldOrientation( qtRot );

			SetReturnMoveState();
		}
		break;
	case CS_End:
		{
			rkPacket >> m_nAttackCnt;
			m_ControlState = CS_End;
		}
		break;
	}
}

bool ioOwnerControlDummy4::IsActivateWeapon()
{
	return true;
}

ApplyWeaponExp ioOwnerControlDummy4::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);

	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;

		ioBaseChar *pOwner = GetOwner();
		if ( pOwner )
			pOwner->SetReserveEndBuff( m_szControlCheckBuff );

		SetDieState( true, true );

		if ( m_nAttackCnt > 0 )
			pAttacker->AddNewBuff( m_szHealOtherBuff, pAttacker->GetCharName(), "", NULL );
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

void ioOwnerControlDummy4::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
}

void ioOwnerControlDummy4::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioOwnerControlDummy4::IsNeedCheckMapCollision() const
{
	return !m_bDisableMapCollision;
}

int ioOwnerControlDummy4::DontMoveEntityLevel() const
{
	return 1;
}

bool ioOwnerControlDummy4::IsMountingAvailable() const
{	
	return false;
}

bool ioOwnerControlDummy4::IsMountAvailable() const
{	
	return false;
}

bool ioOwnerControlDummy4::IsNowMountEnable() const
{
	return false;
}

void ioOwnerControlDummy4::SetControlMoveState()
{
	int nAniID = m_pGroup->GetAnimationIdx( m_szControlMoveAni );
	if( nAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		m_pGroup->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fControlMoveAniRate );
	}

	m_ControlState = CS_Control_Move;
}

void ioOwnerControlDummy4::CheckControlKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != ioUserKeyInput::DKI_NONE && eNewDirKey != m_CurDirKey )
	{
		float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		SetWorldOrientation( qtRotate );
		m_CurDirKey = eNewDirKey;

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << CS_Control_Move;
			kPacket << GetWorldPosition();
			kPacket << GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioOwnerControlDummy4::ProcessControlMove( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	vMoveDir.y = 0;

	Translate( vMoveDir * ( m_fMoveSpeed * fTimePerSec ) );

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fMapHeight > 0.0f )
	{
		vPos.y = fMapHeight;
		SetWorldPosition( vPos );
	}
}

void ioOwnerControlDummy4::SetReturnMoveState()
{
	int nAniID = m_pGroup->GetAnimationIdx( m_szReturnMoveAni );
	if( nAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		m_pGroup->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1/m_fReturnMoveAniRate );
	}

	m_ControlState = CS_Return_Move;
}

void ioOwnerControlDummy4::CheckReturnMove( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bSendNetwork = false;
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCreateTime + m_dwLifeTime < dwCurTime )
	{
		SetReturnMoveState();
		bSendNetwork = true;
	}
	else
	{
		if ( !pOwner->HasBuff( m_szControlCheckBuff ) )
		{
			SetReturnMoveState();
			bSendNetwork = true;
		}
	}

	if ( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CS_Return_Move;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerControlDummy4::ProcessReturnMove( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = pOwner->GetWorldPosition() - GetWorldPosition();
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	Translate( vMoveDir * ( m_fReturnMoveSpeed * fTimePerSec ) );

	vMoveDir.y = 0;
	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	SetWorldOrientationAndDir( qtTargetRot );
}

void ioOwnerControlDummy4::CheckReturnEnd( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bSendNetwork = false;
	D3DXVECTOR3 vDist = pOwner->GetWorldPosition() - GetWorldPosition();
	
	if ( D3DXVec3LengthSq( &vDist ) <= m_fReturnDist )
	{
		m_ControlState = CS_End;
		bSendNetwork = true;
	}
	
	if ( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << CS_End;
		kPacket << m_nAttackCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOwnerControlDummy4::FindTarget( ioBaseChar *pOwner, float fTargetRange, float fTargetAngle, float fTargetUpheight, float fTargetUnderHeight, ioHashStringVec &szTargetList )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	szTargetList.clear();

	BaseCharList vTargetList;

	D3DXVECTOR3 vPos = GetMidPositionByRate();
	pOwner->GetTargetCharListByCylinder( vPos, fTargetRange, fTargetAngle, fTargetUpheight, fTargetUnderHeight, vTargetList );

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )	
			continue;

		if( pOwner == pTarget )	
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if ( !m_szAttackCheckBuff.IsEmpty() && pTarget->HasBuff( pOwner->GetCharName(), m_szAttackCheckBuff ) )
			continue;


		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		szTargetList.push_back( pTarget->GetCharName() );
	}
}

void ioOwnerControlDummy4::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( dwCheckAttackTime + dwAttackTime > dwCurTime )
		return;

	ioHashStringVec szAttackTargetList;
	FindTarget( pOwner, m_fAttackRange, m_fAttackAngle, m_fAttackUpHeight, m_fAttackUnderHeight, szAttackTargetList );

	if ( szAttackTargetList.empty() )
		return;

	dwCheckAttackTime = dwCurTime;

	DWORD dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;
	for ( int i=0; i<(int)szAttackTargetList.size(); ++i )
	{
		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.iAttributeIdx = m_nWeaponType;
		kFireTime.iResistanceIdx = 0;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWoundedAniDuration = 0;
		kFireTime.bLoopWoundedAni = false;
		kFireTime.dwWeaponIdx = dwBaseWeaponIndex+i;
		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

		D3DXVECTOR3 vPos = GetMidPositionByRate();
		D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		ioBaseChar *pTarget = pOwner->GetBaseChar( szAttackTargetList[i] );
		if ( pTarget )
			vDir = pTarget->GetMidPositionByRate() - vPos;

		D3DXVec3Normalize( &vDir, &vDir );
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		if ( pWeapon )
		{
			pWeapon->SetPosition( vPos );
			pWeapon->SetMoveDir( vDir );
			m_nAttackCnt++;
			LOG.PrintTimeAndLog( 0, "!!!!!!!Owner : %s - CreateWeapon : %d", pOwner->GetCharName().c_str(), m_nAttackCnt );
		}
	}
}

void ioOwnerControlDummy4::HealOwnerBuff( ioBaseChar *pOwner )
{
	if ( !pOwner || m_nAttackCnt <= 0 || m_OwnerHealInfoList.empty() )
		return;

	int nMaxCnt = (int)m_OwnerHealInfoList.size();
	if ( m_nAttackCnt >= m_nMaxAttackCnt )
		pOwner->AddNewBuff( m_OwnerHealInfoList[nMaxCnt-1].m_szHealBuff, pOwner->GetCharName(), "", NULL );
	else
	{
		for ( int i=0; i<nMaxCnt; i++ )
		{
			OwnerHealInfo sOwnerHealInfo = m_OwnerHealInfoList[i];
			if ( COMPARE( m_nAttackCnt, sOwnerHealInfo.m_MinAttackCnt, sOwnerHealInfo.m_MaxAttackCnt ) )
			{
				pOwner->AddNewBuff( sOwnerHealInfo.m_szHealBuff, pOwner->GetCharName(), "", NULL );
				return;
			}
		}
	}
}