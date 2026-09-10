#include "StdAfx.h"
#include "ioCreatWeaponByKeyInputAttackSkill.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"
ioCreatWeaponByKeyInputAttackSkill::ioCreatWeaponByKeyInputAttackSkill(void)
{
	ClearData();
}

ioCreatWeaponByKeyInputAttackSkill::ioCreatWeaponByKeyInputAttackSkill( const ioCreatWeaponByKeyInputAttackSkill &rhs ):
ioAttackSkill( rhs ),
m_OwnerBuffList( rhs.m_OwnerBuffList ),
//Pre Action
m_PreAttribute( rhs.m_PreAttribute ),
m_szPreActionAreaWeaponName( rhs.m_szPreActionAreaWeaponName ),
m_vPreActionAreaWeaponOffset( rhs.m_vPreActionAreaWeaponOffset ),
//Loop State
m_szLoopAnimation( rhs.m_szLoopAnimation ),
m_fLoopAniRate( rhs.m_fLoopAniRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_bCharRotateByDirKey( rhs.m_bCharRotateByDirKey ),
//Attack
m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
m_DKeyAttributeList( rhs.m_DKeyAttributeList ),
m_SKeyAttributeList( rhs.m_SKeyAttributeList ),
m_DirectionKeyWithDKeyAttributeList( rhs.m_DirectionKeyWithDKeyAttributeList ),
m_bUseDirectionKeyCheck( rhs.m_bUseDirectionKeyCheck ),
m_bUseAttackKey( rhs.m_bUseAttackKey ),
m_bUseDefenseKey( rhs.m_bUseDefenseKey ),
//End
m_szEndAnimation( rhs.m_szEndAnimation ),
m_fEndAniRate( rhs.m_fEndAniRate ),
//Effect
m_szMapEffectName( rhs.m_szMapEffectName ),
m_iMaxMapEffectCount( rhs.m_iMaxMapEffectCount ),
m_vMapEffectOffsetList( rhs.m_vMapEffectOffsetList ),
//dummy
m_DummyCharList( rhs.m_DummyCharList ),
m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList )
{
	ClearData();
	RoomOptionInfoList22 dddd;
}


ioCreatWeaponByKeyInputAttackSkill::~ioCreatWeaponByKeyInputAttackSkill(void)
{
}

void ioCreatWeaponByKeyInputAttackSkill::ClearData()
{
	m_SkillState = SS_NONE;
	m_dwLoopStartTime = 0;
	m_iCurAttackCount = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwMapEffectIDList.clear();
}

ioSkill* ioCreatWeaponByKeyInputAttackSkill::Clone()
{
	return new ioCreatWeaponByKeyInputAttackSkill( *this );
}

void ioCreatWeaponByKeyInputAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioAttackSkill::LoadProperty( rkLoader );

	//Loop
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_szLoopAnimation = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "loop_ani_rate", 0.0f );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "loop_duration", 0 );
	m_bCharRotateByDirKey = rkLoader.LoadBool_e( "char_rotate_by_dir_key", false );

	//Attack
	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_attack_cnt", 0 );
	m_bUseDirectionKeyCheck = rkLoader.LoadBool_e( "use_direction_key_check", false );

	m_bUseAttackKey = rkLoader.LoadBool_e( "use_attack_key", false );
	m_bUseDefenseKey = rkLoader.LoadBool_e( "use_defense_key", false );
	//End
	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_szEndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", 0.0f );

	LoadAttackAttribute( "pre_action", m_PreAttribute, rkLoader );
	rkLoader.LoadString_e( "pre_action_area_weapon_name", "", szBuf, MAX_PATH );
	m_szPreActionAreaWeaponName = szBuf;
	m_vPreActionAreaWeaponOffset.x = rkLoader.LoadFloat_e( "pre_action_area_weapon_offset_x", 0.0f );
	m_vPreActionAreaWeaponOffset.y = rkLoader.LoadFloat_e( "pre_action_area_weapon_offset_y", 0.0f );
	m_vPreActionAreaWeaponOffset.z = rkLoader.LoadFloat_e( "pre_action_area_weapon_offset_z", 0.0f );


	//Effect
	rkLoader.LoadString_e( "map_effect_name", "", szBuf, MAX_PATH );
	m_szMapEffectName = szBuf;
	m_vMapEffectOffsetList.clear();
	m_iMaxMapEffectCount = rkLoader.LoadInt_e( "map_effect_count", 0 );
	if ( m_iMaxMapEffectCount > 0 )
	{
		m_vMapEffectOffsetList.reserve( m_iMaxMapEffectCount );
		for ( int i=0 ; i<m_iMaxMapEffectCount ; ++i )
		{
			D3DXVECTOR3 vOffset = ioMath::VEC3_ZERO;
			wsprintf_e( szBuf, "map_effect%d_offset_x", i+1 );
			vOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
			wsprintf_e( szBuf, "map_effect%d_offset_y", i+1 );
			vOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
			wsprintf_e( szBuf, "map_effect%d_offset_z", i+1 );
			vOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

			m_vMapEffectOffsetList.push_back( vOffset );
		}
	}

	LoadBuffList( rkLoader );
	LoadKeyAttribute( rkLoader );

	//dummy
	m_DummyCharList.clear();
	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

