#include "StdAfx.h"
#include "ioRSoldierInfoManager.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

template<> ioRSoldierInfoManager* Singleton< ioRSoldierInfoManager >::ms_Singleton = 0;
ioRSoldierInfoManager& ioRSoldierInfoManager::GetSingleton()
{
	return Singleton< ioRSoldierInfoManager >::GetSingleton();
}

ioRSoldierInfoManager::ioRSoldierInfoManager(void)
{
}

ioRSoldierInfoManager::~ioRSoldierInfoManager(void)
{
}

void ioRSoldierInfoManager::Initialize()
{
	LoadRSoldier();
	LoadRandomApplyBuff();
	LoadSoldierSet();
	LoadSoldierSound();
	LoadSoldierDefaultDeco();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRSoldierInfoManager::LoadRSoldier()
{
	char szBuff[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_r_char_state.ini" );
	kLoader.SetTitle_e( "common" );

	int iMaxCount = kLoader.LoadInt_e( "max_r_char_state_count", 0 );

	for ( int i = 0; i<iMaxCount ; ++i )
	{
		IntVec vRSoldierState;

		for( int j = 0; j < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH; ++j )
		{
			sprintf_s_e( szBuff, "r_char%d_growth_%d", i+1, j+1 );
			vRSoldierState.push_back( kLoader.LoadInt( szBuff, 0 ) );
		}

		m_vRSoldierAddStateList.push_back( vRSoldierState );
	}
}

int ioRSoldierInfoManager::GetSoldierAddStat( int iIndex, int iCurRSoldierCount )
{
	if ( m_vRSoldierAddStateList.empty() )
		return 0;
	if ( iCurRSoldierCount <= 0 )
		return 0;

	int iMaxArraySize = m_vRSoldierAddStateList.size() - 1;
	int iArray = iCurRSoldierCount - 1;
	if ( iArray > iMaxArraySize )
		iArray = iMaxArraySize;

	
	if ( iIndex > (int)m_vRSoldierAddStateList[iArray].size() )
		return 0;


	return m_vRSoldierAddStateList[iArray][iIndex];
}

bool ioRSoldierInfoManager::IsRSoldierType( const CHARACTER& rkChar )
{
	if( rkChar.m_eCharPowerUpType != PUGT_NONE )
		return false;

	if ( rkChar.m_chExerciseStyle != EXERCISE_NONE )
		return false;

	if ( rkChar.m_ePeriodType != CPT_MORTMAIN )
		return false;

	if ( !COMPARE( rkChar.m_class_type, DEFAULT_R_CHAR_CODE, DEFAULT_SET_CHAR_CODE + 1 ) )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRSoldierInfoManager::LoadRandomApplyBuff()
{
	char szBuff[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_random_apply_buff.ini" );

	kLoader.SetTitle_e( "common" );

	m_RandomApplyBuffList.clear();

	int nMaxCnt = kLoader.LoadInt_e( "max_cnt", 0 );
	for ( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szBuff, "mercenary%d", i+1 );
		kLoader.SetTitle( szBuff );

		int nCode = kLoader.LoadInt_e( "code", 0 );

		RandomApplyBuff sRandomApplyBuff;
		sRandomApplyBuff.m_nApplyChance = kLoader.LoadInt_e( "apply_chance", 0 );
		sRandomApplyBuff.m_nMaxChance = kLoader.LoadInt_e( "max_chance", 0 );

		LoadOwnerBuff( kLoader, "attack", sRandomApplyBuff.m_AttackOwnerBuff );
		LoadOwnerBuff( kLoader, "wound", sRandomApplyBuff.m_WoundOwnerBuff );

		m_RandomApplyBuffList.insert( std::make_pair( nCode, sRandomApplyBuff ) );
	}

	kLoader.SetTitle_e( "disable_mode_info" );
	int nDisableModeCnt = kLoader.LoadInt_e( "disable_mode_cnt", 0 );
	for( int i=0; i<nDisableModeCnt; ++i )
	{
		wsprintf_e( szBuff, "disable_mode%d", i+1 );
		int nDisableModeType = kLoader.LoadInt( szBuff, 0 );
		if ( nDisableModeType > 0 )
			m_vRandomBuffDisbleModeType.push_back( nDisableModeType );
	}
}

void ioRSoldierInfoManager::LoadOwnerBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList )
{
	char szBuff[MAX_PATH], szKey[MAX_PATH];

	BuffList.clear();

	wsprintf_e( szKey, "%s_owner_buff_cnt", szName );
	int nMaxCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szKey, "%s_owner_buff%d", szName, i+1 );
		rkLoader.LoadString( szKey, "", szBuff, MAX_PATH );
		BuffList.push_back( szBuff );
	}
}

