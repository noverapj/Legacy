

#include "stdafx.h"
#include "ioItem.h"

#include "ioStatAttribute.h"
#include "ioGrowthLevel.h"
#include "ioGrowthUpInfo.h"
#include "ioExceptGrowthBuff.h"

ioItem::ioItem()
{
	m_pCreator = NULL;
	m_iItemGroupIndex = 0;
	m_dwItemCode = 0;

	m_iItemCreateIndex = 0;
	m_dwItemFeature = 0;

	m_iReinforce = 0;
	m_dwMaleCustom = 0;
	m_dwFemaleCustom = 0;
	
	m_iItemCreateMaxLimit = ITEM_CREATE_NO_LIMIT;
	
	m_dwSetItemCode = 0;

	m_fDefaultGauge = 0.0f;
	m_fCurSkillGauge = 0.0f;
	m_dwTicCheckTime = 0;

	m_bUpdateEnableSkillGauge = false;
	m_bVisible = true;
	m_bAimedBall = false;

	m_dwChangeSkillEffect = -1;

	m_ReserveChangeSkillInfo.clear();

	m_CostumeInfo.Init();
	
	m_dwCostumeMaleCustom = 0;
	m_dwCostumeFemaleCustom = 0;
	m_iAccessoryRandomNum = 0;
}

ioItem::ioItem( const ioItem &rhs )
: m_pCreator( NULL ),
  m_iItemGroupIndex( rhs.m_iItemGroupIndex ),
  m_StatIndex( rhs.m_StatIndex ),
  m_dwItemCode( rhs.m_dwItemCode ),
  m_dwItemFeature( rhs.m_dwItemFeature ),
  m_iItemCreateMaxLimit( rhs.m_iItemCreateMaxLimit ),
  m_Name( rhs.m_Name ),
  m_ItemEquipEffect( rhs.m_ItemEquipEffect ),
  m_ItemEquipEffectHw( rhs.m_ItemEquipEffectHw ),
  m_ExtendEquipEffect( rhs.m_ExtendEquipEffect ),
  m_ExtendEquipEffectHw( rhs.m_ExtendEquipEffectHw ),
  m_FieldEntityGrp( rhs.m_FieldEntityGrp ),
  m_SkillName( rhs.m_SkillName ),
  m_FieldEffect( rhs.m_FieldEffect ),
  m_PickEnableEffect( rhs.m_PickEnableEffect ),
  m_DropEffect( rhs.m_DropEffect ),
  m_ProtectionEffect( rhs.m_ProtectionEffect ),
  m_OwnerName( rhs.m_OwnerName ),
  m_ItemLargeIconFile( rhs.m_ItemLargeIconFile ),
  m_CrownType( rhs.m_CrownType ),
  m_CrownTeamType( rhs.m_CrownTeamType ),
  m_bHasSkillGauge( rhs.m_bHasSkillGauge ),
  m_fMaxSkillGauge( rhs.m_fMaxSkillGauge ),
  m_fArmorClass( rhs.m_fArmorClass ),
  m_fSpeedClass( rhs.m_fSpeedClass ),
  m_fMagicCircleOffSet( rhs.m_fMagicCircleOffSet ),
  m_fMagicCircleYOffSet( rhs.m_fMagicCircleYOffSet ),
  m_DelayCameraBuff( rhs.m_DelayCameraBuff ),
  m_vGrowthInfoNum( rhs.m_vGrowthInfoNum ),
  m_iEnableClass( rhs.m_iEnableClass ),
  m_iManual( rhs.m_iManual ),
  m_SkillNameMap( rhs.m_SkillNameMap ),
  m_fChangeSkillHPRate( rhs.m_fChangeSkillHPRate ),
  m_fChangeSkillExtendGauge( rhs.m_fChangeSkillExtendGauge ),
  m_ChangeSkillBuff( rhs.m_ChangeSkillBuff ),
  m_ChangeSkillNameByBuff( rhs.m_ChangeSkillNameByBuff ),
  m_ChangeSkillEffect( rhs.m_ChangeSkillEffect ),
  m_iCharOnlySexType( rhs.m_iCharOnlySexType ),
  m_EquipBuffList( rhs.m_EquipBuffList ),
  m_SkillGaugeType( rhs.m_SkillGaugeType ),
  m_WeaponActionType( rhs.m_WeaponActionType ),
  m_iWeaponActionMaxCnt( rhs.m_iWeaponActionMaxCnt ),
  m_GroupName( rhs.m_GroupName ),
  m_TitleName( rhs.m_TitleName ),
  m_fStartSkillGaugeRate( rhs.m_fStartSkillGaugeRate ),
  m_nGradeType( rhs.m_nGradeType )
{
	m_GlowColor = rhs.m_GlowColor;

	m_fCamDistance = rhs.m_fCamDistance;
	m_fCamHeight = rhs.m_fCamHeight;
	m_fCamFov = rhs.m_fCamFov;
	m_dwCamZoomTime = rhs.m_dwCamZoomTime;

	m_fSkillCamDistance = rhs.m_fSkillCamDistance;
	m_fSkillCamHeight = rhs.m_fSkillCamHeight;
	m_fSkillCamFov = rhs.m_fSkillCamFov;
	m_dwSkillCamZoomTime = rhs.m_dwSkillCamZoomTime;

	m_fRunCamDistance = rhs.m_fRunCamDistance;
	m_fRunCamHeight = rhs.m_fRunCamHeight;
	m_fRunCamFov = rhs.m_fRunCamFov;
	m_dwRunCamZoomTime = rhs.m_dwRunCamZoomTime;

	m_fChargingCamDistance = rhs.m_fChargingCamDistance;
	m_fChargingCamHeight = rhs.m_fChargingCamHeight;
	m_fChargingCamFov = rhs.m_fChargingCamFov;
	m_dwChargingCamZoomTime = rhs.m_dwChargingCamZoomTime;

	m_dwSetItemCode = rhs.m_dwSetItemCode;

	m_iReinforce = 0;
	m_dwMaleCustom = 0;
	m_dwFemaleCustom = 0;
	m_dwTicCheckTime = 0;
	m_fCurSkillGauge = 0.0f;

	m_bUpdateEnableSkillGauge = false;
	m_bVisible = true;
	m_bAimedBall = false;

	m_dwChangeSkillEffect = -1;

	m_ReserveChangeSkillInfo.clear();
	// m_ItemDesc, m_FuncDesc, m_ExpertDesc ,m_ItemExpertList 는 복사하지 않음...용량도 크고 UI에서는 실제 아이템을 통해 접근한다.
	// 나중에 일이생기면 하겠지만 일단은 안함

	m_CostumeInfo.Init();
	m_dwCostumeMaleCustom = 0;
	m_dwCostumeFemaleCustom = 0;
	m_iAccessoryRandomNum = 0;
}