void ioCreatWeaponByKeyInputAttackSkill::LoadBuffList( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );
		ioHashString szBuffName = szBuff;

		if( !szBuffName.IsEmpty() )
		{
			int iType = g_BuffMaker.GetBuffType( szBuffName );
			if( iType != BT_NONE )
			{
				ioBuffInfo kBuffInfo( szBuffName, iType );
				m_OwnerBuffList.push_back( kBuffInfo );
			}
		}
	}
}

void ioCreatWeaponByKeyInputAttackSkill::LoadKeyAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	int iDkeyAttackCount = rkLoader.LoadInt_e( "d_key_attack_count", 0 );
	m_DKeyAttributeList.clear();

	if( iDkeyAttackCount > 0 )
	{
		m_DKeyAttributeList.reserve( iDkeyAttackCount );
		for( int i=0; i < iDkeyAttackCount; ++i )
		{
			AttackAttribute DKeyAttribute;

			wsprintf_e( szBuf, "d_key_attack%d", i+1 );
			LoadAttackAttribute( szBuf, DKeyAttribute, rkLoader );

			m_DKeyAttributeList.push_back( DKeyAttribute );
		}
	}

	int iSkeyAttackCount = rkLoader.LoadInt_e( "s_key_attack_count", 0 );
	m_SKeyAttributeList.clear();

	if( iSkeyAttackCount > 0 )
	{
		m_SKeyAttributeList.reserve( iSkeyAttackCount );
		for( int i=0; i < iSkeyAttackCount; ++i )
		{
			AttackAttribute SKeyAttribute;

			wsprintf_e( szBuf, "s_key_attack%d", i+1 );
			LoadAttackAttribute( szBuf, SKeyAttribute, rkLoader );

			m_SKeyAttributeList.push_back( SKeyAttribute );
		}
	}

	int iDirectionKeyWithDKeyAttackCount = rkLoader.LoadInt_e( "direction_d_key_attack_count", 0 );
	m_DirectionKeyWithDKeyAttributeList.clear();

	if( iDirectionKeyWithDKeyAttackCount > 0 )
	{
		m_DirectionKeyWithDKeyAttributeList.reserve( iSkeyAttackCount );
		for( int i=0; i < iDirectionKeyWithDKeyAttackCount; ++i )
		{
			AttackAttribute DirectionDKeyAttribute;

			wsprintf_e( szBuf, "direction_d_key_attack%d", i+1 );
			LoadAttackAttribute( szBuf, DirectionDKeyAttribute, rkLoader );

			m_DirectionKeyWithDKeyAttributeList.push_back( DirectionDKeyAttribute );
		}
	}

	
}

bool ioCreatWeaponByKeyInputAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bSetHandMesh = false;
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	SetCameraBuff( pChar );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ClearData();

	AddOwnerBuff( pChar );

	CreateSkillMapEfffect( pChar );

	return true;
}

void ioCreatWeaponByKeyInputAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	//g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	RemoveOwnerBuff( pChar );

	for ( int i= 0 ;i<(int)m_dwMapEffectIDList.size() ; ++i )
		DestroySkillMapEffect( pChar, m_dwMapEffectIDList[i] );
	m_dwMapEffectIDList.clear();
	ClearData();
}

void ioCreatWeaponByKeyInputAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelayTime = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				if ( !SetPreAttackState( pChar ) )
					SetLoopState( pChar, true );
			}
		}
		break;
	case SS_PRE_ACTION:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, true );
				return;
			}

			if ( pChar->IsNeedProcess() )
			{
				if ( m_dwPreActionFireStartTime > 0 && m_dwPreActionFireStartTime < dwCurTime )
				{
					CreateAreaWeapon( pChar, m_szPreActionAreaWeaponName, m_vPreActionAreaWeaponOffset );
					m_dwPreActionFireStartTime = 0;
				}
			}
			
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopDuration > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}

			if ( pChar->IsNeedProcess() )
			{
				if( CheckKeyInput( pChar ) )
					return;
			}

		}
		break;
	case SS_ACTION:
		{
			if ( m_dwAttackFireStartTime && m_dwAttackFireStartTime < FRAMEGETTIME() )
			{
				m_dwAttackFireStartTime = 0;
				CreateDummyChar( pChar );
			}

			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_iMaxAttackCount > 0 && m_iCurAttackCount >= m_iMaxAttackCount )
				{
					SetEndState( pChar );
					return;
				}
				else
				{
					SetLoopState( pChar, false );
					return;
				}
			}
		}
		break;
	}
}

bool ioCreatWeaponByKeyInputAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_SkillState == SS_END && m_dwMotionEndTime < dwCurTime )
		return true;

	return false;
}

AttackAttribute& ioCreatWeaponByKeyInputAttackSkill::GetAttribute()
{
	return const_cast<AttackAttribute&>( GetAttributeConst() );
}

const AttackAttribute& ioCreatWeaponByKeyInputAttackSkill::GetAttributeConst() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return GetAttributeNormalConst();
	case SUT_DOWN:
		if( !m_DownAttackAttribute.m_AttackAnimation.IsEmpty() )
			return m_DownAttackAttribute;
		return m_AttackAttribute;
	case SUT_JUMP:
		if( !m_JumpAttackAttribute.m_AttackAnimation.IsEmpty() )
			return m_JumpAttackAttribute;
		return m_AttackAttribute;
	}

	return m_AttackAttribute;
}

void ioCreatWeaponByKeyInputAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar || !pStage )
		return;
	int iType;
	rkPacket >> iType;
	if ( iType == SS_PRE_ACTION )
	{
		CreateAreaWeapon( pChar, m_szPreActionAreaWeaponName, m_vPreActionAreaWeaponOffset );
	}
	else if ( iType == SS_LOOP )
	{
		bool bAttack = false ;
		bool bCangeDirection = false;
		int iDir, iKeyInput;
		rkPacket >> iKeyInput;
		rkPacket >> bAttack;
		rkPacket >> bCangeDirection;
		rkPacket >> iDir;
		m_KeyInputState = (KeyInputState)iKeyInput;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

		if ( bCangeDirection )
			pChar->SetTargetRotToDirKey( m_CurDirKey );

		if ( bAttack )
			SetActionState( pChar );
	}
}

void ioCreatWeaponByKeyInputAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if ( !pWeapon || !pStage )
		return;
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{				
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}		
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

