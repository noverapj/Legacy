// OScopeTimeCtrl.cpp : implementation file//

#include "stdafx.h"
#include "math.h"

#include "OScopeTimeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COScopeTimeCtrl
COScopeTimeCtrl::COScopeTimeCtrl()
	: m_bCountryCode(false)
{
	InitDate();

	m_nYDecimals = 3;

	m_dLowerLimit = -10.0;
	m_dUpperLimit =  10.0;
	m_dLowerXLimit =  0.0;
	m_dUpperYLimit =  24.0;
	m_dRange      =  m_dUpperLimit - m_dLowerLimit;   // protected member variable

	m_nShiftPixels     = 0;
	m_nHalfShiftPixels = m_nShiftPixels/2;                     // protected
	m_nPlotShiftPixels = m_nShiftPixels + m_nHalfShiftPixels;  // protected

	m_crBackColor  = RGB(  0,   0,   0);  // see also SetBackgroundColor
	m_crGridColor  = RGB(  0, 255, 255);  // see also SetGridColor
	m_crPlotColor  = RGB(255, 255, 255);  // see also SetPlotColor

	//set country count color
	m_crCCUColor[CI_EG]		= CCU_RGB_ENG;
	m_crCCUColor[CI_GE]		= CCU_RGB_GER;
	m_crCCUColor[CI_FR]		= CCU_RGB_FRA;
	m_crCCUColor[CI_IT]		= CCU_RGB_ITA;
	m_crCCUColor[CI_PL]		= CCU_RGB_POL;
	m_crCCUColor[CI_TR]		= CCU_RGB_TUR;
	m_crCCUColor[CI_ETC]	= CCU_RGB_ETC;

	// protected variables
	m_penPlot.CreatePen(PS_SOLID, 1, m_crPlotColor);
	m_brushBack.CreateSolidBrush(m_crBackColor);

	//crate ccu pen & CCUArray
	for(int i = 0 ; i < CI_MAX ; ++i)
	{
		m_CCUpen[i].CreatePen(PS_SOLID, 2, m_crCCUColor[i]);
		m_dCurCCUPosition[i]	= 0;
		m_dPreCCUPosition[i]	= 0;
	}

	// public member variables, can be set directly 
	m_strXUnitsString.Format(_T("Concurrent User(%06d/%06d)"), 0, 0);  // can also be set with SetXUnits
	m_strYUnitsString.Format(_T("Y units"));  // can also be set with SetYUnits

	// protected bitmaps to restore the memory DC's
	m_pbitmapOldGrid = NULL;
	m_pbitmapOldPlot = NULL;
	
	InitToolTip();

}  // COScopeTimeCtrl

/////////////////////////////////////////////////////////////////////////////
COScopeTimeCtrl::~COScopeTimeCtrl()
{
	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid);  
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot);  
	if (m_pbitmapCCUOldPlot[0] != NULL) //CCU
	{
		for(int i = 0 ; i < CI_MAX ; ++i)
		{
			m_dcCCUPlot[i].SelectObject(m_pbitmapCCUOldPlot[i]);  
		}
	}

} // ~COScopeTimeCtrl


BEGIN_MESSAGE_MAP(COScopeTimeCtrl, CWnd)
  ON_WM_PAINT()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COScopeTimeCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL COScopeTimeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	BOOL result;
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);

	result = CWnd::CreateEx(
		WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 
		className, NULL, dwStyle, 
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID);
	
	if (result != 0)
		InvalidateCtrl();

	return result;
} // Create

void COScopeTimeCtrl::InitDate()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_stPrevDateTime = sysTime;

	m_stPrevDateTime.wHour = 0;
	m_stPrevDateTime.wMinute = 0;
	m_stPrevDateTime.wSecond = 0;
	m_stPrevDateTime.wMilliseconds = 0;
	m_dPreviousPosition =   0.0;
	m_bGraph = false;
}

