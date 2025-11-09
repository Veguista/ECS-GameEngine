#pragma once

#include "ECS_Interfaces.h"
#include "Engine/Components/Transform/C_Transform2D.h"
#include "assert.h"
#include <type_traits>
#include <tuple>

struct ECS_INTERNAL
{
  static inline unsigned int NumberOfInitializedComponents{ 0 };

  // Decayed template functions
  template <typename T>
  static bool Decayed_HasComponentBeenInitialized(bool _setInitialized = false)
  {
    static bool initialized{ false };

    if (_setInitialized)
    {
      initialized = true;
    }

    return initialized;
  }
  
  template <typename T>
  static unsigned int Decayed_GetComponentId()
  {
    static unsigned int id{ NumberOfInitializedComponents++ };
    return id;
  }


  // Interface functions
  template<typename T>
  consteval static bool Implements_IECS_Update()
  {
    return std::is_base_of<IECS_Update, T>::value;
  }

  template<typename T>
  static void DelayedUpdater(void* _ptr, float _deltaTime)
  {
    reinterpret_cast<T*>(_ptr)->Update(_deltaTime);
  }


  template<typename T>
  consteval static bool Implements_ECS_Destructor()
  {
    return true; // The base behavior is that the ECS system does call the destructor of any class.
  }

  template<typename T>
  static void DelayedDestructor(void* _ptr)
  {
    reinterpret_cast<T*>(_ptr)->~T();
  }


  template<typename T>
  consteval static bool Implements_ECS_Constructor()
  {
    return true; // The base behavior is that the ECS system does implement calling the constructor of any class.
  }

  template<typename T>
  static void* DelayedCallConstructorAtLocation(void* _locationWhereToConstruct)
  {
    return new (_locationWhereToConstruct) T();
  }


  template<typename T>
  consteval static bool Implements_IECS_CopyConstructor()
  {
    return std::is_base_of<IECS_CopyConstructor, T>::value;
  }

  template<typename T>
  static void* DelayedCallCopyConstructorAtLocation(void* _locationWhereToConstruct, const void* _componentToCopy)
  {
    return new (_locationWhereToConstruct) T(*reinterpret_cast<const T*>(_componentToCopy));
  }


  template<typename T>
  consteval static bool Implements_ECS_FunctionWithOneObjectParam()
  {
    if constexpr (std::is_base_of<IECS_Render, T>::value) // Rendering is one of our Functions with One Object Parameters
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  template<typename T>
  static void DelayedFunctionWithOneObjectParam(void* _objectPtr, void* _otherPtr)
  {
    if constexpr (Implements_ECS_FunctionWithOneObjectParam<T>())
    {
      if constexpr (std::is_base_of<IECS_Render, T>::value)
      {
        reinterpret_cast<T*>(_objectPtr)->Render(reinterpret_cast<const C_Transform2D*>(_otherPtr));
      }
      else
      {
        assert(false && "Trying to store pointer to a delayed function with one object parameter but the implementation has not been included in the template.");
      }
    }
  }

  template<typename T>
  consteval static bool Implements_ECS_Serialization()
  {
    return std::is_base_of<IECS_Serializable, T>::value;
  }

  template<typename T>
  static bool DelayedFunctionSerialize(void* _objectPtr, pugi::xml_node* _ComponentNode)
  {
    if constexpr (Implements_ECS_Serialization<T>())
    {
      return reinterpret_cast<T*>(_objectPtr)->Serialize(_ComponentNode);
    }
    else
    {
      return false;
    }
  }

  template<typename T>
  static bool DelayedFunctionLoad(void* _objectPtr, pugi::xml_node* _ComponentNode)
  {
    if constexpr (Implements_ECS_Serialization<T>())
    {
      return reinterpret_cast<T*>(_objectPtr)->Load(_ComponentNode);
    }
    else
    {
      return false;
    }
  }


  // IECS types without specific implementation.
  template<typename T>
  consteval static bool Implements_IECS_Transform()
  {
    return std::is_base_of<IECS_Transform, T>::value;
  }

  template<typename T>
  consteval static bool Implements_IECS_Render()
  {
    return std::is_base_of<IECS_Render, T>::value;
  }
};

namespace ECS
{
  // T Management
  template <typename FirstComponent, typename... OtherComponents>
  static void InitializeAnyNewComponents()
  {
    if (!ECS_INTERNAL::Decayed_HasComponentBeenInitialized<std::remove_cvref_t<FirstComponent>>())
    {
      ECS_INTERNAL::Decayed_HasComponentBeenInitialized<std::remove_cvref_t<FirstComponent>>(true);

      // Calling GetComponentId assigns an Id to our T.
      ECS_INTERNAL::Decayed_GetComponentId<FirstComponent>();
    }

    if constexpr (sizeof...(OtherComponents) > 0)
    {
      InitializeAnyNewComponents<OtherComponents...>();
    }
  }