//Add Owner Buff When Skill Start
void ioCreatWeaponByKeyInputAttackSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.size() <= 0 )
		return;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( !m_OwnerBuffList[i].m_BuffName.IsEmpty() )
		{
			pChar->AddNewBuffWithAnimateTime( m_OwnerBuffList[i].m_BuffName,
				pChar->GetCharName(),
				szItemName,
				this,
				GetSkillMotionTime() );
		}
	}
}
//Delete Owner Buff When Skill End
void ioCreatWeaponByKeyInputAttackSkill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	int iCnt = m_OwnerBuffList.size();
	for(int i=0; i < iCnt; ++i )
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
}

//Create AreaWeapon
void ioCreatWeaponByKeyInputAttackSkill::CreateAreaWeapon( ioBaseChar *pChar, ioHashString szAreaWeaponName, D3DXVECTOR3 vOffset )
{
	if( !pChar || szAreaWeaponName.IsEmpty() )
		return;

	D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( qtRot * vOffset );
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
		"",
		"",
		szAreaWeaponName,
		vPos,
		qtRot,
		ioAreaWeapon::CT_NORMAL );

	if ( pArea )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_PRE_ACTION;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
//Set function
bool ioCreatWeaponByKeyInputAttackSkill::SetPreAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return false;
	int iAniID = pGrp->GetAnimationIdx( m_PreAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return false;

	DWORD	dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_PRE_ACTION;

	if ( GetAttribute().m_vForceInfoList.empty() )
		m_bRefreshFireTimeList = false;
	AttackSkillFire( pChar );
	m_bRefreshFireTimeList = true;

	m_dwPreActionFireStartTime = dwCurTime;
	m_dwPreActionFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_PreAttribute.m_fAttackAniRate;
	m_dwPreActionFireStartTime += m_PreAttribute.m_dwPreDelay;
	return true;
}

void ioCreatWeaponByKeyInputAttackSkill::SetLoopState( ioBaseChar *pChar, bool bFirst )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_LOOP;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	float fAniRate = FLOAT1;
	if( m_fLoopAniRate > 0.0f )
		fAniRate = m_fLoopAniRate;
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if ( bFirst )
	{
		m_dwLoopStartTime = dwCurTime;
	}

	m_KeyInputState = KIS_NONE;
}

void ioCreatWeaponByKeyInputAttackSkill::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;
	
	m_SkillState = SS_ACTION;

	AttackSkillFire( pChar );

	m_iCurAttackCount++;

	DWORDVec::iterator iter = m_dwMapEffectIDList.begin();
	if ( iter != m_dwMapEffectIDList.end() )
	{
		if ( *iter != - 1 )
		{
			DestroySkillMapEffect( pChar, *iter );
			m_dwMapEffectIDList.erase( iter );
		}
	}
}

