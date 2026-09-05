
#include "stdafx.h"

#include "ioDefenseController.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

ioDefenseController::ioDefenseController()
{
	m_dwCurrentTime = 0;
}

ioDefenseController::~ioDefenseController()
{
	DefenseMap::iterator iCreate;
	for( iCreate = m_DefenseMap.begin() ; iCreate != m_DefenseMap.end() ; ++iCreate )
	{
		DefenseList &kList = iCreate->second;
		kList.m_Defense.clear();
	}
	m_CurDefense.m_Defense.clear();
}

void ioDefenseController::LoadController( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "DefenseController" );
	int iMaxController = rkLoader.LoadInt_e( "MaxController", 0 );
	for(int i = 0;i < iMaxController;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "Defense%d", i + 1 );
		rkLoader.SetTitle( szKey );

		DefenseList kDefenseList;
		kDefenseList.m_fDefenseRate		= rkLoader.LoadFloat_e( "DefenseRate", 0.0f );
		kDefenseList.m_fDefenseRange	= rkLoader.LoadFloat_e( "WeaponRange", 99999999.0f );
		kDefenseList.m_fDefenseDirection= rkLoader.LoadFloat_e( "DefenseDirection", 0.0f );

		char szBuf[MAX_PATH] = "";
		rkLoader.LoadString_e( "WeaponSkipList", "", szBuf, MAX_PATH );
		if( strlen( szBuf ) > 0 )
		{
			StringVector vStringVector = ioStringConverter::Split( szBuf, "." );
			StringVector::iterator iter;
			for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
			{
				int iValue = atoi( iter->c_str() );
				if( iValue == -1 ) continue;

				kDefenseList.m_WeaponSkipList.push_back( iValue );
			}
			vStringVector.clear();
		}

		int iMaxDefense = rkLoader.LoadInt_e( "MaxKey", 0 );
		for(int j = 0;j < iMaxDefense;j++)
		{
			DefenseData kData;

			sprintf_e( szKey, "Key%d", j + 1 );
			kData.m_wKeyData  = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_Delay", j + 1 );
			kData.m_dwDelayTime = rkLoader.LoadInt( szKey, 0 );

			kDefenseList.m_Defense.push_back( kData );
		}
		m_DefenseMap.insert( DefenseMap::value_type( i + 1, kDefenseList ) );
	}
}

void ioDefenseController::ClearCurrentData()
{
	m_dwCurrentTime = 0;
	m_CurDefense.m_Defense.clear();
}

float ioDefenseController::GetAngle( D3DXVECTOR3 vPos1, D3DXVECTOR3 vPos2 )
{
	D3DXVECTOR3 vSightDir = vPos1 - vPos2;
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	return fCurYaw;
}

bool ioDefenseController::IsSkipType( DWORDVec &rkType, DWORD dwType )
{
	if( rkType.empty() ) 
		return false;

	DWORDVec::iterator iter = rkType.begin();
	for(;iter != rkType.end();iter++)
	{
		if( *iter == dwType )
			return true;
	}
	return false;
}

void ioDefenseController::AttachDefenseData( DefenseList &rkList, DWORD dwPredicTime )
{
	// Defense Copy
	m_CurDefense.m_fDefenseRange	= rkList.m_fDefenseRange;
	m_CurDefense.m_fDefenseRate		= rkList.m_fDefenseRate;
	m_CurDefense.m_fDefenseDirection= rkList.m_fDefenseDirection;
	m_CurDefense.m_Defense.clear();
	for(int i = 0;i < (int)rkList.m_Defense.size();i++)
	{
		DefenseData kData = rkList.m_Defense[i];
		if( i == 0 && dwPredicTime != 0 )
		{
			// 예상 시간이 있으면 딜레이를 준다.
			kData.m_dwDelayTime = dwPredicTime;
		}
		m_CurDefense.m_Defense.push_back( kData );
		
	}
	m_dwCurrentTime = FRAMEGETTIME();
}