  template <typename FirstComponent, typename... OtherComponents>
  static void SetPoolComponentMask(PoolComponentMask& _newComponentMask)
  {
    _newComponentMask.set(GetComponentId<FirstComponent>());

    if constexpr (sizeof...(OtherComponents) > 0)
    {
      SetPoolComponentMask<OtherComponents...>(_newComponentMask);
    }
  }

  /// <summary>
  /// Returns true if the T has been initialized with an EntityPool.
  /// </summary>
  /// <typeparam name="T"></typeparam>
  /// <param name="_setInitialized">If true, the component is recorded as "initialized".</param>
  /// <returns></returns>
  template <typename T>
  static bool HasComponentBeenInitialized()
  {
    return ECS_INTERNAL::Decayed_HasComponentBeenInitialized<std::remove_cvref_t<T>>();
  }
  template <typename FirstComponent, typename... OtherComponents>
  static bool HaveComponentsBeenInitialized()
  {
    if constexpr (sizeof...(OtherComponents) > 0)
    {
      return HasComponentBeenInitialized<FirstComponent>() && HaveComponentsBeenInitialized<OtherComponents...>();
    }
    else
    {
      return HasComponentBeenInitialized<FirstComponent>();
    }
  }

  template <typename T>
  static unsigned int GetComponentId()
  {
    return ECS_INTERNAL::Decayed_GetComponentId<std::remove_cvref_t<T>>();
  }

  template<typename... Types>
  static unsigned int HowManyTypes()
  {
    return sizeof...(Types);
  };


  template<typename T, template<typename> class TT>
  struct Decayed_IsFirstInstantiationOfSecondType : std::false_type {};
 
  template<typename T, template<typename> class TT> // Overriding the previous type for types when the condition occurs.
  struct Decayed_IsFirstInstantiationOfSecondType<TT<T>, TT> : std::true_type {};

  // Struct used to know when a Type is a Template instantiation of a Class.
  template<typename T, template<typename> class TT>
  struct IsFirstInstantiationOfSecondType : Decayed_IsFirstInstantiationOfSecondType<std::remove_cvref_t<T>, std::remove_cvref_t<TT>> {};


  template<int N, typename... Types> using NthTypeOf =
    typename std::tuple_element<N, std::tuple<Types...>>::type;

  template<auto _startValue, auto _endValue, auto _addition, typename LambdaExpression>
  constexpr void constexpr_loop_lambda(LambdaExpression&& _lambda)
  {
    if constexpr (_startValue < _endValue)
    {
      _lambda(std::integral_constant<decltype(_startValue), _startValue>());
      constexpr_loop_lambda<_startValue + _addition, _endValue, _addition>(_lambda);
    }
  }

  template<typename TypeToTest, typename...Types>
  constexpr bool IsFirstTypeContainedInRestOfTypes()
  {
    static_assert(sizeof...(Types) != 0, "Cannot evaluate this function unless you incorporate Types into it.");

    return ((std::is_same_v<TypeToTest, Types> || ...));
  }

  template<typename TypeToTest, typename...Types>
  constexpr bool DoesFirstTypeInheritFromRestOfTypes()
  {
    static_assert(sizeof...(Types) != 0, "Cannot evaluate this function unless you incorporate Types into it.");

    return (std::is_base_of<std::remove_cvref_t<Types>, std::remove_cvref_t<TypeToTest>>::value || ...);
  }
}