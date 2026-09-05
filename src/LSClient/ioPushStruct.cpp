

#include "stdafx.h"

#include "ioPushStruct.h"
#include "ioGaugeHelper.h"
#include "ioNpcChar.h"
#include "WeaponDefine.h"
#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"
#include "ioVoiceChat/Fourier.h"

#define MAX_MAP_CONTINUOUS_COUNT 10

ioPushStruct::ioPushStruct( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioPlayEntity( pGrp, pMode )
{
	m_iArrayIndex = 0;
	m_State = PS_DELAY;
	m_pGauge = NULL;
	m_pPushChar = NULL;
	m_dwSetPushTime = 0;

	m_ArmorType = AT_UNARMOR;

	m_fDefaultMaxHP = FLOAT1;
	m_fDefaultMinHP = 0.0f;
	m_fGravityAmt = 0.0f;

	m_fCurMoveSpeed = 0.0f;
	m_dwCreateTime = FRAMEGETTIME();
	m_dwProcessTime = FRAMEGETTIME();

	m_bEnableClick = false;
	m_bObjectCollision = false;
	m_bSemiMapCollision = false;
	m_bNotMoveStruct   = false;
	m_bNoDropStruct = false;
	m_bDisableHPVisible = false;
	m_bSendedDieMessage  = false;
	m_bMoveEffectCreated = false;
	m_bMapCollisionPushed = false;

	m_iMapTranslateContinuousCount = 0;
	m_dwMapTranslateFrame = 0;

	m_iWeaponFireCnt = 0;
	m_dwBaseWeaponIndex = 0;
	m_dwAreaWeaponIndex = 0;

	m_iAnimationIndex = -1;

	m_iAttackType = PAT_NONE;
	m_iSearchAutoState = SAS_SEARCHING;

	m_fSearchingRange = 0.0f;
	m_fSearchingAngle = 0.0f;

	m_dwTracingSpeed = 0;
	m_dwTracingStartTime = 0;
	m_dwTracingDuration = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireEndDelay = 0;

	m_vStartDir = ioMath::VEC3_ZERO;

	Help::InitColorValue( m_OutLineColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
	m_fOutLineSize = FLOAT1;
	m_bOutLineModified = false;

	m_dwWeaponCreateTime = 0;

	m_BomberState = BBS_WAIT;
	m_dwBomberSensingStartTime = 0;
	m_dwBomberTraceEndTime = 0;

	m_dwBomberSensingEnableTime = 0;

	m_bNoDropProcess = true;
	m_dwCreateEtcItemCode = 0;

	m_pSpecialBox = NULL;

	m_bAreaCheckCharPos = false;
}

ioPushStruct::~ioPushStruct()
{
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pSpecialBox );

	m_vMountEntityList.clear();
	m_vMountWeaponList.clear();

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );

	m_ObjectItemList.clear();
	m_PushStructInfoList.clear();

	if( !m_SearchingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SearchingSound, m_SearchSoundID );
	}
}

void ioPushStruct::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	m_ArmorType	= (ArmorType)rkLoader.LoadInt_e( "armor_type", AT_UNARMOR );	

	m_iStructType = (PushStructType)rkLoader.LoadInt_e( "struct_type", PSTT_NOMAL );

	m_fPushRange = rkLoader.LoadFloat_e( "range", FLOAT100 );
	m_fHeightGap = rkLoader.LoadFloat_e( "height_gap", FLOAT10 );

	m_fDefaultMaxHP  = rkLoader.LoadFloat_e( "max_hp", FLOAT10 );
	m_fDefaultMinHP  = rkLoader.LoadFloat_e( "min_hp", 0.0f );
	m_HP.m_fMaxValue = m_fDefaultMaxHP;
	m_HP.m_fCurValue = m_fDefaultMaxHP;

	m_HP.m_fRecovery = rkLoader.LoadFloat_e( "hp_recovery", 0.0f );
	m_HP.m_dwRecoveryTick = rkLoader.LoadInt_e( "hp_recovery_tick", 0 );

	m_fMoveSpeedRate = rkLoader.LoadFloat_e( "move_speed_rate", FLOAT05 );

	m_bMountEnable = rkLoader.LoadBool_e( "mount_enable", false );

	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_ExplosionSound = szBuf;
	rkLoader.LoadString_e( "push_animation", "", szBuf, MAX_PATH );
	m_PushAnimation = szBuf;
	rkLoader.LoadString_e( "destroy_effect", "", szBuf, MAX_PATH );
	m_DestroyEffect = szBuf;
	rkLoader.LoadString_e( "move_effect", "", szBuf, MAX_PATH );
	m_MoveEffect = szBuf;
	rkLoader.LoadString_e( "struct_block_buff", "", szBuf, MAX_PATH );
	m_BlockBuff = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	m_bObjectCollision  = rkLoader.LoadBool_e( "object_collision", false );
	m_bSemiMapCollision = rkLoader.LoadBool_e( "semi_map_collision", false );
	m_bNotMoveStruct = rkLoader.LoadBool_e( "not_move_struct", false );
	m_bNoDropStruct = rkLoader.LoadBool_e( "no_drop_struct", false );
	m_bDisableHPVisible = rkLoader.LoadBool_e( "disable_gauge_visible", false );
	m_bEnableClick = rkLoader.LoadBool_e( "enable_click", false );

	m_iAnimationIndex = rkLoader.LoadInt_e( "animation_idx", -1 );
	m_fAnimationRate = rkLoader.LoadFloat_e( "animation_rate", FLOAT1 );

	m_dwTicTime = rkLoader.LoadInt_e( "hp_decrease_tick", 0 );
	m_fDecreaseHP = rkLoader.LoadFloat_e( "hp_decrease", 0.0f );

	int iObjectCount = rkLoader.LoadInt_e( "object_count", 0 );
	
	m_ObjectItemList.clear();
	m_ObjectItemList.reserve( iObjectCount );

	char szName[MAX_PATH];
	int i=0;
	for( i=0; i<iObjectCount; i++ )
	{
		wsprintf_e( szBuf, "object_item%d", i+1 );
		rkLoader.LoadString( szBuf, "", szName, MAX_PATH );

		m_ObjectItemList.push_back( szName );
	}

	int iPushCnt = rkLoader.LoadInt_e( "push_struct_cnt", 0 );
	m_PushStructInfoList.clear();
	m_PushStructInfoList.reserve( iPushCnt );
	for( i=0; i < iPushCnt; ++i )
	{
		CreatePushStructInfo kInfo;

		wsprintf_e( szBuf, "push_struct%d_create_type", i+1 );
		kInfo.m_CreateType = (StructCreateType)rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "push_struct%d_num", i+1 );
		kInfo.m_iNum = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "push_struct%d_x", i+1 );
		kInfo.m_vNewPos.x = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "push_struct%d_y", i+1 );
		kInfo.m_vNewPos.y = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "push_struct%d_z", i+1 );
		kInfo.m_vNewPos.z = rkLoader.LoadFloat( szBuf, 0.0f );

		m_PushStructInfoList.push_back( kInfo );
	}

	m_iAttackType = (AttackType)rkLoader.LoadInt_e( "attack_type", 0 );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fSearchingRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fSearchingAngle = rkLoader.LoadFloat_e( "traget_angle", 0.0f );
	m_dwTracingSpeed = rkLoader.LoadInt_e( "target_tracing_speed", 0.0f );

	m_dwTracingDuration = rkLoader.LoadInt_e( "target_tracing_duration", 0 );
	m_dwFireEndDelay = rkLoader.LoadInt_e( "target_fire_end_delay", 0 );

	m_bOwnerRotate = rkLoader.LoadBool_e( "owner_rotate", false );
	m_dwFireDuration = rkLoader.LoadInt_e( "weapon_fire_duration", 0 );

	int iFireCnt = rkLoader.LoadInt_e( "weapon_fire_cnt", 0 );
	m_vWeaponFireInfoList.clear();
	m_vWeaponFireInfoList.reserve( iFireCnt );

	for( int i=0; i < iFireCnt; ++i )
	{
		WeaponFireInfo kFireInfo;
		DWORD dwFireTime = 0;
		float fFireAngle = 0.0f;

		wsprintf_e( szBuf, "weapon_fire_time%d", i+1 );
		dwFireTime = rkLoader.LoadInt( szBuf, 0 );
		
		wsprintf_e( szBuf, "weapon_fire_angle%d", i+1 );
		fFireAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		kFireInfo.m_dwWeaponFireTime = dwFireTime;
		kFireInfo.m_fWeaponFireAngle = fFireAngle;

		m_vWeaponFireInfoList.push_back( kFireInfo );
	}

	rkLoader.LoadString_e( "weapon_wounded_animation", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "weapon_wounded_duration", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "weapon_wounded_loop_ani", false );
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "weapon_attribte_index", 0 );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "weapon_attribute_resist_index", 0 );

	m_fWeaponHeightRate = rkLoader.LoadFloat_e( "weapon_height_rate", FLOAT05 );
	m_fWeaponOffSet = rkLoader.LoadFloat_e( "weapon_offset", 0.0f );

	rkLoader.LoadString_e( "searching_effect", "", szBuf, MAX_PATH );
	m_SearchingEffect = szBuf;
	rkLoader.LoadString_e( "searching_sound", "", szBuf, MAX_PATH );
	m_SearchingSound = szBuf;
	rkLoader.LoadString_e( "auto_fire_effect", "", szBuf, MAX_PATH );
	m_AutoFireEffect = szBuf;

	rkLoader.LoadString_e( "weapon_fire_effect", "", szBuf, MAX_PATH );
	m_WeaponFireEffect = szBuf;
	rkLoader.LoadString_e( "weapon_fire_sound", "", szBuf, MAX_PATH );
	m_WeaponFireSound = szBuf;

	m_dwAreaWeaponTime = rkLoader.LoadInt_e( "area_weapon_time", 0 );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_fBomberMoveSpeed = rkLoader.LoadFloat_e( "bomber_move_speed", 0.0f );

	m_dwBomberSensingDuration = rkLoader.LoadInt_e( "bomber_sensing_duration", 0 );
	m_fBomberSensingRange = rkLoader.LoadFloat_e( "bomber_sensing_range", 0.0f );
	m_fBomberSensingAngle = rkLoader.LoadFloat_e( "bomber_sensing_angle", 0.0f );

	m_fBomberTraceSpeed = rkLoader.LoadFloat_e( "bomber_trace_speed", 0.0f );
	m_fBomberTraceRange = rkLoader.LoadFloat_e( "bomber_trace_range", 0.0f );

	m_dwBomberTraceDuration = (DWORD)rkLoader.LoadInt_e( "bomber_trace_duration", 0 );

	m_fBomberEnableMoveHeight = rkLoader.LoadFloat_e( "bomber_enable_move_height", 0.0f );
	m_fBomberBoomRange = rkLoader.LoadFloat_e( "bomber_boom_range", 0.0f );
	m_fBomberBoomAngle = rkLoader.LoadFloat_e( "bomber_boom_angle", 0.0f );

	rkLoader.LoadString_e( "bomber_create_ani", "", szBuf, MAX_PATH );
	m_BomberCreateAni = szBuf;
	rkLoader.LoadString_e( "bomber_sencing_ani", "", szBuf, MAX_PATH );
	m_BomberSencingAni = szBuf;
	rkLoader.LoadString_e( "bomber_tracing_ani", "", szBuf, MAX_PATH );
	m_BomberTracingAni = szBuf;

	m_fBomberCreateAniRate = rkLoader.LoadFloat_e( "bomber_create_ani_rate", FLOAT1 );
	m_fBomberSencingAniRate = rkLoader.LoadFloat_e( "bomber_sencing_ani_rate", FLOAT1 );
	m_fBomberTracingAniRate = rkLoader.LoadFloat_e( "bomber_tracing_ani_rate", FLOAT1 );

	m_fBomberSencingMoveAngle = rkLoader.LoadFloat_e( "bomber_sencing_move_angle", 0.0f );
	m_bBomberOnlyCharBoom = rkLoader.LoadBool_e( "bomber_only_char_boom", false );
	
	m_dwBomberRotateSpeed = (DWORD)rkLoader.LoadInt_e( "bomber_rotate_speed", 0 );

	m_bOwnerNameVisible = rkLoader.LoadBool_e( "owner_name_visible", false );
}

