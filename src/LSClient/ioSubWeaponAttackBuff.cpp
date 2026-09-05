#include "StdAfx.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioSubWeaponAttackBuff.h"
#include "WeaponDefine.h"


ioSubWeaponAttackBuff::ioSubWeaponAttackBuff(void)
{
	m_iCurAttackIdx = 0;
}

ioSubWeaponAttackBuff::ioSubWeaponAttackBuff( const ioSubWeaponAttackBuff& rhs ) 
	: ioBuff( rhs ),
	m_fColRange( rhs.m_fColRange ),
	m_fColAngle( rhs.m_fColAngle ),
	m_RemoveBuffList( rhs.m_RemoveBuffList ),
	m_fStartJumpPower( rhs.m_fStartJumpPower ),
	m_fEndJumpPower( rhs.m_fEndJumpPower ),
	m_SubWeaponInfoList( rhs.m_SubWeaponInfoList ),
	m_DisableInputKeyState( rhs.m_DisableInputKeyState ),
	m_szAttackSoundName( rhs.m_szAttackSoundName )
{
	m_iCurAttackIdx = 0;
}

ioSubWeaponAttackBuff::~ioSubWeaponAttackBuff(void)
{
	m_DisableInputKeyState.clear();
}

void ioSubWeaponAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	//타겟 설정
	m_fColAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fColRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	char szBuf[MAX_PATH] = "";
	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}

	m_fStartJumpPower = rkLoader.LoadFloat_e( "start_jump_power", 0.0f );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );

	char szKey[MAX_PATH] = "";
	m_SubWeaponInfoList.clear();
	int nMaxSubCnt = rkLoader.LoadInt_e( "max_sub_weapon_cnt", 0 );
	for ( int i=0; i<nMaxSubCnt; i++ )
	{
		SubWeaponInfo sSubWeaponInfo;
		wsprintf_e( szKey, "sub_weapon%d_effect",i+1);
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sSubWeaponInfo.m_szWeaponEffect = szBuf;

		wsprintf_e( szKey, "sub_weapon%d_angle", i+1 );
		sSubWeaponInfo.m_fWeaponAngle = rkLoader.LoadFloat( szKey, 0.0f );

		D3DXVECTOR3 vPos;
		wsprintf_e( szKey, "sub_weapon%d_offset_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_y", i+1 );
		vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szKey, 0.0f );
		sSubWeaponInfo.m_vWeaponPos = vPos;

		wsprintf_e( szKey, "sub_weapon%d_attack_type_count", i+1 );
		int nMaxAttackCnt = rkLoader.LoadInt( szKey, 0 );
		for ( int j=0; j<nMaxAttackCnt; j++ )
		{
			WeaponInfo sWeaponInfo;
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d", i+1, j+1 );
			sWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_resist", i+1, j+1 );
			sWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			sWeaponInfo.m_WoundedAnimation = szBuf;
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded_time", i+1, j+1 );
			sWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded_loop_ani", i+1, j+1 );
			sWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			sSubWeaponInfo.m_WeaponInfoList.push_back( sWeaponInfo );
		}

		m_SubWeaponInfoList.push_back( sSubWeaponInfo );
	}

	m_DisableInputKeyState.clear();
	int nMaxState = rkLoader.LoadInt_e( "max_disable_input_state", 0 );
	for ( int i=0; i<nMaxState; ++i )
	{
		wsprintf_e( szBuf, "disable_input_state%d", i+1 );
		int nState = rkLoader.LoadInt( szBuf, -1 );
		m_DisableInputKeyState.push_back( nState );
	}

	rkLoader.LoadString_e( "attack_sound", "", szBuf, MAX_PATH );
	m_szAttackSoundName = szBuf;
}

ioBuff* ioSubWeaponAttackBuff::Clone()
{
	return new ioSubWeaponAttackBuff( *this );
}

void ioSubWeaponAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_iCurActiveCount = m_iMaxActiveCount;

	/*if ( pOwner->IsNeedProcess() )
	{
		SetCameraEvent( pOwner );
		SetCameraBuff( pOwner );
	}*/

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );
	//공중에 있는 상태
	if ( pOwner->IsFloating() )
	{
		if ( m_fStartJumpPower > 0.0f )
			pOwner->SetSKillEndJumpState( m_fStartJumpPower, false, false, false, true );
	}

	CreateSubWeaponEffect( pOwner );
}

bool ioSubWeaponAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_iCurActiveCount = m_iMaxActiveCount;

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );
	//공중에 있는 상태
	if ( pOwner->IsFloating() )
	{
		if ( m_fStartJumpPower > 0.0f )
			pOwner->SetSKillEndJumpState( m_fStartJumpPower, false, false, false, true );
	}

	CreateSubWeaponEffect( pOwner );

	return true;
}

void ioSubWeaponAttackBuff::ProcessBuff( float fTimePerSec )
{
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

	DWORD dwCurTime = FRAMEGETTIME();
	UpdateSubWeaponEffect( m_pOwner );
	FindTarget( m_pOwner );
	CheckInputKey( m_pOwner );
}

void ioSubWeaponAttackBuff::EndBuff()
{	
	ioBuff::EndBuff();

	//버프 제거
	for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
		m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
	m_RemoveBuffList.clear();

	for( int i = 0; i < (int)m_SubWeaponEffectList.size(); i++ )
	{
		if( m_SubWeaponEffectList[i] != -1 )
			DestroyEffect( m_pOwner, m_SubWeaponEffectList[i] );
	}
}

void ioSubWeaponAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	rkPacket >> m_szTargetName;
	rkPacket >> m_iCurAttackIdx;
	SetAttackState( pOwner );
}

bool ioSubWeaponAttackBuff::FindTarget( ioBaseChar *pOwner )
{	
	if( !pOwner ) 
		return false;

	//타겟 리스트
	BaseCharList vTargetList;

	//위치 설정
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	//타겟 리스트 얻기
	pOwner->GetTargetCharListBySphere( vPos, m_fColRange, m_fColAngle, vTargetList );

	//타겟 리스트가 비어있다면
	if ( vTargetList.empty() )
		return false;

	//타겟 리스트 소팅
	SortTargetList( pOwner, vTargetList );

	//회전
	if ( !m_SortList.empty() )
	{
		ioBaseChar *pTarget = m_SortList[0].pChar;
		if ( pTarget )
		{
			m_szTargetName = pTarget->GetCharName();
			return true;
		}
	}

	return false;
}

//타겟 리스트 가까운 순서로 정렬
void ioSubWeaponAttackBuff::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList )
{
	if( !pOwner )	
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.reserve( TargetList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != pOwner )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pOwner->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pCurChar ) )
				continue;

			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.pChar = pCurChar;
			sDist.fDistSq = fCurDistSq;
			m_SortList.push_back( sDist );
		}
	}

	std::sort( m_SortList.begin(), m_SortList.end() );
}

//타겟 가능한지 확인
bool ioSubWeaponAttackBuff::IsEnableTarget( ioBaseChar *pTarget )
{
	if ( !pTarget || pTarget->HasHideBuff() || pTarget->IsPrisonerMode() )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	/*CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
	{
	case CS_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() && !m_bSetDownAim )
				return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( !pTarget->IsFloatingState() && !m_bSetDownAim )
				return false;
		}
	case CS_ETC_STATE:
		{
			if ( pTarget->IsApplyDownState(false) && !m_bSetDownAim )
				return false;
		}
	case CS_USING_SKILL:
		{
			if ( pTarget->IsApplyDownState(false) && !m_bSetDownAim )
				return false;
		}
	}*/

	return true;
}

