#include "stdafx.h"

#include "ioBlock.h"
#include "ioBlockWorld.h"
#include "../WeaponDefine.h"

ioBlock::ioBlock( ioEntityGroup *pGrp, ioPlayMode *pMode, ioBlockProperty* pProperty, __int64 nUniqueIndex )
	: ioPlayEntity( pGrp, pMode ),
	m_Property( pProperty ),
	m_nUniqueIndex( nUniqueIndex ),
	m_CollRot( ioMath::QUAT_IDENTITY ),
	m_fRotate( 0.0f ),
	m_ColorSetState( CSS_NONE ),
	m_FadeState( FS_OUT ),
	m_fCurrFadeRate( FLOAT1 ),
	m_bCollChecked( false ),
	m_bDisableInterPlay( false ),
	m_bDisableAreaWeapon( false )
{
	SetTitleCoord( 0, 0, 0, false );
	BackUpColor( m_BackUpColor );
	ZeroMemory( &m_FullTransform, sizeof( D3DXMATRIX ) );
}

ioBlock::~ioBlock()
{
}

void ioBlock::SetTitleCoord( int XPos, int YPos, int ZPos, bool bCallUdateBound /* = true */ )
{
	m_Pos.x = XPos;
	m_Pos.y = YPos;
	m_Pos.z = ZPos;

	D3DXVECTOR3 vPos;
	TileCoordToWorldCoord( XPos, YPos, ZPos, vPos.x, vPos.y, vPos.z );

	SetWorldPosition( vPos );
	UpdateCollBox();
	CreateInterPlayZone();
	CreateAreaWeapon();
	m_FullTransform = *GetFullTransform();
		
	//매 프레임마다 UpdateBound()를 호출 하는 비용이 크기 떄문에 위치 변화 될때만 호출
	if( bCallUdateBound && m_pGroup && m_pGroup->GetParentSceneNode() )		
		m_pGroup->GetParentSceneNode()->UpdateBound( true );	
}

void ioBlock::UpdateBound()
{	
	if( m_pGroup && m_pGroup->GetParentSceneNode() )
	{
		m_pGroup->GetParentSceneNode()->SetDisableUpdateBound( true );	
	}
}

void ioBlock::SetCollBoxRotaion( D3DXQUATERNION qtRot )
{
	m_CollRot = qtRot;
}

void ioBlock::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	
	if( m_pGroup )
	{
		m_pGroup->Update( fSecPerTime );
	}

	UpdateFade();
}

void ioBlock::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

bool ioBlock::IsMountAvailable() const
{
	return true;
}

bool ioBlock::IsMountingAvailable() const
{
	return false;
}

bool ioBlock::IsNowMountEnable() const
{
	return true;
}

void ioBlock::Translate( const D3DXVECTOR3 &vMove )
{
	ioPlayEntity::Translate( vMove );
	UpdateCollBox();
	CreateInterPlayZone();
	CreateAreaWeapon();
}

void ioBlock::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

int ioBlock::GetCellPos() const
{
	return GetTilePos().x + GetTilePos().z * g_BlockManager.GetTileMax();
}

int ioBlock::GetCellLevel() const
{
	return GetTilePos().y;
}

ApplyWeaponExp ioBlock::ApplyWeapon( ioWeapon *pWeapon )
{
	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT		||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT	||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2		||
		pWeapon->GetType() == ioWeapon::WT_CONTACT3 )
		bContactType = true;

	if( pWeapon )
	{
		ioBaseChar *pAttacker = pWeapon->GetOwner();

		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	if( !pWeapon || bContactType )
		return AWE_NO_EXPLOSION;

	if( pWeapon->HasPeculiarity( WP_MISSILE_WEAPON ) )
		return AWE_MISSILE_EXPLOSION;

	return AWE_EXPLOSION;
}

bool ioBlock::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	if( ToBoundBombWeapon( pWeapon ) )
		return false;

	return true;
}

ioOpcodeShape* ioBlock::GetCollShape()
{
	if( m_pGroup )
		return m_pGroup->GetOpcodeShape();

	return NULL;
}

const ioSphere& ioBlock::GetCollSphere()
{
	return m_CollSphere;
}

const ioSphere& ioBlock::GetCollBlockSphere()
{
	return m_CollBlockSphere;
}

