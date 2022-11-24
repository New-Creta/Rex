// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: invoke.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/functional/reference_wrapper.h"
#include "rex_std/internal/type_traits/conditional.h"
#include "rex_std/internal/type_traits/is_base_of.h"
#include "rex_std/internal/type_traits/is_member_function_pointer.h"
#include "rex_std/internal/type_traits/is_member_object_pointer.h"
#include "rex_std/internal/type_traits/remove_reference.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      enum class invoker_strategy
      {
        functor,
        pmf_object,  // pointer member function object
        pmf_rewrap,  // pointer member function rewrap
        pmf_pointer, // pointer member function pointer
        pmd_object,  // pointer data member
        pmd_refwrap, // pointer data member
        pmd_pointer  // pointer data member
      };

      struct invoker_functor
      {
        static constexpr invoker_strategy strategy = invoker_strategy::functor;

        template <typename Callable, typename... Types>
        static constexpr auto call(Callable&& obj, Types&&... args) noexcept -> decltype(static_cast<Callable&&>(obj)(static_cast<Types&&>(args)...))
        {
          return static_cast<Callable&&>(obj)(static_cast<Types&&>(args)...);
        }
      };

      struct invoker_pmf_object
      {
        static constexpr invoker_strategy strategy = invoker_strategy::pmf_object;

        template <typename Decayed, typename T1, typename... Types2>
        static constexpr auto call(Decayed pmf, T1&& args1, Types2&&... args2) noexcept -> decltype((static_cast<T1&&>(args1).*pmf)(static_cast<Types2&&>(args2)...))
        {
          return (static_cast<T1&&>(args1).*pmf)(static_cast<Types2&&>(args2)...);
        }
      };

      struct invoker_pmf_rewrap
      {
        static constexpr invoker_strategy strategy = invoker_strategy::pmf_rewrap;

        template <typename Decayed, typename Refwrap, typename... Types2>
        static constexpr auto call(Decayed pmf, Refwrap rw, Types2&&... args2) noexcept -> decltype((rw.get().*pmf)(static_cast<Types2&&>(args2)...))
        {
          return (rw.get().*pmf)(static_cast<Types2&&>(args2)...);
        }
      };

      struct invoker_pmf_pointer
      {
        static constexpr invoker_strategy strategy = invoker_strategy::pmf_pointer;

        template <typename Decayed, typename T1, typename... Types2>
        static constexpr auto call(Decayed pmf, T1&& args1, Types2&&... args2) noexcept -> decltype(((*static_cast<T1&&>(args1)).*pmf)(static_cast<Types2&&>(args2)...))
        {
          return ((*static_cast<T1&&>(args1)).*pmf)(static_cast<Types2&&>(args2)...);
        }
      };

      struct invoker_pmd_object
      {
        static constexpr invoker_strategy strategy = invoker_strategy::pmd_object;

        template <typename Decayed, typename T1>
        static constexpr auto call(Decayed pmd, T1&& args1) noexcept -> decltype(static_cast<T1&&>(args1).*pmd)
        {
          return static_cast<T1&&>(args1).*pmd;
        }
      };

      struct invoker_pmd_refwrap
      {
        static constexpr invoker_strategy strategy = invoker_strategy::pmd_refwrap;

        template <typename Decayed, typename Refwrap>
        static constexpr auto call(Decayed pmd, Refwrap rw) noexcept -> decltype(rw.get().*pmd)
        {
          return rw.get().*pmd;
        }
      };

      struct invoker_pmd_pointer
      {
        static constexpr invoker_strategy strategy = invoker_strategy::pmd_pointer;

        template <typename Decayed, typename T1>
        static constexpr auto call(Decayed pmd, T1&& args1) noexcept -> decltype((*static_cast<T1&&>(args1)).*pmd)
        {
          return (*static_cast<T1&&>(args1)).*pmd;
        }
      };

      template <typename T>
      using RemoveCVRef = remove_cv_t<remove_reference<T>>;

      template <typename Callable, typename T1, typename RemovedCVRef = RemoveCVRef<Callable>, bool IsPmf = is_member_function_pointer_v<RemovedCVRef>, bool IsPmd = is_member_object_pointer_v<RemovedCVRef>>
      struct Invoker1;

      template <typename T, template <typename...> typename Template>
      inline constexpr bool is_specialization_v = false;

      template <template <typename...> typename Template, typename... Types>
      inline constexpr bool is_specialization_v<Template<Types...>, Template> = true;

      template <typename Type, template <typename...> typename Template>
      struct is_specialization : bool_constant<is_specialization_v<Type, Template>>
      {
      };

      template <typename Callable, typename T1, typename RemovedCVRef>
      struct Invoker1<Callable, T1, RemovedCVRef, true, false> : conditional_t<is_base_of_v<typename is_member_function_pointer<RemovedCVRef>::class_type, remove_reference_t<T1>>, invoker_pmf_object,
                                                                               conditional_t<is_specialization_v<RemoveCVRef<T1>, reference_wrapper>, invoker_pmf_rewrap, invoker_pmf_pointer>>
      {
      }; // pointer to member function

      template <typename Callable, typename T1, typename RemovedCVRef>
      struct Invoker1<Callable, T1, RemovedCVRef, false, true> : conditional_t<is_base_of_v<typename is_member_object_pointer<RemovedCVRef>::class_type, remove_reference<T1>>, invoker_pmd_object,
                                                                               conditional_t<is_specialization_v<RemoveCVRef<T1>, reference_wrapper>, invoker_pmd_refwrap, invoker_pmd_pointer>>
      {
      }; // pointer to member data

      template <typename Callable, typename T1, typename RemovedCVRef>
      struct Invoker1<Callable, T1, RemovedCVRef, false, false> : invoker_functor
      {
      };
    } // namespace internal

    template <typename Callable>
    constexpr auto invoke(Callable&& obj) noexcept -> decltype(static_cast<Callable&&>(obj)())
    {
      return static_cast<Callable&&>(obj)();
    }

    template <typename Callable, typename T1, typename... Types2>
    constexpr auto invoke(Callable&& obj, T1&& args1, Types2&&... args2) noexcept -> decltype(internal::Invoker1<Callable, T1>::call(static_cast<Callable&&>(obj), static_cast<T1&&>(args1), static_cast<Types2&&>(args2)...))
    {
      if constexpr(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::functor)
      {
        return static_cast<Callable&&>(obj)(static_cast<T1&&>(args1), static_cast<Types2&&>(args2)...);
      }
      else if constexpr(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::pmf_object)
      {
        return (static_cast<T1&&>(args1).*obj)(static_cast<Types2&&>(args2)...);
      }
      else if constexpr(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::pmf_rewrap)
      {
        return (args1.get().*obj)(static_cast<Types2&&>(args2)...);
      }
      else if constexpr(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::pmf_pointer)
      {
        return ((*static_cast<T1&&>(args1)).*obj)(static_cast<Types2&&>(args2)...);
      }
      else if constexpr(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::pmd_object)
      {
        return static_cast<T1&&>(args1).*obj;
      }
      else if constexpr(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::pmd_refwrap)
      {
        return args1.get().*obj;
      }
      else
      {
        static_assert(internal::Invoker1<Callable, T1>::strategy == internal::invoker_strategy::pmd_pointer, "bug in invoke");
        return (*static_cast<T1&&>(args1)).*obj;
      }
    }

  } // namespace v1
} // namespace rsl
