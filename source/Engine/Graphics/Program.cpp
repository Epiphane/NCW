// By Thomas Steinke

#include <GL/includes.h>
#include <glm/ext.hpp>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <RGBText/StringHelper.h>

#include "../Core/FileSystemProvider.h"
#include "Program.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

namespace
{

struct Shader
{
public:
   Shader(GLuint id) : id(id), program(0) {};
   ~Shader()
   {
      if (program != 0)
      {
         glDetachShader(program, id);
         glDeleteShader(id);
      }
   }

   void Attach(GLuint programID)
   {
      glAttachShader(programID, id);
      program = programID;
   }

   GLuint id;
   GLuint program;
};

}; // anonymous namespace

// --------------------------------------------------------------------------
// -                           Program Loader                               -
// --------------------------------------------------------------------------

// Considering the shader is used incredibly temporarily, a small optimization would be
// avoid using a unique_ptr. However, it's nice - it guarantees that when the shader
// gets dropped on the floor (either through success, and cleanup or failure) the GL shader
// is also deleted with it.
Maybe<std::unique_ptr<Shader>> LoadShaderSource(const std::string& source, GLenum shaderType)
{
   std::unique_ptr<Shader> shader = std::make_unique<Shader>(glCreateShader(shaderType));

   const char *code_cstr = source.c_str();
   glShaderSource(shader->id, 1, &code_cstr, nullptr);
   glCompileShader(shader->id);

   GLint result = GL_FALSE;
   int infoLogLength;

   // Check Shader
   glGetShaderiv(shader->id, GL_COMPILE_STATUS, &result);
   glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength > 0) {
      char* error = new char[size_t(infoLogLength) + 1];
      CUBEWORLD_SCOPE_EXIT([&] { delete[] error; });

      glGetShaderInfoLog(shader->id, infoLogLength, nullptr, error);

      if (error[0] != '\0') {
         return Failure(error).WithContext("Shader compilation failed");
      }
   }

   CHECK_GL_ERRORS();

   return std::move(shader);
}

Maybe<std::unique_ptr<Shader>> LoadShader(const std::string& filePath, GLenum shaderType)
{
    // Read the Shader code from the file
    Maybe<std::string> maybeCode = FileSystemProvider::Instance().ReadEntireFile(filePath);
    if (!maybeCode)
    {
        return maybeCode.Failure().WithContext("Failed reading shader file");
    }

    return LoadShaderSource(*maybeCode, shaderType);
}

Maybe<std::unique_ptr<Program>> Program::LoadCompute(
    const std::string& computeShaderPath
)
{
    // Read the Shader code from the file
    Maybe<std::string> maybeCode = FileSystemProvider::Instance().ReadEntireFile(computeShaderPath);
    if (!maybeCode)
    {
        return maybeCode.Failure().WithContext("Failed reading shader file");
    }

    return LoadComputeSource(*maybeCode);
}

Maybe<std::unique_ptr<Program>> Program::LoadComputeSource(
    const std::string& computeShaderSource
)
{
    // Another instance of the unique_ptr magic: if we return a failure, then this
    // unique_ptr is deconstructed and we make sure to free the resource.
    std::unique_ptr<Program> program = std::make_unique<Program>(glCreateProgram());

    // Compute shader
    Maybe<std::unique_ptr<Shader>> computeShader = LoadShaderSource(computeShaderSource, GL_COMPUTE_SHADER);
    if (!computeShader)
    {
        return computeShader.Failure().WithContext("Failed loading shader");
    }
    computeShader.Result()->Attach(program->id);

    // Link the program
    glLinkProgram(program->id);

    // Check the program
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(program->id, GL_LINK_STATUS, &result);
    glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        char* error = new char[size_t(infoLogLength) + 1];
        CUBEWORLD_SCOPE_EXIT([&] { delete[] error; });
        glGetProgramInfoLog(program->id, infoLogLength, nullptr, error);

        if (error[0] != '\0') {
            return Failure(error).WithContext("Failed linking program");
        }
    }

    return program;
}

