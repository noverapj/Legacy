#pragma once

#include <string>

const wchar_t* LookupOpcodeName(DWORD dwID);

std::wstring OpcodeNameOrHex(DWORD dwID);

const wchar_t* OpcodeFamily(DWORD dwID);
