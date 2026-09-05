#pragma once

class ioUIRenderImage;
class ioGuildMarkMgr : public Singleton< ioGuildMarkMgr >
{	
protected:          //Mark,  Image
	typedef std::map< DWORD, ioUIRenderImage* > GuildMarkMap;
	GuildMarkMap m_GuildSmallMarkMap;	// 기본 마크 작은것
	GuildMarkMap m_GuildBigMarkMap;		// 기본 마크 큰것
	
	ioUIRenderImage *m_pBigMarkNone;

	struct CustomMarkData
	{
		DWORD m_dwGuildIndex;
		DWORD m_dwGuildMark;

		ioUIRenderImage *m_pCustomMark;
		
		CustomMarkData()
		{
			Init();
			m_pCustomMark = NULL;
		}

		void Init()
		{
			m_dwGuildIndex = m_dwGuildMark = 0;
		}
	};
	typedef std::vector< CustomMarkData * > vCustomMarkData;
	typedef vCustomMarkData::iterator vCustomMarkData_Iter;
	
	vCustomMarkData m_CustomBigMarkPool;
	vCustomMarkData m_CustomBigMarkList;
	vCustomMarkData m_CustomSmallMarkPool;
	vCustomMarkData m_CustomSmallMarkList;

	//INI 정보
	int m_iMaxDefaultMarkSize;
	int m_iMaxReserveMarkIndex;
	
protected://길드 레벨 마크
	typedef std::map< DWORD, ioUIRenderImage* > GuildLevelResMap;
	GuildLevelResMap m_GuildLevelResBlueMap;
	GuildLevelResMap m_GuildLevelResRedMap;

protected:
	ioUIRenderImage *GetSmallMarkImage( DWORD dwGuildIndex, DWORD dwGuildMark );
	ioUIRenderImage *GetSmallMarkImageDef( DWORD dwGuildMark );
	ioUIRenderImage *GetSmallMarkImageCustom( DWORD dwGuildIndex, DWORD dwGuildMark );
	ioUIRenderImage *GetSmallMarkImageCustomList( DWORD dwGuildIndex, DWORD dwGuildMark );
	ioUIRenderImage *GetSmallMarkImageCustomCreate( DWORD dwGuildIndex, DWORD dwGuildMark );

	ioUIRenderImage *GetBigMarkImage( DWORD dwGuildIndex, DWORD dwGuildMark );
	ioUIRenderImage *GetBigMarkImageDef( DWORD dwGuildMark );
	ioUIRenderImage *GetBigMarkImageCustom( DWORD dwGuildIndex, DWORD dwGuildMark );
	ioUIRenderImage *GetBigMarkImageCustomList( DWORD dwGuildIndex, DWORD dwGuildMark );
	ioUIRenderImage *GetBigMarkImageCustomCreate( DWORD dwGuildIndex, DWORD dwGuildMark );

	void CreateGuildSmallMark( DWORD dwGuildMark, const ioHashString &szMarkName );
	void CreateGuildBigMark( DWORD dwGuildMark, const ioHashString &szMarkName );
	void ClearGuildMarkList();	
	void ClearGuildLevelRes();

public:
	void Initialize();

public:
	static ioGuildMarkMgr& GetSingleton();

public:
	void RenderSmallMark( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderSmallMarkDefault( DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderSmallMarkOutImg( ioUIRenderImage *m_pImg, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMarkSize( int iSizeW, int iSizeH, DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMark56x56( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMark71x71( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMark86x86( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMark96x96( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMark80x80Default( DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );
	void RenderMark96x96OutImg( ioUIRenderImage *m_pImg, float fXPos, float fYPos, UIRenderType eRenderType = UI_RENDER_NORMAL );

public:
	int GetGuildSmallMarkSize( DWORD dwGUildIndex, DWORD dwGuildMark );
	int GetMaxReserveMarkIndex(){ return m_iMaxReserveMarkIndex; }
	int GetMaxDefaultMarkSize(){ return m_iMaxDefaultMarkSize; }
	const ioHashString &GetGuildMarkAppDir(){ return m_szAppDirectory; }
	ioUIRenderImage *GetGuildLevelRes( DWORD dwGuildLevel );
       
	//GUILD MARK LOADING
protected:
	ioHashString m_szLoadingMarkURL;
	ioHashString m_szAppDirectory;

	typedef std::list< DWORD > LoadingMarkList;
	LoadingMarkList m_LoadingMarkList;

protected:
	void AddLoadGuildMark( DWORD dwGuildIndex, DWORD dwGuildMark, bool bLarge );
	bool IsAlreadyLoadData( DWORD dwGuildIndex );

public:
	void GuildMarkLoadingDone( DWORD dwGuildIndex, DWORD dwGuildMark, bool bDone );

public:
	ioGuildMarkMgr();
	virtual ~ioGuildMarkMgr();
};

#define g_GuildMarkMgr ioGuildMarkMgr::GetSingleton()