void ioPushStruct::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	CheckLastPushChar();

	UpdateMountedEntityList();
	UpdateMountedWeaponList();

	switch( m_iStructType )
	{
	case PSTT_BOMBER:
		ProcessBomberState( fTimePerSec );
		break;
	case PSTT_BLOCK:
		ProcessBlockState( fTimePerSec );
		break;
	case PSTT_FISHING_GROUND:
		ProcessFishingGroundState( fTimePerSec );
		break;
	default:
		ProcessState( fTimePerSec );
		break;
	}

	CheckStructDieByHp();
	CheckBomberEnd();
	CheckAreaWeapon();

	if( m_iMapTranslateContinuousCount < MAX_MAP_CONTINUOUS_COUNT )
	{
		if( m_dwMapTranslateFrame + 1 < g_FrameTimer.GetFrameCounter() )
		{
			m_iMapTranslateContinuousCount = 0;
		}
	}
}

void ioPushStruct::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

ApplyWeaponExp ioPushStruct::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPushStruct::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	if( P2PNetwork::IsNetworkPlaying() &&
		m_pModeParent->IsNetworkMode() &&
		!pAttacker->IsNeedProcess() )
		return AWE_EXPLOSION;

	float fPreCurHP = m_HP.m_fCurValue;
	float fDamage = pWeapon->GetDamage( this );
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( ToNpcChar( pAttacker) )
		fDamage *= FLOAT100;           // NPC는 한방에 없앤다.

	if( m_bSemiMapCollision )
		fDamage = 0.0f;

	float fRate = pWeapon->GetTargetPushStructDamgeRate();
	if( fRate > 0.0f && 0 < pWeapon->GetTargetPushStructType() && pWeapon->GetTargetPushStructType() == GetStructType() )
	{
		fDamage = fDamage * fRate;
	}

	m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT3 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	bool bOnlyEmoticon = true;
	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );

			bOnlyEmoticon = false;
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

	if( m_HP.m_fCurValue > 0.0f )
	{
		if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_PUSHSTRUCT_DAMAGE );
			kPacket << pWeapon->GetOwnerName();
			kPacket << m_iArrayIndex;
			kPacket << m_HP.m_fCurValue;
			kPacket << m_HP.m_fMaxValue;
			kPacket << bOnlyEmoticon;
			kPacket << vAttackDir;
			kPacket << fDamage;
			kPacket << fPushPower;
			kPacket << (int)eActionStop;
			kPacket << iShakeCamera;
			kPacket << pWeapon->GetWeaponIndex();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( fPreCurHP > 0.0f )
		{
			m_HP.m_fCurValue = 0.0f;
			m_KillerChar = pWeapon->GetOwnerName();

			if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
			{
				ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_KillerChar );
				if( pKillChar )
				{
					if( pKillChar->IsNeedSendNetwork() )
					{
						if( m_iStructType == PSTT_BOMBER )
							SendDieMessageToServer( true, false );
						else
							SendDieMessageToServer( true, true );
					}
				}
				else
				{
					if( P2PNetwork::IsHost() )
					{
						if( m_iStructType == PSTT_BOMBER )
							SendDieMessageToServer( true, false );
						else
							SendDieMessageToServer( true, true );
					}
				}
			}
			else
			{
				if( m_iStructType == PSTT_BOMBER )
					StructDie( true, false );
				else
					StructDie( true, true );
			}
		}
	}

	if( pAttacker->IsOwnerChar() )
	{
		// ChangeJumpping
		if( pWeapon && pWeapon->IsChangeJumpping( false, false ) )
		{
			int iLimitCnt = pWeapon->GetChangeJumppingLimitCnt();
			pAttacker->SetChangeJumppingState( pWeapon->GetChangeJumppingPowerRate(), true, iLimitCnt );
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

bool ioPushStruct::IsNeedCheckMapCollision() const
{
	if( m_iStructType == PSTT_BLOCK )
		return false;

	if( !IsObjectCollision() )
		return false;

	if( m_bNoDropStruct )
		return false;

	if( m_iMapTranslateContinuousCount >= MAX_MAP_CONTINUOUS_COUNT )
		return false;

	return true;
}

bool ioPushStruct::IsMountingAvailable() const
{
	if( m_iStructType == PSTT_BLOCK )
		return false;
	else if( m_iStructType == PSTT_NOMAL )
		return m_bMountEnable;

	return true;
}

bool ioPushStruct::IsMountAvailable() const
{
	if( m_iStructType == PSTT_BOMBER )
		return false;

	return IsObjectCollision();
}

bool ioPushStruct::IsNowMountEnable() const
{
	if( m_State == PS_DIE || !IsObjectCollision() )
		return false;

	if( m_iStructType == PSTT_BOMBER )
		return false;

	return true;
}

void ioPushStruct::UpdateGauge()
{
	m_bGaugeVisible = false;
	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;
	
	if( Setting::Check2DInScreen( vPos, m_iGaugeXPos, m_iGaugeYPos ) )
	{
		m_bGaugeVisible = true;
		if( m_pGauge )
			m_pGauge->UpdateGauge( m_HP.m_fCurValue, m_HP.m_fMaxValue );
	}
}

void ioPushStruct::RenderGauge()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();

	// 낚시터 타입이면 이름 보이게 
	if( m_bOwnerNameVisible && m_iStructType == PSTT_FISHING_GROUND )
	{
		ioBaseChar* pOwner = pStage->GetBaseChar( m_OwnerCharName );
		if( !m_OwnerCharName.IsEmpty() && pOwner && pOwner->IsVisibleID() )
		{
			D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
			vPos.y += 40.0f;

			int iXPos = 0, iYPos = 0;
			if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
			{
				g_FontMgr.SetAlignType( TAT_CENTER );
				// 팀에 따른 오브젝트 텍스트 색깔 표시
				if( pOwner->GetTeam() == pStage->GetOwnerChar()->GetTeam() )
					g_FontMgr.SetTextColor( 0, 150, 255 );
				else
					g_FontMgr.SetTextColor( 255, 60, 0 );

				g_FontMgr.PrintText( iXPos, iYPos - 30, FONT_SIZE_12, STR(1), m_OwnerCharName );
			}
		}
	}

	if( !m_pGauge || !m_bGaugeVisible || m_bDisableHPVisible  ) return;

	bool bCheckMode = g_GUIMgr.IsNoneUIMode();
	if( pStage && !bCheckMode )
	{
		if( pStage->HasCameraFixedStyle(CAMERA_STYLE_MOVIE) || pStage->HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
			bCheckMode = true;
	}

	if( bCheckMode && !Setting::NoUIModeHPShow() ) return;

	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos = 0, iYPos = 0;
	if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
	{
		m_pGauge->RenderGauge( iXPos, iYPos + 1 );
	}
}

ioWorldEventReceiver* ioPushStruct::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

void ioPushStruct::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	SetWorldPosition( vTargetPos );

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
	{
		pAreaWeapon->SetAreaPosition( vTargetPos, m_pModeParent->GetCreator() );
	}
}

void ioPushStruct::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
	// 데미지 처리?
}

void ioPushStruct::OnPoisonZoneDamage()
{
	if( m_HP.m_fCurValue == 0.0f )
		return;

	m_HP.m_fCurValue = 0.0f;

	if( m_LastPushChar.IsEmpty() )
		m_KillerChar = m_OwnerCharName;
	else
		m_KillerChar = m_LastPushChar;

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_KillerChar );
		if( pKillChar )
		{
			if( pKillChar->IsNeedSendNetwork() )
			{
				if( m_iStructType == PSTT_BOMBER )
					SendDieMessageToServer( true, false );
				else
					SendDieMessageToServer( true, true );
			}
		}
		else
		{
			if( P2PNetwork::IsHost() )
			{
				if( m_iStructType == PSTT_BOMBER )
					SendDieMessageToServer( true, false );
				else
					SendDieMessageToServer( true, true );
			}
		}
	}
	else
	{
		if( m_iStructType == PSTT_BOMBER )
			StructDie( true, false );
		else
			StructDie( true, false );
	}
}

bool ioPushStruct::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	// 클릭 가능하면 충돌처리 하지 않음
	if( m_bEnableClick )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT3 )
		bContactType = true;

	if( m_bSemiMapCollision && bContactType )
		return false;

	return true;
}

int ioPushStruct::DontMoveEntityLevel() const
{
	if( !IsObjectCollision() || m_bNoDropStruct )
		return 6;

	if( m_iStructType == PSTT_BLOCK )
		return 3;

	if( m_iStructType == PSTT_BOMBER )
		return 1;

	if( m_State == PS_MOVE )
		return 0;

	return 2;
}

ioPlayEntity::PlaySubType ioPushStruct::GetSubType() const
{
	return PST_PUSHSTRUCT;
}

ArmorType ioPushStruct::GetArmorType() const
{
	return m_ArmorType;
}

void ioPushStruct::NeedUpdateBottomHeight()
{
	ioPlayEntity::NeedUpdateBottomHeight();

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )	return;

	MountedEntityList::iterator iter = m_vMountEntityList.begin();
	while ( iter != m_vMountEntityList.end() )
	{
		ioGameEntity *pEntity = *iter;

		if( !pStage->CheckEnableEntity( pEntity ) )
		{
			iter = m_vMountEntityList.erase( iter );
		}
		else
		{
			pEntity->NeedUpdateBottomHeight();

			++iter;
		}
	}
}

void ioPushStruct::Translate( const D3DXVECTOR3 &vMove )
{
	MountEntityMove( vMove );

	ioPlayEntity::Translate( vMove );
}

