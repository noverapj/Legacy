a::#d:
#define 
a::#u:
#undef 
a::#p:
#pragma 
a::#in:
#include "$end$"
a::#in:
#include <$end$>
a::#in:
#include <strsafe.h>

a::#im:
#import "$end$"
a::#im:
#import <$end$>
a:#ifdef ... #endif:#if:
#ifdef $end$
$selected$
#endif
a:#if 0 ... #endif::
#if 0
$selected$$end$
#endif
a:#ifdef SHIPPING ... #endif:#if:
#ifdef SHIPPING
$selected$
#endif
a:#ifdef SHIPPING ... #else   #endif:#if:
#ifdef SHIPPING
$selected$
#else
#endif
a:#ifdef _ADMIN_ ... #endif:#if:
#ifdef _ADMIN_
$selected$
#endif
a:#ifndef ... #endif:#ifn:
#ifndef $end$
$selected$
#endif
a::#e:
#else
a::#en:
#endif
a::A:
ASSERT($end$)
a::b:
bool
a::DW:
DWORD
a::r:
return
a::T:
TRUE
a::t:
true
a::t:
==
a::F:
FALSE
a::f:
false
a::f:
!=
a::fl:
float
a::HA:
HANDLE
a::HI:
HINSTANCE
a::HR:
HRESULT
a::H:
HWND
a::ll:
long long
a::LP:
LPARAM
a::LPB:
LPBYTE
a::LPC:
LPCTSTR
a::LPT:
LPTSTR
a::LR:
LRESULT
a::TC:
TCHAR
a::U:
UINT
a::ui:
unsigned int
a::ul:
unsigned long
a::UL:
ULONG
a::W:
WORD
a::WP:
WPARAM
a::N:
NULL
a::nn:
!= NULL
a::n0:
!= 0
a::nt:
!= true
a::nf:
!= false
a::tn:
== NULL
a::t0:
== 0
a::tt:
== true
a::tf:
== false
a:GUID IMPLEMENT_OLECREATE:guid:
// {$GUID_STRING$} 
IMPLEMENT_OLECREATE($GUID_Class$, $GUID_ExternalName$, 
$GUID_DEFINITION$);

a:DEFINE_GUID:guid:
// {$GUID_STRING$} 
DEFINE_GUID($GUID_Name$, 
$GUID_DEFINITION$);

a:GUID struct instance:guid:
// {$GUID_STRING$} 
static const GUID $GUID_InstanceName$ = 
{ $GUID_STRUCT$ };

a:GUID string:guid:
"{$GUID_STRING$}"
a:IDL uuid:uuid:
uuid($GUID_STRING$)
a::usi:
using namespace $end$;

a::class:
class $end$
{
public:
protected:
private:
};

a:class with prompt for name:class:
class $Class_name$
{
public:
       $end$
protected:
private:
public:
	$Class_name$();
	virtual ~$Class_name$();
};

a:class with ioWnd:class:
class $Class_name$ : public ioWnd
{
public:
	enum
	{
		
	};
protected:
	$end$
public:

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:	
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	$Class_name$();
	virtual ~$Class_name$();
};

a:class with ioWnd CPP:class:
$Class_name$::$Class_name$()
{

}

$Class_name$::~$Class_name$()
{

}

void $Class_name$::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
}

void $Class_name$::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	//ioMovingWnd::OnProcess( fTimePerSec );
}

void $Class_name$::iwm_create()
{

}

void $Class_name$::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case :
		if( cmd == IOBN_BTNUP )
		{

		}
		break;
	case :
		if( cmd == IOBN_BTNUP )
		{
	
		}
		break;
	case :
		if( cmd == IOBN_BTNUP )
		{

		}
		break;
	}
}

void $Class_name$::iwm_show()
{

}

void $Class_name$::iwm_hide()
{

}