void COScopeTimeCtrl::DateSetting(int iYear, int iMonth, int iDay, int iDayOfWeek, int iHour, int iMinute, int iSecond, int iMilliseconds)
{
	m_stPrevDateTime.wYear = iYear;
	m_stPrevDateTime.wMonth = iMonth;
	m_stPrevDateTime.wDay = iDay;
	m_stPrevDateTime.wDayOfWeek = iDayOfWeek;
	m_stPrevDateTime.wHour = iHour;
	m_stPrevDateTime.wMinute = iMinute;
	m_stPrevDateTime.wSecond = iSecond;
	m_stPrevDateTime.wMilliseconds = iMilliseconds;
}

void COScopeTimeCtrl::SetToolTip(CString text)
{
	m_toolTip.SetMaxTipWidth(10);
	m_toolTip.AddTool(this, text, NULL, 0);
	

	m_toolTip.Activate(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::SetRange( bool b, double dLower, double dUpper, int nDecimalPlaces)
{
	ASSERT(dUpper >= dLower);

	m_dLowerLimit     = dLower;
	m_dUpperLimit     = dUpper;
	m_nYDecimals      = nDecimalPlaces;
	m_dRange          = m_dUpperLimit - m_dLowerLimit;
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange; 
  
	// clear out the existing garbage, re-start with a clean plot
	if( b )
		InvalidateCtrl();
	else
		RefreshRange();

}  // SetRange


void COScopeTimeCtrl::SetDay(int iDay)
{
	m_day = iDay;
}  // SetXUnits


/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::SetXUnits(CString string)
{
	m_strXUnitsString = string;

	// clear out the existing garbage, re-start with a clean plot
	//InvalidateCtrl();
	InvalidateXUnits();
}  // SetXUnits

/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::SetYUnits(CString string)
{
	m_strYUnitsString = string;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl();

}  // SetYUnits

/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::SetGridColor(COLORREF color)
{
	m_crGridColor = color;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl();

}  // SetGridColor


/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::SetPlotColor(COLORREF color)
{
	m_crPlotColor = color;

	m_penPlot.DeleteObject();
	m_penPlot.CreatePen(PS_SOLID, 0, m_crPlotColor);

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl();

}  // SetPlotColor


/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::SetBackgroundColor(COLORREF color)
{
	m_crBackColor = color;

	m_brushBack.DeleteObject();
	m_brushBack.CreateSolidBrush(m_crBackColor);

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl();

}  // SetBackgroundColor

void COScopeTimeCtrl::InitToolTip()
{
	if (m_toolTip.m_hWnd == NULL) 
	{ 
		m_toolTip.Create(this); 
		m_toolTip.Activate(FALSE);
		m_toolTip.SetDelayTime(TTDT_AUTOPOP, 800);
	} 
}

/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::RefreshRange()
{
	// There is a lot of drawing going on here - particularly in terms of 
	// drawing the grid.  Don't panic, this is all being drawn (only once)
	// to a bitmap.  The result is then BitBlt'd to the control whenever needed.
	int i = 0;
	int nCharacters = 0;
	int nTopGridPix = 0, nMidGridPix = 0, nBottomGridPix = 0;

	CPen *oldPen = NULL;
	CPen solidPen(PS_SOLID, 0, m_crGridColor);
	CFont axisFont, yUnitFont, *oldFont;
	CString strTemp;

	// in case we haven't established the memory dc's
	CClientDC dc(this);  

	// if we don't have one yet, set up a memory dc for the grid
	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	m_dcGrid.SetBkColor (m_crBackColor);

	// fill the grid background
	m_dcGrid.FillRect(m_rectClient, &m_brushBack);

	// draw the plot rectangle:
	// determine how wide the y axis scaling values are
	nCharacters = abs((int)log10(fabs(m_dUpperLimit)));
	nCharacters = max(nCharacters, abs((int)log10(fabs(m_dLowerLimit))));
	//nCharacters = 3;

	// add the units digit, decimal point and a minus sign, and an extra space
	// as well as the number of decimal places to display
	nCharacters = nCharacters + 4 + m_nYDecimals;  

	// adjust the plot rectangle dimensions
	// assume 6 pixels per character (this may need to be adjusted)
	m_rectPlot.left = m_rectClient.left + 4*(nCharacters);
	m_nPlotWidth    = m_rectPlot.Width();

	// draw the plot rectangle
	oldPen = m_dcGrid.SelectObject (&solidPen); 
	m_dcGrid.MoveTo (m_rectPlot.left-1, m_rectPlot.top);
	m_dcGrid.LineTo (m_rectPlot.right, m_rectPlot.top);
	m_dcGrid.LineTo (m_rectPlot.right, m_rectPlot.bottom+1);
	m_dcGrid.LineTo (m_rectPlot.left-1, m_rectPlot.bottom+1);
	m_dcGrid.LineTo (m_rectPlot.left-1, m_rectPlot.top);
	m_dcGrid.SelectObject (oldPen); 

	// draw the dotted lines, 
	// use SetPixel instead of a dotted pen - this allows for a 
	// finer dotted line and a more "technical" look
	nMidGridPix    = (m_rectPlot.top + m_rectPlot.bottom)/2;
	nTopGridPix    = nMidGridPix - m_nPlotHeight/4;
	nBottomGridPix = nMidGridPix + m_nPlotHeight/4;

	for (i=m_rectPlot.left; i<m_rectPlot.right; i+=4)
	{
		m_dcGrid.SetPixel (i, nTopGridPix,    m_crGridColor);
		m_dcGrid.SetPixel (i, nMidGridPix,    m_crGridColor);
		m_dcGrid.SetPixel (i, nBottomGridPix, m_crGridColor);
	}

	// create some fonts (horizontal and vertical)
	// use a height of 14 pixels and 300 weight 
	// (these may need to be adjusted depending on the display)
	axisFont.CreateFont (14, 0, 0, 0, 300,
					   FALSE, FALSE, 0, ANSI_CHARSET,
					   OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY, 
					   DEFAULT_PITCH|FF_SWISS, _T("Arial"));
	yUnitFont.CreateFont (14, 0, 900, 0, 300,
					   FALSE, FALSE, 0, ANSI_CHARSET,
					   OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY, 
					   DEFAULT_PITCH|FF_SWISS, _T("Arial"));

	// grab the horizontal font
	oldFont = m_dcGrid.SelectObject(&axisFont);

	// y max
	m_dcGrid.SetTextColor (m_crGridColor);
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
	//strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dUpperLimit);
	strTemp.Format (_T("%d"), (int)m_dUpperLimit);
	m_dcGrid.TextOut (m_rectPlot.left-4, m_rectPlot.top, strTemp);

	// y min
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_BASELINE);
	//strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dLowerLimit);
	strTemp.Format (_T("%d"), (int)m_dLowerLimit);
	m_dcGrid.TextOut (m_rectPlot.left-4, m_rectPlot.bottom, strTemp);

	//// x min
	//m_dcGrid.SetTextAlign (TA_LEFT|TA_TOP);
	//m_dcGrid.TextOut (m_rectPlot.left, m_rectPlot.bottom+4, _T("0"));

	//// x max
	//m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
	//strTemp.Format (_T("%d"), m_nPlotWidth/m_nShiftPixels); 
	//m_dcGrid.TextOut (m_rectPlot.right, m_rectPlot.bottom+4, strTemp);

	// x units
	m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP);
	m_dcGrid.TextOut ((m_rectPlot.left+m_rectPlot.right)/2, 
					m_rectPlot.bottom+20, m_strXUnitsString);

	// restore the font
	m_dcGrid.SelectObject(oldFont);

	// y units
	oldFont = m_dcGrid.SelectObject(&yUnitFont);
	m_dcGrid.SetTextAlign (TA_CENTER|TA_BASELINE);
	m_dcGrid.TextOut ((m_rectClient.left+m_rectPlot.left)/2, 
					(m_rectPlot.bottom+m_rectPlot.top)/2, m_strYUnitsString);
	m_dcGrid.SelectObject(oldFont);

	// at this point we are done filling the the grid bitmap, 
	// no more drawing to this bitmap is needed until the setting are changed
	
	// if we don't have one yet, set up a memory dc for the plot
	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(&dc);
		
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot);

		for(int i = 0 ; i < CI_MAX ; ++i)
		{
			m_dcCCUPlot[i].CreateCompatibleDC(&dc);
			m_bitmapCCUPlot[i].CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
			m_pbitmapCCUOldPlot[i] = m_dcCCUPlot[i].SelectObject(&m_bitmapCCUPlot[i]);
		}
	}
	
	// finally, force the plot area to redraw
	InvalidateRect(m_rectClient);
}

