#include "s3e/Graphics/Shader.hpp"

#include <fstream>

#include "s3e/Support/Util.hpp"
#include "s3e/Support/log.hpp"

namespace Tom::s3e {
    static std::string shaderSource(const std::string & path) {
        std::ifstream t(path);
        if (!t) {
            Logging::Graphics->error("shaderSource path={} failed to open file",
                                     path);
            return std::string();
        }

        auto start = std::istreambuf_iterator<char>(t);
        auto end = std::istreambuf_iterator<char>();

        return std::string((start), end);
    }

    static bool compileSuccess(GLuint shader) {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        return success != GL_FALSE;
    }

    static bool linkSuccess(GLuint program) {
        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        return success != GL_FALSE;
    }

    static std::string compileError(GLuint shader) {
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

        std::vector<GLchar> errorLog(logSize);
        glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);

        return std::string(errorLog.begin(), errorLog.end());
    }

    static std::string linkError(GLuint program) {
        GLint logSize = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

        std::vector<GLchar> errorLog(logSize);
        glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]);

        return std::string(errorLog.begin(), errorLog.end());
    }

    static GLuint compileShader(GLuint shaderType,
                                const std::string & shaderSource) {

        GLuint shader = glCreateShader(shaderType);
        const GLchar * src = shaderSource.c_str();

        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        return shader;
    }

    Shader::Shader() : program(0) {}

    Shader::~Shader() {
        if (program > 0)
            glDeleteProgram(program);
    }

    bool Shader::loadFromPath(const std::string & vertexPath,
                              const std::string & fragmentPath) {
        Logging::Graphics->debug(
            "loading Shader from paths vertex = \"{}\" fragment = \"{}\"",
            vertexPath, fragmentPath);

        std::string vertexSource = shaderSource(vertexPath);
        std::string fragmentSource = shaderSource(fragmentPath);

        if (vertexSource.empty()) {
            Logging::Graphics->error(
                "vertex shader source could not be loaded from path {}",
                vertexPath);
            return false;
        }

        if (fragmentSource.empty()) {
            Logging::Graphics->error(
                "fragment shader source could not be loaded from path {}",
                fragmentPath);
            return false;
        }

        return loadFromSource(vertexSource, fragmentSource);
    }

    bool Shader::loadFromSource(const std::string & vertexSource,
                                const std::string & fragmentSource) {

        GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        if (!compileSuccess(vShader)) {
            Logging::Graphics->error("failed to compile vertex shader {}: {}",
                                     vShader, compileError(vShader));
            glDeleteShader(vShader);
            return false;
        }

        GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (!compileSuccess(fShader)) {
            Logging::Graphics->error("failed to compile fragment shader {}: {}",
                                     fShader, compileError(fShader));
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            return false;
        }

        program = glCreateProgram();
        glAttachShader(program, vShader);
        glAttachShader(program, fShader);

        glLinkProgram(program);

        glDetachShader(program, vShader);
        glDetachShader(program, fShader);
        glDeleteShader(vShader);
        glDeleteShader(fShader);

        if (!linkSuccess(program)) {
            Logging::Graphics->error("failed to link shader program {}: {}",
                                     program, linkError(program));
            glDeleteProgram(program);
            return false;
        }

        Logging::Graphics->debug("Shader created program={}", program);
        return true;
    }

    void Shader::bind() {
        glUseProgram(program);
    }

    void Shader::unbind() {
        glUseProgram(0);
    }

    GLuint Shader::uniformLocation(const std::string & name) const {
        return glGetUniformLocation(program, name.c_str());
    }

    void Shader::setBool(const std::string & name, bool value) const {
        glUniform1i(uniformLocation(name), (int)value);
    }

    void Shader::setInt(const std::string & name, int value) const {
        glUniform1i(uniformLocation(name), value);
    }
    void Shader::setUInt(const std::string & name, unsigned int value) const {
        glUniform1ui(uniformLocation(name), value);
    }

    void Shader::setFloat(const std::string & name, float value) const {
        glUniform1f(uniformLocation(name), value);
    }

    void Shader::setVec2(const std::string & name, const glm::vec2 & value) const {
        glUniform2fv(uniformLocation(name), 1, &value.x);
    }

    void Shader::setVec3(const std::string & name, const glm::vec3 & value) const {
        glUniform3fv(uniformLocation(name), 1, &value.x);
    }

    void Shader::setVec4(const std::string & name, const glm::vec4 & value) const {
        glUniform4fv(uniformLocation(name), 1, &value.x);
    }

    void Shader::setMat2(const std::string & name, const glm::mat2 & value) const {
        glUniformMatrix2fv(uniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }

    void Shader::setMat3(const std::string & name, const glm::mat3 & value) const {
        glUniformMatrix3fv(uniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }

    void Shader::setMat4(const std::string & name, const glm::mat4 & value) const {
        glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }
}

namespace Tom::s3e {
    static const std ::string defaultVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
out vec3 FragPos;
out vec3 FragNorm;
out vec2 FragTex;
uniform mat4 mvp;
uniform mat4 model;
void main() {
    gl_Position = mvp * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    FragNorm = aNorm;
    FragTex = aTex;
})";

    static const std ::string defaultFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform sampler2D gTexture;
in vec3 FragPos;
in vec3 FragNorm;
in vec2 FragTex;
void main() {
    FragColor = texture(gTexture, FragTex);
    //FragColor = vec4(1, 0, 0, 1);
})";

    Shader::Ptr Shader::defaultShader() {
        auto shader = std::make_shared<Shader>();
        if (shader) {
            shader->loadFromSource(defaultVertexShaderSource,
                                   defaultFragmentShaderSource);
        }
        return shader;
    }

}