void $Class_name$::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "" )
	{
		SAFEDELETE( );
		= pImage;
	}
	else if( szType == "" )
	{
		SAFEDELETE( );
		= pImage;
	}
	else if( szType == "" )
	{
		SAFEDELETE( );
		= pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void $Class_name$::ParseExtraInfo( ioXMLElement &xElement )
{
	= xElement.GetStringAttribute( "" );
	= xElement.GetIntAttribute( "" );
}

a:dynamic cast,  run code on valid cast:dyna:
$New_type$ *$New_pointer$ = dynamic_cast<$New_type$ *>($Cast_this$);
if($New_pointer$)
{
	$end$
}

a:dynamic cast, return on invalid cast:dyna:
$New_type$ *$New_pointer$ = dynamic_cast<$New_type$ *>($Cast_this$);
if(!$New_pointer$)
	return;

a:static cast,  run code on valid cast:stat:
$New_type$ *$New_pointer$ = static_cast<$New_type$ *>($Cast_this$);
if($New_pointer$)
{
	$end$
}

a:static cast, return on invalid cast:stat:
$New_type$ *$New_pointer$ = static_cast<$New_type$ *>($Cast_this$);
if($New_pointer$ == 0)
	return;

a::struct:
struct $end$ 
{
};

a::switch:
switch ($end$)
{
	$selected$
}

a::switch:
switch ($end$)
{
case :
	break;
}

a:switch default:switch:
switch($end$)
{
case :
	break;
default:
    break;
}

a:://-:
// $end$ [$MONTH$/$DAY$/$YEAR$ %USERNAME%]
a::///:
//////////////////////////////////////////////////////////////////////////

a::/*-:
/*
 *	$end$
 */
a::/**:
/************************************************************************/
/* $end$                                                                     */
/************************************************************************/
a:atoi(...)::
atoi($selected$)$end$
a:_T(...)::
_T($selected$)$end$
a:_T():tc:
_T($end$)
a:if () { ... }:if:
if ($end$)
{
	$selected$
}

a:if () { ... } else { }:if:
if ($end$)
{
	$selected$
} 
else
{
}

a:if () { ... } else if{ }:if:
if ($end$)
{
	$selected$
} 
else if()
{
}

a:if () { } else { ... }::
if ($end$)
{
} 
else
{
	$selected$
}

a:while () { ... }:while:
while ($end$)
{
	$selected$
}

a:while STL erase () { ... }:while:
$container_type$::iterator iter = $container$.begin();
while ( iter != $container$.end() )
{
	if($end$)
	{
		++iter;
	}
	else
	{
		iter = $container$.erase( iter );
	}
}

a:for () { ... }:for:
for ($end$)
{
	$selected$
}

a:for loop  forward:forr:
for (int $Index$ = 0; $Index$ < $Length$ ; $Index$++)
{
	$end$
}

a:for loop reverse:forr:
for (int $Index$ = $Length$ - 1; $Index$ >= 0 ; $Index$--)
{
	$end$
}

a:for loop across STL container with  iterator:forr:
for($container_type$::iterator iter = $container$.begin(); iter != $container$.end(); ++iter)
{
    $end$
}
a:for loop across STL container with const_iterator:forr:
for($container_type$::const_iterator iter = $container$.begin(); iter != $container$.end(); ++iter)
{
    $end$
}

a:for loop across STL container with  iterator(delete):forr:
for($container_type$::iterator iter = $container$.begin(); iter != $container$.end(); ++iter)
{
    delete *iter;$end$
}

a:for loop across STL container with  iterator(SAFEDELETE):forr:
for($container_type$::iterator iter = $container$.begin(); iter != $container$.end(); ++iter)
{
    SAFEDELETE( *iter );$end$
}

a:for loop across STL container with  iterator(erase):forr:
for($container_type$::iterator iter = $container$.begin(); iter != $container$.end(); ++iter)
{
    $container$.erase( iter );
    return;$end$
}
a:do { ... } while ():do:
do 
{
	$selected$
} while ($end$);

a:try { ... } catch {} catch {} catch {}:try:
try
{
	$selected$
}
catch (CMemoryException* e)
{
	$end$
}
catch (CFileException* e)
{
}
catch (CException* e)
{
}

a:TRY { ... } CATCH {}:TRY:
TRY 
{
	$selected$
}
CATCH (CMemoryException, e)
{
	$end$
}
END_CATCH

a:#ifdef guard in a header:#if:
#ifndef __$FILE_BASE$_h__
#define __$FILE_BASE$_h__

$selected$
#endif // __$FILE_BASE$_h__

a:File header detailed::
/********************************************************************
	created:	$DATE$
	created:	$DAY$:$MONTH$:$YEAR$   $HOUR$:$MINUTE$
	filename: 	$FILE$
	file path:	$FILE_PATH$
	file base:	$FILE_BASE$
	file ext:	$FILE_EXT$
	author:		$Author$
	
	purpose:	$end$
*********************************************************************/

a:Win32 standard application::
#include <windows.h>
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("$end$");
    HWND         hwnd;
    MSG          msg;
    WNDCLASSEX   wndclassex = {0};
    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = WndProc;
    wndclassex.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = 0;
    wndclassex.hInstance     = hInstance;
    wndclassex.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclassex.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclassex.lpszMenuName  = NULL;
    wndclassex.lpszClassName = szAppName;
    wndclassex.hIconSm       = wndclassex.hIcon;
	
    if (!RegisterClassEx (&wndclassex))
    {
        MessageBox (NULL, TEXT ("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
        return 0;
    }
    hwnd = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, 
		                  szAppName, 
        		          TEXT ("WindowTitle"),
                		  WS_OVERLAPPEDWINDOW,
		                  CW_USEDEFAULT, 
        		          CW_USEDEFAULT, 
                		  CW_USEDEFAULT, 
		                  CW_USEDEFAULT, 
        		          NULL, 
                		  NULL, 
		                  hInstance,
        		          NULL); 
						  
    ShowWindow (hwnd, iCmdShow);
    UpdateWindow (hwnd);
	
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        return (0);
		
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        TextOut (hdc, 0, 0, "A Window!", 27);
        EndPaint (hwnd, &ps);
        return (0);
		
    case WM_DESTROY:
        PostQuitMessage (0);
        return (0);
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

a:Dialog procedure::
BOOL CALLBACK $end$ (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
        {
			
        }
		return (TRUE);
	case WM_CLOSE:
        {
            EndDialog(hDlg,0);
        }
		return (TRUE);
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
            {
                SendMessage(hDlg, WM_CLOSE, 0, 0);
            }
            return (TRUE);
		case IDOK:
            {
                
            }
			return (TRUE);
		}
		return (FALSE);
	}
	return (FALSE);
}

