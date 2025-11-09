#pragma once

#include "ECS_Typedefs.h"

namespace ECS
{
  namespace CONSTANTS
  {
    static consteval ComponentIndex InvalidComponentIndex()
    {
      static_assert(255 > MAX_TOTAL_NUMBER_OF_COMPONENTS, "You gotta change the type of ComponentIds to something bigger.");
      return 255u;
    }
    static consteval unsigned int InvalidEntityIndex() { return 16581375u; }
    static consteval unsigned int InvalidEntityVersion() { return 268435455u; }
    static consteval unsigned int InvalidPoolId() { return 4095u; }
    static constexpr EntityID InvalidEntityID() { return static_cast<EntityID>(InvalidEntityIndex() << 40 | static_cast<EntityID>(InvalidPoolId()) << 28 | static_cast<EntityID>(InvalidEntityVersion())); }
  }
}