ioItem::~ioItem()
{
	if( m_pCreator )
	{
		m_pCreator->NotifyItemDestroyed( m_dwItemCode );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioItem::~ioItem - Create is NULL" );
	}
	m_FieldEntityGrpMap.clear();
	m_ItemLargeIconMap.clear();
	m_SkillNameMap.clear();
	m_ReserveChangeSkillInfo.clear();
	m_EquipBuffList.clear();
}

bool ioItem::HasFeature( DWORD dwFeature ) const
{
	if( m_dwItemFeature & dwFeature )
		return true;

	return false;
}

void ioItem::LoadProperty( ioINILoader &rkLoader )
{
	m_iItemGroupIndex = rkLoader.LoadInt_e( "group", 0 );
	m_StatIndex = (StatAttributeType)rkLoader.LoadInt_e( "stat_type", 0 );
	m_dwItemCode = rkLoader.LoadInt_e( "code", 0 );	
	m_iEnableClass = rkLoader.LoadInt_e( "enable_class", 0 );
	m_iManual = rkLoader.LoadInt_e( "manual", 0 );
	m_iCharOnlySexType = rkLoader.LoadInt_e( "char_sex_type", ioSetItemInfo::CST_ALL );

	m_CrownType		= (ModeCrownType)rkLoader.LoadInt_e( "crown_item_type", 0 );
	m_CrownTeamType = (TeamType)rkLoader.LoadInt_e( "item_team_type", 0 );

	char szBuf[MAX_PATH];
	char szTitle[MAX_PATH];
	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
	m_Name = szBuf;

	m_fStartSkillGaugeRate = rkLoader.LoadFloat_e( "start_skill_gauge_rate", FLOAT1 );

	rkLoader.LoadString_e( "item_equip_effect", "", szBuf, MAX_PATH );
	m_ItemEquipEffect = szBuf;

	rkLoader.LoadString_e( "item_equip_effect_hw", "", szBuf, MAX_PATH );
	m_ItemEquipEffectHw = szBuf;

	rkLoader.LoadString_e( "extend_equip_effect", "", szBuf, MAX_PATH );
	m_ExtendEquipEffect = szBuf;

	rkLoader.LoadString_e( "extend_equip_effect_hw", "", szBuf, MAX_PATH );
	m_ExtendEquipEffectHw = szBuf;

	int iDecoCnt = 0;
	m_FieldEntityGrpMap.clear();	
	m_ItemLargeIconMap.clear();
	while( true )
	{
		sprintf_e( szTitle, "field_mesh%d", iDecoCnt );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		m_FieldEntityGrpMap.insert( DecoInfoMap::value_type( iDecoCnt, szBuf ) );

		sprintf_e( szTitle, "item_large_icon%d", iDecoCnt );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );		
		m_ItemLargeIconMap.insert( DecoInfoMap::value_type( iDecoCnt, szBuf ) );

		iDecoCnt++;
	}	

	int iSkillCnt = 0;
	m_SkillNameMap.clear();
	while( true )
	{
		sprintf_e( szTitle, "skill%d", iSkillCnt );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		if( iSkillCnt > 0 && strcmp( szBuf, "" ) == 0 )
			break;

		m_SkillNameMap.insert( SkillInfoMap::value_type( iSkillCnt, szBuf ) );

		iSkillCnt++;
	}	

	rkLoader.LoadString_e( "field_effect", "", szBuf, MAX_PATH );
	m_FieldEffect = szBuf;

	rkLoader.LoadString_e( "pick_enable_effect", "", szBuf, MAX_PATH );
	m_PickEnableEffect = szBuf;

	rkLoader.LoadString_e( "drop_effect", "", szBuf, MAX_PATH );
	m_DropEffect = szBuf;

	rkLoader.LoadString_e( "protection_effect", "", szBuf, MAX_PATH );
	m_ProtectionEffect = szBuf;
	
	m_iItemCreateMaxLimit = rkLoader.LoadInt_e( "item_create_limit_count", ITEM_CREATE_NO_LIMIT );

	// Armor & Speed Class
	m_fArmorClass = rkLoader.LoadFloat_e( "armor_class", 0.0f );
	m_fSpeedClass = rkLoader.LoadFloat_e( "speed_class", 0.0f );

	m_fMagicCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	m_fMagicCircleYOffSet = rkLoader.LoadFloat_e( "magic_circle_y_offset", 0.0f );

	// Skill Gauge
	m_SkillGaugeType = (GaugeType)rkLoader.LoadInt_e( "skill_gauge_type", GT_NONE );

	m_bHasSkillGauge = rkLoader.LoadBool_e( "has_skill_gauge", true );
	m_fMaxSkillGauge = rkLoader.LoadFloat_e( "max_skill_gauge", FLOAT100 );
	m_fMaxSkillGauge = min( m_fMaxSkillGauge, FLOAT100 );

	m_GlowColor.r = rkLoader.LoadFloat_e( "skill_color_red", 0.0f ) / 255.0f;
	m_GlowColor.g = rkLoader.LoadFloat_e( "skill_color_green", 0.0f ) / 255.0f;
	m_GlowColor.b = rkLoader.LoadFloat_e( "skill_color_blue", 0.0f ) / 255.0f;
	m_GlowColor.a = rkLoader.LoadFloat_e( "skill_color_alpha", 255.0f ) / 255.0f;

	// 카메라설정
	rkLoader.LoadString_e( "delay_camera_buff", "", szBuf, MAX_PATH );
	m_DelayCameraBuff = szBuf;

	m_fCamHeight = rkLoader.LoadFloat_e( "item_cam_height", 450.0f );
	m_fCamDistance = rkLoader.LoadFloat_e( "item_cam_distance", FLOAT1000 );
	m_fCamFov = rkLoader.LoadFloat_e( "item_cam_fov", 35.0f );
	m_dwCamZoomTime = rkLoader.LoadInt_e( "item_cam_zoom_time", 1000 );

	m_fSkillCamHeight = rkLoader.LoadFloat_e( "skill_cam_height", 540.0f );
	m_fSkillCamDistance = rkLoader.LoadFloat_e( "skill_cam_distance", 1200.0f );
	m_fSkillCamFov = rkLoader.LoadFloat_e( "skill_cam_fov", 35.0f );
	m_dwSkillCamZoomTime = rkLoader.LoadInt_e( "skill_cam_zoom_time", 2500 );

	m_fRunCamHeight = rkLoader.LoadFloat_e( "run_cam_height", 450.0f );
	m_fRunCamDistance = rkLoader.LoadFloat_e( "run_cam_distance", 1400.0f );
	m_fRunCamFov = rkLoader.LoadFloat_e( "run_cam_fov", 35.0f );
	m_dwRunCamZoomTime = rkLoader.LoadInt_e( "run_cam_zoom_time", 1000 );

	m_fChargingCamHeight = rkLoader.LoadFloat_e( "charging_cam_height", 400.0f );
	m_fChargingCamDistance = rkLoader.LoadFloat_e( "charging_cam_distance", 1200.0f );
	m_fChargingCamFov = rkLoader.LoadFloat_e( "charging_cam_fov", 35.0f );
	m_dwChargingCamZoomTime = rkLoader.LoadInt_e( "charging_cam_zoom_time", 1000 );
	//

	m_dwSetItemCode = rkLoader.LoadInt_e( "set_item_code", 0 );

	char szBigBuf[MAX_PATH * 2];
	rkLoader.LoadString_e( "item_desc", "", szBigBuf, MAX_PATH*2 );
	m_ItemDesc = szBigBuf;

	rkLoader.LoadString_e( "func_desc", "", szBigBuf, MAX_PATH*2 );
	m_FuncDesc = szBigBuf;

	// GrowthInfo Num
	m_vGrowthInfoNum.clear();
	m_vGrowthInfoNum.reserve( 1 );

	wsprintf_e( szBuf, "growth_info_num%d", 1 );
	int iGrowthInfoNum = rkLoader.LoadInt( szBuf, 0 );

	m_vGrowthInfoNum.push_back( iGrowthInfoNum );

	//
	m_fChangeSkillHPRate = rkLoader.LoadFloat_e( "change_skill_hp_rate", -FLOAT1 );
	m_fChangeSkillExtendGauge = rkLoader.LoadFloat_e( "change_skill_extend_gauge", 0.0f );
	m_fChangeSkillExtendGauge = max( 0.0f, m_fChangeSkillExtendGauge );

	float fMaxGauge = m_fMaxSkillGauge + m_fChangeSkillExtendGauge;
	float fGap = fMaxGauge - FLOAT100;
	if( fGap > 0.0f )
		m_fChangeSkillExtendGauge -= fGap;

	rkLoader.LoadString_e( "change_skill_buff", "", szBuf, MAX_PATH );
	m_ChangeSkillBuff = szBuf;

	rkLoader.LoadString_e( "change_skill_name", "", szBuf, MAX_PATH );
	m_ChangeSkillNameByBuff = szBuf;

	rkLoader.LoadString_e( "change_skill_effect", "", szBuf, MAX_PATH );
	m_ChangeSkillEffect = szBuf;

	//
	m_EquipBuffList.clear();
	int iEquipBuffCnt = rkLoader.LoadInt_e( "item_equip_buff_cnt", 0 );
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		wsprintf_e( szTitle, "item_equip_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_EquipBuffList.push_back( szBuf );
	}

	// weapon action
	m_WeaponActionType = (WeaponActionType)rkLoader.LoadInt_e( "weapon_action_type", WAT_NONE );
	m_iWeaponActionMaxCnt = rkLoader.LoadInt_e( "weapon_action_max_cnt", 0 );

	m_nGradeType = rkLoader.LoadInt_e( "grade_type", 0 );
}

