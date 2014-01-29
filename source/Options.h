#include "app/ProgramOption.h"

enum {
  kOptionHelp,
  kOptionPort,
  kOptionTcp,
  kOptionVerbose,
  kOptionVersion,
  kNumOptions
} kProgramOptions;

ProgramOptions getProgramOptions();
