

#include "stdafx.h"

#include "ioMysteryTargetPosChnageSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ioAttackAttribute.h"

ioMysteryTargetPosChnageSkill::ioMysteryTargetPosChnageSkill()
{
	Init();
}

ioMysteryTargetPosChnageSkill::ioMysteryTargetPosChnageSkill( const ioMysteryTargetPosChnageSkill &rhs )
	: ioAttackSkill( rhs ),
	m_AttributeList( rhs.m_AttributeList ),
	m_EndAttribute( rhs.m_EndAttribute ),
	m_szDummyBuffName( rhs.m_szDummyBuffName ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwTeleportTime( rhs.m_dwTeleportTime ),
	m_szTeleportRedEffect( rhs.m_szTeleportRedEffect ),
	m_szTeleportBlueEffect( rhs.m_szTeleportBlueEffect ),
	m_iTeleportHeightOffset( rhs.m_iTeleportHeightOffset ),
	m_SkillEndState( rhs.m_SkillEndState )
{
	Init();
}

ioMysteryTargetPosChnageSkill::~ioMysteryTargetPosChnageSkill()
{
	Init();
}

void ioMysteryTargetPosChnageSkill::Init()
{	
	m_TargetCharList.clear();
	m_ChangePosList.clear();

	m_bRefreshFireTimeList	= true;
	m_bSetHandMesh			= false;
	m_iCurrAttribute		= 0;
	m_dwStartChangePosTime	= 0;
	m_State					= SS_DELAY;
	m_KeyState				= KS_NONE;
}

void ioMysteryTargetPosChnageSkill::CreateMapEffect( ioBaseChar* pChar, ioPlayStage* pStage )
{
	if( m_TargetCharList.empty() )
		return;

	DestoryMapEffect( pChar );
	
	for( ioHashStringVec::iterator iter = m_TargetCharList.begin(); iter != m_TargetCharList.end(); ++iter ) 
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( *iter );
		if( !pTarget )
			continue;

		ioMapEffect* pMapEffect = NULL;
		if( pChar->GetTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szTeleportBlueEffect, pTarget->GetWorldPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		else
			pMapEffect = pStage->CreateMapEffect( m_szTeleportRedEffect, pTarget->GetWorldPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

		if( pMapEffect )
		{
			m_UniqueObjIDList.push_back( pMapEffect->GetUniqueID() );
		}
	}
}

void ioMysteryTargetPosChnageSkill::DestoryMapEffect( ioBaseChar* pChar )
{
	for( UniqueObjIDList::iterator iter = m_UniqueObjIDList.begin(); iter != m_UniqueObjIDList.end(); ++iter )
	{
		pChar->EndEffect( *iter );
	}

	m_UniqueObjIDList.clear();
}

void ioMysteryTargetPosChnageSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString( "dummy_buff", "", szBuf, MAX_PATH );
	m_szDummyBuffName = szBuf;

	rkLoader.LoadString( "teleport_red_effect", "", szBuf, MAX_PATH );
	m_szTeleportRedEffect = szBuf;

	rkLoader.LoadString( "teleport_blue_effect", "", szBuf, MAX_PATH );
	m_szTeleportBlueEffect = szBuf;
	
	m_dwSkillProtectTime	= rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_iTeleportHeightOffset	= rkLoader.LoadFloat_e( "teleport_height_offset", 0.0f );

	LoadAttribute( rkLoader );

	//추가
	m_SkillEndState = ( SkillEndState )rkLoader.LoadInt_e( "skill_end_state", SES_NONE );
}

void ioMysteryTargetPosChnageSkill::LoadAttribute( ioINILoader &rkLoader )
{
	m_AttributeList.clear();

	int iCount = rkLoader.LoadInt_e( "max_count", 0 );

	char szBuf[MAX_PATH];
	for( int i = 0; i < iCount; ++i )
	{
		AttackAttribute attr;
		sprintf_s_e( szBuf, "attack%d", i+1 );
		LoadAttackAttribute( szBuf, attr, rkLoader );	
		m_AttributeList.push_back( attr );
	}
	
	m_EndAttribute.Init();
	LoadAttackAttribute( "end_animation", m_EndAttribute, rkLoader );

	m_dwTeleportTime = rkLoader.LoadInt_e( "end_animation_teleport_time", 300 );
}

ioSkill* ioMysteryTargetPosChnageSkill::Clone()
{
	return new ioMysteryTargetPosChnageSkill( *this );
}

bool ioMysteryTargetPosChnageSkill::HasSkillMotion() const
{
	return !m_AttributeList.empty();
}

bool ioMysteryTargetPosChnageSkill::OnSkillStart( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillStart( pChar );
	Init();	

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwSkillStartTime = FRAMEGETTIME();

	return true;
}

void ioMysteryTargetPosChnageSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckAttackKey( pChar );
	
	switch( m_State )
	{
	case SS_DELAY:		
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				SetActionState( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			OnActionState( pChar, pStage );
		}
		break;
	case SS_END:
		{
			OnEndState( pChar, pStage );
		}
		break;
	}
}

