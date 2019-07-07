// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <GL/includes.h>
#include <glm/glm.hpp>

#include <RGBDesignPatterns/Maybe.h>
#include <RGBDesignPatterns/Scope.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

class Program
{
public:
   static Maybe<std::unique_ptr<Program>> Load(
      const std::string& vertexShaderPath,
      const std::string& geometryShaderPath,
      const std::string& fragmentShaderPath,
      const std::vector<std::string>& interleavedAttributes
   );

   inline static Maybe<std::unique_ptr<Program>> Load(
      const std::string& vertexShaderPath,
      const std::string& geometryShaderPath,
      const std::string& fragmentShaderPath
   ) {
      return Load(vertexShaderPath, geometryShaderPath, fragmentShaderPath, {});
   }

   inline static Maybe<std::unique_ptr<Program>> Load(
      const std::string& vertexShaderPath,
      const std::string& fragmentShaderPath
   ) {
      return Load(vertexShaderPath, "", fragmentShaderPath, {});
   }

   void Bind();
   void Unbind();
#define BIND_PROGRAM_IN_SCOPE(program) program->Bind();\
   CUBEWORLD_SCOPE_EXIT([&] { program->Unbind(); });

   // There's so much boilerplate is it really worth defining them all?
   void Uniform1i(const std::string& name, const int value);
   void Uniform2i(const std::string& name, const int value1, const int value2);
   void Uniform3i(const std::string& name, const int value1, const int value2, const int value3);
   void Uniform1f(const std::string& name, const float value);
   void Uniform2f(const std::string& name, const float value1, const float value2);
   void Uniform3f(const std::string& name, const float value1, const float value2, const float value3);
   void Uniform4f(const std::string& name, const float value1, const float value2, const float value3, const float value4);
   void UniformVector3f(const std::string& name, const glm::vec3& vector);
   void UniformVector4f(const std::string& name, const glm::vec4& vector);
   void UniformMatrix4f(const std::string& name, const glm::mat4& matrix);

   //
   // Loads the ID for the specified attribute into memory.
   // Can be used both for preloading all attributes, using the reference later,
   // or called every time the ID is needed.
   //
   // For best performance when preloading, call in the same order you use attributes in.
   //
   GLuint Attrib(const std::string& name);

   //
   // Loads the ID for the specified attribute into memory.
   // Can be used both for preloading all uniforms, using the reference later,
   // or called every time the ID is needed.
   //
   // For best performance when preloading, call in the same order you use uniforms in.
   //
   GLint Uniform(const std::string& name);

   inline void CheckErrors()
   {
#if !NDEBUG
      GLenum error = glGetError();
      assert(error == 0);
#endif
   }

public:
   Program() = delete;
   Program(GLuint program)
      : id(program)
      , attributes{}
      , uniforms{}
      , attributeCursor(0)
      , uniformCursor(0)
   {};
   ~Program();

private:
   GLuint id;

   std::vector<std::pair<std::string, GLuint>> attributes;
   std::vector<std::pair<std::string, GLint>> uniforms;

private:
   // Optimizing
   size_t attributeCursor;
   size_t uniformCursor;
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