ioPlayEntity::PlaySubType ioBlock::GetSubType() const
{
	return PST_HOUSING_BLOCK;
}

ArmorType ioBlock::GetArmorType() const
{
	return AT_UNARMOR;
}

int ioBlock::DontMoveEntityLevel() const
{
	return 5;
}

void ioBlock::Rotate( float fRotate )
{
	m_fRotate = fRotate;
	m_fRotate = ioMath::ArrangeHead( m_fRotate );
	
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot, DEGtoRAD( fRotate ), 0.0f, 0.0f );	
	SetWorldOrientation( qtRot );

	UpdateCollBox();
	CreateInterPlayZone();
	CreateAreaWeapon();
	m_FullTransform = *GetFullTransform();

	//매 프레임마다 UpdateBound()를 호출 하는 비용이 크기 떄문에 위치 변화 될때만 호출
	if( m_pGroup && m_pGroup->GetParentSceneNode() )
		m_pGroup->GetParentSceneNode()->UpdateBound( true );	
}

void ioBlock::RotateCoord( IN float fRotate, IN TilePosList& List ) const
{
	if( fRotate == 0.0f )
		return;

	for_each( List.begin(), List.end(), bind2nd( std::mem_fun_ref( &TilePos::ToRotatePos ), fRotate ) );
}

void ioBlock::UpdateCollBox()
{
	if( m_Property == NULL )
		return;

	m_TileCollBoxList.clear();

	m_CurrPosList.clear();
	m_Property->GetCollTileList( m_CurrPosList );
	RotateCoord( m_fRotate, m_CurrPosList );

	//CollList를 월드 기준으로 변환
	for_each( m_CurrPosList.begin(), m_CurrPosList.end(), bind2nd( std::mem_fun_ref( &TilePos::ToWorldPos ), m_Pos ) );

	UpdateCollBox( m_CurrPosList, m_TileCollBoxList, m_CollSphere, m_CollBlockSphere );
}

void ioBlock::UpdateCollBox( IN TilePosList& CollList, OUT ioOrientBoxList& CollBoxList, OUT ioSphere& CollSphere, OUT ioSphere& CollBlockSphere ) const
{
	float fDistMax = 0.0f;
	for( int i = 0; i < (int)CollList.size(); ++i )
	{
		TilePos Pos = CollList[i];
		D3DXVECTOR3 vBoxPos;
		TileCoordToWorldCoord( Pos.x, Pos.y, Pos.z, vBoxPos.x, vBoxPos.y, vBoxPos.z );

		vBoxPos.y += UD_SIZE * FLOAT05;

		D3DXVECTOR3 vMin, vMax;
		vMax = vMin = vBoxPos;

		vMin.x -= ( UD_SIZE * FLOAT05 );
		vMax.x += ( UD_SIZE * FLOAT05 );

		vMin.y -= UD_SIZE * FLOAT05;
		vMax.y += UD_SIZE * FLOAT05;

		vMin.z -= UD_SIZE * FLOAT05;
		vMax.z += UD_SIZE * FLOAT05;

		ioOrientBox kBox;
		kBox.SetBoxByMinMax( vMin, vMax );
		kBox.Transform( vBoxPos, ioMath::UNIT_ALL, m_CollRot );
		kBox.SetCenter( vBoxPos );
		CollBoxList.push_back( kBox );

		D3DXVECTOR3 vDiff = vBoxPos;
		float fDiff = D3DXVec3Length( &vDiff );

		if( fDistMax < fDiff )
			fDistMax = fDiff;
	}

	if( m_pGroup )
	{
		CollSphere = m_pGroup->GetCollisionSphere();
		CollSphere.SetCenter( GetWorldPosition() );

		CollBlockSphere = ioSphere( GetWorldPosition(), fDistMax );
	}
}

void ioBlock::GetCollBoxList( ioOrientBoxList& CollList )
{
	CollList = m_TileCollBoxList;
}

void ioBlock::GetCollInterPlayBoxList( ioOrientBoxList& CollList )
{
	CollList = m_InterPlayBoxList;
}

void ioBlock::GetPosList( TilePosList& PosList ) const
{
	PosList = m_CurrPosList;
}