void COScopeTimeCtrl::InvalidateCtrl()
{

	// There is a lot of drawing going on here - particularly in terms of 
	// drawing the grid.  Don't panic, this is all being drawn (only once)
	// to a bitmap.  The result is then BitBlt'd to the control whenever needed.
	int i;
	int nCharacters;
	int nTopGridPix, nMidGridPix, nBottomGridPix;

	CPen *oldPen;
	CPen solidPen(PS_SOLID, 0, m_crGridColor);
	CFont axisFont, yUnitFont, *oldFont;
	CString strTemp;

	// in case we haven't established the memory dc's
	CClientDC dc(this);  

	// if we don't have one yet, set up a memory dc for the grid
	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	m_dcGrid.SetBkColor (m_crBackColor);

	// fill the grid background
	m_dcGrid.FillRect(m_rectClient, &m_brushBack);

	// draw the plot rectangle:
	// determine how wide the y axis scaling values are
	nCharacters = abs((int)log10(fabs(m_dUpperLimit)));
	nCharacters = max(nCharacters, abs((int)log10(fabs(m_dLowerLimit))));

	// add the units digit, decimal point and a minus sign, and an extra space
	// as well as the number of decimal places to display
	nCharacters = nCharacters + 4 + m_nYDecimals;  

	// adjust the plot rectangle dimensions
	// assume 6 pixels per character (this may need to be adjusted)
	m_rectPlot.left = m_rectClient.left + 4*(10);
	m_rectPlot.right = (m_rectClient.right - m_rectPlot.left)/ 24*24+m_rectPlot.left;
	m_rectPlot.bottom = m_rectClient.bottom - 40;

	m_nPlotHeight	= m_rectPlot.Height();
	m_nPlotWidth    = m_rectPlot.Width();

	// draw the plot rectangle
	oldPen = m_dcGrid.SelectObject (&solidPen); 
	m_dcGrid.MoveTo (m_rectPlot.left-1, m_rectPlot.top);
	m_dcGrid.LineTo (m_rectPlot.right, m_rectPlot.top);
	m_dcGrid.LineTo (m_rectPlot.right, m_rectPlot.bottom+1);
	m_dcGrid.LineTo (m_rectPlot.left-1, m_rectPlot.bottom+1);
	m_dcGrid.LineTo (m_rectPlot.left-1, m_rectPlot.top);
	m_dcGrid.SelectObject (oldPen); 

	// draw the dotted lines, 
	// use SetPixel instead of a dotted pen - this allows for a 
	// finer dotted line and a more "technical" look
	nMidGridPix    = (m_rectPlot.top + m_rectPlot.bottom)/2;
	nTopGridPix    = nMidGridPix - m_nPlotHeight/4;
	nBottomGridPix = nMidGridPix + m_nPlotHeight/4;

	for (i=m_rectPlot.left; i<m_rectPlot.right; i+=4)
	{
		m_dcGrid.SetPixel (i, nTopGridPix,    m_crGridColor);
		m_dcGrid.SetPixel (i, nMidGridPix,    m_crGridColor);
		m_dcGrid.SetPixel (i, nBottomGridPix, m_crGridColor);
	}

	// create some fonts (horizontal and vertical)
	// use a height of 14 pixels and 300 weight 
	// (these may need to be adjusted depending on the display)
	axisFont.CreateFont (14, 0, 0, 0, 300,
					   FALSE, FALSE, 0, ANSI_CHARSET,
					   OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY, 
					   DEFAULT_PITCH|FF_SWISS, _T("Arial"));
	yUnitFont.CreateFont (14, 0, 900, 0, 300,
					   FALSE, FALSE, 0, ANSI_CHARSET,
					   OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY, 
					   DEFAULT_PITCH|FF_SWISS, _T("Arial"));

	// grab the horizontal font
	oldFont = m_dcGrid.SelectObject(&axisFont);

	CountSetting();

	TimeSetting();

	if( strTemp == "102" )
	{
		TRACE( "%s\n", strTemp );
	}

	// x units
	m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP);
	m_dcGrid.TextOut ((m_rectPlot.left+m_rectPlot.right)/2, 
					m_rectPlot.bottom+20, m_strXUnitsString);

	// restore the font
	m_dcGrid.SelectObject(oldFont);	

	// y units
	oldFont = m_dcGrid.SelectObject(&yUnitFont);
	m_dcGrid.SetTextAlign (TA_CENTER|TA_BASELINE);
	m_dcGrid.TextOut ((m_rectClient.left+m_rectPlot.left)/2, (m_rectPlot.bottom+m_rectPlot.top)/2, m_strYUnitsString);
	m_dcGrid.SelectObject(oldFont);

	// at this point we are done filling the the grid bitmap, 
	// no more drawing to this bitmap is needed until the setting are changed

	// if we don't have one yet, set up a memory dc for the plot
	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(&dc);
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot); 
	}
	for(int i = 0 ; i < CI_MAX ; ++i)
	{
		if(m_dcCCUPlot[i].GetSafeHdc() == NULL)
		{
			m_dcCCUPlot[i].CreateCompatibleDC(&dc);
			m_bitmapCCUPlot[i].CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
			m_pbitmapCCUOldPlot[i] = m_dcCCUPlot[i].SelectObject(&m_bitmapCCUPlot[i]);
		}
	}
	

	// make sure the plot bitmap is cleared
	m_dcPlot.SetBkColor (m_crBackColor);
	m_dcPlot.FillRect(m_rectClient, &m_brushBack);

	// finally, force the plot area to redraw
	InvalidateRect(m_rectClient);

} // InvalidateCtrl

