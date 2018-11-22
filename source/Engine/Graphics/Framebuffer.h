// By Thomas Steinke

#pragma once

#include <GL/includes.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

//
// Framebuffer lets you do all sorts of cool stuff with screen space.
//
// For example, you can render the world to a framebuffer, and then
// render that buffer with a motion blur to the actual screen.
//
// It also provides a neat alternative to using glViewport. In the
// case of the Editor, Framebuffers are still more flexible because
// the open the door to tricks like fading and hiding pieces.
//
// Framebuffer uses an implicit stack, since only one framebuffer can
// be bound at a time. A framebuffer places itself "on the stack"
// when you call Bind() by saving the current viewport and framebuffer
// and pops itself off the stack when you call Unbind(), by restoring
// those values. It's kind of janky, but if you don't mess anything
// up it should be okay. :)
//
class Framebuffer
{
public:
   Framebuffer(GLsizei width = 0, GLsizei height = 0);
   ~Framebuffer();

   void Resize(GLsizei width, GLsizei height);

   void Bind(bool clear = true);
   void Unbind();

   GLuint GetTexture() { return mTexture; }

   GLsizei GetWidth() { return mWidth; }
   GLsizei GetHeight() { return mHeight; }

private:
   GLsizei mWidth, mHeight;

   GLuint mFBO;
   GLuint mTexture;
   GLuint mDepthBuffer;

private:
   GLuint mPreviousFBO;
   GLint mPreviousViewport[4];
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
