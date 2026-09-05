

#include "stdafx.h"

#include "../io3DEngine/ioFileTokenDefine.h"

#include "ioAniCollisionBoxGrp.h"

#define COL_MESH_ANI_VERSION 5500

ioAniCollisionBoxGrp::ioAniCollisionBoxGrp()
{
}

ioAniCollisionBoxGrp::ioAniCollisionBoxGrp( const ioAniCollisionBoxGrp &rhs )
{
	AniCollisionBox *pBox = NULL;

	AniCollisionBoxList::const_iterator iter;
	for( iter=rhs.m_AniCollisionBoxList.begin() ; iter!=rhs.m_AniCollisionBoxList.end() ; ++iter )
	{
		pBox = CreateNewBox();
		*pBox = *(*iter);
	}
}

ioAniCollisionBoxGrp::~ioAniCollisionBoxGrp()
{
	ClearBoxList();
}

bool ioAniCollisionBoxGrp::InitBoxList( const char *szFileName, bool bSpecialType )
{
	ClearBoxList();

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioAniCollisionBoxGrp::InitBoxList - %s Load Failed", szFileName );
		return false;
	}

	char szToken[4];
	kStream.Read(szToken, 4);

	int iVersion = 0;
	kStream.Read(&iVersion, sizeof(int));
	if( !IsAniCollisionFile( szToken, iVersion ) )
	{
		LOG.PrintTimeAndLog( 0, "ioAniCollisionBoxGrp::InitBoxList - %s Not AniCollision File", szFileName );
		return false;
	}

	int iBoxCount;
	kStream.Read(&iBoxCount, sizeof(int));

	for( int i=0 ; i<iBoxCount ; i++ )
	{
		AniCollisionType eType;
		kStream.Read(&eType, sizeof(AniCollisionType));

		D3DXVECTOR3 vCenter;
		kStream.Read(&vCenter, sizeof(D3DXVECTOR3));

		D3DXVECTOR3 vAxis[3];
		kStream.Read(vAxis, sizeof(D3DXVECTOR3)*3);

		float fExtents[3];
		kStream.Read(fExtents, sizeof(float)*3);

		int iLen;
		kStream.Read(&iLen, sizeof(int));

		char szName[MAX_PATH];
		kStream.Read(szName, iLen);
		szName[iLen] = '\0';

		AniCollisionBox *pBox = CreateNewBox();
		pBox->m_Type = eType;
		pBox->m_Box.SetCenter( vCenter );
		pBox->m_Box.SetAxis( 0, vAxis[0] );
		pBox->m_Box.SetAxis( 1, vAxis[1] );
		pBox->m_Box.SetAxis( 2, vAxis[2] );
		pBox->m_Box.SetExtents( 0, fExtents[0] );
		pBox->m_Box.SetExtents( 1, fExtents[1] );
		pBox->m_Box.SetExtents( 2, fExtents[2] );
		pBox->m_LinkBone = szName;

		if( bSpecialType )
		{
			pBox->m_Type = ACT_SPECIAL;
		}
	}

	return true;
}

AniCollisionBox* ioAniCollisionBoxGrp::CreateNewBox()
{
	AniCollisionBox *pBox = new AniCollisionBox;
	m_AniCollisionBoxList.push_back( pBox );

	return pBox;
}

bool ioAniCollisionBoxGrp::IsAniCollisionFile( const char *szToken, int iVersion )
{
	if( !strcmp( szToken, COL_MESH_TOKEN ) && iVersion == COL_MESH_ANI_VERSION )
		return true;

	return false;
}

bool ioAniCollisionBoxGrp::HasCollisionBox( AniCollisionType eType )
{
	AniCollisionBoxList::iterator iter;
	for( iter=m_AniCollisionBoxList.begin() ; iter!=m_AniCollisionBoxList.end() ; ++iter )
	{
		if( (*iter)->m_Type == eType )
			return true;
	}

	return false;
}

AniCollisionBox* ioAniCollisionBoxGrp::GetCollisionBox( AniCollisionType eType )
{
	AniCollisionBoxList::iterator iter;
	for( iter=m_AniCollisionBoxList.begin() ; iter!=m_AniCollisionBoxList.end() ; ++iter )
	{
		AniCollisionBox *pBox = *iter;
		if( pBox->m_Type == eType )
			return *iter;
	}

	return NULL;
}

int ioAniCollisionBoxGrp::GetCollisionBoxCnt()
{
	return m_AniCollisionBoxList.size();
}

void ioAniCollisionBoxGrp::ClearBoxList()
{
	AniCollisionBoxList::iterator iter;
	for( iter=m_AniCollisionBoxList.begin() ; iter!=m_AniCollisionBoxList.end() ; ++iter )
	{
		delete *iter;
	}

	m_AniCollisionBoxList.clear();
}

ioAniCollisionBoxGrp* ioAniCollisionBoxGrp::Clone()
{
	return new ioAniCollisionBoxGrp( *this );
}