void COScopeTimeCtrl::InvalidateXUnits()
{
	// There is a lot of drawing going on here - particularly in terms of 
	// drawing the grid.  Don't panic, this is all being drawn (only once)
	// to a bitmap.  The result is then BitBlt'd to the control whenever needed.

	CFont axisFont, *oldFont;

	// in case we haven't established the memory dc's
	CClientDC dc(this);  

	// if we don't have one yet, set up a memory dc for the grid
	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	// create some fonts (horizontal and vertical)
	// use a height of 14 pixels and 300 weight 
	// (these may need to be adjusted depending on the display)
	axisFont.CreateFont (14, 0, 0, 0, 300,
					   FALSE, FALSE, 0, ANSI_CHARSET,
					   OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY, 
					   DEFAULT_PITCH|FF_SWISS, _T("Arial"));

	// grab the horizontal font
	oldFont = m_dcGrid.SelectObject(&axisFont);

	// x units
	m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP);
	m_dcGrid.TextOut ((m_rectPlot.left+m_rectPlot.right)/2, m_rectPlot.bottom+20, m_strXUnitsString);

	// restore the font
	m_dcGrid.SelectObject(oldFont);	
	
	// finally, force the plot area to redraw
	InvalidateRect(m_rectClient);
}

/////////////////////////////////////////////////////////////////////////////
double COScopeTimeCtrl::AppendPoint(bool bCCU, double dNewPoint)
{
	// append a data point to the plot
	// return the previous point
	m_bCountryCode = bCCU;
	double dPrevious;
  
	dPrevious = m_dCurrentPosition;
	m_dCurrentPosition = dNewPoint;
	DrawPoint();

	Invalidate();
	return dPrevious;

} // AppendPoint

/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::AppendDateTimePoint(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, bool bCCU, double dNewPoint)
{
	m_bCountryCode = bCCU;
	double dPrevious;
  
	dPrevious = m_dCurrentPosition;
	m_dCurrentPosition = dNewPoint;
	DrawPoint(iYear, iMonth, iDay, iHour, iMinute, iSecond, bCCU, dNewPoint);

	m_day = iDay;
	Invalidate();

}


void COScopeTimeCtrl::AppendCCUPoint(double dNewGB,double dNewDE,double dNewFR,double dNewIT,double dNewPL,double dNewTR, double dNewETC )
{
	m_dCurCCUPosition[CI_EG] = dNewGB;
	m_dCurCCUPosition[CI_GE] = dNewDE;
	m_dCurCCUPosition[CI_FR] = dNewFR;
	m_dCurCCUPosition[CI_IT] = dNewIT;
	m_dCurCCUPosition[CI_PL] = dNewPL;
	m_dCurCCUPosition[CI_TR] = dNewTR;
	m_dCurCCUPosition[CI_ETC] = dNewETC;
} //AppendCCUPoint




 
////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::OnPaint() 
{
	CPaintDC dc(this);  // device context for painting
	CDC memDC;
	CBitmap memBitmap;
	CBitmap* oldBitmap; // bitmap originally found in CMemDC

	// no real plotting work is performed here, 
	// just putting the existing bitmaps on the client

	// to avoid flicker, establish a memory dc, draw to it 
	// and then BitBlt it to the client
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the grid on the memory dc
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &m_dcGrid, 0, 0, SRCCOPY);
		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &m_dcPlot, 0, 0, SRCPAINT);  //SRCPAINT

		//CCU
		if(m_bCountryCode)
		{
			for(int i = 0 ; i < CI_MAX ; ++i)
				memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &m_dcCCUPlot[i], 0, 0, SRCPAINT);
		}


		// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &memDC, 0, 0, SRCCOPY);
	}

	memDC.SelectObject(oldBitmap);

} // OnPaint