void ioPushStruct::MapCollisionPushingTranslate( const D3DXVECTOR3 &vMoveAmt )
{
	m_iMapTranslateContinuousCount++;
	m_dwMapTranslateFrame = g_FrameTimer.GetFrameCounter();

	if( m_iMapTranslateContinuousCount < MAX_MAP_CONTINUOUS_COUNT )
	{
		D3DXVECTOR3 vPushAmt( vMoveAmt.x, 0.0f, vMoveAmt.z );	// 좌우 이동만 처리
		ioPlayEntity::MapCollisionPushingTranslate( vPushAmt );
	}

	m_bMapCollisionPushed = true;
}

void ioPushStruct::DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt )
{
	ioNpcChar *pNpcChar = ToNpcChar(pPusher);

	if(pNpcChar)
	{
		if(COMPARE(pNpcChar->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC))
			return;
	}

	if( !IsMapCollisionPushed() )
	{
		
		ioPlayEntity::DontMoveCollisionPushingTranslate( pPusher, vMoveAmt );
	}
}

void ioPushStruct::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

void ioPushStruct::SetHP( float fCurHP, float fMaxHP )
{
	m_HP.m_fCurValue = fCurHP;
	m_HP.m_fMaxValue = fMaxHP;
}

void ioPushStruct::SetMoveState( ioBaseChar *pPushChar )
{
	m_State = PS_MOVE;
	m_pPushChar = pPushChar;
	m_LastPushChar = pPushChar->GetCharName();
	m_vMoveDir = pPushChar->GetMoveDir();
	m_fCurMoveSpeed = pPushChar->GetMaxSpeed() * m_fMoveSpeedRate;
	m_pPushChar->GetGroup()->ClearAllLoopAni( FLOAT100, true );
	m_pPushChar->GetGroup()->SetLoopAni( m_PushAnimation, 0.0f );

	if( !m_MoveEffect.IsEmpty() && !m_bMoveEffectCreated )
	{
		m_pGroup->AttachEffect( m_MoveEffect, NULL );
		m_bMoveEffectCreated = true;
	}	
}

void ioPushStruct::SetDelayState()
{
	m_State = PS_DELAY;
	m_pPushChar = NULL;
	m_fCurMoveSpeed = 0.0f;
	m_dwSetPushTime = FRAMEGETTIME();

	if( m_bMoveEffectCreated )
	{
		m_pGroup->EndEffect( m_MoveEffect, true );
		m_bMoveEffectCreated = false;
	}
}

void ioPushStruct::SetOwner( ioHashString szOwnerName )
{
	m_OwnerCharName = szOwnerName;
}

void ioPushStruct::SetCreateItem( ioHashString szCreateItem )
{
	m_CreateItem = szCreateItem;
}

void ioPushStruct::ProcessState( float fTimePerSec )
{
	ProcessMoveState( fTimePerSec );

	bool bRePosition = false;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fMapHeight = GetBottomHeight();

	if( !m_bNoDropStruct && vCurPos.y != fMapHeight )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwProcessTime;
		do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( vCurPos.y > fMapHeight )
			{
				m_fGravityAmt += m_pModeParent->GetCreator()->GetRoomGravity() * fNewTimePerSec;
				vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

				if( vCurPos.y == fMapHeight )
					m_fGravityAmt = 0.0f;

				bRePosition = true;
			}
		} while( dwGapTime > 0 );

		if( vCurPos.y < fMapHeight )
		{
			vCurPos.y = fMapHeight;
			m_fGravityAmt = 0.0f;
			bRePosition = true;
		}

		SetWorldPosition( vCurPos );

		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
		if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
		{
			pAreaWeapon->SetAreaPosition( vCurPos, m_pModeParent->GetCreator() );
		}
	}

	if( bRePosition )
	{
		if( !m_vMountWeaponList.empty() )
		{
			const ioOrientBox &rkBox = GetWorldCollisionBox();
			float fNewYPos = rkBox.GetCenter().y + rkBox.GetExtents( 1 );

			int iCount = m_vMountWeaponList.size();
			for( int i=0; i<iCount; i++ )
			{
				vCurPos = m_vMountWeaponList[i]->GetPosition();
				vCurPos.y = fNewYPos;

				m_vMountWeaponList[i]->SetPosition( vCurPos );
			}
		}
	}
	else
	{
		if( m_State == PS_DELAY )
		{
			ProcessAttack( fTimePerSec );
		}
	}

	m_dwProcessTime = FRAMEGETTIME();

	DropZoneCheck();
}

void ioPushStruct::ProcessMoveState( float fTimePerSec )
{
	if( m_State != PS_MOVE )	return;
	if( !m_pPushChar )
	{
		SetDelayState();
		return;
	}
	if( m_pPushChar->GetState() != CS_STRUCT_PUSH )
	{
		SetDelayState();
		return;
	}

	D3DXVECTOR3 vMoveAmt = m_vMoveDir * ( m_fCurMoveSpeed * fTimePerSec );
	D3DXVECTOR3 vNewPos  = GetWorldPosition() + vMoveAmt;

	SetWorldPosition( vNewPos );

	int iCount = m_vMountEntityList.size();
	for( int i=0; i<iCount; i++ )
	{
		m_vMountEntityList[i]->Translate( vMoveAmt );
	}

	const ioOrientBox &rkBox = GetWorldCollisionBox();

	iCount = m_vMountWeaponList.size();
	for( i=0; i<iCount; i++ )
	{
		vNewPos = m_vMountWeaponList[i]->GetPosition();
		vNewPos.y = rkBox.GetCenter().y + rkBox.GetExtents( 1 );

		m_vMountWeaponList[i]->SetPosition( vNewPos + vMoveAmt );
	}
}

void ioPushStruct::ProcessAttack( float fTimePerSec )
{
	switch( m_iAttackType )
	{
	case PAT_NORMAL:
		CheckOwnerRotate();
		ProcessNormalAttack();
		break;
	case PAT_SEARCH_AUTO:
		ProcessSearchAutoAttack( fTimePerSec );
		break;
	}
}

void ioPushStruct::ProcessNormalAttack()
{
	if( m_OwnerCharName.IsEmpty() ) return;
	if( m_bSendedDieMessage )	return;

	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner || !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !COMPARE( m_iWeaponFireCnt, 0, iFireTimeCnt) )
		return;

	DWORD dwFireTime = m_vWeaponFireInfoList[m_iWeaponFireCnt].m_dwWeaponFireTime;

	bool bAniFire = false;
	if( m_iWeaponFireCnt == 0 && m_dwCollisionStartTime > 0 )
	{
		bAniFire = true;
		dwFireTime = m_dwCollisionStartTime;
	}

	if( dwFireTime == 0 )
		return;

	if( !bAniFire && dwFireTime > 0 )
		dwFireTime += m_dwCreateTime;

	if( dwCurTime > dwFireTime )
	{
		float fAngle = m_vWeaponFireInfoList[m_iWeaponFireCnt].m_fWeaponFireAngle;
		CreateWeapon( fAngle );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_PUSHSTRUCT_CREATEWEAPON );
			kPacket << GetPushArrayIdx();
			kPacket << fAngle;
			kPacket << GetWorldOrientation();
			kPacket << m_dwBaseWeaponIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPushStruct::CheckOwnerRotate()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bOwnerRotate ) return;

	DWORD dwSkipEndTime = m_dwWeaponCreateTime+m_dwFireDuration;
	if( m_dwWeaponCreateTime > 0 && COMPARE( dwCurTime, m_dwWeaponCreateTime, dwSkipEndTime ) )
		return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;

	SetWorldOrientation( pOwner->GetTargetRot() );
}

void ioPushStruct::ProcessSearchAutoAttack( float fTimePerSec )
{
	if( m_OwnerCharName.IsEmpty() ) return;
	if( m_bSendedDieMessage )	return;

	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	switch( m_iSearchAutoState )
	{
	case SAS_SEARCHING:
		TargetSearching( fTimePerSec );
		break;
	case SAS_TRACING:
		TargetTracing( fTimePerSec );
		break;
	case SAS_FIRE:
		TargetFire( fTimePerSec );
		break;
	}
}

void ioPushStruct::ChangeToSearchingState()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;

	m_TargetName.Clear();
	m_iSearchAutoState = SAS_SEARCHING;
	m_iWeaponFireCnt = 0;
	m_dwTracingStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_vStartDir = ioMath::VEC3_ZERO;

	if( !m_SearchingEffect.IsEmpty() )
		m_pGroup->AttachEffect( m_SearchingEffect, NULL );

	if( !m_SearchingSound.IsEmpty() )
		m_SearchSoundID = PlaySound( m_SearchingSound, true );

	if( !m_AutoFireEffect.IsEmpty() )
		m_pGroup->EndEffect( m_AutoFireEffect, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_PUSHSTRUCT_SEARCH_STATE );
		kPacket << GetPushArrayIdx();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPushStruct::ChangeToTracingState( const ioHashString &szTargetName, const D3DXVECTOR3 &vDir )
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;

	m_iSearchAutoState = SAS_TRACING;
	m_TargetName = szTargetName;
	m_vStartDir = vDir;

	m_dwTracingStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_PUSHSTRUCT_TRACING_STATE );
		kPacket << GetPushArrayIdx();
		kPacket << GetWorldOrientation();
		kPacket << m_TargetName;
		kPacket << m_vStartDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPushStruct::ChangeToFireState()
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_iSearchAutoState = SAS_FIRE;
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = dwCurTime + m_dwFireEndDelay;
	m_iWeaponFireCnt = 0;

	int iSize = m_vWeaponFireInfoList.size();
	if( iSize <= 0 )
		return;

	m_dwFireEndTime += m_vWeaponFireInfoList[iSize-1].m_dwWeaponFireTime;

	if( !m_SearchingEffect.IsEmpty() )
		m_pGroup->EndEffect( m_SearchingEffect, false );

	if( !m_SearchingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SearchingSound, m_SearchSoundID );
	}

	if( !m_AutoFireEffect.IsEmpty() )
		m_pGroup->AttachEffect( m_AutoFireEffect, NULL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_PUSHSTRUCT_FIRE_STATE );
		kPacket << GetPushArrayIdx();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPushStruct::TargetSearching( float fTimePerSec )
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner || !pOwner->IsNeedProcess() ) return;
	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE ) return;

	// Range, Angle
	D3DXVECTOR3 vStartPos = GetWorldPosition();
	float fRangeSq = m_fSearchingRange * m_fSearchingRange;
	float fHalfCosine = cosf( DEGtoRAD( m_fSearchingAngle ) * FLOAT05 );
	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fDistance = std::numeric_limits<float>::infinity();
	ioBaseChar *pTarget = NULL;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar )
			continue;

		if( pChar->IsSystemState() )
			continue;
			
		if( !pChar->IsEnableTarget() )
			continue;

		if( pChar->GetTeam() == pOwner->GetTeam() )
			continue;

		D3DXVECTOR3 vPoint;
		if( !pChar->CheckSpherePoint( vStartPos, fRangeSq, vPoint ) )
			continue;

		D3DXVECTOR3 vDir = pChar->GetWorldPosition() - vStartPos;
		float fCurDistance = D3DXVec3LengthSq( &vDir );
		if( fCurDistance > fRangeSq )
			continue;

		vDir.y = 0.0f;
		D3DXVec3Normalize( &vDir, &vDir );

		if( D3DXVec3Dot( &vCurDir, &vDir ) >= fHalfCosine )
		{
			if( !pChar->CheckTargetWoundType( m_TargetWoundType ) )
				continue;

			if( fCurDistance < fDistance )
			{
				fDistance = fCurDistance;
				pTarget = pChar;
			}
		}
	}

	if( pTarget )
	{
		ChangeToTracingState( pTarget->GetCharName(), vCurDir );
		return;
	}
}

