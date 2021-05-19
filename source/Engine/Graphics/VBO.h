// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <vector>

namespace CubeWorld
{

enum class VBOTarget : GLenum
{
    // Vertex attributes
    VertexData = GL_ARRAY_BUFFER,
    // Atomic counter storage
    AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
    // Source for buffer copying
    CopyReadBuffer = GL_COPY_READ_BUFFER,
    // Destination for buffer copying
    CopyWriteBuffer = GL_COPY_WRITE_BUFFER,
    // For glDispatchComputeIndirect
    DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
    // For glDrawArraysIndirect
    DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
    // For vertex indices
    VertexIndices = GL_ELEMENT_ARRAY_BUFFER,
    // Pixel read target
    PixelPack = GL_PIXEL_PACK_BUFFER,
    // Texture data source
    PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
    // GL query results
    Query = GL_QUERY_BUFFER,
    // Read+Write storage for compute shaders
    ShaderStorage = GL_SHADER_STORAGE_BUFFER,
    // Texture data
    Texture = GL_TEXTURE_BUFFER,
    // Transform feedback
    TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
    // Uniform block storage
    Uniform = GL_UNIFORM_BUFFER,
};

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
    VBO() = default;
    VBO(const GLuint buffer);
    VBO(const VBO& other);
    VBO(VBO&& other);
    ~VBO();
    bool EnsureBuffer();
    void SetBuffer(GLuint buffer);

    VBO& operator=(const VBO& other);
    VBO& operator=(VBO&& other) noexcept;

    void Bind(VBOTarget target);
    void AttribPointer(GLuint location, GLint count, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
    void AttribIPointer(GLuint location, GLint count, GLenum type, GLsizei stride, const GLvoid* pointer);

    GLuint GetBuffer() { return mBuffer; }
    void BufferData(size_t size, void* data, GLuint type);
    void CopyFrom(const VBO& other, size_t amount, void* readOffset = nullptr, void* writeOffset = nullptr);

    template<typename T>
    void BufferData(const std::vector<T>& data, GLuint type = GL_STATIC_DRAW)
    {
        BufferData(sizeof(T) * data.size(), (void*)&data[0], type);
    }

protected:
    GLuint mBuffer = 0;
};

}; // namespace Graphics
}; // namespace Engine
}; // namespace CubeWorld
