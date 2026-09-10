#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioModeItem.h"

#include "ioNpcChar.h"

ioModeItem::ioModeItem()
{
	m_pCreator = NULL;
	m_vPosition = ioMath::VEC3_ZERO;
	m_dwID = -1;
	m_bLive = true;
	m_bSyncWait = false;
	m_bAttach = false;
	m_bVisible = false;
}

ioModeItem::ioModeItem( const ioModeItem &rhs )
: m_iType( rhs.m_iType ),
  m_MeshEffectName( rhs.m_MeshEffectName ),
  m_DestroyEffectName( rhs.m_DestroyEffectName ),
  m_fCollisionRange( rhs.m_fCollisionRange ),
  m_CreateSoundName( rhs.m_CreateSoundName ),
  m_DestroySoundName( rhs.m_DestroySoundName ),
  m_fHeightOffset( rhs.m_fHeightOffset ),
  m_fZoneRange( rhs.m_fZoneRange ),
  m_fUpHeight( rhs.m_fUpHeight ),
  m_fUnderHeight( rhs.m_fUnderHeight ),
  m_fWidth( rhs.m_fWidth )
{
	m_vPosition = ioMath::VEC3_ZERO;
	m_dwID = -1;
	m_bLive = true;
	m_bSyncWait = false;
	m_bAttach = false;
	m_bVisible = false;
}

ioModeItem::~ioModeItem()
{

}

void ioModeItem::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	m_iType = rkLoader.LoadInt_e( "item_type", MIT_NONE );

	rkLoader.LoadString_e( "mesh_effect", "", szBuf, MAX_PATH );
	m_MeshEffectName = szBuf;

	rkLoader.LoadString_e( "destroy_effect", "", szBuf, MAX_PATH );
	m_DestroyEffectName = szBuf;

	m_fCollisionRange = rkLoader.LoadFloat_e( "collision_range", FLOAT100 );

	rkLoader.LoadString_e( "create_sound", "", szBuf, MAX_PATH );
	m_CreateSoundName = szBuf;

	rkLoader.LoadString_e( "destroy_sound", "", szBuf, MAX_PATH );
	m_DestroySoundName = szBuf;	
	
	m_fHeightOffset = rkLoader.LoadFloat_e( "height_offset", 0.0f );
	m_fZoneRange	= rkLoader.LoadFloat_e( "zone_range", 0.0f );
	m_fUpHeight		= rkLoader.LoadFloat_e( "zone_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "zone_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "zone_width", 0.0f );
}

ioModeItem *ioModeItem::Clone()
{
	return new ioModeItem( *this );
}

void ioModeItem::OnCreate()
{
	if( !m_pCreator )
		return;

	ioMapEffect *pMapEffect = m_pCreator->CreateMapEffect( m_MeshEffectName, GetMidPositionByOffset(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		m_dwMeshEffectID = pMapEffect->GetUniqueID();
		m_bVisible = true;

		ioEffect *pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			if( !m_CreateSoundName.IsEmpty() )
			{				
				g_SoundMgr.PlaySound( m_CreateSoundName, pEffect->GetParentSceneNode() );
			}
		}
	}
}

void ioModeItem::OnDestroy()
{
	if( !m_pCreator )
		return;

	if( m_dwMeshEffectID != -1 )
	{
		m_pCreator->DestroyMapEffect( m_dwMeshEffectID );
		m_dwMeshEffectID = -1;
	}

	D3DXVECTOR3 vPos = m_vPosition;
	vPos.y += m_fHeightOffset;

	ioMapEffect *pMapEffect = m_pCreator->CreateMapEffect( m_DestroyEffectName, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		ioEffect *pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			if( !m_DestroySoundName.IsEmpty() )
			{				
				g_SoundMgr.PlaySound( m_DestroySoundName, pEffect->GetParentSceneNode() );
			}
		}
	}	
}

void ioModeItem::SetItemValue( DWORD dwIndex, float fXPos, float fZPos )
{
	SetUniqueID( dwIndex );

	float fHeight = m_pCreator->GetMapHeight( fXPos, fZPos, NULL );
	D3DXVECTOR3 vPos( fXPos, fHeight, fZPos );	
	SetPosition( vPos );
}

void ioModeItem::Process()
{
	UpdateAttackBox();
}

bool ioModeItem::IsCollision( ioBaseChar *pChar )
{
	if( !m_pCreator )
		return false;

	if( !pChar )
		return false;

	if( !pChar->IsNeedProcess() )
		return false;

	if( IsNpcChar( pChar ) )
		return false;

	if ( pChar->IsSystemState() )
		return false;

	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - m_vPosition;
	float fLength = D3DXVec3Length( &vDiff );
	if( fLength > m_fCollisionRange )
		return false;
	
	SendGetModeItem( pChar );
	return true;
}

void ioModeItem::SetPosition( D3DXVECTOR3 vPos )
{
	m_vPosition = vPos;

	if( !m_pCreator )
		return;

	if( m_dwMeshEffectID != -1  )
	{
		ioMapEffect *pEffect = m_pCreator->FindMapEffect( m_dwMeshEffectID );
		if( pEffect )
		{
			vPos.y += m_fHeightOffset;
			pEffect->SetWorldPosition( vPos );
		}
	}
}

D3DXVECTOR3 ioModeItem::GetPosition() const
{ 
	return m_vPosition; 
}

D3DXVECTOR3 ioModeItem::GetMidPositionByOffset() const
{
	D3DXVECTOR3 vPos = m_vPosition;
	vPos.y += m_fHeightOffset;
	
	return vPos;
}

void ioModeItem::SendGetModeItem( ioBaseChar *pChar )
{
	if( pChar && pChar->IsNeedSendNetwork() && !m_bSyncWait )
	{
		m_bSyncWait = true;

		SP2Packet kPacket(CTPK_GET_MODE_ITEM);
		kPacket << g_MyInfo.GetUserIndex();
		kPacket << GetUniqueID();
		TCPNetwork::SendToServer( kPacket );
	}
}

bool ioModeItem::IsAttach()
{
	return m_bAttach;
}

void ioModeItem::UpdateAttackBox()
{	
	if( m_fZoneRange == 0.0f && m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f && m_fWidth == 0.0f )
	{
		m_bAttach = false;
		return;
	}
	
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = GetMidPositionByOffset();
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fZoneRange;

	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );
	kBuffBox.Transform( GetMidPositionByOffset(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );		
	kBuffBox.SetCenter( GetMidPositionByOffset() );
	m_AttackBox = kBuffBox;
	m_bAttach = true;
}

const ioOrientBox& ioModeItem::GetAttackBox() const
{
	return m_AttackBox;
}

bool ioModeItem::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	return true;
}