void ioItem::SetGroupName( const ioHashString& szGroupName )
{
	m_GroupName = szGroupName;
}

void ioItem::SetTitleName( const ioHashString& szTitleName )
{
	m_TitleName = szTitleName;
}

void ioItem::ReLoadProperty()
{
	/*
	ioINILoader kSetItemLoader( m_GroupName.c_str() );
	kSetItemLoader.SetTitle( m_TitleName.c_str() );

	// 치트관련 1단계에서는 max_skill_gauge만...
	m_fMaxSkillGauge = kSetItemLoader.LoadFloat_e( "max_skill_gauge", FLOAT100 );
	
	// HARDCODE :: 치트관련 1단계에서는 미적용
	//LoadProperty( kSetItemLoader );
	*/
}

const ioHashString& ioItem::FindFieldEntityGrp() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_FieldEntityGrpMap.find( 0 );
	if( iter != m_FieldEntityGrpMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioItem::FindFieldEntityGrp(%d) : Unknown Default", (int)GetItemCode() );
	return m_FieldEntityGrp;
}

const ioHashString& ioItem::FindLargeIconName() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_ItemLargeIconMap.find( 0 );
	if( iter != m_ItemLargeIconMap.end() )
		return iter->second;			

	LOG.PrintTimeAndLog( 0, "ioItem::FindLargeIconName(%d) : Unknown Default", (int)GetItemCode() );
	return m_ItemLargeIconFile;
}

const ioHashString& ioItem::FindSkillName( int iIndex ) const
{
	// Default Return
	SkillInfoMap::const_iterator iter = m_SkillNameMap.find( iIndex );
	if( iter != m_SkillNameMap.end() )
		return iter->second;				

	LOG.PrintTimeAndLog( 0, "ioItem::FindSkillName : Unknown Default : %d", iIndex );
	return m_SkillName;
}