bool ioBlock::TestIntersection( IN const ioRay& kYRay, OUT D3DXVECTOR3* pCol )  const
{
	for( int i = 0; i < (int)m_TileCollBoxList.size(); ++i )
	{
		const ioOrientBox& kTileColBox = m_TileCollBoxList[i];
		if( ioMath::TestIntersection( kYRay, kTileColBox ) )
		{
			if( pCol )
			{
				*pCol	= kTileColBox.GetCenter();
				pCol->y	+= kTileColBox.GetExtents(2);
			}
			return true;
		}
	}

	return false;
}

bool ioBlock::TestIntersection( IN const ioOrientBox& kBox ) const
{
	for( int i = 0; i < (int)m_TileCollBoxList.size(); ++i )
	{
		const ioOrientBox& kTileColBox = m_TileCollBoxList[i];
		if( ioMath::TestIntersection( kTileColBox, kBox, NULL ) )
			return true;
	}

	return false;
}

bool ioBlock::TestIntersection( IN const TilePos& Pos ) const 
{
	if( m_Pos == Pos )
		return true;

	TilePosList List;
	GetPosList( List );

	for( int i = 0; i < (int)List.size(); ++i )
	{
		if( List[i] == Pos )
			return true;
	}

	return false;
}

bool ioBlock::TestIntersection( IN const TilePosList& List ) const
{
	for( int i = 0; i < (int)List.size(); ++i )
	{
		if( TestIntersection( List[i] ) )
			return true;
	}

	return false;
}

bool ioBlock::TestPreIntersection( IN const ioBlock* pBlock, IN const TilePos& NextPos, IN float fNextRotate ) const
{
	TilePosList List;
	m_Property->GetCollTileList( List );
	RotateCoord( fNextRotate, List );

	//월드 타일 좌표계로 변환
	for_each( List.begin(), List.end(), bind2nd( std::mem_fun_ref( &TilePos::ToWorldPos ), m_Pos ) );

	for( int i = 0; i < (int)List.size(); ++i )
	{
		TilePos Pos = NextPos + List[i];
		if( pBlock->TestIntersection( Pos ) )
			return true;
	}

	return false;
}

bool ioBlock::IsSetUp() const
{
	TilePosList List;
	GetPosList( List );

	for( int i = 0; i < (int)List.size(); ++i )
	{
		TilePos& Pos = List[i];
		if( !COMPARE( Pos.x, 0, g_BlockManager.GetTileMax() ) || !COMPARE( Pos.y, 0, g_BlockManager.GetHeightMax() ) || !COMPARE( Pos.z, 0, g_BlockManager.GetTileMax() ) )
		{
			return false;
		}
	}

	return true;
}

ioBlock::ColorSetState ioBlock::IsColorState()
{
	return  m_ColorSetState;
}

void ioBlock::ChangeDisable()
{
	//해당 블럭이 그림자가 드리워지는 메테리얼(쉐이더)이 비활성화 되있다면
	//컬러 적용을 위해 잠시 활성화
	if( GetProperty()->GetShadowType() == ioBlockProperty::ST_RECIVE )
	{
		SetShadowRecvEnable( true );		
	}

	m_ColorSetState = CSS_DISABLE;

	ColorSet kColorSet;
	kColorSet.kAmbientColor  = m_Property->GetDisableColor();
	kColorSet.kDiffuseColor  = m_Property->GetDisableColor();
	kColorSet.kSpecularColor = m_Property->GetDisableColor();
	kColorSet.kEmissiveColor = m_Property->GetDisableColor();

	ChangeColor( kColorSet );
	SetWorldScale( D3DXVECTOR3( 1.01f, 1.01f, 1.01f ) ); // Z-FIGHTING 방지
}

void ioBlock::ChangeSelect()
{
	//해당 블럭이 그림자가 드리워지는 메테리얼(쉐이더)이 비활성화 되있다면
	//컬러 적용을 위해 잠시 활성화
	if( GetProperty()->GetShadowType() == ioBlockProperty::ST_RECIVE )
	{	
		SetShadowRecvEnable( true );		
	}

	m_ColorSetState = CSS_SELECT;

	ColorSet kColorSet;
	kColorSet.kAmbientColor  = m_Property->GetSelectColor();
	kColorSet.kDiffuseColor  = m_Property->GetSelectColor();
	kColorSet.kSpecularColor = m_Property->GetSelectColor();
	kColorSet.kEmissiveColor = m_Property->GetSelectColor();

	ChangeColor( kColorSet );
}

