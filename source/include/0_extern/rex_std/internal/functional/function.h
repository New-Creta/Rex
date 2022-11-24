// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: function.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/compiler.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/functional/invoke.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/memory/byte.h"
#include "rex_std/internal/stddef/nullptr.h"
#include "rex_std/internal/type_traits/alignment_of.h"
#include "rex_std/internal/type_traits/is_base_of.h"
#include "rex_std/internal/type_traits/is_invocable_r.h"
#include "rex_std/internal/type_traits/is_member_function_pointer.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      bool is_null(const T& /*arg*/)
      {
        return false;
      }

      template <typename Result, typename... Arguments>
      bool is_null(Result (*const& functionPointer)(Arguments...))
      {
        return functionPointer == nullptr;
      }

      template <typename Result, typename Class, typename... Arguments>
      bool is_null(Result (Class::*const& functionPointer)(Arguments...))
      {
        return functionPointer == nullptr;
      }

      template <typename Result, typename Class, typename... Arguments>
      bool is_null(Result (Class::*const& functionPointer)(Arguments...) const)
      {
        return functionPointer == nullptr;
      }

      class unused_class
      {
      };

      union functor_storage_alignment
      {
        void (*unused_func_ptr)();
        void (unused_class::*unused_func_mem_ptr)();
        void* unused_ptr;
      };

      template <count_t SizeInBytes>
      struct alignas(sizeof(functor_storage_alignment)) functor_storage
      {
        static_assert(SizeInBytes >= 0, "functor storage must be a positive size");

        template <typename Ret>
        Ret& get_storage_type_ref() const
        {
          return *reinterpret_cast<Ret*>(const_cast<byte*>(&storage[0])); // NOLINT(cppcoreguidelines-pro-type-const-cast, cppcoreguidelines-pro-type-reinterpret-cast)
        }

        byte storage[SizeInBytes]; // NOLINT(modernize-avoid-c-arrays)
      };

      template <>
      struct functor_storage<0>
      {
        template <typename Ret>
        Ret& get_storage_type_ref() const
        {
          return *reinterpret_cast<Ret*>(const_cast<byte*>(&storage[0]));
        }

        byte storage[sizeof(functor_storage_alignment)]; // NOLINT(modernize-avoid-c-arrays)
      };

      template <typename Functor, count_t SizeInBytes>
      struct is_functor_inplace_allocatable
      {
        static constexpr bool value = sizeof(Functor) <= sizeof(functor_storage<SizeInBytes>) && (rsl::alignment_of_v<functor_storage<SizeInBytes>> % rsl::alignment_of_v<Functor>) == 0;
      };

      template <count_t SizeInBytes>
      class function_detail_base
      {
      public:
        using functor_storage_type = functor_storage<SizeInBytes>;

        enum class manager_operations
        {
          destroy_functor,
          copy_functor,
          move_functor
        };

        // functor allocated inplace
        template <typename Functor, typename = void>
        class function_manager_base
        {
        public:
          static Functor* get_functor_ptr(const functor_storage_type& storage)
          {
            return &storage.template get_storage_type_ref<Functor>();
          }

          template <typename T>
          static void create_functor(functor_storage_type& storage, T&& functor)
          {
            new(get_functor_ptr(storage)) Functor(rsl::forward<T>(functor));
          }

          static void destroy_functor(functor_storage_type& storage)
          {
            get_functor_ptr(storage)->~Functor();
          }

          static void copy_functor(functor_storage_type& to, const functor_storage_type& from)
          {
            new(get_functor_ptr(to)) Functor(*get_functor_ptr(from));
          }

          static void move_functor(functor_storage_type& to, functor_storage_type& from)
          {
            new(get_functor_ptr(to)) Functor(rsl::move(*get_functor_ptr(from)));
          }

          static void* manager(void* to, void* from, typename function_detail_base::manager_operations ops)
          {
            switch(ops)
            {
              case function_detail_base::manager_operations::destroy_functor:
              {
                destroy_functor(*static_cast<functor_storage_type*>(to));
              }
              break;
              case function_detail_base::manager_operations::copy_functor:
              {
                copy_functor(*static_cast<functor_storage_type*>(to), *static_cast<const functor_storage_type*>(from));
              }
              break;
              case function_detail_base::manager_operations::move_functor:
              {
                move_functor(*static_cast<functor_storage_type*>(to), *static_cast<functor_storage_type*>(from));
                destroy_functor(*static_cast<functor_storage_type*>(from));
              }
              break;
              default: break;
            }

            return nullptr;
          }
        };

        // functor allocated on the heap
        template <typename Functor>
        class function_manager_base<Functor, typename enable_if_t<!is_functor_inplace_allocatable<Functor, SizeInBytes>::value>::type>
        {
          static Functor* get_functor_ptr(const functor_storage_type& storage)
          {
            return storage.template get_storage_type_ref<Functor*>();
          }
          static Functor*& get_functor_ptr_ref(const functor_storage_type& storage)
          {
            return storage.template get_storage_type_ref<Functor*>();
          }

          template <typename T>
          static void create_functor(functor_storage_type& storage, T&& functor)
          {
            auto allocator = rsl::allocator();
            Functor* func  = static_cast<Functor*>(allocator.allocate(sizeof(Functor), alignof(Functor), 0));

            REX_ASSERT_X(func != nullptr, "Functor allocation failed");

            new(static_cast<void*>(func)) Functor(rsl::forward<T>(functor));
            get_functor_ptr_ref(storage) = func;
          }

          static void destroy_functor(functor_storage_type& storage)
          {
            Functor* func = get_functor_ptr(storage);
            if(func)
            {
              func->~Functor();
              auto allocator = rsl::allocator();
              allocator.deallocate(static_cast<void*>(func), sizeof(Functor));
            }
          }

          static void copy_functor(functor_storage_type& to, const functor_storage_type& from)
          {
            auto allocator = rsl::allocator();
            Functor* func  = static_cast<Functor*>(allocator.allocate(sizeof(Functor), alignof(Functor), 0));

            REX_ASSERT_X(func != nullptr, "Allocation with {} failed", allocator.name());

            new(static_cast<void*>(func)) Functor(*get_functor_ptr(from));
            get_functor_ptr_ref(to) = func;
          }

          static void move_functor(functor_storage_type& to, functor_storage_type& from)
          {
            Functor* func             = get_functor_ptr(from);
            get_functor_ptr_ref(to)   = func;
            get_functor_ptr_ref(from) = nullptr;
          }

          static void* manager(void* to, void* from, typename function_detail_base::manager_operations ops)
          {
            switch(ops)
            {
              case function_detail_base::manager_operations::destroy_functor:
              {
                destroy_functor(*static_cast<functor_storage_type*>(to));
              }
              break;
              case function_detail_base::manager_operations::copy_functor:
              {
                copy_functor(*static_cast<functor_storage_type*>(to), *static_cast<const functor_storage_type*>(from));
              }
              break;
              case function_detail_base::manager_operations::move_functor:
              {
                move_functor(*static_cast<functor_storage_type*>(to, *static_cast<functor_storage_type>(from)));
              }
              break;
              default: break;
            }

            return nullptr;
          }
        };

        template <typename Functor, typename R, typename... Args>
        class function_manager final : public function_manager_base<Functor>
        {
        public:
          using base = function_manager_base<Functor>;

          static R invoker(Args... args, const functor_storage_type& functor)
          {
            return rsl::invoke(*base::get_functor_ptr(functor), rsl::forward<Args>(args)...);
          }
        };

        function_detail_base()  = default;
        ~function_detail_base() = default;

      protected:
        const functor_storage_type& storage() const
        {
          return m_storage;
        }

        functor_storage_type& storage()
        {
          return m_storage;
        }

      private:
        functor_storage_type m_storage;
      };

      template <typename Myself, typename Base, typename Functor, typename Ret, typename... Args>
      inline constexpr bool valid_function_args = rsl::is_invocable_r_v<Ret, Functor, Args...> && !rsl::is_base_of_v<Base, rsl::decay_t<Functor>> && !rsl::is_same_v<rsl::decay_t<Functor>, Myself>;

      template <count_t, typename>
      class function_detail;

      template <count_t SizeInBytes, typename R, typename... Args>
      class function_detail<SizeInBytes, R(Args...)> : public function_detail_base<SizeInBytes>
      {
      private:
        using base = function_detail_base<SizeInBytes>;

      public:
        function_detail()
            : m_func_ptr_mgr(nullptr)
            , m_func_ptr_invoker(&invoke_bad_func_call)
        {
        }
        explicit function_detail(nullptr_t)
            : m_func_ptr_mgr(nullptr)
            , m_func_ptr_invoker(&invoke_bad_func_call)
        {
        }

        function_detail(const function_detail& other)
        {
          copy(other);
        }
        function_detail(function_detail&& other)
        {
          this->move(rsl::move(other));
        }
        template <typename Functor, typename = enable_if_t<internal::valid_function_args<function_detail, function_detail, Functor, R, Args...>, bool>>
        explicit function_detail(Functor f)
        {
          create_forward_functor(rsl::move(f));
        }

        ~function_detail()
        {
          destroy();
        }

        function_detail& operator=(const function_detail& other)
        {
          REX_ASSERT_X(this != rsl::addressof(other), "can't copy assign to yourself");

          destroy();
          copy(other);

          return *this;
        }
        function_detail& operator=(function_detail&& other)
        {
          REX_ASSERT_X(this != rsl::addressof(other), "can't move assign to yourself");

          destroy();
          move(rsl::move(other));

          return *this;
        }
        function_detail& operator=(nullptr_t)
        {
          destroy();

          m_func_ptr_mgr     = nullptr;
          m_func_ptr_invoker = &invoke_bad_func_call;

          return *this;
        }
        template <typename Functor, typename = enable_if_t<internal::valid_function_args<function_detail, function_detail, Functor, R, Args...>, bool>>
        function_detail& operator=(Functor&& f)
        {
          destroy();
          create_forward_functor(rsl::forward<Functor>(f));

          return *this;
        }
        template <typename Functor>
        function_detail& operator=(reference_wrapper<Functor> f)
        {
          destroy();
          create_forward_functor(f);

          return *this;
        }

        void swap(function_detail& other)
        {
          REX_ASSERT_X(this != rsl::addressof(other));

          functor_storage<SizeInBytes> tmp_storage {};
          if(other.has_manager())
          {
            (*other.m_func_ptr_mgr)(static_cast<void*>(&tmp_storage), static_cast<void*>(&other.storage()), base::manager_operations::move_functor);
          }

          if(has_manager())
          {
            (*m_func_ptr_mgr)(static_cast<void*>(&other.storage()), static_cast<void*>(&base::storage()), base::manager_operations::move_functor);
          }

          if(other.has_manager())
          {
            (*other.m_func_ptr_mgr)(static_cast<void*>(&base::storage()), static_cast<void*>(&tmp_storage), base::manager_operations::move_functor);
          }

          rsl::swap(m_func_ptr_mgr, other.m_func_ptr_mgr);
          rsl::swap(m_func_ptr_invoker, other.m_func_ptr_invoker);
        }

        explicit operator bool() const
        {
          return has_manager();
        }

        R operator()(Args... args) const
        {
          return (*m_func_ptr_invoker)(rsl::forward<Args>(args)..., base::storage());
        }

      private:
        bool has_manager() const
        {
          return m_func_ptr_mgr != nullptr;
        }

        void destroy()
        {
          if(has_manager())
          {
            (*m_func_ptr_mgr)(static_cast<void*>(&base::storage()), nullptr, base::manager_operations::destroy_functor);
          }
        }

        void copy(const function_detail& other)
        {
          if(other.has_manager())
          {
            (*other.m_func_ptr_mgr)(static_cast<void*>(&base::storage()), const_cast<void*>(static_cast<const void*>(&other.storage())), base::manager_operations::copy_functor); // NOLINT(cppcoreguidelines-pro-type-const-cast)
          }

          m_func_ptr_mgr     = other.m_func_ptr_mgr;
          m_func_ptr_invoker = other.m_func_ptr_invoker;
        }

        void move(function_detail&& other)
        {
          if(other.has_manager())
          {
            (*other.m_func_ptr_mgr)(static_cast<void*>(&base::storage()), static_cast<void*>(other.m_storage), base::manager_operations::move_functor);
          }

          m_func_ptr_mgr           = other.m_func_ptr_mgr;
          m_func_ptr_invoker       = other.m_func_ptr_invoker;
          other.m_func_ptr_mgr     = nullptr;
          other.m_func_ptr_invoker = &invoke_bad_func_call;
        }

        template <typename Functor>
        void create_forward_functor(Functor&& f)
        {
          using DecayedFunctorType  = typename rsl::decay<Functor>::type;
          using FunctionManagerType = typename base::template function_manager<DecayedFunctorType, R, Args...>;

          if(is_null(f))
          {
            m_func_ptr_mgr     = nullptr;
            m_func_ptr_invoker = &invoke_bad_func_call;
          }
          else
          {
            m_func_ptr_mgr     = &FunctionManagerType::manager;
            m_func_ptr_invoker = &FunctionManagerType::invoker;
            FunctionManagerType::create_functor(base::storage(), rsl::forward<Functor>(f));
          }
        }

#if defined(REX_MSVC_COMPILER)
  #pragma warning(push)
  #pragma warning(disable : 4716) // 'function' must return a value
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wreturn-type" // non-void function should return a value
#endif
        static R invoke_bad_func_call(Args... /*args*/, const typename base::functor_storage_type& /*storage*/)
        {
          // can't assume R is default constructible, so we disable no-return warning instead.
          REX_ASSERT("Bad function call!");
          // if R is not default constructible, we'll get a compiler error here, meaning we did something wrong.
        } // NOLINT(clang-diagnostic-return-type)
#if defined(REX_MSVC_COMPILER)
  #pragma warning(pop)
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic push
#endif

      private:
        using FuncPtrManager = void* (*)(void*, void*, typename base::manager_operations);
        using FuncPtrInvoker = R (*)(Args... /*args*/, const typename base::functor_storage_type& /*storage*/);

        FuncPtrManager m_func_ptr_mgr;
        FuncPtrInvoker m_func_ptr_invoker;
      };
    } // namespace internal

    template <typename>
    class function;

    template <typename R, typename... Args>
    class function<R(Args...)> : public internal::function_detail<2 * sizeof(void*), R(Args...)>
    {
    private:
      using base = internal::function_detail<2 * sizeof(void*), R(Args...)>;

    public:
      // creates an empty function
      function() = default;
      // creates an empty function
      function(nullptr_t p) // NOLINT(google-explicit-constructor)
          : base(p)
      {
      }
      // copies the target of other to this
      function(const function& other)
          : base(other)
      {
      }
      // moves the target of other to this
      function(function<R(Args...)>&& other) noexcept
          : base(rsl::move(other))
      {
      }
      // initializes the target with rsl::forward<F>(f)
      template <typename F, typename = enable_if_t<internal::valid_function_args<function, base, F, R, Args...>, bool>>
      function(F&& f) // NOLINT(google-explicit-constructor)
          : base(rsl::forward<F>(f))
      {
      }

      // destroys the instance, if this is not empty, the target is also destroyed
      ~function() = default;

      // assigns a copy of target of other as if by execution function(other).swap(*this)
      function& operator=(const function& other)
      {
        base::operator=(other);
        return *this;
      }
      // moves the target of other to this.
      function& operator=(function&& other)
      {
        base::operator=(rsl::move(other));
        return *this;
      }
      // drops the current target, this is empty after the call
      function& operator=(nullptr_t p)
      {
        base::operator=(p);
        return *this;
      }
      // set the target of this to the callable f
      template <typename F, typename = enable_if_t<internal::valid_function_args<function, base, F, R, Args...>, bool>>
      function& operator=(F&& f)
      {
        base::operator=(rsl::forward<F>(f));
        return *this;
      }
      // sets the target of this to a copy of f, as if by execution function(f).swap(*this)
      template <typename F>
      function& operator=(reference_wrapper<F> f)
      {
        base::operator=(f);
        return *this;
      }

      // exchanges the stored callable objects of this and other
      void swap(function& other)
      {
        base::swap(other);
      }

      // checks wheter this stores a callable function target, i.e. is not empty
      explicit operator bool() const
      {
        return base::operator bool();
      }

      // invokes the stored callable function target with the parameter args
      R operator()(Args... args) const
      {
        return base::operator()(rsl::forward<Args>(args)...);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
      // the following functions are not implemented
      // as rsl does not support rtti.
      // returns the type of the stored function
      // const type_info& target_type() const;
      // returns a pointer to the stored callable function
      // template <typename T>
      // T* target();
      // returns a pointer to the stored callable function
      // template <typename T>
      // const T* target() const;
    };

    // overloads the swap algorithm for function::swap.
    template <typename R, typename... Args>
    void swap(function<R(Args...)>& lhs, function<R(Args...)>& rhs)
    {
      return lhs.swap(rhs);
    }

    // compares a function with a null pointer. empty functions will return true
    template <typename R, typename... Args>
    bool operator==(function<R(Args...)>& f, nullptr_t)
    {
      return !f;
    }

    // compares a function with a null pointer. empty functions will return true
    template <typename R, typename... Args>
    bool operator==(nullptr_t, function<R(Args...)>& f)
    {
      return !f;
    }

    // compares a function with a null pointer. non-empty functions will return true
    template <typename R, typename... Args>
    bool operator!=(function<R(Args...)>& f, nullptr_t)
    {
      return !!f;
    }

    // compares a function with a null pointer. non-empty functions will return true
    template <typename R, typename... Args>
    bool operator!=(nullptr_t, function<R(Args...)>& f)
    {
      return !!f;
    }

    template <typename R, typename... Args>
    function(R (*)(Args...)) -> function<R(Args...)>;

    namespace internal
    {

      template <typename Fx, class = void>
      struct deduce_signature
      {
      }; // can't deduce signature when &Fx::operator() is missing, inaccessible, or ambiguous

      template <typename Fx>
      struct deduce_signature<Fx, void_t<decltype(&Fx::operator())>> : is_member_function_pointer<decltype(&Fx::operator())>::GuideType
      {
      };
    } // namespace internal

    template <typename Fx>
    function(Fx) -> function<typename internal::deduce_signature<Fx>::type>;

  } // namespace v1
} // namespace rsl

#include "rex_std/internal/assert/assert_impl.h"