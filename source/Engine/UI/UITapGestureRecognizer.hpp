//
//  UITapGestureRecognizer.hpp
//
//  Created by Elliot Fiske on 10/18/18.
//

#pragma once

#include "UIGestureRecognizer.hpp"

namespace CubeWorld
{
    
namespace Engine
{
    
class UITapGestureRecognizer : public UIGestureRecognizer {
public:
    void touchBegan(const UITouch* touch);
};

}
    
}
