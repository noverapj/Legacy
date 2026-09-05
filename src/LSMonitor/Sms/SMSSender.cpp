
#include "stdafx.h"
#include ".\smssender.h"

CSMSSender::CSMSSender(void)
{
}

CSMSSender::~CSMSSender(void)
{
}

bool CSMSSender::Init()
{
	HRESULT hr;
	hr = m_pSoapClient.CreateInstance(__uuidof(SoapClient30));
	if(FAILED(hr))
	{
		LOG.PrintTimeAndLog(0, "SMS:Cannot create SoapClient.[%d]", hr);
		return false;
	}

	try
	{
		CString WsdlFile= _T("http://aspdll.xonda.net/SMSWS/webservice/xSMSWebService.wsdl");
		m_pSoapClient->MSSoapInit2(
			_variant_t(WsdlFile), 
			_T(""), 
			_T(""), 
			_T(""), 
			_T(""));
	}
	catch(_com_error err)
	{
		LOG.PrintTimeAndLog(0, "SMS:Cannot initialize SoapClient.");
		return false;
	}

	return true;
}

bool CSMSSender::SendSMS( const CString &szID, const CString &szPassWord,const CString &szDNSName
						 ,const CString &szSendNumber  ,const CString &szRecieveNumber,const CString &szContents )
{
	
	HRESULT hr;
	DISPID dispid;
	DISPPARAMS dispparams;
	VARIANTARG params[MAX_PARAMS];
	VARIANT result;
	CString ParamText;
	EXCEPINFO ExceptInfo;

	// Get dispatch ID corrisponding to method name.
	OLECHAR *pMethodName = L"SendSMS";
	hr = m_pSoapClient->GetIDsOfNames(IID_NULL, &pMethodName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
	if(FAILED(hr))
	{
		LOG.PrintTimeAndLog(0, "SMS:Cannot get dispatch id of SendSMS method.[%d]", hr);
		return false;
	}

	// change type
	for (int i = 0; i < MAX_PARAMS; i++)
	{
		CString szParamText="";
		switch(i)
		{
		case USER_DATA3:
			break;
		case USER_DATA2:
			break;
		case USER_DATA1:
		    break;
		case RESERVED_DATE:
		    break;
		case MERGE_NAME:
			break;
		case SMS_CONTENTS:
			szParamText = szContents;
			break;
		case RECIEVE_NUMBER:
			szParamText = szRecieveNumber;
		    break;
		case SEND_NUMBER:
			szParamText = szSendNumber;
		    break;
		case DNS_NAME:
			szParamText = szDNSName;
		    break;
		case PASSWORD:
			szParamText = szPassWord;
			break;
		case ID:
			szParamText = szID;
			break;
		}
		if(!ChangeType(params[i], szParamText))
			return false;
	}
	

	// Initialize DISPPARAMS structure.
	dispparams.cArgs = MAX_PARAMS;
	dispparams.cNamedArgs = 0;
	dispparams.rgdispidNamedArgs = NULL;
	dispparams.rgvarg = params;

	// Prepare result variant.
	VariantInit(&result);


	// Invoke the specified method.
	hr = m_pSoapClient->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, &result, &ExceptInfo, NULL);
	if(FAILED(hr))
	{
		LOG.PrintTimeAndLog(0, "SMS:Invoke of SendSMS method failed.");
		return false;
	}
	else
	{

		// Convert result to a string.
		VariantChangeType(&result, &result, 0, VT_BSTR);
		LOG.PrintTimeAndLog(0, "SMS:RESULT:%s", CString(result.bstrVal));
	}

	// Clean up variants.
	VariantClear(&result);
	for(int i = 0 ;i<MAX_PARAMS;i++)
	{
		VariantClear(&params[i]);
	}

	CString szResult = CString(result.bstrVal);
	szResult.MakeLower();
	if(szResult.Find("return_value:true") == 0)
		return true;
	else
		return false;
}

bool CSMSSender::ChangeType( VARIANTARG  &params, const CString &ParamText)
{
	VariantInit(&params);
	params.vt = VT_BSTR;
	params.bstrVal = ParamText.AllocSysString();
	HRESULT hr = VariantChangeType(&params, &params, 0, VT_BSTR);
	if(FAILED(hr))
	{
		LOG.PrintTimeAndLog(0, "SMS:Cannot convert a string.[%d]", hr);
		return false;
	}

	return true;
}