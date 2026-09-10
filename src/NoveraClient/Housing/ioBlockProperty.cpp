#include "stdafx.h"
#include "ioBlockProperty.h"

ioBlockProperty::ioBlockProperty()
{
}

ioBlockProperty::ioBlockProperty( const ioBlockProperty& rhs ) :
	m_nCode( rhs.m_nCode ),
	m_nOrder( rhs.m_nOrder ),
	m_eShadowType( rhs.m_eShadowType ),
	m_eItemCategory( rhs.m_eItemCategory ),
	m_eBlockAlphaType( rhs.m_eBlockAlphaType ),
	m_Name( rhs.m_Name ),
	m_MeshFileName( rhs.m_MeshFileName ),
	m_IconName( rhs.m_IconName ), 
	m_Rotate( rhs.m_Rotate ),
	m_CollTilePosList( rhs.m_CollTilePosList ),
	m_nWidth( 0 ),
	m_nDepth( 0 ),
	m_nHeight( 0 ),
	m_InterPlayInfoList( rhs.m_InterPlayInfoList ),
	m_BlockAreaWeaponList( rhs.m_BlockAreaWeaponList ) 
{

}

ioBlockProperty::~ioBlockProperty()
{
}

bool ioBlockProperty::LoadBlockProperty( ioINILoader &rkLoader )
{
	m_nOrder = rkLoader.LoadInt( "order", 10000000 );

	char szBuf[MAX_PATH];

	rkLoader.LoadString( "name", "", szBuf, MAX_PATH );
	m_Name = szBuf;

	rkLoader.LoadString( "mesh_file", "", szBuf, MAX_PATH );
	m_MeshFileName = szBuf;
	
	rkLoader.LoadString( "icon_name", "", szBuf, MAX_PATH );
	m_IconName = szBuf;

	m_eShadowType		= (ShadowType)rkLoader.LoadInt( "shadow_type", ST_NONE );
	m_eItemCategory		= (ItemCategory)rkLoader.LoadInt( "item_category", IC_NONE );
	m_eBlockAlphaType	= (BlockAlphaType)rkLoader.LoadInt( "alpha_type", BAT_ON );

	//비활성 강조색
	m_DisableColor.r = rkLoader.LoadFloat( "disable_color_red", 255.0f )	/ 255.0f;
	m_DisableColor.g = rkLoader.LoadFloat( "disable_color_green", 255.0f )	/ 255.0f;
	m_DisableColor.b = rkLoader.LoadFloat( "disable_color_blue", 255.0f )	/ 255.0f;
	m_DisableColor.a = rkLoader.LoadFloat( "disable_color_alpha", 255.0f )	/ 255.0f;

	//선택 강조색
	m_SelectColor.r = rkLoader.LoadFloat( "select_color_red", 255.0f )		/ 255.0f;
	m_SelectColor.g = rkLoader.LoadFloat( "select_color_green", 255.0f )	/ 255.0f;
	m_SelectColor.b = rkLoader.LoadFloat( "select_color_blue", 255.0f )		/ 255.0f;
	m_SelectColor.a = rkLoader.LoadFloat( "select_color_alpha", 255.0f )	/ 255.0f;

	//회전값, 인 게임내 키보드 4방향 기준으로 만 회전
	int nRotate	= rkLoader.LoadInt( "rotate", 0 );
	if( COMPARE( nRotate, BR_UP, BR_MAX ) )
	{
		m_Rotate = nRotate;
	}
	else
	{
		return false;
	}

	//로컬 내부의 pivot기준으로의 배치될 영역
	int nMax = rkLoader.LoadInt( "collision_tile_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		TilePos Pos;
		sprintf_s_e( szBuf, "collision_tile%d_x", i+1 );
		Pos.x = rkLoader.LoadInt( szBuf, 0 );

		sprintf_s_e( szBuf, "collision_tile%d_y", i+1 );
		Pos.y = rkLoader.LoadInt( szBuf, 0 );

		sprintf_s_e( szBuf, "collision_tile%d_z", i+1 );
		Pos.z = rkLoader.LoadInt( szBuf, 0 );

		sprintf_s_e( szBuf, "collision_tile%d_event", i+1 );
		Pos.event = rkLoader.LoadInt( szBuf, 0 );

		m_CollTilePosList.push_back( Pos );
	}

	LoadInterPlay( rkLoader );
	LoadAreaWeapon( rkLoader );

	UpdateCollTileSize();
	return true;
}

