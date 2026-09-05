#include "StdAfx.h"
#include "ioCreateDummyChar9Buff.h"

#include "WeaponDefine.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioCreateDummyChar9Buff::ioCreateDummyChar9Buff(void)
{
	ClearData();
}

ioCreateDummyChar9Buff::ioCreateDummyChar9Buff( const ioCreateDummyChar9Buff &rhs ):
ioBuff( rhs ),
m_vDummyOffset( rhs.m_vDummyOffset ),
m_szDummyName( rhs.m_szDummyName ),
m_dwRangeCheckDisableTime( rhs.m_dwRangeCheckDisableTime ),
m_fEnableReturnPositionRange( rhs.m_fEnableReturnPositionRange ),
m_OwnerBuffList( rhs.m_OwnerBuffList ),
m_bHideFootDustEffect( rhs.m_bHideFootDustEffect ),
m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
m_iChainWeapon( rhs.m_iChainWeapon ),
m_dwSlerpTime( rhs.m_dwSlerpTime ),
m_fAlphaRate( rhs.m_fAlphaRate ),
m_szBuffMapEffectName( rhs.m_szBuffMapEffectName ),
m_fOwnerAttachEffectPositionRate( rhs.m_fOwnerAttachEffectPositionRate ),
m_bReturnPosition( rhs.m_bReturnPosition )
{
	ClearData();
}

ioCreateDummyChar9Buff::~ioCreateDummyChar9Buff(void)
{
}

void ioCreateDummyChar9Buff::ClearData()
{
	m_BuffState				= BS_NONE;
	m_dwRangeCheckStartTime = 0;
	m_iDummyIndex			= -1;
	m_dwChainWeaponIndex	= -1;
	m_vDummyPos				= ioMath::VEC3_ZERO;
	m_bSlerpEnd				= false;
	m_dwBuffMapEffectID		= -1;
}

ioBuff* ioCreateDummyChar9Buff::Clone()
{
	return new ioCreateDummyChar9Buff( *this );
}

void ioCreateDummyChar9Buff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_offset_z", 0.0f );

	m_bHideFootDustEffect = rkLoader.LoadBool_e( "hide_foot_dust_effect", false );
	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
	m_bReturnPosition = rkLoader.LoadBool_e( "return_posiotion", false );

	int nBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_OwnerBuffList.push_back( szBuf );
	}

	m_dwRangeCheckDisableTime = (DWORD)rkLoader.LoadInt_e( "range_check_disable_time", 0 );
	m_fEnableReturnPositionRange = rkLoader.LoadFloat_e( "enable_return_position_range", 0.0f );

	//For Chain
	m_iChainWeapon = rkLoader.LoadInt_e( "chain_weapon", NO_WEAPON_ATTRIBUTE );

	//For Hide
	m_fAlphaRate = rkLoader.LoadFloat_e( "alpha_rate", FLOAT1 );
	m_dwSlerpTime = rkLoader.LoadInt_e( "slerp_time", 1 );

	//For Map Effect
	rkLoader.LoadString_e( "buff_map_effect_name", "", szBuf, MAX_PATH );
	m_szBuffMapEffectName = szBuf;
	m_fOwnerAttachEffectPositionRate = rkLoader.LoadFloat_e( "owner_attach_effect_position_rate", FLOAT05 );
}

void ioCreateDummyChar9Buff::StartBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	

	ioBuff::StartBuff( pOwner );

	ClearData();

	m_BuffState = BS_CREAT_DUMMY;

	if( m_bHideFootDustEffect )
		pOwner->SetHideFootDustEffect( true );

	CreateMapEffect( pOwner );
}

bool ioCreateDummyChar9Buff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
	{
		EndBuff();
		return false;
	}

	ClearData();

	m_BuffState = BS_CREAT_DUMMY;

	if( m_bHideFootDustEffect )
		pOwner->SetHideFootDustEffect( true );

	CreateMapEffect( pOwner );

	return true;
}

void ioCreateDummyChar9Buff::CreateMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( m_fOwnerAttachEffectPositionRate );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szBuffMapEffectName, vPos, vScale );
	if( pMapEffect )
		m_dwBuffMapEffectID = pMapEffect->GetUniqueID();
}

void ioCreateDummyChar9Buff::ProcessBuff( float fTimePerSec )
{
	if ( !m_pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if ( !CheckLiveBuff( m_pOwner ) )
	{
		SetReserveEndBuff();
		return;
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
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if ( m_bLive )
	{
		switch( m_BuffState )
		{
		case BS_CREAT_DUMMY:
			{
				CreateDummy( m_pOwner );
				CreateChain( m_pOwner );
			}
			break;
		case BS_RANGE_CHECK:
			{
				CheckRange( m_pOwner );
				SetChainPos( m_pOwner );
				SetMapEffectPos( m_pOwner );
			}
			break;
		}

		if( !m_pOwner->IsCanInvisible() && !m_bSlerpEnd )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwBuffStartTime;
			float fGapRate = (float)dwGap / (float)m_dwSlerpTime;
			fGapRate = min( fGapRate, FLOAT1 );
			int iAlphaRate =  (1.0f - (1.0f - m_fAlphaRate)*fGapRate ) * MAX_ALPHA_RATE;

			if( fGapRate == FLOAT1 )
				m_bSlerpEnd = true;

			iAlphaRate = max( 1, iAlphaRate );
			m_pOwner->SetAlphaRateDirect( iAlphaRate );
		}
	}
}

bool ioCreateDummyChar9Buff::CheckLiveBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->IsPrisonerMode() )
		return false;

	if ( m_iDummyIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
		if ( !pDummy || pDummy->GetState() == DCS_DIE )
			return false;
	}

	return true;
}

