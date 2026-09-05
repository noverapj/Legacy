
#ifndef _ioIME_h_
#define _ioIME_h_

#define MAX_IME_STRING	512
#define MAX_COMP_LEN	128
#define MAX_IMEDESC_LEN	128

class __EX ioIME
{
protected:
	DWORD	m_dwProperty;
	HIMC	m_hOldIMC;
	HWND	m_hParent;
	LPCANDIDATELIST		m_lpCandList;

protected:		// About Editing String
	int		m_iInputLen;
	char	m_szInput[MAX_IME_STRING];
	int		m_iPreCompLen;
	char	m_szComp[MAX_COMP_LEN];

protected:			// About Current State
	int		m_iCurCaretPos;
	bool	m_bComposition;
	char	m_szIMEDesc[MAX_IMEDESC_LEN];	

protected:		// About Select Area
	int		m_iShiftDownPos;
	int		m_iSelectStart;
	int		m_iSelectEnd;

protected:
	int		m_iMaxInputLen;

protected:
	bool	IsThereSelectArea();
	bool	IsEnglish(int iCheckPos);
	bool	IsKeyDownWithShift();
	void	InitSelectArea();
	bool	DeleteSelectArea();
	int		FilterCompString(char *szComp, int strLen);
	void	MakeInputString(char *szComp, int iStrLen);

	virtual void ProcessCompStr(HIMC hIMC, LPARAM lParam);
	void ProcessResultStr(HIMC hIMC, LPARAM lParam);
	void DeleteTokenToString(long *strLen); 

public:
	LRESULT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT	OnIMESetContext(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnInputLangChange(HWND hWnd, WPARAM wParam, LPARAM lParam);

public:
	void DeleteComposingText();
	
public:
	// if(remain process to DefWindowProc)   return FALSE 
	// else if( all process end ) return TRUE
	virtual BOOL IMEWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Create( HWND hWnd,  HINSTANCE hInstance );
	virtual void Process();

	virtual void SetString( const char *szText );
	virtual void SetStringNotDeleteComposing( const char *szText );
	virtual void ClearString();

	virtual const char* GetString();	

	virtual void CheckCurIMEDesc();
	virtual char* GetCompString(){ return m_szComp; }
	virtual int GetCaretPos() { return m_iCurCaretPos; }
	virtual void SetCaretPos( int iPos );

	virtual void SetMaxInputLen(int len) { m_iMaxInputLen = len; }

	virtual bool IsNowComposing() { return m_bComposition; }

	virtual bool HasFocus( HWND hFocus ) { return false; }

	virtual void ChangeConversionStatus(DWORD dwConvMode, DWORD dwSentMode = 0);

public:
	inline const char* GetCurIMEDesc() const { return m_szIMEDesc; }

	inline int  GetInputLength() { return m_iInputLen; }	

	LPCANDIDATELIST GetCandList(){ return m_lpCandList; }
	
public:
	ioIME();
	virtual ~ioIME();

private:
	bool m_bCtrl_V;
	bool m_bCtrl;
	bool m_bV;

private:
	void OnCheckPasteStr();
};

DWORD BASE_GetCandidateList(HIMC hIMC, DWORD dwIndex,
								LPCANDIDATELIST lpCand, DWORD dwBufLen, LPDWORD lpdwListCount);

#endif
