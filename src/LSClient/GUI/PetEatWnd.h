#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////
class PetEatWnd : public ioWnd
{
public: 
	enum 
	{
		ID_EAT_BTN		= 1,
		ID_BACK_BTN		= 2,
		ID_EMPTY_BTN	= 3,
				
		ID_MATERIALPET_BTN	 = 11,
		ID_MATERIALPET_WND	 = 101,	

		ID_HELP_MARK = 12,
		ID_HELP_TOOL_TIP = 13,
	};

protected:	
	ioUIRenderImage *m_pManualMark;

	int m_nHelpTipIdx;
	int m_nSelectPetSlot;
	int m_nSelectMaterialCode;
	bool m_bIsSelectMaterial;

	bool m_bEmpty;
	bool m_bMaxLv;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	
public:
	//펫코드 설정
	void SelectPetSlot( int nSelectSlot );
	//재료 코드 설정
	void SetMaterialCode( int nMaterialCode );

protected:
	//초기화
	void InitData();
	//자동 재료 아이템 설정
	void AutoShowMaterialList();
	//리셋 윈도우
	void ResetWnd();

protected:
	//경험치 패킷 전송
	void SendExpUp();

protected:
	//재료 리스트 위치 설정
	int GetMaterialListPosY( int nSize );
	//재료 리스트 얻기
	void GetMaterialList( vNewShopPullDownItem &rkList );
	//재료 리스트 버튼 클릭 시
	void MaterialListBtnUp( ioWnd *pOpenBtn, int nListArray );
	
public:
	//최대 레벨시 UI 적용
	void ApplyMaxLv();

protected:
	//UI 설정
	void HideMainInfo();
	void ShowMainInfo();

public:
	PetEatWnd(void);
	virtual ~PetEatWnd(void);
};