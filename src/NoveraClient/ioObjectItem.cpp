

#include "stdafx.h"

#include "ioObjectItem.h"
#include "ioBaseChar.h"

#include "BuffDefine.h"

ioObjectItem::ioObjectItem()
{
	// 생성시에만 초기화
	m_eObejctCreatorType	= OCT_NONE;
	Init();
}

ioObjectItem::ioObjectItem( const ioObjectItem &rhs )
: ioWeaponItem( rhs ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_fOffSet( rhs.m_fOffSet ),
 m_fHeightOffSet( rhs.m_fHeightOffSet ),
 m_bEnableWeaponSkill( rhs.m_bEnableWeaponSkill ),
 m_bEnableArmorSkill( rhs.m_bEnableArmorSkill ),
 m_bEnableHelmetSkill( rhs.m_bEnableHelmetSkill ),
 m_bEnableCloakSkill( rhs.m_bEnableCloakSkill ),
 m_bEnableBuffFlyJump( rhs.m_bEnableBuffFlyJump ),
 m_bDisableNormalAttack( rhs.m_bDisableNormalAttack ),
 m_bPreventRecoveryEquipSKillGauge( rhs.m_bPreventRecoveryEquipSKillGauge ),
 m_bPreventRecoveryAllSKillGauge( rhs.m_bPreventRecoveryAllSKillGauge ),
 m_iMaxUseCount( rhs.m_iMaxUseCount ),
 m_nEableWeaponSkillType( rhs.m_nEableWeaponSkillType ),
 m_nEnableArmorSkillType( rhs.m_nEnableArmorSkillType ),
 m_nEnableHelmetSkillType( rhs.m_nEnableHelmetSkillType ),
 m_nEnableCloakSkillType( rhs.m_nEnableCloakSkillType ),
 m_FemaleEquipMesh( rhs.m_FemaleEquipMesh ),
 m_ObjectReleaseSelctionBuffList( rhs.m_ObjectReleaseSelctionBuffList ),
 m_szDeleteFieldItemAni( rhs.m_szDeleteFieldItemAni )
{
	// 생성시에만 초기화
	m_eObejctCreatorType	= OCT_NONE;
	Init();
}

ioObjectItem::~ioObjectItem()
{
}

void ioObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );
	
	LoadBuffList( rkLoader );

	m_fOffSet = rkLoader.LoadFloat_e( "offset", 0.0f );
	m_fHeightOffSet = rkLoader.LoadFloat_e( "height_offset", 0.0f );

	m_bEnableWeaponSkill = rkLoader.LoadBool_e( "enable_weapon_skill", false );
	m_nEableWeaponSkillType = rkLoader.LoadInt_e( "enable_weapon_skill_type", (int)EST_ALL );

	m_bEnableArmorSkill = rkLoader.LoadBool_e( "enable_armor_skill", false );
	m_nEnableArmorSkillType = rkLoader.LoadInt_e( "enable_armor_skill_type", (int)EST_ALL );

	m_bEnableHelmetSkill = rkLoader.LoadBool_e( "enable_helmet_skill", false );
	m_nEnableHelmetSkillType = rkLoader.LoadInt_e( "enable_helmet_skill_type", (int)EST_ALL );

	m_bEnableCloakSkill = rkLoader.LoadBool_e( "enable_cloak_skill", false );
	m_nEnableCloakSkillType = rkLoader.LoadInt_e( "enable_cloak_skill_type", (int)EST_ALL );

	m_bDisableNormalAttack = rkLoader.LoadBool_e( "disable_normal_attack", false );

	m_bEnableBuffFlyJump = rkLoader.LoadBool_e( "enable_fly_jump_buff", false );

	m_bPreventRecoveryEquipSKillGauge = rkLoader.LoadBool_e( "prevent_recovery_equip_skill_gauge", false );
	m_bPreventRecoveryAllSKillGauge   = rkLoader.LoadBool_e( "prevent_recovery_all_skill_gauge", false );

	m_iMaxUseCount = rkLoader.LoadInt_e( "max_use_count", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "equip_mesh_female", "", szBuf, MAX_PATH );
	m_FemaleEquipMesh = szBuf;

	int iReleaseSelectionBuffCnt = rkLoader.LoadInt_e( "object_release_selection_buff_cnt", 0 );
	for( int i=0; i < iReleaseSelectionBuffCnt; ++i )
	{
		wsprintf_e( szBuf, "object_release_selection_buff%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );

		m_ObjectReleaseSelctionBuffList.push_back( szBuf );
	}
	rkLoader.LoadString_e( "delete_fielditem_ani", "", szBuf, MAX_PATH );
	m_szDeleteFieldItemAni = szBuf;
}

