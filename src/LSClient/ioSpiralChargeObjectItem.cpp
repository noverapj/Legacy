#include "stdafx.h"

#include "ioSpiralChargeObjectItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioActiveCountBuff.h"

ioSpiralChargeObjectItem::ioSpiralChargeObjectItem()
{
	Init();
}

ioSpiralChargeObjectItem::ioSpiralChargeObjectItem( const ioSpiralChargeObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_WeaponChargingEffect( rhs.m_WeaponChargingEffect ),
	m_vChargingInfoList( rhs.m_vChargingInfoList ),
	m_szChargingSound( rhs.m_szChargingSound ),
	m_WoundedAttackList( rhs.m_WoundedAttackList ),
	m_iReduceGaugeSlot( rhs.m_iReduceGaugeSlot )
{
	Init();
}

ioSpiralChargeObjectItem::~ioSpiralChargeObjectItem()
{
	if( m_WeaponChargingEffectID != -1 )
	{
		g_EffectFactory.DestroyEffect( m_WeaponChargingEffectID );
		m_WeaponChargingEffectID = -1;
	}

	if( m_dwChargingSoundID != -1 )
	{
		g_SoundMgr.StopSound( m_szChargingSound, m_dwChargingSoundID );
		m_dwChargingSoundID = -1;
	}
}

void ioSpiralChargeObjectItem::Init()
{
	m_WeaponChargingEffectID	= -1;
	m_dwChargingSoundID			= -1;

	m_fMaxScaleRate				= FLOAT1;
	m_fMinScaleRate				= FLOAT1;
	m_fMaxScaleTime				= 0.0f;

	m_vLinkOffsetPos			= ioMath::VEC3_ZERO;

	m_iCurChargingIndex			= 0;
	m_dwMotionEndTime			= 0;
}

void ioSpiralChargeObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "weapon_charging_effect", "", szBuf, MAX_PATH );
	m_WeaponChargingEffect = szBuf;

	LoadChargingScale( rkLoader, "charging", m_vChargingInfoList );

	rkLoader.LoadString_e( "charging_loop_sound", "", szBuf, MAX_PATH );
	m_szChargingSound = szBuf;	

	m_iReduceGaugeSlot = rkLoader.LoadInt_e( "reduce_gauge_slot", 0 );
}

void ioSpiralChargeObjectItem::LoadChargingScale( IN ioINILoader &rkLoader, IN const ioHashString& szTitle, OUT ScaleInfoList& vList )
{
	m_vChargingInfoList.clear();

	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_cnt", szTitle.c_str() );	
	int iMax = rkLoader.LoadInt( szKey, 0 );

	for( int i = 0; i < iMax; ++i )
	{
		ScaleInfo Info;		
		wsprintf_e( szKey, "%s%d_scale", szTitle.c_str(), i + 1 );
		Info.m_fScaleRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "%s%d_time", szTitle.c_str(), i + 1 );
		Info.m_dwScaleTime = rkLoader.LoadInt( szKey, i * 100 );


		AttackAttribute kJumpAttribute;
		wsprintf_e( szKey, "%s%d_jump_attack", szTitle.c_str(), i + 1 );
		LoadAttackAttribute( szKey, kJumpAttribute, rkLoader );
		Info.m_JumpAttackAttribute = kJumpAttribute;

		AttackAttribute kNormalAttribute;
		wsprintf_e( szKey, "%s%d_normal_attack", szTitle.c_str(), i + 1 );
		LoadAttackAttribute( szKey, kNormalAttribute, rkLoader );
		Info.m_NormalAttackAttribute = kNormalAttribute;

		vList.push_back( Info );
	}
}

ioItem* ioSpiralChargeObjectItem::Clone()
{		
	return new ioSpiralChargeObjectItem( *this );
}

ioObjectItem::ObjectSubType ioSpiralChargeObjectItem::GetObjectSubType() const
{
	return OST_SPIRAL_CHARGE;
}