/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::DrawPoint()
{
	TimeCountDraw();
	return ;
	// this does the work of "scrolling" the plot to the left
	// and appending a new data point all of the plotting is 
	// directed to the memory based bitmap associated with m_dcPlot
	// the will subsequently be BitBlt'd to the client in OnPaint
  
	int currX, prevX, currY, prevY;
	int currCCUX[CI_MAX], prevCCUX[CI_MAX], currCCUY[CI_MAX], prevCCUY[CI_MAX];
	CPen *oldPen;
	CPen *oldCCUPen[CI_MAX];
	CRect rectCleanUp;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top+1, m_nPlotWidth, m_nPlotHeight, &m_dcPlot, m_rectPlot.left+m_nShiftPixels, m_rectPlot.top+1, SRCCOPY);

		rectCleanUp = m_rectPlot;
		rectCleanUp.left  = rectCleanUp.right - m_nShiftPixels;

		m_dcPlot.FillRect(rectCleanUp, &m_brushBack);

		// draw the next line segement

		// grab the plotting pen
//		oldPen = m_dcPlot.SelectObject(&m_penPlot);

		// move to the previous point
		prevX = m_rectPlot.right - m_nPlotShiftPixels;
		prevY = m_rectPlot.bottom - (long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor);
		m_dcPlot.MoveTo (prevX, prevY);

		// draw to the current point
		currX = m_rectPlot.right-m_nHalfShiftPixels;
		currY = m_rectPlot.bottom -	(long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor);
		m_dcPlot.LineTo (currX, currY);

		// restore the pen 
		m_dcPlot.SelectObject(oldPen);

		// if the data leaks over the upper or lower plot boundaries
		// fill the upper and lower leakage with the background
		// this will facilitate clipping on an as needed basis
		// as opposed to always calling IntersectClipRect
		if ((prevY <= m_rectPlot.top) || (currY <= m_rectPlot.top))
			m_dcPlot.FillRect(CRect(prevX, m_rectClient.top, currX+1, m_rectPlot.top+1), &m_brushBack);
		if ((prevY >= m_rectPlot.bottom) || (currY >= m_rectPlot.bottom))
			m_dcPlot.FillRect(CRect(prevX, m_rectPlot.bottom+1, currX+1, m_rectClient.bottom+1), &m_brushBack);

		// store the current point for connection to the next point
		m_dPreviousPosition = m_dCurrentPosition;

		if(m_bCountryCode)
		{
			for(int i = 0 ; i < CI_MAX ; ++i)
			{
				m_dcCCUPlot[i].BitBlt(m_rectPlot.left, m_rectPlot.top+1, m_nPlotWidth, m_nPlotHeight, &m_dcCCUPlot[i], m_rectPlot.left+m_nShiftPixels, m_rectPlot.top+1, SRCCOPY);
				if(m_dcCCUPlot[i].GetSafeHdc() != NULL)
				{
					//grab the plotting pen
					oldCCUPen[i] = m_dcCCUPlot[i].SelectObject(&m_CCUpen[i]);

					//previous point
					prevCCUX[i] = m_rectPlot.right - m_nPlotShiftPixels;
					prevCCUY[i] = m_rectPlot.bottom - (long)((m_dPreCCUPosition[i] - m_dLowerLimit) * m_dVerticalFactor);		
					m_dcCCUPlot[i].MoveTo(prevCCUX[i], prevCCUY[i]);
					//current point
					currCCUX[i] = m_rectPlot.right-m_nHalfShiftPixels;
					currCCUY[i] = m_rectPlot.bottom -	(long)((m_dCurCCUPosition[i] - m_dLowerLimit) * m_dVerticalFactor);
					m_dcCCUPlot[i].LineTo (currCCUX[i], currCCUY[i]);
					//restore the pen
					m_dcCCUPlot[i].SelectObject(oldCCUPen[i]);

					if ((prevCCUY[i] <= m_rectPlot.top) || (currCCUY[i] <= m_rectPlot.top))
						m_dcCCUPlot[i].FillRect(CRect(prevCCUX[i], m_rectClient.top, currCCUX[i]+1, m_rectPlot.top+1), &m_brushBack);
					if ((prevCCUY[i] >= m_rectPlot.bottom) || (currCCUY[i] >= m_rectPlot.bottom))
						m_dcCCUPlot[i].FillRect(CRect(prevCCUX[i], m_rectPlot.bottom+1, currCCUX[i]+1, m_rectClient.bottom+1), &m_brushBack);

					// store the current point for connection to the next point
					m_dPreCCUPosition[i] = m_dCurCCUPosition[i];
				}
			}
		}
	}
} // end DrawPoint

