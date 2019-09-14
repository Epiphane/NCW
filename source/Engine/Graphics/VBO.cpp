// By Thomas Steinke

#include <cassert>
#include <vector>
#include <glad/glad.h>

#include "VBO.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

//
// Track number of references to a Vertex Buffer Object.
// This way, we can soft-clone a VBO, and only re-buffer the data
// when it's actually changed, allowing us to save on buffer creation
// for buffers that don't change.
//
#define BUFFER_COUNT_INCREMENT 256
std::vector<int> gBufferReferences(BUFFER_COUNT_INCREMENT);

GLuint GenerateBuffer()
{
   GLuint buffer;
   glGenBuffers(1, &buffer);
   assert(buffer != 0 && "Buffer capacity exceeded");

   if (gBufferReferences.size() <= buffer) {
      gBufferReferences.resize(gBufferReferences.size() + BUFFER_COUNT_INCREMENT);
   }

   gBufferReferences[buffer] = 1;
   return buffer;
}

VBO::VBO(const DataType type, const GLuint buffer) : mBuffer(buffer)
{
   if (type == Indices) mBufferType = GL_ELEMENT_ARRAY_BUFFER;
   else mBufferType = GL_ARRAY_BUFFER;

   ++gBufferReferences[mBuffer];
}

VBO::VBO(const DataType type)
{
   Init(type);
}

VBO::VBO(const GLuint bufferType, const GLuint buffer) : mBuffer(buffer), mBufferType(bufferType)
{
   ++gBufferReferences[mBuffer];
}

VBO::VBO(const VBO& other) : VBO(other.mBufferType, other.mBuffer) {}

VBO& VBO::operator=(const VBO& other) {
   if (mBuffer != 0)
   {
      --gBufferReferences[mBuffer];
   }

   mBuffer = other.mBuffer;
   mBufferType = other.mBufferType;

   ++gBufferReferences[mBuffer];

   return *this;
}

VBO::~VBO()
{
   // Fun fact: gBufferReferences might have already been cleaned up at this point. If so, don't use it
   if (mBuffer != GL_FALSE && mBuffer < gBufferReferences.size())
   {
      --gBufferReferences[mBuffer];
   }
}

void VBO::Init(const DataType type)
{
   assert(mBuffer == 0);

   mBuffer = GenerateBuffer();
   if (type == Indices) mBufferType = GL_ELEMENT_ARRAY_BUFFER;
   else mBufferType = GL_ARRAY_BUFFER;
}

void VBO::BufferData(size_t size, void* data, GLuint strategy)
{
   assert(mBuffer != 0);
   if (mBuffer < gBufferReferences.size() && gBufferReferences[mBuffer] > 1)
   {
      --gBufferReferences[mBuffer];

      mBuffer = GenerateBuffer();
   }

   glBindBuffer(mBufferType, mBuffer);
   glBufferData(mBufferType, GLsizei(size), data, strategy);
}

void VBO::Bind()
{
   assert(mBuffer != 0);
   glBindBuffer(mBufferType, mBuffer);
}

void VBO::AttribPointer(GLuint location, GLint count, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
   assert(mBuffer != 0);
   glEnableVertexAttribArray(location);
   glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
   glVertexAttribPointer(location, count, type, normalized, stride, pointer);
}

void VBO::AttribIPointer(GLuint location, GLint count, GLenum type, GLsizei stride, const GLvoid *pointer)
{
   assert(mBuffer != 0);
   glEnableVertexAttribArray(location);
   glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
   glVertexAttribIPointer(location, count, type, stride, pointer);
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld