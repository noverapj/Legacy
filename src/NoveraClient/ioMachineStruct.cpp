

#include "stdafx.h"

#include "ioMachineStruct.h"

#include "ioVoiceChat/Fourier.h"

#include "ioGaugeHelper.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "WeaponDefine.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

#define MAX_MAP_CONTINUOUS_COUNT 10

ioMachineStruct::ioMachineStruct( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioPlayEntity( pGrp, pMode )
{
	m_bLive = true;

	m_iArrayIndex = 0;
	m_ArmorType = AT_UNARMOR;

	m_fDefaultMaxHP = FLOAT1;
	m_fDefaultMinHP = 0.0f;

	m_iMapTranslateContinuousCount = 0;
	m_dwMapTranslateFrame = 0;

	Help::InitColorValue( m_OutLineColor, 0.0f, 0.0f, 0.0f, FLOAT1 );
	m_fOutLineSize = FLOAT1;
	m_bOutLineModified = false;

	m_bSendedDieMessage  = false;
	m_bMapCollisionPushed = false;

	m_pSpecialBox = NULL;

	m_bEnableTakeEffect = false;

	// charging
	m_dwChargingStartTime = 0;
	m_fCurChargeRate = 0.0f;
}

ioMachineStruct::~ioMachineStruct()
{
	SAFEDELETE( m_pSpecialBox );
}

void ioMachineStruct::LoadProperty( ioINILoader &rkLoader )
{
	m_ArmorType	    = (ArmorType)rkLoader.LoadInt_e( "armor_type", AT_UNARMOR );	

	m_fDefaultMaxHP  = rkLoader.LoadFloat_e( "max_hp", FLOAT10 );
	m_fDefaultMinHP  = rkLoader.LoadFloat_e( "min_hp", 0.0f );
	m_HP.m_fMaxValue = m_fDefaultMaxHP;
	m_HP.m_fCurValue = m_fDefaultMaxHP;

	m_HP.m_fRecovery = rkLoader.LoadFloat_e( "hp_recovery", 0.0f );
	m_HP.m_dwRecoveryTick = rkLoader.LoadInt_e( "hp_recovery_tick", 0 );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "destroy_effect", "", szBuf, MAX_PATH );
	m_DestroyEffect = szBuf;
	rkLoader.LoadString_e( "destroy_sound", "", szBuf, MAX_PATH );
	m_DestroySound = szBuf;
	rkLoader.LoadString_e( "struct_block_buff", "", szBuf, MAX_PATH );
	m_BlockBuff = szBuf;
	
	m_fTakeEnableRange = rkLoader.LoadFloat_e( "take_enable_range", 0.0f );

	rkLoader.LoadString_e( "enable_take_effect", "", szBuf, MAX_PATH );
	m_EnableTakeEffect = szBuf;

	rkLoader.LoadString_e( "machine_take_ani", "", szBuf, MAX_PATH );
	m_TakeCharMotion = szBuf;
	m_fTakeCharMotionRate = rkLoader.LoadFloat_e( "machine_take_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "weapon_fire_effect", "", szBuf, MAX_PATH );
	m_WeaponFireEffect = szBuf;
	rkLoader.LoadString_e( "weapon_fire_sound", "", szBuf, MAX_PATH );
	m_WeaponFireSound = szBuf;

	rkLoader.LoadString_e( "camera_buff", "", szBuf, MAX_PATH );
	m_CameraBuff = szBuf;

	// charging
	m_dwChargingMaxTime = (DWORD)rkLoader.LoadInt_e( "charging_max_time", 0 );

	m_fMaxDamageRate = rkLoader.LoadFloat_e( "charging_max_damage_rate", FLOAT1 );
	m_fMaxForceRate = rkLoader.LoadFloat_e( "charging_max_force_rate", FLOAT1 );
	m_fMaxBlowRate = rkLoader.LoadFloat_e( "charging_max_blow_rate", FLOAT1 );
	m_fMaxSpeedRate = rkLoader.LoadFloat_e( "charging_max_speed_rate", FLOAT1 );
	m_fMaxFloatRate = rkLoader.LoadFloat_e( "charging_max_float_rate", 0.0f );

	rkLoader.LoadString_e( "charging_effect", "", szBuf, MAX_PATH );
	m_ChargingEffect = szBuf;
}

bool ioMachineStruct::IsLive()
{
	return m_bLive;
}

void ioMachineStruct::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ProcessState( fTimePerSec );

	if( m_iMapTranslateContinuousCount < MAX_MAP_CONTINUOUS_COUNT )
	{
		if( m_dwMapTranslateFrame + 1 < g_FrameTimer.GetFrameCounter() )
		{
			m_iMapTranslateContinuousCount = 0;
		}
	}
}

void ioMachineStruct::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

ApplyWeaponExp ioMachineStruct::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioMachineStruct::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
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

	m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bOnlyEmoticon = true;
	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	if( bContactType && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
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
						SendDieMessageToServer( true );
					}
				}
				else
				{
					if( P2PNetwork::IsHost() )
					{
						SendDieMessageToServer( true );
					}
				}
			}
			else
			{
				StructDie();
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

	return AWE_EXPLOSION;
}

