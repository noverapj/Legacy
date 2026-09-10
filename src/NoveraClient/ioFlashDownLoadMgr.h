#pragma once

typedef struct tagFlashDownLoadData
{
	ioHashString m_szFileName;
	ioHashString m_szUrlAddress;
	int m_nType;
	bool m_bAfterProcess;

	bool operator == ( const tagFlashDownLoadData& rhs ) 
	{
		if( m_szFileName != rhs.m_szFileName )
			return false;
		if( m_szUrlAddress != rhs.m_szUrlAddress )
			return false;
		if( m_nType != rhs.m_nType )
			return false;
		if( m_bAfterProcess != rhs.m_bAfterProcess )
			return false;
		return true;
	}
}FlashDownLoadData;

typedef struct tagFlashBaseDownLoadData
{
	ioHashString m_szFileDir;
	ioHashString m_szFileName;
	ioHashString m_szUrlAddress;
}FlashBaseDownLoadData;

class ioFlashDownLoadTask : public IThreadTask
{
protected:
	ioHashString m_FullName;
	ioHashString m_FullURL;

protected:
	FlashDownLoadData m_DownData;

protected:
	bool m_bDone;

public:
	virtual void Process();
	virtual void Done();

public:
	virtual int GetPriority() const { return 3; }

public:
	ioFlashDownLoadTask( const char *szFullName, const char *szFullURL, FlashDownLoadData &kData );
	virtual ~ioFlashDownLoadTask();
};

class ioFlashDownLoadMgr : public Singleton< ioFlashDownLoadMgr >
{	
public:
	enum
	{
		FDEP_ShopItemMovieWnd,
	};

protected:
	ioHashString m_szResourceDir;
	ioHashString m_szDefaultDir;

protected:
	typedef std::list< FlashDownLoadData > FlashDownLoadDataList;
	FlashDownLoadDataList m_DownDataList;

	typedef std::vector< FlashBaseDownLoadData > FlashBaseDownLoadDataList;
	FlashBaseDownLoadDataList m_BaseDownDataList;

public:
	void Initialize();
	void BaseDownLoad();

public:
	inline const ioHashString &GetDefaultDir() const { return m_szDefaultDir; }

protected:
	bool IsDownLoading( FlashDownLoadData &rkData );
	bool IsFileCheck( const ioHashString &rkFileName );

public:
	bool IsExistenceFile( const ioHashString &szFileDir, const ioHashString &szFileName, const ioHashString &szUrlAddress, int nType, bool bAfterProcess );
	void FileDownLoadingDone( FlashDownLoadData &rkData, bool bDone );

protected:
	void CreateDownLoadDir( const ioHashString &szFileDir );
	void DownLoadEndProcess( int nType, const ioHashString &szFileName, bool bDone );
	
protected:
	void NoneDeleteList( ioHashStringVec &szFileList );
	void GetFilesFromDirectory( const char *Path, ioHashStringVec &szFileList );
	void DeleteFileList( ioHashStringVec &szFileList );
	bool SearchFileList( ioHashStringVec &szFileList, const char* szFileName );
	void DeleteProcess();

protected:
	void LoadBaseDownLoadList();

public:
	static ioFlashDownLoadMgr& GetSingleton();

public:
	ioFlashDownLoadMgr();
	virtual ~ioFlashDownLoadMgr();
};

#define g_FlashDownLoadMgr ioFlashDownLoadMgr::GetSingleton()