#ifndef CLIENT_WINDOWS_COMMON_AUTO_CRITICAL_SECTION_H__
#define CLIENT_WINDOWS_COMMON_AUTO_CRITICAL_SECTION_H__

#include <Windows.h>

namespace google_breakpad {

// Automatically enters the critical section in the constructor and leaves
// the critical section in the destructor.
class AutoCriticalSection {
 public:
  // Creates a new instance with the given critical section object
  // and enters the critical section immediately.
  explicit AutoCriticalSection(CRITICAL_SECTION* cs) : cs_(cs), taken_(false) {
    assert(cs_);
    Acquire();
  }

  // Destructor: leaves the critical section.
  ~AutoCriticalSection() {
    if (taken_) {
      Release();
    }
  }

  // Enters the critical section. Recursive Acquire() calls are not allowed.
  void Acquire() {
    assert(!taken_);
    EnterCriticalSection(cs_);
    taken_ = true;
  }

  // Leaves the critical section. The caller should not call Release() unless
  // the critical seciton has been entered already.
  void Release() {
    assert(taken_);
    taken_ = false;
    LeaveCriticalSection(cs_);
  }

 private:
  // Disable copy ctor and operator=.
  AutoCriticalSection(const AutoCriticalSection&);
  AutoCriticalSection& operator=(const AutoCriticalSection&);

  CRITICAL_SECTION* cs_;
  bool taken_;
};

}  // namespace google_breakpad

#endif  // CLIENT_WINDOWS_COMMON_AUTO_CRITICAL_SECTION_H__