bool ioMachineStruct::IsNeedCheckMapCollision() const
{
	if( m_iMapTranslateContinuousCount >= MAX_MAP_CONTINUOUS_COUNT )
		return false;

	return true;
}

bool ioMachineStruct::IsMountAvailable() const
{
	return true;
}

bool ioMachineStruct::IsNowMountEnable() const
{
	return false;
}

ioWorldEventReceiver* ioMachineStruct::GetWorldEventReceiver()
{
	return dynamic_cast<ioWorldEventReceiver*>(this);
}

void ioMachineStruct::OnTelepotation( const D3DXVECTOR3 &vTargetPos )
{
	SetWorldPosition( vTargetPos );
}

void ioMachineStruct::OnFireZoneDamage( float fDamage, const ioHashString &szBuff )
{
	// 데미지 처리?
}

void ioMachineStruct::OnPoisonZoneDamage()
{
	if( m_HP.m_fCurValue == 0.0f )
		return;

	m_HP.m_fCurValue = 0.0f;
	m_KillerChar = m_TakeCharName;

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_KillerChar );
		if( pKillChar )
		{
			if( pKillChar->IsNeedSendNetwork() )
			{
				SendDieMessageToServer( true );
			}
		}
		else
		{
			if( P2PNetwork::IsHost() )
			{
				SendDieMessageToServer( true );
			}
		}
	}
	else
	{
		StructDie();
	}
}

bool ioMachineStruct::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	return true;
}

int ioMachineStruct::DontMoveEntityLevel() const
{
	return 6;
}

ioPlayEntity::PlaySubType ioMachineStruct::GetSubType() const
{
	return PST_MACHINE_STRUCT;
}

MachineStructType ioMachineStruct::GetType() const
{
	return MCT_NONE;
}

ArmorType ioMachineStruct::GetArmorType() const
{
	return m_ArmorType;
}

void ioMachineStruct::NeedUpdateBottomHeight()
{
	ioPlayEntity::NeedUpdateBottomHeight();
}

void ioMachineStruct::Translate( const D3DXVECTOR3 &vMove )
{
	ioPlayEntity::Translate( vMove );
}

void ioMachineStruct::MapCollisionPushingTranslate( const D3DXVECTOR3 &vMoveAmt )
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

void ioMachineStruct::DontMoveCollisionPushingTranslate( ioGameEntity *pPusher, const D3DXVECTOR3 &vMoveAmt )
{
	if( !IsMapCollisionPushed() )
	{
		ioPlayEntity::DontMoveCollisionPushingTranslate( pPusher, vMoveAmt );
	}
}

void ioMachineStruct::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

void ioMachineStruct::SetHP( float fCurHP, float fMaxHP )
{
	m_HP.m_fCurValue = fCurHP;
	m_HP.m_fMaxValue = fMaxHP;
}

void ioMachineStruct::SetTakeChar( ioHashString szTakeCharName )
{
	m_TakeCharName = szTakeCharName;
}

void ioMachineStruct::SetTakeState( const ioHashString &szTakeCharName )
{
	m_TakeCharName = szTakeCharName;
}

void ioMachineStruct::ReleaseState()
{
	m_TakeCharName.Clear();
}

void ioMachineStruct::ProcessState( float fTimePerSec )
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;
}

void ioMachineStruct::InitMachineStructInfo( int iStructNum, int iArrayNum, DWORD dwCreateGapTime, DWORD dwSeed,
											 const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale )
{
	m_iStructNum = iStructNum;
	m_iArrayIndex = iArrayNum;
	m_bEnableTakeEffect = false;

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
}

void ioMachineStruct::OnMachineStructDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker )
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
}

void ioMachineStruct::StructDie( bool bEffect )
{
	m_bLive = false;
	PlaySound( m_DestroySound );

	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	pOwner->SetMachineRelease( this, vPos );
}

bool ioMachineStruct::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return false;
}

void ioMachineStruct::SendMachineStructInfo()
{
	SP2Packet kPacket( CUPK_PUSHSTRUCT_INFO );
	kPacket << GetMachineArrayIdx();
	kPacket << m_HP.m_fMaxValue;
	kPacket << m_HP.m_fCurValue;
	kPacket << GetWorldPosition();
	P2PNetwork::SendToAllPlayingUser( kPacket );
	if( m_pModeParent && m_pModeParent->GetCreator() )
		m_pModeParent->GetCreator()->NotifyPushStructSendInfo();
}

void ioMachineStruct::OnMachineStructInfo( SP2Packet &rkPacket )
{
	D3DXVECTOR3 vPos;
	rkPacket >> m_HP.m_fMaxValue;
	rkPacket >> m_HP.m_fCurValue;
	rkPacket >> vPos;

	vPos.y = m_pModeParent->GetMapHeight( vPos.x, vPos.z, this, false );
	SetWorldPosition( vPos );
}

