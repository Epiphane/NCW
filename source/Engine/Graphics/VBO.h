// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <vector>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

//
// -------------------------- IMPORTANT NOTE ---------------------------
// (putting it here so I don't have to remember it months down the line)
//
// A key limitation of the current design (gBufferReferences is indexes
// by GL buffer) is that it implicitly expects only one GLFW/GL context
// (in other words, one window) to exist at any given time. That means,
// without making significant changes here, multi-window support isn't
// really available. Key things that would have to change in order to
// add this support include:
// - Implicitly create an InputManager whenever a Window is created
// - Change the gBufferReferences global to include both buffer ID and
//   the corresponding context.
// - Ensure there's a good way to make sure that a window is the current
//   context when doing any rendering calls that use its resources.
// - Resolve alllll the issues that would come with those.
//
// It's not impossible, but it would take a while and for one thing (an
// editor), it doesn't really seem worth it right now.
//

extern std::vector<int> gBufferReferences;

class VBO
{
public:
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

protected:
   GLuint mBuffer = 0;
   GLuint mBufferType = 0;

public:
   // CAUTION: When using the default constructor, you must either call vbo.Init() or assign it another VBO before it is usable.
   VBO() : mBuffer(0), mBufferType(0) {};

   VBO(const DataType type);
   VBO(const DataType type, const GLuint buffer);
   VBO(const GLuint bufferType, const GLuint buffer);
   VBO(const VBO& other);
   ~VBO();
   void Init(const DataType type);

   VBO& operator=(const VBO& other);

   void Bind();
   void AttribPointer(GLuint location, GLint count, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
   void AttribIPointer(GLuint location, GLint count, GLenum type, GLsizei stride, const GLvoid *pointer);

   GLuint GetBuffer() { return mBuffer; }
   void BufferData(size_t size, void *data, GLuint type);

   template<typename T>
   void BufferData(const std::vector<T>& data, GLuint type = GL_STATIC_DRAW)
   {
       BufferData(sizeof(T) * data.size(), (void*)&data[0], type);
   }
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
