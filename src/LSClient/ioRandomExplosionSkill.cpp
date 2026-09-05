#include "StdAfx.h"
#include "ioRandomExplosionSkill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioRandomExplosionSkill::ioRandomExplosionSkill(void)
{
	ClearData();
}

ioRandomExplosionSkill::ioRandomExplosionSkill( const ioRandomExplosionSkill &rhs ):
ioAttackSkill( rhs ),
m_AreaWeaponName( rhs.m_AreaWeaponName ),
m_szLoopAni( rhs.m_szLoopAni ),
m_fLoopAniRate( rhs.m_fLoopAniRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_dwKeyAddDuration( rhs.m_dwKeyAddDuration ),
m_dwKeyEnableTime( rhs.m_dwKeyEnableTime ),
m_fCollisionRange( rhs.m_fCollisionRange ),
m_vWeaponInfoList( rhs.m_vWeaponInfoList ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_vLoopStartWeaponList( rhs.m_vLoopStartWeaponList ),
m_vLoopStartWeaponPositionList( rhs.m_vLoopStartWeaponPositionList),
m_CreateWeaponType( rhs.m_CreateWeaponType )
{
	ClearData();
}

ioRandomExplosionSkill::~ioRandomExplosionSkill(void)
{
}

void ioRandomExplosionSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwKeyCheckStartTime = 0;
	m_dwAreaWeaponIndex = -1;
	m_bAddKey = false;
	m_iCurWeaponCount = 0;
	m_dwSkillProtectStartTime = 0;
}

ioSkill* ioRandomExplosionSkill::Clone()
{
	return new ioRandomExplosionSkill( *this );
}

void ioRandomExplosionSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH],szKey[MAX_PATH];

	// areaweapon
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	//Loop
	rkLoader.LoadString_e( "skill_loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_ani_rate", 0.0f );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwKeyAddDuration	= (DWORD)rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwKeyEnableTime	= (DWORD)rkLoader.LoadInt_e( "key_enable_time", 0 );

	m_fCollisionRange = rkLoader.LoadFloat_e( "collision_range", 0.0f );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	int iCount = rkLoader.LoadInt_e( "loop_start_attack_type_count", 0 );
	m_vLoopStartWeaponList.clear();
	m_vLoopStartWeaponList.reserve( iCount );
	m_vLoopStartWeaponPositionList.clear();
	m_vLoopStartWeaponPositionList.reserve( iCount );
	for ( int i=0 ; i<iCount ; ++i )
	{
		WeaponInfo Info;
		D3DXVECTOR3 vWeaponPos = ioMath::VEC3_ZERO;
		wsprintf_e( szKey, "loop_start_attack_type%d", i+1 );
		Info.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "loop_start_attack_type%d_resist", i+1 );
		Info.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "loop_start_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "loop_start_attack_type%d_wounded_time", i+1 );
		Info.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "loop_start_attack_type%d_wounded_loop_ani", i+1 );
		Info.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "loop_start_attack_type%d_offset_x", i+1 );
		vWeaponPos.x = rkLoader.LoadFloat( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "loop_start_attack_type%d_offset_y", i+1 );
		vWeaponPos.y = rkLoader.LoadFloat( szKey, NO_WEAPON_ATTRIBUTE );
		wsprintf_e( szKey, "loop_start_attack_type%d_offset_z", i+1 );
		vWeaponPos.z = rkLoader.LoadFloat( szKey, NO_WEAPON_ATTRIBUTE );

		m_vLoopStartWeaponList.push_back( Info );
		m_vLoopStartWeaponPositionList.push_back( vWeaponPos );

	}

	iCount = rkLoader.LoadInt_e( "key_attack_type_count", 0 );
	m_vWeaponInfoList.clear();
	m_vWeaponInfoList.reserve( iCount );
	for ( int i=0 ; i<iCount ; ++i )
	{
		WeaponInfo Info;
		wsprintf_e( szKey, "key_attack_type%d", i+1 );
		Info.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "key_attack_type%d_resist", i+1 );
		Info.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "key_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "key_attack_type%d_wounded_time", i+1 );
		Info.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "key_attack_type%d_wounded_loop_ani", i+1 );
		Info.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponInfoList.push_back( Info );
	}

	m_CreateWeaponType = (CreateWeaponType)rkLoader.LoadInt( "create_weapon_type", 0 );
}