Maybe<std::unique_ptr<Program>> Program::Load(
   const std::string& vertexShaderPath,
   const std::string& geometryShaderPath,
   const std::string& fragmentShaderPath,
   const std::vector<std::string>& interleavedAttributes
)
{
   // Another instance of the unique_ptr magic: if we return a failure, then this
   // unique_ptr is deconstructed and we make sure to free the resource.
   std::unique_ptr<Program> program = std::make_unique<Program>(glCreateProgram());

   // Vertex
   Maybe<std::unique_ptr<Shader>> vertexShader = LoadShader(vertexShaderPath, GL_VERTEX_SHADER);
   if (!vertexShader)
   {
      return vertexShader.Failure().WithContext("Failed loading vertex shader at {path}", vertexShaderPath);
   }
   vertexShader.Result()->Attach(program->id);

   // Geometry
   std::unique_ptr<Shader> geomShader;
   if (!geometryShaderPath.empty())
   {
      Maybe<std::unique_ptr<Shader>> maybeGeomShader = LoadShader(geometryShaderPath, GL_GEOMETRY_SHADER);
      if (!maybeGeomShader)
      {
         return maybeGeomShader.Failure().WithContext("Failed loading geometry shader at {path}", geometryShaderPath);
      }
      geomShader = std::move(*maybeGeomShader);
      geomShader->Attach(program->id);
   }

   // Fragment
   Maybe<std::unique_ptr<Shader>> fragShader = LoadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
   if (!fragShader)
   {
      return fragShader.Failure().WithContext("Failed loading fragment shader at {path}", fragmentShaderPath);
   }
   fragShader.Result()->Attach(program->id);

   if (!interleavedAttributes.empty())
   {
      std::unique_ptr<GLchar*> data{new GLchar*[interleavedAttributes.size()]};
      for (size_t i = 0; i < interleavedAttributes.size(); ++i)
      {
         data.get()[i] = (GLchar*)interleavedAttributes[i].c_str();
      }

      glTransformFeedbackVaryings(program->id, (GLsizei)interleavedAttributes.size(), data.get(), GL_INTERLEAVED_ATTRIBS);
      CHECK_GL_ERRORS();
   }

   // Link the program
   glLinkProgram(program->id);

   // Check the program
   GLint result = GL_FALSE;
   int infoLogLength;
   glGetProgramiv(program->id, GL_LINK_STATUS, &result);
   glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength > 0) {
      char* error = new char[size_t(infoLogLength) + 1];
      CUBEWORLD_SCOPE_EXIT([&] { delete[] error; });
      glGetProgramInfoLog(program->id, infoLogLength, nullptr, error);

      if (error[0] != '\0') {
         return Failure(error).WithContext("Failed linking program");
      }
   }

   // Shaders will automatically detach and clean up here!
   return std::move(program);
}

// --------------------------------------------------------------------------
// -                       Program Implementation                           -
// --------------------------------------------------------------------------

Program::~Program()
{
   if (id != 0)
   {
      glDeleteProgram(id);
   }
}

void Program::Bind()
{
   CheckErrors();
   glUseProgram(id);
}

void Program::Unbind()
{
   CheckErrors();
   for (auto& entry : attributes)
   {
      glDisableVertexAttribArray(entry.second);
   }
   glUseProgram(0);
}

void Program::Uniform1u(const std::string& name, const uint32_t value)
{
   glUniform1ui(Uniform(name), value);
}

void Program::Uniform1i(const std::string& name, const int32_t value)
{
   glUniform1i(Uniform(name), value);
}

void Program::Uniform1ui(const std::string& name, const uint32_t value)
{
    glUniform1ui(Uniform(name), value);
}

void Program::Uniform2i(const std::string& name, const int32_t value1, const int32_t value2)
{
   glUniform2i(Uniform(name), value1, value2);
}

void Program::Uniform3i(const std::string& name, const int32_t value1, const int32_t value2, const int32_t value3)
{
   glUniform3i(Uniform(name), value1, value2, value3);
}

void Program::Uniform3ui(const std::string& name, const uint32_t value1, const uint32_t value2, const uint32_t value3)
{
    glUniform3ui(Uniform(name), value1, value2, value3);
}

void Program::Uniform1f(const std::string& name, const float value)
{
   glUniform1f(Uniform(name), value);
}

void Program::Uniform2f(const std::string& name, const float value1, const float value2)
{
   glUniform2f(Uniform(name), value1, value2);
}

void Program::Uniform3f(const std::string& name, const float value1, const float value2, const float value3)
{
   glUniform3f(Uniform(name), value1, value2, value3);
}

void Program::Uniform4f(const std::string& name, const float value1, const float value2, const float value3, const float value4)
{
   glUniform4f(Uniform(name), value1, value2, value3, value4);
}

void Program::UniformVector3f(const std::string& name, const glm::vec3& vector)
{
   glUniform3fv(Uniform(name), 1, glm::value_ptr(vector));
}

void Program::UniformVector4f(const std::string& name, const glm::vec4& vector)
{
   glUniform4fv(Uniform(name), 1, glm::value_ptr(vector));
}

void Program::UniformMatrix4f(const std::string& name, const glm::mat4& matrix)
{
   glUniformMatrix4fv(Uniform(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

GLuint Program::Attrib(const std::string& name)
{
   size_t oldCursor = attributeCursor;
   while (attributeCursor < attributes.size())
   {
      std::pair<std::string, GLuint>& entry = attributes[attributeCursor++];
      if (entry.first == name)
      {
         return entry.second;
      }
   }

   attributeCursor = 0;
   while (attributeCursor < oldCursor)
   {
      std::pair<std::string, GLuint>& entry = attributes[attributeCursor++];
      if (entry.first == name)
      {
         return entry.second;
      }
   }

   attributeCursor = 0;
   GLint location = glGetAttribLocation(id, name.c_str());
   if (location >= 0)
   {
      attributes.push_back(std::make_pair(name, (GLuint)location));
   }

   CheckErrors();
   return GLuint(location);
}

GLint Program::Uniform(const std::string& name)
{
   size_t oldCursor = uniformCursor;
   while (uniformCursor < uniforms.size())
   {
      std::pair<std::string, GLint>& entry = uniforms[uniformCursor++];
      if (entry.first == name)
      {
         return entry.second;
      }
   }

   uniformCursor = 0;
   while (uniformCursor < oldCursor)
   {
      std::pair<std::string, GLint>& entry = uniforms[uniformCursor++];
      if (entry.first == name)
      {
         return entry.second;
      }
   }

   uniformCursor = 0;
   GLint location = glGetUniformLocation(id, name.c_str());
   if (location >= 0)
   {
      uniforms.push_back(std::make_pair(name, location));
   }

   CheckErrors();
   return GLint(location);
}


}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