void COScopeTimeCtrl::DrawPoint(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecound, bool bCCU, double dNewPoint)
{
	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		CRect rectCleanUp = m_rectPlot;
		//시간에 따른 포인트 위치 수정

		float fPosTerm = (m_rectPlot.right - m_rectPlot.left)/24.0;
		rectCleanUp.left = m_rectPlot.left + (fPosTerm*m_stPrevDateTime.wHour + (fPosTerm * m_stPrevDateTime.wMinute /60));
		rectCleanUp.right = m_rectPlot.left + (fPosTerm*iHour + (fPosTerm * iMinute /60));
		rectCleanUp.top = m_rectPlot.bottom - (long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor);
		rectCleanUp.bottom = m_rectPlot.bottom - (long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor);

		if(false ==((rectCleanUp.left == rectCleanUp.right) && (rectCleanUp.top == rectCleanUp.bottom)))
		{
	//		m_dcPlot.FillRect(rectCleanUp, &m_brushBack);

			// grab the plotting pen
			CPen *oldPen = m_dcPlot.SelectObject(&m_penPlot);

			m_dcPlot.MoveTo (rectCleanUp.left, rectCleanUp.top);
			m_dcPlot.LineTo (rectCleanUp.right, rectCleanUp.bottom);

			DateSetting(iYear, iMonth, iDay, m_stPrevDateTime.wDayOfWeek, iHour, iMinute, iSecound, m_stPrevDateTime.wMilliseconds);
			m_dPreviousPosition = m_dCurrentPosition;
			m_dcPlot.SelectObject(oldPen);
		}
	}
	m_bGraph = true;
}



/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// NOTE: OnSize automatically gets called during the setup of the control
  
	GetClientRect(m_rectClient);

	// set some member variables to avoid multiple function calls
	m_nClientHeight = m_rectClient.Height();
	m_nClientWidth  = m_rectClient.Width();

	// the "left" coordinate and "width" will be modified in 
	// InvalidateCtrl to be based on the width of the y axis scaling
	m_rectPlot.left   = 5;  
	m_rectPlot.top    = 5;
	m_rectPlot.right  = m_rectClient.right-10;
	m_rectPlot.bottom = m_rectClient.bottom-20;

	// set some member variables to avoid multiple function calls
	m_nPlotHeight = m_rectPlot.Height();
	m_nPlotWidth  = m_rectPlot.Width();

	// set the scaling factor for now, this can be adjusted 
	// in the SetRange functions
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange; 

} // OnSize


