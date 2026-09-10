#pragma once


#include "../io3DEngine/ioThreadTaskManager.h"

class ioRenderSystem;

class ioScreenShotTask : public IThreadTask
{
protected:
	ioHashString m_SavePath;
	bool m_bSaveJPG;
	bool m_bSource16Bits;

	ioRenderSystem *m_pSystem;
	int m_iWidth;
	int m_iHeight;

	char m_szSaveFullPath[MAX_PATH*2];
	bool m_bDone;

public:
	// In MainThread
	bool Initialize( ioRenderSystem *pSystem,
					 D3DTEXTUREFILTERTYPE eFilter,
					 float fSizeRate );

public:
	virtual void Process();	// In TaskThread
	virtual void Done();	// In MainThread

protected:
	void MakeFullPathName();

public:
	virtual int  GetPriority() const { return 2; }

public:
	ioScreenShotTask( const ioHashString &szSavePath,
					  bool bSaveJPG,
					  bool bSource16Bits );

	virtual ~ioScreenShotTask();
};

