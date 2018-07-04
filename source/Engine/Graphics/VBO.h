// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

extern std::vector<int> gBufferReferences;

enum DataType
{
   Vertices,
   Colors,
   UVs,
   Normals,
   Indices,
   Materials,
   Opacities,
   ArrayBuffer,
};

class VBO
{
protected:
   GLuint mBuffer;
   GLuint mBufferType;

public:
   // CAUTION: When using the default constructor, you must either call vbo.Init() or assign it another VBO before it is usable.
   VBO() : mBuffer(0) {};

   VBO(const DataType type);
   VBO(const DataType type, const GLuint buffer);
   VBO(const GLuint bufferType, const GLuint buffer);
   VBO(const VBO& other);
   ~VBO();
   void Init();

   VBO& operator=(const VBO& other);

   void Bind();
   void AttribPointer(GLuint location, GLint count, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
   void AttribIPointer(GLuint location, GLint count, GLenum type, GLsizei stride, const GLvoid *pointer);

   GLuint GetBuffer() { return mBuffer; }
   void BufferData(GLsizei size, void *data, GLuint type);
};

class AttributeVBO : public VBO
{
public:
   struct AttributeOptions {
      GLuint mLocation;
      GLint mSize;
      GLenum mType;
      GLboolean mNormalized;
   };

private:
   AttributeOptions mOptions;

public:
   AttributeVBO(DataType dataType, AttributeOptions options) : VBO(dataType), mOptions(options) {}
   AttributeVBO(DataType dataType, GLuint buffer, AttributeOptions options)
      : VBO(dataType, buffer), mOptions(options) {}
   AttributeVBO(const VBO& other);

   AttributeVBO& operator=(const AttributeVBO& other);

   void BindAttribute(GLsizei stride, const GLvoid *pointer);
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