/////////////////////////////////////////////////////////////////////////////
void COScopeTimeCtrl::Reset()
{
	m_dPreviousPosition	= 0;
	m_dCurrentPosition = 0;

	InvalidateCtrl();
}

int COScopeTimeCtrl::GetWidth()
{
	int nWidth = m_rectPlot.right - m_rectPlot.left;
	return nWidth;
}

BOOL COScopeTimeCtrl::PreTranslateMessage(MSG* pMsg)
{
	m_toolTip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg); 
}

int COScopeTimeCtrl::GetRightPixel()
{
	return m_rectPlot.right;
}

int COScopeTimeCtrl::GetLeftPixel()
{
	return m_rectPlot.left;
}


// 초기화
void COScopeTimeCtrl::Init()
{
	m_dPreviousPosition	= 0;
	m_dCurrentPosition = 0;

	InvalidateCtrl();
}


// 인원수
void COScopeTimeCtrl::CountSetting()
{
	CString strTemp;
	// y max
	m_dcGrid.SetTextColor (m_crGridColor);
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_TOP);
	strTemp.Format (_T("%d"), (int)m_dUpperLimit);
	m_dcGrid.TextOut (m_rectPlot.left-4, m_rectPlot.top, strTemp);

	// y min
	m_dcGrid.SetTextAlign (TA_RIGHT|TA_BASELINE);
	strTemp.Format (_T("%d"), (int)m_dLowerLimit);
	m_dcGrid.TextOut (m_rectPlot.left-4, m_rectPlot.bottom, strTemp);

}

// 밑에 시간
void COScopeTimeCtrl::TimeSetting()
{
	float fPosTerm = (m_rectPlot.right - m_rectPlot.left)/24.0;

	CString strTemp;

	for(int iHour = m_dLowerXLimit; iHour <= m_dUpperYLimit; iHour+=2)
	{
		strTemp.Format (_T("%d"), (int)iHour);
		m_dcGrid.TextOut (m_rectPlot.left + 4 + (fPosTerm*iHour), m_rectPlot.bottom+17, strTemp);
	}
}

void COScopeTimeCtrl::TimeCountDraw()
{
	if(false == m_bGraph)
	{
		return ;
	}

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		//시간에 따른 포인트 위치 수정
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		if(m_day != sysTime.wDay)
		{
			// 그래프를 초기화한다. 초기화 루틴을 시작한다.
			Init();
		}

		CRect rectCleanUp = m_rectPlot;

		float fPosTerm = (m_rectPlot.right - m_rectPlot.left)/24.0;
		rectCleanUp.left = m_rectPlot.left + (fPosTerm*m_stPrevDateTime.wHour + (fPosTerm * m_stPrevDateTime.wMinute /60));
		rectCleanUp.right = m_rectPlot.left + (fPosTerm*sysTime.wHour + (fPosTerm * sysTime.wMinute /60));
		rectCleanUp.top = m_rectPlot.bottom - (long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor);
		rectCleanUp.bottom = m_rectPlot.bottom - (long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor);

		if(false ==((rectCleanUp.left == rectCleanUp.right) && (rectCleanUp.top == rectCleanUp.bottom)))
		{
//			m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top+1, m_nPlotWidth, m_nPlotHeight, &m_dcPlot, m_rectPlot.left+m_nShiftPixels, m_rectPlot.top+1, SRCCOPY);

//			m_dcPlot.FillRect(rectCleanUp, &m_brushBack);

			// grab the plotting pen
			CPen *oldPen = m_dcPlot.SelectObject(&m_penPlot);

			m_dcPlot.MoveTo (rectCleanUp.left, rectCleanUp.top);
			m_dcPlot.LineTo (rectCleanUp.right, rectCleanUp.bottom);

			DateSetting(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wDayOfWeek, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
			m_dPreviousPosition = m_dCurrentPosition;
			m_dcPlot.SelectObject(oldPen);
		}

		m_day = sysTime.wDay;
	}
}