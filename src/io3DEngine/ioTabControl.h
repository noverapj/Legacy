

#ifndef _ioTabControl_h_
#define _ioTabControl_h_

class __EX ioTabButton : public ioRadioButton
{
public:
	void ReSize( bool bClick );

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );

public:
	virtual DWORD GetWndType() const;

public:
	ioTabButton();
	virtual ~ioTabButton();
};

// 탭 버튼간의 재정렬을 책임져야한다.
class __EX ioTabControl : public ioWnd
{
protected:
	typedef std::vector< std::pair<ioTabButton*, ioWnd* > > ioTabButtonVec;
	ioTabButtonVec m_TabWndList;

public:
	void AddTabButton( ioTabButton *pTabBtn, ioWnd* pWnd );
	void CheckTabButton( DWORD dwCheckID );
	void ReArrangeButton();

public:
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param);

public:
	virtual DWORD GetWndType() const;

public:
	ioTabControl();
	virtual ~ioTabControl();
};

#endif