bool ioDefenseController::Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fTargetRange )
{
	if( pOwner == NULL ) return false;
	if( m_DefenseMap.empty() ) return false;

	// 근접 타겟 유저의 현재 애니메이션에서 fire_s가 장착되어있으면 방어 체크
	if( pTarget )
	{
		DWORD dwFireTime = pTarget->GetCurAnimationFireTime();
		if( dwFireTime != 0 )
		{
			DWORDVec vDefenseID;
			DefenseMap::iterator iCreate;
			for( iCreate = m_DefenseMap.begin() ; iCreate != m_DefenseMap.end() ; ++iCreate )
			{
				DefenseList &rkList = iCreate->second;

				// 제외 타입
				if( IsSkipType( rkList.m_WeaponSkipList, 0 ) ) continue;

				// 방어률
				int iRandomRate = rand() % 100; 
				if( iRandomRate > rkList.m_fDefenseRate * 100 ) continue;       // 랜덤 적용

				// 거리 체크
				if( fTargetRange > rkList.m_fDefenseRange * rkList.m_fDefenseRange ) continue;

				// 내가 보는 방향과 상대가 보는 방향 : 180.0f면 서로 마주 보고 있음
				if( rkList.m_fDefenseDirection != 0.0f )
				{
					if( pTarget )
					{
						// 타겟이 내 앞에 있는지 확인
						D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 ) + ( pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
						if( !pOwner->IsNpcTargetCharListByCylinder( vPos, rkList.m_fDefenseRange * rkList.m_fDefenseRange, 180.0f, FLOAT500, FLOAT500, pTarget ) ) continue;

						D3DXVECTOR3 vOwnerDir   = pOwner->GetMoveDir();
						D3DXVECTOR3 vCreatorDir = pTarget->GetMoveDir();
						D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );
						D3DXVec3Normalize( &vCreatorDir, &vCreatorDir );
						float fCos = D3DXVec3Dot( &vCreatorDir, &vOwnerDir );
						float fRadian = acos( fCos );
						float fDeg    = RADtoDEG(fRadian);
						if( fDeg < rkList.m_fDefenseDirection ) continue;
					}
				}

				vDefenseID.push_back( iCreate->first );
			}

			if( !vDefenseID.empty() )
			{
				int r = rand()%vDefenseID.size();
				DefenseMap::iterator iter = m_DefenseMap.find( vDefenseID[r] );
				if( iter != m_DefenseMap.end() )
				{
					AttachDefenseData( iter->second, 0 );
					return true;
				}
			}		
		}
	}

	// 발사체 체크
	ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
	while( kIter.HasMoreElements() )
	{
		ioWeapon *pWeapon = kIter.Next();
		if( !pWeapon->IsLive() ) continue;
		if( pWeapon->GetDefenseBreakType() != DBT_NONE ) continue;			// 방어 불가 기술이면 방어가 의미 없다.
		if( pWeapon->GetTeam() == pOwner->GetTeam() ) continue;
			
		DWORDVec vDefenseID;
		DefenseMap::iterator iCreate;
		for( iCreate = m_DefenseMap.begin() ; iCreate != m_DefenseMap.end() ; ++iCreate )
		{
			DefenseList &rkList = iCreate->second;

			// 제외 타입
			if( IsSkipType( rkList.m_WeaponSkipList, pWeapon->GetType() ) ) continue;
			
			// 방어률
			int iRandomRate = rand() % 100; 
			if( iRandomRate > rkList.m_fDefenseRate * 100 ) continue;       // 랜덤 적용
			
			// 거리 체크
			D3DXVECTOR3 vDiff = pWeapon->GetPosition() - pOwner->GetWorldPosition();
			float fDistSq =  D3DXVec3LengthSq( &vDiff );	
			if( fDistSq > rkList.m_fDefenseRange * rkList.m_fDefenseRange ) continue;

			// 내가 보는 방향과 상대가 보는 방향 : 180.0f면 서로 마주 보고 있음
			if( rkList.m_fDefenseDirection != 0.0f )
			{
				ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
				if( pWeaponOwner )
				{
					// 지뢰는 방향체크 안함
					if( pWeapon->GetType() == ioWeapon::WT_MINE )
					{
						if( pTarget )	
						{
							// 나와 타겟 사이에 지뢰가 있는지만 체크
							float fAngle1   = GetAngle( pTarget->GetWorldPosition(), pOwner->GetWorldPosition() );
							float fAngle2   = GetAngle( pWeapon->GetPosition(), pOwner->GetWorldPosition() );
							float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
							if( fGapAngle > 45.0f ) continue;
						}
					}
					else
					{
						// 타겟이 내 앞에 있는지 확인
						D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate( FLOAT08 ) + ( pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
						if( !pOwner->IsNpcTargetCharListByCylinder( vPos, rkList.m_fDefenseRange * rkList.m_fDefenseRange, 180.0f, FLOAT500, FLOAT500, pWeaponOwner ) ) continue;

						D3DXVECTOR3 vOwnerDir   = pOwner->GetMoveDir();
						D3DXVECTOR3 vCreatorDir = pWeaponOwner->GetMoveDir();
						D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );
						D3DXVec3Normalize( &vCreatorDir, &vCreatorDir );
						float fCos = D3DXVec3Dot( &vCreatorDir, &vOwnerDir );
						float fRadian = acos( fCos );
						float fDeg    = RADtoDEG(fRadian);
						if( fDeg < rkList.m_fDefenseDirection ) continue;
					}
				}
			}
			vDefenseID.push_back( iCreate->first );
		}

		if( vDefenseID.empty() ) continue;

		int r = rand()%vDefenseID.size();
		DefenseMap::iterator iter = m_DefenseMap.find( vDefenseID[r] );
		if( iter != m_DefenseMap.end() )
		{
			AttachDefenseData( iter->second, 0 );
			return true;
		}		
	}	
	return false;
}

bool ioDefenseController::OnDefenseAction( ioNpcChar *pOwner, ioBaseChar *pTarget )
{
	if( m_CurDefense.m_Defense.empty() ) return false;

	DefenseData &rkData = m_CurDefense.m_Defense[0];
	{
		pOwner->SetActionKey( rkData.m_wKeyData, KEY_NONE, KEY_NONE );
		if( FRAMEGETTIME() - m_dwCurrentTime > rkData.m_dwDelayTime )
		{
			m_dwCurrentTime = FRAMEGETTIME();
			m_CurDefense.m_Defense.erase( m_CurDefense.m_Defense.begin() );
		}
	}			
	return true;
}

bool ioDefenseController::IsDefense()
{
	if( m_CurDefense.m_Defense.empty() )
		return false;
	return true;
}