#include "OscMonitor.h"
#include "lo/lo.h"

// MrsWatson SDK
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
      ("Unknown type!");
      break;
  }

  return result;
}

static int messageHandler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  CharString formattedData;
  lo_address source = lo_message_get_source(data);
  int i;

  for(i = 0; i < argc; ++i) {
    formattedData = formatData(types[i], argv[i]);
    logInfo("%s '%s' %c %s", lo_address_get_url(source), path, types[i], formattedData->data);
    freeCharString(formattedData);
  }
  return 0;
}

int main(int argc, char* argv[]) {
  lo_server_thread serverThread = NULL;

  initEventLogger();

  serverThread = lo_server_thread_new_with_proto("7000", LO_UDP, errorHandler);
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

  lo_server_thread_stop(serverThread);
  lo_server_thread_free(serverThread);
  return RETURN_CODE_SUCCESS;
}
