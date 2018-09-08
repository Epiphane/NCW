// By Thomas Steinke

#include <glad/glad.h>

#include "../Core/Scope.h"
#include "../Logger/Logger.h"
#include "Framebuffer.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

GLuint gCurrentFrameBuffer = 0;

void BindFramebuffer(GLuint buffer)
{
   gCurrentFrameBuffer = buffer;
   glBindFramebuffer(GL_FRAMEBUFFER, buffer);
}

Framebuffer::Framebuffer(GLsizei width, GLsizei height)
   : mWidth(width)
   , mHeight(height)
{
   glGenFramebuffers(1, &mFBO);

   // Push onto the stack
   GLuint previous = gCurrentFrameBuffer;
   CUBEWORLD_SCOPE_EXIT([&] { BindFramebuffer(previous); });
   BindFramebuffer(mFBO);

   // Texture setup
   {
      // The texture we're going to render to.
      glGenTextures(1, &mTexture);

      // Bind the newly created texture for modification.
      glBindTexture(GL_TEXTURE_2D, mTexture);

      // Give an empty image to OpenGL ( the last "0" )
      glTexImage2D(
         GL_TEXTURE_2D,    // target
         0,                // level of detail
         GL_RGB,           // internal format
         width,            // width
         height,           // height
         0,                // border - "This value must be 0." LOL
         GL_RGB,           // format
         GL_UNSIGNED_BYTE, // type
         0                 // data
      );

      // Very simple filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   }

   // Depth buffer setup
   {
      glGenRenderbuffers(1, &mDepthBuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 760);
   }

   // Tie it all together
   {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
   }

   // Final check
   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   assert(status == GL_FRAMEBUFFER_COMPLETE);
}

Framebuffer::~Framebuffer()
{
   glDeleteFramebuffers(1, &mFBO);
   glDeleteTextures(1, &mTexture);
   glDeleteRenderbuffers(1, &mDepthBuffer);
}

void Framebuffer::Bind(bool clear)
{
   mPreviousFBO = gCurrentFrameBuffer;
   glGetIntegerv(GL_VIEWPORT, mPreviousViewport);
   
   BindFramebuffer(mFBO);
   glViewport(0, 0, mWidth, mHeight);

   if (clear)
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }
}

void Framebuffer::Unbind()
{
   glViewport(
      mPreviousViewport[0],
      mPreviousViewport[1],
      mPreviousViewport[2],
      mPreviousViewport[3]
   );
   BindFramebuffer(mPreviousFBO);
}

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld