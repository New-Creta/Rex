// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: hash_map.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/hashtable/hashtable.h"
#include "rex_std/bonus/hashtable/mod_range_hashing.h"
#include "rex_std/bonus/utility/key_value.h"
#include "rex_std/bonus/utility/use_first.h"
#include "rex_std/internal/functional/equal_to.h"
#include "rex_std/internal/functional/hash.h"
#include "rex_std/internal/memory/allocator.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Key, typename Value, typename Hash = rsl::hash<Key>, typename Equal = rsl::equal_to<Key>, typename Allocator = allocator>
    class hash_map : public hashtable<Key, key_value<const Key, Value>, Allocator, use_first<key_value<const Key, Value>>, Equal, Hash, mod_range_hashing, prime_rehash_policy, true, true>
    {
    public:
      using base_type          = hashtable<Key, key_value<const Key, Value>, Allocator, use_first<key_value<const Key, Value>>, Equal, Hash, mod_range_hashing, prime_rehash_policy, true, true>;
      using this_type          = hash_map<Key, Value, Hash, Equal, Allocator>;
      using size_type          = typename base_type::size_type;
      using key_type           = typename base_type::key_type;
      using mapped_type        = Value;
      using value_type         = typename base_type::value_type;
      using allocator_type     = typename base_type::allocator_type;
      using node_type          = typename base_type::node_type;
      using insert_return_type = typename base_type::insert_return_type;
      using iterator           = typename base_type::iterator;
      using const_iterator     = typename base_type::const_iterator;

      using base_type::insert;

      explicit hash_map(const allocator_type& allocator = allocator_type())
          : base_type(0, Hash(), mod_range_hashing(), Equal(), use_first<value_type>(), allocator)
      {
      }

      explicit hash_map(size_type bucketCount, const Hash& hashFunction = Hash(), const Equal& keyEqual = Equal(), const allocator_type& allocator = allocator_type())
          : base_type(bucketCount, hashFunction, mod_range_hashing(), keyEqual, use_first<value_type>(), allocator)
      {
      }

      hash_map(const this_type& other)
          : base_type(other)
      {
      }

      hash_map(this_type&& other)
          : base_type(rsl::move(other))
      {
      }

      ~hash_map() = default;

      hash_map(initializer_list<value_type> ilist, size_type bucketCount = 0, const Hash& hashFunction = Hash(), const Equal& keyEqual = Equal(), const allocator_type& allocator = allocator_type())
          : base_type(ilist.begin(), ilist.end(), bucketCount, hashFunction, mod_range_hashing(), keyEqual, use_first<value_type>(), allocator)
      {
      }

      template <typename ForwardIterator>
      hash_map(ForwardIterator first, ForwardIterator last, size_type bucketCount = 0, const Hash& hashFunction = Hash(), const Equal& keyEqual = Equal(), const allocator_type& allocator = allocator_type())
          : base_type(first, last, bucketCount, hashFunction, mod_range_hashing(), keyEqual, use_first<value_type>(), allocator)
      {
      }

      this_type& operator=(const this_type& other)
      {
        return static_cast<this_type&>(base_type::operator=(other));
      }
      this_type& operator=(this_type&& other)
      {
        return static_cast<this_type&>(base_type::operator=(rsl::move(other)));
      }
      this_type& operator=(initializer_list<value_type> ilist)
      {
        return static_Cast<this_type&>(base_type::operator=(ilist));
      }

      Value& at(const key_type& k)
      {
        iterator it = base_type::find(k);

        if(it == base_type::end())
        {
          REX_ASSERT("Key k not found in hash_map");
        }

        return it->value;
      }
      const Value& at(const key_type& k) const
      {
        const_iterator it = base_type::find(k);

        if(it == base_type::end())
        {
          REX_ASSERT("Key k not found in hash_map");
        }

        return it->value;
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Sep/2022)
      template <typename K>
      Value& at(K&& k)
      {
        iterator it = base_type::find(rsl::forward<K>(k));

        if(it == base_type::end())
        {
          REX_ASSERT("Key k not found in hash_map");
        }

        return it->value;
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Sep/2022)
      template <typename K>
      const Value& at(K&& k) const
      {
        const_iterator it = base_type::find(rsl::forward<K>(k));

        if(it == base_type::end())
        {
          REX_ASSERT("Key k not found in hash_map");
        }

        return it->value;
      }

      mapped_type& operator[](const key_type& key)
      {
        return (*base_type::insert_value(true_type(), key).inserted_element).value;
      }
      mapped_type& operator[](key_type&& key)
      {
        return (*base_type::insert_value(true_type(), key).inserted_element).value;
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Sep/2022)
      template <typename K>
      mapped_type& operator[](K&& k)
      {
        return (*base_type::insert_value(true_type(), rsl::forward<K>(k)).inserted_element).value;
      }
    };

    template <typename Key, typename Value, typename Hash = rsl::hash<Key>, typename Equal = rsl::equal_to<Key>, typename Allocator = allocator>
    class hash_multimap : public hashtable<Key, key_value<const Key, Value>, Allocator, use_first<key_value<const Key, Value>>, Equal, Hash, mod_range_hashing, prime_rehash_policy, true, false>
    {
    public:
      using base_type          = hashtable<Key, key_value<const Key, Value>, Allocator, use_first<key_value<const Key, Value>>, Equal, Hash, mod_range_hashing, prime_rehash_policy, true, false>;
      using this_type          = hash_multimap<Key, Value, Hash, Equal, Allocator>;
      using size_type          = typename base_type::size_type;
      using key_type           = typename base_type::key_type;
      using mapped_type        = Value;
      using value_type         = typename base_type::value_type;
      using allocator_type     = typename base_type::allocator_type;
      using node_type          = typename base_type::node_type;
      using insert_return_type = typename base_type::insert_return_type;
      using iterator           = typename base_type::iterator;

      using base_type::insert;

      explicit hash_multimap(const allocator_type& allocator = allocator_type())
          : base_type(0, Hash(), mod_range_hashing(), Equal(), use_first<value_type>(), allocator)
      {
      }
      explicit hash_multimap(size_type bucketCount, const Hash& hashFunction = Hash(), const Equal& keyEqual = Equal(), const allocator_type& allocator = allocator_type())
          : base_type(bucketCount, hashFunction, mod_range_hashing, keyEqual, use_first<value_type>(), allocator)
      {
      }

      hash_multimap(const this_type& other)
          : base_type(other)
      {
      }
      hash_multimap(this_type&& other)
          : base_type(rsl::move(other))
      {
      }

      hash_multimap(initializer_list<value_type> ilist, size_type bucketCount, const Hash& hashFunction, const Equal& keyEqual, const allocator_type& allocator)
          : base_type(ilist.begin(), ilist.end(), bucketCount, hashFunction, mod_range_hashing(), keyEqual, use_first<value_type>(), allocator)
      {
      }

      template <typename ForwardIterator>
      hash_multimap(ForwardIterator first, ForwardIterator last, size_type bucketCount = 0, const Hash& hash = Hash(), const Equal& keyEqual = Equal(), const allocator_type& alloc = allocator_type())
      {
      }

      ~hash_multimap() = default;

      this_type& operator=(const this_type& other)
      {
        return static_cast<this_type&>(base_type::operator=(other));
      }
      this_type& operator=(this_type&& other)
      {
        return static_Cast<this_type&>(base_type::operator=(rsl::move(other)));
      }
      this_type& operator=(initializer_list<value_type> ilist)
      {
        return static_cast<this_type&>(base_type::operator=(ilist));
      }
    };

  } // namespace v1
} // namespace rsl