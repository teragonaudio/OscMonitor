#include "Options.h"

// LibLo header
#include "lo/lo.h"

// MrsWatson SDK
#include "app/ProgramOption.h"
#include "app/ReturnCodes.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

#include <signal.h>

#define SLEEP_TIME_MS 200

boolByte runloopStarted = false;

static void handleSignal(int signum) {
  logInfo("Caught signal %d, terminating", signum);
  runloopStarted = false;
}

static void errorHandler(int num, const char *msg, const char *where) {
  logWarn("OSC server thread error %d: %s", num, msg);
}

static CharString formatData(const char type, lo_arg *data) {
  CharString result = newCharString();

  switch(type) {
    case LO_INT32:
      sprintf(result->data, "%d", data->i);
      break;
    case LO_INT64:
      sprintf(result->data, "%ld", data->h);
      break;
    case LO_FLOAT:
      sprintf(result->data, "%f", data->f);
      break;
    case LO_FALSE:
      charStringCopyCString(result, "(false)");
      break;
    case LO_TRUE:
      charStringCopyCString(result, "(true)");
      break;
    case LO_STRING:
      charStringCopyCString(result, (char*)data);
      break;
    default:
      sprintf(result->data, "Formatting for OSC '%c' data types", type);
      logUnsupportedFeature(result->data);
      charStringClear(result);
      break;
  }

  return result;
}

static int messageHandler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  CharString formattedData;
  lo_address source = lo_message_get_source(data);
  int i = 0;

  if(argc == 1) {
    formattedData = formatData(types[i], argv[i]);
    logOscMessage(lo_address_get_hostname(source), path, types[i], formattedData);
    freeCharString(formattedData);
  }
  else {
    logDebug("Received OSC bundle with %d parts", argc);
    for(i = 0; i < argc; ++i) {
      formattedData = formatData(types[i], argv[i]);
      logOscMessage(lo_address_get_hostname(source), path, types[i], formattedData);
      freeCharString(formattedData);
    }
    logDebug("End of OSC bundle");
  }
  return 0;
}

int main(int argc, char* argv[]) {
  ProgramOptions options;
  lo_server_thread serverThread = NULL;
  CharString port = newCharStringWithCapacity(kCharStringLengthShort);
  int protocol = LO_UDP;

  options = getProgramOptions();
  programOptionsParseArgs(options, argc, argv);
  initEventLogger();

  if(options->options[kOptionHelp]->enabled) {
    printf("OscMonitor options:\n");
    programOptionsPrintHelp(options, true, 2);
    return RETURN_CODE_NOT_RUN;
  }
  else if(options->options[kOptionVersion]->enabled) {
    printf("OscMonitor version 1.0.0\n");
    return RETURN_CODE_NOT_RUN;
  }

  if(options->options[kOptionVerbose]->enabled) {
    setLogLevel(LOG_DEBUG);
  }
  if(options->options[kOptionTcp]->enabled) {
    protocol = LO_TCP;
  }

  snprintf(port->data, port->capacity, "%d", (int)programOptionsGetNumber(options, kOptionPort));
  serverThread = lo_server_thread_new_with_proto(port->data, protocol, errorHandler);
  if(serverThread == NULL) {
    logError("Could not create OSC server therad");
    return RETURN_CODE_NOT_RUN;
  }

  if(lo_server_thread_add_method(serverThread, NULL, NULL, messageHandler, NULL) == NULL) {
    logError("Could not add method to server thread");
    lo_server_thread_free(serverThread);
    return RETURN_CODE_INTERNAL_ERROR;
  }

  if(lo_server_thread_start(serverThread) != 0) {
    logError("Could not start server thread (port unavailable?)");
    return RETURN_CODE_NOT_RUN;
  }
  else {
    logInfo("Listening for messages on %s", lo_server_thread_get_url(serverThread));

    // Install signal handlers
    signal(SIGHUP, handleSignal);
    signal(SIGINT, handleSignal);
    signal(SIGQUIT, handleSignal);
    signal(SIGKILL, handleSignal);
    signal(SIGTERM, handleSignal);

    // Start the main runloop
    runloopStarted = true;
  }

  while(runloopStarted) {
    sleepMilliseconds(SLEEP_TIME_MS);
  }

  logDebug("Shutting down server thread");
  lo_server_thread_stop(serverThread);
  lo_server_thread_free(serverThread);
  freeProgramOptions(options);
  freeCharString(port);
  return RETURN_CODE_SUCCESS;
}
