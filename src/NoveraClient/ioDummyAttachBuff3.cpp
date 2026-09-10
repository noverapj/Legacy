
#include "stdafx.h"

#include "ioDummyAttachBuff3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "DummyCharDefine.h"

ioDummyAttachBuff3::ioDummyAttachBuff3()
{
	m_BuffState = BS_None;
	m_bSetReduceGauge = false;
	m_dwCheckGaugeTime = 0;
}

ioDummyAttachBuff3::ioDummyAttachBuff3( const ioDummyAttachBuff3 &rhs )
	: ioBuff( rhs ),
	  m_vDummyOffset( rhs.m_vDummyOffset ),
	  m_szDummyName( rhs.m_szDummyName ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_dwGaugeTicTime( rhs.m_dwGaugeTicTime ),
	  m_fUseGauge( rhs.m_fUseGauge ),
	  m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
	  m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
	  m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
	  m_bEnableCloakSkill( rhs.m_bEnableCloakSkill )
{
	m_BuffState = BS_None;
	m_bSetReduceGauge = false;
	m_dwCheckGaugeTime = 0;
}

ioDummyAttachBuff3::~ioDummyAttachBuff3()
{
}

void ioDummyAttachBuff3::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_szDummyName = szBuf;

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_offset_z", 0.0f );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	m_dwGaugeTicTime = rkLoader.LoadInt_e( "gauge_tic_time", 0 );
	m_fUseGauge = rkLoader.LoadFloat_e( "gauge_tic", 0.0f );

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
	
	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioDummyAttachBuff3::Clone()
{
	return new ioDummyAttachBuff3( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyAttachBuff3::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_BuffState = BS_None;
	m_bSetReduceGauge = false;
	m_dwCheckGaugeTime = 0;
}

bool ioDummyAttachBuff3::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_BuffState = BS_None;
	m_bSetReduceGauge = false;
	m_dwCheckGaugeTime = 0;
	
	return true;
}

void ioDummyAttachBuff3::ProcessBuff( float fTimePerSec )
{	
	if( m_pOwner->IsNeedProcess() && 
		m_dwBuffEndTime <= FRAMEGETTIME() )
	{
		SetEndState( m_pOwner );
		return;
	}
	
	switch( m_BuffState )
	{
	case BS_None:
		{
			SetOwnerBuffList( m_pOwner );
			CreateDummy( m_pOwner );
			SetMoveState( m_pOwner );
		}
		break;
	case BS_Move:
		{
			if ( !CheckOwnerState( m_pOwner ) )
			{
				SetEndState( m_pOwner );
				return;
			}
			CheckMove( m_pOwner );
			CheckGauge( m_pOwner );
		}
		break;
	case BS_End:
		break;
	}
}

void ioDummyAttachBuff3::EndBuff()
{	
	ProcessEnd( m_pOwner );
	ioBuff::EndBuff();
}

void ioDummyAttachBuff3::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_CreateDummy:
		{
			int iCurIndex; 
			rkPacket >> iCurIndex;
			D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, iCurIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );
		}
		break;
	case BS_End:
		{		
			SetEndState( pOwner );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioDummyAttachBuff3::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		
		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioDummyAttachBuff3::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();

	//해당 버프를 생성
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			//pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );	
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

void ioDummyAttachBuff3::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + ( qtRot * m_vDummyOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyName, iCurIndex, pOwner->GetCharName(),	vPos, 0.0f, 0, true );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CreateDummy;
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioDummyAttachBuff3::SetMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	m_BuffState = BS_Move;

	CharState nState = pOwner->GetState();
	switch( nState )
	{
	case CS_HELICOPTER:
		return;
	}

	pOwner->SetState( CS_HELICOPTER );
}

void ioDummyAttachBuff3::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_BuffState = BS_End;

	SetReserveEndBuff();
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyAttachBuff3::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->CheckUseBuffSkill( m_Name, false );

	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_OwnerBuffList[i], true );
	
	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if ( pDummyChar && pDummyChar->GetState() != DCS_DIE )
		pDummyChar->SetDieState( true );

	if ( pOwner->IsFloating() )
		pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
	else
		pOwner->SetState( CS_DELAY );

	ioItem *pItem = GetOwnerItem();
	if ( pItem )
		pItem->SetCurSkillGauge( 0.0f );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDummyAttachBuff3::CheckOwnerState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->GetState() != CS_HELICOPTER )
		return false;

	return true;
}

void ioDummyAttachBuff3::CheckGauge( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || !m_bSetReduceGauge )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckGaugeTime + m_dwGaugeTicTime < dwCurTime )
	{
		m_dwCheckGaugeTime = dwCurTime;
		if( pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, m_fUseGauge ) <= 0.0f )
			SetEndState( pOwner );
	}
}

void ioDummyAttachBuff3::CheckMove( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if( !pDummy )
		return;

	pOwner->SetWorldPosition( pDummy->GetWorldPosition() );
	pOwner->SetWorldOrientation( pDummy->GetWorldOrientation() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDummyAttachBuff3::CheckEnableUseSkill( int iSlot )
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

void ioDummyAttachBuff3::SetReduceStartGauge()
{
	m_bSetReduceGauge = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioDummyAttachBuff3::CheckDummy()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if( pDummy && pDummy->GetState() != DCS_DIE )
		return true;

	return false;
}

D3DXVECTOR3 ioDummyAttachBuff3::GetDummyCharPos()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return ioMath::VEC3_ZERO;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_szDummyName );
	if( pDummy )
		return pDummy->GetWorldPosition();

	return pOwner->GetWorldPosition();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHelicopterSpecialState::ioHelicopterSpecialState()
{
}

ioHelicopterSpecialState::~ioHelicopterSpecialState()
{
}

void ioHelicopterSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioHelicopterSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioHelicopterSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 && !pOwner->IsBuffLimitSkill() )
		pOwner->SetUseSkill( iSkillNum, SUT_NORMAL );
}

void ioHelicopterSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioHelicopterSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}