void ioBlock::ChangeNone()
{
	//원복하기
	if( GetProperty()->GetShadowType() == ioBlockProperty::ST_RECIVE )
	{
		if( Setting::UseShadow() )
			SetShadowRecvEnable( true );
		else
			SetShadowRecvEnable( false );
	}

	m_ColorSetState = CSS_NONE;

	ChangeColor( m_BackUpColor );
	SetWorldScale( D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
}

void ioBlock::ChangeColor( IN const ColorSet& kColor )
{
	int iEntityCnt = m_pGroup->GetNumEntities();
	for( int i=0 ; i<iEntityCnt ; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );
		if( !pEntity )
			continue;

		int iSubEntityCnt = pEntity->GetNumSubEntity();
		for( int j = 0; j < iSubEntityCnt ; j++ )
		{
			ioMaterial *pMtl = NULL;
			if( pEntity->GetSubEntity( j ) )
				pMtl = pEntity->GetSubEntity( j )->GetMaterial();

			if( pMtl )
			{
				pMtl->SetAmbient( kColor.kAmbientColor );
				pMtl->SetDiffuse( kColor.kDiffuseColor );
				pMtl->SetSpecular( kColor.kSpecularColor );
				pMtl->SetEmissive( kColor.kEmissiveColor );
			}
		}
	}
}

void ioBlock::BackUpColor( OUT ColorSet& kBackUpColor )
{
	int iEntityCnt = m_pGroup->GetNumEntities();
	for( int i=0 ; i<iEntityCnt ; i++ )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );
		if( !pEntity )
			continue;

		int iSubEntityCnt = pEntity->GetNumSubEntity();
		for( int j = 0; j < iSubEntityCnt ; j++ )
		{
			ioMaterial *pMtl = NULL;
			if( pEntity->GetSubEntity( j ) )
				pMtl = pEntity->GetSubEntity( j )->GetMaterial();

			if( pMtl )
			{
				kBackUpColor.kAmbientColor	= pMtl->GetAmbient();
				kBackUpColor.kDiffuseColor	= pMtl->GetDiffuse();
				kBackUpColor.kSpecularColor	= pMtl->GetSpecular();
				kBackUpColor.kEmissiveColor	= pMtl->GetEmissive();
			}
		}
	}
}

void ioBlock::FadeIn()
{
	if( m_FadeState != FS_IN )
	{
		m_FadeState		= FS_IN;
		m_dwRateTime	= FRAMEGETTIME();
		SetShadowRecvEnable( false );
	}
}

void ioBlock::FadeOut()
{
	if( m_FadeState != FS_OUT )
	{
		m_FadeState		= FS_OUT;
		m_dwRateTime	= FRAMEGETTIME();
	}
}

void ioBlock::UpdateFade()
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_FadeState )
	{
	case FS_OUT:
		{
			float fRate = min( FLOAT1, (float)(dwCurTime - m_dwRateTime ) / FLOAT500 );
			fRate = max( 0.1f, fRate );
			if( fRate == FLOAT1 && Setting::UseShadow() )
			{
				SetShadowRecvEnable( true );
			}
			SetAlphaRateDirect( fRate * 255 );
		}
		break;
	case FS_IN:
		{
			float fRate = FLOAT1 - min( FLOAT1, (float)(dwCurTime - m_dwRateTime ) / FLOAT500 );
			fRate = max( 0.1f, fRate );
			SetAlphaRateDirect( fRate * 255 );
		}
		break;
	}
}

void ioBlock::SetShadowCastEnable( bool bCast )
{
	if( m_pGroup )
		m_pGroup->SetShadowCastEnable( bCast );
}

void ioBlock::SetShadowRecvEnable( bool bRecv )
{
	if( !m_pGroup )
		return;
	
	m_pGroup->SetShadowRecvEnable( bRecv );
}

