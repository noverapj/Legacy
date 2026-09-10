
#include "stdafx.h"

#include "ioDummyChainBuff.h"

#include "WeaponDefine.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioDummyChainBuff::ioDummyChainBuff()
{
	m_dwChainWeaponIndex = -1;
	m_dwMoveEffectID = -1;
	m_dwArriveEffectID = -1;
	m_bSetInvisible = false;
	m_vArrivePos = ioMath::VEC3_ZERO;
}

ioDummyChainBuff::ioDummyChainBuff( const ioDummyChainBuff &rhs )
	: ioBuff( rhs ),
	  m_vDummyOffset( rhs.m_vDummyOffset ),
	  m_szDummyName( rhs.m_szDummyName ),
	  m_nChainWeapon( rhs.m_nChainWeapon ),
	  m_fMoveSpeed( rhs.m_fMoveSpeed ),
	  m_fArriveDiff( rhs.m_fArriveDiff ),
	  m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
	  m_bSetEndJump( rhs.m_bSetEndJump ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_fFloatDiff( rhs.m_fFloatDiff ),
	  m_szMoveEffect( rhs.m_szMoveEffect ),
	  m_vMoveEffectOffset( rhs.m_vMoveEffectOffset ),
	  m_szArriveEffect( rhs.m_szArriveEffect )
{	
	m_dwChainWeaponIndex = -1;
	m_dwMoveEffectID = -1;
	m_dwArriveEffectID = -1;
	m_bSetInvisible = false;
	m_vArrivePos = ioMath::VEC3_ZERO;
}

ioDummyChainBuff::~ioDummyChainBuff()
{	
}

ioBuff* ioDummyChainBuff::Clone()
{
	return new ioDummyChainBuff( *this );
}

void ioDummyChainBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_offset_z", 0.0f );

	m_nChainWeapon = rkLoader.LoadInt_e( "chain_weapon", NO_WEAPON_ATTRIBUTE );
	
	rkLoader.LoadString_e( "move_effect", "", szBuf, MAX_PATH );
	m_szMoveEffect = szBuf;

	m_vMoveEffectOffset.x = rkLoader.LoadFloat_e( "move_effect_offset_x", 0.0f );
	m_vMoveEffectOffset.y = rkLoader.LoadFloat_e( "move_effect_offset_y", 0.0f );
	m_vMoveEffectOffset.z = rkLoader.LoadFloat_e( "move_effect_offset_z", 0.0f );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fArriveDiff = rkLoader.LoadFloat_e( "arrive_diff", 0.0f );

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );

	m_bSetEndJump = rkLoader.LoadBool_e( "set_end_jump", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );
	m_fFloatDiff = rkLoader.LoadFloat_e( "float_diff", 0.0f );

	rkLoader.LoadString_e( "arrive_effect", "", szBuf, MAX_PATH );
	m_szArriveEffect = szBuf;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyChainBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwChainWeaponIndex = -1;
	m_dwMoveEffectID = -1;
	m_dwArriveEffectID = -1;
	m_dwArriveEffectID = -1;
	m_bSetInvisible = false;
	m_BuffState = BS_START;
	m_vArrivePos = ioMath::VEC3_ZERO;		
}

bool ioDummyChainBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwChainWeaponIndex = -1;
	m_dwMoveEffectID = -1;
	m_bSetInvisible = false;
	m_BuffState = BS_START;
	m_vArrivePos = ioMath::VEC3_ZERO;

	return true;
}

void ioDummyChainBuff::ProcessBuff( float fTimePerSec )
{	
	if ( !m_pOwner || m_pOwner->IsSystemState() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) && m_BuffState != BS_MOVE )
		{
			SetReserveEndBuff();
			return;
		}
		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
	
	switch( m_BuffState )
	{
	case BS_START:
		{
			CreateDummy( m_pOwner );
			CreateChain( m_pOwner );
		}
		break;
	case BS_WAIT:
		CheckWaitState( m_pOwner );
		break;
	case BS_MOVE:
		{
			ProcessMove( m_pOwner );
			UpdateMapEffect( m_pOwner );
		}
		break;
	case BS_END:
		{			
			m_pOwner->ClearReservedSliding();
			m_pOwner->SetCurJumpPowerAmt( 0.0f );
			m_pOwner->SetForcePowerAmt( 0.0f );
			m_pOwner->SetGravityAmt( 0.0f );
			m_pOwner->SetCurMoveSpeed( 0.0f );

			SetReserveEndBuff();
			return;
		}
		break;
	}

	if ( m_BuffState == BS_WAIT || m_BuffState == BS_MOVE )
		SetChainPos( m_pOwner );
}

void ioDummyChainBuff::EndBuff()
{
	SetEndState( m_pOwner );

	ioBuff::EndBuff();
}

void ioDummyChainBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nBuffState;
	rkPacket >> nBuffState;

	switch( (BuffState)nBuffState )
	{
	case BS_CreateDummy:
		{
			bool bLiveBuff;
			rkPacket >> bLiveBuff;

			if ( bLiveBuff )
			{
				int nIndex;
				rkPacket >> nIndex;

				D3DXVECTOR3 vPos;
				rkPacket >> vPos;
				rkPacket >> m_vArrivePos;

				ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, nIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
			}
			else
				SetReserveEndBuff();
		}
		break;
	case BS_MOVE:
		{
			rkPacket >> m_vArrivePos;
			SetMoveState( pOwner );
		}
		break;
	case BS_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			m_BuffState = BS_END;
		}
		break;
	}
}

