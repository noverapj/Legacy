#include "Singleton.h"
#include "OptionManager.h"

typedef ioSingleton<OptionManager> S_OPTION;
#define OPTION() S_OPTION::instance()