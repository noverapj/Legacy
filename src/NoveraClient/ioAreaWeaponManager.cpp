
#include "stdafx.h"

#include "ioAreaWeaponManager.h"

template<> ioAreaWeaponManager* Singleton< ioAreaWeaponManager >::ms_Singleton = 0;

ioAreaWeaponManager::ioAreaWeaponManager( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
	m_vAreaWeaponList.clear();

	m_dwCurAreaWeaponIndex = 0;
}

ioAreaWeaponManager::~ioAreaWeaponManager()
{
	ClearAllAreaWeaponList();
}

ioAreaWeaponManager& ioAreaWeaponManager::GetSingleton()
{
	return Singleton< ioAreaWeaponManager >::GetSingleton();
}

void ioAreaWeaponManager::LoadAreaWeaponList()
{
	LoadAreaWeaponInfoList();
	LoadAreaWeaponNPCInfoList();
}

void ioAreaWeaponManager::LoadAreaWeaponInfoList()
{
	ioINILoader_e kLoader( "config/sp2_area_weapon.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "area_weapon_group_cnt", 0 );

	for( int i=0; i < iGroupCnt; ++i )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "area_weapon_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		ioINILoader kSetAreaWeaponLoader( szGroupName );
		kSetAreaWeaponLoader.SetTitle_e( "common_info" );

		int iCnt = kSetAreaWeaponLoader.LoadInt_e( "area_weapon_cnt", 0 );
		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szTitle, "area_weapon%d", j+1 );
			kSetAreaWeaponLoader.SetTitle( szTitle );

			ioAreaWeapon *pAreaWeapon = new ioAreaWeapon;

			if( pAreaWeapon )
			{
				pAreaWeapon->LoadProperty( kSetAreaWeaponLoader );

				if( pAreaWeapon->GetAreaWeaponName().IsEmpty() )
				{
					LOG.PrintTimeAndLog( 0, "LoadAreaWeaponList - Error! %s, %s", szGroupName, szTitle );
					delete pAreaWeapon;
				}
				else if( FindAreaWeapon( pAreaWeapon->GetAreaWeaponName() ) )
				{
					LOG.PrintTimeAndLog( 0, "LoadAreaWeaponList - %s Already Exist(%s, %s)", pAreaWeapon->GetAreaWeaponName().c_str(),
																							 szGroupName,
																							 szTitle );
					delete pAreaWeapon;
				}
				else
				{
					m_vAreaWeaponList.push_back( pAreaWeapon );
				}
			}
		}
	}
}

void ioAreaWeaponManager::LoadAreaWeaponNPCInfoList()
{
	ioINILoader_e kLoader( "config/sp2_area_weapon_npc.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "area_weapon_group_cnt", 0 );

	for( int i=0; i < iGroupCnt; ++i )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "area_weapon_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		ioINILoader kSetAreaWeaponLoader( szGroupName );
		kSetAreaWeaponLoader.SetTitle_e( "common_info" );

		int iCnt = kSetAreaWeaponLoader.LoadInt_e( "area_weapon_cnt", 0 );
		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szTitle, "area_weapon%d", j+1 );
			kSetAreaWeaponLoader.SetTitle( szTitle );

			ioAreaWeapon *pAreaWeapon = new ioAreaWeapon;

			if( pAreaWeapon )
			{
				pAreaWeapon->LoadProperty( kSetAreaWeaponLoader );

				if( pAreaWeapon->GetAreaWeaponName().IsEmpty() )
				{
					LOG.PrintTimeAndLog( 0, "LoadAreaWeaponNPCInfoList - Error! %s, %s", szGroupName, szTitle );
					delete pAreaWeapon;
				}
				else if( FindAreaWeapon( pAreaWeapon->GetAreaWeaponName() ) )
				{
					LOG.PrintTimeAndLog( 0, "LoadAreaWeaponNPCInfoList - %s Already Exist(%s, %s)", pAreaWeapon->GetAreaWeaponName().c_str(),
																									szGroupName,
																									szTitle );
					delete pAreaWeapon;
				}
				else
				{
					m_vAreaWeaponList.push_back( pAreaWeapon );
				}
			}
		}
	}
}

