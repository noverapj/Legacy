#pragma once

#include <boost/unordered_map.hpp>
#include "ioExcelReader.h"

//--------------------------------------------------
// class
//--------------------------------------------------
class IBaseData
{
public:
	virtual bool LoadData(int iIdx) = 0;
	virtual void Release() = 0;
};

template <typename TKey, typename TData>
class BaseDataManager : public IBaseData
{
    typedef boost::unordered_map<TKey, TData*> MAPDATA;

public:
	BaseDataManager();
	virtual ~BaseDataManager();

public:
	virtual bool		LoadData(int iIdx);
	virtual int         GetTotal();
	virtual TData*      GetAt(int nIndex);
	virtual TData*      GetData(TKey nKeyVal);
	virtual void        Release();
	virtual MAPDATA&    GetMapData();

protected:
	virtual bool        LoadBin(const char* SheetName);
	virtual bool        LoadExcel(const char* FileName, const char* SheetName);
    virtual int         GetVersion() = 0;
	virtual void        CreateMapData() = 0;

protected:
	int                 m_nTotal;
	TData*              m_pDatas;
	MAPDATA             m_mapData;
	int					m_iTableIdx;
};


//--------------------------------------------------
// function
//--------------------------------------------------
template <typename TKey, typename TData>
BaseDataManager<TKey, TData>::BaseDataManager()
	: m_nTotal(0)
	, m_pDatas(0)
	, m_mapData()
	, m_iTableIdx(0)
{
}

template <typename TKey, typename TData>
BaseDataManager<TKey, TData>::~BaseDataManager()
{
	Release();
}

template <typename TKey, typename TData>
bool BaseDataManager<TKey, TData>::LoadData(int iIdx)
{
	m_iTableIdx = iIdx;

	tstring szSheet;
	g_TableDataMgr.GetSheetName(iIdx, szSheet);

	if( g_TableDataMgr.IsReadExcel() )
	{
		tstring szFile;
		g_TableDataMgr.GetFileName( szSheet.c_str(), szFile );

		if( LoadExcel( szFile.c_str(), szSheet.c_str() ) )
			return true;
	}

	tstring szPath = "config/tables/";
	szSheet = szSheet + ".dat";
	szPath = szPath + szSheet;

	return LoadBin( szPath.c_str() );
}

template <typename TKey, typename TData>
bool BaseDataManager<TKey, TData>::LoadBin(const char* SheetName)
{
	//if (GetTotal() != 0)
	//{
		//return false;
	//}

	FILE* fp = NULL;
	fopen_s(&fp, SheetName, "rb");
	if (fp)
	{
        int nToolVersion;
        fread_s(&nToolVersion, sizeof(int), sizeof(int), 1, fp);

        int nHeaderVersion;
        fread_s(&nHeaderVersion, sizeof(int), sizeof(int), 1, fp);

        int nDataVersion;
        fread_s(&nDataVersion, sizeof(int), sizeof(int), 1, fp);

        if (GetVersion() != nHeaderVersion)
        {
            return false;
        }

        int nHeaderCount;
        fread_s(&nHeaderCount, sizeof(int), sizeof(int), 1, fp);

        for (int i = 0; i < nHeaderCount; ++i)
        {
            int nTypeSize;
            int nType;
            int nNameSize;
            char szName[256];
            memset(szName, 0, 256);

            fread_s(&nTypeSize, sizeof(int), sizeof(int), 1, fp);
            fread_s(&nType, sizeof(int), sizeof(int), 1, fp);
            fread_s(&nNameSize, sizeof(int), sizeof(int), 1, fp);
            fread_s(szName, 256, sizeof(char), nNameSize, fp);
        }

		fread_s(&m_nTotal, sizeof(int), sizeof(int), 1, fp);

		m_pDatas = new TData[m_nTotal];
		fread_s(m_pDatas, sizeof(TData)*m_nTotal, sizeof(TData)*m_nTotal, 1, fp);

		CreateMapData();

		fclose(fp);

		return true;
	}

	LOG.PrintTimeAndLog( LOG_RELEASE_LEVEL, "%s Binary Load Fail", SheetName );

	return false;
}

template <typename TKey, typename TData>
bool BaseDataManager<TKey, TData>::LoadExcel(const char* FileName, const char* SheetName)
{
	ioAdoAutoInit autoinit;

	ioExcelReader excel;
	if( excel.Open( FileName, SheetName) )
	{
		if( excel.m_row <= 1 )
			return false;

		m_pDatas = new TData[excel.m_row - 1];

		if( excel.SetData( m_pDatas ) )
			m_nTotal = excel.m_row - 1;

		CreateMapData();

		return true;
	}

	LOG.PrintTimeAndLog( LOG_RELEASE_LEVEL, "%s Excel Load Fail", SheetName );

	return false;
}

template <typename TKey, typename TData>
int BaseDataManager<TKey, TData>::GetTotal()
{
	return m_nTotal;
}

template <typename TKey, typename TData>
TData* BaseDataManager<TKey, TData>::GetAt(int nIndex)
{
	if (nIndex >= 0 && nIndex < GetTotal())
	{
		return &m_pDatas[nIndex];
	}

	return NULL;
}

template <typename TKey, typename TData>
TData* BaseDataManager<TKey, TData>::GetData(TKey nKeyVal)
{
	boost::unordered_map<TKey, TData*>::iterator it;
	it = m_mapData.find(nKeyVal);
	if (it != m_mapData.end())
	{
		return it->second;
	}

	return NULL;
}

template <typename TKey, typename TData>
void BaseDataManager<TKey, TData>::Release()
{
    m_nTotal = 0;

	if (m_pDatas != NULL)
	{
		delete[] m_pDatas;
		m_pDatas = NULL;
	}

	if (m_mapData.size() > 0)
	{
		m_mapData.clear();
	}
}

template <typename TKey, typename TData>
boost::unordered_map<TKey, TData*>& BaseDataManager<TKey, TData>::GetMapData()
{
    return m_mapData;
}