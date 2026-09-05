#ifndef CLIENT_WINDOWS_TESTS_CRASH_GENERATION_APP_ABSTRACT_CLASS_H__
#define CLIENT_WINDOWS_TESTS_CRASH_GENERATION_APP_ABSTRACT_CLASS_H__

namespace google_breakpad {

// Dummy classes to help generate a pure call violation.

class Derived;

class Base {
 public:
  Base(Derived* derived);
  virtual ~Base();
  virtual void DoSomething() = 0;

 private:
  Derived* derived_;
};

class Derived : public Base {
 public:
  Derived();
  virtual void DoSomething();
};

}  // namespace google_breakpad

#endif  // CLIENT_WINDOWS_TESTS_CRASH_GENERATION_APP_CRASH_GENERATION_APP_H__
