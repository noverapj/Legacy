

#include "stdafx.h"

#include "../io3DEngine/ioFileTokenDefine.h"
#include "../io3DEngine/ioAxisAlignBox.h"

#include "ioWoundedCollisionBoxGrp.h"

#define COL_MESH_WOUNDED_VERSION 5700

ioWoundedCollisionBoxGrp::ioWoundedCollisionBoxGrp()
{
}

ioWoundedCollisionBoxGrp::ioWoundedCollisionBoxGrp( const ioWoundedCollisionBoxGrp &rhs )
{
	WoundedCollisionBoxList::const_iterator iter=rhs.m_WoundedCollisionBoxList.begin();
	for( ; iter!=rhs.m_WoundedCollisionBoxList.end() ; ++iter )
	{
		WoundedCollisionBox *pBox = CreateNewBox();
		if( pBox )
		{
			*pBox = *(*iter);
		}
	}
}

ioWoundedCollisionBoxGrp::~ioWoundedCollisionBoxGrp()
{
	ClearBoxList();
}

bool ioWoundedCollisionBoxGrp::InitBoxList( const char *szFileName )
{
	ClearBoxList();

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWoundedCollisionBoxGrp::InitBoxList - %s Load Failed", szFileName );
		return false;
	}

	char szToken[4];
	kStream.Read( szToken, 4);

	int iVersion = 0;
	kStream.Read( &iVersion, sizeof(int));
	if( !IsWoundedCollisionFile( szToken, iVersion ) )
	{
		LOG.PrintTimeAndLog( 0, "ioWoundedCollisionBoxGrp::InitBoxList - %s Not AniCollision File",
								szFileName );
		return false;
	}

	int iBoxCount;
	kStream.Read( &iBoxCount, sizeof(int));

	for( int i=0 ; i<iBoxCount ; i++ )
	{
		D3DXVECTOR3 vCenter;
		kStream.Read( &vCenter, sizeof(D3DXVECTOR3));

		D3DXVECTOR3 vAxis[3];
		kStream.Read( vAxis, sizeof(D3DXVECTOR3)*3);

		float fExtents[3];
		kStream.Read( fExtents, sizeof(float)*3);

		int iLen;
		kStream.Read( &iLen, sizeof(int));

		char szName[MAX_PATH];
		kStream.Read( szName, iLen);
		szName[iLen] = '\0';

		WoundedCollisionBox *pBox = CreateNewBox();

		pBox->m_LocalBox.SetCenter( vCenter );
		pBox->m_LocalBox.SetAxis( 0, vAxis[0] );
		pBox->m_LocalBox.SetAxis( 1, vAxis[1] );
		pBox->m_LocalBox.SetAxis( 2, vAxis[2] );
		pBox->m_LocalBox.SetExtents( 0, fExtents[0] );
		pBox->m_LocalBox.SetExtents( 1, fExtents[1] );
		pBox->m_LocalBox.SetExtents( 2, fExtents[2] );

		pBox->m_LinkBone = szName;
		pBox->m_iLinkBipedIndex = -1;	// Not Initialized..
	}

	return true;
}

void ioWoundedCollisionBoxGrp::UpdateBipedIndex( const ioSkeleton *pSkeleton )
{
	if( !pSkeleton )	return;

	WoundedCollisionBoxList::iterator iter=m_WoundedCollisionBoxList.begin();
	for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		WoundedCollisionBox *pBox = *iter;
		if( pBox )
		{
			pBox->m_iLinkBipedIndex = pSkeleton->GetBipedIndex( pBox->m_LinkBone );
		}
	}
}

void ioWoundedCollisionBoxGrp::UpdateWorldBoxList( const ioSkeleton *pSkeleton,
												   const D3DXMATRIX *pWorldXform )
{
	if( !pSkeleton || !pWorldXform )
		return;

	D3DXVECTOR3 vVtxArray[8];
	ioAxisAlignBox kBoundAABB;

	D3DXMATRIX matWorld;
	WoundedCollisionBoxList::iterator iter = m_WoundedCollisionBoxList.begin();
	for(  ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		WoundedCollisionBox *pBox = *iter;

		ioBiped *pBiped = pSkeleton->GetBiped( pBox->m_iLinkBipedIndex );
		if( !pBiped )	continue;

		pBox->m_WorldBox = pBox->m_LocalBox;

		D3DXMatrixMultiply( &matWorld, pBiped->GetBipedSpaceFullXform(), pWorldXform );
		pBox->m_WorldBox.Transform( &matWorld );

		pBox->m_WorldBox.GetVertices( vVtxArray );

		D3DXVECTOR3 vMin, vMax;
		ioMath::CalcMinMaxPoint( vMin, vMax, vVtxArray, 8 );
		kBoundAABB.Merge( vMin, vMax );
	}

	if( !kBoundAABB.IsNull() )
	{
		m_WorldBoundingSphere.SetCenter( kBoundAABB.GetCenter() );
		m_WorldBoundingSphere.SetRadius( kBoundAABB.GetHalfDiagonal() );
	}
	else
	{
		m_WorldBoundingSphere.SetRadius( 0.0001f );
	}
}