void ioCreatWeaponByKeyInputAttackSkill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;
	m_SkillState = SS_END;
	int iAniID = pGrp->GetAnimationIdx( m_szEndAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	if ( m_fEndAniRate > 0.0f )
		fTimeRate = m_fEndAniRate;	
	pGrp->SetActionAni( iAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

///////////////////////////////////////////////////////////////////////////////////

//Get AttributeNormal
const AttackAttribute& ioCreatWeaponByKeyInputAttackSkill::GetAttributeNormalConst() const
{
	if ( m_SkillState == SS_PRE_ACTION )
	{
		return m_PreAttribute;
	}
	else if ( m_SkillState == SS_ACTION )
	{
		switch( m_KeyInputState )
		{
		case KIS_S:
			if ( (int)m_SKeyAttributeList.size() > m_iCurAttackCount )
			{
				if ( !m_SKeyAttributeList[m_iCurAttackCount].m_AttackAnimation.IsEmpty() )
					return m_SKeyAttributeList[m_iCurAttackCount];
			}
			return m_AttackAttribute;
		case KIS_D:
			if ( (int)m_DKeyAttributeList.size() > m_iCurAttackCount )
			{
				if ( !m_DKeyAttributeList[m_iCurAttackCount].m_AttackAnimation.IsEmpty() )
					return m_DKeyAttributeList[m_iCurAttackCount];
			}
			return m_AttackAttribute;
		case KIS_DIRECTION_D_KEY:
			if ( (int)m_DirectionKeyWithDKeyAttributeList.size() > m_iCurAttackCount )
			{
				if ( !m_DirectionKeyWithDKeyAttributeList[m_iCurAttackCount].m_AttackAnimation.IsEmpty() )
					return m_DirectionKeyWithDKeyAttributeList[m_iCurAttackCount];
			}
			return m_AttackAttribute;
		}
	}

	return m_AttackAttribute;
}

//Key Check Only Owner
bool ioCreatWeaponByKeyInputAttackSkill::CheckKeyInput( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return false;

	bool bCangeDirection = false;
	bool bAttack = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if ( m_bCharRotateByDirKey || m_bUseDirectionKeyCheck )
		eNewDirKey = pChar->CheckCurDirKey();

	if( m_bCharRotateByDirKey )
	{
		if( eNewDirKey != m_CurDirKey )
		{
			bCangeDirection = true;
			m_CurDirKey = eNewDirKey;
			pChar->SetTargetRotToDirKey( eNewDirKey );
		}
	}

	if ( m_bUseAttackKey && pChar->IsAttackKey() )
	{
		if ( m_bUseDirectionKeyCheck && eNewDirKey != ioUserKeyInput::DKI_NONE )
			m_KeyInputState = KIS_DIRECTION_D_KEY;
		else
			m_KeyInputState = KIS_D;

		bAttack = true;
	}
	if ( m_bUseDefenseKey && pChar->IsDefenseKey() )
	{
		m_KeyInputState = KIS_S;
		bAttack = true;
	}

	if ( bAttack || bCangeDirection )
	{
		if ( bAttack )
			SetActionState( pChar );
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_LOOP;
			kPacket << (int)m_KeyInputState;
			kPacket << bAttack;
			kPacket << bCangeDirection;
			kPacket << (int )m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return true;
	}

	return false;
}

void ioCreatWeaponByKeyInputAttackSkill::CreateSkillMapEfffect( ioBaseChar *pChar )
{
	if ( m_szMapEffectName.IsEmpty() || m_iMaxMapEffectCount <= 0 || !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	m_dwMapEffectIDList.clear();
	m_dwMapEffectIDList.reserve( m_iMaxMapEffectCount );
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXQUATERNION qtRot = pChar->GetWorldOrientation();
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	for ( int i=0 ; i<m_iMaxMapEffectCount ; ++i )
	{
		ioMapEffect *pMapEffect = NULL;
		D3DXVECTOR3 vPos = vCharPos;
		if ( (int)m_vMapEffectOffsetList.size() > i )
			vPos += qtRot * m_vMapEffectOffsetList[i];
		
		pMapEffect = pStage->CreateMapEffect( m_szMapEffectName, vPos, vScale );

		DWORD	dwEffectID = -1;
		if ( pMapEffect )
			dwEffectID = pMapEffect->GetUniqueID();
		m_dwMapEffectIDList.push_back( dwEffectID );
	}
}

void ioCreatWeaponByKeyInputAttackSkill::DestroySkillMapEffect( ioBaseChar *pChar, DWORD& dwMapEffectID )
{
	if( dwMapEffectID != -1 )
	{
		pChar->EndMapEffect( dwMapEffectID, false );
		dwMapEffectID = -1;
	}
}

void ioCreatWeaponByKeyInputAttackSkill::CreateDummyChar( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;

	if ( m_DummyCharList.empty() )
		return;

	D3DXVECTOR3 vDir = pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	ioHashString szCharName = pChar->GetCharName();
	for ( int i=0 ; i<(int)m_DummyCharList.size() ; ++i )
	{
		const DummyCharLoadInfo& Info = m_DummyCharList[i];
		D3DXVECTOR3 vPos = vCharPos + (Info.m_fDummyCharOffset * vDir);
		vPos += Info.m_fDummyCharSideOffset * vSideDir;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

		ioDummyChar *pDummy = pStage->CreateDummyChar( Info.m_DummyCharName, iCurIndex, 
			szCharName, vPos, Info.m_fDummyCharStartAngle, 0 );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////