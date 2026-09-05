

#include "stdafx.h"

#include "ioAdhesiveBuff.h"
#include "WeaponDefine.h"
#include "ioDonQuixoteItem.h"

ioAdhesiveBuff::ioAdhesiveBuff()
{
	m_vRelativeAxis = ioMath::VEC3_ZERO;
	m_fRelativeAngle = 0.0f; 
	m_fRelativeLength = 0.0f;
	m_vRelativePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_fRelativeOffsetY = 0.0f;
}

ioAdhesiveBuff::ioAdhesiveBuff( const ioAdhesiveBuff &rhs )
: ioBuff( rhs ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation ),
m_bDisableCollsion( rhs.m_bDisableCollsion ),
m_bRotateEqualCreater( rhs.m_bRotateEqualCreater ),
m_iAttachType( rhs.m_iAttachType ),
m_bAttachCharPos( rhs.m_bAttachCharPos ),
m_bAttachCharPos2( rhs.m_bAttachCharPos2 ),
m_bAttachCharPos3( rhs.m_bAttachCharPos3 ),
m_bMustRunEndBuffCall( rhs.m_bMustRunEndBuffCall ),
m_bWaitCreateWeaponTimeUse( rhs.m_bWaitCreateWeaponTimeUse ),
m_dwWaitCreateWeaponTime( rhs.m_dwWaitCreateWeaponTime ),
m_iPositionCheckType( rhs.m_iPositionCheckType )
{
	m_vRelativeAxis = ioMath::VEC3_ZERO;
	m_fRelativeAngle = 0.0f;
	m_fRelativeLength = 0.0f;
	m_bWorked = false;

	m_bCallEndBuff2 = false;
	m_vRelativePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_fRelativeOffsetY = 0.0f;
}

ioAdhesiveBuff::~ioAdhesiveBuff()
{
}

void ioAdhesiveBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;

	m_bDisableCollsion = rkLoader.LoadBool_e( "disable_collsion", true );	
	m_bRotateEqualCreater = rkLoader.LoadBool_e( "rotate_equal_creater", false );

	m_iAttachType = (AttachType)rkLoader.LoadInt_e( "attach_type", ATTACH_CHAR );
	m_bAttachCharPos = rkLoader.LoadBool_e( "attach_char_pos", false );
	m_bAttachCharPos2 = rkLoader.LoadBool_e( "weapon_dir_attach_char", false );
	m_bAttachCharPos3 = rkLoader.LoadBool_e( "attach_char_by_collision_offset", false );

	m_bMustRunEndBuffCall = rkLoader.LoadBool_e( "must_call_end_buff", false );
	m_bWaitCreateWeaponTimeUse = rkLoader.LoadBool_e( "is_wait_create_weapon_time", false );
	m_dwWaitCreateWeaponTime = (DWORD)rkLoader.LoadInt_e( "wait_create_weapon_time", 0 );

	m_iPositionCheckType = (PositionCheckType)rkLoader.LoadInt_e( "position_safe_check_type", PCT_NONE );
}

ioBuff* ioAdhesiveBuff::Clone()
{
	return new ioAdhesiveBuff( *this );
}

void ioAdhesiveBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;
	
	m_bLive = true;

	m_vRelativeAxis = ioMath::VEC3_ZERO;
	m_fRelativeAngle = 0.0f;
	m_fRelativeLength = 0.0f;

	m_fRelativeOffsetY = 0.0f;
	m_vRelativePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioAdhesiveBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioAdhesiveBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_iOperationType == OT_DELAY )
	{
		if( m_bBuffStart )
		{
			if( pOwner->GetState() != CS_ADHESIVE_BUFF ||
				pOwner->GetCurAdhesiveBuffName() != GetName() )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
	else
	{
		if( pOwner->GetState() != CS_ADHESIVE_BUFF ||
			pOwner->GetCurAdhesiveBuffName() != GetName() )
		{
			SetReserveEndBuff();
			return;
		}
	}
	
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}
		CheckCallingProcess();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if( !IsLive() )
		return;

	ProcessPosition();
}

