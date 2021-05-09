// By Thomas Steinke

#include <cassert>
#include <mutex>
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
std::mutex gBufferReferencesMutex;
constexpr size_t kBufferCountIncrement = 256;
std::vector<int> gBufferReferences(kBufferCountIncrement);

GLuint GenerateBuffer()
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    assert(buffer != 0 && "Buffer capacity exceeded");

    std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
    if (gBufferReferences.size() <= buffer) {
        gBufferReferences.resize(gBufferReferences.size() + kBufferCountIncrement);
    }

    gBufferReferences[buffer] = 1;
    return buffer;
}

VBO::VBO(const DataType type, const GLuint buffer) : mBuffer(buffer)
{
    if (type == Indices) mBufferType = GL_ELEMENT_ARRAY_BUFFER;
    else if (type == ShaderStorage) mBufferType = GL_SHADER_STORAGE_BUFFER;
    else mBufferType = GL_ARRAY_BUFFER;

    std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
    ++gBufferReferences[mBuffer];
}

VBO::VBO(const DataType type)
{
    Init(type);
}

VBO::VBO(const GLuint bufferType, const GLuint buffer) : mBuffer(buffer), mBufferType(bufferType)
{
    std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
    ++gBufferReferences[mBuffer];
}

VBO::VBO(const VBO& other) : VBO(other.mBufferType, other.mBuffer) {}

VBO& VBO::operator=(const VBO& other) {
    if (mBuffer != 0)
    {
        std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
        --gBufferReferences[mBuffer];
    }

    mBuffer = other.mBuffer;
    mBufferType = other.mBufferType;

    std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
    ++gBufferReferences[mBuffer];

    return *this;
}

VBO::~VBO()
{
    // Fun fact: gBufferReferences might have already been cleaned up at this point. If so, don't use it
    std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
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
    else if (type == ShaderStorage) mBufferType = GL_SHADER_STORAGE_BUFFER;
    else mBufferType = GL_ARRAY_BUFFER;
}

void VBO::BufferData(size_t size, void* data, GLuint strategy)
{
    assert(mBuffer != 0);
    {
        std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
        if (mBuffer < gBufferReferences.size() && gBufferReferences[mBuffer] > 1)
        {
            --gBufferReferences[mBuffer];

            mBuffer = GenerateBuffer();
        }
    }

    glBindBuffer(mBufferType, mBuffer);
    glBufferData(mBufferType, GLsizei(size), data, strategy);
}

void VBO::Bind()
{
    assert(mBuffer != 0);
    glBindBuffer(mBufferType, mBuffer);
}

void VBO::AttribPointer(GLuint location, GLint count, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    assert(mBuffer != 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    glVertexAttribPointer(location, count, type, normalized, stride, pointer);
}

void VBO::AttribIPointer(GLuint location, GLint count, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    assert(mBuffer != 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    glVertexAttribIPointer(location, count, type, stride, pointer);
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld