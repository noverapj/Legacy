
// guid_string.cc: Convert GUIDs to strings.
//
// See guid_string.h for documentation.

#include <wchar.h>
#include <stdio.h>

#include "StringUtils-inl.h"

#include "GuidString.h"
#include "ExceptionHandler.h"

namespace google_breakpad {

// static
string GUIDString::GUIDToWString(GUID *guid) {
  CHAR guid_string[37];
  sprintf_s(
      guid_string, sizeof(guid_string) / sizeof(guid_string[0]),
      "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
      guid->Data1, guid->Data2, guid->Data3,
      guid->Data4[0], guid->Data4[1], guid->Data4[2],
      guid->Data4[3], guid->Data4[4], guid->Data4[5],
      guid->Data4[6], guid->Data4[7]);


  // remove when VC++7.1 is no longer supported
  guid_string[sizeof(guid_string) / sizeof(guid_string[0]) - 1] = L'\0';

  return string(guid_string);
}

// static
string GUIDString::GUIDToSymbolServerWString(GUID *guid) {
  CHAR guid_string[33];
  sprintf_s(
      guid_string, sizeof(guid_string) / sizeof(guid_string[0]),
      "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
      guid->Data1, guid->Data2, guid->Data3,
      guid->Data4[0], guid->Data4[1], guid->Data4[2],
      guid->Data4[3], guid->Data4[4], guid->Data4[5],
      guid->Data4[6], guid->Data4[7]);

  // remove when VC++7.1 is no longer supported
  guid_string[sizeof(guid_string) / sizeof(guid_string[0]) - 1] = L'\0';

  return string(guid_string);
}

}  // namespace google_breakpad