bool ioRandomExplosionSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	SetCameraBuff( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	m_iCurActiveCount = m_iMaxActiveCount;

	return true;
}

void ioRandomExplosionSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				CreateAreaWeapon( pChar );
				//루프 상태로 전환
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopDuration > 0 && m_dwMotionStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				break;;
			}

			if ( m_iCurActiveCount == 0 )
			{
				SetEndState( pChar );
				break;
			}

			if( pChar->IsNeedProcess() )
			{
				DWORD dwCheck1 = m_dwKeyCheckStartTime + m_dwKeyEnableTime;
				DWORD dwCheck2 = m_dwKeyCheckStartTime + m_dwKeyAddDuration;

				if ( !m_bAddKey && dwCheck1 < dwCheck2 )
				{
					if( m_dwKeyCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
					{
						if( pChar->IsAttackKey() )
						{
							m_bAddKey = true;
						}
					}
				}

				if( dwCheck2 < dwCurTime )
				{
					if( m_bAddKey )
					{
						m_bAddKey = false;
						m_dwKeyCheckStartTime = dwCurTime;
						if ( m_CreateWeaponType == CWT_ONE )
							CreateRandomPosWeapon( pChar );
						else if ( m_CreateWeaponType == CWT_ALL )
							CreateRandomPosAllWeapon( pChar );
					}
					else
					{
						SetEndState( pChar );
						if( pChar->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pChar->GetCharName();
							kPacket << GetName();
							kPacket << static_cast<BYTE>(ST_END);
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}
				}
			}
		}
		break;
	case SS_END:
		break;
	}	
}

void ioRandomExplosionSkill::CreateAreaWeapon( ioBaseChar *pChar )
{
	if( !pChar ) 
		return;

	if( m_AreaWeaponName.IsEmpty() ) 
		return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
		pChar->GetCharName(),
		GetName(),
		m_AreaWeaponName,
		vPos,
		pChar->GetTargetRot(),
		ioAreaWeapon::CT_ATTACHED );

	if( pArea )
		m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
}

void ioRandomExplosionSkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	m_SkillState = SS_LOOP;	
	
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID == -1 )	
		return;

	float fAniRate = FLOAT1;
	if ( fAniRate > 0.0f )
		fAniRate = m_fLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100,1.0f, FLOAT1/ fAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSkillProtectStartTime = m_dwMotionStartTime = FRAMEGETTIME();
	m_dwKeyCheckStartTime = dwCurTime;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	D3DXVECTOR3 vCharPos = pChar->GetMidPositionByRate();
	for ( int i=0 ; i<(int)m_vLoopStartWeaponList.size() ; ++i )
	{
		D3DXVECTOR3 pWeaponPos = vCharPos + ( pChar->GetWorldOrientation() * m_vLoopStartWeaponPositionList[i] );
		pChar->SkillFireExplicit( m_vLoopStartWeaponList[i], GetName(), m_dwWeaponBaseIndex++, pWeaponPos );
	}
	
}

void ioRandomExplosionSkill::CreateRandomPosWeapon( ioBaseChar *pChar )
{
	if ( !pChar ) 
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	IORandom random;
	DWORD dwSeed = pChar->GetRandomSeed() + m_iCurActiveCount;

	random.SetRandomSeed( dwSeed );

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

	int iValue = m_fCollisionRange * 2;
	int iWidth1 = random.Random( iValue );
	int iWidth2 = random.Random( iValue );
	int iWidth3 = random.Random( iValue );

	iWidth1 -= static_cast<int>( m_fCollisionRange );
	iWidth2 -= static_cast<int>( m_fCollisionRange );
	iWidth3 -= static_cast<int>( m_fCollisionRange );

	vPos.x += iWidth1;
	vPos.y += iWidth2;
	vPos.z += iWidth3;

	if ( vPos.y < pStage->GetMapHeight( vPos.x, vPos.z ) )
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );

	m_iCurActiveCount--;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	
	if ( m_iCurWeaponCount >= (int)m_vWeaponInfoList.size() )
		m_iCurWeaponCount = 0;

	pChar->SkillFireExplicit( m_vWeaponInfoList[m_iCurWeaponCount],	GetName(),
		m_dwWeaponBaseIndex, vPos );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket << static_cast<BYTE>( ST_CREATE_WEAPON );
		kPacket << vPos;
		kPacket << m_iCurWeaponCount;
		kPacket << m_iCurActiveCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurWeaponCount++;
}

