#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Shader
{
public:
    unsigned int ID = 0;

    Shader(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath)
    {
        const std::string vertexCode = readSource(vertexPath);
        const std::string fragmentCode = readSource(fragmentPath);

        unsigned int vertex = 0;
        unsigned int fragment = 0;
        try
        {
            vertex = compileShader(GL_VERTEX_SHADER, vertexCode, vertexPath);
            fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentPath);
            ID = linkProgram(vertex, fragment, vertexPath, fragmentPath);
        }
        catch (...)
        {
            if (vertex != 0)
                glDeleteShader(vertex);
            if (fragment != 0)
                glDeleteShader(fragment);
            throw;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    ~Shader()
    {
        if (ID != 0)
            glDeleteProgram(ID);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);   
    }

private:
    static std::string readSource(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file)
            throw std::runtime_error("Failed to open shader file: " + path.string());

        std::ostringstream stream;
        stream << file.rdbuf();
        if (file.bad())
            throw std::runtime_error("Failed while reading shader file: " + path.string());

        std::string source = stream.str();
        if (source.empty())
            throw std::runtime_error("Shader file is empty: " + path.string());
        return source;
    }

    static std::string shaderLog(unsigned int shader)
    {
        int length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> buffer(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<int>(buffer.size()), nullptr, buffer.data());
        return std::string(buffer.data());
    }

    static std::string programLog(unsigned int program)
    {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> buffer(length > 0 ? length : 1, '\0');
        glGetProgramInfoLog(program, static_cast<int>(buffer.size()), nullptr, buffer.data());
        return std::string(buffer.data());
    }

    static unsigned int compileShader(
        unsigned int type,
        const std::string& source,
        const std::filesystem::path& path)
    {
        const char* sourcePointer = source.c_str();
        const unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &sourcePointer, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            const std::string log = shaderLog(shader);
            glDeleteShader(shader);
            throw std::runtime_error(
                "Shader compilation failed for " + path.string() + ":\n" + log);
        }
        return shader;
    }

    static unsigned int linkProgram(
        unsigned int vertex,
        unsigned int fragment,
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath)
    {
        const unsigned int program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        int success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            const std::string log = programLog(program);
            glDeleteProgram(program);
            throw std::runtime_error(
                "Shader program linking failed for " + vertexPath.string()
                + " and " + fragmentPath.string() + ":\n" + log);
        }
        return program;
    }
};
#endif