void ioMysteryTargetPosChnageSkill::CheckAttackKey( ioBaseChar *pChar )
{
	DWORD dwCurrTime = FRAMEGETTIME();
	if( m_State != SS_DELAY )
	{
		if( pChar->IsAttackKeyDown() || pChar->IsAttackKeyPress() )
		{
			m_KeyState = KS_PRESS;
		}
		else if( pChar->IsAttackKeyRelease() )
		{
			m_KeyState = KS_RELEASE;
		}
	}
}

AttackAttribute& ioMysteryTargetPosChnageSkill::GetAttribute()
{
	static AttackAttribute kNullAttribute;
	if( m_State == SS_END )
	{
		return m_EndAttribute;
	}
	else
	{
		if( COMPARE( m_iCurrAttribute, 0, static_cast<int>( m_AttributeList.size() ) ) )
			return m_AttributeList[m_iCurrAttribute];
	}

	kNullAttribute.Init();
	return kNullAttribute;
}


const AttackAttribute& ioMysteryTargetPosChnageSkill::GetAttributeConst() const
{
	static const AttackAttribute kNullAttribute;
	if( m_State == SS_END )
	{
		return m_EndAttribute;
	}
	else
	{
		if( COMPARE( m_iCurrAttribute, 0, static_cast<int>( m_AttributeList.size() ) ) )
			return m_AttributeList[m_iCurrAttribute];
	}
	return kNullAttribute;
}

void ioMysteryTargetPosChnageSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )	
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioMysteryTargetPosChnageSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_dwPreDelayStartTime = 0;
	EndPreDelayEffect( pChar );
	m_State = SS_ACTION;
	AttackSkillFire( pChar );

	//네크워크 패킷
	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<byte>( S_ACTION );
		kPacket << m_iCurrAttribute;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioMysteryTargetPosChnageSkill::OnActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar->IsNeedProcess() )
		return;
	
	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_iCurrAttribute++;
		if( m_KeyState == KS_PRESS && m_iCurrAttribute < static_cast<int>( m_AttributeList.size() ) )
		{			
			SetActionState( pChar, pStage );
		}
		else
		{
			SetEndState( pChar, pStage );
		}
	}

	//키가 눌리지 않았을 경우 motion이 끝난 후 체크가 아닌 즉시 EndState로
	if ( m_SkillEndState == SES_DIRECT )
	{
		if ( m_KeyState == KS_RELEASE )
		{
			SetEndState( pChar, pStage );
		}
	}	
}