WoundedCollisionBox* ioWoundedCollisionBoxGrp::CreateNewBox()
{
	WoundedCollisionBox *pBox = new WoundedCollisionBox;
	if( pBox )
	{
		m_WoundedCollisionBoxList.push_back( pBox );
	}

	return pBox;
}

bool ioWoundedCollisionBoxGrp::IsWoundedCollisionFile( const char *szToken, int iVersion )
{
	if( !strcmp( szToken, COL_MESH_TOKEN ) && iVersion == COL_MESH_WOUNDED_VERSION )
		return true;

	return false;
}

void ioWoundedCollisionBoxGrp::ClearBoxList()
{
	WoundedCollisionBoxList::iterator iter=m_WoundedCollisionBoxList.begin();
	for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		delete *iter;
	}

	m_WoundedCollisionBoxList.clear();
}

bool ioWoundedCollisionBoxGrp::IsCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	if( !ioMath::SphereTestLightly( rkAttackBox, m_WorldBoundingSphere ) )
		return false;

	if( pColPoint )
	{
		bool bCollision = false;

		D3DXVECTOR3 vDiff;
		float fCurDistSq = 0.0f;
		float fMinDistSq = std::numeric_limits<float>::infinity();

		WoundedCollisionBoxList::const_iterator iter=m_WoundedCollisionBoxList.begin();
		for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
		{
			const WoundedCollisionBox *pBox = *iter;

			if( ioMath::TestIntersection( rkAttackBox, pBox->m_WorldBox, NULL ) )
			{
				bCollision = true;

				vDiff = rkAttackBox.GetCenter() - pBox->m_WorldBox.GetCenter();
				fCurDistSq = D3DXVec3LengthSq( &vDiff );

				if( fMinDistSq > fCurDistSq )
				{
					*pColPoint = pBox->m_WorldBox.GetCenter();
					fMinDistSq = fCurDistSq;
				}
			}
		}

		return bCollision;
	}
	else
	{
		WoundedCollisionBoxList::const_iterator iter=m_WoundedCollisionBoxList.begin();
		for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
		{
			if( ioMath::TestIntersection( rkAttackBox, (*iter)->m_WorldBox, NULL ) )
				return true;
		}
	}

	return false;
}

bool ioWoundedCollisionBoxGrp::CheckSphereDistance( const D3DXVECTOR3 &vStart, float fRange, float &fDistance ) const
{
	fDistance = -FLOAT1;

	// m_WorldBoundingSphere 자체가 전체를 감싸는 구이기때문에 ioMath::SphereTestLightly의 역할을 한다.
	ioSphere kRangeSphere( vStart, fRange );
	if( !ioMath::TestIntersection( m_WorldBoundingSphere, kRangeSphere ) )
		return false;

	float fMinDistSq = -FLOAT1;
	WoundedCollisionBoxList::const_iterator iter=m_WoundedCollisionBoxList.begin();
	for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		const WoundedCollisionBox *pBox = *iter;

		int iQuantity = 0;
		D3DXVECTOR3 vPoints[2];
		ioSegment kColSegment( vStart, pBox->m_WorldBox.GetCenter() );

		if( ioMath::FindIntersection( kColSegment, pBox->m_WorldBox, iQuantity, vPoints ) )
		{
			if( iQuantity > 0 )
			{
				D3DXVECTOR3 vDiff = vStart - vPoints[0];

				float fCurDistSq = D3DXVec3LengthSq( &vDiff );
				if( fMinDistSq == -FLOAT1 )
					fMinDistSq = fCurDistSq;
				else
				{
					if( fMinDistSq > fCurDistSq )
						fMinDistSq = fCurDistSq;
				}
			}
		}
	}

	if( fMinDistSq > 0.0f && fMinDistSq <= fRange * fRange )
	{
		fDistance = sqrt( fMinDistSq );
		return true;
	}
	
	return false;
}

