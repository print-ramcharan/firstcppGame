// Game.cpp
#include "Game.h"
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "FirstGame"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

bool isGameRunning = false;
bool isGamePaused = false;
float playerX = 0.0f;
float playerY = 0.0f;


extern "C" JNIEXPORT void JNICALL
Java_com_example_firstgame_MainActivity_initGame(JNIEnv* env, jobject /* this */) {
    LOGI("Initializing game...");
    playerX = 100.0f;
    playerY = 200.0f;
    isGameRunning = true;
    isGamePaused = false;
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_firstgame_MainActivity_updateGame(JNIEnv* env, jobject) {
    if (isGameRunning) {
        LOGI("Updating game...");
        playerX += 1.0f;
        if (playerX > 800.0f) playerX = 0.0f;
    } else {
        LOGI("Game is not running.");
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_firstgame_MainActivity_startGame(JNIEnv* env, jobject /* this */) {
    isGameRunning = true;
    LOGI("Game started.");
}


// JNI function to pause the game
extern "C" JNIEXPORT void JNICALL
Java_com_example_firstgame_MainActivity_pauseGame(JNIEnv* env, jobject /* this */) {
    if (isGameRunning && !isGamePaused) {
        isGamePaused = true;
        LOGI("Game paused.");
    }
}

// JNI function to resume the game
extern "C" JNIEXPORT void JNICALL
Java_com_example_firstgame_MainActivity_resumeGame(JNIEnv* env, jobject /* this */) {
    if (isGameRunning && isGamePaused) {
        isGamePaused = false;
        LOGI("Game resumed.");
    }
}

// JNI function to stop the game
extern "C" JNIEXPORT void JNICALL
Java_com_example_firstgame_MainActivity_stopGame(JNIEnv* env, jobject /* this */) {
    isGameRunning = false;
    isGamePaused = false;
    LOGI("Game stopped.");
}
