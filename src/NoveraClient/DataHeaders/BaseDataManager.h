#pragma once

#include "ioExcelReader.h"

//--------------------------------------------------
// class
//--------------------------------------------------
class IBaseData
{
public:
	virtual bool LoadData(int iIdx) = 0;
	virtual bool Reload() = 0;
	virtual void Release() = 0;
};

template <typename TKey, typename TData>
class BaseDataManager : public IBaseData
{
public:
	BaseDataManager();
	virtual ~BaseDataManager();

public:
	virtual bool					LoadData(int iIdx);
	virtual bool					Reload();
	virtual int                     GetTotal();
	virtual TData*                  GetAt(int nIndex);
	virtual TData*                  GetData(TKey tKeyVal);
	virtual void                    Release();
	virtual std::map<TKey, TData*>& GetMapData();

protected:
	virtual bool                    LoadBin(const char* FileName);
	virtual bool                    LoadExcel(const char* FileName, const char* SheetName);
    virtual int						GetVersion() = 0;
	virtual void					CreateMapData() = 0;

protected:
	int                     m_nTotal;
	TData*                  m_pDatas;
	std::map<TKey, TData*>  m_mapData;
	int						m_iTableIdx;
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

	ioHashString szSheet;
	g_TableDataMgr.GetSheetName(iIdx, szSheet);

#ifndef SHIPIING
	if( g_TableDataMgr.IsReadExcel() )
	{
		ioHashString szFile;
		g_TableDataMgr.GetFileName( szSheet.c_str(), szFile );
		g_TableDataMgr.SetDataMgr( iIdx, this );

		if( LoadExcel( szFile.c_str(), szSheet.c_str() ) )
			return true;
	}
#endif
	ioHashString szPath = "Resource/Tables/";
	szSheet += ".dat";
	szPath += szSheet;

	return LoadBin( szPath.c_str() );
}

template <typename TKey, typename TData>
bool BaseDataManager<TKey, TData>::LoadBin(const char* FileName)
{
	if (GetTotal() != 0)
	{
		return false;
	}

    ioBinaryStream kStream;
	if (g_ResourceLoader.LoadStream(FileName, &kStream))
	{
        int nToolVersion;
        int nHeaderVersion;
        int nDataVersion;

        kStream.Read(&nToolVersion, sizeof(int));
        kStream.Read(&nHeaderVersion, sizeof(int));
        kStream.Read(&nDataVersion, sizeof(int));

        if (GetVersion() != nHeaderVersion)
        {
#ifndef SHIPIING
			char buf[MAX_PATH];
			memset( buf, 0, MAX_PATH );
			sprintf_s_e( buf, "file: %s\nheader: %08x\ndat: %08x", FileName, nHeaderVersion, GetVersion() );
			MessageBox( 0, buf, "DataVersionError", 0 );
#endif
            return false;
        }

        int nHeaderCount;
        kStream.Read(&nHeaderCount, sizeof(int));
        for (int i = 0; i < nHeaderCount; ++i)
        {
            int nTypeSize;
            int nType;
            int nNameSize;
            char szName[256];
            memset(szName, 0, 256);

            kStream.Read(&nTypeSize, sizeof(int));
            kStream.Read(&nType, sizeof(int));
            kStream.Read(&nNameSize, sizeof(int));
            kStream.Read(szName, nNameSize);
        }

		kStream.Read(&m_nTotal, sizeof(int));

		m_pDatas = new TData[m_nTotal];
		kStream.Read(m_pDatas, sizeof(TData)*m_nTotal);

		CreateMapData();

		return true;
	}

	LOG.PrintTimeAndLog( 0, "%s Data Load Fail", FileName );

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

	LOG.PrintTimeAndLog( 0, "%s Excel Load Fail", SheetName );

	return false;
}

template <typename TKey, typename TData>
bool BaseDataManager<TKey, TData>::Reload()
{
#ifndef SHIPIING
	Release();
	if( LoadData(m_iTableIdx) )
	{
		ioHashString szSheet;
		g_TableDataMgr.GetSheetName( m_iTableIdx, szSheet);
		LOG.PrintConsoleLog( 1, "%s Data Reload Complete", szSheet.c_str() );
	}
#endif

	return true;
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
TData* BaseDataManager<TKey, TData>::GetData(TKey tKeyVal)
{
	std::map<TKey, TData*>::iterator it;
	it = m_mapData.find(tKeyVal);
	if (it != m_mapData.end())
	{
		return it->second;
	}

	return NULL;
}

template <typename TKey, typename TData>
void BaseDataManager<TKey, TData>::Release()
{
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
std::map<TKey, TData*>& BaseDataManager<TKey, TData>::GetMapData()
{
    return m_mapData;
}