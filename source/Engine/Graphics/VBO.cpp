// By Thomas Steinke

#include <cassert>
#include <mutex>
#include <vector>
#include <glad/glad.h>

#include <RGBLogger/Logger.h>

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
    if (gBufferReferences.size() <= buffer)
    {
        gBufferReferences.resize(gBufferReferences.size() + kBufferCountIncrement, 0);
    }

    gBufferReferences[buffer] = 1;
    return buffer;
}

VBO::VBO(const GLuint buffer)
{
    SetBuffer(buffer);
}

VBO::VBO(const VBO& other) : VBO(other.mBuffer)
{}

VBO::VBO(VBO&& other)
{
    mBuffer = other.mBuffer;
    other.mBuffer = 0;
}

VBO& VBO::operator=(const VBO& other)
{
    SetBuffer(other.mBuffer);
    return *this;
}

VBO& VBO::operator=(VBO&& other) noexcept
{
    std::swap(mBuffer, other.mBuffer);
    return *this;
}

VBO::~VBO()
{
    SetBuffer(0);
}

bool VBO::EnsureBuffer()
{
    if (mBuffer == 0)
    {
        mBuffer = GenerateBuffer();
        return true;
    }
    return false;
}

void VBO::SetBuffer(GLuint buffer)
{
    if (mBuffer != 0)
    {
        std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
        if (--gBufferReferences[mBuffer] == 0)
        {
            glDeleteBuffers(1, &mBuffer);
        }
    }

    mBuffer = buffer;
    if (mBuffer != 0)
    {
        std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
        ++gBufferReferences[mBuffer];
    }
}

void VBO::BufferData(size_t size, void* data, GLuint strategy)
{
    EnsureBuffer();
    {
        std::unique_lock<std::mutex> lock{ gBufferReferencesMutex };
        if (mBuffer < gBufferReferences.size() && gBufferReferences[mBuffer] > 1)
        {
            --gBufferReferences[mBuffer];
            mBuffer = GenerateBuffer();
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    glBufferData(GL_ARRAY_BUFFER, GLsizei(size), data, strategy);
}

void VBO::CopyFrom(const VBO& other, size_t size, void* readOffset, void* writeOffset)
{
    EnsureBuffer();
    glBindBuffer(GL_COPY_READ_BUFFER, other.mBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, mBuffer);
    glBufferData(GL_COPY_WRITE_BUFFER, GLsizeiptr(size), nullptr, GL_STATIC_DRAW);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GLintptr(readOffset), GLintptr(writeOffset), GLsizeiptr(size));
}

void VBO::Bind(VBOTarget target)
{
    EnsureBuffer();
    glBindBuffer(GLenum(target), mBuffer);
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