bool ioItem::HasSkillName( const ioHashString &szName )
{
	SkillInfoMap::const_iterator iter = m_SkillNameMap.begin();
	
	while( iter != m_SkillNameMap.end() )
	{
		if( iter->second == szName )
			return true;

		++iter;
	}

	return false;
}

void ioItem::SetItemCustom( DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	m_dwMaleCustom = dwMaleCustom;
	m_dwFemaleCustom = dwFemaleCustom;
}

void ioItem::SetItemType()
{
	m_FieldEntityGrp	= FindFieldEntityGrp();
	m_ItemLargeIconFile = FindLargeIconName();

	int iCurIndex = 0;
	m_SkillName = FindSkillName( iCurIndex );
}

void ioItem::SkillNameMapShuffle()
{
}

bool ioItem::UseCreateIndexItem() const
{
	return true;
}

DWORD ioItem::GetSetFocusColor() const
{
	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByItemCode( GetItemCode() );
	if( pInfo )
	{
		return pInfo->GetSetFocusColor();
	}

	return 0xff000000;
}

void ioItem::OnEquiped( ioBaseChar *pOwner )
{
	m_dwTicCheckTime = FRAMEGETTIME();

	bool bBaseEquipMesh = false;
	if( g_ItemMaker.IsBaseEquipMesh() && (GetType() == ES_WEAPON ||	GetType() == ES_OBJECT) )
		bBaseEquipMesh = true;

	if( pOwner && !bBaseEquipMesh )
	{
		SetCurEquipEffect( pOwner->IsMale() );

		if ( !m_CurItemEquipEffect.IsEmpty() )
			pOwner->AttachEffect( m_CurItemEquipEffect );
	}

	// Check changeskill
	CheckChangeSkillByBuff( pOwner );
	CheckChangeSkillByHP( pOwner );
	CheckChangeSkillByGauge( pOwner );

	//
	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	int iSlot = GetType() - 1;
	ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
	ioCountSkill *pCountSkill = ToCountSkill( pSkill );
	if( pCountSkill )
	{
		pCountSkill->SetEquip();
	}
}

void ioItem::OnReleased( ioBaseChar *pOwner )
{
	m_dwTicCheckTime = 0;

	if( pOwner )
		m_fDefaultGauge = pOwner->GetDefaultRecoveryGauge();

	if( pOwner && !m_CurItemEquipEffect.IsEmpty() )
		pOwner->EndEffect( m_CurItemEquipEffect, false );
	
	RemoveGlowLine( pOwner );

	// skillname init
	if( m_dwChangeSkillEffect != -1 )
	{
		pOwner->EndEffect( m_dwChangeSkillEffect, false );
		m_dwChangeSkillEffect = -1;
	}

	ioHashString szNewName = FindSkillName( 0 );
	if( m_SkillName != szNewName )
	{
		m_SkillName = szNewName;
	}

	//
	int iEquipBuffCnt = m_EquipBuffList.size();
	for( int i=0; i < iEquipBuffCnt; ++i )
	{
		ioHashString szBuffName = m_EquipBuffList[i];
		pOwner->RemoveBuff( szBuffName );
	}
}

void ioItem::SetExtendEquipEffect( ioBaseChar *pOwner )
{
	if( pOwner )
	{
		if( pOwner->IsMale() )
		{
			pOwner->AttachEffect( m_ExtendEquipEffect );
			m_CurExtendEquipEffect = m_ExtendEquipEffect;
		}
		else
		{
			pOwner->AttachEffect( m_ExtendEquipEffectHw );
			m_CurExtendEquipEffect = m_ExtendEquipEffectHw;
		}
	}
}

void ioItem::ReleaseExtendEquipEffect( ioBaseChar *pOwner )
{
	if( pOwner && !m_CurExtendEquipEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurExtendEquipEffect, false );
		m_CurExtendEquipEffect.Clear();
	}
}

void ioItem::SetVisible( ioBaseChar *pOwner, bool bVisible )
{
	m_bVisible = bVisible;

	bool bBaseEquipMesh = false;
	if( g_ItemMaker.IsBaseEquipMesh() && (GetType() == ES_WEAPON ||	GetType() == ES_OBJECT) )
		bBaseEquipMesh = true;

	if( pOwner && !bBaseEquipMesh )
	{
		if( !m_CurItemEquipEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_CurItemEquipEffect, false );
			m_CurItemEquipEffect.Clear();
		}

		if( m_bVisible )
		{
			SetCurEquipEffect( pOwner->IsMale() );

			if ( !m_CurItemEquipEffect.IsEmpty() )
				pOwner->AttachEffect( m_CurItemEquipEffect );
		}
	}
}

void ioItem::AddGlowLine( ioBaseChar *pOwner )
{
}

void ioItem::RemoveGlowLine( ioBaseChar *pOwner )
{
}

void ioItem::SetCreator( ioItemMaker *pCreator )
{
	m_pCreator = pCreator;
}

void ioItem::ClearIfOwnerName( const ioHashString &rkUser )
{
	if( HasOwnerName() )
	{
		if( m_OwnerName == rkUser )
			EraseOwnerName();
	}
}

void ioItem::EraseOwnerName()
{
	m_OwnerName.Clear();
}

ioHashString ioItem::GetTargetName()
{
	return m_AimedTarget;
}

void ioItem::ClearTarget()
{
	m_bAimedBall = false;
	m_AimedTarget.Clear();
}

void ioItem::UpdateRecoverySkillGauge( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_fDefaultGauge = pOwner->GetDefaultRecoveryGauge();

	switch( m_StatIndex )
	{
	case SAT_STR:
		m_fDefaultGauge = g_StatAttribute.GetCalcStatAttribute( m_fDefaultGauge, SA_STR_GAUGE_FILL_UP, pOwner );
		break;
	case SAT_DEX:
		m_fDefaultGauge = g_StatAttribute.GetCalcStatAttribute( m_fDefaultGauge, SA_DEX_GAUGE_FILL_UP, pOwner );
		break;
	case SAT_INT:
		m_fDefaultGauge = g_StatAttribute.GetCalcStatAttribute( m_fDefaultGauge, SA_INT_GAUGE_FILL_UP, pOwner );
		break;
	case SAT_VIT:
		m_fDefaultGauge = g_StatAttribute.GetCalcStatAttribute( m_fDefaultGauge, SA_VIT_GAUGE_FILL_UP, pOwner );
		break;
	}
}

