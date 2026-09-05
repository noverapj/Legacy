#pragma once

class ioPlayStage;

struct TexDownLoadData
{
	ioHashString m_szOwnerName;
	ioHashString m_szDstTexName;
	DWORD        m_dwCustomIndex;
	DWORD        m_dwSubIndex;

	bool operator == ( const TexDownLoadData& rhs ) 
	{
		if( m_szOwnerName != rhs.m_szOwnerName )
			return false;
		if( m_szDstTexName != rhs.m_szDstTexName )
			return false;
		if( m_dwCustomIndex != rhs.m_dwCustomIndex )
			return false;
		if( m_dwSubIndex != rhs.m_dwSubIndex )
			return false;
		return true;
	}
};

class ioCustomTextureMgr : public Singleton< ioCustomTextureMgr >
{	
protected:
	ioPlayStage *m_pPlayStage;

protected:
	ioHashString m_szAppDirectory;
	ioHashString m_szStorageURL;

protected:
	typedef std::list< TexDownLoadData > TexDownLoadDataList;
	TexDownLoadDataList m_DownDataList;

protected:
	DWORDVec m_dwUICharPtrList;

protected:
	struct TrainExtraItem
	{
		DWORD m_dwItemCode;
		ioHashString m_szItemName;
		ioHashString m_szItemIcon;
		TrainExtraItem()
		{
			m_dwItemCode = 0;
		}
	};
	typedef std::vector< TrainExtraItem > TrainExtraItemList;
	TrainExtraItemList m_TrainExtraItemList;

public:
	void Initialize();

public:
	inline void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }
	inline const ioHashString &GetAppDirectory() const { return m_szAppDirectory; }
	inline const ioHashString &GetStorageURL() const { return m_szStorageURL; }

public:
	bool IsExistenceFile( const ioHashString &rkOwnerName, const ioHashString &rkDstTexName, DWORD dwCustomIndex, DWORD dwSubIndex );
	void FileDownLoadingDone( TexDownLoadData &rkData );

protected:
	bool IsDownLoading( TexDownLoadData &rkData );
	bool IsFileCheck( const ioHashString &rkFileName );

public:
	void CreateNewUIChar( DWORD dwCharPtr );
	void DestroyUIChar( DWORD dwCharPtr );
	bool IsUICharPtrCheck( DWORD dwCharPtr );

public:
	void CompleteCustomItem( DWORD dwItemCode, DWORD dwMaleCustom, DWORD dwFemaleCustom );

public:
	int GetMaxTrainExtraItem(){ return m_TrainExtraItemList.size(); }
	DWORD GetTrainExtraItem( int iArray );
	const ioHashString &GetTrainExtraItemName( DWORD dwItemCode );
	const ioHashString &GetTrainExtraItemIcon( DWORD dwItemCode );

public:
	static ioCustomTextureMgr& GetSingleton();

public:
	ioCustomTextureMgr();
	virtual ~ioCustomTextureMgr();
};

#define g_CustomTextureMgr ioCustomTextureMgr::GetSingleton()