void ioObjectItem::Init()
{
	m_dwObjectReleaseTime	= 0;
	m_bObjectReleaseTCPSend	= false;
}

bool ioObjectItem::IsOneTimeWeaponItem() const
{
	return true;
}

bool ioObjectItem::IsUseDelete() const
{
	return IsOneTimeWeaponItem();
}

bool ioObjectItem::IsEnableDash() const
{
	return false;
}

bool ioObjectItem::IsEnableDefense() const
{
	return false;
}

ioItem::ItemType ioObjectItem::GetType() const
{
	return IT_OBJECT;
}

ioWeaponItem::WeaponSubType ioObjectItem::GetSubType() const
{
	return WST_OBJECT;
}

void ioObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	Init();

	m_ActiveCountBuff.Clear();	
	ioActiveCountBuff* pActive1 = ToActiveCountBuff( pOwner->GetBuff( BT_ACTIVE_COUNT ) );
	if( pActive1 && m_iMaxUseCount > 0 )		
	{
		pActive1->SetActiveCount( m_iMaxUseCount );
		m_ActiveCountBuff = pActive1->GetName();
		return;
	}

	ioActiveCountBuff2* pActive2 = ToActiveCountBuff2( pOwner->GetBuff( BT_ACTIVE_COUNT2 ) );
	if( pActive2 )
	{
		m_ActiveCountBuff = pActive2->GetName();
		return;
	}
}

void ioObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
}

void ioObjectItem::LoadBuffList( ioINILoader &rkLoader )
{
	m_OwnerBuffList.clear();

	char szBuf[MAX_PATH], szBuf2[MAX_PATH];
	int iCnt;

	iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szBuf2, "owner_buff%d", i+1 );
		rkLoader.LoadString( szBuf2, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName  = szBuf;
		tBuffInfo.m_iBuffType = 0;
		m_OwnerBuffList.push_back( tBuffInfo );
	}
}

int ioObjectItem::GetGrowthInfoNum( int iGrowthNum ) const
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( m_ObjectCreateItem, __FUNCTION__ );
	if( pItem )
	{
		return pItem->GetGrowthInfoNum( iGrowthNum );
	}

	return 0;
}

int ioObjectItem::GetGrowthInfoNumByType( int iType, int &iGrowthNum ) const
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( m_ObjectCreateItem, __FUNCTION__ );
	if( pItem )
	{
		return pItem->GetGrowthInfoNumByType( iType, iGrowthNum );
	}

	return 0;
}

void ioObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( pWeapon )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( m_ObjectCreateItem, __FUNCTION__ );
		if( pItem )
		{
			pWeapon->SetWeaponBySkill( true );
			pWeapon->SetCreateItem( m_ObjectCreateItem );
		}
		else
		{
			pWeapon->SetCreateItem( m_Name );
		}
	}
}

bool ioObjectItem::IsProtectState()
{
	return false;
}

bool ioObjectItem::EnableReduceGaugeOnSwitchBuff()
{
	return true;
}

void ioObjectItem::SetObjectCreateItem( const ioHashString& szItemName )
{
	m_ObjectCreateItem = szItemName;
}

bool ioObjectItem::CheckEnableUseSkill( int iSlot, ioBaseChar *pOwner )
{
	ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
	if( !pSkill )
		return false;

	switch( iSlot )
	{
	case ES_WEAPON:
		if( m_bEnableWeaponSkill )
		{
			if( m_nEableWeaponSkillType == EST_ALL )
				return true;
			if( m_nEableWeaponSkillType == EST_TYPE1 && pSkill->GetObjEnableSkillType() == ioSkill::OET_TYPE1 )
				return true;
		}
		break;
	case ES_ARMOR:
		if( m_bEnableArmorSkill )
		{
			if( m_nEnableCloakSkillType == EST_ALL )
				return true;
			if( m_nEnableCloakSkillType == EST_TYPE1 && pSkill->GetObjEnableSkillType() == ioSkill::OET_TYPE1 )
				return true;
		}
		break;
	case ES_HELMET:
		if( m_bEnableHelmetSkill )
		{
			if( m_nEnableHelmetSkillType == EST_ALL )
				return true;
			if( m_nEnableHelmetSkillType == EST_TYPE1 && pSkill->GetObjEnableSkillType() == ioSkill::OET_TYPE1 )
				return true;
		}
		break;
	case ES_CLOAK:
		if( m_bEnableCloakSkill )
		{
			if( m_nEnableCloakSkillType == EST_ALL )
				return true;
			if( m_nEnableCloakSkillType == EST_TYPE1 && pSkill->GetObjEnableSkillType() == ioSkill::OET_TYPE1 )
				return true;
		}
		break;
	}

	return false;
}