void ioPushStruct::TargetTracing( float fTimePerSec )
{
	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );

	if( !CheckTargetValid() )
	{
		if( !pOwner || !pOwner->IsNeedProcess() )
			return;

		ChangeToSearchingState();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwTracingStartTime+m_dwTracingDuration < dwCurTime )
	{
		if( !pOwner || !pOwner->IsNeedProcess() )
			return;

		ChangeToFireState();
		return;
	}

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( !pTarget ) return;

	D3DXVECTOR3 vTargetDir = pTarget->GetWorldPosition() - GetWorldPosition();
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	D3DXQUATERNION qtStartRot;
	ioMath::CalcDirectionQuaternion( qtStartRot, -m_vStartDir );

	fCurYaw = RADtoDEG( ioMath::QuaterToYaw(qtStartRot) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fCurAngle = 0.0f;
	float fGapAngle = DEGtoRAD(ioMath::AngleGap( fCurYaw, fTargetYaw ));
	DWORD dwNeedTime = (DWORD)(fabs(fGapAngle) / (2*PI) * m_dwTracingSpeed);
	DWORD dwTimeGap = dwCurTime - m_dwTracingStartTime;

	if( dwTimeGap >= dwNeedTime )
	{
		// Target Pos
		SetWorldOrientation( qtTargetRot );
	}
	else
	{
		if( m_dwTracingSpeed > 0 )
			fCurAngle = (float)(dwTimeGap * 360.0f / m_dwTracingSpeed);

		if( fGapAngle < 0 )
			fCurAngle = -fCurAngle;

		fCurAngle = DEGtoRAD( fCurAngle );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, fCurAngle, 0.0f, 0.0f );

		D3DXQuaternionMultiply( &qtAngle, &qtStartRot, &qtAngle );
		SetWorldOrientation( qtAngle );
	}
}

void ioPushStruct::TargetFire( float fTimePerSec )
{
	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckTargetValid() )
	{
		ChangeToSearchingState();
		return;
	}

	if( m_dwFireEndTime < dwCurTime )
	{
		ChangeToSearchingState();
		return;
	}

	int iFireTimeCnt = m_vWeaponFireInfoList.size();
	if( iFireTimeCnt <= 0 )
		return;

	if( !COMPARE( m_iWeaponFireCnt, 0, iFireTimeCnt) )
		return;

	DWORD dwFireTime = m_vWeaponFireInfoList[m_iWeaponFireCnt].m_dwWeaponFireTime;
	if( dwFireTime == 0 )
		return;

	if( dwFireTime > 0 )
		dwFireTime += m_dwFireStartTime;

	if( dwCurTime > dwFireTime )
	{
		float fAngle = m_vWeaponFireInfoList[m_iWeaponFireCnt].m_fWeaponFireAngle;
		CreateWeapon( fAngle );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_PUSHSTRUCT_CREATEWEAPON );
			kPacket << GetPushArrayIdx();
			kPacket << fAngle;
			kPacket << GetWorldOrientation();
			kPacket << m_dwBaseWeaponIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioPushStruct::CheckTargetValid()
{
	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner || !pOwner->IsNeedProcess() )
		return true;

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )
		return false;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( !pTarget )
		return false;

	if( pTarget->HasHideBuff() )
		return false;

	if( pTarget->IsChatModeState( false ) )
		return false;

	D3DXVECTOR3 vStartPos = GetWorldPosition();
	float fRangeSq = m_fSearchingRange * m_fSearchingRange;
	float fHalfCosine = cosf( DEGtoRAD( m_fSearchingAngle ) * FLOAT05 );
	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vStartPos;
	if( D3DXVec3LengthSq( &vDiff ) > fRangeSq )
		return false;

	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
		return false;

	return true;
}

void ioPushStruct::SetBaseWeaponIndex( int iWeaponIndex )
{
	m_dwBaseWeaponIndex = iWeaponIndex;
}

void ioPushStruct::CreateWeapon( float fAngle )
{
	if( m_OwnerCharName.IsEmpty() ) return;
	if( m_bSendedDieMessage )	return;

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwWeaponCreateTime = dwCurTime;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vCurDir = qtRot * vCurDir;

	D3DXVECTOR3 vCurPos = GetMidPositionByRate( m_fWeaponHeightRate );
	vCurPos += vCurDir * m_fWeaponOffSet;

	DWORD dwWeaponIndex = m_dwBaseWeaponIndex + m_iWeaponFireCnt;


	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.dwDuration = 1;

	if( m_dwCollisionStartTime > 0 && m_dwCollisionEndTime > 0 && m_dwCollisionEndTime > dwCurTime )
	{
		kFireTime.dwDuration = m_dwCollisionEndTime - dwCurTime;
	}

	kFireTime.iAttributeIdx = m_WeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_WeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_WeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_WeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_WeaponInfo.m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx = dwWeaponIndex;
	kFireTime.eCollisionType = ACT_SPECIAL;
	kFireTime.eFireTimeType = FTT_EXTEND_ATTACK;
	kFireTime.szSpecialCollisionBox = m_szCollisionBox;

	kFireTime.m_iExtraValue = GetPushArrayIdx();

	pOwner->ExtendFireExplicit( kFireTime,
								vCurPos,
								vCurDir,
								m_CreateItem );

	m_vCreateWeaponIndexList.push_back( dwWeaponIndex );

	m_iWeaponFireCnt++;

	if( !m_WeaponFireEffect.IsEmpty() )
		m_pGroup->AttachEffect( m_WeaponFireEffect, NULL );

	if( !m_WeaponFireSound.IsEmpty() )
		PlaySound( m_WeaponFireSound );
}

void ioPushStruct::MountEntityMove( const D3DXVECTOR3 &vMoveAmt )
{
	UpdateMountedEntityList();

	int iCnt = m_vMountEntityList.size();
	for( int i=0; i<iCnt; i++ )
	{
		m_vMountEntityList[i]->Translate( vMoveAmt );
		
		ioFieldItem *pItem = ToFieldItem( m_vMountEntityList[i] );
		if( pItem && !m_LastPushChar.IsEmpty())
		{
			if( m_LastPushChar != pItem->GetLastPushChar() )
			{
				pItem->SetLastPushChar( m_LastPushChar );
			}
		}
	}

	UpdateMountedWeaponList();

	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	iCnt = m_vMountWeaponList.size();
	for( i=0; i<iCnt; i++ )
	{
		vPos = m_vMountWeaponList[i]->GetPosition();

		m_vMountWeaponList[i]->SetPosition( vPos + vMoveAmt );
	}
}

void ioPushStruct::InitResource()
{
	m_pGauge = new ioGaugeHelper;
	m_pGauge->InitResource();
}

void ioPushStruct::InitPushStructInfo( int iStructNum,
									   int iIndex,
									   DWORD dwCreateGapTime,
									   DWORD dwSeed,
									   DWORD dwEtcCode,
									   const D3DXVECTOR3 &vPos,
									   const D3DXVECTOR3 &vScale )
{
	m_SearchSoundID = 0;
	m_iStructNum = iStructNum;
	m_iArrayIndex = iIndex;
	m_dwCreateEtcItemCode = dwEtcCode;

	if( m_fDefaultMinHP > 0.0f && m_fDefaultMaxHP != m_fDefaultMinHP )
	{
		int iGapHP = (int)(m_fDefaultMaxHP - m_fDefaultMinHP);
		iGapHP = max( 0, iGapHP );

		IORandom random;
		random.SetRandomSeed( dwSeed );
		int iRandom = random.Random( iGapHP );

		m_HP.m_fCurValue = m_fDefaultMinHP + iRandom;
	}

	SetWorldPosition( vPos );
	SetWorldScale( vScale );

	m_dwGapTime = FRAMEGETTIME();

	m_vCreateWeaponIndexList.clear();
	m_dwAreaWeaponIndex = 0;

	if( m_dwAreaWeaponTime == 0 )
	{
		CreateAreaWeapon( vPos );
		m_dwAreaWeaponCreateTime = 0;
	}
	else
	{
		DWORD dwCreateTime = m_dwCreateTime + m_dwAreaWeaponTime;
		
		if( dwCreateTime < dwCreateGapTime )
			m_dwAreaWeaponCreateTime = m_dwCreateTime;
		else
			m_dwAreaWeaponCreateTime = dwCreateTime - dwCreateGapTime;
	}

	ioBaseChar *pOwner = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( pOwner )
	{
		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		if( pOwner->GetTeam() == TEAM_RED && !m_RedTeamEffect.IsEmpty() )
		{
			m_pGroup->AttachEffect( m_RedTeamEffect, NULL );
		}
		else if( pOwner->GetTeam() == TEAM_BLUE )
		{
			m_pGroup->AttachEffect( m_BlueTeamEffect, NULL );
		}
		else
		{
			if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
				m_pGroup->AttachEffect( m_BlueTeamEffect, NULL );
			else
				m_pGroup->AttachEffect( m_RedTeamEffect, NULL );
		}

	}
	
	m_dwCollisionStartTime = 0;
	m_dwCollisionEndTime = 0;
	m_szCollisionBox.Clear();

	if( m_iAnimationIndex >= 0 )
	{
		if( !m_pGroup )	return;
		if( !m_pGroup->HasSkeleton() )	return;

		if( COMPARE( m_iAnimationIndex, 0, m_pGroup->GetAnimationCnt() ) )
		{
			float fAniRate = FLOAT1;
			if( m_fAnimationRate > 0.0f )
				fAniRate = m_fAnimationRate;

			m_pGroup->SetLoopAni( m_iAnimationIndex, FLOAT100, FLOAT1, FLOAT1/fAniRate );

			AniEventConstPtrList vFireStartList;
			m_pGroup->GetAnimationEventConstPtrList_e( m_iAnimationIndex, "fire_s", vFireStartList );

			std::list<float> vFireEndList;
			m_pGroup->GetAnimationEventTimeList_e( m_iAnimationIndex, "fire_e", vFireEndList );

			DWORD dwStartTime = FRAMEGETTIME();

			int i=0;
			AniEventConstPtrList::iterator iter = vFireStartList.begin();

			if( !vFireStartList.empty() )
			{
				float fFireStart = (*iter)->fEventTime;
				if( fFireStart > 0.0f )
				{
					m_dwCollisionStartTime = dwStartTime + fFireStart * fAniRate;

					if( !(*iter)->szSubInfo.IsEmpty() )
					{
						const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
						if( vParam.size() >= 1 )
						{
							ioHashString szTypeName = vParam[0].c_str();
							AniCollisionType eType = GetAniCollisionType( szTypeName );

							if( vParam.size() == 2 && eType == ACT_SPECIAL )
							{
								m_szCollisionBox = vParam[1].c_str();
							}
						}
					}
				}
			}

			std::list< float >::iterator iterEndTime = vFireEndList.begin();
			if( iterEndTime != vFireEndList.end() )
			{
				float fFireEnd = (*iterEndTime);
				if( fFireEnd > 0.0f )
				{
					m_dwCollisionEndTime = dwStartTime + fFireEnd * fAniRate;
				}
			}
		}
	}

	if( !m_SearchingEffect.IsEmpty() )
	{
		m_pGroup->AttachEffect( m_SearchingEffect, NULL );
	}

	if( !m_SearchingSound.IsEmpty() )
		m_SearchSoundID = PlaySound( m_SearchingSound, true );

	if( m_iStructType == PSTT_BOMBER )
	{
		D3DXQUATERNION qtAngle;
		float fCurAngle = ioMath::ArrangeHead(m_fBomberSencingMoveAngle);
		if( fCurAngle > 0.0f )
		{
			fCurAngle = DEGtoRAD(fCurAngle);
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, fCurAngle, 0.0f, 0.0f );

			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXQuaternionMultiply( &qtAngle, &qtRot, &qtAngle );

			m_vBomberMoveDir = qtAngle * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
			SetWorldOrientation( qtAngle );
		}
		else
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			m_vBomberMoveDir = qtRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
		}

		if( !m_pGroup )	return;
		if( !m_pGroup->HasSkeleton() )	return;
		if( m_BomberCreateAni.IsEmpty() ) return;

		int iAniID = m_pGroup->GetAnimationIdx( m_BomberCreateAni );
		float fRate = FLOAT1;
		if( m_fBomberCreateAniRate > 0.0f )
			fRate = m_fBomberCreateAniRate;

		m_pGroup->SetActionAni( iAniID, 0.0f, FLOAT100, FLOAT1, FLOAT1/fRate );
		DWORD dwDuration = m_pGroup->GetAnimationFullTime(iAniID) * fRate;

		m_dwBomberSensingEnableTime = dwDuration;
	}

	m_bAreaCheckCharPos = false;
}

