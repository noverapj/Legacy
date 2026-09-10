#pragma once

class GradeUpBonusEtcItemWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_OK   = 2,
	};
protected:
	bool m_bOneShow; 

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

protected:
	virtual void OnRender();

public:
	GradeUpBonusEtcItemWnd(void);
	virtual ~GradeUpBonusEtcItemWnd(void);
};