void ioAdhesiveBuff::EndBuff()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
	}

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		CheckInvisibleWeapon( true );
		CheckInvisibleCloak( true );
		CheckInvisibleHelmet( true );
		CheckChangeSkill( false );
		CheckChangeColor( false );

		pOwner->EndEffect( m_BuffEffect, false );
		pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );

		if( m_bDescEqualDuration )
			pOwner->EndEmoticonByName( m_DescName );

		if( IsBadStateBuff() )
		{
			if( !pOwner->IsOwnerChar() )
				pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
		}
		else
		{
			if( !pOwner->IsOwnerChar() )
				pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
		}

		DestroyAreaWeapon();
		CheckCallWeaponEnd();

		//돈키호때 추가
		if( m_bCallEndBuff2 && pOwner->GetCurAdhesiveBuffName() == GetName() )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			CheckBuffEndBuff2();
		}
		else if( pOwner->GetState() == CS_ADHESIVE_BUFF && pOwner->GetCurAdhesiveBuffName() == GetName() && m_bMustRunEndBuffCall == false )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			CheckBuffEndBuff();
		}
		else if( pOwner->GetCurAdhesiveBuffName() == GetName() && m_bMustRunEndBuffCall == true )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			CheckBuffEndBuff();
		}
	}
	ioBuff::EndBuff();
}

bool ioAdhesiveBuff::GetAdhesiveCollisionDisable()
{
	return m_bDisableCollsion;
}

void ioAdhesiveBuff::SetCallEndBuff2( bool bCall )
{
	m_bCallEndBuff2 = bCall;
}

void ioAdhesiveBuff::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_LOADING &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_VIEW )
	{
		pOwner->SetAdhesiveState( GetName() );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );
	}

	m_bBuffStart = true;
}

void ioAdhesiveBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
			SetAction();
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		SetAction();

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	CheckCallingProcess();
}

void ioAdhesiveBuff::SetRelativeVector( D3DXVECTOR3 &vVector )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vRelativeDir = ioMath::VEC3_ZERO;
	
	if ( ( m_bAttachCharPos || m_bAttachCharPos2 || m_bAttachCharPos3 ) && m_iAttachType == ATTACH_WEAPON  )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );
		if ( pWeapon )
		{
			vDir = pWeapon->GetMoveDir();
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRelative =  pOwner->GetWorldPosition() - pWeapon->GetPosition();
			
			m_fRelativeOffsetY = vRelative.y;

			vRelativeDir = vRelative;

			D3DXVec3Normalize( &vRelativeDir, &vRelativeDir );

			m_fRelativeLength = D3DXVec3Length( &vRelative );
			if ( m_bAttachCharPos3 )
				m_vRelativePos = vRelativeDir * m_fRelativeLength;
		}
	}
	else
	{
		vDir= pCreator->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		vRelativeDir = vVector;
		D3DXVec3Normalize( &vRelativeDir, &vRelativeDir );

		m_fRelativeLength = D3DXVec3Length( &vVector );
	}

	float fDotValue = D3DXVec3Dot( &vDir, &vRelativeDir );

	if( fDotValue >= FLOAT1 )		// 같은 방향으로 인식
	{
		m_vRelativeAxis = ioMath::UNIT_Y;
		m_fRelativeAngle = 0.0f;
	}
	else if( fDotValue <= -FLOAT1 )	// 반대 방향으로 인식
	{
		m_vRelativeAxis = ioMath::UNIT_Y;
		m_fRelativeAngle = 180.0f;
	}
	else
	{
		D3DXVec3Cross( &m_vRelativeAxis, &vDir, &vRelativeDir );
		D3DXVec3Normalize( &m_vRelativeAxis, &m_vRelativeAxis );
		m_fRelativeAngle = RADtoDEG( acosf(fDotValue) );
	}
}

