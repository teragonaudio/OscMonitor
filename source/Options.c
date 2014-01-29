#include "Options.h"

ProgramOptions getProgramOptions() {
  ProgramOptions options = newProgramOptions(kNumOptions);

  programOptionsAdd(options, newProgramOptionWithName(kOptionHelp, "help", "Show application help", true,
                                                      kProgramOptionTypeEmpty, kProgramOptionArgumentTypeNone));
  programOptionsAdd(options, newProgramOptionWithName(kOptionPort, "port", "Port number to listen on", true,
                                                      kProgramOptionTypeNumber, kProgramOptionArgumentTypeRequired));
  programOptionsSetNumber(options, kOptionPort, 7000);
  programOptionsAdd(options, newProgramOptionWithName(kOptionTcp, "tcp", "Use TCP connection instead of UDP", true,
                                                      kProgramOptionTypeEmpty, kProgramOptionArgumentTypeNone));
  programOptionsAdd(options, newProgramOptionWithName(kOptionVerbose, "verbose", "Use verbose logging", true,
                                                      kProgramOptionTypeEmpty, kProgramOptionArgumentTypeNone));
  programOptionsAdd(options, newProgramOptionWithName(kOptionVersion, "version", "Show application version", true,
                                                      kProgramOptionTypeEmpty, kProgramOptionArgumentTypeNone));

  return options;
}
