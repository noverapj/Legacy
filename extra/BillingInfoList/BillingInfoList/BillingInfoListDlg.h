
// BillingInfoListDlg.h : 헤더 파일
//

// CBillingInfoListDlg 대화 상자
#include "afxwin.h"
class CBillingInfoListDlg : public CDialogEx
{
// 생성입니다.
public:

	CBillingInfoListDlg(const TCHAR* strParameter, CWnd* pParent = NULL);	// 표준 생성자입니다.

public:

	float m_fChangeCash;
	int m_iEtcItemValue;
	int m_iDecoItem;	
	int m_iHero_Normal;
	int m_iHero_Rare;
	int m_iHero_Premium;
	int m_iChange;

	// 빌링 
	std::map<int, CString> m_GoodsInfoMap;
	typedef std::vector< SoldierInfo > vSoldierInfo;
	vSoldierInfo	m_vSoldierInfo;

	int				m_iMortmainCharGoodsNoValue;
	DWORDVec     m_vPackageEtcItemShortTypeList;
	DWORDVec     m_vPackageExtraItemMachineCodeList;


	// 아이템 이름리스트 
	std::map<int, CString> m_mapItemName;

	// 기타아이템
	float fMortmainCharMultiplyCash;
	float fMortmainPremiumCharMultiplyCash;
	float fMortmainRareCharMultiplyCash;

	typedef std::vector< LimitData* > vLimitData; // vector을 다른것으로 대체할때 INI Reload 개선 필요
	vLimitData m_vPesoLimitDataList;
	vLimitData m_vCashLimitDataList;
	vLimitData m_vPremiumCashLimitDataList;
	vLimitData m_vRareCashLimitDataList;

	std::map<int, CItemEtc*> MapItemEtc;

	// 용병아이템
	vHeroClassRate m_vecHeroClassRate;
	std::map<int, PriceData*> m_MapItemClass;

	// 치장아이템
	typedef std::vector< SexDecoList > vSexDecoList; // vector을 다른것으로 대체할때 INI Reload 개선 필요
	vSexDecoList m_vSexList;

	//ExtraItem
	mExtraItem	m_mapExtraItem;

	// 팝업스토어상점
	vPopupItemInfo	m_vPopupItemInfo;

	// 세일관련
	vSaleInfoVector m_vSaleInfoVector;

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_BILLINGINFOLIST_DIALOG };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:

	// Init Info
	void LoadChangeItemInfo();

	// INI Read
	int LoadIntSetting(CString lpAppName, CString lpKeyName, int iDefailt, CString strfullPath);
	float LoadFloatSetting(CString lpAppName, CString lpKeyName, float fDefailt, CString strfullPath);
	CString LoadStringSetting(CString lpAppName, CString lpKeyName, CString strDefault, CString strfullPath);

	void ChangeIntSetting(CString lpAppName, CString lpKeyName, int ChangeValue, CString strfullPath);

	// Load
	void LoadBillingGoods();
	void LoadBillingEtcItem(CString fullPath);
	void LoadBillingExtraItem(CString fullPath);

	void LoadItemNameINI(const TCHAR* file);
	void LoadEtcItemINI(const TCHAR* file);
	void LoadDecoItemINI(const TCHAR* file);
	void LoadDecoInfoItemINI(const TCHAR* path);
	void LoadSoldierItemINI(const TCHAR* file);

	void LoadExtraItemINI(const TCHAR* file);
	void LoadPopupItemINI(const TCHAR* file);

	// 저장처리시 필요
	int GetGoodsNoValue( int iLimitSeconds );
	bool IsPackageItem( int iEtcItemShortType );
	bool IsPackageItemExtra( int iExtraItemMachineCode );
	int GetClassRate(int iClass);
	CString GetGoodsName( int iGoodsNo);
	DWORD ConvertYYMMDDHHMMToDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute );
	bool IsCheckAlive( SYSTEMTIME st , DWORD dwStartDate, DWORD dwEndDate );
	int GetCash( ItemType eItemType, int iType1, int iType2 );

	// 저장 
	void SaveEtcItemTotal();
	void SaveEtcItemActive();

	void SaveDecoItemTotal();
	void SaveDecoItemActive();

	void SaveSoldierItemTotal();
	void SaveSoldierItemActive();

	void SaveExtraItemTotal();
	void SaveExtraItemActive();

	void SavePopupItemTotal();
	void SavePopupItemActive();

// 구현입니다.
protected:
	HICON m_hIcon;
	CString m_strParameter;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEtcitem();
	afx_msg void OnBnClickedDecoitem();
	afx_msg void OnBnClickedSoldier();
	afx_msg void OnBnClickedExtraitem();
	afx_msg void OnBnClickedPopup();
	afx_msg void OnBnClickedTotal();
	afx_msg void OnBnClickedItemtotal();
	afx_msg void OnBnClickedItemetc();
	afx_msg void OnBnClickedItemdeco();
	afx_msg void OnBnClickedItemsoldier();
	afx_msg void OnBnClickedItemextra();
	afx_msg void OnBnClickedItemPopup();
	CStatic text_total;
};
