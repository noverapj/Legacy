#include "stdafx.h"

#include "ioPoweredObjectItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioPoweredObjectItem::ioPoweredObjectItem()
{
	m_WeaponChargingEffectID = -1;
	m_dwChargingSoundID = -1;

	m_fMaxScaleRate = FLOAT1;
	m_fMinScaleRate = FLOAT1;
	m_fMaxScaleTime = 0.0f;

	m_vLinkOffsetPos = ioMath::VEC3_ZERO;

	m_iCurChargingList = 0;

	m_dwMotionEndTime = 0;
	m_bAttacked = false;
}

ioPoweredObjectItem::ioPoweredObjectItem( const ioPoweredObjectItem &rhs )
: ioObjectItem( rhs ),
  m_EquipedEffect( rhs.m_EquipedEffect ),
  m_WeaponChargingEffect( rhs.m_WeaponChargingEffect ),
  m_vChargingInfoList( rhs.m_vChargingInfoList ),
  m_szChargingSound( rhs.m_szChargingSound ),
  m_WoundedAttackList( rhs.m_WoundedAttackList ),
  m_iReduceGaugeSlot( rhs.m_iReduceGaugeSlot )
{
	m_WeaponChargingEffectID = -1;
	m_dwChargingSoundID = -1;

	m_fMaxScaleRate = FLOAT1;
	m_fMinScaleRate = FLOAT1;
	m_fMaxScaleTime = 0.0f;

	m_vLinkOffsetPos = ioMath::VEC3_ZERO;

	m_iCurChargingList = 0;

	m_dwMotionEndTime = 0;
	m_bAttacked = false;
}

ioPoweredObjectItem::~ioPoweredObjectItem()
{
	if( m_WeaponChargingEffectID != -1 )
	{
		g_EffectFactory.DestroyEffect(m_WeaponChargingEffectID);
		m_WeaponChargingEffectID = -1;
	}

	if( m_dwChargingSoundID != -1 )
	{
		g_SoundMgr.StopSound( m_szChargingSound, m_dwChargingSoundID );
		m_dwChargingSoundID = -1;
	}
}

void ioPoweredObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "equiped_effect", "", szBuf, MAX_PATH );
	m_EquipedEffect = szBuf;

	rkLoader.LoadString_e( "weapon_charging_effect", "", szBuf, MAX_PATH );
	m_WeaponChargingEffect = szBuf;

	int iCnt = rkLoader.LoadInt_e("charging_scale_cnt", 0);
	if( iCnt != 0 )		
	{
		m_vChargingInfoList.clear();
		m_vChargingInfoList.reserve(iCnt);
		for( int i = 0; i < iCnt; ++i )
		{
			ScaleInfo Info;		
			wsprintf_e( szBuf, "charging%d_scale", i+1 );		
			Info.m_fScaleRate = rkLoader.LoadFloat( szBuf, FLOAT1 );

			wsprintf_e( szBuf, "charging%d_time", i+1 );
			Info.m_dwScaleTime = rkLoader.LoadInt( szBuf, i * 100 );

			AttackAttribute kAttribute;
			wsprintf_e( szBuf, "charging%d_attack", i+1 );
			LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
			Info.m_AttackAttribute = kAttribute;
			m_vChargingInfoList.push_back( Info );
		}
	}

	rkLoader.LoadString_e( "charging_loop_sound", "", szBuf, MAX_PATH );
	m_szChargingSound = szBuf;

	iCnt = rkLoader.LoadInt_e( "wounded_attack_type_count", 0 );
	if( iCnt != 0 )		
	{
		m_WoundedAttackList.clear();
		m_WoundedAttackList.reserve( iCnt );

		for( int i = 0;i < iCnt ; ++i )
		{
			WeaponInfo kInfo;
			int iAttackTypeCnt = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "wounded_attack_type%d", i+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "wounded_attack_type%d_resist", i+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "wounded_attack_type%d_wounded", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "wounded_attack_type%d_wounded_time", i+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "wounded_attack_type%d_wounded_loop_ani",  i+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );				

			m_WoundedAttackList.push_back( kInfo );
		}
	}
	
	m_iReduceGaugeSlot = rkLoader.LoadInt_e( "reduce_gauge_slot", 0 );
}

ioItem* ioPoweredObjectItem::Clone()
{		
	return new ioPoweredObjectItem( *this );
}

