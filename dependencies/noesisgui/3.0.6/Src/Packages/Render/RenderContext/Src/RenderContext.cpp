////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsRender/RenderContext.h>
#include <NsCore/Factory.h>
#include <NsCore/Error.h>
#include <NsCore/Ptr.h>
#include <NsCore/Sort.h>
#include <NsCore/Symbol.h>
#include <NsCore/ArrayRef.h>
#include <NsCore/DynamicCast.h>
#include <NsCore/ReflectionImplementEmpty.h>

#include <stdio.h>


using namespace Noesis;
using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
void RenderContext::SetWindow(void*)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<RenderContext> RenderContext::Create()
{
    ArrayRef<Symbol> v = Factory::EnumComponents(Symbol("RenderContext"));

    Vector<Ptr<RenderContext>, 32> contexts;

    for (uint32_t i = 0; i < v.Size(); i++)
    {
        contexts.PushBack(StaticPtrCast<RenderContext>(Factory::CreateComponent(v[i])));
    }

    InsertionSort(contexts.Begin(), contexts.End(), [](const Ptr<RenderContext>& c0,
        const Ptr<RenderContext>& c1)
    {
        return c1->Score() < c0->Score();
    });

    for (uint32_t i = 0; i < contexts.Size(); i++)
    {
        if (contexts[i]->Validate())
        {
            return contexts[i];
        }
    }

    NS_FATAL("No valid render context found");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<RenderContext> RenderContext::Create(const char* name)
{
    char id[256];
    snprintf(id, 256, "%sRenderContext", name);

    return StaticPtrCast<RenderContext>(Factory::CreateComponent(Symbol(id)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION_(RenderContext)
