// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: hashtable.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/functional/hash_result.h"
#include "rex_std/bonus/hashtable/hashtable_iterator.h"
#include "rex_std/bonus/hashtable/node_iterator.h"
#include "rex_std/bonus/hashtable/rehash_base.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/bonus/utility/emplace_result.h"
#include "rex_std/bonus/utility/range.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/piecewise_construct.h"
#include "rex_std/internal/utility/swap.h"
#include "rex_std/tuple.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Key, typename Value, typename Allocator, typename ExtractKey, typename Equal, typename KeyHash, typename BucketIndexFinder, typename RehashPolicy, bool UsesMutableIterators, bool UsesUniqueKeys>
    class hashtable : public rehash_base<RehashPolicy, hashtable<Key, Value, Allocator, ExtractKey, Equal, KeyHash, BucketIndexFinder, RehashPolicy, UsesMutableIterators, UsesUniqueKeys>>
    {
    public:
      using key_type                 = Key;
      using value_type               = Value;
      using mapped_type              = typename ExtractKey::result_type;
      using allocator_type           = Allocator;
      using key_equal                = Equal;
      using difference_type          = ptrdiff;
      using size_type                = count_t;
      using reference                = value_type&;
      using const_reference          = const value_type&;
      using local_iterator           = node_iterator<value_type, !UsesMutableIterators>;
      using const_local_iterator     = node_iterator<value_type, true>;
      using iterator                 = hashtable_iterator<value_type, !UsesMutableIterators>;
      using const_iterator           = hashtable_iterator<value_type, true>;
      using node_type                = hash_node<value_type>;
      using insert_return_type       = typename type_select<UsesUniqueKeys, insert_result<iterator>, iterator>::type;
      using this_type                = hashtable<Key, Value, Allocator, ExtractKey, Equal, KeyHash, BucketIndexFinder, RehashPolicy, UsesMutableIterators, UsesUniqueKeys>;
      using rehash_policy_type       = RehashPolicy;
      using extract_key_type         = ExtractKey;
      using key_hash_type            = KeyHash;
      using bucket_index_finder_type = BucketIndexFinder;
      using has_unique_keys_type     = bool_constant<UsesUniqueKeys>;

      hashtable()
          : rehash_base<RehashPolicy, hashtable>()
          , m_cp_key_equal_and_bucket_array(nullptr)
          , m_cp_key_hash_and_bucket_count(0)
          , m_cp_bucket_idx_finder_and_element_count(0)
          , m_cp_extract_key_and_rehash_policy()
          , m_allocator()
      {
      }
      hashtable(size_type bucketCount, const KeyHash& keyHash, const BucketIndexFinder& bucketIndexFinder, const Equal& equal, const ExtractKey& extractKey, const allocator_type& allocator)
          : m_cp_key_equal_and_bucket_array(equal)
          , m_cp_key_hash_and_bucket_count(keyHash, bucketCount)
          , m_cp_bucket_idx_finder_and_element_count(bucketIndexFinder)
          , m_cp_extract_key_and_rehash_policy(extractKey)
          , m_allocator(allocator)
      {
        if(bucket_count() > 0)
        {
          m_cp_key_equal_and_bucket_array.second() = allocate_buckets(bucket_count());
        }
      }

      template <typename ForwardIterator>
      hashtable(ForwardIterator first, ForwardIterator last, size_type bucketCount, const KeyHash& keyHash, const BucketIndexFinder& bucketIndexFinder, const Equal& equal, const ExtractKey& extractKey, const allocator_type& allocator)
          : hashtable(bucketCount, keyHash, bucketIndexFinder, equal, extractKey, allocator)
      {
        if(bucketCount < 2)
        {
          const size_type element_count           = static_cast<size_type>(internal::ht_distance(first, last));
          m_cp_key_hash_and_bucket_count.second() = static_cast<size_type>(m_cp_extract_key_and_rehash_policy.second().get_bucket_count(element_count));
        }
        else
        {
          REX_ASSERT_X(bucketCount < 10'000'000, "Bucket count too big for hashtable. bucketcount: {}", bucketCount);
          m_cp_key_hash_and_bucket_count.second() = bucketCount;
        }

        m_cp_key_equal_and_bucket_array.second() = allocate_buckets(bucket_count());

        for(; first != last; ++first)
        {
          insert(*first);
        }
      }

      hashtable(const hashtable& other)
          : rehash_base<RehashPolicy, hashtable>(other)
          , m_cp_key_equal_and_bucket_array(other.m_cp_key_equal_and_bucket_array)
          , m_cp_key_hash_and_bucket_count(other.m_cp_key_hash_and_bucket_count)
          , m_cp_bucket_idx_finder_and_element_count(other.m_cp_bucket_idx_finder_and_element_count)
          , m_cp_extract_key_and_rehash_policy(other.m_cp_extract_key_and_rehash_policy)
          , m_allocator(other.m_allocator)
      {
        if(size())
        {
          m_cp_key_equal_and_bucket_array.second() = allocate_buckets(bucket_count());

          for(size_type i = 0; i < other.bucket_count(); ++i)
          {
            node_type* node_source = other.bucket_array()[i];
            node_type** node_dest  = bucket_array() + i;

            while(node_source)
            {
              *node_dest = allocate_node(node_source->value);
              copy_node(*node_dest, node_source);
              node_dest   = &(*node_dest)->next;
              node_source = node_source->next;
            }
          }
        }
      }

      hashtable(this_type&& other)
          : hashtable()
      {
        swap(other);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Sep/2022)
      // We're not implementing an overload taking it's own allocator, as allocators
      // need to be equivalent when moving in Rex Standard Library

      ~hashtable()
      {
        clear();
        free_buckets(bucket_array());
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Sep/2022)
      // This returns a copy of the allocator in the C++ Standard.
      const allocator_type& get_allocator() const
      {
        return m_allocator;
      }

      this_type& operator=(const this_type& other)
      {
        REX_ASSERT_X(this != addressof(other), "Can't copy to yourself");

        clear();
        insert(other.begin(), other.end());

        return *this;
      }
      this_type& operator=(initializer_list<value_type> ilist)
      {
        clear();
        insert(ilist.begin(), ilist.end());
        return *this;
      }
      this_type& operator=(this_type&& other)
      {
        REX_ASSERT_X(this != addressof(other, "Can't move to yourself"));

        clear();
        swap(other);
        return *this;
      }

      void swap(this_type& other)
      {
        rsl::swap(m_cp_key_equal_and_bucket_array, other.m_cp_key_equal_and_bucket_array);
        rsl::swap(m_cp_key_hash_and_bucket_count, other.m_cp_key_hash_and_bucket_count);
        rsl::swap(m_cp_bucket_idx_finder_and_element_count, other.m_cp_bucket_idx_finder_and_element_count);
        rsl::swap(m_cp_extract_key_and_rehash_policy, other.m_cp_extract_key_and_rehash_policy);
        rsl::swap(m_allocator, other.m_allocator);
      }

      iterator begin()
      {
        iterator it(m_cp_key_equal_and_bucket_array.second());
        if(!it.node())
        {
          it.increment_bucket();
        }
      }
      const_iterator begin() const
      {
        const_iterator it(m_cp_key_equal_and_bucket_array.second());
        if(!it.node())
        {
          it.increment_bucket();
        }
        return it;
      }
      const_iterator cbegin() const
      {
        return begin();
      }
      iterator end()
      {
        return iterator(bucket_array() + bucket_count());
      }
      const_iterator end() const
      {
        return const_iterator(bucket_array() + bucket_count());
      }
      const_iterator cend() const
      {
        return end();
      }

      local_iterator begin(size_type n)
      {
        return local_iterator(m_cp_key_equal_and_bucket_array.second()[n]);
      }
      const_local_iterator begin(size_type n) const
      {
        return const_local_iterator(m_cp_key_equal_and_bucket_array.second()[n]);
      }
      const_local_iterator cbegin(size_type n) const
      {
        return const_local_iterator(m_cp_key_equal_and_bucket_array.second()[n]);
      }

      local_iterator end(size_type /*unused*/)
      {
        return local_iterator(nullptr);
      }
      const_local_iterator end(size_type /*unused*/) const
      {
        return const_local_iterator(nullptr);
      }
      const_local_iterator cend(size_type /*unused*/) const
      {
        return const_local_iterator(nullptr);
      }

      bool empty() const
      {
        return m_cp_bucket_idx_finder_and_element_count.second() == 0;
      }

      size_type size() const
      {
        return m_cp_bucket_idx_finder_and_element_count.second();
      }
      size_type bucket_count() const
      {
        return m_cp_key_hash_and_bucket_count.second();
      }
      size_type bucket_size(size_type n) const
      {
        return static_cast<size_type>(distance(begin(n), end(n)));
      }

      float32 load_factor() const
      {
        const float32 element_count = static_cast<float32>(m_cp_bucket_idx_finder_and_element_count.second());
        const float32 bucket_count  = static_cast<float32>(m_cp_key_hash_and_bucket_count.second());
        return bucket_count == 0.0f ? 0.0f : element_count / bucket_count;
      }

      const rehash_policy_type& rehash_policy() const
      {
        return m_cp_extract_key_and_rehash_policy.second();
      }

      template <typename... Args>
      insert_return_type emplace(Args&&... args)
      {
        return insert_value(has_unique_keys_type(), rsl::forward<Args>(args)...);
      }

      template <typename... Args>
      insert_return_type try_emplace(const key_type& k, Args&&... args)
      {
        return insert_value(has_unique_keys_type(), piecewise_construct, rsl::forward_as_tuple(k), rsl::forward_as_tuple(rsl::forward<Args>(args)...));
      }
      template <typename... Args>
      insert_return_type try_emplace(key_type&& k, Args&&... args)
      {
        return insert_value(has_unique_keys_type(), piecewise_construct, rsl::forward_as_tuple(rsl::move(k)), rsl::forward_as_tuple(rsl::forward<Args>(args)...));
      }

      insert_return_type insert(const value_type& value)
      {
        return insert_value(has_unique_keys_type(), value);
      }
      insert_return_type insert(value_type&& otherValue)
      {
        return insert_value(has_unique_keys_type(), rsl::move(otherValue));
      }
      void insert(initializer_list<value_type> ilist)
      {
        return insert(ilist.begin(), ilist.end());
      }
      template <typename InputIterator>
      void insert(InputIterator first, InputIterator last)
      {
        const count_t num_elements_to_add = internal::ht_distance(first, last);
        hash_required_result result       = m_cp_extract_key_and_rehash_policy.second().is_rehash_required(bucket_count(), size(), num_elements_to_add);

        if(result.is_hash_required)
        {
          rehash(result.new_bucket_count);
        }
        else
        {
          for(; first != last; ++first)
          {
            insert_value(has_unique_keys_type(), *first);
          }
        }
      }

      template <typename P, typename = enable_if_t<!is_constructible_v<value_type, P&&>>>
      insert_return_type insert(P&& otherValue)
      {
        return emplace(rsl::forward<P>(otherValue));
      }

      template <typename M>
      insert_return_type insert_or_assign(const key_type& k, M&& obj)
      {
        auto it = find(k);
        if(it == end())
        {
          return insert(value_type(piecewise_construct, rsl::forward_as_tuple(k), rsl::forward_as_tuple(rsl::forward<M>(obj))));
        }
        else
        {
          it->second = rsl::forward<M>(obj);
          return {it, false};
        }
      }
      template <typename M>
      insert_return_type insert_or_assign(key_type&& k, M&& obj)
      {
        auto it = find(k);
        if(it == end())
        {
          return insert(value_type(piecewise_construct, rsl::forward_as_tuple(rsl::move(k)), rsl::forward_as_tuple(rsl::forward<M>(obj))));
        }
        else
        {
          it->second = rsl::forward<M>(obj);
          return {it, false};
        }
      }

      iterator erase(const_iterator position)
      {
        iterator it_next(position.node(), position.bucket());
        ++it_next;

        node_type* node         = it_next.node();
        node_type* current_node = *it_next.bucket();

        if(current_node == node)
        {
          *it_next.bucket() = current_node->next;
        }
        else
        {
          node_type* next_node = current_node->next;

          while(next_node != node)
          {
            current_node = next_node;
            next_node    = current_node->next;
          }

          current_node->next = next_node->next;
        }

        free_node(node);
        --m_cp_bucket_idx_finder_and_element_count.second();

        return it_next;
      }
      iterator erase(const_iterator first, const_iterator last)
      {
        while(first != last)
          first = erase(first);

        return iterator(first.node(), first.bucket());
      }
      size_type erase(const key_type& k)
      {
        const size_type n                   = static_cast<size_type>(bucket_index(k, bucket_count()));
        const size_type element_count_saved = size();

        node_type** bucket_array = m_cp_key_equal_and_bucket_array.second() + n;

        while(*bucket_array && !compare(k, *bucket_array))
        {
          bucket_array = &(*bucket_array)->next;
        }

        while(*bucket_array && compare(k, *bucket_array))
        {
          node_type* node = *bucket_array;
          *bucket_array   = node->next;
          free_node(node);
          --m_cp_bucket_idx_finder_and_element_count.second();
        }

        return element_count_saved - size();
      }

      template <typename K>
      size_type erase(K&& k)
      {
        const size_type n                   = static_cast<size_type>(bucket_index(k, bucket_count()));
        const size_type element_count_saved = size();

        node_type** bucket_array = bucket_array() + n;

        while(*bucket_array && !compare(rsl::forward<K>(k), *bucket_array))
        {
          bucket_array = &(*bucket_array)->next;
        }

        while(*bucket_array && compare(rsl::forward<K>(k), *bucket_array))
        {
          node_type* node = *bucket_array;
          *bucket_array   = node->next;
          free_node(node);
          --m_cp_bucket_idx_finder_and_element_count.second();
        }

        return element_count_saved - size();
      }

      void clear()
      {
        free_nodes(bucket_array(), bucket_count());
        m_cp_bucket_idx_finder_and_element_count.second() = 0;
      }
      void rehash(size_type bucketCount)
      {
        node_type** const new_bucket_array = allocate_buckets(bucketCount);

        node_type* node = nullptr;

        for(size_type i = 0; i < bucket_count(); ++i)
        {
          while((node = bucket_array()[i]) != nullptr)
          {
            const size_type new_bucket_index = bucket_index(node, bucketCount);

            bucket_array()[i]                  = node->next;
            node->next                         = new_bucket_array[new_bucket_index];
            new_bucket_array[new_bucket_index] = node;
          }
        }

        free_buckets(bucket_array());
        m_cp_key_hash_and_bucket_count.second()  = bucketCount;
        m_cp_key_equal_and_bucket_array.second() = new_bucket_array;
      }
      void reserve(size_type elementCount)
      {
        rehash(rehash_policy().get_bucket_count(elementCount));
      }

      iterator find(const key_type& key)
      {
        const size_type n = static_cast<size_type>(bucket_index(key, bucket_count()));

        node_type* node = find_node(bucket_array()[n], key);
        return node ? iterator(node, bucket_array() + n) : iterator(bucket_array() + bucket_count());
      }
      const_iterator find(const key_type& key) const
      {
        const size_type n = static_cast<size_type>(bucket_index(key, bucket_count()));

        node_type* node = find_node(bucket_array()[n], key);
        return node ? const_iterator(node, bucket_array() + n) : iterator(bucket_array() + bucket_count());
      }
      template <typename K>
      iterator find(const K& x)
      {
        const size_type n = static_cast<size_type>(bucket_index(x, bucket_count()));

        node_type* node = find_node(bucket_array()[n], x);
        return node ? iterator(node, bucket_array() + n) : end();
      }
      template <typename K>
      const_iterator find(const K& x) const
      {
        const size_type n = static_cast<size_type>(bucket_index(x, bucket_count()));

        node_type* node = find_node(bucket_array()[n], x);
        return node ? const_iterator(node, bucket_array() + n) : cend();
      }

      bool contains(const Key& key) const
      {
        return find(key) != cend();
      }
      template <typename K>
      bool contains(const K& x) const
      {
        return find(rsl::forward<K>(x)) != cend();
      }

      range<iterator> equal_range(const key_type& k)
      {
        const size_type n = static_cast<size_type>(bucket_index(k, bucket_count()));
        node_type** head  = bucket_array() + n;
        node_type* node   = find_node(*head, k);

        if(node)
        {
          node_type* p1 = node->next;

          for(; p1; p1 = p1->next)
          {
            if(!compare(k, p1))
            {
              break;
            }
          }

          iterator first(node, head);
          iterator last(p1, head);

          if(!p1)
          {
            last.increment_bucket();
          }

          return {first, last};
        }

        return {end(), end()};
      }
      range<const_iterator> equal_range(const key_type& k) const
      {
        const size_type n = static_cast<size_type>(bucket_index(k, bucket_count()));
        node_type** head  = bucket_array() + n;
        node_type* node   = find_node(*head, k);

        if(node)
        {
          node_type* p1 = node->next;

          for(; p1; p1 = p1->next)
          {
            if(!compare(k, p1))
            {
              break;
            }
          }

          const_iterator first(node, head);
          const_iterator last(p1, head);

          if(!p1)
          {
            last.increment_bucket();
          }

          return {first, last};
        }

        return {end(), end()};
      }

    protected:
      template <typename... Args>
      insert_return_type insert_value(true_type /*unused*/, Args&&... args)
      {
        void* new_node_addr    = static_cast<node_type*>(m_allocator.allocate(sizeof(node_type)));
        node_type* new_node    = new(new_node_addr) node_type(value_type {rsl::forward<Args>(args)...});
        const key_type& k      = m_cp_extract_key_and_rehash_policy.first()(new_node->value);
        const hash_result hr   = m_cp_key_hash_and_bucket_count.first()(k);
        size_type n            = static_cast<size_type>(bucket_index(hr, bucket_count()));
        node_type* node        = nullptr;
        node_type** bucket_arr = bucket_array();
        if(bucket_arr)
        {
          node = find_node(bucket_arr[n], k);
        }

        if(node == nullptr)
        {
          const hash_required_result result = m_cp_extract_key_and_rehash_policy.second().is_rehash_required(bucket_count(), size(), 1);

          if(result.is_hash_required)
          {
            n = static_cast<size_type>(bucket_index(k, result.new_bucket_count));
            rehash(result.new_bucket_count);
          }

          new_node->next                              = m_cp_key_equal_and_bucket_array.second()[n];
          m_cp_key_equal_and_bucket_array.second()[n] = new_node;
          ++m_cp_bucket_idx_finder_and_element_count.second();

          return insert_return_type {iterator(new_node, m_cp_key_equal_and_bucket_array.second() + n), true};
        }
        else
        {
          free_node(new_node);
        }

        return insert_return_type {iterator(node, m_cp_key_equal_and_bucket_array.second() + n), false};
      }

    private:
      node_type**& bucket_array()
      {
        return m_cp_key_equal_and_bucket_array.second();
      }
      bucket_index_finder_type bucket_index_finder()
      {
        return m_cp_bucket_idx_finder_and_element_count.first();
      }
      extract_key_type extract_key()
      {
        return m_cp_extract_key_and_rehash_policy.first();
      }
      rehash_policy_type rehash_policy()
      {
        return m_cp_extract_key_and_rehash_policy.second();
      }

      node_type** allocate_buckets(size_type bucketCount)
      {
        node_type** const bucket_array = static_cast<node_type**>(m_allocator.allocate(bucketCount * sizeof(node_type**)));
        memset(bucket_array, 0, bucketCount * sizeof(node_type**));
        return bucket_array;
      }

      void free_buckets(node_type** bucketArray)
      {
        m_allocator.deallocate(bucketArray);
      }

      void free_nodes(node_type** nodeArray, size_type bucketCount)
      {
        for(size_type i = 0; i < bucketCount; ++i)
        {
          node_type* p_node = nodeArray[i];
          while(p_node)
          {
            node_type* const p_temp_node = p_node;
            p_node                       = p_node->next;
            free_node(p_temp_node);
          }
          nodeArray[i] = nullptr;
        }
      }

      void free_node(node_type* node)
      {
        node->~node_type();
        m_allocator.deallocate(node);
      }

      template <typename... Args>
      node_type* allocate_node(Args&&... args)
      {
        void* node = m_allocator.allocate(sizeof(node_type));
        new(node) node_type(rsl::forward<Args>(args)...);
        return node;
      }

      size_type bucket_index(hash_result hr, size_type bucketCount)
      {
        return static_cast<size_type>(hr) % bucketCount;
      }

      size_type bucket_index(node_type* node, size_type bucketCount)
      {
        const hash_result hr = m_cp_key_hash_and_bucket_count.first()(m_cp_extract_key_and_rehash_policy.first()(node->value));
        return bucket_index(hr, bucketCount);
      }

      node_type* find_node(node_type* node, const key_type& k)
      {
        for(; node; node = node->next)
        {
          if(m_cp_key_equal_and_bucket_array.first()(k, m_cp_extract_key_and_rehash_policy.first()(node->value)))
          {
            return node;
          }
        }

        return nullptr;
      }

    private:
      compressed_pair<key_equal, node_type**> m_cp_key_equal_and_bucket_array;
      compressed_pair<key_hash_type, size_type> m_cp_key_hash_and_bucket_count;
      compressed_pair<bucket_index_finder_type, size_type> m_cp_bucket_idx_finder_and_element_count;
      compressed_pair<extract_key_type, rehash_policy_type> m_cp_extract_key_and_rehash_policy;
      allocator_type m_allocator;
    };

  } // namespace v1
} // namespace rsl

#include "rex_std/internal/assert/assert_impl.h"
