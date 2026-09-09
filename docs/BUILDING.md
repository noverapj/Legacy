# Building

## Requirements

1. **Visual Studio 2022** with the *Desktop development with C++* workload
   **and the "C++ ATL" component** (used by `atltime.h` / LS_HTTP).
2. **Boost 1.84** (prebuilt Windows binaries) installed at `C:\Boost\boost_1_84_0`
   — including the `lib32-msvc-14.3` libs (required by `ls_loginsvr` /
   `ls_relaysvr`, which link boost thread / asio / date_time / filesystem).
3. **vcpkg** with a static 32-bit libcurl (used by `LS_RestAPI`):
   ```powershell
   git clone https://github.com/microsoft/vcpkg D:\Tools\vcpkg
   cd D:\Tools\vcpkg; .\bootstrap-vcpkg.bat
   .\vcpkg.exe install curl:x86-windows-static
   # copy libcurl.lib / libcurl-d.lib + zlib (zs.lib, zsd.lib) into .\lib\
   # and curl headers into .\include\libcurl\
   ```
4. Everything else (OpenSSL import libs, CrashFind, NMCrypt, mini-dump
   tooling) is already included in-tree.

All projects target **Win32** with `PlatformToolset=v143` and
`LanguageStandard=c++17`. No 64-bit or non-Windows builds are supported
(the wire protocol assumes 32-bit x86 layout).

## Build order

Project files live in `win\projects\*.vcxproj` (solutions in `win\`).
Build in this order (each step: Debug **and** Release):

```
 1. iocpSocketDLL          6. LS_HTTP          11. ls_gamesvr
 2. Log                    7. LS_NXSoap        12. ls_filewritesvr
 3. ioINILoader            8. LS_RestAPI       13. ls_loginsvr
 4. FrameTimerDLL          9. ls_dbagent       14. ls_relaysvr
 5. tinyxml              10. ls_billingsvr    (10b. ls_mainsvr)
```

## Notes

- `ls_mainsvr` builds before `ls_gamesvr`; both link the same shared libs.
- `ls_gamesvr` links with `/SAFESEH:NO` (a bundled legacy static library is
  not SAFESEH-compatible).
- Windows SDK libs required by `ls_billingsvr` beyond the defaults:
  `crypt32, secur32, normaliz, wldap32, iphlpapi, bcrypt`.
