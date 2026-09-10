#pragma once

#include "../io3DEngine/ioButton.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//재료 선택 버튼
class SelectMaterialItemBtn : public ioButton
{
protected:
	int m_iCode;
	int m_iPreCode;
	int m_iCurCnt;
	int m_nRecommandCode;

	ioComplexStringPrinter m_CountText;

	ioUIRenderFrame *m_pMaterialFrm;
	ioUIRenderImage *m_pMaterialImg;
	ioUIRenderImage *m_pRecommandMaterialImg;
	ioUIRenderImage *m_pIconImg;
	ioUIRenderImage *m_pSubIconImg;
	ioUIRenderImage *m_pIconBackImg;

public:
	virtual void iwm_hide();
	
public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	//초기화
	void InitData();
	//데이터 설정
	void SetData( int iCode, int iCnt );
	//데이터 설정
	void SetRecommand( int iCode, int iCnt );
	//코드 얻기
	int GetCode();

public:
	SelectMaterialItemBtn();
	virtual ~SelectMaterialItemBtn();
};
