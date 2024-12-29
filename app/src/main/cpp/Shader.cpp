#include "Shader.h"
#include "Model.h"
#include <android/log.h>

#define LOG_TAG "Shader"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

Shader::Shader(const char* vertexCode, const char* fragmentCode) {
    programID = glCreateProgram();
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentCode);

    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const char* vertexCode, const char* fragmentCode, const char* positionAttrib, const char* uvAttrib, const char* projectionUniform) {
    // Initialize shaders and other resources here using these arguments
}

GLuint Shader::loadShader(GLenum shaderType, const std::string &shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    const GLchar* shaderRawString = shaderSource.c_str();
    GLint shaderLength = static_cast<GLint>(shaderSource.length());
    glShaderSource(shader, 1, &shaderRawString, &shaderLength);
    glCompileShader(shader);

    GLint shaderCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    if (!shaderCompiled) {
        GLint infoLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

        if (infoLength) {
            GLchar *infoLog = new GLchar[infoLength];
            glGetShaderInfoLog(shader, infoLength, nullptr, infoLog);
            LOGE("Failed to compile shader: %s", infoLog);
            delete[] infoLog;
        }

        glDeleteShader(shader);
        shader = 0;
    }

    return shader;
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::activate() const {
    glUseProgram(program_);
}

void Shader::deactivate() const {
    glUseProgram(0);
}

void Shader::drawModel(const Model &model) const {
    glVertexAttribPointer(position_, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), model.getVertexData());
    glEnableVertexAttribArray(position_);

    glVertexAttribPointer(uv_, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((uint8_t*)model.getVertexData()) + sizeof(Vector3));
    glEnableVertexAttribArray(uv_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model.getTexture().getTextureID());

    glDrawElements(GL_TRIANGLES, model.getIndexCount(), GL_UNSIGNED_SHORT, model.getIndexData());

    glDisableVertexAttribArray(uv_);
    glDisableVertexAttribArray(position_);
}

void Shader::setProjectionMatrix(float* projectionMatrix) const {
    glUniformMatrix4fv(projectionMatrix_, 1, GL_FALSE, projectionMatrix);
}

Shader* Shader::loadShader(
        const std::string &vertexSource,
        const std::string &fragmentSource,
        const std::string &positionAttributeName,
        const std::string &uvAttributeName,
        const std::string &projectionMatrixUniformName) {

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) return nullptr;

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return nullptr;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength) {
                GLchar *log = new GLchar[logLength];
                glGetProgramInfoLog(program, logLength, nullptr, log);
                LOGE("Failed to link program: %s", log);
                delete[] log;
            }
            glDeleteProgram(program);
            return nullptr;
        }

        GLint positionAttrib = glGetAttribLocation(program, positionAttributeName.c_str());
        GLint uvAttrib = glGetAttribLocation(program, uvAttributeName.c_str());
        GLint projectionMatrixUniform = glGetUniformLocation(program, projectionMatrixUniformName.c_str());

        if (positionAttrib != -1 && uvAttrib != -1 && projectionMatrixUniform != -1) {
            Shader* shader = new Shader(program, positionAttrib, uvAttrib, projectionMatrixUniform);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return shader;
        } else {
            glDeleteProgram(program);
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return nullptr;
}
