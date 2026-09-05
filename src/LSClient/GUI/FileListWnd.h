#pragma once

#include "../io3DEngine/ioMovingWnd.h"


struct FileDriveItem
{
	ioHashString m_szName;
	int          m_iMagicCode; 
	FileDriveItem()
	{
		m_iMagicCode = 0;
	}
};
typedef std::vector< FileDriveItem > vFileDriveItem;

class FileBtn : public ioButton
{
public:
	virtual void iwm_create();
public:
	FileBtn();
	virtual ~FileBtn();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class FileListItem : public ioWnd
{
public:
	enum
	{
		ID_FILENAME_BTN = 1,
		ID_PARAM_FILENAME_BTNUP = 1001,
	};

protected:
	bool m_bFolder;
	
public:
	void SetInfo( const char *szFileName, bool bFolder, bool bShowPlayBtn );
	void GetFileName( OUT ioHashString &rszFileName );
	bool IsFolder() const { return m_bFolder; }

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	FileListItem();
	virtual ~FileListItem();
};
//////////////////////////////////////////////////////////////////////////
class FileDriveListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST  = 10,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iCutWidth;
	int   m_iStartXPos;
	int   m_iStartYPos;

protected:
	vFileDriveItem  m_FileDriveItemList;
	FileDriveItem   m_OverDriveItem;
	int				m_iCurPos;	

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void ShowList( vFileDriveItem &rList );
	FileDriveItem &GetOverDriveItem(){ return m_OverDriveItem; }


public:
	FileDriveListWnd();
	virtual ~FileDriveListWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class FileListWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_PARENT_FOLDER	= 2,
		ID_SELECT_DISK		= 3,
		ID_BACKGROUND_CLEAR = 4,

		ID_LIST_1  = 101,
		ID_LIST_2  = 102,
		ID_LIST_3  = 103,
		ID_LIST_4  = 104,
		ID_LIST_5  = 105,
		ID_LIST_6  = 106,
		ID_LIST_7  = 107,
		ID_LIST_8  = 108,
		ID_LIST_9  = 109,
		ID_LIST_10 = 110,
		ID_LIST_11 = 111,
		ID_LIST_12 = 112,
		ID_LIST_13 = 113,


		ID_PULLDOWN_WND= 1001,
		ID_VERT_SCROLL = 1002,
	};

	enum
	{
		MAX_LIST = 13,
	};

	enum ReturnWndType
	{
		RWT_NONE = 0,
		RWT_COUSTOMSOUND_SETTING_ITEM_WND = 1,
		RWT_GUILD_MARK_CHANGE_WND         = 2,
		RWT_LOBBY_BACK_CHANGE_WND         = 3,
		RWT_LOADING_BACK_CHANGE_WND       = 4,
		RWT_BATTLE_BACK_CHANGE_WND        = 5,
		RWT_LADDER_BACK_CHANGE_WND        = 6,
		RWT_CUSTOM_TEXTURE_FIND_WND       = 7, 
		RWT_MODE_FIND_BACK_CHANGE_WND	  = 8,
	};

	enum 
	{
		COMPLETE_FILE_NAME_PARAM   = 100001,
	};

	struct FileInfo
	{
		ioHashString m_szFileName;
		bool         m_bFolder;

		FileInfo()
		{
			m_bFolder = false;
		}
	};

	typedef std::vector< FileInfo > FileInfoVector;

protected:
	FileInfoVector  m_vFileList;
	int             m_iCurPage;
	int             m_iMaxPage;
	ioWnd          *m_pReturnWnd;
	ReturnWndType   m_eReturnType;
	ioUIRenderImage *m_pDescIcon;
	ioHashString    m_szRootFolder;
	ioHashString    m_szFileExt;

protected:
	void UpdateShowFileList();
	void ClearFileList();
	void SetFileNameBtnUP( DWORD dwID );

	void PlayByCaller( DWORD dwID );
	void SetShowWndPos();
	void SetParentFolderFileList();

	void SetDriveList( vFileDriveItem &rList );
	void SetPullDownMenu( ioWnd *pOpenBtn );
	void SetPullDownBtnUP( ioWnd *pBtnUpWnd );

	void SetScrollRange();
	void SetDefaultDriveName();
	void UpdateByNewPath( const char *szNewPath );

public:
	void SetFileList(const char *szRootFolder, const char *szFileExt);
	bool IsFileListEmpty();
	void SetReturnInfo( ioWnd *pReturnWnd, ReturnWndType eType = RWT_NONE ); 

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

public:
	FileListWnd(void);
	virtual ~FileListWnd(void);
};

//////////////////////////////////////////////////////////////////////////////////////
class FileListInfoSort : public std::binary_function< const FileListWnd::FileInfo& , const FileListWnd::FileInfo&, bool >
{
public:
	bool operator() ( const FileListWnd::FileInfo &lhs, const FileListWnd::FileInfo &rhs ) const
	{
		if( lhs.m_bFolder && !rhs.m_bFolder )
			return true;
        else if( lhs.m_bFolder == rhs.m_bFolder )
		{
			int iCmpValue = stricmp( lhs.m_szFileName.c_str(), rhs.m_szFileName.c_str() );     
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