bool ioRSoldierInfoManager::CheckEnableBuff( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return false;

	for ( int i=0; i<(int)m_vRandomBuffDisbleModeType.size(); i++ )
	{
		if ( m_vRandomBuffDisbleModeType[i] == pOwner->GetCurModeType() )
			return false;
	}

	return true;
}

void ioRSoldierInfoManager::ApplyRandomBuff( ioBaseChar *pAttackChar, ioBaseChar *pWoundChar )
{
	if ( !pAttackChar || !pWoundChar )
		return;

	if( pWoundChar->IsProtectState() )
		return;

	if ( pAttackChar->GetCharName() == pWoundChar->GetCharName() )
		return;

	if (m_RandomApplyBuffList.empty())
		return;

	if( !CheckEnableBuff( pAttackChar ) )
		return;

	int nClassType = pAttackChar->GetCharacterInfo().m_class_type;
	if (!P2PNetwork::IsNetworkPlaying())
	{
		ioWeaponItem *pWeaponItem = pAttackChar->GetWeapon();
		if ( pWeaponItem )
			nClassType = pWeaponItem->GetItemCode();
	}

	RandomApplyBuffList::iterator Find_Iter = m_RandomApplyBuffList.find( nClassType );
	if ( Find_Iter == m_RandomApplyBuffList.end() )
		return;

	RandomApplyBuff sRandomApplyBuff = Find_Iter->second;

	IORandom random;
	random.Randomize();

	int nChance = (int)random.Random(sRandomApplyBuff.m_nMaxChance);
	if ( nChance <= sRandomApplyBuff.m_nApplyChance )
	{
		if ( pAttackChar->GetState() != CS_DIE )
		{
			for (int i=0; i<(int)sRandomApplyBuff.m_AttackOwnerBuff.size(); i++)
				pAttackChar->AddNewBuff( sRandomApplyBuff.m_AttackOwnerBuff[i], pAttackChar->GetCharName(), "", NULL );
		}

		if ( pWoundChar->GetState() != CS_DIE )
		{
			for (int i=0; i<(int)sRandomApplyBuff.m_WoundOwnerBuff.size(); i++)
				pWoundChar->AddNewBuff( sRandomApplyBuff.m_WoundOwnerBuff[i], pAttackChar->GetCharName(), "", NULL );
		}

		if ( pWoundChar->IsNeedProcess() && pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_RANDOM_APPLY_BUFF );
			kPacket << nClassType;
			kPacket << pAttackChar->GetCharName();
			kPacket << pWoundChar->GetCharName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRSoldierInfoManager::OnRandomBuff( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	if ( !pStage )
		return;

	int nClassType;
	rkPacket >> nClassType;
	
	ioHashString szAttackChar;
	rkPacket >> szAttackChar;
	
	ioHashString szWoundChar;
	rkPacket >> szWoundChar;

	ioBaseChar *pAttackChar = pStage->GetBaseChar( szAttackChar );
	if ( !pAttackChar )
		return;

	ioBaseChar *pWoundChar = pStage->GetBaseChar( szWoundChar );
	if ( !pWoundChar )
		return;

	RandomApplyBuffList::iterator Find_Iter = m_RandomApplyBuffList.find( nClassType );
	if ( Find_Iter == m_RandomApplyBuffList.end() )
		return;

	RandomApplyBuff sRandomApplyBuff = Find_Iter->second;

	if ( pAttackChar->GetState() != CS_DIE )
	{
		for (int i=0; i<(int)sRandomApplyBuff.m_AttackOwnerBuff.size(); i++)
			pAttackChar->AddNewBuff( sRandomApplyBuff.m_AttackOwnerBuff[i], pAttackChar->GetCharName(), "", NULL );
	}

	if ( pWoundChar->GetState() != CS_DIE )
	{
		for (int i=0; i<(int)sRandomApplyBuff.m_WoundOwnerBuff.size(); i++)
			pWoundChar->AddNewBuff( sRandomApplyBuff.m_WoundOwnerBuff[i], pAttackChar->GetCharName(), "", NULL );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRSoldierInfoManager::LoadSoldierSet()
{
	char szBuff[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_soldier_set.ini" );

	kLoader.SetTitle_e( "common" );

	m_SoldierSetNumList.clear();
	int nSoldierSetMaxCnt = kLoader.LoadInt_e( "soldier_max_cnt", 0 );
	for ( int i=0; i<nSoldierSetMaxCnt; i++ )
	{
		wsprintf_e( szBuff, "soldier%d_code", i+1 );
		int nCode = kLoader.LoadInt( szBuff, 0 );

		wsprintf_e( szBuff, "soldier%d_set", i+1 );
		int nSetNum = kLoader.LoadInt( szBuff, 0 );

		m_SoldierSetNumList.insert( std::make_pair( nCode, nSetNum ) );
	}

	m_SoldierSetDataList.clear();

	m_nSoldierSetDataMaxCnt = kLoader.LoadInt_e( "soldier_set_max_cnt", 0 );
	for ( int i=0; i<m_nSoldierSetDataMaxCnt; i++ )
	{
		wsprintf_e( szBuff, "soldier_set%d", i+1 );
		kLoader.SetTitle( szBuff );

		SoldierSetBuffList vSoldierSetBuffList;

		vSoldierSetBuffList.clear();
		int nMaxCnt = kLoader.LoadInt_e( "set_buff_max_cnt", 0 );
		for ( int j=0; j<nMaxCnt; j++ )
		{
			SoldierSetBuff sSoldierSetBuff;

			wsprintf_e( szBuff, "set_buff%d_cnt", j+1 );
			sSoldierSetBuff.m_nSoldierSetCnt = kLoader.LoadInt( szBuff, 0 );

			for ( int k=0; k<8; k++ )
			{
				wsprintf_e( szBuff, "set_buff%d_add_stat%d", j+1, k+1 );
				int nAddStat = kLoader.LoadInt( szBuff, 0 );
				sSoldierSetBuff.m_nAddStat.push_back( nAddStat );
			}

			vSoldierSetBuffList.push_back( sSoldierSetBuff );
		}

		m_SoldierSetDataList.insert( std::make_pair( i+1, vSoldierSetBuffList ) );
	}
}

int ioRSoldierInfoManager::GetSoldierSetNum( int nCode )
{
	if ( nCode <= 0 && m_SoldierSetNumList.empty() )
		return 0;

	std::map<int, int>::iterator Find_Iter = m_SoldierSetNumList.find( nCode );
	if (Find_Iter == m_SoldierSetNumList.end() )
		return 0;

	return Find_Iter->second;
}

bool ioRSoldierInfoManager::GetSoldierSetData( int nCode, SoldierSetBuffList& vSoldierSetBuffList )
{
	int nSoldierSetNum = GetSoldierSetNum( nCode );
	if ( nSoldierSetNum <= 0 )
		return false;

	SoldierSetDataList::iterator Find_Iter = m_SoldierSetDataList.find( nSoldierSetNum );
	if ( Find_Iter == m_SoldierSetDataList.end() )
		return false;

	vSoldierSetBuffList = Find_Iter->second;
	return true;		
}

int ioRSoldierInfoManager::GetSoldierSetAddStat( int nIndex, int nCode, int nCnt )
{
	SoldierSetBuffList vSoldierSetBuffList;
	if ( !GetSoldierSetData( nCode, vSoldierSetBuffList ) )
		return 0;

	int nAddStat = 0;
	for ( int i=0; i<(int)vSoldierSetBuffList.size(); i++ )
	{
		if ( vSoldierSetBuffList[i].m_nSoldierSetCnt <= nCnt )
			nAddStat += vSoldierSetBuffList[i].m_nAddStat[nIndex];
	}

	return nAddStat;
}

bool ioRSoldierInfoManager::IsSoldierSet( const CHARACTER& rkChar )
{
	if( rkChar.m_eCharPowerUpType != PUGT_NONE )
		return false;

	if ( rkChar.m_chExerciseStyle != EXERCISE_NONE )
		return false;

	if ( rkChar.m_ePeriodType != CPT_MORTMAIN )
		return false;

	if ( rkChar.m_class_type < DEFAULT_SET_CHAR_CODE )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRSoldierInfoManager::LoadSoldierSound()
{
	char szBuff[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_soldier_sound.ini" );

	kLoader.SetTitle_e( "common" );

	m_SoldierSoundDataList.clear();

	int nMaxCnt = kLoader.LoadInt_e( "max_cnt", 0 );
	for ( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szBuff, "sound%d", i+1 );
		kLoader.SetTitle( szBuff );

		int nCode = kLoader.LoadInt_e( "mercenary_code", 0 );

		SoldierSoundData sSoldierSoundData;
		LoadSound( kLoader, "start", sSoldierSoundData.m_StartSound );
		LoadSound( kLoader, "weapon_skill", sSoldierSoundData.m_WeaponSkillSound );
		LoadSound( kLoader, "armor_skill", sSoldierSoundData.m_ArmorSkillSound );
		LoadSound( kLoader, "helmet_skill", sSoldierSoundData.m_HelmetSkillSound );
		LoadSound( kLoader, "cloak_skill", sSoldierSoundData.m_CloakSkillSound );
		LoadSound( kLoader, "kill", sSoldierSoundData.m_KillSound );
		LoadSound( kLoader, "die", sSoldierSoundData.m_DieSound );
		m_SoldierSoundDataList.insert( std::make_pair( nCode, sSoldierSoundData ) );
	}
}

void ioRSoldierInfoManager::LoadSound( ioINILoader &rkLoader, const char* szName, ioHashStringVec &SoundList )
{
	char szBuff[MAX_PATH], szKey[MAX_PATH];

	SoundList.clear();

	wsprintf_e( szKey, "%s_max_cnt", szName );
	int nMaxCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szKey, "%s%d", szName, i+1 );
		rkLoader.LoadString( szKey, "", szBuff, MAX_PATH );
		SoundList.push_back( szBuff );
	}
}

void ioRSoldierInfoManager::PlaySoldierSound( ioBaseChar *pChar, SoldierSoundType eType )
{
	if ( m_SoldierSoundDataList.empty() || !pChar || !pChar->IsNeedProcess() )
		return;

	int nClassType = pChar->GetCharacterInfo().m_class_type;
	if (!P2PNetwork::IsNetworkPlaying())
	{
		ioWeaponItem *pWeaponItem = pChar->GetWeapon();
		if ( pWeaponItem )
			nClassType = pWeaponItem->GetItemCode();
	}

	if ( nClassType <= 0 )
		return;

	SoldierSoundDataList::iterator Find_Iter = m_SoldierSoundDataList.find( nClassType );
	if ( Find_Iter == m_SoldierSoundDataList.end() )
		return;

	ioHashString szPlaySound;

	const SoldierSoundData &sSoldierSoundData = Find_Iter->second;

	switch( eType )
	{
	case SST_Start:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_StartSound );
		break;
	case SST_WeaponSkill:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_WeaponSkillSound );
		break;
	case SST_ArmorSkill:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_ArmorSkillSound );
		break;
	case SST_HelmetSkill:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_HelmetSkillSound );
		break;
	case SST_CloakSkill:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_CloakSkillSound );
		break;
	case SST_Kill:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_KillSound );
		break;
	case SST_Die:
		szPlaySound = GetRandomSound( sSoldierSoundData.m_DieSound );
		break;
	}

	if ( szPlaySound.IsEmpty() )
		return;
	
	//사운드 재생 옵션이 꺼져있으면 플레이 하지 않음
	if( !Setting::IsPlayExtendSound() )
		return;

	g_SoundMgr.StopSound( m_szCurPlaySound, 0 );
	
	m_szCurPlaySound = szPlaySound;

	g_SoundMgr.PlaySound( m_szCurPlaySound );
}