bool ioPushStruct::IsCanPushed( ioPlayEntity *pEntity )
{
	if( m_State == PS_DIE ) return false;
	if( m_bNotMoveStruct ) return false;
	
	const ioOrientBox &rkPushColBox = GetWorldCollisionBox();
	m_fPushRange = rkPushColBox.GetExtents( 0 ) * rkPushColBox.GetExtents( 0 ) + 
				   rkPushColBox.GetExtents( 2 ) * rkPushColBox.GetExtents( 2 );

	m_fPushRange = sqrt( m_fPushRange );

	ioBaseChar *pTarget = ToBaseChar( pEntity );
	if( !pTarget ) 
		return false;

	ioBiped *pRHand = pEntity->GetGroup()->GetBiped( "Bip01 R Hand" );
	D3DXVECTOR3 vHandPos;
	float fCharHandLen;
	if( pRHand )
	{
		vHandPos = pRHand->GetDerivedPosition();
		vHandPos = vHandPos * pEntity->GetWorldScale();
	}

	fCharHandLen = fabs( vHandPos.z );

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	D3DXVECTOR3 vDiff = vCurPos - pEntity->GetWorldPosition();
	if( D3DXVec3Length( &vDiff ) < m_fPushRange + fCharHandLen && !m_pPushChar )
	{
		float fCosine = cosf( DEGtoRAD( 50.0f ) );
		D3DXVECTOR3 vTargetDir = pTarget->GetMoveDir();
		D3DXVec3Normalize( &vDiff, &vDiff );
		if( D3DXVec3Dot( &vDiff, &vTargetDir ) < fCosine )
			return false;
		
		float fHeightGap = fabs( vCurPos.y - pTarget->GetWorldPosition().y );
		if( fHeightGap > m_fHeightGap )
			return false;

		return true;
	}

	return false;
}

float ioPushStruct::GetPushRange() const
{
	if( m_pPushChar )
	{
		D3DXVECTOR3 vHandPos( 0.0f, 0.0f, 0.0f );

		ioBiped *pRHand = m_pPushChar->GetGroup()->GetBiped( "Bip01 R Hand" );
		if( pRHand )
		{
			vHandPos = pRHand->GetDerivedPosition() * m_pPushChar->GetWorldScale();
		}

		return m_fPushRange + fabs( vHandPos.z );		
	}

	return m_fPushRange;
}

void ioPushStruct::OnPushDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker )
{
	rkPacket >> m_HP.m_fCurValue >> m_HP.m_fMaxValue;

	bool bOnlyEmoticon;
	D3DXVECTOR3 vAttackDir;
	float fDamage, fPushPower;

	rkPacket >> bOnlyEmoticon;
	rkPacket >> vAttackDir;
	rkPacket >> fDamage;
	rkPacket >> fPushPower;

	if( fDamage > 0.0f )
	{
		AddHeadDamageInfo( fDamage );
	}

	if( !bOnlyEmoticon && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iActionStop = 0;
	rkPacket >> iActionStop;

	int iShakeCamera = 0;
	rkPacket >> iShakeCamera;

	if( iActionStop == AST_BOTH || iActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
	}

	int iWeaponIndex = 0;
	rkPacket >> iWeaponIndex;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIndex );
	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}
}

