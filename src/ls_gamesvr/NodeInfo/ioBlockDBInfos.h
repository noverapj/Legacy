#pragma once

#include "../BoostPooler.h"
#include <boost/unordered/unordered_map.hpp>

struct ArrayIndexInfo
{
	int iXZ;
	int iY;

	ArrayIndexInfo()
	{
		iXZ	= 0;
		iY	= 0;
	}
};

typedef std::vector<ArrayIndexInfo> ARRAYINFO;
struct ioBlockDBItem : public BoostPooler<ioBlockDBItem>
{
	__int64 m_iIndex;
	int m_iItemCode;
	int m_iPivotXZIndex;
	int m_iPivotY;
	int m_iDirection;
	int m_iScore;
	ARRAYINFO vInstalledArrayIndex;

	ioBlockDBItem()
	{
		Init();
	}

	void Init()
	{
		m_iIndex		= 0;
		m_iItemCode		= 0;
		m_iPivotXZIndex	= 0;
		m_iPivotY		= 0;
		m_iDirection	= 0;
		m_iScore		= 0;
		vInstalledArrayIndex.clear();
	}
};

class ioBlockDBInfos
{
public:
	ioBlockDBInfos();
	virtual ~ioBlockDBInfos();

	void Init();
	void Destroy();

protected:
	virtual BOOL AddBlockItem(const __int64 iIndex, const int m_iItemCode, const int iXZIndex, const int iY, const int iDirection);

public:
	virtual void DBToData(CQueryResultData *query_data);
	virtual void GetBlockCoordRotateValue(const int iX, const int iZ, int iDirection, int& iOutX, int& iOutZ);

	BOOL ChangeDirection(const __int64 iIndex, const int iDirection);
	BOOL ChangePos(const __int64 iIndex, const int iXZIndex, const int iY);

	ioBlockDBItem* GetBlockItemInfo(const __int64 iIndex);

	BOOL IsConstructedBlock(const __int64 iIndex);

	void PushInstalledArrayIndex(const __int64 iIndex, const int iXZIndex, const int iY);
	void DeleteInstalledBlockInfo(const __int64 iIndex);

	int GetInstalledCount() { return m_mBlockInfos.size(); }
	DWORD GetBlockItemCode(const __int64 iIndex);

protected:
	template <typename type>					// RADtoDEG
	type  RADtoDEG (type val) {return (type) (val * 57.2957795132);}
	
	template <typename type>					// DEGtoRAD
	type  DEGtoRAD (type val) {return (type) (val * 0.0174532925199);}
	//type  DEGtoRAD (type val) {return (type) (val * 3.14159265 / 180);}

protected:
	typedef boost::unordered_map<__int64, ioBlockDBItem*> BLOCKINFOS;

protected:
	BLOCKINFOS	m_mBlockInfos;
};