bool ioItem::IsEnableUpdateSkillGauge( const ioHashString &szName )
{
	if( m_bUpdateEnableSkillGauge )
		return true;

	if( m_fCurSkillGauge >= GetMaxSkillGauge(szName) )
		return true;

	return false;
}

void ioItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	switch( m_SkillGaugeType )
	{
	case GT_NONE:
		UpdateNormalSkillGauge( pOwner, fRate, bUpdate );
		break;
	case GT_DAMAGE:
		{
			if( fRate > 0.0f )
				m_bUpdateEnableSkillGauge = bUpdate;
			else
				m_bUpdateEnableSkillGauge = false;
		}
		break;
	}
}

void ioItem::UpdateNormalSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
		pOwner->IsCanUpdateGauge() )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
		pOwner->GetState() == CS_VIEW ||
		pOwner->GetState() == CS_LOADING ||
		pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}

	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	m_fRecoverSkillGauge = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				bool bCheckBuff = false;
				if( pOwner->HasBuff( BT_ESCAPE_DROP_ZONE ) && pOwner->GetState() == CS_DROP_ZONE_DOWN )
					bCheckBuff = true;

				if( !bCheckBuff )
					m_fCurSkillGauge = min( m_fCurSkillGauge+m_fRecoverSkillGauge, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

//#ifndef SHIPPING 
//	if( Setting::ShowExtraInfo() )
//	{
//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
//
//		if( fMaxGauge < FLOAT100 )
//		{
//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//
//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//		}
//	}
//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

void ioItem::ReduceSkillGauge( float fUseGauge )
{
	m_fCurSkillGauge -= fUseGauge;
	m_fCurSkillGauge = max( m_fCurSkillGauge, 0.0f );
}

void ioItem::ReduceNeedGauge( float fUseGauge, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fSecoundGauge = 0.0f;
	float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
	float fExtendGauge = GetExtendSkillGauge( pOwner->GetCharName() );
	float fExtendMaxGauge = GetMaxExtendSkillGauge( pOwner->GetCharName() );

	if( fExtendMaxGauge > 0.0f && fExtendMaxGauge <= m_fCurSkillGauge )
		m_fCurSkillGauge = fExtendGauge;
	else
		m_fCurSkillGauge = fMaxGauge;

	m_fCurSkillGauge -= fUseGauge;
	m_fCurSkillGauge = max( m_fCurSkillGauge, 0.0f );
}

void ioItem::SetCurSkillGauge( float fCurGauge )
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && pStage->GetRoomCoolTimeRate() <= 0.0f )
	{
		fCurGauge = 0.0f;
	}

	m_fCurSkillGauge = max( min( fCurGauge, FLOAT100 ), 0.0f );
}

void ioItem::UpdateExtraData( ioBaseChar *pOwner )
{
}

void ioItem::CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner )
{
}

void ioItem::CheckWoundedState( ioBaseChar *pOwner )
{
}

int ioItem::GetCurBullet()
{
	return -1;
}

int ioItem::GetMaxBullet()
{
	return 0;
}

int ioItem::GetNeedBullet()
{
	return 0;
}

void ioItem::SetCurBullet( int iCurBullet )
{
}

int ioItem::GetCurSecoundBullet()
{
	return -1;
}

int ioItem::GetMaxSecoundBullet()
{
	return 0;
}

void ioItem::SetCurSecoundBullet( int iCurBullet )
{}

int ioItem::GetGrowthInfoNum( int iGrowthNum ) const
{
	int iNewNum = iGrowthNum - 1;

	if( COMPARE( iNewNum, 0, (int)m_vGrowthInfoNum.size() ) )
	{
		return m_vGrowthInfoNum[iNewNum];
	}

	return 0;
}

int ioItem::GetGrowthInfoNumByType( int iType, int &iGrowthNum ) const
{
	int iInfoNum = 0;
	int iSize = m_vGrowthInfoNum.size();

	for( int i=0; i < iSize; ++i )
	{
		iInfoNum = m_vGrowthInfoNum[i];
		if( g_GrowthInfoMgr.CheckGrowthInfoType( iInfoNum, iType ) )
		{
			iGrowthNum = i+1;
			return iInfoNum;
		}
	}

	return 0;
}

float ioItem::GetMaxSkillGauge( const ioHashString &szName )
{
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, szName) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fValue = FLOAT1 + (fValue/FLOAT100);

	CEncrypt<float> fMaxGauge = m_fMaxSkillGauge / fValue;
	fMaxGauge = max( 0.0f, fMaxGauge );
	
	return (float)fMaxGauge;
}

float ioItem::GetExtendSkillGauge( const ioHashString &szName )
{
	if( m_fChangeSkillExtendGauge <= 0.0f )
		return 0.0f;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, szName) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fValue = FLOAT1 + (fValue/FLOAT100);

	float fMaxGauge = m_fChangeSkillExtendGauge / fValue;
	fMaxGauge = max( 0.0f, fMaxGauge );

	return fMaxGauge;
}

float ioItem::GetMaxExtendSkillGauge( const ioHashString &szName )
{
	if( m_fChangeSkillExtendGauge <= 0.0f )
		return 0.0f;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, szName) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fValue = FLOAT1 + (fValue/FLOAT100);

	float fMaxGauge = (m_fMaxSkillGauge + m_fChangeSkillExtendGauge) / fValue;
	fMaxGauge = max( 0.0f, fMaxGauge );

	return fMaxGauge;
}

float ioItem::GetItemArmorClass( const ioHashString &szName )
{
	float fArmorClass = m_fArmorClass;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_DEFENSE, szName) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fArmorClass += fValue;
	//fArmorClass = min( fArmorClass, FLOAT100 );

	return fArmorClass;
}

