#ifndef __NMPCH_H_5C7BE2B1_CBB4_4f90_AFE8_45733B8612A2__
#define __NMPCH_H_5C7BE2B1_CBB4_4f90_AFE8_45733B8612A2__

#if ( 1500 <= _MSC_VER )			//	VC++ 2008
	#ifndef _CRT_NON_CONFORMING_SWPRINTFS
	#define _CRT_NON_CONFORMING_SWPRINTFS
	#endif
	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif

// standard windows headers
#include <tchar.h>
#include <winsock2.h>

// crt headers
#ifndef STRSAFE_NO_DEPRECATE
#define STRSAFE_NO_DEPRECATE
#endif
#include <strsafe.h>

// standard c / c++ headers
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <time.h>

// for atl conversion function
#ifndef _CONVERSION_DONT_USE_THREAD_LOCALE
#define _CONVERSION_DONT_USE_THREAD_LOCALE
#endif

// only use ATL::CString
#ifndef _WTL_NO_CSTRING
#define _WTL_NO_CSTRING
#endif

// atl headers
/*
#include <atlbase.h>
#include <comdef.h>
#include <atlstr.h>
*/

// stl header
#pragma warning ( disable:4786 )
#pragma warning ( disable:4018 )

// define header
#ifndef _NMCOCLIENTOBJECT
#define _NMCOCLIENTOBJECT
#endif

#include <algorithm>
#include <vector>
#include <map>
#include <deque>

using std::min;
using std::max;

#pragma warning ( default:4786 )
#pragma warning ( default:4018 )

#ifndef check
#define	check( x )
#endif

#include "NMGeneral.h"

#include "NMDataObject.h"
#include "NMFunctionObject.h"
#include "NMEventObject.h"

#include "NMCOHelpers.h"
#include "NMCOClientObject.h"
#include "NMGSGeneralSession.h"
#include "NMCSChatSession.h"
#include "NMGameLogManager.h"

#include "NMPlayFeedManager.h"

#include "NMCONew.h"

#endif	//	#ifndef __NMPCH_H_5C7BE2B1_CBB4_4f90_AFE8_45733B8612A2__