ioHashString ioRSoldierInfoManager::GetRandomSound( const ioHashStringVec &SoundList )
{
	static ioHashString szNull;
	if ( SoundList.empty() )
		return szNull;

	IORandom random;
	random.Randomize();

	int nIdx = (int)random.Random( SoundList.size() );
	return SoundList[nIdx];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioRSoldierInfoManager::LoadSoldierDefaultDeco()
{
	char szBuff[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_soldier_default_deco.ini" );

	kLoader.SetTitle_e( "common" );

	m_SoldierDefaultDecoList.clear();

	int nMaxCnt = kLoader.LoadInt_e( "max_cnt", 0 );
	for ( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szBuff, "deco%d", i+1 );
		kLoader.SetTitle( szBuff );

		int nCode = kLoader.LoadInt_e( "mercenary_code", 0 );

		SoldierDefaultDeco sSoldierDefaultDeco;
		sSoldierDefaultDeco.m_nSex = kLoader.LoadInt( "sex", 0 );
		sSoldierDefaultDeco.m_nFace = kLoader.LoadInt_e( "face", 0 );
		sSoldierDefaultDeco.m_nHair = kLoader.LoadInt_e( "hair", 0 );
		sSoldierDefaultDeco.m_nSkinColor = kLoader.LoadInt_e( "skin_color", 0 );
		sSoldierDefaultDeco.m_nHairColor = kLoader.LoadInt_e( "hair_color", 0 );
		sSoldierDefaultDeco.m_nUnderwear = kLoader.LoadInt_e( "under_wear", 0 );

		m_SoldierDefaultDecoList.insert( std::make_pair( nCode, sSoldierDefaultDeco ) );
	}
}

bool ioRSoldierInfoManager::GetSoldierDefaultDeco( CHARACTER &rkCharInfo )
{
	if ( rkCharInfo.m_class_type <= 0 || m_SoldierDefaultDecoList.empty() )
		return false;

	if( !g_MyInfo.GetCharIndexByClassType( rkCharInfo.m_class_type ) == 0 )
		return false;

	SoldierDefaultDecoList::iterator Find_Iter = m_SoldierDefaultDecoList.find( rkCharInfo.m_class_type );
	if ( Find_Iter == m_SoldierDefaultDecoList.end() )
		return false;

	SoldierDefaultDeco sSoldierDefaultDeco = Find_Iter->second;
	rkCharInfo.m_sex = sSoldierDefaultDeco.m_nSex;
	rkCharInfo.m_face = sSoldierDefaultDeco.m_nFace;
	rkCharInfo.m_hair = sSoldierDefaultDeco.m_nHair;
	rkCharInfo.m_skin_color = sSoldierDefaultDeco.m_nSkinColor;
	rkCharInfo.m_hair_color = sSoldierDefaultDeco.m_nHairColor;
	rkCharInfo.m_underwear = sSoldierDefaultDeco.m_nUnderwear;
	return true;
}