bool ioMachineStruct::IsCanSendNetwork()
{
	bool bSend = false;

	if( !m_TakeCharName.IsEmpty() )
	{
		if( m_pModeParent->CheckCharExist( m_TakeCharName ) && m_pModeParent->GetOwnerName() == m_TakeCharName )
			bSend = true;
		else if( !m_pModeParent->CheckCharExist( m_TakeCharName ) && P2PNetwork::IsHost() )
			bSend = true;
	}
	else if( m_TakeCharName.IsEmpty() && P2PNetwork::IsHost() )
	{
		bSend = true;
	}

	return bSend;
}

void ioMachineStruct::SendDieMessageToServer( bool bEffect )
{
	if( m_bSendedDieMessage )
		return;

	/*
	SP2Packet kPacket( CTPK_MACHINESTRUCT );
	kPacket << MACHINE_DIE;
	kPacket << GetMachineArrayIdx();
	kPacket << bEffect;
	TCPNetwork::SendToServer( kPacket );
	*/

	m_bSendedDieMessage = true;
}

void ioMachineStruct::SetStructDie( ioBaseChar *pChar, bool bEffect )
{
	bool bSend = false;
	if( !m_TakeCharName.IsEmpty() )
	{
		if( pChar->IsNeedSendNetwork() )
			bSend = true;
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		if( bSend )
		{
			SendDieMessageToServer( bEffect );
		}
	}
	else
	{
		StructDie();
	}
}

void ioMachineStruct::TranslateByWorldEntity( const D3DXVECTOR3 &vMove )
{
	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();
}

void ioMachineStruct::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
	D3DXVECTOR3 vPos;
	const ioOrientBox &rkBox = GetWorldCollisionBox();

	m_pGroup->GetParentNode()->Translate( vMove );
	NeedUpdateBound();
	NeedUpdateBottomHeight();
}

void ioMachineStruct::ApplyOutLineChange( ChangeOutLineType eType, const D3DCOLORVALUE &rkColor, float fSize )
{
	RemoveChangeOutLineRecord( eType );

	OutLineRecord kRecord;
	kRecord.m_Type   = eType;
	kRecord.m_Color  = rkColor;
	kRecord.m_fThick = fSize;
	m_OutLineChangeList.push_front( kRecord );

	ChangeOutLine( rkColor, fSize );
}

void ioMachineStruct::RestoreOutLineChange( ChangeOutLineType eType )
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

bool ioMachineStruct::HasOutLineType( ChangeOutLineType eType )
{
	OutLineRecordList::iterator iter;
	for( iter=m_OutLineChangeList.begin() ; iter!=m_OutLineChangeList.end() ; ++iter )
	{
		if( iter->m_Type == eType )
			return true;
	}

	return false;
}

void ioMachineStruct::RemoveChangeOutLineRecord( ChangeOutLineType eType )
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

void ioMachineStruct::ChangeOutLine( const D3DCOLORVALUE &rkOutLineColor, float fOutLineSize )
{
	m_OutLineColor = rkOutLineColor;
	m_fOutLineSize = fOutLineSize;
	m_bOutLineModified = true;

	ApplyModifiedOutLine();
}

void ioMachineStruct::RestoreOutLine()
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

void ioMachineStruct::ApplyModifiedOutLine()
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

bool ioMachineStruct::IsTakeEnableState()
{
	if( !m_TakeCharName.IsEmpty() )
		return false;

	return true;
}

void ioMachineStruct::ApplyMachineStructExtraInfo( SP2Packet &rkPacket )
{
}

void ioMachineStruct::CheckSpecialCollisionBox( const ioHashString &szFileName )
{
	if( szFileName.IsEmpty() ) return;

	SAFEDELETE( m_pSpecialBox );

	m_pSpecialBox = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szFileName.c_str(), true );
}

ioOrientBox ioMachineStruct::GetAttackCollisionBox( AniCollisionType eType, bool bNoScale )
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

void ioMachineStruct::SetEnableTakeEffect( bool bSet )
{
	if( m_EnableTakeEffect.IsEmpty() )
		return;

	if( !m_bEnableTakeEffect && bSet )
	{
		m_bEnableTakeEffect = true;

		if( m_pGroup )
			m_pGroup->AttachEffect( m_EnableTakeEffect, NULL );
	}
	else if( !bSet )
	{
		m_bEnableTakeEffect = false;
		
		if( m_pGroup )
			m_pGroup->EndEffect( m_EnableTakeEffect, false );
	}
}

void ioMachineStruct::SetExtraInfoToWeapon( ioWeapon *pWeapon )
{
	CheckChargeInfo( pWeapon );
}

void ioMachineStruct::CheckChargeInfo( ioWeapon *pWeapon )
{
	if( !pWeapon ) return;

	float fCurDamageRate, fCurForceRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	fCurDamageRate = fCurForceRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );
		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );
	}
}