void ioObjectItem::CheckAttackCount( ioBaseChar* pOwner )
{
}

void ioObjectItem::CheckActiveCount( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_ActiveCountBuff.IsEmpty() )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_ActiveCountBuff );
		ioActiveCountBuff* pActivetBuff = ToActiveCountBuff( pBuff );
		if( pActivetBuff )
			pActivetBuff->CheckActiveCount();
	}
}

bool ioObjectItem::IsAttackComboInit() const
{
	return true;
}

bool ioObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( m_dwObjectReleaseTime == 0 )
	{
		if( pOwner ) 		
		{
			switch( GetObejctCreateType() )
			{
			case ioObjectItem::OCT_EQUIP_BUFF1:
			case ioObjectItem::OCT_EQUIP_BUFF2:
				{
					//버프에 의해서 생성된 오브젝트 아이템은 버프가 제거 되면 해당 아이템도 제거됨
					ioHashString szControlBuff = GetControlBuffName();
					if( !szControlBuff.IsEmpty() && !pOwner->HasBuff( szControlBuff ) )
					{
						m_dwObjectReleaseTime = FRAMEGETTIME();
						return true;
					}
					
					//예외적으로 지정된 버프가 제거되면 오브젝트 아이템도 제거
					if( CheckObjectReleaseSelectionBuff( pOwner ) )
					{
						m_dwObjectReleaseTime = FRAMEGETTIME();
						return true;
					}
				}
				break;
			case ioObjectItem::OCT_SOILDER:
				{
					//각 용병에서 생성된 오브젝트 아이템에서 스킬 및 용병쪽에서 판단하여 제거됨
					
					//14.11.28 내용추가 : 자신을 생성한 무기 아이템이 없으면 Release				
					ioWeaponItem* pWeaponItem = pOwner->GetWeapon();
					if( pWeaponItem == NULL )
					{
						m_dwObjectReleaseTime = FRAMEGETTIME();
						return true;
					}

					//14.11.28 내용추가 : 현재 무기가 자신을 생성한 무기 아이템이 아니면 Release
					if( m_ObjectCreateItem != pWeaponItem->GetName() )
					{
						m_dwObjectReleaseTime = FRAMEGETTIME();
						return true;
					}
				}
				break;
			case ioObjectItem::OCT_EQUIP_SKILL:
				{
					//횟수 제한을 체크하는 오브젝트 아이템의 경우 엑티브 버프가 제거 되면 삭제
					if( !m_ActiveCountBuff.IsEmpty() && !pOwner->HasBuff( m_ActiveCountBuff ) )
					{
						m_dwObjectReleaseTime = FRAMEGETTIME();
						return true;
					}

					//예외적으로 지정된 버프가 제거되면 오브젝트 아이템도 제거
					if( CheckObjectReleaseSelectionBuff( pOwner ) )
					{
						m_dwObjectReleaseTime = FRAMEGETTIME();
						return true;
					}
				}
				break;
			}
		}
	}
	
	return false;
}

bool ioObjectItem::CheckReleaseByNormalAttack( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	return false;
}

bool ioObjectItem::CheckObjectReleaseSelectionBuff( ioBaseChar *pOwner )
{
	if( m_ObjectReleaseSelctionBuffList.empty() )
		return false;

	for( ioHashStringVec::iterator iter = m_ObjectReleaseSelctionBuffList.begin(); iter != m_ObjectReleaseSelctionBuffList.end(); ++iter )
	{
		const ioHashString& szBuffName = *iter;
		if( !szBuffName.IsEmpty() && !pOwner->HasBuff( szBuffName ) )
		{
			return true;
		}
	}

	return false;
}

const ioHashString& ioObjectItem::GetEquipMesh( ioBaseChar *pOwner ) const
{
	if ( !pOwner )
		return m_EquipMesh;

	if ( pOwner->IsMale() )
		return m_EquipMesh;
	else if ( m_FemaleEquipMesh.IsEmpty() )
		return m_EquipMesh;

	return m_FemaleEquipMesh;
}