void ioPushStruct::StructDie( bool bEffect, bool bCreateStruct, bool bUsing )
{
	m_pPushChar = NULL;
	m_fCurMoveSpeed = 0.0f;
	m_State = PS_DIE;

	if( !m_WeaponFireEffect.IsEmpty() )
	{
		m_pGroup->EndEffect( m_WeaponFireEffect, true );
	}

	if( m_bMoveEffectCreated )
	{
		m_pGroup->EndEffect( m_MoveEffect, true );
		m_bMoveEffectCreated = false;
	}

	if( bEffect )
	{
		ioMapEffect *pEffect = m_pModeParent->GetCreator()->CreateMapEffect( m_DestroyEffect,
																			 GetWorldPosition(),
																			 ioMath::UNIT_ALL );

		if( pEffect )
		{
			pEffect->SetWorldOrientation( GetWorldOrientation() );
		}
	}

	// 낚시터 삭제 시
	if( m_iStructType == PSTT_FISHING_GROUND )
	{
		ioEtcItem* pFishingGroundItem = g_EtcItemMgr.FindEtcItem( m_dwCreateEtcItemCode );
		if( pFishingGroundItem )
		{
			char szBuf[MAX_PATH] = "";
			if( bUsing )
			{
				wsprintf( szBuf, STR(1), GetOwnerName().c_str(), pFishingGroundItem->GetName().c_str() );
				g_ChatMgr.SetSystemMsg( szBuf );
			}
		}
	}

	if( !m_ExplosionSound.IsEmpty() )
		PlaySound( m_ExplosionSound );
	
	UpdateMountedEntityList();
	UpdateMountedWeaponList();

	int iEntityCount = m_vMountEntityList.size();
	for( int i=0; i<iEntityCount; i++ )
	{
		ioGameEntity *pEntity = m_vMountEntityList[i];
		if( pEntity )
		{
			ioFieldItem *pItem = ToFieldItem( pEntity );
			if( pItem )
			{
				pItem->SetDropState( pItem->GetWorldPosition() );
			}
			else
			{
				pEntity->NeedUpdateBottomHeight();
			}
		}
	}

	int iWeaponCount = m_vMountWeaponList.size();	
	for( i=0; i<iWeaponCount; i++ )
	{
		ioMineWeapon *pMine = ToMineWeapon( m_vMountWeaponList[i] );
		if( !pMine || !pMine->IsCanExplosion() )
			continue;

		ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_KillerChar );
		if( pKillChar )
		{
			if( pKillChar->IsNeedSendNetwork() )
			{
				pMine->ExplosionMine( m_pModeParent->GetCreator(), true );

				if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
				{
					SP2Packet kPacket( CUPK_MINE_EXPLOSION );
					kPacket << pMine->GetOwnerName();
					kPacket << pMine->GetWeaponIndex();
					kPacket << pMine->GetPosition();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
		{
			if( P2PNetwork::IsHost() )
			{
				pMine->ExplosionMine( m_pModeParent->GetCreator(), true );

				if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
				{
					SP2Packet kPacket( CUPK_MINE_EXPLOSION );
					kPacket << pMine->GetOwnerName();
					kPacket << pMine->GetWeaponIndex();
					kPacket << pMine->GetPosition();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}

	if( !P2PNetwork::IsNetworkPlaying() || !m_pModeParent->IsNetworkMode() )
	{
		ioPlayStage *pStage = m_pModeParent->GetCreator();
		if( !pStage ) return;

		if( bCreateStruct )
		{
			int iCnt = m_PushStructInfoList.size();
			for( int i=0; i < iCnt; ++i )
			{
				if( m_PushStructInfoList[i].m_iNum <= 0 )
					return;

				DWORD dwSeed = timeGetTime();
				int iCurIndex = g_PushStructListMgr.GetCurLocalIndex() + 1;
				D3DXVECTOR3 vNewPos = GetWorldPosition() + m_PushStructInfoList[i].m_vNewPos;

				ioHashString szOwner = m_OwnerCharName;

				switch( m_PushStructInfoList[i].m_CreateType )
				{
				case SCT_KILLER:
					szOwner = m_KillerChar;
					break;
				case SCT_NO_OWNER:
					szOwner.Clear();
					break;
				}

				pStage->CreatePushStruct( m_PushStructInfoList[i].m_iNum,
										  iCurIndex,
										  vNewPos,
										  GetWorldOrientation(),
										  szOwner,
										  0,
										  dwSeed,
										  0 );
			}
		}

		ioHashString szName = GetObjectItem();
		if( szName.IsEmpty() ) return;

		ioItem *pItem = pStage->CreateItem( szName );
		if( !pItem )	return;
		pItem->ReLoadProperty();

		ioFieldItem *pField = pStage->CreateFieldItem( pItem );
		if( pField )
		{
			pField->SetWorldPositionByServerSide( GetWorldPosition() );
			pField->SetDropState( pField->GetWorldPosition() );
			pField->CreateDropEffect();
		}
	}
}

bool ioPushStruct::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	if( m_State == PS_DIE || !IsObjectCollision() )
		return true;

	return false;
}

void ioPushStruct::ClearMountEntity()
{
	if( !m_vMountEntityList.empty() )
	{
		m_vMountEntityList.erase( m_vMountEntityList.begin(), m_vMountEntityList.end() );
	}
}

void ioPushStruct::UpdateMountedEntityList()
{
	if( m_pModeParent && m_pModeParent->GetCreator() )
	{
		m_pModeParent->GetCreator()->UpdateMountedEntityList( this, m_vMountEntityList, NULL );
	}
}

void ioPushStruct::UpdateMountedWeaponList()
{
	if( m_pModeParent && m_pModeParent->GetCreator() )
	{
		m_pModeParent->GetCreator()->UpdateMountedWeaponListByBox( this, m_vMountWeaponList );
	}
}

void ioPushStruct::SendPushStructInfo()
{
	SP2Packet kPacket( CUPK_PUSHSTRUCT_INFO );
	kPacket << GetPushArrayIdx();
	kPacket << m_HP.m_fMaxValue;
	kPacket << m_HP.m_fCurValue;
	kPacket << GetWorldPosition();
	P2PNetwork::SendToAllPlayingUser( kPacket );
	if( m_pModeParent && m_pModeParent->GetCreator() )
		m_pModeParent->GetCreator()->NotifyPushStructSendInfo();
}

void ioPushStruct::OnPushStructInfo( SP2Packet &rkPacket )
{
	D3DXVECTOR3 vPos;
	rkPacket >> m_HP.m_fMaxValue;
	rkPacket >> m_HP.m_fCurValue;
	rkPacket >> vPos;

	vPos.y = m_pModeParent->GetMapHeight( vPos.x, vPos.z, this, false );
	SetWorldPosition( vPos );
}

ioHashString ioPushStruct::GetObjectItem()
{
	if( !m_ObjectItemList.empty() )
	{
		int iRandNum = rand()%m_ObjectItemList.size();
		return m_ObjectItemList[iRandNum];
	}

	return ioHashString();	
}

void ioPushStruct::DropZoneCheck()
{
	if( m_bSendedDieMessage )	return;

	D3DXVECTOR3 vPos = GetWorldPosition();

	if( m_HP.m_fCurValue > 0.0f && vPos.y <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;

		if( m_LastPushChar.IsEmpty() )
			m_KillerChar = m_OwnerCharName;
		else
			m_KillerChar = m_LastPushChar;

		if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
		{
			ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_KillerChar );
			if( pKillChar )
			{
				if( pKillChar->IsNeedSendNetwork() )
				{
					if( m_iStructType == PSTT_BOMBER )
						SendDieMessageToServer( true, false );
					else
						SendDieMessageToServer( true, true );
				}
			}
			else
			{
				if( P2PNetwork::IsHost() )
				{
					if( m_iStructType == PSTT_BOMBER )
						SendDieMessageToServer( true, false );
					else
						SendDieMessageToServer( true, true );
				}
			}
		}
		else
		{
			if( m_iStructType == PSTT_BOMBER )
				StructDie( true, false );
			else
				StructDie( true, true );
		}
	}
}

void ioPushStruct::CheckLastPushChar()
{
	if( m_LastPushChar.IsEmpty() )
		return;

	ioBaseChar *pChar = m_pModeParent->GetCreator()->GetBaseChar( m_LastPushChar );
	if( !pChar )
	{
		m_LastPushChar.Clear();
	}
	else
	{
		if( m_State == PS_DELAY && (FRAMEGETTIME() - m_dwSetPushTime) > 10000 )
		{
			m_LastPushChar.Clear();
			m_dwSetPushTime = 0;
		}
	}
}

bool ioPushStruct::IsCanSendNetwork()
{
	bool bSend = false;

	if( !m_OwnerCharName.IsEmpty() )
	{
		if( m_pModeParent->CheckCharExist( m_OwnerCharName ) &&
			m_pModeParent->GetOwnerName() == m_OwnerCharName )
			bSend = true;
		else if( !m_pModeParent->CheckCharExist( m_OwnerCharName ) && P2PNetwork::IsHost() )
			bSend = true;
	}
	else if( m_OwnerCharName.IsEmpty() && P2PNetwork::IsHost() )
	{
		bSend = true;
	}

	return bSend;
}

void ioPushStruct::SendDieMessageToServer( bool bEffect, bool bSendStructInfo )
{
	if( m_bSendedDieMessage )
		return;

    SP2Packet kPacket( CTPK_PUSHSTRUCT_DIE );
	kPacket << GetPushArrayIdx();
	kPacket << bEffect;

	if( bSendStructInfo )
	{
		int iCnt = m_PushStructInfoList.size();
		kPacket << iCnt;
		for( int i=0; i < iCnt; ++i )
		{
			D3DXVECTOR3 vNewPos = GetWorldPosition() + m_PushStructInfoList[i].m_vNewPos;
			kPacket << m_PushStructInfoList[i].m_iNum;
			kPacket << vNewPos;
			kPacket << GetWorldOrientation();
			kPacket << m_PushStructInfoList[i].m_CreateType;

			if( m_PushStructInfoList[i].m_CreateType == SCT_KILLER )
				kPacket << m_KillerChar;
		}
	}
	else
	{
		kPacket << 0;
	}

	kPacket << GetWorldPosition();
	kPacket << GetObjectItem();

	TCPNetwork::SendToServer( kPacket );

	m_bSendedDieMessage = true;
}

void ioPushStruct::CheckStructDieByHp()
{
	if( m_dwTicTime <= 0 || m_fDecreaseHP <= 0.0f ) return;
	if( m_bSendedDieMessage )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwGapTime );
	if( iTickGap >= (int)m_dwTicTime )
	{
		m_dwGapTime = dwCurTime - ( iTickGap - m_dwTicTime );
		m_HP.m_fCurValue -= m_fDecreaseHP;
	}

	bool bDie = false;
	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		bDie = true;
	}

	if( bDie )
	{
		m_KillerChar = m_OwnerCharName;

		if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
		{
			if( IsCanSendNetwork() )
			{
				if( m_iStructType == PSTT_BOMBER )
					SendDieMessageToServer( m_bBomberOnlyCharBoom, !m_bBomberOnlyCharBoom );
				else
					SendDieMessageToServer( true, true );
			}
		}
		else
		{
			if( m_iStructType == PSTT_BOMBER )
				StructDie( m_bBomberOnlyCharBoom, !m_bBomberOnlyCharBoom );
			else
				StructDie( true, true );
		}
	}
}

void ioPushStruct::CheckAreaWeapon()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAreaWeaponCreateTime > 0 && m_dwAreaWeaponCreateTime < dwCurTime )
	{
		CreateAreaWeapon( GetWorldPosition() );
		m_dwAreaWeaponCreateTime = 0;
	}
}

void ioPushStruct::SetStructDie( ioBaseChar *pChar, bool bEffect )
{
	bool bSend = false;
	if( !m_OwnerCharName.IsEmpty() )
	{
		if( pChar && pChar->IsNeedSendNetwork() )
			bSend = true;
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		if( bSend )
		{
			SendDieMessageToServer( bEffect, false );
		}
	}
	else
	{
		StructDie( bEffect, false );
	}
}

void ioPushStruct::SetStructDieByOwnerClear( ioBaseChar *pChar, bool bEffect )
{
	bool bSend = false;
	if( !m_OwnerCharName.IsEmpty() )
	{
		if( pChar && pChar->IsNeedSendNetwork() )
			bSend = true;
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		if( bSend && !m_bSendedDieMessage )
		{
			m_bSendedDieMessage = true;
		}
	}
	else
	{
		StructDie( bEffect, false );
	}
}

void ioPushStruct::CreateAreaWeapon( const D3DXVECTOR3 &vPos )
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	if( m_AreaWeaponName.IsEmpty() )
		return;

	if( m_dwAreaWeaponIndex != 0 )
		return;

	D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( m_OwnerCharName,
																  "",
																  "",
																  m_AreaWeaponName,
																  vPos,
																  qtRot,
																  ioAreaWeapon::CT_STRUCT );
	if( pAreaWeapon )
	{
		m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
	}
}

void ioPushStruct::TranslateByWorldEntity( const D3DXVECTOR3 &vMove )
{
	UpdateMountedWeaponList();

	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	int iCnt = m_vMountWeaponList.size();
	for( int i=0; i<iCnt; i++ )
	{
		vPos = m_vMountWeaponList[i]->GetPosition();

		m_vMountWeaponList[i]->SetPosition( vPos + vMove );
	}

	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
	{
		pAreaWeapon->SetAreaPosition( GetWorldPosition(), m_pModeParent->GetCreator() );
	}
}

void ioPushStruct::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	if( m_iStructType == PSTT_BLOCK )
		return;

	UpdateMountedEntityList();

	int iCnt = m_vMountEntityList.size();
	for( int i=0; i<iCnt; i++ )
	{
		m_vMountEntityList[i]->TranslateByWorldEntity( vMove );
		
		ioFieldItem *pItem = ToFieldItem( m_vMountEntityList[i] );
		if( pItem && !m_LastPushChar.IsEmpty())
		{
			if( m_LastPushChar != pItem->GetLastPushChar() )
			{
				pItem->SetLastPushChar( m_LastPushChar );
			}
		}
	}

	UpdateMountedWeaponList();

	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	iCnt = m_vMountWeaponList.size();
	for( i=0; i<iCnt; i++ )
	{
		vPos = m_vMountWeaponList[i]->GetPosition();

		m_vMountWeaponList[i]->SetPosition( vPos + vMove );
	}

	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
	{
		pAreaWeapon->SetAreaPosition( GetWorldPosition(), m_pModeParent->GetCreator() );
	}
}

bool ioPushStruct::CheckCreateWeaponIndex( int iIndex )
{
	int iCnt = m_vCreateWeaponIndexList.size();

	for( int i=0; i < iCnt; ++i )
	{
		if( m_vCreateWeaponIndexList[i] == iIndex )
			return true;
	}

	return false;
}

bool ioPushStruct::GetCollisionRect( RECT *pRect ) const
{
	const ioAxisAlignBox &rkAxisBox = GetWorldAxisBox();

	int iMaxXPos=0, iMaxYPos=0;
	int iMinXPos=0, iMinYPos=0;

	if( !Setting::Check2DInScreen( rkAxisBox.GetMaxPos(), iMaxXPos, iMaxYPos ) )
		return false;

	if( !Setting::Check2DInScreen( rkAxisBox.GetMinPos(), iMinXPos, iMinYPos ) )
		return false;

	pRect->top = iMaxYPos;
	pRect->right = iMaxXPos;
	pRect->left = iMinXPos;
	pRect->bottom = iMinYPos;

	return true;
}

