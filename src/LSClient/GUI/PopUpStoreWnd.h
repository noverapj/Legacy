#pragma once

class ioButton;
class ioUIRenderImage;
class ioUI3DEffectRender;

class PopupStoreWnd : public ioWnd
{
public:
	//버튼, 이미지 등등
	enum { 
		//LAYER
		eID_BASE = 0,
		eID_EFFECTS = 2,
		//버튼
		eID_ITEM = 1,
		eID_NAME,
		eID_GOLDS,
		eID_INFO,
		eID_BUY,
		eID_ETCS,
		eID_SHOW,
		eID_CLOSE,
		eID_IMGS1,
		eID_IMGS2,
		eID_COUNT,
		eID_INFO2,

		eID_ITEM_IMG = 101,
		eID_GOLD_SUB1 = 301,
		eID_GOLD_SUB2,
		eID_GOLD_SUB3,
		eID_GOLD_SUB4,

		eID_ETC_SUB1 = 601,
		eID_ETC_SUB2,
		eID_ETC_SUB3,
		eID_ETC_SUB4,
		eID_ETC_SUB5,
		eID_ETC_SUB6,
	};

	enum { ePopupConditionTotalCash = 0, ePopupConditionMonthCash, ePopupConditionCurCash, ePopupConditionGrade, ePopupConditionPlayTime, ePopupConditionMax };
	enum { eEffectBefore = 0, eEffectAfter, eEffectResult, eEffectSub, eEffectMax };
	enum { eMsgClose = 0, eMsgRealBuy, eMsgError };

	struct sPopupItemInfo
	{
		bool bActive;

		int iIndex;
		int iPresentType;
		int iPresentValue1;
		int iPresentValue2;
		int iCash;
		int iPrevCash;
		int iDisCount;
		int iManualIdx;
		int iChance;

		DWORD dwConditionMin[ePopupConditionMax];
		DWORD dwConditionMax[ePopupConditionMax];
	};

private:
	ioButton*			m_pBtnBuy;
	ioButton*			m_pBtnShow;
	ioButton*			m_pBtnClose;

	ioWnd*				m_pWndImgs1;
	ioWnd*				m_pWndImgs2;

	ioWnd*				m_pWndName;
	ioWnd*				m_pWndCount;
	ioWnd*				m_pWndItem;

	ioWnd*				m_pWndInfo1;
	ioWnd*				m_pWndInfo2;

	ioWnd*				m_pWndGolds;
	ioWnd*				m_pWndGoldWnd1;	// 원래가격
	ioWnd*				m_pWndGoldWnd2;	// 이미지 ▶
	ioWnd*				m_pWndGoldWnd3;	// 할인된가격
	ioWnd*				m_pWndGoldWnd4;	// 할인율

	ioWnd*				m_pWndEtcs;
	ioWnd*				m_pWndEtcWnd1;	// 텍스트
	ioWnd*				m_pWndEtcWnd2;	// 골드
	ioWnd*				m_pWndEtcWnd3;	// 충전
	ioWnd*				m_pWndEtcWnd4;	// 창닫기
	ioWnd*				m_pWndEtcWnd5;	// | 이미지
	ioWnd*				m_pWndEtcWnd6;	// | 이미지

	ioUI3DEffectRender* m_pEffectItem[eEffectMax];

private:
	bool		m_bGoldShowType; // 할인인지 아닌지

	//string
	ioHashString m_szGold;			// 골드
	ioHashString m_szRealClose;		// 정말 닫으시겠습니까
	ioHashString m_szRealBuy;		// 정말 구매..
	ioHashString m_szCount;			// 개
	ioHashString m_szString1;	//마우스 오버시 상세 설명을 볼수 있습니다.
	ioHashString m_szString2;	//구입하신 아이템은 선물함에서 확인하실 수 있습니다.
	ioHashString m_szError;		// 에러 문구 출력
	ioHashString m_szBtnNameBuy;	//구매
	ioHashString m_szBtnNameCharge;	// 충전
	ioHashString m_szBtnNameClose;	// 닫기
	ioHashString m_szBtnNameShow;	// 선물함가기
	ioHashString m_szEffectPlaySound;
	ioHashString m_szBtnOverSound;
	ioHashString m_szBtnClickSound;

	ioHashString m_szTitle;
	ioHashString m_szTitle2;
	ioHashString m_szIconName;
	ioHashString m_szSubIconName;
	ioHashString m_szCashText1;
	ioHashString m_szCashText2;
	ioHashString m_szCashText3;

public:
	bool			m_bSubPopup;
	bool			m_bPage;
	int				m_iCash;
	int				m_iMsgType;
	int				m_iPopupIndex;	
	DWORD			m_dwShowStartTime;
	sPopupItemInfo	m_OnPopupItemInfo;
	std::vector<sPopupItemInfo> m_vecPopupItemInfo;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

	void OnUpdateMyCash( int iCash );	
	void OnUpdatePresentText();			// TEXT1번째꺼
	void OnUpdatePresentText2();		// TEXT2번째꺼
	void OnUpdatePresentIconText();
	void OnUpdatePresentWnd();
	void OnUpdateGoldWindow();

public:	

	void SetSubPopup( bool bSet ) { m_bSubPopup = bSet; }
	void OnChanceAfter();	//앞에 당첨창 안 쓸경우..
	void OnPage1();	//기본창
	void OnPage2();	//결과창

	int OnItemOver();
	int OnItemLeave();

	int OnBtnBuy();
	int OnBtnShow();
	int OnBtnClose();

	int OnChargeOver();
	int OnBtnCharge();
	int OnChargeLeave();

	int OnCloseOver();
	int OnBtnExit();
	int OnCloseLeave();

	int GetPopupIndex() { return m_iPopupIndex; }
	sPopupItemInfo GetPopupItemInfo();
	
	void OpenPopup( int iPopupIndex );
	void OnBuySuccess();
	void OnButFaild( DWORD dwReturnType );

public:
	PopupStoreWnd();
	virtual ~PopupStoreWnd();
};

class PopupStoreSubWnd : public ioWnd
{
public:
	enum { eID_BASE = 0, eBTN_OPEN };
		
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

	DWORD		m_dwShowStartTime;
	ioButton*	m_pBtnOpen;
	ioUI3DEffectRender* m_pEffectReady;

	int OnBtnOpen();


	PopupStoreSubWnd();
	virtual ~PopupStoreSubWnd();
};