ioObjectItem::ObjectSubType ioPoweredObjectItem::GetObjectSubType() const
{
	return OST_POWEREDARMOR;
}

bool ioPoweredObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioPoweredObjectItem::IsEnableDefense() const
{
	return false;
}

bool ioPoweredObjectItem::IsEnableDash() const
{
	return true;
}

const WeaponInfoList& ioPoweredObjectItem::GetWoundedAttributeList( ioBaseChar *pOwner )
{
	ReduceSkillGauge( pOwner, m_iReduceGaugeSlot );
	SetAttacked();
	return m_WoundedAttackList;
}

const AttackAttribute* ioPoweredObjectItem::GetAttackAttribute( AttackType eType, int iSubIdx /* = 0 */ ) const
{
	if( m_vChargingInfoList.empty() )
		return ioAttackableItem::GetAttackAttribute( eType, iSubIdx );

	return &m_vChargingInfoList[m_iCurChargingList].m_AttackAttribute;
}

const WeaponInfoList& ioPoweredObjectItem::GetAttackAttributeIdx( AttackType eType, int iSubIdx /* = 0 */ ) const
{
	if( m_vChargingInfoList.empty() )
		return ioAttackableItem::GetAttackAttributeIdx( eType, iSubIdx );

	return m_vChargingInfoList[m_iCurChargingList].m_AttackAttribute.m_vWeaponInfoList;
}

const ForceInfoList& ioPoweredObjectItem::GetForceInfoList( AttackType eType, int iSubIdx /* = 0 */ ) const
{
	if( m_vChargingInfoList.empty() )
		return ioAttackableItem::GetForceInfoList( eType, iSubIdx );

	return m_vChargingInfoList[m_iCurChargingList].m_AttackAttribute.m_vForceInfoList;
}

void ioPoweredObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	if( pOwner->GetState() == CS_USING_SKILL )
		return;

	if( IsAttacked() )
		return;

	ioHashString szAniName = m_vChargingInfoList[m_iCurChargingList].m_AttackAttribute.m_AttackAnimation;
	float fTimeRate = m_vChargingInfoList[m_iCurChargingList].m_AttackAttribute.m_fAttackAniRate;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		SetAttackMotionTime( pOwner, pGrp->GetAnimationIdx( szAniName ), fTimeRate );
	}

	ioObjectItem::SetNormalAttack(pOwner, iCurCombo);
}

bool ioPoweredObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( IsAttacked() )
		return false;

	SetAttackMotionTime( pOwner, iAniID, fTimeRate );

	return ioObjectItem::SetJumpAttack(pOwner, iAniID, fTimeRate, eType);
}

void ioPoweredObjectItem::SetAttackMotionTime( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
	if( !pOwner )
		return;

	if( iAniID == -1 )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	}

	ReleaseChargingEffect(pOwner);
	SetAttacked();
	ReduceSkillGauge( pOwner, m_iReduceGaugeSlot );
}

void ioPoweredObjectItem::OnEquiped( ioBaseChar *pOwner )
{	
	m_dwTicCheckTime = FRAMEGETTIME();
	m_dwPoweredTime = m_dwTicCheckTime;

	if( pOwner )
	{
		if( pOwner->IsMale() )
		{
			if( !m_ItemEquipEffect.IsEmpty() )
			{
				pOwner->AttachEffect( m_ItemEquipEffect );
				m_CurItemEquipEffect = m_ItemEquipEffect;
			}
		}
		else
		{
			if( !m_ItemEquipEffectHw.IsEmpty() )
			{
				pOwner->AttachEffect( m_ItemEquipEffectHw );
				m_CurItemEquipEffect = m_ItemEquipEffectHw;
			}
			else if( !m_ItemEquipEffect.IsEmpty() )
			{
				pOwner->AttachEffect( m_ItemEquipEffect );
				m_CurItemEquipEffect = m_ItemEquipEffect;
			}
		}

		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_WeaponChargingEffect, vPos, vScale );
			if( pMapEffect )
			{
				m_WeaponChargingEffectID = pMapEffect->GetUniqueID();
			}
		}
	}

	m_vLinkOffsetPos = ioMath::VEC3_ZERO;

	m_fMaxScaleRate = FLOAT1;
	m_fMinScaleRate = FLOAT1;
	m_fMaxScaleTime = 0.0f;	

	ScaleInfoList::iterator iter;
	for( iter = m_vChargingInfoList.begin(); iter != m_vChargingInfoList.end(); ++iter )
	{
		ScaleInfo Info = (*iter);

		if( Info.m_fScaleRate < m_fMinScaleRate )
			m_fMinScaleRate = Info.m_fScaleRate;

		if( m_fMaxScaleRate < Info.m_fScaleRate )
		{
			m_fMaxScaleRate = Info.m_fScaleRate;
			m_fMaxScaleTime = (float)Info.m_dwScaleTime;
		}
	}

	// Check changeskill
	CheckChangeSkillByBuff( pOwner );
	CheckChangeSkillByHP( pOwner );
	CheckChangeSkillByGauge( pOwner );

	if( !m_EquipedEffect.IsEmpty() && pOwner )
	{
		pOwner->AttachEffect( m_EquipedEffect );
	}
}