void ioAdhesiveBuff::ProcessPosition()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCreateWeaponIndex == 0 )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
	{
		SetReserveEndBuff();
		return;
	}

	ioWeapon *pWeapon = NULL;
	pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );

	if( !pWeapon && m_bWaitCreateWeaponTimeUse && (m_dwWaitCreateWeaponTime+m_dwBuffStartTime) > dwCurTime && m_bWorked == false )
		return;

	if( !pWeapon )
	{
		SetReserveEndBuff();
		return;
	}

	if( !pWeapon->IsLive() )
	{
		SetReserveEndBuff();
		return;
	}

	if( pWeapon )
	{
		if( m_bWorked == false  )
		{
			//돈키호테때 추가
			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			if( pZone && pZone->GetWoundItemType() == ioZoneEffectWeapon::WIT_DONQUIXOTE )
			{
				ioDonQuixoteItem *pDonQuixoteItem = ToDonQuixoteItem( pCreator->GetPriorityItem(SPT_ATTACK) );
				if( pDonQuixoteItem )
				{
					pDonQuixoteItem->WeaponWounded( pWeapon, pOwner );
				}
			}
		}
		m_bWorked = true;
	}

	switch( m_iAttachType )
	{
	case ATTACH_CHAR:
		{
			D3DXVECTOR3 vCharDir = pCreator->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &m_vRelativeAxis, DEGtoRAD(m_fRelativeAngle) );

			D3DXVECTOR3 vRelativeDir = qtRot * vCharDir;
			D3DXVec3Normalize( &vRelativeDir, &vRelativeDir );
			vRelativeDir = vRelativeDir * m_fRelativeLength;

			D3DXVECTOR3 vNewPos = pCreator->GetWorldPosition() + vRelativeDir;
			vNewPos.y = pCreator->GetWorldPosition().y;
			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotToTargetPos( pCreator->GetWorldPosition(), true );
			if( m_bRotateEqualCreater )
				pOwner->SetTargetRotToDir(vCharDir, true);
		}
		break;
	case ATTACH_WEAPON:
		{
			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			ioContactWeapon* pContact = ToContactWeapon( pWeapon );
			ioZoneMoveWeapon* pZone = ToZoneMoveWeapon( pWeapon );
			ioFloatThrowWeapon* pFloatThrow = ToFloatThrowWeapon( pWeapon );

			if( pContact )
			{
				vNewPos = pContact->GetAttchBoxPosition();
			}
			else if( pZone )
			{
				vNewPos = pZone->GetAttchPosition();
			}
			else if ( pFloatThrow )
			{
				vNewPos = pFloatThrow->GetAttchBoxPosition();

				D3DXQUATERNION qtNewRot = pWeapon->GetOrientation();
				pOwner->SetWorldOrientation( qtNewRot );
			}
			else if ( m_bAttachCharPos )
			{
				// 웨폰이 진행방향을 상속받아서 가지고 있어야함
				D3DXVECTOR3 vWeaponDir = pWeapon->GetMoveDir();

				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis( &qtRot, &m_vRelativeAxis, DEGtoRAD(m_fRelativeAngle) );

				D3DXVECTOR3 vRelativeDir = qtRot * vWeaponDir;
				D3DXVec3Normalize( &vRelativeDir, &vRelativeDir );
				vRelativeDir = vRelativeDir * m_fRelativeLength;

				vNewPos = pWeapon->GetPosition() + vRelativeDir;
				vNewPos.y = pOwner->GetWorldPosition().y;
				
				D3DXQUATERNION qtNewRot = pWeapon->GetOrientation();
				pOwner->SetWorldOrientation( qtNewRot );
			}
			else if ( m_bAttachCharPos2 )
			{
				// 웨폰이 진행방향을 상속받아서 가지고 있어야함
				D3DXVECTOR3 vWeaponDir = pWeapon->GetMoveDir();

				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis( &qtRot, &m_vRelativeAxis, DEGtoRAD(m_fRelativeAngle) );

				D3DXVECTOR3 vRelativeDir = qtRot * vWeaponDir;
				D3DXVec3Normalize( &vRelativeDir, &vRelativeDir );
				vRelativeDir = vRelativeDir * m_fRelativeLength;

				vNewPos = pWeapon->GetPosition() + vRelativeDir;
				
			}
			else if ( m_bAttachCharPos3 )
			{
				vNewPos = pWeapon->GetPosition() + m_vRelativePos;
				vNewPos.y = pWeapon->GetPosition().y + m_fRelativeOffsetY;

				float fPreHeight = pOwner->GetBottomHeight();

				vNewPos.y = max( vNewPos.y,fPreHeight );
			}
			else
			{
				vNewPos = pWeapon->GetPosition();
			}
			pOwner->SetWorldPosition( vNewPos );
		}
		break;
	}

	switch( m_iPositionCheckType )
	{
	case PCT_LAND_OVER:
		{
			float fOwnerHeight = m_pOwner->GetWorldPosition().y;
			float fMapBottomHeight = m_pOwner->GetBottomHeight();
			if( fMapBottomHeight > fOwnerHeight && (fMapBottomHeight - fOwnerHeight) < 25.f )
			{
				D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
				vPos.y = fMapBottomHeight + FLOAT1;
				m_pOwner->SetWorldPosition( vPos );
				SetReserveEndBuff();
			}
		}
		break;
	}
}