a:Window procedure::
LRESULT CALLBACK $end$ (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        return (0);
		
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        
        EndPaint (hwnd, &ps);
        return (0);
		
    case WM_DESTROY:
        PostQuitMessage (0);
        return (0);
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

readme:
VA Snippet used for refactoring: Change Signature, Create Implementation, and Move Implementation to Source File.
Delete this item to restore the default upon next use.

a:Refactor Create Implementation::

$SymbolType$ $SymbolContext$( $ParameterList$ ) $MethodQualifier$
{
	$end$$MethodBody$
}

readme:
VA Snippet used for refactoring.
Delete this item to restore the default upon next use.

a:Refactor Document Method::
//************************************
// Method:    $SymbolName$
// FullName:  $SymbolContext$
// Access:    $SymbolVirtual$$SymbolPrivileges$$SymbolStatic$
// Returns:   $SymbolType$
// Qualifier: $MethodQualifier$
// Parameter: $MethodArg$
//************************************

readme:
VA Snippet used for refactoring.
Delete this item to restore the default upon next use.

a:Refactor Encapsulate Field::
	$end$$SymbolType$ Get$GeneratedPropertyName$() const { return $SymbolName$; }
	void Set$GeneratedPropertyName$($SymbolType$ $GeneratedPropertyName$) { $SymbolName$ = $GeneratedPropertyName$; }

readme:
VA Snippet used for refactoring.
Delete this item to restore the default upon next use.

a:Refactor Extract Method::

$end$$SymbolType$ $SymbolContext$( $ParameterList$ ) $MethodQualifier$
{
	$MethodBody$
}

readme:
VA Snippet used by Surround With #ifdef.
Delete this item to restore the default upon next use.

a:#ifdef (VA X):#if:
#ifdef $condition=_DEBUG$$end$
$selected$
#endif // $condition$

readme:
VA Snippet used by Surround With #region.
Delete this item to restore the default upon next use.

a:#region (VA X):#r:
#pragma region $end$$regionName$
$selected$
#pragma endregion $regionName$

readme:
Delete this item to restore the default when the IDE starts.

a:{...}::
{
	$end$$selected$
}

readme:
Delete this item to restore the default when the IDE starts.

a:(...)::
($selected$)

readme:
VA Snippet used for suggestions in loops.
Delete this item to restore the default upon next use.

a:SuggestionsForType loop::
continue;
break;


readme:
VA Snippet used for suggestions in switch statements.
Delete this item to restore the default upon next use.

a:SuggestionsForType switch::
case 
default:
break;


readme:
VA Snippet used for suggestions in class definitions.
Delete this item to restore the default upon next use.

a:SuggestionsForType class::
public:
private:
protected:
virtual
void
bool
string
static
const


readme:
VA Snippet used for suggestions of type bool.
Delete this item to restore the default upon next use.

a:SuggestionsForType bool::
true
false


readme:
VA Snippet used for suggestions of type BOOL.
Delete this item to restore the default upon next use.

a:SuggestionsForType BOOL::
TRUE
FALSE


readme:
VA Snippet used for suggestions of type HRESULT.
Delete this item to restore the default upon next use.

a:SuggestionsForType HRESULT::
S_OK
S_FALSE


readme:
VA Snippet used for suggestions of type VARIANT_BOOL.
Delete this item to restore the default upon next use.

a:SuggestionsForType VARIANT_BOOL::
VARIANT_TRUE
VARIANT_FALSE


readme:
VA Snippet used for suggestions of type HANDLE.
Delete this item to restore the default upon next use.

a:SuggestionsForType HANDLE::
INVALID_HANDLE_VALUE
NULL


readme:
VA Snippet used for refactoring: Create From Usage and Implement Interface.
Delete this item to restore the default upon next use.

a:Refactor Create From Usage Method Body::
throw std::exception("The method or operation is not implemented.");