void ioBlock::AddShadowRecvPass()
{
	int iEntityCnt = m_pGroup->GetNumEntities();
	for( int i = 0; i < iEntityCnt; ++i )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );
		int iSubEntityCnt = pEntity->GetNumSubEntity();
		for( int j = 0; j < iSubEntityCnt; ++j )
		{
			ioTechnique *pTech = pEntity->GetSubEntity( j )->GetTechnique();
			if( !pTech )
				continue;

			ioPass *pPass = pTech->GetPass( 0 );
			if( pPass && pPass->HasRenderShadowShader() )
				pPass->AddPassProperty( ioPass::PP_SHADOW_RECV_PASS );
		}
	}
}

void ioBlock::EnableMapEdge( bool bEnable )
{
	if( m_pGroup )
		m_pGroup->EnableEdgeRender( bEnable );
}

void ioBlock::StopAllTextureEffect( bool bStop )
{
	int iEntityCnt = m_pGroup->GetNumEntities();
	for( int i = 0; i < iEntityCnt; ++i )
	{
		ioEntity *pEntity = m_pGroup->GetEntity( i );
		int iSubEntityCnt = pEntity->GetNumSubEntity();
		for( int j = 0 ; j < iSubEntityCnt ; ++j )
		{
			ioMaterial *pMaterial = pEntity->GetSubEntity( j )->GetMaterial();
			if( pMaterial )
			{
				pMaterial->StopAllTextureEffect( bStop );
			}
		}
	}
}

void ioBlock::SetRenderQueueGroup( RenderGroupID eGroupID )
{
	if( !m_pGroup )
		return;

	m_pGroup->SetRenderQueueGroup( eGroupID );
}

void ioBlock::DisableInterPlay()
{
	m_bDisableInterPlay = true;
}

void ioBlock::CreateInterPlayZone()
{
	ioBlockProperty* pProperty = GetProperty();
	if( !pProperty )
		return;

	if( m_bDisableInterPlay )
		return;

	m_InterPlayBoxList.clear();
	const ioBlockProperty::InterPlayInfoList& List = pProperty->GetInterPlayInfoList();
	for( int i = 0; i < (int)List.size(); ++i )
	{
		const ioBlockProperty::InterPlayInfo& Info = List[i];
		
		D3DXVECTOR3 vMin, vMax;
		vMin = vMax = ioMath::VEC3_ZERO;
		vMin.x -= ( Info.m_fWidth * FLOAT05 );
		vMin.y -= Info.m_fUnderHeight;
		vMax.x += ( Info.m_fWidth * FLOAT05 );
		vMax.y += Info.m_fUpHeight;
		vMax.z += Info.m_fZoneRange;

		// 사각형 생성
		ioOrientBox kBox;		
		kBox.SetBoxByMinMax( vMin, vMax );
		kBox.Transform( ioMath::VEC3_ZERO, ioMath::UNIT_ALL, GetWorldOrientation() * GetCollRotate() );

		D3DXVECTOR3 vOffset = ioMath::VEC3_ZERO;
		if( Info.m_vZoneOffset != ioMath::VEC3_ZERO )
		{
			ioPlayEntityHelp::MoveOffset( Info.m_vZoneOffset, GetWorldOrientation() * GetCollRotate(), vOffset );
		}
		kBox.SetCenter( GetWorldPosition() + vOffset );
		m_InterPlayBoxList.push_back( kBox );
	}
}

bool ioBlock::TestInterPlayIntersection( IN ioPlayEntity* pEntity, OUT int& nCollIndex, OUT D3DXVECTOR3& vCollPos ) const
{
	if( pEntity == NULL )
		return false;
	
	if( m_bDisableInterPlay )
		return false;

	for( int i = 0; i < (int)m_InterPlayBoxList.size(); ++i )
	{
		if( ioMath::TestIntersection( pEntity->GetWorldCollisionBox(), m_InterPlayBoxList[i], NULL ) )
		{
			nCollIndex	= i;			
			vCollPos	= m_InterPlayBoxList[i].GetCenter();
			return true;
		}
	}

	return false;
}

void ioBlock::DisableAreaWeapon()
{
	m_bDisableInterPlay = true;

	DestroyAreaWeapon();
}

void ioBlock::DestroyAreaWeapon()
{
	for( int i = 0; i < (int)m_LiveAreaWeaponList.size(); ++i )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_LiveAreaWeaponList[i] );
	}
	m_LiveAreaWeaponList.clear();
}