bool ioDummyChainBuff::IsCollisionAvailableWeapon() const
{
	if( m_BuffState == BS_MOVE )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyChainBuff::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

	int nCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, nCurIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );
	if ( !pDummy )
	{
		SetReserveEndBuff();

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << BS_CreateDummy;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	m_vArrivePos = pDummy->GetMidPositionByRate();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateDummy;
		kPacket << true;
		kPacket << nCurIndex;
		kPacket << vPos;
		kPacket << m_vArrivePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChainBuff::CreateChain( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_dwChainWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nChainWeapon;
	kFireTime.dwWeaponIdx = m_dwChainWeaponIndex++;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	
	if ( pWeapon )
	{
		ioUroborusWeapon4 *pChainWeapon = ToUroborusWeapon4( pWeapon );
		if ( pChainWeapon )
		{
			pChainWeapon->SetVisible( false );
			m_dwChainWeaponIndex = pWeapon->GetWeaponIndex();
		}
	}

	m_BuffState = BS_WAIT;
}

void ioDummyChainBuff::SetChainPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	if ( m_vArrivePos == ioMath::VEC3_ZERO )
		return;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwChainWeaponIndex );
	ioUroborusWeapon4 *pChainWeapon = ToUroborusWeapon4( pWeapon );
	if ( pChainWeapon )
	{
		if ( pChainWeapon->IsHasArrow() )
			pChainWeapon->SetVisible( true );
		
		pChainWeapon->SetPosition( m_vArrivePos );
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyChainBuff::CheckWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if ( !pDummy )
	{
		SetMoveState( pOwner );
		return;
	}

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_JUMP:
	case CS_FALL:
	case CS_WOUNDED:
	case CS_BLOW_WOUNDED:
		break;
	default:
		return;
	}

	if ( pOwner->GetCurActionKey() == ioUserKeyInput::AKI_ZX )
	{
		SetMoveState( pOwner );
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyChainBuff::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	
	pOwner->SetSpecialHide( true );
	pOwner->SetBottomCircleVisible( false );
	m_bSetInvisible = true;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vMoveEffectOffset;
	m_dwMoveEffectID = CreateMapEffect( pOwner, m_szMoveEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	m_dwArriveEffectID = CreateMapEffect( pOwner, m_szArriveEffect, m_vArrivePos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	pOwner->SetState( CS_BUFF_ACTION );

	m_BuffState = BS_MOVE;
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_MOVE;
		kPacket << m_vArrivePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyChainBuff::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_dwChainWeaponIndex == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwChainWeaponIndex );
	if ( pWeapon )
		pWeapon->SetWeaponDead();

	if ( m_bSetInvisible )
	{
		pOwner->SetSpecialHide( false );
		pOwner->SetBottomCircleVisible( true );
		m_bSetInvisible = false;

		DestroyMapEffect( pOwner, m_dwMoveEffectID );
		DestroyMapEffect( pOwner, m_dwArriveEffectID );
	}

	if ( m_BuffState == BS_END )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );		
		if ( m_BuffState == BS_END && fabs( fMapHeight - vPos.y ) >= m_fFloatDiff )
		{		
			if ( m_bSetEndJump )
				pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );
			else
				pOwner->SetFallState( true );
		}
		else
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyChainBuff::ProcessMove( ioBaseChar *pOwner )
{	
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwArriveEffectID );
	if ( !pEffect )
		return;

	if( pOwner->IsNeedProcess() && pOwner->IsOwnerChar() )
		pStage->SetCameraToOwnerExplicit( pOwner );
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	D3DXVECTOR3 vDir = pEffect->GetWorldPosition() - pOwner->GetMidPositionByRate();
	float fDiff = D3DXVec3LengthSq( &vDir );
	if ( fDiff <= m_fArriveDiff )
	{
		m_BuffState = BS_END;

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << BS_END;
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	D3DXVec3Normalize( &vDir, &vDir );

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fMoveSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{		
		float fPreHeight = pOwner->GetBottomHeight();

		pOwner->Translate( vTotalMove );

		if( pOwner->GetForceAmt() != 0.0f )
		{
			float fMoveHeight = pOwner->GetBottomHeight();

			if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

				float fTopHeight = pStage->GetMapHeight( vCurPos.x,
					vCurPos.z,
					pOwner,
					false,
					pOwner->GetMidHeightByRate( FLOAT1 ) );

				if( fTopHeight > FLOAT1 )
				{
					vCurPos.y = max( fTopHeight, fPreHeight );
					pOwner->SetWorldPosition( vCurPos );
				}
			}
		}
	}

	if( pStage && pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDummyChainBuff::IsCharCollisionSkipState()
{
	switch( m_BuffState )
	{
	case BS_MOVE:
		return true;
	}

	return false;
}

bool ioDummyChainBuff::CheckEnableUseSkill( int nSlot )
{
	if ( IsCharCollisionSkipState() )
		return false;

	switch( nSlot )
	{
	case ES_WEAPON:
		if( m_bEnableWeaponSkill )
			return true;
		break;
	case ES_ARMOR:
		if( m_bEnableArmorSkill )
			return true;
		break;
	case ES_HELMET:
		if( m_bEnableHelmetSkill )
			return true;
		break;
	case ES_CLOAK:
		if( m_bEnableCloakSkill )
			return true;
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioDummyChainBuff::CreateMapEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pOwner || szEffect.IsEmpty() )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	ioMapEffect *pEffect = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pEffect )
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();

		pEffect->SetWorldPosition( vPos );
		pEffect->SetWorldOrientation( qtRot );

		return pEffect->GetUniqueID();
	}

	return -1;
}

void ioDummyChainBuff::DestroyMapEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioDummyChainBuff::UpdateMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || m_dwMoveEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMoveEffectID );
	if ( pEffect )		
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vMoveEffectOffset;

		pEffect->SetWorldPosition( vPos );
		pEffect->SetWorldOrientation( qtRot );
	}
}