bool ioSpiralChargeObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioSpiralChargeObjectItem::IsEnableDefense() const
{
	return false;
}

bool ioSpiralChargeObjectItem::IsEnableDash() const
{
	return true;
}

const AttackAttribute* ioSpiralChargeObjectItem::GetAttackAttribute( AttackType eType, int iSubIdx /* = 0 */ ) const
{
	if( !m_vChargingInfoList.empty() )		
	{
		if( eType == AT_JUMP_ATTACK )
		{
			return &m_vChargingInfoList[m_iCurChargingIndex].m_JumpAttackAttribute;			
		}
		else
		{
			return &m_vChargingInfoList[m_iCurChargingIndex].m_NormalAttackAttribute;
		}
	}

	return ioAttackableItem::GetAttackAttribute( eType, iSubIdx );
}

const WeaponInfoList& ioSpiralChargeObjectItem::GetAttackAttributeIdx( AttackType eType, int iSubIdx /* = 0 */ ) const
{
	if( !m_vChargingInfoList.empty() )
	{
		if( eType == AT_JUMP_ATTACK )
		{
			return m_vChargingInfoList[m_iCurChargingIndex].m_JumpAttackAttribute.m_vWeaponInfoList;			
		}
		else
		{
			return m_vChargingInfoList[m_iCurChargingIndex].m_NormalAttackAttribute.m_vWeaponInfoList;
		}
	}

	return ioAttackableItem::GetAttackAttributeIdx( eType, iSubIdx );
}

const ForceInfoList& ioSpiralChargeObjectItem::GetForceInfoList( AttackType eType, int iSubIdx /* = 0 */ ) const
{
	if( !m_vChargingInfoList.empty() )
	{
		if( eType == AT_JUMP_ATTACK )
		{
			return m_vChargingInfoList[m_iCurChargingIndex].m_JumpAttackAttribute.m_vForceInfoList;			
		}
		else
		{
			return m_vChargingInfoList[m_iCurChargingIndex].m_NormalAttackAttribute.m_vForceInfoList;
		}
	}

	return ioAttackableItem::GetForceInfoList( eType, iSubIdx );
}

int ioSpiralChargeObjectItem::SetAttackMotionTime( ioBaseChar *pOwner, AttackType eType )
{
	int iAniID = -1;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	const AttackAttribute* pAttr = GetAttackAttribute( eType );
	if( pAttr && pGrp )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );

		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * pAttr->m_fAttackAniRate;
	}

	return iAniID;
}

void ioSpiralChargeObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( 0 < m_dwMotionEndTime )
		return;

	CheckActiveCount( pOwner );
	ReleaseChargingEffect( pOwner );

	SetAttackMotionTime( pOwner, AT_NORMAL );

	ioObjectItem::SetNormalAttack( pOwner, iCurCombo );
}

void ioSpiralChargeObjectItem::SetDashAttack( ioBaseChar *pOwner )
{	
	ReleaseChargingEffect( pOwner );
}

bool ioSpiralChargeObjectItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( 0 < m_dwMotionEndTime )
		return false;

	CheckActiveCount( pOwner );	
	ReleaseChargingEffect( pOwner );

	int iJumpAniID = SetAttackMotionTime( pOwner, AT_JUMP_ATTACK );	
	if( iJumpAniID == -1 )
		return ioObjectItem::SetJumpAttack(pOwner, iAniID, fTimeRate, eType );

	return ioObjectItem::SetJumpAttack(pOwner, iJumpAniID, fTimeRate, eType );
}

void ioSpiralChargeObjectItem::OnEquiped( ioBaseChar *pOwner )
{	
	ioObjectItem::OnEquiped( pOwner );

	Init();
	CreateEffect( pOwner );

	m_dwTicCheckTime = FRAMEGETTIME();
	m_dwStartTime = m_dwTicCheckTime;

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
}