void ioBlock::CreateAreaWeapon()
{
	ioBlockProperty* pProperty = GetProperty();
	if( !pProperty )
		return;

	DestroyAreaWeapon();

	if( m_bDisableAreaWeapon )
		return;

	const ioBlockProperty::BlockAreaWeaponList& List = pProperty->GetBlockAreaWeaponList();
	for( int i = 0; i < (int)List.size(); ++i )
	{
		const ioBlockProperty::BlockAreaWeapon& Info = List[i];

		float fAngle = Info.m_fCreateAngle;
		D3DXQUATERNION qtRot;		
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
		D3DXVECTOR3 vOffset = GetWorldPosition() + ( qtRot * Info.m_vPos );
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( "", "", "", Info.m_AreaWeaponName, vOffset, qtRot, ioAreaWeapon::CT_NORMAL );
		if( pAreaWeapon )
		{
			m_LiveAreaWeaponList.push_back( pAreaWeapon->GetAreaWeaponIndex() );
		}
	}
}

void ioBlock::RenderBlockAABB( CollRenderType eType, ioPlayStage* pStage, ioCamera* pCamera )
{
	D3DXVECTOR3 vVtxArray[8];
	ioOrientBoxList CollList;

	if( eType == RENDER_BOX )
		GetCollBoxList( CollList );
	else if( eType == RENDER_INTERPLAY_BOX )
		GetCollInterPlayBoxList( CollList );

	if( CollList.empty() )
		return;

	if( !pCamera || !pCamera->IsVisible( GetCollSphere() ) )
		return;

	for( ioOrientBoxList::iterator iter = CollList.begin(); iter != CollList.end(); ++iter )
	{
		const ioOrientBox& rkBox = *iter;
		switch( eType )
		{	
		case RENDER_BOX:
			{
				rkBox.GetVertices( vVtxArray );
				pStage->RenderBox( vVtxArray, 0xff0000ff );
			}
			break;
		case RENDER_INTERPLAY_BOX:
			{
				rkBox.GetVertices( vVtxArray );
				pStage->RenderBox( vVtxArray, 0xffff00ff );
			}
			break;
		case RENDER_SPHERE:
			{
				pStage->RenderSphere( rkBox.GetBoxRadius(), GetWorldOrientation(), GetMidPositionByRate(), GetWorldScale(), D3DCOLOR_XRGB( 0, 155, 255 ) );
			}
		}
	}
}

void ioBlock::TestBlock( IN const TilePosList& SrcPosList )
{
	int iSize = 0;
	if( SrcPosList.size() <= m_CurrPosList.size() )
		iSize = m_CurrPosList.size();
	else
		iSize = SrcPosList.size();
	
	int nError = 0;
	for( int i = 0; i < iSize; ++i )
	{		
		if( !COMPARE( i, 0, (int)SrcPosList.size() ) )
		{
			LOG.PrintTimeAndLog(0, "[TEST BLOCK POS] Out of Position(Server) : %d(Size : %d, Try Index : %d)", m_Property->GetCode(), SrcPosList.size(), i );
			nError++;
			continue;;
		}

		if( !COMPARE( i, 0, (int)m_CurrPosList.size() ) )
		{
			LOG.PrintTimeAndLog(0, "[TEST BLOCK POS] Out of Position(Client) : %d(Size : %d, Try Index : %d)", m_Property->GetCode(), m_CurrPosList.size(), i );
			nError++;
			continue;;
		}

		if( SrcPosList[i] != m_CurrPosList[i] )
		{
			nError++;
			LOG.PrintTimeAndLog(0, "[TEST BLOCK POS] not match : %d( %d, %d, %d != %d, %d, %d)", m_Property->GetCode(), 
																								SrcPosList[i].x,
																								SrcPosList[i].y,
																								SrcPosList[i].z,
																								m_CurrPosList[i].x,
																								m_CurrPosList[i].y,
																								m_CurrPosList[i].z );
		}
	}

	if( nError == 0 )
	{
		LOG.PrintTimeAndLog(0, "[TEST BLOCK POS] %d - perfect match", m_Property->GetCode() );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "[TEST BLOCK POS] %d - miss match : %d", m_Property->GetCode(), nError );
	}
}