#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ioUIRenderFrame;
class GuildMarkChangeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT                  = 1,
		ID_FIND_BIG_FILE_BTN     = 2,
		ID_FIND_SMALL_FILE_BTN   = 3,
		ID_GUILD_MARK_CHANGE_BTN = 4,
		ID_CANCEL_BTN            = 5,
		ID_BIG_PATH_EDIT         = 6,
		ID_SMALL_PATH_EDIT       = 7,
	};

	enum 
	{
		 MAX_FILE_SIZE = 100000, 
		 
		 BIG_MARK_WIDTH    = 100, 
		 BIG_MARK_HEIGHT   = 100, 
		 SMALL_MARK_WIDTH  = 13, 
		 SMALL_MARK_HEIGHT = 13, 
	};

	enum AniType 
	{
		AT_NONE          = 0,
		AT_FRONT         = 1,
		AT_BACK          = 2,
	};

protected:
	enum FileUploadState
	{
		FUS_NONE                    = 0,
		FUS_BIG_FILE_UPLOAD_DALAY   = 1,
		FUS_BIG_FILE_UPLOAD         = 2,
		FUS_SMALL_FILE_UPLOAD_DALAY = 3,
		FUS_SMALL_FILE_UPLOAD       = 4,
	};

protected:
	ioUIRenderFrame *m_pMarkFrame;
	ioUIRenderImage *m_pEmptyBigMark;
	ioUIRenderImage *m_pEmptySmallMark;
	ioUIRenderImage *m_pBigMark;
	ioUIRenderImage *m_pSmallMark;

	FileUploadState m_eFileUplaodState;
	ioHashString    m_szDefaultBigEdit;
	ioHashString    m_szDefaultSmallEdit;

	DWORD           m_dwUploadDelayTime;

	static ioHashString m_szFileListRootFoler;
	
protected:
	void Init();

	void OpenFileListWnd( DWORD dwID );
	void SetChangeBtnUp();
	void EndFileUpload( DWORD param );
	void SetMarkImg( DWORD dwID );

	void ProcessUploadDelay();
	void SetFileListRootFolder(  const char *szFullPath );

protected:
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

public:
	GuildMarkChangeWnd(void);
	~GuildMarkChangeWnd(void);
};

