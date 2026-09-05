#include "Abstract.h"

namespace google_breakpad {

Base::Base(Derived* derived)
    : derived_(derived) {
}

Base::~Base() {
  derived_->DoSomething();
}

#pragma warning(push)
#pragma warning(disable:4355)
// Disable warning C4355: 'this' : used in base member initializer list.
Derived::Derived()
    : Base(this) {  // C4355
}
#pragma warning(pop)

void Derived::DoSomething() {
}

}  // namespace google_breakpad