void  ioAreaWeaponManager::ReserveAreaWeapon( DWORD dwGenerateTime,
											  const ioHashString& szOwner,
											  const ioHashString& szTarget,
											  const ioHashString& szSkill,
											  const ioHashString& szAreaWeaponName,
											  const D3DXVECTOR3 &vStartPos,
											  const D3DXQUATERNION &qtRot,
											  int eCreateType )
{
	ReserveAreaWeaponInfo CallInfo;
	CallInfo.dwGenerateTime		= dwGenerateTime;
	CallInfo.szOwner			= szOwner;
	CallInfo.szTarget			= szTarget;
	CallInfo.szSkill			= szSkill;
	CallInfo.szAreaWeaponName	= szAreaWeaponName;
	CallInfo.vStartPos			= vStartPos;
	CallInfo.qtRot				= qtRot;
	CallInfo.eCreateType		= eCreateType;

	m_ReserveAreaWeaponInfoList.push_back( CallInfo );

	std::sort( m_ReserveAreaWeaponInfoList.begin(), m_ReserveAreaWeaponInfoList.end(), ReserveAreaWeaponInfoSort() );
}

ioAreaWeapon* ioAreaWeaponManager::CreateAreaWeapon( const ioHashString& szOwner,
													 const ioHashString& szTarget,
													 const ioHashString& szSkill,
													 const ioHashString& szAreaWeaponName,
													 const D3DXVECTOR3 &vStartPos,
													 const D3DXQUATERNION &qtRot,
													 int eCreateType )
{
	ioAreaWeapon *pAreaWeapon = FindAreaWeapon( szAreaWeaponName );
	if( pAreaWeapon )
	{
		if( pAreaWeapon->IsCreateOnlyOne() )
		{
			ioAreaWeapon *pPrevAreaWeapon = FindAreaWeaponByName( szAreaWeaponName, szOwner );
			if( pPrevAreaWeapon )
				g_AreaWeaponMgr.DestroyAreaWeapon( pPrevAreaWeapon->GetAreaWeaponIndex() );
		}
		ioAreaWeapon *pNewAreaWeapon = pAreaWeapon->Clone();
		if( pNewAreaWeapon )
		{
			m_dwCurAreaWeaponIndex++;
			pNewAreaWeapon->SetInitInfo( szOwner,
										 szTarget,
										 szSkill,
										 m_pPlayStage,
										 vStartPos,
										 qtRot,
										 m_dwCurAreaWeaponIndex,
										 eCreateType );

			m_vActiveAreaWeaponList.push_back( pNewAreaWeapon );
			return pNewAreaWeapon;
		}
	}

	LOG.PrintTimeAndLog( 0, "ioAreaWeaponManager::CreateAreaWeapon - Not Find AreaWeapon: %s", szAreaWeaponName.c_str() );
	return NULL;
}

void ioAreaWeaponManager::Update()
{
	ClearDeadAreaWeaponList();

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && pAreaWeapon->IsLive() )
		{
			pAreaWeapon->ProcessState( m_pPlayStage );
		}

		++iter;
	}
}

ioAreaWeapon* ioAreaWeaponManager::FindAreaWeapon( const ioHashString &szName )
{
	AreaWeaponList::iterator iter = m_vAreaWeaponList.begin();
	while( iter != m_vAreaWeaponList.end() )
	{
		if( (*iter)->GetAreaWeaponName() == szName )
		{
			return (*iter);
		}
		else
		{
			++iter;
		}
	}

	return NULL;
}

ioAreaWeapon* ioAreaWeaponManager::FindAreaWeaponByIndex( DWORD dwAreaWeaponIndex )
{
	if( dwAreaWeaponIndex == 0 ) return NULL;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && dwAreaWeaponIndex == pAreaWeapon->GetAreaWeaponIndex() )
		{
			return pAreaWeapon;
		}

		++iter;
	}

	return NULL;
}

ioAreaWeapon* ioAreaWeaponManager::FindAreaWeaponByName( const ioHashString &szName )
{
	if( szName.IsEmpty() ) return NULL;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && szName == pAreaWeapon->GetAreaWeaponName() )
		{
			return pAreaWeapon;
		}

		++iter;
	}

	return NULL;
}

ioAreaWeapon* ioAreaWeaponManager::FindAreaWeaponByName( const ioHashString &szName, const ioHashString &rkOwner )
{
	if( szName.IsEmpty() ) return NULL;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && szName == pAreaWeapon->GetAreaWeaponName() &&
			pAreaWeapon->GetOwnerName() == rkOwner )
		{
			return pAreaWeapon;
		}

		++iter;
	}

	return NULL;
}

ioAreaWeapon* ioAreaWeaponManager::FindAreaWeaponInRangeByName( const ioHashString &szName, const ioHashString &rkOwner, D3DXVECTOR3 vPos )
{
	if( szName.IsEmpty() ) return NULL;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && szName == pAreaWeapon->GetAreaWeaponName() &&
			pAreaWeapon->GetOwnerName() == rkOwner && pAreaWeapon->IsLive() )
		{
			D3DXVECTOR3 vDiff = pAreaWeapon->GetStartPos() - vPos;
			float fTargetDist = D3DXVec3LengthSq( &vDiff );
			float fRangeSq = pAreaWeapon->GetAreaRange() * pAreaWeapon->GetAreaRange();
			if( fTargetDist < fRangeSq )
				return pAreaWeapon;
		}

		++iter;
	}

	return NULL;
}

void ioAreaWeaponManager::FindAllAreaWeaponByName( const ioHashString &szName, std::vector< ioAreaWeapon* >& vAreaWeaponList )
{
	if( szName.IsEmpty() ) return;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && szName == pAreaWeapon->GetAreaWeaponName() && pAreaWeapon->IsLive() )
		{
			vAreaWeaponList.push_back( pAreaWeapon );
		}

		++iter;
	}
}

ioAreaWeapon* ioAreaWeaponManager::FindAttachedAreaWeapon( DWORD dwAreaWeaponIndex )
{
	if( dwAreaWeaponIndex == 0 ) return NULL;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && pAreaWeapon->GetCreateType() == ioAreaWeapon::CT_ATTACHED )
		{
			if( dwAreaWeaponIndex == pAreaWeapon->GetAreaWeaponIndex() )
			{
				return pAreaWeapon;
			}
		}

		++iter;
	}

	return NULL;
}

void ioAreaWeaponManager::ClearDeadAreaWeaponList()
{
	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( !pAreaWeapon )
		{
			++iter;
			continue;
		}

		if( !pAreaWeapon->IsLive() )
		{
			pAreaWeapon->DestroyAreaWeapon( m_pPlayStage );
			delete pAreaWeapon;
			iter = m_vActiveAreaWeaponList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioAreaWeaponManager::ClearMapAreaWeaponList()
{
	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( !pAreaWeapon )
		{
			++iter;
			continue;
		}

		if( pAreaWeapon->GetCreateType() == ioAreaWeapon::CT_MAP )
		{
			pAreaWeapon->DestroyAreaWeapon( m_pPlayStage );
			delete pAreaWeapon;
			iter = m_vActiveAreaWeaponList.erase( iter );
		}
		else
			++iter;
	}
}

void ioAreaWeaponManager::ClearAreaWeaponListByOwnerChar( const ioHashString &rkOwner )
{
	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( !pAreaWeapon )
		{
			iter = m_vActiveAreaWeaponList.erase( iter );
		}
		else
		{
			if( rkOwner == pAreaWeapon->GetOwnerName() )
			{
				pAreaWeapon->DestroyAreaWeapon( m_pPlayStage );
				delete pAreaWeapon;

				iter = m_vActiveAreaWeaponList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}

void ioAreaWeaponManager::ClearAllAreaWeaponList()
{
	AreaWeaponList::iterator iter = m_vAreaWeaponList.begin();
	while( iter != m_vAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( !pAreaWeapon )
		{
			++iter;
			continue;
		}

		pAreaWeapon->DestroyAreaWeapon(m_pPlayStage, false );
		delete pAreaWeapon;

		iter = m_vAreaWeaponList.erase( iter );
	}
	m_vAreaWeaponList.clear();

	iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( !pAreaWeapon )
		{
			++iter;
			continue;
		}

		pAreaWeapon->DestroyAreaWeapon(m_pPlayStage);
		delete pAreaWeapon;

		iter = m_vActiveAreaWeaponList.erase( iter );
	}
	m_vActiveAreaWeaponList.clear();
}

void ioAreaWeaponManager::DestroyAreaWeapon( DWORD dwIndex )
{
	if( dwIndex == 0 ) return;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( !pAreaWeapon )
		{
			++iter;
			continue;
		}

		if( dwIndex == pAreaWeapon->GetAreaWeaponIndex() )
		{
			// HARDCODE: AreaWeapon::Process() 과정에서 이함수가 호출될수 있다.
			//			 이 함수에서는 iter가 변경되어서는 않된다.

			pAreaWeapon->DestroyAreaWeapon( m_pPlayStage );
			break;
		}

		++iter;
	}
}

void ioAreaWeaponManager::CheckCurAreaWeapon()
{
	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon )
		{
			LOG.PrintTimeAndLog( 0, "CheckCurAreaWeapon() - CheckArea: %d", pAreaWeapon->GetAreaWeaponIndex() );
		}

		++iter;
	}
}