void ioCreateDummyChar9Buff::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_szDummyName.IsEmpty() )
		return;

	g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szDummyName );

	m_BuffState = BS_RANGE_CHECK;

	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

	m_iDummyIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, m_iDummyIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );
	if ( !pDummy )
		return;

	m_vDummyPos = pDummy->GetMidPositionByRate();
	m_dwRangeCheckStartTime = FRAMEGETTIME() + m_dwRangeCheckDisableTime;

	AddOwnerBuff( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CREAT_DUMMY;
		kPacket << m_iDummyIndex;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreateDummyChar9Buff::AddOwnerBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	if ( m_OwnerBuffList.empty() )
		return;
	
	for ( int i=0; i<(int)m_OwnerBuffList.size(); i++ )
		pOwner->AddNewBuff( m_OwnerBuffList[i], pOwner->GetCharName(), "", NULL );
}

void ioCreateDummyChar9Buff::CreateChain( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwChainWeaponIndex != - 1 )
		return;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_iChainWeapon;
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();

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
}

void ioCreateDummyChar9Buff::SetChainPos( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwChainWeaponIndex == - 1 )
		return;

	if ( m_vDummyPos == ioMath::VEC3_ZERO )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwChainWeaponIndex );
	ioUroborusWeapon4 *pChainWeapon = ToUroborusWeapon4( pWeapon );
	if ( pChainWeapon )
	{
		if ( pChainWeapon->IsHasArrow() )
			pChainWeapon->SetVisible( true );

		pChainWeapon->SetPosition( m_vDummyPos );
	}	
}

void ioCreateDummyChar9Buff::SetMapEffectPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_dwBuffMapEffectID == - 1 )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwBuffMapEffectID );
	if ( pMapEffect)
		pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate( m_fOwnerAttachEffectPositionRate ) );
	
}

void ioCreateDummyChar9Buff::CheckRange( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_iDummyIndex == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwRangeCheckStartTime < dwCurTime )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
		if ( pDummy )
		{
			m_vDummyPos = pDummy->GetMidPositionByRate();

			float fDist = D3DXVec3LengthSq( &( pDummy->GetWorldPosition() - pOwner->GetWorldPosition() ) );
			if( fDist <= m_fEnableReturnPositionRange * m_fEnableReturnPositionRange )
			{
				SetReserveEndBuff();
				if ( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
					kPacket << pOwner->GetCharName();
					kPacket << GetName();
					kPacket << BS_END;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
	}
}

void ioCreateDummyChar9Buff::EndBuff()
{
	ioBuff::EndBuff();

	if ( !m_pOwner )
		return;
	
	ioPlayStage *pStage = m_pOwner->GetCreator();
	if ( !pStage )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		ioBuff *pBuff = m_pOwner->GetBuff( m_OwnerBuffList[i] );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}

	if( !m_pOwner->IsCanInvisible() )
		m_pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	if( m_bReturnPosition )
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( m_pOwner, m_szDummyName );
		if( pDummyChar )
			m_pOwner->SetWorldPosition( pDummyChar->GetWorldPosition() );
	}
	g_DummyCharMgr.DestroyDummyCharByName( m_pOwner, m_szDummyName );
	m_iDummyIndex = -1;
	if ( m_dwChainWeaponIndex != -1 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, m_dwChainWeaponIndex );
		if ( pWeapon )
			pWeapon->SetWeaponDead();

		m_dwChainWeaponIndex = -1;
	}
	
	if ( m_dwBuffMapEffectID != - 1 )
	{
		pStage->DestroyMapEffect( m_dwBuffMapEffectID );
		m_dwBuffMapEffectID = -1;
	}

	ClearData();
}

void ioCreateDummyChar9Buff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
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
	case BS_CREAT_DUMMY:
		{
			rkPacket >> m_iDummyIndex;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_szDummyName );

			m_BuffState = BS_RANGE_CHECK;

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, m_iDummyIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );

			m_dwRangeCheckStartTime = FRAMEGETTIME() + m_dwRangeCheckDisableTime;

			AddOwnerBuff( pOwner );
		}
		break;
	case BS_END:
		{
			SetReserveEndBuff();
		}
		break;
	}
}

bool ioCreateDummyChar9Buff::CheckEnableUseSkill( int iSlot )
{
	switch( iSlot )
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