float ioItem::GetItemArmorClassNPC( const ioHashString &szName )
{
	float fArmorClass = m_fArmorClass;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_DEFENSE, szName) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = m_pCreator->GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	fArmorClass += fValue;
	return fArmorClass;
}

ioGrowthUpInfo* ioItem::GetGrowthUpInfoByType( int iType, const ioHashString &szName )
{
	int iLevel = 0;
	int iInfoNum = 0;
	int iClassType = 0;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioGrowthLevel* pLevel = NULL;
	ioBaseChar *pChar     = NULL;
	if( pStage )
	{
		pLevel = pStage->GetGrowthLevel( szName );

		pChar = pStage->GetBaseChar( szName );
		if( pChar )
		{
			if( pChar->HasBuff( BT_EXCEPT_GROWTH_BUFF ) )
			{
				ioBuff* pBuff = pChar->GetBuff( BT_EXCEPT_GROWTH_BUFF );
				ioExceptGrowthBuff* pExceptGrowthBuff = ToExceptGrowthBuff( pBuff );
				if( pExceptGrowthBuff )
				{
					DWORDVec exceptType = pExceptGrowthBuff->GetExceptGrowthType();
					for( int i = 0; i < (int)exceptType.size(); i++ )
					{
						if( iType == exceptType[i] )
							return NULL;
					}
				}
			}

			iClassType = pChar->GetCharacterInfo().m_class_type;
		}
	}

	if( pLevel )
	{
		int iItemType = m_dwItemCode / DEFAULT_BASIC_ITEM_CODE;
		int iGrowthNum = 0;

		int iSize = m_vGrowthInfoNum.size();
		for( int i=0; i < iSize; ++i )
		{
			if( g_GrowthInfoMgr.CheckGrowthInfoType( m_vGrowthInfoNum[i], iType ) )
			{
				iInfoNum = m_vGrowthInfoNum[i];
				iGrowthNum = i+1;
				break;
			}
		}

		int iSlot = GetType() - 1;
		iLevel = pLevel->GetItemGrowthLevel( iClassType, iSlot, false );

		int nReinforce = GetItemReinforce();
		//강화레벨에 따른 추가 스탯
		if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
			iLevel += nReinforce;
		else
			iLevel += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );

		if( pChar )
		{
			pChar->GetMedalItemGrowth( iLevel, false, iSlot );			
			pChar->GetPetGrowth( iLevel, false, iSlot );

			//각성
			pChar->GetAwakeCharGrowth( iLevel, iSlot );
		}
	}

	return g_GrowthInfoMgr.GetGrowthUpInfo( iInfoNum, iLevel );
}

int ioItem::GetGrowthLevel( const ioHashString &szName )
{
	int iLevel = 0;
	int iInfoNum = 0;
	int iClassType = 0;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioGrowthLevel* pLevel = NULL;
	if( pStage )
	{
		pLevel = pStage->GetGrowthLevel( szName );

		ioBaseChar *pChar = pStage->GetBaseChar( szName );
		if( pChar )
		{
			iClassType = pChar->GetCharacterInfo().m_class_type;
		}
	}

	if( pLevel )
	{
		int iSlot = GetType() - 1;
		iLevel = pLevel->GetItemGrowthLevel( iClassType, iSlot, false );
	}

	return iLevel;
}

void ioItem::ResetTicCheck()
{
	m_dwTicCheckTime = FRAMEGETTIME();
}

void ioItem::SetSkillName( const ioHashString &rkSkillName )
{
	m_SkillName = rkSkillName;

	// 없으면 Insert
	int iSkillIndex = 0;
	SkillInfoMap::const_iterator iter = m_SkillNameMap.begin();
	while( iter != m_SkillNameMap.end() )
	{
		if( iter->second == rkSkillName )
			return;
		++iter;
		++iSkillIndex;
	}
	m_SkillNameMap.insert( SkillInfoMap::value_type( iSkillIndex, rkSkillName ) );
}

bool ioItem::ApplyCurSkillName( ioBaseChar *pOwner, const ioHashString &szName )
{
	if( !pOwner )
		return false;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return false;

	if( szName.IsEmpty() )
		return false;

	if( m_SkillName == szName )
		return false;

	if( m_ChangeSkillNameByBuff == szName )
	{
		m_SkillName = m_ChangeSkillNameByBuff;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		if( !m_ChangeSkillEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ChangeSkillEffect );
			if( pEffect )
			{
				m_dwChangeSkillEffect = pEffect->GetUniqueID();
			}
		}

		return true;
	}

	return false;
}

float ioItem::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	return fCurDamageRate;
}

float ioItem::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	return fCurForceRate;
}

float ioItem::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	return fCurBlowRate;
}

float ioItem::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	return fCurRangeRate;
}

float ioItem::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	return fCurSpeedRate;
}

float ioItem::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	return fCurFloatRate;
}

float ioItem::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	return fCurScaleRate;
}

float ioItem::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	return fCurExplosionRate;
}

float ioItem::GetChargeWoundedTimeRateForSkill()
{
	float fCurWoundedTimeRate = FLOAT1;
	return fCurWoundedTimeRate;
}

void ioItem::ClearCurChargeRateForSkill()
{
}

//void ioItem::PrepareLoadingAnimationList( const ioHashString &rkAniName )
//{
// 	if( rkAniName.IsEmpty() ) return;
// 
// 	char szExt[MAX_PATH] = "";
// 	_splitpath( rkAniName.c_str(), NULL, NULL, NULL, szExt );
// 	if( strcmp( szExt, ".ani" ) != 0 )
// 		return;
// 
// 	m_AnimationList.push_back( rkAniName );
// 
// 	// 여성 애니가 따로 존재하면 포함
// 	ioHashString kFemaleAniName = Help::ConvertTailNameAndFileCheck( rkAniName, false );
// 	if( !kFemaleAniName.IsEmpty() )
// 	{
// 		m_AnimationList.push_back( kFemaleAniName );
// 	}
//}