void ioAreaWeaponManager::GetAreaWeaponInfo( SP2Packet &rkPacket )
{
	AreaWeaponList vActiveList;
	vActiveList.clear();

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon &&
			pAreaWeapon->IsLive() &&
			pAreaWeapon->GetCreateType() != ioAreaWeapon::CT_TARGET_ATTACHED &&
			!pAreaWeapon->IsCreateByStruct() )
		{
			vActiveList.push_back( pAreaWeapon );
		}

		++iter;
	}

	int iActiveCnt = vActiveList.size();
	rkPacket << iActiveCnt;

	for( int i=0; i < iActiveCnt; ++i )
	{
		rkPacket << vActiveList[i]->GetOwnerName();
		rkPacket << vActiveList[i]->GetTargetName();
		rkPacket << vActiveList[i]->GetOwnerSkillName();
		rkPacket << vActiveList[i]->GetAreaWeaponName();
		rkPacket << vActiveList[i]->GetStartPos();
		rkPacket << vActiveList[i]->GetRotation();
		rkPacket << vActiveList[i]->GetCurLiveTime();
		rkPacket << vActiveList[i]->GetCreateType();

		vActiveList[i]->FillCurTargetList( rkPacket );
	}
}

void ioAreaWeaponManager::ApplyAreaWeaponInfo( SP2Packet &rkPacket )
{
	int iActiveCnt;
	rkPacket >> iActiveCnt;

	ioHashString szOwnerName, szTargetName, szOwnerSkill;
	ioHashString szAreaWeaponName;
	int iCreateType;
	D3DXVECTOR3 vStartPos;
	D3DXQUATERNION qtRot;
	DWORD dwLiveGapTime;

	int iTargetCnt;
	ioHashStringVec vTargetList;

	for( int i=0; i < iActiveCnt; ++i )
	{
		rkPacket >> szOwnerName;
		rkPacket >> szTargetName;
		rkPacket >> szOwnerSkill;
		rkPacket >> szAreaWeaponName;
		rkPacket >> vStartPos;
		rkPacket >> qtRot;
		rkPacket >> dwLiveGapTime;
		rkPacket >> iCreateType;

		rkPacket >> iTargetCnt;
		vTargetList.clear();
		for( int j=0; j < iTargetCnt; ++j )
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			vTargetList.push_back( szTargetName );
		}

		ioAreaWeapon *pAreaWeapon = NULL;
		if( szOwnerName.IsEmpty() ) continue;

		ioBaseChar *pOwner = m_pPlayStage->GetBaseChar( szOwnerName );
		if( !pOwner ) continue;

		pAreaWeapon = CreateAreaWeapon( szOwnerName,
										szTargetName,
										szOwnerSkill,
										szAreaWeaponName,
										vStartPos,
										qtRot,
										iCreateType );


		if( !pAreaWeapon ) continue;

		pAreaWeapon->SetCurLiveTime( dwLiveGapTime );
		pAreaWeapon->ApplyCurTargetList( vTargetList );
	}
}

ioAreaWeapon* ioAreaWeaponManager::FindAreaWeaponByMapIndex( DWORD dwAreaWeaponMapIndex )
{
	if( dwAreaWeaponMapIndex == 0 ) return NULL;

	AreaWeaponList::iterator iter = m_vActiveAreaWeaponList.begin();
	while( iter != m_vActiveAreaWeaponList.end() )
	{
		ioAreaWeapon *pAreaWeapon = (ioAreaWeapon*)(*iter);
		if( pAreaWeapon && dwAreaWeaponMapIndex == pAreaWeapon->GetAreaWeaponMapIndex() )
		{
			return pAreaWeapon;
		}

		++iter;
	}

	return NULL;
}

ioPlayStage* ioAreaWeaponManager::GetPlayStage()
{
	return m_pPlayStage;
}

void ioAreaWeaponManager::ProcessCallAreaWeaponInfo()
{
	if( m_ReserveAreaWeaponInfoList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ReserveAreaWeaponInfoList::iterator iter = m_ReserveAreaWeaponInfoList.begin();
	while( iter != m_ReserveAreaWeaponInfoList.end() )
	{
		ReserveAreaWeaponInfo &rkInfo = (*iter);

		ioBaseChar *pChar = m_pPlayStage->GetBaseChar( rkInfo.szOwner );
		if( !pChar )
		{
			iter = m_ReserveAreaWeaponInfoList.erase( iter );
			continue;
		}

		if( rkInfo.dwGenerateTime < dwCurTime )
		{	
			ioAreaWeapon* pArea = CreateAreaWeapon( rkInfo.szOwner, rkInfo.szTarget, rkInfo.szSkill, rkInfo.szAreaWeaponName, rkInfo.vStartPos, rkInfo.qtRot, rkInfo.eCreateType );
			iter = m_ReserveAreaWeaponInfoList.erase( iter );
			continue;
		}

		++iter;
	}
}