bool ioWoundedCollisionBoxGrp::CheckSpherePoint( const D3DXVECTOR3 &vStart, float fRange, D3DXVECTOR3 &vPoint ) const
{
	// m_WorldBoundingSphere 자체가 전체를 감싸는 구이기때문에 ioMath::SphereTestLightly의 역할을 한다.
	ioSphere kRangeSphere( vStart, fRange );
	if( !ioMath::TestIntersection( m_WorldBoundingSphere, kRangeSphere ) )
		return false;

	float fMinDistSq = -FLOAT1;
	WoundedCollisionBoxList::const_iterator iter=m_WoundedCollisionBoxList.begin();
	for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		const WoundedCollisionBox *pBox = *iter;

		int iQuantity = 0;
		D3DXVECTOR3 vContact[2];
		ioSegment kColSegment( vStart, pBox->m_WorldBox.GetCenter() );

		if( ioMath::FindIntersection( kColSegment, pBox->m_WorldBox, iQuantity, vContact ) )
		{
			if( iQuantity > 0 )
			{
				D3DXVECTOR3 vDiff = vStart - vContact[0];

				float fCurDistSq = D3DXVec3LengthSq( &vDiff );
				if( fMinDistSq == -FLOAT1 )
				{
					fMinDistSq = fCurDistSq;
					vPoint = vContact[0];
				}
				else
				{
					if( fMinDistSq > fCurDistSq )
					{
						fMinDistSq = fCurDistSq;
						vPoint = vContact[0];
					}
				}
			}
		}
	}

	if( fMinDistSq > 0.0f && fMinDistSq <= fRange * fRange )
		return true;

	return false;
}

bool ioWoundedCollisionBoxGrp::CheckCylinderPoint( const ioCylinder &rkCylinder,
												   const D3DXVECTOR3 &vStart,
												   D3DXVECTOR3 &vPoint ) const
{
	if( !ioMath::TestIntersection( m_WorldBoundingSphere, rkCylinder ) )
		return false;

	D3DXVECTOR3 vCylinderCenter = rkCylinder.GetCenter();
	float fTop = vCylinderCenter.y + (rkCylinder.GetHeight() * FLOAT05);
	float fBottom = vCylinderCenter.y - (rkCylinder.GetHeight() * FLOAT05);
	float fRadiusSq = rkCylinder.GetRadius() * rkCylinder.GetRadius();

	float fMinDistSq = -FLOAT1;
	WoundedCollisionBoxList::const_iterator iter=m_WoundedCollisionBoxList.begin();
	for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		const WoundedCollisionBox *pBox = *iter;

		int iQuantity = 0;
		D3DXVECTOR3 vContact[2];
		ioSegment kColSegment( vStart, pBox->m_WorldBox.GetCenter() );

		if( ioMath::FindIntersection( kColSegment, pBox->m_WorldBox, iQuantity, vContact ) )
		{
			if( iQuantity > 0 )
			{
				D3DXVECTOR3 vDiff = vStart - vContact[0];

				// Check
				if( !COMPARE( vContact[0].y, fBottom, fTop ) )
					continue;
			
				D3DXVECTOR3 vCheck = vDiff;
				vCheck.y = 0.0f;
				float fCheckSq = D3DXVec3LengthSq(&vCheck);
				if( fCheckSq > fRadiusSq )
					continue;
				//

				float fCurDistSq = D3DXVec3LengthSq( &vDiff );
				if( fMinDistSq == -FLOAT1 )
				{
					fMinDistSq = fCurDistSq;
					vPoint = vContact[0];
				}
				else
				{
					if( fMinDistSq > fCurDistSq )
					{
						fMinDistSq = fCurDistSq;
						vPoint = vContact[0];
					}
				}
			}
		}
	}

	if( fMinDistSq != -FLOAT1 )
		return true;
	
	return false;
}

int ioWoundedCollisionBoxGrp::GetBoxCount() const
{
	return m_WoundedCollisionBoxList.size();
}

bool ioWoundedCollisionBoxGrp::GetLocalBox( const ioHashString &rkBiped, ioOrientBox &rkLocalBox ) const
{
	WoundedCollisionBoxList::const_iterator iter = m_WoundedCollisionBoxList.begin();
	for( ; iter!=m_WoundedCollisionBoxList.end() ; ++iter )
	{
		WoundedCollisionBox *pBox = *iter;

		if( pBox->m_LinkBone == rkBiped )
		{
			rkLocalBox = pBox->m_LocalBox;
			return true;
		}
	}

	return false;
}

bool ioWoundedCollisionBoxGrp::GetBoxVertexArray( int iIndex, D3DXVECTOR3 pArray[8] ) const
{
	if( COMPARE( iIndex, 0, GetBoxCount() ) )
	{
		m_WoundedCollisionBoxList[iIndex]->m_WorldBox.GetVertices( pArray );
		return true;
	}

	return false;
}

ioWoundedCollisionBoxGrp* ioWoundedCollisionBoxGrp::Clone()
{
	return new ioWoundedCollisionBoxGrp( *this );
}