D3DXVECTOR3 ioPoweredObjectItem::GetLinkPosOffset( ioEffect* pEffect, ioBaseChar* pOwner )
{
	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset = ioMath::VEC3_ZERO;
	D3DXQUATERNION qtLinkRotOffset;

	if( pEffect && pOwner &&
		pEffect->GetSkeletonName().IsEmpty() && pEffect->GetFirstLinkOffset( szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{						
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();						
		}
	}

	return vLinkPosOffset;
}

void ioPoweredObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	ioObjectItem::OnProcessState( pStage, pOwner );

	if( !pOwner )
		return;

	float fCurTime = (float)FRAMEGETTIME();

	float fRate = (fCurTime - m_dwPoweredTime) / m_fMaxScaleTime;
	fRate = fRate * m_fMaxScaleRate;
	fRate = min( fRate, m_fMaxScaleRate );
	fRate = max( fRate, m_fMinScaleRate );

	// Â÷Â¡ ÀÌÆåÆ®
	if( pStage )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_WeaponChargingEffectID );
		if( pMapEffect )
		{
			ioEffect* pEffect = pMapEffect->GetEffect();
			if( pEffect )			
				pEffect->GetParentNode()->SetScale( fRate, fRate, fRate );				

			m_vLinkOffsetPos = GetLinkPosOffset( pEffect, pOwner );
			pMapEffect->SetWorldPosition( m_vLinkOffsetPos );
		}
	}

	// Â÷Â¡ ´Ü°è
	if( !m_vChargingInfoList.empty() )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwPoweredTime;
		if( m_vChargingInfoList[m_iCurChargingList].m_dwScaleTime < dwGapTime )
		{
			int iSize = m_vChargingInfoList.size();
			m_iCurChargingList++;
			m_iCurChargingList = min( m_iCurChargingList, iSize-1 );
		}
	}

	// »ç¿îµå Ã³¸®
	if( m_dwChargingSoundID == -1 && !IsAttacked() )
	{
		if( !m_szChargingSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
				m_dwChargingSoundID = g_SoundMgr.PlaySound( m_szChargingSound, pGrp->GetParentSceneNode(), PSM_LOOP );
		}
	}
}

bool ioPoweredObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;
	
	DWORD dwMotionEndTime = GetMotionEndTime();
	if( IsAttackEnd() )
	{
		return true;
	}

	return false;
}

void ioPoweredObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	if( !m_EquipedEffect.IsEmpty() && pOwner )
	{
		pOwner->EndEffect( m_EquipedEffect );
	}

	ReleaseChargingEffect(pOwner);
}

void ioPoweredObjectItem::ReleaseChargingEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_WeaponChargingEffectID != -1 )
	{
		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )		
		{			
			pStage->DestroyMapEffect( m_WeaponChargingEffectID );
			m_WeaponChargingEffectID = -1;
		}
	}

	if( m_dwChargingSoundID != -1 )
	{
		g_SoundMgr.StopSound( m_szChargingSound, m_dwChargingSoundID );
		m_dwChargingSoundID = -1;
	}
}

void ioPoweredObjectItem::ReduceSkillGauge( ioBaseChar *pOwner, int iSlot )
{
	if( !pOwner )
		return;

	if( !COMPARE( iSlot, ES_WEAPON, ES_CLOAK+1 ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem( iSlot );
	if( pItem && pItem->HasSkillGauge() )
	{
		pItem->ReduceNeedGauge( pItem->GetMaxSkillGauge( pOwner->GetCharName() ), pOwner );
	}
}

const bool ioPoweredObjectItem::IsAttackEnd()
{
	if( IsAttacked() && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		return true;
	}
	return false;
}