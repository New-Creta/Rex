// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: shared_ptr.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/concurrency/atomic_decrement.h"
#include "rex_std/bonus/concurrency/atomic_increment.h"
#include "rex_std/bonus/functional/hash_result.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/memory/default_delete.h"
#include "rex_std/internal/type_traits/aligned_storage.h"
#include "rex_std/internal/type_traits/alignment_of.h"
#include "rex_std/internal/type_traits/is_empty.h"
#include "rex_std/internal/type_traits/remove_extent.h"
#include "rex_std/internal/utility/move.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      class ref_count_base
      {
      public:
        explicit ref_count_base(count_t refCount)
            : m_ref_count(refCount)
        {
        }

        virtual ~ref_count_base() = default;

        count_t use_count() const
        {
          /// [06/Aug/2022] RSL Comment: If this causes any issues
          // it's because the value is read at the same time it is written to.
          return m_ref_count;
        }

        void inc_ref()
        {
          rsl::atomic_increment(m_ref_count);
        }

        bool release()
        {
          REX_ASSERT_X(use_count() > 0, "Invalid ref count for shared ptr");
          if(rsl::atomic_decrement(m_ref_count) == 0)
          {
            delete_ptr();
            delete_this();
            return true;
          }

          return false;
        }

      private:
        virtual void delete_ptr()         = 0;
        virtual void delete_this()        = 0;
        virtual void* get_deleter() const = 0;

      private:
        count_t m_ref_count;
      };

      template <typename T, typename Allocator, typename Deleter>
      class ref_count : ref_count_base
      {
      public:
        using value_type     = T;
        using pointer        = T*;
        using allocator_type = allocator;
        using deleter_type   = Deleter;

        ref_count(T* ptr, allocator allocator, Deleter deleter)
            : ref_count_base(0)
            , m_ptr(ptr)
            , m_deleter(rsl::move(deleter))
            , m_allocator(rsl::move(allocator))
        {
        }

        void delete_ptr() override
        {
          m_deleter(m_ptr);
          m_ptr = nullptr;
        }

        void delete_this() override
        {
          this->~RefCount();
          m_allocator.deallocate(this, sizeof(*this));
        }

        void* get_deleter() const final
        {
          return (void*)m_deleter;
        }

      private:
        T* m_ptr;
        Deleter m_deleter;
        Allocator m_allocator;
      };

      template <typename T>
      class shared_ptr;

      template <typename T>
      void allocate_shared_helper(shared_ptr<T>& sharedPtr, ref_count_base* refCount, T* value);

      template <typename T, typename Allocator>
      class ref_count_inst : ref_count_base
      {
      public:
        using value_type     = T;
        using pointer        = T*;
        using allocator_type = allocator;
        using storage_type   = rsl::aligned_storage<T>;

        pointer value()
        {
          return static_cast<value_type*>(static_cast<void*>(&m_memory));
        }

        template <typename... Args>
        explicit ref_count_inst(allocator_type allocator, Args&&... args)
        {
          new(&m_memory) value_type(rsl::forward<Args>(args)...);
        }

        void delete_ptr() override
        {
          value()->~value_type();
        }
        void delete_this() override
        {
          this->~RefCountInst();
          m_allocator.deallocate(this, sizeof(*this));
        }
        void* get_deleter() const final
        {
          return nullptr;
        }

      private:
        template <typename U>
        friend class shared_ptr;
        template <typename U>
        friend void internal::allocate_shared_helper(shared_ptr<U>&, ref_count_base*, U*);

      private:
        storage_type m_memory;
        allocator_type m_allocator;
      };
    } // namespace internal

    template <typename T, typename Deleter>
    class unique_ptr;

    template <typename T>
    class shared_ptr
    {
    public:
      using element_type   = remove_extent_t<T>;
      using default_delete = default_delete<T>;
      using reference_type = T&;

      // constructs new shared ptr with no managed object
      shared_ptr()
          : m_ptr(nullptr)
          , m_ref_count(nullptr)
      {
      }
      // constructs new shared ptr with no managed object
      shared_ptr(nullptr_t) // NOLINT(google-explicit-constructor)
          : m_ptr(nullptr)
          , m_ref_count(nullptr)
      {
      }
      // constructs a shared ptr with a ptr as the managed objects
      // uses the delete expression as the deleter
      template <typename U>
      explicit shared_ptr(U* ptr)
          : m_ptr(ptr)
          , m_ref_count(nullptr)
      {
        REX_ASSERT_X(ptr != nullptr, "nullptr can't be provided for this shared ptr constructor");
        alloc(ptr, default_delete(), allocator());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (04/Aug/2022)
      // the pointer provided cannot be nullptr, in the standard, it can be
      // constructs a shared ptr with a ptr as the managed objects
      // uses the provided deleter as the deleter
      template <typename U, typename Deleter>
      explicit shared_ptr(U* ptr, Deleter d)
          : m_ptr(ptr)
          , m_ref_count(nullptr)
      {
        REX_ASSERT_X(ptr != nullptr, "nullptr can't be provided for this shared ptr constructor");
        alloc(ptr, d, allocator());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (04/Aug/2022)
      // in the standard, the use count after this construct call == 1
      // in rex, the use count after this is 0
      // construct a shared ptr with no managed object
      // uses the provided deleter as the deleter
      template <typename Deleter>
      explicit shared_ptr(nullptr_t, Deleter d)
          : m_ptr(nullptr)
          , m_ref_count(nullptr)
      {
        alloc_without_ptr(d, allocator());
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (04/Aug/2022)
      // in the standard, the use count after this construct call == 1
      // in rex, the use count after this is 0
      // also the allocator is capture by value, not by reference
      // construct a shared ptr with no managed object
      // uses the provided deleter as the deleter
      // uses a copy of alloc for heap allocation
      template <typename U, typename Deleter, typename Alloc>
      explicit shared_ptr(U* ptr, Deleter d, Alloc& alloc)
          : m_ptr(ptr)
          , m_ref_count(nullptr)
      {
        REX_ASSERT_X(ptr != nullptr, "nullptr can't be provided for this shared ptr constructor");
        alloc(ptr, d, alloc);
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (04/Aug/2022)
      // in the standard, the use count after this construct call == 1
      // in rex, the use count after this is 0
      // also the allocator is capture by value, not by reference
      // construct a shared ptr with no managed object
      // uses the provided deleter as the deleter
      // uses a copy of alloc for heap allocation
      template <typename Deleter, typename Alloc>
      explicit shared_ptr(nullptr_t, Deleter d, Alloc& alloc)
          : m_ptr(nullptr)
          , m_ref_count(nullptr)
      {
        alloc_without_ptr(d, alloc);
      }
      // constructs a shared ptr instance that stores p and shares ownership with r.
      // post condition: get() == ptr && use_count() == sharedPtr.use_count()
      // the user is responsible to make sure "ptr" doesn't leak
      // all what this function does is store an UNMANAGED ptr
      // this is useful for when the pointer is a downcast of an existing pointer
      // it is not recommended to use this constructor if "ptr" is a pointer that needs to be deleted
      template <typename U>
      explicit shared_ptr(const shared_ptr<U>& sharedPtr, element_type* ptr)
          : m_ptr(ptr)
          , m_ref_count(sharedPtr.m_ref_count)
      {
        if(m_ref_count)
        {
          m_ref_count->inc_ref();
        }
      }
      // constructs a shared ptr that stores p and shares ownership with r
      // post condition: get() == ptr && use_count() == sharedPtr.use_count()
      // the provided sharedPtr is empty after the constructor call
      // the user is responsible to make sure "ptr" doesn't leak
      // all what this function does is store an UNMANAGED ptr
      // this is useful for when the pointer is a downcast of an existing pointer
      // it is not recommended to use this constructor if "ptr" is a pointer that needs to be deleted
      template <typename U>
      explicit shared_ptr(shared_ptr<U>&& sharedPtr, element_type* ptr)
          : m_ptr(ptr)
          , m_ref_count(sharedPtr.m_ref_count)
      {
        sharedPtr.m_ptr       = nullptr;
        sharedPtr.m_ref_count = nullptr;
      }
      // constructs a shared pointer which shares ownership with the sharedPtr provided.
      shared_ptr(const shared_ptr<T>& sharedPtr)
          : m_ptr(sharedPtr.m_ptr)
          , m_ref_count(sharedPtr.m_ref_count)
      {
        if(m_ref_count)
        {
          m_ref_count->inc_ref();
        }
      }
      // constructs a shared pointer which shares ownership with the sharedPtr provided
      template <typename U>
      explicit shared_ptr(const shared_ptr<U>& sharedPtr)
          : m_ptr(sharedPtr.m_ptr)
          , m_ref_count(sharedPtr.m_ref_count)
      {
        if(m_ref_count)
        {
          m_ref_count->inc_ref();
        }
      }
      // moves ownership from the sharedPtr provided
      // after the call the sharedPtr provided is empty
      shared_ptr(shared_ptr&& sharedPtr)
          : m_ptr(sharedPtr.m_ptr)
          , m_ref_count(sharedPtr.m_ref_count)
      {
        sharedPtr.m_ptr       = nullptr;
        sharedPtr.m_ref_count = nullptr;
      }
      // moves ownership from the sharedPtr provided
      // after the call the sharedPtr provided is empty
      template <typename U>
      explicit shared_ptr(shared_ptr<U>&& sharedPtr)
          : m_ptr(sharedPtr.m_ptr)
          , m_ref_count(sharedPtr.m_ref_count)
      {
        sharedPtr.m_ptr       = nullptr;
        sharedPtr.m_ref_count = nullptr;
      }
      /// [05/Aug/2022] RSL TODO: implement Weak Pointer constructor
      // constructs a shared ptr that shares ownership with the provided weakPtr
      // template <typename U>
      // explicit shared_ptr(const WeakPtr<U>& weakPtr)
      //     : m_ptr(nullptr)
      //     , m_ref_count(nullptr)
      // {}

      // constructs a shared ptr from a uniquePtr
      // post condition: ref count == 1
      // uniquePtr.get() == nullptr
      template <typename U, typename Deleter>
      explicit shared_ptr(unique_ptr<U, Deleter>&& uniquePtr)
          : m_ptr(uniquePtr.get())
          , m_ref_count(nullptr)
      {
        alloc(uniquePtr.get(), uniquePtr.get_deleter(), allocator());
      }

      // decrements the reference count if this has a managed object.
      // if this is the last managed object, it destroys it through the provided deleter
      ~shared_ptr()
      {
        if(m_ref_count)
        {
          m_ref_count->release();
        }
      }

      // replaces the managed object with the one managed by r
      shared_ptr& operator=(const shared_ptr& other) // NOLINT(bugprone-unhandled-self-assignment)
      {
        REX_ASSERT_X(&other != this, "copying shared ptr to itself");

        shared_ptr<T>(other).swap(*this);

        return *this;
      }

      // replaces the managed object with the one managed by r
      template <typename U>
      shared_ptr& operator=(const shared_ptr<U>& other)
      {
        if(!equivalent_ownership(other))
        {
          shared_ptr(other).swap(*this);
        }

        return *this;
      }

      // moves assigns a shared ptr from other.
      // after the assignment other is empty and this holds what r used to hold
      shared_ptr& operator=(shared_ptr&& other)
      {
        REX_ASSERT_X(&other != this, "copying shared ptr to itself");

        shared_ptr(rsl::move(other)).swap(*this);

        return *this;
      }

      // moves assigns a shared ptr from other.
      // after the assignment other is empty and this holds what r used to hold
      template <typename U>
      shared_ptr& operator=(shared_ptr<U>&& other)
      {
        if(!equivalent_onwership(other))
        {
          shared_ptr(rsl::move(other)).swap(*this);
        }

        return *this;
      }

      // transfers ownership of an unique ptr to this
      template <typename U, typename Deleter>
      shared_ptr& operator=(unique_ptr<U, Deleter>&& other)
      {
        shared_ptr(rsl::move(other)).swap(*this);

        return *this;
      }

      // makes this shared ptr not manage the object anymore
      // "releases" the managed object, not to be confused with unique ptr's release.
      void reset()
      {
        shared_ptr().swap(*this);
      }
      // "releases" ownership and takes ownership of the passed in pointer
      // the word "release" is not to be confused with unique_ptr's release function
      template <typename U>
      void reset(U* ptr)
      {
        shared_ptr(ptr).swap(*this);
      }
      // "releases" ownership and takes ownership of the passed in pointer
      // the word "release" is not to be confused with unique_ptr's release function
      template <typename U, typename Deleter>
      void reset(U* ptr, Deleter deleter)
      {
        shared_ptr(ptr, deleter).swap(*this);
      }
      // "releases" ownership and takes ownership of the passed in pointer
      // the word "release" is not to be confused with unique_ptr's release function
      template <typename U, typename Deleter, typename Allocator>
      void reset(U* ptr, Deleter deleter, allocator& alloc)
      {
        shared_ptr(ptr, deleter, alloc).swap(*this);
      }

      // exchanges the owned pointer between two shared ptr objects
      void swap(shared_ptr& sharedPtr)
      {
        element_type* const value = sharedPtr.m_ptr;
        sharedPtr.m_ptr           = m_ptr;
        m_ptr                     = value;
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (05/Aug/2022)
      // the standard doesn't propagate const, rex does
      // returns a reference to the managed object
      reference_type operator*() const
      {
        return *m_ptr;
      }
      // returns a reference to the managed object
      reference_type operator*()
      {
        return *m_ptr;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (05/Aug/2022)
      // the standard doesn't propagate const, rex does
      // returns a pointer to the managed object
      const element_type* operator->() const
      {
        return m_ptr;
      }
      element_type* operator->()
      {
        return m_ptr;
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (05/Aug/2022)
      // the standard doesn't propagate const, rex does
      // returns a pointer to the managed object
      const element_type* get() const
      {
        return m_ptr;
      }
      // returns a pointer to the managed object
      element_type* get()
      {
        return m_ptr;
      }

      // returns the number of shared ptr objects referring to the same managed object
      count_t use_count() const
      {
        return m_ref_count->use_count();
      }

      // checks if the stored pointer is not null
      explicit operator bool() const
      {
        return get() != nullptr;
      }

      // checks whether this shared ptr precedes other order
      template <typename U>
      bool owner_before(const shared_ptr<U>& sharedPtr) const
      {
        return m_ref_count < shared_ptr.m_ref_count;
      }

      /// [05/Aug/2022] RSL TODO: implement owner_before for weak pointers
      // template <typename U>
      // bool owner_beforE(const WeakPtr<U>& other) const
      // {

      // }

    private:
      template <typename U, typename Deleter, typename Allocator, enable_if_t<rsl::is_empty_v<Allocator>, bool> = true>
      void alloc(U* ptr, Deleter deleter, const Allocator& alloc)
      {
        void* mem = alloc.allocate(1_elem);

        m_ref_count = new(mem)(internal::ref_count<T, Allocator, Deleter>)(ptr, rsl::move(deleter), rsl::move(alloc));
        m_ptr       = ptr;
      }
      template <typename U, typename Deleter, typename Allocator, enable_if_t<!rsl::is_empty_v<Allocator>, bool> = true>
      void alloc(U* ptr, Deleter deleter, Allocator& alloc)
      {
        void* mem = alloc.allocate(1_elem);

        m_ref_count = new(mem)(internal::ref_count<T, Allocator, Deleter>)(ptr, rsl::move(deleter), rsl::move(alloc));
        m_ptr       = ptr;
      }
      template <typename Deleter, typename Allocator, enable_if_t<rsl::is_empty_v<Allocator>, bool> = true>
      void alloc_without_ptr(Deleter deleter, const Allocator& alloc)
      {
        void* mem = alloc.allocate(1_elem);

        m_ref_count = new(mem)(internal::ref_count<T, Allocator, Deleter>)(nullptr, rsl::move(deleter), rsl::move(alloc));
      }
      template <typename Deleter, typename Allocator, enable_if_t<!rsl::is_empty_v<Allocator>, bool> = true>
      void alloc_without_ptr(Deleter deleter, Allocator& alloc)
      {
        void* mem = alloc.allocate(1_elem);

        m_ref_count = new(mem)(internal::ref_count<T, Allocator, Deleter>)(nullptr, rsl::move(deleter), rsl::move(alloc));
      }

    private:
      element_type* m_ptr; // the only reason why we have to store this ptr is because of the ctor taking in a shared ptr and a ptr.
      internal::ref_count_base* m_ref_count;

      template <class Deleter, class U>
      friend Deleter* get_deleter(const shared_ptr<U>& ptr);
    };

    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args)
    {
      allocator alloc;
      return allocate_shared<T>(alloc, rsl::forward<Args>(args)...);
    }

    namespace internal
    {
      template <typename T>
      void allocate_shared_helper(shared_ptr<T>& sharedPtr, ref_count_base* refCount, T* value)
      {
        sharedPtr.m_ref_count = refCount;
        sharedPtr.m_ptr       = value;
      }
    } // namespace internal

    template <typename T, typename Allocator, typename... Args>
    shared_ptr<T> allocate_shared(allocator& allocator, Args&&... args)
    {
      using ref_count_type = internal::ref_count_inst<T>;
      shared_ptr<T> ret;
      void* const mem           = allocator.allocate(sizeof(ref_count_type));
      ref_count_type* ref_count = new(mem) ref_count_type(rsl::forward<Args>(args)...);
      allocate_shared_helper(ret, ref_count, ref_count->value());
    }

    template <typename T>
    struct hash;

    template <typename T>
    struct hash<shared_ptr<T>>
    {
      hash_result operator()(const shared_ptr<T>& ptr) const
      {
        hash<T*> new_hash;
        return new_hash(ptr.get());
      }
    };

    template <typename T, typename U>
    shared_ptr<T> static_pointer_cast(const shared_ptr<U>& other)
    {
      const auto ptr = static_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(other, ptr);
    }
    template <typename T, typename U>
    shared_ptr<T> static_pointer_cast(shared_ptr<U>&& other)
    {
      const auto ptr = static_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(rsl::move(other), ptr);
    }

    template <typename T, typename U>
    shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& other)
    {
      const auto ptr = dynamic_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(other, ptr);
    }
    template <typename T, typename U>
    shared_ptr<T> dynamic_pointer_cast(shared_ptr<U>&& other)
    {
      const auto ptr = dynamic_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(rsl::move(other), ptr);
    }

    template <typename T, typename U>
    shared_ptr<T> const_pointer_cast(const shared_ptr<U>& other)
    {
      const auto ptr = const_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(other, ptr);
    }
    template <typename T, typename U>
    shared_ptr<T> const_pointer_cast(shared_ptr<U>&& other)
    {
      const auto ptr = const_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(rsl::move(other), ptr);
    }

    template <typename T, typename U>
    shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U>& other)
    {
      const auto ptr = reinterpret_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(other, ptr);
    }
    template <typename T, typename U>
    shared_ptr<T> reinterpret_pointer_cast(shared_ptr<U>&& other)
    {
      const auto ptr = reinterpret_cast<typename shared_ptr<T>::element_type*>(other.get());
      return shared_ptr<T>(rsl::move(other), ptr);
    }

    template <typename Deleter, typename T>
    Deleter* get_deleter(const shared_ptr<T>& p)
    {
      if(p.m_ref_count)
      {
        return static_cast<Deleter*>(p.m_ref_count->get_deleter());
      }
    }

  } // namespace v1
} // namespace rsl
