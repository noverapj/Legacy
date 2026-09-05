#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SelectPetTargetBtn : public ioButton
{
protected:
	int m_nCurSelectSlot;
	DWORD m_dwCurTitleColor;

	ioHashString m_szDefaultText;
	ioHashString m_szSelectText;
	
public:
	virtual void iwm_show();

protected:
	virtual void OnRender();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	//초기화
	void InitData();
	//펫 설정
	void SetSelectPetSlot( int nSelectSlot );
	//재료 설정
	void SetSelectMaterial( int nSelectMaterial, ioHashString szTitle );
	//색 설정
	void SetCurTitleColor( DWORD dwTitleColor );

public:
	inline int GetSelectSlot() const { return m_nCurSelectSlot; }

public:
	SelectPetTargetBtn();
	virtual ~SelectPetTargetBtn();
};