void ioSubWeaponAttackBuff::CreateSubWeaponEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for ( int i=0; i<(int)m_SubWeaponInfoList.size(); i++ )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_SubWeaponInfoList[i].m_vWeaponPos;
		DWORD dwEffectID = CreateEffect( pOwner, m_SubWeaponInfoList[i].m_szWeaponEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		m_SubWeaponEffectList.push_back( dwEffectID );
	}
}

void ioSubWeaponAttackBuff::UpdateSubWeaponEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || m_SubWeaponEffectList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nEffectCnt = (int)m_SubWeaponEffectList.size();
	if ( nEffectCnt > 0 && nEffectCnt <= (int)m_SubWeaponInfoList.size() )
	{
		for ( int i=0; i<nEffectCnt; i++ )
		{
			DWORD dwEffectID = m_SubWeaponEffectList[i];
			ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
			if ( pSubWeaponEffect )
			{
				D3DXVECTOR3 vNewPos = pOwner->GetMidPositionByRate() + ( pOwner->GetWorldOrientation() * m_SubWeaponInfoList[i].m_vWeaponPos );
				D3DXQUATERNION qtRotate = pOwner->GetWorldOrientation();

				pSubWeaponEffect->SetWorldPosition( vNewPos );
				pSubWeaponEffect->SetWorldOrientation( qtRotate );
			}
		}
	}
}

DWORD ioSubWeaponAttackBuff::CreateEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pOwner || szEffect.IsEmpty() )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioSubWeaponAttackBuff::DestroyEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioSubWeaponAttackBuff::CheckInputKey(  ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() || CheckDisableKey( pOwner ) )
		return;

	int iSlot = GetOwnerItem()->GetType() - 1;
	if( m_pOwner->GetSkillKeyInput() == iSlot )
		SetAttackState( pOwner );
}

bool ioSubWeaponAttackBuff::CheckDisableKey( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	int nOwnerState = (int)pOwner->GetState();
	for ( int i=0; i<(int)m_DisableInputKeyState.size(); ++i )
	{
		if ( nOwnerState == m_DisableInputKeyState[i] )
			return true;
	}

	return false;
}

void ioSubWeaponAttackBuff::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( !COMPARE( m_iCurAttackIdx, 0, (int)m_SubWeaponInfoList.size() ) )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	//D3DXVECTOR3 vWeaponPos = vPos + m_SubWeaponInfoList[m_iCurAttackIdx].m_vWeaponPos;
	D3DXVECTOR3 vWeaponPos = vPos + qtRot * m_SubWeaponInfoList[m_iCurAttackIdx].m_vWeaponPos;

	ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pTarget )
	{
		vDir = pTarget->GetMidPositionByRate() - vWeaponPos;
		D3DXVec3Normalize( &vDir, &vDir );
	}
	D3DXVec3Normalize( &vDir, &vDir );

	WeaponInfoList weaponInfoList = m_SubWeaponInfoList[m_iCurAttackIdx].m_WeaponInfoList;
	for( int i = 0; i < (int)weaponInfoList.size(); i++ )
	{
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME() + weaponInfoList[i].m_dwCallTime;
		kFireTime.iAttributeIdx = weaponInfoList[i].m_iWeaponIdx;
		kFireTime.iResistanceIdx = weaponInfoList[i].m_iResistanceIdx;
		kFireTime.szWoundedAni  = weaponInfoList[i].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = weaponInfoList[i].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = weaponInfoList[i].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
		kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;

		pOwner->ExtendFireExplicit( kFireTime, vWeaponPos, vDir, "" );
	}

	DestroyEffect( pOwner, m_SubWeaponEffectList[m_iCurAttackIdx] );

	pOwner->PlaySound( m_szAttackSoundName );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << m_szTargetName;
		kPacket << m_iCurAttackIdx;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//if( pOwner->IsNeedProcess() )
	{
		m_iCurAttackIdx++;
		m_iCurActiveCount = m_iMaxActiveCount - m_iCurAttackIdx;
	}

	if( m_iCurAttackIdx >= m_iMaxActiveCount )
		SetReserveEndBuff();
}