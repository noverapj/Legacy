#include "StdAfx.h"
#include "ioCircleReserveAttackSkill.h"
#include "ItemDefine.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioCircleReserveAttackSkill::ioCircleReserveAttackSkill(void) :
m_fDefaultCircleOffSet( 0.0f )
{
	ClearData();
}

ioCircleReserveAttackSkill::ioCircleReserveAttackSkill( const ioCircleReserveAttackSkill &rhs )
: ioAttackSkill( rhs ),
m_fDefaultCircleOffSet( rhs.m_fDefaultCircleOffSet ),
m_SkillAttackType( rhs.m_SkillAttackType ),
m_fRandomRange( rhs.m_fRandomRange ),
m_fWeaponStartHeight( rhs.m_fWeaponStartHeight ),
m_szFireEffect( rhs.m_szFireEffect )
{
	ClearData();
}

ioCircleReserveAttackSkill::~ioCircleReserveAttackSkill(void)
{
	ClearData();
}

void ioCircleReserveAttackSkill::ClearData()
{
	m_dwReserveWeaponFireStartTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_vMoveDir = ioMath::VEC3_ZERO;
}

void ioCircleReserveAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";
	m_fDefaultCircleOffSet = rkLoader.LoadFloat_e( "default_circle_offset", 0.0f );

	LoadAttributeCallTimeList( rkLoader );

	m_SkillAttackType = (SkillAttackType)rkLoader.LoadInt_e( "skill_attack_type", 0 );

	m_fRandomRange = rkLoader.LoadFloat_e( "random_range", 0.0f );
	m_fWeaponStartHeight = rkLoader.LoadFloat_e( "weapon_start_height", 0.0f );

	rkLoader.LoadString_e( "fire_effect", "", szBuf, MAX_PATH );
	m_szFireEffect = szBuf;
}

void ioCircleReserveAttackSkill::LoadAttributeCallTimeList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	for ( int i=0 ; i < (int)m_AttackAttribute.m_vWeaponInfoList.size() ; ++i )
	{
		wsprintf_e( szBuf, "attack_type%d_call_time", i+1 );
		m_AttackAttribute.m_vWeaponInfoList[i].m_dwCallTime = (DWORD)rkLoader.LoadInt( szBuf, 0 );
	}

	for ( int i=0 ; i < (int)m_DownAttackAttribute.m_vWeaponInfoList.size() ; ++i )
	{
		wsprintf_e( szBuf, "down_attack_type%d_call_time", i+1 );
		m_DownAttackAttribute.m_vWeaponInfoList[i].m_dwCallTime = (DWORD)rkLoader.LoadInt( szBuf, 0 );
	}

	for ( int i=0 ; i < (int)m_JumpAttackAttribute.m_vWeaponInfoList.size() ; ++i )
	{
		wsprintf_e( szBuf, "jump_attack_type%d_call_time", i+1 );
		m_JumpAttackAttribute.m_vWeaponInfoList[i].m_dwCallTime = (DWORD)rkLoader.LoadInt( szBuf, 0 );
	}
}

ioSkill* ioCircleReserveAttackSkill::Clone()
{
	return new ioCircleReserveAttackSkill( *this );
}

bool ioCircleReserveAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_vMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if( pChar->IsNeedProcess() )
	{
		//제우스 아이템은 circlepos를 동기화시켜주지 않는다.
		ioZeusItem *pZeus = ToZeusItem( GetOwnerItem() );
		if( pZeus )
		{
			m_vCirclePos = pZeus->GetCirclePos();
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
			if ( m_vCirclePos == ioMath::VEC3_ZERO  )
			{
				float fCircleOffset = pZeus->GetCircleDefaultOffset();
				D3DXVECTOR3 vPlayerPos = pChar->GetWorldPosition();
				float fOwnerPosHeight = vPlayerPos.y;
				m_vCirclePos = vPlayerPos;
				m_vCirclePos += ( m_vMoveDir * fCircleOffset );
				m_vCirclePos.y = fOwnerPosHeight;
			}
		}
	}
	
	return true;
}

void ioCircleReserveAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	ClearData();
	ioZeusItem *pZeus = ToZeusItem( GetOwnerItem() );
	if( pZeus )
		pZeus->SetNoneState( pChar );
}

void ioCircleReserveAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )	
		return;

	ioAttackSkill::OnProcessState( pChar, pStage );

	if( m_dwReserveWeaponFireStartTime > 0 && m_dwReserveWeaponFireStartTime < FRAMEGETTIME() )
	{
		m_dwReserveWeaponFireStartTime = 0;
		if ( m_SkillAttackType == SAT_CIRCLE )
			CallReserveWeapon( pChar );
		else if ( m_SkillAttackType == SAT_PLAYER )
		{
			if ( pChar->IsNeedProcess() )
				CallReserveRandomPositionWeapon( pChar );
			CreateMapEffect( pChar, pStage );
		}
		
	}
}

void ioCircleReserveAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if ( !pChar )	
		return;

	EnableRefreshFireTimeList( false );

	ioAttackSkill::AttackSkillFire( pChar );

	m_dwReserveWeaponFireStartTime = FRAMEGETTIME() + GetAttribute().m_dwPreDelay;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID != -1 )
	{
		float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
		m_dwReserveWeaponFireStartTime += fFireTime * GetAttribute().m_fAttackAniRate;
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_WEAPON_RESERVE;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleReserveAttackSkill::CallReserveWeapon( ioBaseChar *pChar )
{
	if ( !pChar )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iWeaponCount = GetAttribute().m_vWeaponInfoList.size();
	if ( iWeaponCount == 0 )
		return;

	if ( m_vCirclePos == ioMath::VEC3_ZERO )
		return;

	D3DXVECTOR3 vPlayerPos = pChar->GetWorldPosition();
	float fDist = D3DXVec3Length( &( m_vCirclePos - vPlayerPos ) ) ;
	float fLength = fDist - m_fDefaultCircleOffSet;
	if ( fLength <= 0.0f )
		fLength = 0.0f;
	else
		fLength = fLength / iWeaponCount;

	for( int i=0 ; i < (int)iWeaponCount; ++i )
	{
		D3DXVECTOR3 vWeaponPos = vPlayerPos + ( m_vMoveDir * ( ( fLength * (i+1) ) + m_fDefaultCircleOffSet ) );
		g_WeaponMgr.SetReserveCallWeaponInfo( pChar->GetCharName(),
			GetAttribute().m_vWeaponInfoList[i],
			vWeaponPos,
			m_vMoveDir,
			dwCurTime + GetAttribute().m_vWeaponInfoList[i].m_dwCallTime,
			m_dwWeaponBaseIndex++ );
	}
}

void ioCircleReserveAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioAttackSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCircleReserveAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar || !pStage )
		return;
	int iType;
	rkPacket >> iType;
	if ( iType == SST_WEAPON_RESERVE )
		rkPacket >> m_vCirclePos;
	else if ( iType == SST_RANDOM_WEAPON_RESERVE )
	{
		int iWeaponCount = GetAttribute().m_vWeaponInfoList.size();
		if ( iWeaponCount == 0 )
			return;
		int iPosSize;
		rkPacket >> iPosSize;
		std::vector<D3DXVECTOR3> vPosList;
		for ( int i=0 ; i< iPosSize ; ++i )
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			vPosList.push_back( vPos );
		}
		rkPacket >> m_vMoveDir;
		rkPacket >> m_dwWeaponBaseIndex;

		DWORD dwCurTime = FRAMEGETTIME();
		for( int i=0 ; i < iWeaponCount; ++i )
		{
			if ( iPosSize > i )
			{
				g_WeaponMgr.SetReserveCallWeaponInfo( pChar->GetCharName(),
					GetAttribute().m_vWeaponInfoList[i],
					vPosList[i],
					m_vMoveDir,
					dwCurTime + GetAttribute().m_vWeaponInfoList[i].m_dwCallTime,
					m_dwWeaponBaseIndex++ );
			}
		}
	}
}

void ioCircleReserveAttackSkill::CallReserveRandomPositionWeapon( ioBaseChar *pChar )
{
	if ( !pChar )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iWeaponCount = GetAttribute().m_vWeaponInfoList.size();
	if ( iWeaponCount == 0 )
		return;


	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPlayerPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vStartDirPos = vPlayerPos;
	vStartDirPos.y += m_fWeaponStartHeight;
	D3DXVECTOR3 vEndDirPos = vPlayerPos + vDir * m_fDefaultCircleOffSet;

	m_vMoveDir = vEndDirPos - vStartDirPos;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir);

	DWORD dwStartBaseIndex = m_dwWeaponBaseIndex;
	std::vector<D3DXVECTOR3> vPosList;
	for( int i=0 ; i < iWeaponCount; ++i )
	{
		IORandom random;
		random.SetRandomSeed( dwCurTime + i );
		D3DXVECTOR3 vWeaponPos;

		int iAngle = random.Random( 360 + i );
		iAngle = (int)ioMath::ArrangeHead( (float)iAngle );
		int iRange = (int)m_fRandomRange;
		if (iRange < 0)
			iRange = 0;
		iRange = random.Random(iRange);

		int iXPos = cos((double) iAngle ) * iRange;
		int iZPos = sin((double) iAngle ) * iRange;

		vWeaponPos = vPlayerPos;
		vWeaponPos.x += iXPos;
		vWeaponPos.y += m_fWeaponStartHeight;
		vWeaponPos.z += iZPos;
		vPosList.push_back( vWeaponPos );

		g_WeaponMgr.SetReserveCallWeaponInfo( pChar->GetCharName(),
			GetAttribute().m_vWeaponInfoList[i],
			vWeaponPos,
			m_vMoveDir,
			dwCurTime + GetAttribute().m_vWeaponInfoList[i].m_dwCallTime,
			m_dwWeaponBaseIndex++ );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		int vPosSize = vPosList.size();
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_RANDOM_WEAPON_RESERVE;
		kPacket << vPosSize;
		for( int i=0 ; i < vPosSize; ++i )
			kPacket << vPosList[i];
		kPacket << m_vMoveDir;
		kPacket << dwStartBaseIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleReserveAttackSkill::CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	vPos.y = pChar->GetBottomHeight();
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szFireEffect, vPos, pChar->GetWorldScale() );
	if( pMapEffect )
		pMapEffect->SetWorldOrientation( pChar->GetWorldOrientation() );
}