void ioBlockProperty::LoadInterPlay( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//상호작용
	int nMax = rkLoader.LoadInt( "interplay_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		InterPlayInfo Info;
		sprintf_s_e( szKey, "interplay%d_type", i + 1 );
		Info.m_eInterPlayType = (InterPlayType)rkLoader.LoadInt( szKey, 0 );

		//PRE
		sprintf_s_e( szKey, "interplay%d_pre", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_PreAnimation = szBuf;

		sprintf_s_e( szKey, "interplay%d_pre_rate", i + 1 );
		Info.m_fPreAnimationRate = rkLoader.LoadFloat( szKey, 0 );

		//ACTION
		sprintf_s_e( szKey, "interplay%d_action", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_ActionAnimation = szBuf;

		sprintf_s_e( szKey, "interplay%d_action_rate", i + 1 );
		Info.m_fActionAnimationRate = rkLoader.LoadFloat( szKey, 0 );

		//LOOP
		sprintf_s_e( szKey, "interplay%d_loop", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_LoopAnimation = szBuf;

		sprintf_s_e( szKey, "interplay%d_loop_rate", i + 1 );
		Info.m_fLoopAnimationRate = rkLoader.LoadFloat( szKey, 0 );

		//END
		sprintf_s_e( szKey, "interplay%d_end", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_EndAnimation = szBuf;

		sprintf_s_e( szKey, "interplay%d_end_rate", i + 1 );
		Info.m_fEndAnimationRate = rkLoader.LoadFloat( szKey, 0 );

		//COLLISION
		sprintf_s_e( szKey, "interplay%d_width", i + 1 );
		Info.m_fWidth = rkLoader.LoadFloat( szKey, 0 );

		sprintf_s_e( szKey, "interplay%d_under_height", i + 1 );
		Info.m_fUnderHeight = rkLoader.LoadFloat( szKey, 0 );

		sprintf_s_e( szKey, "interplay%d_up_height", i + 1 );
		Info.m_fUpHeight = rkLoader.LoadFloat( szKey, 0 );

		sprintf_s_e( szKey, "interplay%d_zone_range", i + 1 );
		Info.m_fZoneRange = rkLoader.LoadFloat( szKey, 0 );

		//CHAR ROTATE
		sprintf_s_e( szKey, "interplay%d_char_rotate", i + 1 );
		Info.m_fCharRotate = rkLoader.LoadFloat( szKey, 0 );

		//COLLISION OFFSET
		sprintf_s_e( szKey, "interplay%d_zone_offset_x", i + 1 );
		Info.m_vZoneOffset.x = rkLoader.LoadFloat( szKey, 0 );
		sprintf_s_e( szKey, "interplay%d_zone_offset_y", i + 1 );
		Info.m_vZoneOffset.y = rkLoader.LoadFloat( szKey, 0 );
		sprintf_s_e( szKey, "interplay%d_zone_offset_z", i + 1 );
		Info.m_vZoneOffset.z = rkLoader.LoadFloat( szKey, 0 );

		//EMOTICON MENT
		sprintf_s_e( szKey, "interplay%d_emoticon_ment", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_EmoticonMent = szBuf;

		sprintf_s_e( szKey, "interplay%d_emoticon_ment_color", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		char *pColor = NULL;
		Info.m_EmoticonMentColor = strtoul( szBuf, &pColor, 16 );

		m_InterPlayInfoList.push_back( Info );
	}
}

void ioBlockProperty::LoadAreaWeapon( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	int nMax = rkLoader.LoadInt( "area_weapon_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		BlockAreaWeapon kAreaWeapon;

		sprintf_s_e( szKey, "area_weapon%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kAreaWeapon.m_AreaWeaponName = szBuf;
				
		sprintf_s_e( szKey, "area_weapon%d_angle", i + 1 );
		kAreaWeapon.m_fCreateAngle = rkLoader.LoadFloat( szKey, 0.0f );
				
		sprintf_s_e( szKey, "area_weapon%d_offset_x", i + 1 );
		kAreaWeapon.m_vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_s_e( szKey, "area_weapon%d_offset_y", i + 1 );
		kAreaWeapon.m_vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_s_e( szKey, "area_weapon%d_offset_z", i + 1 );
		kAreaWeapon.m_vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		m_BlockAreaWeaponList.push_back( kAreaWeapon );
	}
}

ioBlockProperty* ioBlockProperty::Clone()
{
	return new ioBlockProperty( *this );
}

void ioBlockProperty::GetCollTileList( OUT TilePosList& List )
{
	List = m_CollTilePosList;
}

void ioBlockProperty::UpdateCollTileSize()
{
	int nMin	= 0;
	int nMax	= 0;

	m_nWidth	= 0;
	m_nDepth	= 0;
	m_nHeight	= 0;

	//넓이
	for( int i = 0; i < (int)m_CollTilePosList.size(); ++i )
	{
		const TilePos& Pos = m_CollTilePosList[i];

		if( Pos.x < nMin )
			nMin = Pos.x;
		else if( Pos.x > nMax )
			nMax = Pos.x;
	}

	if( !m_CollTilePosList.empty() )
		m_nWidth = abs( nMin )  + abs( nMax ) + 1;

	//깊이
	for( int i = 0; i < (int)m_CollTilePosList.size(); ++i )
	{
		const TilePos& Pos = m_CollTilePosList[i];

		if( Pos.z < nMin )
			nMin = Pos.y;
		else if( Pos.z > nMax )
			nMax = Pos.z;
	}

	if( !m_CollTilePosList.empty() )
		m_nDepth = abs( nMin )  + abs( nMax ) + 1;

	//높이
	for( int i = 0; i < (int)m_CollTilePosList.size(); ++i )
	{
		const TilePos& Pos = m_CollTilePosList[i];

		if( Pos.y < nMin )
			nMin = Pos.y;
		else if( Pos.y > nMax )
			nMax = Pos.y;
	}

	if( !m_CollTilePosList.empty() )
		m_nHeight = abs( nMin )  + abs( nMax ) + 1;
}

const ioBlockProperty::InterPlayInfo* ioBlockProperty::GetInterPlayInfo( int nIndex ) const
{
	if( COMPARE( nIndex, 0, (int)m_InterPlayInfoList.size() ) )
		return &m_InterPlayInfoList[nIndex];

	return NULL;
}

const ioBlockProperty::InterPlayInfoList& ioBlockProperty::GetInterPlayInfoList() const
{
	return m_InterPlayInfoList;
}

const ioBlockProperty::BlockAreaWeaponList& ioBlockProperty::GetBlockAreaWeaponList() const
{
	return m_BlockAreaWeaponList;
}