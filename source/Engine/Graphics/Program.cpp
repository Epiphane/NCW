// By Thomas Steinke

#include <cassert>
#include <fstream>
#include <glad/glad.h>
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

GLuint LoadShader(const std::string& filePath, GLenum shaderType) {
   GLuint shaderID = glCreateShader(shaderType);

   // Read the Shader code from the file
   std::ifstream file(filePath);
   std::string code;

   file.seekg(0, std::ios::end);
   code.reserve(static_cast<size_t>(file.tellg()));
   file.seekg(0, std::ios::beg);

   code.assign((std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>());

   const char *code_cstr = code.c_str();
   glShaderSource(shaderID, 1, &code_cstr, nullptr);
   glCompileShader(shaderID);

   GLint result = GL_FALSE;
   int infoLogLength;

   // Check Shader
   glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
   glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength > 0) {
      char* error = new char[infoLogLength + 1];
      CUBEWORLD_SCOPE_EXIT([&] { delete[] error; });

      glGetShaderInfoLog(shaderID, infoLogLength, nullptr, error);

      if (error[0] != '\0') {
         LOG_ERROR("%s\n", error);
         return 0;
      }
   }

   GLenum error = glGetError();
   assert(error == 0);

   return shaderID;
}

Program LoadProgram(
   const char* vertexShaderPath,
   const char* geometryShaderPath,
   const char* fragmentShaderPath
)
{
   GLuint programID = glCreateProgram();

   // Vertex
   GLuint vertexShader = LoadShader(vertexShaderPath, GL_VERTEX_SHADER);
   if (vertexShader == 0)
   {
      glDeleteProgram(programID);
      return 0;
   }
   glAttachShader(programID, vertexShader);

   // Geometry
   GLuint geomShader = 0;
   if (geometryShaderPath != nullptr)
   {
      geomShader = LoadShader(geometryShaderPath, GL_GEOMETRY_SHADER);
      if (geomShader == 0)
      {
         glDeleteProgram(programID);
         return 0;
      }
      glAttachShader(programID, geomShader);
   }

   // Fragment
   GLuint fragShader = LoadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
   if (fragShader == 0)
   {
      glDeleteProgram(programID);
      return 0;
   }
   glAttachShader(programID, fragShader);

   // Link the program
   glLinkProgram(programID);

   // Check the program
   GLint result = GL_FALSE;
   int infoLogLength;
   glGetProgramiv(programID, GL_LINK_STATUS, &result);
   glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength > 0) {
      char* error = new char[infoLogLength + 1];
      CUBEWORLD_SCOPE_EXIT([&] { delete[] error; });
      glGetProgramInfoLog(programID, infoLogLength, nullptr, error);

      if (error[0] != '\0') {
         LOG_ERROR("%s\n", error);
      }
   }

   GLenum error = glGetError();
   assert(error == 0);

   glDetachShader(programID, vertexShader);
   glDeleteShader(vertexShader);
   if (geomShader != 0)
   {
      glDetachShader(programID, geomShader);
      glDeleteShader(geomShader);
   }
   glDetachShader(programID, fragShader);
   glDeleteShader(fragShader);


   return programID;
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld