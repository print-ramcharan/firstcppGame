#include "Utility.h"
#include "AndroidOut.h"

#include <GLES3/gl3.h>
#include <android/log.h>

#define LOG_TAG "Utility"
#define LOG_GL_ERROR(e) case e: __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "GL Error: %s", #e); break;

float Utility::clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

bool Utility::checkAndLogGlError(bool alwaysLog) {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        if (alwaysLog) {
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "No GL error");
        }
        return true;
    } else {
        switch (error) {
            LOG_GL_ERROR(GL_INVALID_ENUM);
            LOG_GL_ERROR(GL_INVALID_VALUE);
            LOG_GL_ERROR(GL_INVALID_OPERATION);
            LOG_GL_ERROR(GL_INVALID_FRAMEBUFFER_OPERATION);
            LOG_GL_ERROR(GL_OUT_OF_MEMORY);
            default:
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Unknown GL error: %d", error);
        }
        return false;
    }
}

float* Utility::buildOrthographicMatrix(float* outMatrix, float halfHeight, float aspect, float near, float far) {
    float halfWidth = halfHeight * aspect;

    // column 1
    outMatrix[0] = 1.f / halfWidth;
    outMatrix[1] = 0.f;
    outMatrix[2] = 0.f;
    outMatrix[3] = 0.f;

    // column 2
    outMatrix[4] = 0.f;
    outMatrix[5] = 1.f / halfHeight;
    outMatrix[6] = 0.f;
    outMatrix[7] = 0.f;

    // column 3
    outMatrix[8] = 0.f;
    outMatrix[9] = 0.f;
    outMatrix[10] = -2.f / (far - near);
    outMatrix[11] = -(far + near) / (far - near);

    // column 4
    outMatrix[12] = 0.f;
    outMatrix[13] = 0.f;
    outMatrix[14] = 0.f;
    outMatrix[15] = 1.f;

    return outMatrix;
}

float* Utility::buildIdentityMatrix(float* outMatrix) {
    // column 1
    outMatrix[0] = 1.f;
    outMatrix[1] = 0.f;
    outMatrix[2] = 0.f;
    outMatrix[3] = 0.f;

    // column 2
    outMatrix[4] = 0.f;
    outMatrix[5] = 1.f;
    outMatrix[6] = 0.f;
    outMatrix[7] = 0.f;

    // column 3
    outMatrix[8] = 0.f;
    outMatrix[9] = 0.f;
    outMatrix[10] = 1.f;
    outMatrix[11] = 0.f;

    // column 4
    outMatrix[12] = 0.f;
    outMatrix[13] = 0.f;
    outMatrix[14] = 0.f;
    outMatrix[15] = 1.f;

    return outMatrix;
}