void ioPushStruct::ApplyOutLineChange( ChangeOutLineType eType, const D3DCOLORVALUE &rkColor, float fSize )
{
	RemoveChangeOutLineRecord( eType );

	OutLineRecord kRecord;
	kRecord.m_Type   = eType;
	kRecord.m_Color  = rkColor;
	kRecord.m_fThick = fSize;
	m_OutLineChangeList.push_front( kRecord );

	ChangeOutLine( rkColor, fSize );
}

void ioPushStruct::RestoreOutLineChange( ChangeOutLineType eType )
{
	RemoveChangeOutLineRecord( eType );

	if( !m_OutLineChangeList.empty() )
	{
		const OutLineRecord &rkRecord = m_OutLineChangeList.front();
		ChangeOutLine( rkRecord.m_Color, rkRecord.m_fThick );
	}
	else
	{
		// 최초 상태로 원상복구
		RestoreOutLine();
	}
}

bool ioPushStruct::HasOutLineType( ChangeOutLineType eType )
{
	OutLineRecordList::iterator iter;
	for( iter=m_OutLineChangeList.begin() ; iter!=m_OutLineChangeList.end() ; ++iter )
	{
		if( iter->m_Type == eType )
			return true;
	}

	return false;
}

void ioPushStruct::RemoveChangeOutLineRecord( ChangeOutLineType eType )
{
	OutLineRecordList::iterator iter = m_OutLineChangeList.begin();
	while( iter != m_OutLineChangeList.end() )
	{
		if( iter->m_Type == eType )
		{
			iter = m_OutLineChangeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPushStruct::ChangeOutLine( const D3DCOLORVALUE &rkOutLineColor, float fOutLineSize )
{
	m_OutLineColor = rkOutLineColor;
	m_fOutLineSize = fOutLineSize;
	m_bOutLineModified = true;

	ApplyModifiedOutLine();
}

void ioPushStruct::RestoreOutLine()
{
	if( !m_bOutLineModified )	return;

	int iNumEntity = m_pGroup->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );
			if( !pSubEntity )	continue;

			ioTechnique *pCloneTech = NULL;
			ioMaterial *pClone = pSubEntity->GetMaterial();
			if( pClone )
			{
				pCloneTech = pClone->GetBestTechnique();
			}

			ioTechnique *pSrcTech = NULL;
			ioMaterial *pSrc = ioMaterialManager::GetSingleton().GetMaterial( pSubEntity->GetMaterialName() );
			if( pSrc )
			{
				pSrcTech = pSrc->GetBestTechnique();
			}

			if( !pSrcTech || !pCloneTech )
				continue;

			ioPass *pClonePass = pCloneTech->GetPassByShaderGrp( SGT_OUTLINE );
			ioPass *pSrcPass = pSrcTech->GetPassByShaderGrp( SGT_OUTLINE );

			if( !pClonePass || !pSrcPass )
				continue;

			ioShaderCustomArg *pArg = pSrcPass->GetCustomArg( "OutLineColor" );
			if( pArg )
			{
				pClonePass->AddCustomParameter( *pArg );
			}

			pArg = pSrcPass->GetCustomArg( "OutLineThickness" );
			if( pArg )
			{
				pClonePass->AddCustomParameter( *pArg );
			}

			if( !Setting::UseObjectEdge() )
			{
				pClonePass->SetPassEnable( false );
			}
		}
	}

	m_bOutLineModified = false;
}

void ioPushStruct::ApplyModifiedOutLine()
{
	if( !m_bOutLineModified )	return;

	ioShaderCustomArg kLineColor( "OutLineColor" );
	kLineColor.SetValue( m_OutLineColor );

	ioShaderCustomArg kLineThickness( "OutLineThickness" );
	kLineThickness.SetValue( m_fOutLineSize );

	int iNumEntity = m_pGroup->GetNumEntities();
	for( int i=0; i<iNumEntity; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );

			ioMaterial *pMaterial = pSubEntity->GetMaterial();
			if( !pMaterial ) continue;

			ioTechnique *pTech = pMaterial->GetBestTechnique();
			if( !pTech ) continue;

			ioPass *pOutLine = pTech->GetPassByShaderGrp( SGT_OUTLINE );
			if( pOutLine )
			{
				pOutLine->SetPassEnable( true );
				pOutLine->AddCustomParameter( kLineColor );
				pOutLine->AddCustomParameter( kLineThickness );
			}
		}
	}
}

void ioPushStruct::ProcessBomberState( float fTimePerSec )
{
	ProcessBomberMoveState( fTimePerSec );

	m_dwProcessTime = FRAMEGETTIME();

	DropZoneCheck();
}

void ioPushStruct::ProcessBomberMoveState( float fTimePerSec )
{
	if( m_iStructType != PSTT_BOMBER ) return;

	switch( m_BomberState )
	{
	case BBS_WAIT:
		BomberMoving( fTimePerSec );
		break;
	case BBS_SENCING:
		BomberSencing( fTimePerSec );
		break;
	case BBS_TRACE:
		BomberTracing( fTimePerSec );
		break;
	}
}

void ioPushStruct::BomberMoving( float fTimePerSec )
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCreateTime + m_dwBomberSensingEnableTime < dwCurTime )
	{
		SetBomberSencingState();
		return;
	}

	// 이동처리
	float fMoveDist = 0.0f;
	BomberProcessMoving( pStage, fMoveDist, fTimePerSec );
}

void ioPushStruct::BomberSencing( float fTimePerSec )
{
	if( !m_BomberTargetName.IsEmpty() )
		m_BomberTargetName.Clear();

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_dwBomberSensingStartTime + m_dwBomberSensingDuration < dwCurTime )
	{
		float fMoveDist = m_fBomberMoveSpeed * fTimePerSec;
		BomberProcessMoving( pStage, fMoveDist, fTimePerSec );
		return;
	}

	float fMoveDist = m_fBomberMoveSpeed * fTimePerSec;
	BomberProcessMoving( pStage, fMoveDist, fTimePerSec );

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE ) return;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fSensingRangeSq = m_fBomberSensingRange * m_fBomberSensingRange;

	float fSensingHalfAnigle = m_fBomberSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fBomberSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_BomberTargetName = szTarget;
			SetBomberTraceState( pStage, true );
		}
	}
}

void ioPushStruct::BomberTracing( float fTimePerSec )
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwBomberTraceEndTime > dwCurTime )
	{
		CheckTraceTarget( pStage );
		FindTraceTarget( pStage );

		ioBaseChar *pTarget = pStage->GetBaseChar( m_BomberTargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fBomberTraceRange * m_fBomberTraceRange )
			{
				D3DXVECTOR3 vTargetDir = vDiff;
				vTargetDir.y = 0;
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );

				if( m_dwBomberRotateSpeed > 0 )
				{
					D3DXQUATERNION	qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );

					float fAngle = D3DXVec3Dot( &vTargetDir, &m_vBomberMoveDir );

					float fAngleGap = RADtoDEG( acosf(fAngle) );
					float fRate = (fTimePerSec*1000) / m_dwBomberRotateSpeed;
					float fRotateAngle = 360.0f * fRate;

					if( fAngleGap < fRotateAngle )
					{
						m_vBomberMoveDir = vTargetDir;
					}
					else
					{
						D3DXQUATERNION qtRotate;
						ioMath::CalcDirectionQuaternion( qtRotate, -m_vBomberMoveDir );

						bool bLeftRot = false;
						if( D3DXVec3Dot( &vTargetDir, &m_vBomberMoveDir ) > -FLOAT1 )
						{
							float fCurYaw, fTargetYaw;
							fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
							fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

							fCurYaw = ioMath::ArrangeHead( fCurYaw );
							fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

							float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
							if( fRotAngle < 0.0f )
								bLeftRot = true;
						}

						if( bLeftRot )
							fRotateAngle *= -1;

						fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

						D3DXQUATERNION	qtNewRot;
						D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
						m_vBomberMoveDir = qtNewRot * m_vBomberMoveDir;
					}
				}
				else
				{
					m_vBomberMoveDir = vTargetDir;
				}
			}
		}
		else
		{
			m_BomberTargetName.Clear();
		}
	}

	float fMoveDist = m_fBomberTraceSpeed * fTimePerSec;
	BomberProcessMoving( pStage, fMoveDist, fTimePerSec );
}

void ioPushStruct::SetBomberSencingState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_BomberState = BBS_SENCING;
	m_dwBomberSensingStartTime = dwCurTime;

	if( !m_pGroup )	return;
	if( !m_pGroup->HasSkeleton() )	return;
	if( m_BomberSencingAni.IsEmpty() ) return;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_BomberSencingAni );
	float fRate = FLOAT1;
	if( m_fBomberSencingAniRate > 0.0f )
		fRate = m_fBomberSencingAniRate;

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
}

void ioPushStruct::SetBomberTraceState( ioPlayStage *pStage, bool bSendNetwork )
{
	m_BomberState = BBS_TRACE;
	m_dwBomberTraceEndTime = FRAMEGETTIME() + m_dwBomberTraceDuration;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			// HARDCODE :: TestCode Add 2012.04.02. KDH
			SP2Packet kPacket( CUPK_PUSHSTRUCT_BOMBER );
			kPacket << GetPushArrayIdx();
			kPacket << m_iStructNum;
			kPacket << 1;
			kPacket << GetWorldPosition();
			kPacket << m_BomberTargetName;
			kPacket << true;			// first
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( !m_pGroup )	return;
	if( !m_pGroup->HasSkeleton() )	return;
	if( m_BomberTracingAni.IsEmpty() ) return;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_BomberTracingAni );
	float fRate = FLOAT1;
	if( m_fBomberTracingAniRate > 0.0f )
		fRate = m_fBomberTracingAniRate;

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
}

bool ioPushStruct::IsEnableTargetState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( pChar->IsSystemState() )
		return false;
			
	if( !pChar->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( pChar->IsApplyDownState() )
			return false;
	}
	else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pChar->IsFloating() )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{
		if( !pChar->IsApplyDownState() || pChar->IsFloating() )
			return false;
	}

	return true;
}

