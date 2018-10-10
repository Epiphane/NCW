// By Thomas Steinke

#include <cassert>
#include <fstream>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <stdio.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Core/Scope.h>

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
Maybe<std::unique_ptr<Shader>> LoadShader(const std::string& filePath, GLenum shaderType) {
   std::unique_ptr<Shader> shader = std::make_unique<Shader>(glCreateShader(shaderType));

   // Read the Shader code from the file
   std::ifstream file(filePath);
   std::string code;

   file.seekg(0, std::ios::end);
   code.reserve(static_cast<size_t>(file.tellg()));
   file.seekg(0, std::ios::beg);

   code.assign((std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>());

#if !NDEBUG
   LOG_DEBUG("Compiling shader %1", filePath);
   LOG_DEBUG("----------------------------------------------");
#endif

   const char *code_cstr = code.c_str();
   glShaderSource(shader->id, 1, &code_cstr, nullptr);
   glCompileShader(shader->id);

   GLint result = GL_FALSE;
   int infoLogLength;

   // Check Shader
   glGetShaderiv(shader->id, GL_COMPILE_STATUS, &result);
   glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength > 0) {
      char* error = new char[infoLogLength + 1];
      CUBEWORLD_SCOPE_EXIT([&] { delete[] error; });

      glGetShaderInfoLog(shader->id, infoLogLength, nullptr, error);

      if (error[0] != '\0') {
         return Failure(error).WithContext("Shader compilation failed");
      }
   }

   GLenum error = glGetError();
   assert(error == 0);

   return std::move(shader);
}

Maybe<std::unique_ptr<Program>> Program::Load(
   const std::string& vertexShaderPath,
   const std::string& geometryShaderPath,
   const std::string& fragmentShaderPath
)
{
   // Another instance of the unique_ptr magic: if we return a failure, then this
   // unique_ptr is deconstructed and we make sure to free the resource.
   std::unique_ptr<Program> program = std::make_unique<Program>(glCreateProgram());

   // Vertex
   Maybe<std::unique_ptr<Shader>> vertexShader = LoadShader(vertexShaderPath, GL_VERTEX_SHADER);
   if (!vertexShader)
   {
      return vertexShader.Failure().WithContext("Failed loading vertex shader");
   }
   vertexShader.Result()->Attach(program->id);

   // Geometry
   std::unique_ptr<Shader> geomShader;
   if (!geometryShaderPath.empty())
   {
      Maybe<std::unique_ptr<Shader>> maybeGeomShader = LoadShader(geometryShaderPath, GL_GEOMETRY_SHADER);
      if (!maybeGeomShader)
      {
         return maybeGeomShader.Failure().WithContext("Failed loading geometry shader");
      }
      geomShader = std::move(*maybeGeomShader);
      geomShader->Attach(program->id);
   }

   // Fragment
   Maybe<std::unique_ptr<Shader>> fragShader = LoadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
   if (!fragShader)
   {
      return fragShader.Failure().WithContext("Failed loading vertex shader");
   }
   fragShader.Result()->Attach(program->id);

   // Link the program
   glLinkProgram(program->id);

   // Check the program
   GLint result = GL_FALSE;
   int infoLogLength;
   glGetProgramiv(program->id, GL_LINK_STATUS, &result);
   glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength > 0) {
      char* error = new char[infoLogLength + 1];
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

void Program::Uniform1i(const std::string& name, const int value)
{
   glUniform1i(Uniform(name), value);
}

void Program::Uniform2i(const std::string& name, const int value1, const int value2)
{
   glUniform2i(Uniform(name), value1, value2);
}

void Program::Uniform3i(const std::string& name, const int value1, const int value2, const int value3)
{
   glUniform3i(Uniform(name), value1, value2, value3);
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
   GLuint location = glGetAttribLocation(id, name.c_str());
   attributes.push_back(std::make_pair(name, location));

   CheckErrors();
   return location;
}

GLuint Program::Uniform(const std::string& name)
{
   size_t oldCursor = uniformCursor;
   while (uniformCursor < uniforms.size())
   {
      std::pair<std::string, GLuint>& entry = uniforms[uniformCursor++];
      if (entry.first == name)
      {
         return entry.second;
      }
   }

   uniformCursor = 0;
   while (uniformCursor < oldCursor)
   {
      std::pair<std::string, GLuint>& entry = uniforms[uniformCursor++];
      if (entry.first == name)
      {
         return entry.second;
      }
   }

   uniformCursor = 0;
   GLuint location = glGetUniformLocation(id, name.c_str());
   uniforms.push_back(std::make_pair(name, location));

   CheckErrors();
   return location;
}


}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