const ioHashString& ioItem::GetItemEquipEffect( bool bMale ) const
{
	bool bBaseEquipMesh = false;
	if( g_ItemMaker.IsBaseEquipMesh() && (GetType() == ES_WEAPON ||	GetType() == ES_OBJECT) )
		bBaseEquipMesh = true;

	if( bBaseEquipMesh )
		return m_ItemEquipDefaultEffect;

	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if( bMale )
			return m_CostumeInfo.m_szEffect;

		if( m_CostumeInfo.m_szEffectHW.IsEmpty() )
			return m_CostumeInfo.m_szEffect;

		return m_CostumeInfo.m_szEffectHW;
	}

	if( bMale )
	{
		return m_ItemEquipEffect;
	}

	if( m_ItemEquipEffectHw.IsEmpty() )
		return m_ItemEquipEffect;

	return m_ItemEquipEffectHw;
}

const ioHashString& ioItem::GetItemEquipEffect() const
{
	bool bBaseEquipMesh = false;
	if( g_ItemMaker.IsBaseEquipMesh() && (GetType() == ES_WEAPON ||	GetType() == ES_OBJECT) )
		bBaseEquipMesh = true;

	if( bBaseEquipMesh )
		return m_ItemEquipDefaultEffect;

	return m_CurItemEquipEffect;
}

void ioItem::ChangeSkillByBuff( ioBaseChar *pOwner, const ioHashString &rkBuffName, bool bChange )
{
	if( m_ChangeSkillBuff.IsEmpty() )
		return;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return;

	if( rkBuffName.IsEmpty() )
		return;

	if( rkBuffName != m_ChangeSkillBuff )
		return;

	if( bChange )
	{
		if( m_SkillName == m_ChangeSkillNameByBuff )
		{
			return;
		}

		m_SkillName = m_ChangeSkillNameByBuff;
	}
	else
	{
		ioHashString szNewName = FindSkillName( 0 );

		if( m_SkillName == szNewName )
		{
			return;
		}

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		m_SkillName = szNewName;
	}
	
	if( pOwner )
	{
		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );
	}
}

void ioItem::ChangeSkillByHP( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return;

	if( pOwner->GetState() == CS_DIE )
	{
		ioHashString szNewName = FindSkillName( 0 );

		if( m_SkillName == szNewName )
			return;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		m_SkillName = szNewName;

		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );

		return;
	}

	if( m_fChangeSkillHPRate < 0.0f )
		return;

	float fMax = pOwner->GetHP().m_fMaxValue;
	float fCur = pOwner->GetHP().m_fCurValue;

	if( fMax <= 0.0f )
		return;

	float fCurRate = fCur / fMax;

	if( fCurRate <= m_fChangeSkillHPRate )
	{
		if( m_SkillName == m_ChangeSkillNameByBuff )
			return;

		m_SkillName = m_ChangeSkillNameByBuff;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		if( !m_ChangeSkillEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ChangeSkillEffect );
			if( pEffect )
			{
				m_dwChangeSkillEffect = pEffect->GetUniqueID();
			}
		}

		int iSlot = GetType() - 1;
		ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
		ioHashString szAutoStartBuff = pSkill->GetAutoStartBuff();
		if( !szAutoStartBuff.IsEmpty() )
		{
			pOwner->AddNewBuff( szAutoStartBuff, pOwner->GetCharName(), GetName(), NULL );
		}
	}
	else
	{
		ioHashString szNewName = FindSkillName( 0 );

		if( m_SkillName == szNewName )
			return;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		m_SkillName = szNewName;
	}

	if( pOwner )
	{
		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );
	}
}

void ioItem::ChangeSkillByGauge( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return;

	if( pOwner->GetState() == CS_DIE )
	{
		ioHashString szNewName = FindSkillName( 0 );

		if( m_SkillName == szNewName )
			return;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		m_SkillName = szNewName;

		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );

		return;
	}

	if( m_fChangeSkillExtendGauge <= 0.0f )
		return;

	float fMaxGauge = GetMaxExtendSkillGauge( pOwner->GetCharName() );
	float fCurGauge = GetCurSkillGuage();

	if( fMaxGauge <= 0.0f )
		return;

	if( fMaxGauge <= fCurGauge )
	{
		if( m_SkillName == m_ChangeSkillNameByBuff )
			return;

		m_SkillName = m_ChangeSkillNameByBuff;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		if( !m_ChangeSkillEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ChangeSkillEffect );
			if( pEffect )
			{
				m_dwChangeSkillEffect = pEffect->GetUniqueID();
			}
		}

		int iSlot = GetType() - 1;
		ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
		ioHashString szAutoStartBuff = pSkill->GetAutoStartBuff();
		if( !szAutoStartBuff.IsEmpty() )
		{
			pOwner->AddNewBuff( szAutoStartBuff, pOwner->GetCharName(), GetName(), NULL );
		}
	}
	else
	{
		ioHashString szNewName = FindSkillName( 0 );

		if( m_SkillName == szNewName )
			return;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		m_SkillName = szNewName;
	}

	if( pOwner )
	{
		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );
	}
}

void ioItem::ReserveChangeSkillByBuff( const ioHashString &rkBuffName, bool bChange )
{
	ChangeSkillInfo kInfo;
	kInfo.m_bChange = bChange;
	kInfo.m_BuffName = rkBuffName;

	m_ReserveChangeSkillInfo.push_back( kInfo );
}

void ioItem::CheckChangeSkillByBuff( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ChangeSkillBuff.IsEmpty() )
		return;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return;

	if( pOwner->HasBuff( m_ChangeSkillBuff ) )
	{
		m_SkillName = m_ChangeSkillNameByBuff;

		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );
	}
}

void ioItem::CheckChangeSkillByHP( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return;

	if( m_fChangeSkillHPRate < 0.0f )
		return;

	float fMax = pOwner->GetHP().m_fMaxValue;
	float fCur = pOwner->GetHP().m_fCurValue;

	if( fMax <= 0.0f )
		return;

	float fCurRate = fCur / fMax;
	if( fCurRate <= m_fChangeSkillHPRate )
	{
		m_SkillName = m_ChangeSkillNameByBuff;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		if( !m_ChangeSkillEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ChangeSkillEffect );
			if( pEffect )
			{
				m_dwChangeSkillEffect = pEffect->GetUniqueID();
			}
		}

		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );
	}
}