void ioPushStruct::BomberProcessMoving( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec )
{
	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vPos = vPrePos;

	m_vBomberMoveDir.y = 0.0f;
	D3DXVec3Normalize( &m_vBomberMoveDir, &m_vBomberMoveDir );

	vPos += m_vBomberMoveDir * fMoveAmt;

	float fTopHeight = m_fBomberEnableMoveHeight * 4.0f;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+fTopHeight );
	float fGapHeight = fMapHeight - vPos.y;

	if( m_fBomberEnableMoveHeight < fabs(fGapHeight) )
	{
		if( fGapHeight <= 0.0f )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vBomberMoveDir );
			SetWorldOrientation( qtRot );

			m_KillerChar = m_OwnerCharName;
			if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
			{
				if( IsCanSendNetwork() )
				{
					if( m_iStructType == PSTT_BOMBER )
						SendDieMessageToServer( m_bBomberOnlyCharBoom, !m_bBomberOnlyCharBoom );
					else
						SendDieMessageToServer( true, true );
				}
			}
			else
			{
				if( m_iStructType == PSTT_BOMBER )
					StructDie( m_bBomberOnlyCharBoom, !m_bBomberOnlyCharBoom );
				else
					StructDie( true, true );
			}

			return;
		}
		else
		{
			float fNewMapHeight = 0.0f;
			D3DXVECTOR3 vDiff = vPrePos;

			if( fabs(m_vBomberMoveDir.x) >= fabs(m_vBomberMoveDir.z) )
			{
				if( m_vBomberMoveDir.x >= 0.0f )
					vDiff += D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
			}
			else
			{
				if( m_vBomberMoveDir.z >= 0.0f )
					vDiff += D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * fMoveAmt;
			}

			fNewMapHeight = pStage->GetMapHeightNoneEntity( vDiff.x, vDiff.z, vDiff.y+fTopHeight );

			if( m_fBomberEnableMoveHeight < fabs(vDiff.y - fNewMapHeight) )
			{
				return;
			}

			vPos = vDiff;
			fMapHeight = fNewMapHeight;
		}
	}

	vPos.y = fMapHeight;
	SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vBomberMoveDir );
	SetWorldOrientation( qtRot );
}

void ioPushStruct::OnPushStructBomber( SP2Packet &rkPacket )
{
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	SetWorldPosition( vPos );
	rkPacket >> m_BomberTargetName;

	bool bFirstTrace;
	rkPacket >> bFirstTrace;

	if( bFirstTrace )
	{
		ioPlayStage *pStage = m_pModeParent->GetCreator();
		SetBomberTraceState( pStage, false );
	}
}

void ioPushStruct::CheckBomberEnd()
{
	if( m_bSendedDieMessage )	return;
	if( m_BomberTargetName.IsEmpty() ) return;
	if( m_BomberState != BBS_TRACE ) return;

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_BomberTargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vPrePos = GetWorldPosition();
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

		// 각도 체크
		bool bEnableAngle = false;
		float fBoomHalfAnigle = m_fBomberBoomAngle * FLOAT05;
		fBoomHalfAnigle = ioMath::ArrangeHead(fBoomHalfAnigle);
		float fHalfValue = cosf( DEGtoRAD(fBoomHalfAnigle) );

		if( m_fBomberBoomAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2;
			D3DXVec3Normalize( &vDiff2, &vDiff );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fHalfValue <= fCurValue )
				bEnableAngle = true;
		}
		else
		{
			bEnableAngle = true;
		}
	
		float fRangeSq = D3DXVec3LengthSq( & vDiff );
		if( bEnableAngle && fRangeSq < m_fBomberBoomRange * m_fBomberBoomRange )
		{
			m_KillerChar = m_OwnerCharName;
			if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
			{
				if( IsCanSendNetwork() )
				{
					SendDieMessageToServer( false, true );
				}
			}
			else
			{
				StructDie( false, true );
			}
		}
	}
}

void ioPushStruct::FindTraceTarget( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerCharName );
	if( !pOwner ) return;
	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fSensingRangeSq = m_fBomberSensingRange * m_fBomberSensingRange;

	float fSensingHalfAnigle = m_fBomberSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetMidPositionByRate();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fBomberSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetMidPositionByRate();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() && szTarget != m_BomberTargetName )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_BomberTargetName = szTarget;

			if( pTarget->IsNeedSendNetwork() )
			{
				// HARDCODE :: TestCode Add 2012.04.02. KDH
				SP2Packet kPacket( CUPK_PUSHSTRUCT_BOMBER );
				kPacket << GetPushArrayIdx();
				kPacket << m_iStructNum;
				kPacket << 2;
				kPacket << GetWorldPosition();
				kPacket << m_BomberTargetName;
				kPacket << false;			// first
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioPushStruct::CheckTraceTarget( ioPlayStage *pStage )
{
	if( !pStage )
		return false;

	if( m_BomberTargetName.IsEmpty() )
		return false;

	bool bLost = false;
	ioBaseChar *pTarget = pStage->GetBaseChar( m_BomberTargetName );
	if( pTarget )
	{
		if( !m_BomberTargetName.IsEmpty() )
		{
			if( pTarget->IsSystemState() )
			{
				m_BomberTargetName.Clear();
			}
		}

		if( !m_BomberTargetName.IsEmpty() && pTarget->HasHideBuff() )
		{
			m_BomberTargetName.Clear();
		}

		if( !m_BomberTargetName.IsEmpty() && pTarget->IsChatModeState( false ) )
		{
			m_BomberTargetName.Clear();
		}

		if( !m_BomberTargetName.IsEmpty() )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq >= m_fBomberTraceRange * m_fBomberTraceRange )
			{
				m_BomberTargetName.Clear();
			}
		}
	}
	else
	{
		m_BomberTargetName.Clear();
	}

	if( m_BomberTargetName.IsEmpty() )
		bLost = true;

	if( bLost )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			// HARDCODE :: TestCode Add 2012.04.02. KDH
			SP2Packet kPacket( CUPK_PUSHSTRUCT_BOMBER );
			kPacket << GetPushArrayIdx();
			kPacket << m_iStructNum;
			kPacket << 3;
			kPacket << GetWorldPosition();
			kPacket << m_BomberTargetName;
			kPacket << false;			// first
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return false;
}

bool ioPushStruct::IsCharCollisionSkipState( TeamType eTeamType, int iState ) const
{
	if( m_iStructType == PSTT_BOMBER )
		return true;

	return false;
}

void ioPushStruct::AddEntityEvent( ioEntity *pEntity )
{
	if( m_pModeParent == NULL )
		return;

	ioBaseChar *pOwnerChar = m_pModeParent->GetCreator()->GetBaseChar( m_OwnerCharName );
	if( pOwnerChar )
	{
		pOwnerChar->AddEntityEvent( pEntity );
	}
}

void ioPushStruct::ProcessBlockState( float fTimePerSec )
{
	bool bRePosition = false;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fMapHeight = GetBottomHeight();

	if( !m_bNoDropStruct && !m_bNoDropProcess && vCurPos.y != fMapHeight )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwProcessTime;
		do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( vCurPos.y > fMapHeight )
			{
				m_fGravityAmt += m_pModeParent->GetCreator()->GetRoomGravity() * fNewTimePerSec;
				vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

				if( vCurPos.y == fMapHeight )
					m_fGravityAmt = 0.0f;

				bRePosition = true;
			}
		} while( dwGapTime > 0 );

		if( vCurPos.y < fMapHeight )
		{
			vCurPos.y = fMapHeight;
			m_fGravityAmt = 0.0f;
			bRePosition = true;
		}

		SetWorldPosition( vCurPos );

		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
		if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
		{
			pAreaWeapon->SetAreaPosition( vCurPos, m_pModeParent->GetCreator() );
		}
	}
	else
	{
		m_bNoDropProcess = true;
	}

	if( bRePosition )
	{
		if( !m_vMountWeaponList.empty() )
		{
			const ioOrientBox &rkBox = GetWorldCollisionBox();
			float fNewYPos = rkBox.GetCenter().y + rkBox.GetExtents( 1 );

			int iCount = m_vMountWeaponList.size();
			for( int i=0; i<iCount; i++ )
			{
				vCurPos = m_vMountWeaponList[i]->GetPosition();
				vCurPos.y = fNewYPos;

				m_vMountWeaponList[i]->SetPosition( vCurPos );
			}
		}
	}
	else
	{
		if( m_State == PS_DELAY )
		{
			ProcessAttack( fTimePerSec );
		}
	}

	m_dwProcessTime = FRAMEGETTIME();
}

void ioPushStruct::ProcessFishingGroundState( float fTimePerSec )
{
	bool bRePosition = false;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fMapHeight = GetBottomHeight();

	if( !m_bNoDropStruct && !m_bNoDropProcess && vCurPos.y != fMapHeight )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwProcessTime;
		do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( vCurPos.y > fMapHeight )
			{
				m_fGravityAmt += m_pModeParent->GetCreator()->GetRoomGravity() * fNewTimePerSec;
				vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

				if( vCurPos.y == fMapHeight )
					m_fGravityAmt = 0.0f;

				bRePosition = true;
			}
		} while( dwGapTime > 0 );

		if( vCurPos.y < fMapHeight )
		{
			vCurPos.y = fMapHeight;
			m_fGravityAmt = 0.0f;
			bRePosition = true;
		}

		SetWorldPosition( vCurPos );

		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
		if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
		{
			pAreaWeapon->SetAreaPosition( vCurPos, m_pModeParent->GetCreator() );
		}
	}
	else
	{
		m_bNoDropProcess = true;
	}

	if( bRePosition )
	{
		if( !m_vMountWeaponList.empty() )
		{
			const ioOrientBox &rkBox = GetWorldCollisionBox();
			float fNewYPos = rkBox.GetCenter().y + rkBox.GetExtents( 1 );

			int iCount = m_vMountWeaponList.size();
			for( int i=0; i<iCount; i++ )
			{
				vCurPos = m_vMountWeaponList[i]->GetPosition();
				vCurPos.y = fNewYPos;

				m_vMountWeaponList[i]->SetPosition( vCurPos );
			}
		}
	}
	else
	{
		if( m_State == PS_DELAY )
		{
			ProcessAttack( fTimePerSec );
		}
	}

	m_dwProcessTime = FRAMEGETTIME();
}

void ioPushStruct::CheckSpecialCollisionBox( const ioHashString &szFileName )
{
	if( szFileName.IsEmpty() ) return;

	SAFEDELETE( m_pSpecialBox );

	m_pSpecialBox = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szFileName.c_str(), true );
}

ioOrientBox ioPushStruct::GetAttackCollisionBox( AniCollisionType eType, bool bNoScale )
{
	AniCollisionBox *pColBox = NULL;
	float fBoxScaleRate = FLOAT1;
	float fBoxMoveOffset = 0.0f;

	if( !pColBox && eType == ACT_SPECIAL )
	{
		if( m_pSpecialBox && m_pSpecialBox->HasCollisionBox( eType ) )
		{
			pColBox = m_pSpecialBox->GetCollisionBox( eType );
			fBoxMoveOffset = 0.0f;
		}
	}

	ioOrientBox kBox;
	if( pColBox )
	{
		ioNode *pNode = m_pGroup->GetParentNode();
		ioBiped *pBiped = m_pGroup->GetBiped( pColBox->m_LinkBone );
		if( pNode && pBiped )
		{
			kBox = pColBox->m_Box;

			if( fBoxMoveOffset != 0.0f )
			{
				kBox.SetCenter( kBox.GetCenter() - kBox.GetAxis(2) * fBoxMoveOffset );
			}

			D3DXMATRIX matWorld;
			D3DXMatrixMultiply( &matWorld, pBiped->GetBipedSpaceFullXform(), pNode->GetFullTransform() );
			kBox.Transform( &matWorld );
		}
	}
	else
	{
		kBox = GetWorldCollisionBox();
	}

	return kBox;
}