void ioSpiralChargeObjectItem::CreateEffect( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_WeaponChargingEffect, pOwner->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			if( pMapEffect )
			{
				m_WeaponChargingEffectID = pMapEffect->GetUniqueID();
			}
		}
	}
}

D3DXVECTOR3 ioSpiralChargeObjectItem::GetLinkPosOffset( ioEffect* pEffect, ioBaseChar* pOwner )
{
	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset = ioMath::VEC3_ZERO;
	D3DXQUATERNION qtLinkRotOffset;

	if( pEffect && pOwner && pEffect->GetSkeletonName().IsEmpty() && pEffect->GetFirstLinkOffset( szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

int	ioSpiralChargeObjectItem::GetCurrActiveCount( ioBaseChar* pOwner )
{
	if( pOwner )
	{
		ioActiveCountBuff* pBuff = ToActiveCountBuff( pOwner->GetBuff( m_ActiveCountBuff ) );
		if( pBuff )
		{
			return pBuff->GetCurActiveCount();
		}
	}

	return 0;
}

void ioSpiralChargeObjectItem::OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	ioObjectItem::OnProcessState( pStage, pOwner );

	if( !pOwner )
		return;

	UpdateEffect( pStage, pOwner );
	UpdateCharging( pStage, pOwner );
	UpdateSound( pStage, pOwner );
	CheckCount( pStage, pOwner );

}

void ioSpiralChargeObjectItem::UpdateEffect( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	float fCurTime = (float)FRAMEGETTIME();

	float fRate = (fCurTime - m_dwStartTime) / m_fMaxScaleTime;
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
}

void ioSpiralChargeObjectItem::UpdateCharging( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !m_vChargingInfoList.empty() )
	{
		DWORD dwGapTime = FRAMEGETTIME() - m_dwStartTime;
		if( m_vChargingInfoList[m_iCurChargingIndex].m_dwScaleTime < dwGapTime )
		{
			int iSize = m_vChargingInfoList.size();
			m_iCurChargingIndex++;
			m_iCurChargingIndex = min( m_iCurChargingIndex, iSize-1 );
		}
	}
}

void ioSpiralChargeObjectItem::UpdateSound( ioPlayStage *pStage, ioBaseChar *pOwner )
{	
	if( !m_szChargingSound.IsEmpty() && m_dwChargingSoundID == -1 )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			m_dwChargingSoundID = g_SoundMgr.PlaySound( m_szChargingSound, pGrp->GetParentSceneNode(), PSM_LOOP );
		}
	}
}

void ioSpiralChargeObjectItem::CheckCount( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( 0 < GetCurrActiveCount( pOwner ) && 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime	= 0;
		m_iCurChargingIndex	= 0;
		m_dwTicCheckTime	= FRAMEGETTIME();
		m_dwStartTime		= m_dwTicCheckTime;

		CreateEffect( pOwner );
	}
}

bool ioSpiralChargeObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;

	if( IsAttackEnd( pOwner ) )
	{
		return true;
	}

	return false;
}

void ioSpiralChargeObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );
	ReleaseChargingEffect( pOwner );

	Init();
}

void ioSpiralChargeObjectItem::ReleaseChargingEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_WeaponChargingEffectID != -1 )
	{
		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )		
		{			
			ioMapEffect *pEff = pStage->FindMapEffect( m_WeaponChargingEffectID );
			if( pEff )			
				pEff->EndEffectGrace();

			m_WeaponChargingEffectID = -1;
		}
	}

	if( m_dwChargingSoundID != -1 )
	{
		g_SoundMgr.StopSound( m_szChargingSound, m_dwChargingSoundID );
		m_dwChargingSoundID = -1;
	}
}

const bool ioSpiralChargeObjectItem::IsAttackEnd( ioBaseChar* pOwner )
{
	if( 0 < m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( GetCurrActiveCount( pOwner ) <= 0 )
		{
			return true;
		}		
	}

	return false;
}