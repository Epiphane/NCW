// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>
#include <cmath>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Window.h>

#include "Input.h"

namespace CubeWorld
{

namespace Engine
{

Input::Input()
   : mKeyCallbacks{nullptr}
   , mMouseDownCallback{nullptr}
   , mMouseUpCallback{nullptr}
   , mMouseClickCallback{nullptr}
{}

Input::~Input()
{}

void Input::Reset()
{
   memset(mKeyCallbacks, 0, sizeof(mKeyCallbacks));
   mMouseDownCallback = nullptr;
   mMouseUpCallback = nullptr;
   mMouseClickCallback = nullptr;
}

void Input::RemoveCallback(std::unique_ptr<KeyCallbackLink> /*link*/)
{
   // link will be deconstructed at the end of this function, which is now the owner.
}

void Input::RemoveCallback(KeyCallbackLink* link)
{
   link->next->prev = link->prev;
   link->prev->next = link->next;

   // See if we're emptying a ring.
   if (link->key >= 0 && mKeyCallbacks[link->key] == link)
   {
      if (link->next == link)
      {
         mKeyCallbacks[link->key] = nullptr;
      }
      else
      {
         mKeyCallbacks[link->key] = link->next;
      }
   }
   else if (link->key < 0 && mKeyCallback == link)
   {
      if (link->next == link)
      {
         mKeyCallback = nullptr;
      }
      else
      {
         mKeyCallback = link->next;
      }
   }

   // Not really necessary, but it helps tie up the "complete disconnect" idea.
   link->next = link->prev = nullptr;
}

void Input::RemoveCallback(CharCallbackLink* link)
{
   link->next->prev = link->prev;
   link->prev->next = link->next;

   // See if we're emptying a ring.
   if (mCharCallback == link)
   {
      if (link->next == link)
      {
         mCharCallback = nullptr;
      }
      else
      {
         mCharCallback = link->next;
      }
   }

   // Not really necessary, but it helps tie up the "complete disconnect" idea.
   link->next = link->prev = nullptr;
}

std::unique_ptr<Input::KeyCallbackLink> Input::AddCallback(int key, input_key_callback cb)
{
   return AddCallback(KeyCombination{key, 0}, cb);
}

std::unique_ptr<Input::KeyCallbackLink> Input::AddCallback(KeyCombination key, input_key_callback cb)
{
   std::unique_ptr<KeyCallbackLink> link = std::make_unique<KeyCallbackLink>(this, key, cb);

   if (mKeyCallbacks[key.key] == nullptr)
   {
      mKeyCallbacks[key.key] = link.get();
      link->next = mKeyCallbacks[key.key];
      link->prev = mKeyCallbacks[key.key];
   }
   else
   {
      link->next = mKeyCallbacks[key.key];
      link->prev = link->next->prev;
      link->prev->next = link.get();
      link->next->prev = link.get();
   }

   return link;
}

std::vector<std::unique_ptr<Input::KeyCallbackLink>> Input::AddCallback(const std::vector<KeyCombination>& keys, input_key_callback cb)
{
   std::vector<std::unique_ptr<Input::KeyCallbackLink>> result;

   for (auto key : keys)
   {
      result.push_back(AddCallback(key, cb));
   }

   return result;
}

std::unique_ptr<Input::CharCallbackLink> Input::OnCharacter(char_callback cb)
{
   std::unique_ptr<CharCallbackLink> link = std::make_unique<CharCallbackLink>(this, cb);

   if (mCharCallback == nullptr)
   {
      mCharCallback = link.get();
      link->next = mCharCallback;
      link->prev = mCharCallback;
   }
   else
   {
      link->next = mCharCallback;
      link->prev = link->next->prev;
      link->prev->next = link.get();
      link->next->prev = link.get();
   }

   return link;
}

std::unique_ptr<Input::KeyCallbackLink> Input::OnKey(input_key_callback cb)
{
   std::unique_ptr<KeyCallbackLink> link = std::make_unique<KeyCallbackLink>(this, KeyCombination{-1, 0}, cb);

   if (mKeyCallback == nullptr)
   {
      mKeyCallback = link.get();
      link->next = mKeyCallback;
      link->prev = mKeyCallback;
   }
   else
   {
      link->next = mKeyCallback;
      link->prev = link->next->prev;
      link->prev->next = link.get();
      link->next->prev = link.get();
   }

   return link;
}

void Input::TriggerKeyCallbacks(int key, int action, int mods)
{
   if (key < 0)
   {
      return;
   }

   if (action == GLFW_PRESS)
   {
      if (mKeyCallbacks[key] != nullptr)
      {
         // Circle the ring, invoking callbacks.
         KeyCallbackLink* start = mKeyCallbacks[key];
         KeyCallbackLink* link = start;
         do
         {
            // Check for equivalence on the modifier keys.
            // Future consideration, is a superset okay?
            if (link->mods == mods && link->callback)
            {
               link->callback(key, action, mods);
            }
            link = link->next;
         } while (link != start);
      }
   }

   if (mKeyCallback != nullptr)
   {
      // Circle the ring, invoking callbacks.
      KeyCallbackLink* start = mKeyCallback;
      KeyCallbackLink* link = start;
      do
      {
         link->callback(key, action, mods);
         link = link->next;
      } while (link != start);
   }
}

void Input::TriggerCharCallbacks(unsigned int c)
{
   if (mCharCallback != nullptr)
   {
      // Circle the ring, invoking callbacks.
      CharCallbackLink* start = mCharCallback;
      CharCallbackLink* link = start;
      do
      {
         link->callback(c);
         link = link->next;
      } while (link != start);
   }
}

}; // namespace Engine

}; // namespace CubeWorld
