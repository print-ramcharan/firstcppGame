#include "AndroidOut.h"
#include <android/log.h>

#define LOG_TAG "AndroidOut"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Global instance of AndroidOut for use in logging
std::ostream& aout = *new std::ostream(new AndroidOut(LOG_TAG));

void AndroidOut::logInfo(const char* message) {
    LOGI("%s", message);  // This logs directly to Android Logcat
}
