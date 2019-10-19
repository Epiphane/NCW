// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <vector>

#include "VBO.h"

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

class FeedbackBuffer
{
protected:
   GLuint mBuffer = 0;

public:
   FeedbackBuffer();
   FeedbackBuffer(const FeedbackBuffer&) : FeedbackBuffer() {};
   FeedbackBuffer(FeedbackBuffer&& other) noexcept;
   ~FeedbackBuffer();

   FeedbackBuffer& operator=(const FeedbackBuffer& other) noexcept;
   FeedbackBuffer& operator=(FeedbackBuffer&& other) noexcept;

   void Bind() { data.Bind(); }
   void Draw(GLenum type);

   void Begin(GLenum type);
   void End();

   GLuint GetBuffer() { return mBuffer; }

   VBO data;
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
