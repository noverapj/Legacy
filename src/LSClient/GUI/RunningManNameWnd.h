#pragma once

class RunningManNameWnd : public ioWnd
{
protected:
	enum
	{
		ID_OK        = 1,
		ID_NAME_EDIT = 2,
	};

protected:
	DWORD m_dwEditStartTime;
	DWORD m_dwEditLimitTime;

protected:
	bool  m_bSelectName;

protected:
	ioWnd *m_pPreEdit;

protected:
	static ioHashString m_szSaveName;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void SetEditFocus();
	void SendName();

public:
	bool IsSelectName(){ return m_bSelectName; }

public:
	RunningManNameWnd();
	virtual ~RunningManNameWnd();
};

