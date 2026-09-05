#ifndef CLIENT_WINDOWS_CRASH_GENERATION_MINIDUMP_GENERATOR_H_
#define CLIENT_WINDOWS_CRASH_GENERATION_MINIDUMP_GENERATOR_H_

#include <windows.h>
#include <dbghelp.h>
#include <rpc.h>
#include <list>
#include "MinidumpFormat.h"

namespace google_breakpad {

// Abstraction for various objects and operations needed to generate
// minidump on Windows. This abstraction is useful to hide all the gory
// details for minidump generation and provide a clean interface to
// the clients to generate minidumps.
class MinidumpGenerator {
 public:
  // Creates an instance with the given dump path.
  explicit MinidumpGenerator(const std::string& dump_path);

  ~MinidumpGenerator();

  // Writes the minidump with the given parameters. Stores the
  // dump file path in the dump_path parameter if dump generation
  // succeeds.
  bool WriteMinidump(HANDLE process_handle,
                     DWORD process_id,
                     DWORD thread_id,
                     DWORD requesting_thread_id,
                     EXCEPTION_POINTERS* exception_pointers,
                     MDRawAssertionInfo* assert_info,
                     MINIDUMP_TYPE dump_type,
                     bool is_client_pointers,
                     std::string* dump_path);

  // Writes the minidump with the given parameters. Stores the dump file
  // path in the dump_path (and full_dump_path) parameter if dump
  // generation succeeds. full_dump_path and dump_path can be NULL.
  bool WriteMinidump(HANDLE process_handle,
                     DWORD process_id,
                     DWORD thread_id,
                     DWORD requesting_thread_id,
                     EXCEPTION_POINTERS* exception_pointers,
                     MDRawAssertionInfo* assert_info,
                     MINIDUMP_TYPE dump_type,
                     bool is_client_pointers,
                     std::string* dump_path,
                     std::string* full_dump_path);

  // Writes the minidump with the given parameters. Writes the minidump and
  // full dump to the file handles supplied. This allows the caller to handle
  // the creation of the files for the dump. The file handles are not closed
  // by this function.
  bool WriteMinidump(HANDLE process_handle,
                     DWORD process_id,
                     DWORD thread_id,
                     DWORD requesting_thread_id,
                     EXCEPTION_POINTERS* exception_pointers,
                     MDRawAssertionInfo* assert_info,
                     MINIDUMP_TYPE dump_type,
                     bool is_client_pointers,
                     HANDLE dump_file,
                     HANDLE full_dump_file);

 private:
  // Function pointer type for MiniDumpWriteDump, which is looked up
  // dynamically.
  typedef BOOL (WINAPI* MiniDumpWriteDumpType)(
      HANDLE hProcess,
      DWORD ProcessId,
      HANDLE hFile,
      MINIDUMP_TYPE DumpType,
      CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
      CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
      CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

  // Function pointer type for UuidCreate, which is looked up dynamically.
  typedef RPC_STATUS (RPC_ENTRY* UuidCreateType)(UUID* Uuid);

  // Loads the appropriate DLL lazily in a thread safe way.
  HMODULE GetDbghelpModule();

  // Loads the appropriate DLL and gets a pointer to the MiniDumpWriteDump
  // function lazily and in a thread-safe manner.
  MiniDumpWriteDumpType GetWriteDump();

  // Loads the appropriate DLL lazily in a thread safe way.
  HMODULE GetRpcrt4Module();

  // Loads the appropriate DLL and gets a pointer to the UuidCreate
  // function lazily and in a thread-safe manner.
  UuidCreateType GetCreateUuid();

  // Returns the path for the file to write dump to.
  bool GenerateDumpFilePath(std::string* file_path);

  // Handle to dynamically loaded DbgHelp.dll.
  HMODULE dbghelp_module_;

  // Pointer to the MiniDumpWriteDump function.
  MiniDumpWriteDumpType write_dump_;

  // Handle to dynamically loaded rpcrt4.dll.
  HMODULE rpcrt4_module_;

  // Pointer to the UuidCreate function.
  UuidCreateType create_uuid_;

  // Folder path to store dump files.
  std::string dump_path_;

  // Critical section to sychronize action of loading modules dynamically.
  CRITICAL_SECTION module_load_sync_;

  // Critical section to synchronize action of dynamically getting function
  // addresses from modules.
  CRITICAL_SECTION get_proc_address_sync_;
};

}  // namespace google_breakpad

#endif  // CLIENT_WINDOWS_CRASH_GENERATION_MINIDUMP_GENERATOR_H_