void ioRandomExplosionSkill::CreateRandomPosAllWeapon( ioBaseChar *pChar )
{
	if ( !pChar ) 
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	pChar->SetRandomSeed(0);
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	std::vector<D3DXVECTOR3> vWeaponPosList;
	for ( int i=0; i<(int)m_vWeaponInfoList.size() ; ++i )
	{
		IORandom random;
		DWORD dwSeed = pChar->GetRandomSeed() + i;

		random.SetRandomSeed( dwSeed );

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();

		int iValue = m_fCollisionRange * 2;
		int iWidth1 = random.Random( iValue );
		int iWidth2 = random.Random( iValue );
		int iWidth3 = random.Random( iValue );

		iWidth1 -= static_cast<int>( m_fCollisionRange );
		iWidth2 -= static_cast<int>( m_fCollisionRange );
		iWidth3 -= static_cast<int>( m_fCollisionRange );

		vPos.x += iWidth1;
		vPos.y += iWidth2;
		vPos.z += iWidth3;

		if ( vPos.y < pStage->GetMapHeight( vPos.x, vPos.z ) )
			vPos.y = pStage->GetMapHeight( vPos.x, vPos.z );

		pChar->SkillFireExplicit( m_vWeaponInfoList[i],	GetName(),
			m_dwWeaponBaseIndex++, vPos );

		vWeaponPosList.push_back( vPos );
	}

	m_iCurActiveCount--;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket << static_cast<BYTE>( ST_CREATE_ALL_WEAPON );
		kPacket << (int)vWeaponPosList.size();
		for ( int i=0; i<(int)vWeaponPosList.size() ; ++i )
			kPacket << vWeaponPosList[i];
		kPacket << m_iCurActiveCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRandomExplosionSkill::SetEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;
}

bool ioRandomExplosionSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if ( m_SkillState == SS_END && m_fSkillEndNextJumpAmt <= 0.0f)
		return true;
	return false;
}

bool ioRandomExplosionSkill::IsEndState() const
{
	if ( m_SkillState == SS_END && m_fSkillEndNextJumpAmt > 0.0f)
		return true;

	return false;
}

bool ioRandomExplosionSkill::IsAttackEndState() const
{
	return IsEndState();
}

void ioRandomExplosionSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( m_dwAreaWeaponIndex > -1 )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
		m_dwAreaWeaponIndex = -1;
	}

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );

	EndCameraBuff( pChar );

	ClearData();
}

void ioRandomExplosionSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar )
		return;
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pChar->SetTargetRot( qtRot );

	BYTE btType;
	rkPacket >> btType;

	if ( btType == ST_CREATE_WEAPON )
	{
		D3DXVECTOR3 vWeaponPos;
		rkPacket >> vWeaponPos;
		rkPacket >> m_iCurWeaponCount;
		rkPacket >> m_iCurActiveCount;
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		pChar->SkillFireExplicit( m_vWeaponInfoList[m_iCurWeaponCount],	GetName(),
			m_dwWeaponBaseIndex, vWeaponPos );
	}
	else if ( btType == ST_CREATE_ALL_WEAPON )
	{
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		int iPosCount;
		rkPacket >> iPosCount;
		for ( int i=0 ; i<iPosCount ; ++i )
		{
			D3DXVECTOR3 vWeaponPos;
			rkPacket >> vWeaponPos;
			if ( i < (int)m_vWeaponInfoList.size() )
			{
				pChar->SkillFireExplicit( m_vWeaponInfoList[i],	GetName(),
					m_dwWeaponBaseIndex++, vWeaponPos );
			}
		}
		rkPacket >> m_iCurActiveCount;
	}
	else if ( btType == ST_END )
	{
		SetEndState( pChar );
	}
}

bool ioRandomExplosionSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		return true;
	case SS_LOOP:
		if( m_dwSkillProtectTime > 0  && m_dwSkillProtectStartTime + m_dwSkillProtectTime > dwCurTime )
			return true;
		break;
	}

	return false;
}
