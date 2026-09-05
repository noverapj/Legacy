// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#include <SDKDDKVer.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include "../include/common.h"

extern void	Trace( const TCHAR *format, ... );
extern void Debug( const TCHAR *format, ... );
extern void Information( const TCHAR *format, ... );
extern void	Debug();

namespace std { typedef std::basic_string<TCHAR> tstring; }
