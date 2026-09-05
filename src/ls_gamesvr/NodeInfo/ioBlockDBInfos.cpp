#include "stdafx.h"
#include "..\EtcHelpFunc.h"
#include "ioBlockDBInfos.h"

ioBlockDBInfos::ioBlockDBInfos()
{
	Init();
}

ioBlockDBInfos::~ioBlockDBInfos()
{
	Destroy();
}

void ioBlockDBInfos::Init()
{
	m_mBlockInfos.clear();
}

void ioBlockDBInfos::Destroy()
{
	BLOCKINFOS::iterator it = m_mBlockInfos.begin();

	for(	; it != m_mBlockInfos.end(); it++ )
	{
		ioBlockDBItem* pInfo = it->second;
		if( pInfo )
			delete pInfo;
	}

	m_mBlockInfos.clear();
}

BOOL ioBlockDBInfos::AddBlockItem(const __int64 iIndex, const int iItemCode, const int iXZIndex, const int iY, const int iDirection)
{
	if( GetBlockItemInfo(iIndex) )
		return FALSE;

	ioBlockDBItem* pInfo	= new ioBlockDBItem;
	if( !pInfo )
		return FALSE;

	pInfo->Init();
	pInfo->m_iDirection		= iDirection;
	pInfo->m_iIndex			= iIndex;
	pInfo->m_iItemCode		= iItemCode;
	pInfo->m_iPivotXZIndex	= iXZIndex;
	pInfo->m_iPivotY		= iY;

	m_mBlockInfos.insert( std::make_pair(pInfo->m_iIndex, pInfo) );
	return TRUE;
}

void ioBlockDBInfos::DBToData(CQueryResultData *query_data)
{
}

BOOL ioBlockDBInfos::ChangeDirection(const __int64 iIndex, const int iDirection)
{
	ioBlockDBItem* pInfo	= GetBlockItemInfo(iIndex);

	if( !pInfo )
		return FALSE;

	pInfo->m_iDirection	= iDirection;
	return TRUE;
}

BOOL ioBlockDBInfos::ChangePos(const __int64 iIndex, const int iXZIndex, const int iY)
{
	ioBlockDBItem* pInfo = GetBlockItemInfo(iIndex);

	if( !pInfo )
		return FALSE;

	pInfo->m_iPivotXZIndex	= iXZIndex;
	pInfo->m_iPivotY		= iY;
	return TRUE;
}

ioBlockDBItem* ioBlockDBInfos::GetBlockItemInfo(const __int64 iIndex)
{
	BLOCKINFOS::iterator it	= m_mBlockInfos.find(iIndex);

	if( it == m_mBlockInfos.end() )
		return NULL;

	return it->second;
}

BOOL ioBlockDBInfos::IsConstructedBlock(const __int64 iIndex)
{
	BLOCKINFOS::iterator it	= m_mBlockInfos.find(iIndex);
	
	if( it == m_mBlockInfos.end() )
		return FALSE;

	return TRUE;
}

void ioBlockDBInfos::GetBlockCoordRotateValue( const int iX, const int iZ, int iDirection, int& iOutX, int& iOutZ)
{
	if( !COMPARE(iDirection, 0, 4) ) 
		return;

	float fX	= 0.0f;
	float fZ	= 0.0f;

	float fTheta	= iDirection * 90;

	float mMat[2][2] = {0,};
	mMat[0][0] = cos( DEGtoRAD( fTheta ) );
	mMat[1][1] = cos( DEGtoRAD( fTheta ) );

	mMat[0][1] = -1 * sin( DEGtoRAD( fTheta ) );
	mMat[1][0] = sin( DEGtoRAD( fTheta ) );	

	fX = mMat[0][0] * iX + mMat[0][1] * iZ;
	fZ = mMat[1][0] * iX + mMat[1][1] * iZ;

	/*fX	= iX * cos( DEGtoRAD(dTheta) ) - iZ * sin( DEGtoRAD(dTheta) );
	fZ	= iX * sin( DEGtoRAD(dTheta) ) + iZ * cos( DEGtoRAD(dTheta) );*/

	fX	= Help::RoundOff(fX);
	fZ	= Help::RoundOff(fZ);

	iOutX	= fX;
	iOutZ	= fZ;
}

void ioBlockDBInfos::PushInstalledArrayIndex(const __int64 iIndex, const int iXZIndex, const int iY)
{
	BLOCKINFOS::iterator it	= m_mBlockInfos.find(iIndex);
	
	if( it == m_mBlockInfos.end() )
		return;

	ioBlockDBItem* pInfo	= it->second;
	if( !pInfo )
		return;

	ArrayIndexInfo stInfo;

	stInfo.iXZ	= iXZIndex;
	stInfo.iY	= iY;

	pInfo->vInstalledArrayIndex.push_back(stInfo);
}

void ioBlockDBInfos::DeleteInstalledBlockInfo(const __int64 iIndex)
{
	BLOCKINFOS::iterator it	= m_mBlockInfos.find(iIndex);
	
	if( it == m_mBlockInfos.end() )
		return;

	delete it->second;

	m_mBlockInfos.erase(it);
}

DWORD ioBlockDBInfos::GetBlockItemCode(const __int64 iIndex)
{
	BLOCKINFOS::iterator it	= m_mBlockInfos.find(iIndex);
	
	if( it == m_mBlockInfos.end() )
		return 0;

	ioBlockDBItem* pInfo	= it->second;
	if( !pInfo )
		return 0;

	return pInfo->m_iItemCode;
}