void ioItem::CheckChangeSkillByGauge( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ChangeSkillNameByBuff.IsEmpty() )
		return;

	if( m_fChangeSkillExtendGauge <= 0.0f )
		return;

	float fMaxGauge = GetMaxExtendSkillGauge( pOwner->GetCharName() );
	float fCurGauge = GetCurSkillGuage();

	if( fMaxGauge <= 0.0f )
		return;

	if( fMaxGauge <= fCurGauge )
	{
		m_SkillName = m_ChangeSkillNameByBuff;

		if( m_dwChangeSkillEffect != -1 )
		{
			pOwner->EndEffect( m_dwChangeSkillEffect, false );
			m_dwChangeSkillEffect = -1;
		}

		if( !m_ChangeSkillEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ChangeSkillEffect );
			if( pEffect )
			{
				m_dwChangeSkillEffect = pEffect->GetUniqueID();
			}
		}

		int iSlot = GetType() - 1;
		pOwner->ChangeEquipedSkill( iSlot, this );
	}
}

void ioItem::CheckReserveChangeSkillByBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ChangeSkillInfoList::iterator iter = m_ReserveChangeSkillInfo.begin();
	while( iter != m_ReserveChangeSkillInfo.end() )
	{
		ChangeSkillByBuff( pOwner, (*iter).m_BuffName, (*iter).m_bChange );

		iter = m_ReserveChangeSkillInfo.erase( iter );
	}
}

int ioItem::GetCharOnlySexType() const
{
	/*
	int iOnlySexType = ioSetItemInfo::CST_ALL;
	DWORD dwCode = GetItemCode();

	int iCheck1 = dwCode % 10000;
	if( iCheck1 < 1000 )
	{
		int iClassType = dwCode % 1000;
		iOnlySexType = g_SetItemInfoMgr.GetCharOnlySexType( iClassType );
	}

	return iOnlySexType;
	*/

	return m_iCharOnlySexType;
}

int ioItem::GetGraffitiWeaponActionMax()
{
	if( m_WeaponActionType == WAT_GRAFFITI )
		return m_iWeaponActionMaxCnt;

	return 0;
}

bool ioItem::GetExtendSkillName( OUT ioHashString& szExtendSkill )
{
	if( m_fChangeSkillExtendGauge > 0.0f )
	{
		szExtendSkill = m_ChangeSkillNameByBuff;
		return true;
	}

	return false;
}

void ioItem::Initialize( ioBaseChar *pOwner, ioPlayStage* pStage )
{
}

void ioItem::InitializeBeforeEquip( ioBaseChar *pOwner, ioPlayStage* pStage )
{
}

void ioItem::SetCostume( int nCostumeCode, bool bIsOwnerChar )
{
	// 코스튬 관련 (자신만 적용)
	if ( nCostumeCode <= 0 || 
		( g_CostumeInfoMgr.IsDisableCostumeMode() && !bIsOwnerChar ) ||
		( !Setting::IsCostumeShowChange() && !g_CostumeInfoMgr.IsEnableCostumeMode() && !bIsOwnerChar ) )
	{
		m_CostumeInfo.Init();
		return;
	}
	g_CostumeInfoMgr.GetCostumeInfo( nCostumeCode, m_CostumeInfo );
}

void ioItem::SetCurEquipEffect( bool bMale )
{
	if( bMale )
	{
		if ( m_CostumeInfo.m_nCostumeCode > 0 )
		{
			if ( !m_CostumeInfo.m_szEffect.IsEmpty() )
				m_CurItemEquipEffect = m_CostumeInfo.m_szEffect;
		}
		else 
		{
			if( !m_ItemEquipEffect.IsEmpty() )
				m_CurItemEquipEffect = m_ItemEquipEffect;
		}
	}
	else
	{
		if ( m_CostumeInfo.m_nCostumeCode > 0 )
		{
			if ( !m_CostumeInfo.m_szEffectHW.IsEmpty() )
				m_CurItemEquipEffect = m_CostumeInfo.m_szEffectHW;
			else if( !m_CostumeInfo.m_szEffect.IsEmpty() )
				m_CurItemEquipEffect = m_CostumeInfo.m_szEffect;
		}
		else
		{
			if( !m_ItemEquipEffectHw.IsEmpty() )
				m_CurItemEquipEffect = m_ItemEquipEffectHw;
			else if( !m_ItemEquipEffect.IsEmpty() )
				m_CurItemEquipEffect = m_ItemEquipEffect;
		}
	}
}

DWORD ioItem::GetItemMaleCustom() const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		return m_dwCostumeMaleCustom;
	return m_dwMaleCustom; 
}

DWORD ioItem::GetItemFemaleCustom()	const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		return m_dwCostumeFemaleCustom;
	return m_dwFemaleCustom; 
}

void ioItem::SetCostumeCustom( DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	m_dwCostumeMaleCustom = dwMaleCustom;
	m_dwCostumeFemaleCustom = dwFemaleCustom;
}

int ioItem::GetCostumeCode()
{
	return m_CostumeInfo.m_nCostumeCode;
}

DWORD ioItem::GetItemMaleCustombyBase() const
{
	return m_dwMaleCustom; 
}

DWORD ioItem::GetItemFemaleCustombyBase()	const
{
	return m_dwFemaleCustom; 
}

void ioItem::ShareSkillGauge()
{
	if(GetType() >= IT_WEAPON && GetType() <= IT_CLOAK)
	{
		SP2Packet kPacket( CUPK_SYNC_SKILL_GAUGE );
		kPacket << GetOwnerName() << ((int)GetType() - 1 );
		kPacket << m_fCurSkillGauge;
		P2PNetwork::SendToAllUser( kPacket );
		float fCurSkillGauge = (float)m_fCurSkillGauge;
		//LOG.PrintTimeAndLog( 0, "Send CUPK_SYNC_SKILL_GAUGE - Owner : %s, SkillGauge : %5.3f", GetOwnerName().c_str(), fCurSkillGauge );
	}
}