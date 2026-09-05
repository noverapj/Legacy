

#include "stdafx.h"

#include "ioResource.h"
#include "ioResourceManager.h"
#include "ioRcBiped.h"
#include "ioRcSkeleton.h"
#include "ioSkeleton.h"
#include "ioSkeletonManager.h"

#include "ioFileTokenDefine.h"
#include "ioStream.h"
#include "ioResourceLoader.h"

#include "ioBulletHelper.h"

template<> ioSkeletonManager* Singleton< ioSkeletonManager >::ms_Singleton = 0;

ioSkeletonManager::ioSkeletonManager()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Skeleton" );

	btAllocHelp::InitializeAlloc( 1024, 1024, 512 * 1024 );
}

ioSkeletonManager::~ioSkeletonManager()
{
	ResourceMap::iterator iter;
	for( iter=m_Resource.begin() ; iter!=m_Resource.end() ; ++iter )
	{
		ioRcSkeleton *pRcSkel = dynamic_cast< ioRcSkeleton* >( iter->second );
		pRcSkel->ReleaseLinkedSkeleton();
	}

	btAllocHelp::DestroyAlloc();
}

ioSkeleton* ioSkeletonManager::CreateInstance( const ioHashString &name )
{
	ioRcSkeleton *pRc = ioSkeletonManager::GetSingleton().CreateSkeleton( name );
	if( pRc )
		return new ioSkeleton( pRc );

	return NULL;
}

ioResource* ioSkeletonManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioRcSkeleton *pRcSkl = new ioRcSkeleton(name);
	if( LoadFile( name, pRcSkl ) )
		return pRcSkl;

	SAFEDELETE(pRcSkl);
	return NULL;
}

void ioSkeletonManager::ReLoadImpl( const ioHashString &name )
{
	ioRcSkeleton *pRcSkl = GetSkeletonByName( name );
	if( pRcSkl )
	{
		pRcSkl->ClearBipedList();
		LoadFile( name, pRcSkl );
	}
}

ioRcSkeleton* ioSkeletonManager::CreateSkeleton( const ioHashString &name )
{
	return dynamic_cast< ioRcSkeleton* >( Create( name ) );
}

ioRcSkeleton* ioSkeletonManager::GetSkeletonByName( const ioHashString &name )
{
	return dynamic_cast< ioRcSkeleton* >( GetByName( name ) );
}

bool ioSkeletonManager::IsSkeletonFile( ioBinaryStream &rkStream )
{
	char szBuf[8];
	ZeroMemory( szBuf, 8 );
	rkStream.Read( szBuf, 4 );

	if( !strcmp( szBuf, SKELETON_TOKEN ) )
		return true;

	return false;
}

bool ioSkeletonManager::LoadFile( const ioHashString &name, ioRcSkeleton* &pRcSkel )
{
	char szPath[MAX_PATH];
	wsprintf( szPath, "%s/%s", m_szStartDir, name.c_str() );

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szPath, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSkeletonManager::LoadFile - %s Load Failed", szPath );
		return false;
	}

	if( !IsSkeletonFile( kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSkeletonManager::LoadFile - %s Is Not *.skl File", szPath );
		return false;
	}

	// Skeleton Version
	int iVersion = 0;
	kStream.Read( &iVersion, sizeof(int) );

	// Biped
	int iBipedCnt = 0;
	kStream.Read( &iBipedCnt, sizeof(int) );
	pRcSkel->AllocBipedList( iBipedCnt );

	char szName[MAX_PATH];
	for( int i=0 ; i<iBipedCnt ; i++ )
	{
		ioRcBiped *pRcBiped = pRcSkel->GetRcBiped( i );

		// Biped Name
		kStream.ReadString( szName );
		pRcBiped->SetName( szName );

		D3DXVECTOR3		vPos;
		D3DXQUATERNION	qtRot;

		// LocalTM
		kStream.Read( &vPos, sizeof(D3DXVECTOR3) );
		kStream.Read( &qtRot, sizeof(D3DXQUATERNION) );
		pRcBiped->SetTranslation( vPos );
		pRcBiped->SetRotation( qtRot );

		// ObjectTM Inverse..
		kStream.Read( &vPos, sizeof(D3DXVECTOR3) );
		kStream.Read( &qtRot, sizeof(D3DXQUATERNION) );

		D3DXMATRIX kBipedSpaceTM;
		D3DXMatrixRotationQuaternion( &kBipedSpaceTM, &qtRot );
		kBipedSpaceTM._41 = vPos.x;
		kBipedSpaceTM._42 = vPos.y;
		kBipedSpaceTM._43 = vPos.z;
		pRcBiped->SetBipedSpaceTM( kBipedSpaceTM );

		D3DXMATRIX kObjectTM;
		kStream.Read( &kObjectTM, sizeof(D3DXMATRIX) );
		pRcBiped->SetObjectTM( kObjectTM );

		// Parent Name
		kStream.ReadString( szName );
		pRcBiped->SetParentName( szName );

		// Child List
		int iChildCount = 0;
		kStream.Read( &iChildCount, sizeof(int) );

		pRcBiped->ReserveChildCount( iChildCount );
		for( int j=0 ; j<iChildCount ; j++ )
		{
			kStream.ReadString( szName );
			pRcBiped->AddChild( szName );
		}
	}

	// Linked Skeleton
	if( kStream.ReadString( szName ) > 0 )
	{
		pRcSkel->SetLinkedSkeleton( szName );
	}

	AddMem( pRcSkel->GetName(), kStream.GetSize() );

	return true;
}

ioSkeletonManager& ioSkeletonManager::GetSingleton()
{
	return Singleton<ioSkeletonManager>::GetSingleton();
}