// By Thomas Steinke

#include <cassert>
#include <vector>
#include <glad/glad.h>

#include "FeedbackBuffer.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

FeedbackBuffer::FeedbackBuffer()
{
    data.EnsureBuffer();
    glGenTransformFeedbacks(1, &mBuffer);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mBuffer);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, data.GetBuffer());
}

FeedbackBuffer::FeedbackBuffer(FeedbackBuffer&& other) noexcept
    : data(std::move(other.data))
{
    mBuffer = other.mBuffer;
    other.mBuffer = 0;
}

FeedbackBuffer& FeedbackBuffer::operator=(const FeedbackBuffer&) noexcept
{
    return *this;
}

FeedbackBuffer& FeedbackBuffer::operator=(FeedbackBuffer&& other) noexcept
{
    data = std::move(other.data);
    mBuffer = other.mBuffer;
    other.mBuffer = 0;

    return *this;
}

FeedbackBuffer::~FeedbackBuffer()
{
    if (mBuffer != 0)
    {
        glDeleteTransformFeedbacks(1, &mBuffer);
    }
}

void FeedbackBuffer::Begin(GLenum type)
{
    assert(mBuffer != 0);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mBuffer);
    glBeginTransformFeedback(type);
}

void FeedbackBuffer::Draw(GLenum type)
{
    assert(mBuffer != 0);
    glDrawTransformFeedback(type, mBuffer);
}

void FeedbackBuffer::End()
{
    glEndTransformFeedback();
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
