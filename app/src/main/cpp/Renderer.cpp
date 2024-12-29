#include "Renderer.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <memory>
#include <vector>
#include <android/imagedecoder.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "AndroidOut.h"
#include "Shader.h"
#include "Utility.h"
#include "TextureAsset.h"

constexpr float kProjectionHalfHeight = 1.0f;
constexpr float kProjectionNearPlane = 0.1f;
constexpr float kProjectionFarPlane = 100.0f;

#define PRINT_GL_STRING(s) {std::cout << #s": "<< glGetString(s) << std::endl;}
#define PRINT_GL_STRING_AS_LIST(s) { \
    std::istringstream extensionStream((const char *) glGetString(s));\
    std::vector<std::string> extensionList(\
            std::istream_iterator<std::string>{extensionStream},\
            std::istream_iterator<std::string>());\
    std::cout << #s":\n";\
    for (auto& extension: extensionList) {\
        std::cout << extension << "\n";\
    }\
    std::cout << std::endl;\
}

#define CORNFLOWER_BLUE 100 / 255.f, 149 / 255.f, 237 / 255.f, 1
#define LOG_TAG "Renderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* vertex = R"(
    #version 300 es
    layout(location = 0) in vec4 inPosition;
    layout(location = 1) in vec2 inUV;
    uniform mat4 uProjection;
    out vec2 fragUV;
    void main() {
        gl_Position = uProjection * inPosition;
        fragUV = inUV;
    }
)";

const char* fragment = R"(
    #version 300 es
    precision mediump float;
    in vec2 fragUV;
    out vec4 color;
    uniform sampler2D uTexture;
    void main() {
        color = texture(uTexture, fragUV);
    }
)";

void Renderer::handleInput() {
    // Handle the input from the android_app here
    // Assuming you'll get input from the app's input queue, this function could clear the input queue
    // or handle any user actions like touch events, key presses, etc.
    __android_log_print(ANDROID_LOG_INFO, "Renderer", "Handling input...");

    // Example: Clear the input queue (if needed)
    // android_app_clear_input(app_);
}

void Renderer::init() {
    LOGI("Initializing renderer...");
}

Renderer::~Renderer() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
            context_ = EGL_NO_CONTEXT;
        }
        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
            surface_ = EGL_NO_SURFACE;
        }
        eglTerminate(display_);
        display_ = EGL_NO_DISPLAY;
    }
}

void Renderer::updateRenderArea(int width, int height) {
    // Update the render area (resize viewport, etc.)
    glViewport(0, 0, width, height);  // For example, to update OpenGL viewport
}

void Renderer::render() {
    // Ensure valid width and height before calling updateRenderArea
    if (width_ > 0 && height_ > 0) {
        updateRenderArea(width_, height_);
    }

    if (shaderNeedsNewProjectionMatrix_) {
        float projectionMatrix[16] = {0};
        Utility::buildOrthographicMatrix(
                projectionMatrix,
                kProjectionHalfHeight,
                float(width_) / height_,
                kProjectionNearPlane,
                kProjectionFarPlane);

        shader_->setProjectionMatrix(projectionMatrix);
        shaderNeedsNewProjectionMatrix_ = false;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    if (!models_.empty()) {
        for (const auto& model : models_) {
            shader_->drawModel(model);
        }
    }

    eglSwapBuffers(display_, surface_);
}

void Renderer::initRenderer() {
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display_, nullptr, nullptr);

    EGLint numConfigs;
    eglChooseConfig(display_, attribs, nullptr, 0, &numConfigs);

    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display_, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [this](const EGLConfig& config) {
                EGLint red, green, blue, depth;
                return eglGetConfigAttrib(display_, config, EGL_RED_SIZE, &red)
                       && eglGetConfigAttrib(display_, config, EGL_GREEN_SIZE, &green)
                       && eglGetConfigAttrib(display_, config, EGL_BLUE_SIZE, &blue)
                       && eglGetConfigAttrib(display_, config, EGL_DEPTH_SIZE, &depth)
                       && red == 8 && green == 8 && blue == 8 && depth == 24;
            });

    surface_ = eglCreateWindowSurface(display_, config, app_->window, nullptr);

    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    context_ = eglCreateContext(display_, config, nullptr, contextAttribs);

    eglMakeCurrent(display_, surface_, surface_, context_);

    width_ = -1;
    height_ = -1;

    PRINT_GL_STRING(GL_VENDOR);
    PRINT_GL_STRING(GL_RENDERER);
    PRINT_GL_STRING(GL_VERSION);
    PRINT_GL_STRING_AS_LIST(GL_EXTENSIONS);

    shader_ = std::make_unique<Shader>(
            vertex,
            fragment,
            "inPosition",    // Attribute for position
            "inUV",          // Attribute for UV
            "uProjection"    // Uniform for projection matrix
    );
    shader_->activate();
    glClearColor(CORNFLOWER_BLUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    createModels();
}

void Renderer::createModels() {
    auto assetManager = app_->activity->assetManager;
    if (!assetManager) {
        LOGI("Asset manager is null, texture loading failed.");
        return;
    }

    std::vector<Vertex> vertices = {
            Vertex(Vector3{1, 1, 0}, Vector2{0, 0}),
            Vertex(Vector3{-1, 1, 0}, Vector2{1, 0}),
            Vertex(Vector3{-1, -1, 0}, Vector2{1, 1}),
            Vertex(Vector3{1, -1, 0}, Vector2{0, 1})
    };
    std::vector<Index> indices = {0, 1, 2, 0, 2, 3};

    auto spAndroidRobotTexture = TextureAsset::loadAsset(assetManager, "android_robot.png");
    models_.emplace_back(vertices, indices, spAndroidRobotTexture);
}