void ioMysteryTargetPosChnageSkill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if(	!m_szDummyBuffName.IsEmpty() && pChar->IsNeedProcess() )
	{
		const BaseCharList &rkTargetList = pStage->GetBaseCharList();
		for( BaseCharList::const_iterator iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
		{
			ioBaseChar *pTarget = *iter;
			if( !pTarget ) 
				continue;

			if( pTarget == pChar )
				continue;	

			ioBuff* pBuff = pTarget->GetBuff( BT_DUMMY_BUFF );
			if( !pBuff )
				continue;

			//스킬시전자에 의해 걸린 더미 버프가 아닌 경우는 패스
			if( pBuff->GetCreatorName() != pChar->GetCharName() )
				continue;

			//스킬용 더미버프인지 확인
			if( pBuff->GetName() != m_szDummyBuffName )
				continue;

			m_TargetCharList.push_back( pTarget->GetCharName() );

			ChangePos Pos;
			Pos.szBanName	= pTarget->GetCharName();
			Pos.vPos		= pTarget->GetWorldPosition();
			m_ChangePosList.push_back( Pos );	
		}
	
		if( !m_ChangePosList.empty() )
			std::random_shuffle( m_ChangePosList.begin(), m_ChangePosList.end() );
	}

	CreateMapEffect( pChar, pStage );

	m_State					= SS_END;
	m_dwStartChangePosTime	= FRAMEGETTIME();

	//스킬 종료 모션 재생을 AttackSkillFire를 통해서 함으로 아래 플래그로 해당 함수 내의 RefreshFireTimeListWithBase()함수 사용을 블럭
	m_bRefreshFireTimeList	= false;	

	//앤드 에니메이션 재생
	AttackSkillFire( pChar );

	//네트워크 동기화 패킷
	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<byte>( S_END );
		int iSize = static_cast<int>( m_TargetCharList.size() );
		kPacket << iSize;
		for( int i = 0; i < iSize; ++i )
		{
			kPacket << m_TargetCharList[i];
		}
		iSize = static_cast<int>( m_ChangePosList.size() );
		kPacket << iSize;
		for( int i = 0; i < iSize; ++i )
		{
			kPacket << m_ChangePosList[i].szBanName;
			kPacket << m_ChangePosList[i].vPos;
		}

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioMysteryTargetPosChnageSkill::OnEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwStartChangePosTime != 0 && m_dwStartChangePosTime + m_dwTeleportTime < FRAMEGETTIME() )
	{
		for( ioHashStringVec::iterator iter = m_TargetCharList.begin(); iter != m_TargetCharList.end(); ++iter )
		{
			ioBaseChar* pTarget = pStage->GetBaseChar( *iter );
			if( !pTarget )
				continue;

			D3DXVECTOR3 vPos = GetRandPos( pTarget->GetCharName() );
			vPos.y += m_iTeleportHeightOffset;
			pTarget->SetWorldPosition( vPos );

			ioBuff* pBuff = pTarget->GetBuff( m_szDummyBuffName );
			if( pBuff )
				pBuff->SetReserveEndBuff();
		}

		DestoryMapEffect( pChar );
		m_dwStartChangePosTime = 0;
	}
}

D3DXVECTOR3 ioMysteryTargetPosChnageSkill::GetRandPos( ioHashString szBanName )
{
	if( m_ChangePosList.size() == 1 )
	{
		D3DXVECTOR3 vPos = (*m_ChangePosList.begin() ).vPos;
		m_ChangePosList.clear();
		return vPos;
	}

	for( ChangePosList::iterator iter = m_ChangePosList.begin(); iter != m_ChangePosList.end(); ++iter )
	{
		const ChangePos& rkPos = *iter;
		if( rkPos.szBanName == szBanName )
			continue;

		D3DXVECTOR3 vPos = rkPos.vPos;
		m_ChangePosList.erase( iter );
		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioMysteryTargetPosChnageSkill::IsAttackEndState() const
{
	if( m_State == SS_END && m_dwStartChangePosTime == 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
		return true;

	return false;
}

bool ioMysteryTargetPosChnageSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return IsAttackEndState();
}

void ioMysteryTargetPosChnageSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	
	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	
	Init();
	DestoryMapEffect( pChar ); 
}

bool ioMysteryTargetPosChnageSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioMysteryTargetPosChnageSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}
		
	if( m_State == SS_DELAY && m_bPreDelayProtect )
		return true;
		
	if( 0 < m_dwSkillProtectTime && FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
		return true;

	return false;
}

void ioMysteryTargetPosChnageSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	byte cState = 0;
	rkPacket >> cState;

	switch( cState )
	{
	case S_ACTION:
		{
			rkPacket >> m_iCurrAttribute;
			SetActionState( pChar, pStage );
		}
		break;
	case S_END:
		{
			m_TargetCharList.clear();
			m_ChangePosList.clear();

			int iSize = 0;
			rkPacket >> iSize;

			for( int i = 0; i < iSize; ++i )
			{
				ioHashString szName;
				rkPacket >> szName;
				m_TargetCharList.push_back( szName );
			}

			rkPacket >> iSize;
			for( int i = 0; i < iSize; ++i )
			{
				ChangePos Change;
				rkPacket >> Change.szBanName;
				rkPacket >> Change.vPos;
				m_ChangePosList.push_back( Change );
			}

			SetEndState( pChar, pStage );
		}
		break;
	}
}

void ioMysteryTargetPosChnageSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );

	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( false, true );
		}
	}
}
