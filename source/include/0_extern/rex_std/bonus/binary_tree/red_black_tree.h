// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: red_black_tree.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/binary_tree/red_black_tree_base.h"
#include "rex_std/bonus/utility/compressed_pair.h"
#include "rex_std/bonus/utility/emplace_result.h"
#include "rex_std/bonus/utility/key_value.h"
#include "rex_std/bonus/utility/output_param.h"
#include "rex_std/initializer_list.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/memory/allocator_traits.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/pair.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      enum class RedBlackTreeSide
      {
        Left,
        Right
      };

      enum class RedBlackTreeColor : char
      {
        Red,
        Black
      };

      struct RedBlackTreeNodeBase
      {
      private:
        using this_type = RedBlackTreeNodeBase;

      public:
        this_type* right_node;
        this_type* left_node;
        this_type* parent_node;
        RedBlackTreeColor color; // red or black?
      };

      template <typename Value>
      struct RedBlackTreeNode : RedBlackTreeNodeBase
      {
        Value value;

#if defined(MSC_VER)
        RedBlackTreeNode(const RedBlackTreeNode&) = delete;
#endif
      };

      RedBlackTreeNodeBase* red_black_tree_min_child(const RedBlackTreeNodeBase* nodeBase);
      RedBlackTreeNodeBase* red_black_tree_max_child(const RedBlackTreeNodeBase* nodeBase);
      void red_black_tree_insert(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* nodeParent, RedBlackTreeNodeBase* nodeAnchor, internal::RedBlackTreeSide insertionSide);
      RedBlackTreeNodeBase* red_black_tree_increment(const RedBlackTreeNodeBase* node);
      RedBlackTreeNodeBase* red_black_tree_decrement(const RedBlackTreeNodeBase* node);
      count_t red_black_tree_black_count(const RedBlackTreeNodeBase* top_node, const RedBlackTreeNodeBase* bottom_node);
      RedBlackTreeNodeBase* red_black_tree_rotate_left(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* root_node);
      RedBlackTreeNodeBase* red_black_tree_rotate_right(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* root_node);
      void red_black_tree_erase(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* nodeAnchor);

      RedBlackTreeNodeBase* red_black_tree_min_child(const RedBlackTreeNodeBase* nodeBase)
      {
        while(nodeBase->left_node)
        {
          nodeBase = nodeBase->left_node;
        }
        return const_cast<RedBlackTreeNodeBase*>(nodeBase);
      }
      RedBlackTreeNodeBase* red_black_tree_max_child(const RedBlackTreeNodeBase* nodeBase)
      {
        while(nodeBase->right_node)
        {
          nodeBase = nodeBase->right_node;
        }
        return const_cast<RedBlackTreeNodeBase*>(nodeBase);
      }

      void red_black_tree_insert(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* nodeParent, RedBlackTreeNodeBase* nodeAnchor, internal::RedBlackTreeSide insertionSide)
      {
        RedBlackTreeNodeBase*& node_root_ref = nodeAnchor->parent_node;

        node->parent_node = nodeParent;
        node->right_node  = nullptr;
        node->left_node   = nullptr;
        node->color       = internal::RedBlackTreeColor::Red;

        if(insertionSide == internal::RedBlackTreeSide::Left)
        {
          nodeParent->left_node = node;

          if(nodeParent == nodeAnchor)
          {
            nodeAnchor->parent_node = node;
            nodeAnchor->right_node  = node;
          }
          else if(nodeParent == nodeAnchor->left_node)
          {
            nodeAnchor->left_node = node;
          }
        }
        else
        {
          nodeParent->right_node = node;

          if(nodeParent == nodeAnchor->right_node)
          {
            nodeAnchor->right_node = node;
          }
        }

        while((node != node_root_ref) && (node->parent_node->color == internal::RedBlackTreeColor::Red))
        {
          REX_ASSERT_X(node->parent_node != nullptr, "inserting a node without a parent");
          RedBlackTreeNodeBase* node_parent_parent = node->parent_node->parent_node;

          if(node->parent_node == node_parent_parent->left_node)
          {
            RedBlackTreeNodeBase* temp_node = node_parent_parent->right_node;

            if(temp_node && (temp_node->color == internal::RedBlackTreeColor::Red))
            {
              node->parent_node->color  = internal::RedBlackTreeColor::Black;
              temp_node->color          = internal::RedBlackTreeColor::Black;
              node_parent_parent->color = internal::RedBlackTreeColor::Red;
              node                      = node_parent_parent;
            }
            else
            {
              if(node->parent_node && node == node->parent_node->right_node)
              {
                node          = node->parent_node;
                node_root_ref = red_black_tree_rotate_left(node, node_root_ref);
              }

              REX_ASSERT_X(node->parent_node != nullptr, "inserting a node without a parent");
            }
          }
          else
          {
            RedBlackTreeNodeBase* temp_node = node_parent_parent->left_node;

            if(temp_node && (temp_node->color == internal::RedBlackTreeColor::Red))
            {
              node->parent_node->color  = internal::RedBlackTreeColor::Black;
              temp_node->color          = internal::RedBlackTreeColor::Black;
              node_parent_parent->color = internal::RedBlackTreeColor::Red;
              node                      = node_parent_parent;
            }
            else
            {
              REX_ASSERT_X(node != nullptr && node->parent_node != nullptr);

              if(node == node->parent_node->left_node)
              {
                node          = node->parent_node;
                node_root_ref = red_black_tree_rotate_right(node, node_root_ref);
              }

              node->parent_node->color  = internal::RedBlackTreeColor::Black;
              node_parent_parent->color = internal::RedBlackTreeColor::Red;
              node_root_ref             = red_black_tree_rotate_left(node_parent_parent, node_root_ref);
            }
          }

          REX_ASSERT_X(node_root_ref != nullptr);
          node_root_ref->color = internal::RedBlackTreeColor::Black;
        }
      }

      // returns the next item in a sorted red-black tree
      RedBlackTreeNodeBase* red_black_tree_increment(const RedBlackTreeNodeBase* node)
      {
        if(node->right_node)
        {
          node = node->right_node;

          while(node->left_node)
          {
            node = node->left_node;
          }
        }
        else
        {
          RedBlackTreeNodeBase* temp_node = node->parent_node;

          while(node == temp_node->right_node)
          {
            node      = temp_node;
            temp_node = temp_node->parent_node;
          }

          if(node->right_node != temp_node)
          {
            node = temp_node;
          }
        }

        return const_cast<RedBlackTreeNodeBase*>(node);
      }

      // returns the previous item in a sorted red-black tree
      RedBlackTreeNodeBase* red_black_tree_decrement(const RedBlackTreeNodeBase* node)
      {
        if((node->parent_node->parent_node == node) && (node->color == internal::RedBlackTreeColor::Red))
        {
          return node->right_node;
        }
        else if(node->left_node)
        {
          RedBlackTreeNodeBase* temp_node = node->left_node;

          while(temp_node->right_node)
          {
            temp_node = temp_node->right_node;
          }

          return temp_node;
        }

        RedBlackTreeNodeBase* temp_node = node->parent_node;

        while(node == temp_node->left_node)
        {
          node      = temp_node;
          temp_node = temp_node->parent_node;
        }

        return const_cast<RedBlackTreeNodeBase*>(temp_node);
      }

      count_t red_black_tree_black_count(const RedBlackTreeNodeBase* top_node, const RedBlackTreeNodeBase* bottom_node)
      {
        count_t count = 0;

        for(; bottom_node; bottom_node = bottom_node->parent_node)
        {
          if(bottom_node->color == internal::RedBlackTreeColor::Black)
          {
            ++count;
          }

          if(bottom_node == top_node)
          {
            break;
          }
        }

        return count;
      }

      RedBlackTreeNodeBase* red_black_tree_rotate_left(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* root_node)
      {
        RedBlackTreeNodeBase* temp_node = node->right_node;

        node->right_node = temp_node->left_node;

        if(temp_node->left_node)
        {
          temp_node->left_node->parent_node = node;
        }
        temp_node->parent_node = node->parent_node;

        if(node == root_node)
        {
          root_node = temp_node;
        }
        else if(node == node->parent_node->left_node)
        {
          node->parent_node->left_node = temp_node;
        }
        else
        {
          node->parent_node->right_node = temp_node;
        }

        temp_node->left_node = node;
        node->parent_node    = temp_node;

        return root_node;
      }

      RedBlackTreeNodeBase* red_black_tree_rotate_right(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* root_node)
      {
        RedBlackTreeNodeBase* temp_node = node->left_node;

        node->left_node = temp_node->right_node;

        if(temp_node->right_node)
        {
          temp_node->right_node->parent_node = node;
        }
        temp_node->parent_node = node->parent_node;

        if(node == root_node)
        {
          root_node = temp_node;
        }
        else if(node == node->parent_node->right_node)
        {
          node->parent_node->right_node = temp_node;
        }
        else
        {
          node->parent_node->left_node = temp_node;
        }

        temp_node->right_node = node;
        node->parent_node     = temp_node;

        return root_node;
      }

      void red_black_tree_erase(RedBlackTreeNodeBase* node, RedBlackTreeNodeBase* nodeAnchor)
      {
        RedBlackTreeNodeBase*& node_root_ref      = nodeAnchor->parent_node;
        RedBlackTreeNodeBase*& node_leftmost_ref  = nodeAnchor->left_node;
        RedBlackTreeNodeBase*& node_rightmost_ref = nodeAnchor->right_node;
        RedBlackTreeNodeBase* node_successor      = node;
        RedBlackTreeNodeBase* child_node          = nullptr;
        RedBlackTreeNodeBase* child_node_parent   = nullptr;

        if(node_successor->left_node == nullptr)
        {
          child_node = node_successor->right_node;
        }
        else if(node_successor->right_node == nullptr)
        {
          child_node = node_successor->left_node;
        }
        else
        {
          node_successor = node_successor->right_node;

          while(node_successor->left_node)
          {
            node_successor = node_successor->left_node;
          }

          child_node = node_successor->right_node;
        }

        // here we remove node from the tree and fix up the node pointers appropriately around it
        if(node_successor == node)
        {
          child_node_parent = node_successor->parent_node;

          if(child_node)
          {
            child_node->parent_node = node_successor->parent_node;
          }

          if(node == node_root_ref)
          {
            node_root_ref = child_node;
          }
          else
          {
            if(node == node->parent_node->left_node)
            {
              node->parent_node->left_node = child_node;
            }
            else
            {
              node->parent_node->right_node = child_node;
            }
          }

          if(node == node_leftmost_ref)
          {
            if(node->right_node && child_node)
            {
              REX_ASSERT_X(child_node != nullptr, "there should always be a child node");
              node_leftmost_ref = red_black_tree_min_child(child_node);
            }
            else
            {
              node_leftmost_ref = node->parent_node;
            }
          }

          if(node == node_rightmost_ref)
          {
            if(node->left_node && child_node)
            {
              REX_ASSERT_X(child_node != nullptr, "there should always be a child node");
              node_rightmost_ref = red_black_tree_max_child(child_node);
            }
            else
            {
              node_rightmost_ref = node->parent_node;
            }
          }
        }
        else
        {
          node->left_node->parent_node = node_successor;
          node_successor->left_node    = node->left_node;

          if(node_successor == node->right_node)
          {
            child_node_parent = node_successor;
          }
          else
          {
            child_node_parent = node_successor->parent_node;

            if(child_node)
            {
              child_node->parent_node = child_node_parent;
            }

            child_node_parent->left_node = child_node;

            node_successor->right_node    = node->right_node;
            node->right_node->parent_node = node_successor;
          }

          if(node == node_root_ref)
          {
            node_root_ref = node_successor;
          }
          else if(node == node->parent_node->left_node)
          {
            node->parent_node->left_node = node_successor;
          }
          else
          {
            node->parent_node->right_node = node_successor;
          }

          // now node is disconnected from the tree
          node_successor->parent_node = node->parent_node;
          ::rsl::swap(node_successor->color, node->color);
        }

        // here we do tree balancing as per the conventional red-black tree algorithm
        if(node->color == internal::RedBlackTreeColor::Black)
        {
          while((child_node != node_root_ref) && ((child_node == nullptr) || (child_node->color == internal::RedBlackTreeColor::Black)))
          {
            if(child_node == child_node_parent->left_node)
            {
              RedBlackTreeNodeBase* temp_node = child_node_parent->right_node;

              if(temp_node->color == internal::RedBlackTreeColor::Red)
              {
                temp_node->color         = internal::RedBlackTreeColor::Black;
                child_node_parent->color = internal::RedBlackTreeColor::Red;
                node_root_ref            = red_black_tree_rotate_left(child_node_parent, node_root_ref);
                temp_node                = child_node_parent->right_node;
              }

              if(((temp_node->left_node == nullptr) || (temp_node->left_node->color == internal::RedBlackTreeColor::Black)) && ((temp_node->right_node == nullptr) || (temp_node->right_node->color == internal::RedBlackTreeColor::Black)))
              {
                temp_node->color  = internal::RedBlackTreeColor::Red;
                child_node        = child_node_parent;
                child_node_parent = child_node_parent->parent_node;
              }
              else
              {
                if((temp_node->right_node == nullptr) || (temp_node->right_node->color == internal::RedBlackTreeColor::Black))
                {
                  temp_node->left_node->color = internal::RedBlackTreeColor::Black;
                  temp_node->color            = internal::RedBlackTreeColor::Red;
                  node_root_ref               = red_black_tree_rotate_right(temp_node, node_root_ref);
                  temp_node                   = child_node_parent->right_node;
                }

                temp_node->color         = child_node_parent->color;
                child_node_parent->color = internal::RedBlackTreeColor::Black;

                if(temp_node->right_node)
                {
                  temp_node->right_node->color = internal::RedBlackTreeColor::Black;
                }

                node_root_ref = red_black_tree_rotate_left(child_node_parent, node_root_ref);
                break;
              }
            }
            else
            {
              RedBlackTreeNodeBase* temp_node = child_node_parent->left_node;

              if(temp_node->color == internal::RedBlackTreeColor::Red)
              {
                temp_node->color         = internal::RedBlackTreeColor::Black;
                child_node_parent->color = internal::RedBlackTreeColor::Red;

                node_root_ref = red_black_tree_rotate_right(child_node_parent, node_root_ref);
                temp_node     = child_node_parent->left_node;
              }

              if(((temp_node->right_node == nullptr) || (temp_node->right_node->color == internal::RedBlackTreeColor::Black)) && ((temp_node->left_node == nullptr) || (temp_node->left_node->color == internal::RedBlackTreeColor::Black)))
              {
                temp_node->color  = internal::RedBlackTreeColor::Red;
                child_node        = child_node_parent;
                child_node_parent = child_node_parent->parent_node;
              }
              else
              {
                if((temp_node->left_node == nullptr) || (temp_node->left_node->color == internal::RedBlackTreeColor::Black))
                {
                  temp_node->right_node->color = internal::RedBlackTreeColor::Black;
                  temp_node->color             = internal::RedBlackTreeColor::Red;

                  node_root_ref = red_black_tree_rotate_left(temp_node, node_root_ref);
                  temp_node     = child_node_parent->left_node;
                }

                temp_node->color         = child_node_parent->color;
                child_node_parent->color = internal::RedBlackTreeColor::Black;

                if(temp_node->left_node)
                {
                  temp_node->left_node->color = internal::RedBlackTreeColor::Black;
                }

                node_root_ref = red_black_tree_rotate_right(child_node_parent, node_root_ref);
                break;
              }
            }
          }

          if(child_node)
          {
            child_node->color = internal::RedBlackTreeColor::Black;
          }
        }
      }
    } // namespace internal

    template <typename T, typename Pointer, typename Reference>
    class RedBlackTreeIterator
    {
    public:
      using this_type         = RedBlackTreeIterator<T, Pointer, Reference>;
      using iterator          = RedBlackTreeIterator<T, T*, T&>;
      using const_iterator    = RedBlackTreeIterator<T, const T*, const T&>;
      using size_type         = count_t;
      using different_type    = ptrdiff;
      using value_type        = T;
      using base_node_type    = internal::RedBlackTreeNodeBase;
      using node_type         = internal::RedBlackTreeNode<T>;
      using pointer           = Pointer;
      using reference         = Reference;
      using iterator_category = rsl::bidirectional_iterator_tag;

      RedBlackTreeIterator()
          : m_node(nullptr)
      {
      }
      explicit RedBlackTreeIterator(const node_type* node)
          : m_node(static_cast<node_type*>(const_cast<node_type*>(node)))
      {
      }
      RedBlackTreeIterator(const iterator& other)
          : m_node(other.m_node)
      {
      }
      RedBlackTreeIterator& operator=(const iterator& other)
      {
        m_node = other.m_node;
        return *this;
      }

      reference operator*() const
      {
        return m_node->value;
      }
      pointer operator->() const
      {
        return &m_node->value;
      }

      RedBlackTreeIterator& operator++()
      {
        m_node = static_cast<node_type*>(internal::red_black_tree_increment(m_node));
        return *this;
      }
      RedBlackTreeIterator operator++(int)
      {
        this_type temp = *this;
        m_node         = static_cast<node_type*>(internal::red_black_tree_increment(m_node));
        return temp;
      }

      RedBlackTreeIterator& operator--()
      {
        m_node = static_cast<node_type*>(internal::red_black_tree_decrement(m_node));
        return *this;
      }
      RedBlackTreeIterator operator--(int)
      {
        this_type temp = *this;
        m_node         = static_cast<node_type*>(internal::red_black_tree_decrement(m_node));
        return temp;
      }

      node_type* node()
      {
        return m_node;
      }

    private:
      node_type* m_node;
    };

    template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
    bool operator==(const RedBlackTreeIterator<T, PointerA, ReferenceA>& lhs, const RedBlackTreeIterator<T, PointerB, ReferenceB>& rhs)
    {
      return lhs.node() == rhs.node();
    }
    template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
    bool operator!=(const RedBlackTreeIterator<T, PointerA, ReferenceA>& lhs, const RedBlackTreeIterator<T, PointerB, ReferenceB>& rhs)
    {
      return lhs.node() != rhs.node();
    }
    template <typename T, typename Pointer, typename Reference>
    bool operator!=(const RedBlackTreeIterator<T, Pointer, Reference>& lhs, const RedBlackTreeIterator<T, Pointer, Reference>& rhs)
    {
      return lhs.node() != rhs.node();
    }

    template <typename Key, typename Value, typename Compare, typename Allocator, typename ExtractKey, bool bMutableIterator, bool bUniqueKeys>
    class RedBlackTree : public RedBlackTreeBase<Key, Value, Compare, ExtractKey, bUniqueKeys, RedBlackTree<Key, Value, Compare, allocator, ExtractKey, bMutableIterator, bUniqueKeys>>
    {
    private:
      using has_unique_keys_type = bool_constant<bUniqueKeys>;
      using base_type            = RedBlackTreeBase<Key, Value, Compare, ExtractKey, bUniqueKeys, RedBlackTree<Key, Value, Compare, allocator, ExtractKey, bMutableIterator, bUniqueKeys>>;
      using this_type            = RedBlackTree<Key, Value, Compare, allocator, ExtractKey, bMutableIterator, bUniqueKeys>;
      using extract_key          = ExtractKey;

    public:
      using key_type               = Key;
      using value_type             = Value;
      using size_type              = count_t; /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      using difference_type        = int32;
      using key_compare            = Compare;
      using allocator_type         = allocator;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using pointer                = allocator_traits<allocator>::pointer;
      using const_pointer          = allocator_traits<allocator>::const_pointer;
      using iterator               = RedBlackTreeIterator<value_type, value_type*, value_type&>;
      using const_iterator         = RedBlackTreeIterator<const value_type, const value_type*, const value_type&>;
      using reverse_iterator       = rsl::reverse_iterator<iterator>;
      using const_reverse_iterator = rsl::reverse_iterator<const_iterator>;
      using node_type              = internal::RedBlackTreeNode<value_type>;

      RedBlackTree()
          : m_anchor()
          , m_cp_size_and_allocator(0)
      {
        reset();
      }
      RedBlackTree(const allocator_type& allocator)
          : m_anchor()
          , m_cp_size_and_allocator(0, allocator)
      {
        reset();
      }
      RedBlackTree(const Compare& compare, const allocator_type& allocator = allocator_type())
          : base_type(compare)
          , m_anchor()
          , m_cp_size_and_allocator(0, allocator)
      {
        reset();
      }
      RedBlackTree(const this_type& other)
          : base_type(other.get_compare())
          , m_anchor()
          , m_cp_size_and_allocator(other.m_cp_size_and_allocator)
      {
        reset();

        if(other.m_anchor.parent_node)
        {
          m_anchor.parent_node = copy_subtree(static_cast<const node_type*>(other.anchor.parent_node), static_cast<node_type*>(&m_anchor));
          m_anchor.right_node  = red_black_tree_max_child(m_anchor.parent_node);
          m_anchor.left_node   = red_black_tree_min_child(m_anchor.parent_node);
        }
      }
      RedBlackTree(const this_type& other, const allocator& alloc)
          : base_type(other.get_compare())
          , m_anchor()
          , m_cp_size_and_allocator(other.size(), alloc)
      {
        reset();

        if(other.m_anchor.parent_node)
        {
          m_anchor.parent_node = copy_subtree(static_cast<const node_type*>(other.anchor.parent_node), static_cast<node_type*>(&m_anchor));
          m_anchor.right_node  = red_black_tree_max_child(m_anchor.parent_node);
          m_anchor.left_node   = red_black_tree_min_child(m_anchor.parent_node);
        }
      }
      RedBlackTree(this_type&& other)
          : base_type(other.get_compare())
          , m_anchor()
          , m_cp_size_and_allocator(other.m_cp_size_and_allocator)
      {
        reset();
        swap(other);
      }
      RedBlackTree(this_type&& other, const allocator_type& alloc)
          : base_type(other.get_compare())
          , m_anchor()
          , m_cp_size_and_allocator(other.size(), alloc)
      {
        reset();
        swap(other);
      }

      template <typename InputIt>
      RedBlackTree(InputIt first, InputIt last, const Compare& compare, const allocator_type& alloc = allocator_type())
          : base_type(compare)
          , m_anchor()
          , m_cp_size_and_allocator(0, allocator)
      {
        reset();
        for(; first != last; ++first)
        {
          insert(*first);
        }
      }

      ~RedBlackTree()
      {
        nuke_subtree(static_cast<node_type*>(m_anchor.parent_node));
      }

      RedBlackTree& operator=(const this_type& other)
      {
        REX_ASSERT_X(this, &other, "Can't copy to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in copy assignment, this is not allowed");

        clear();
        base_type::get_compare() = other.get_compare();

        if(other.m_anchor.parent_node)
        {
          m_anchor.parent_node            = copy_subtree(static_cast<const node_type*>(other.m_anchor.parent_node), static_cast<node_type*>(&m_anchor));
          m_anchor.right_node             = red_black_tree_max_child(m_anchor.parent_node);
          m_anchor.left_node              = red_black_tree_min_child(m_anchor.parent_node);
          m_cp_size_and_allocator.first() = other.m_cp_size_and_allocator.first();
        }

        return *this;
      }
      RedBlackTree& operator=(this_type&& other)
      {
        REX_ASSERT_X(this, &other, "Can't copy to yourself");
        REX_ASSERT_X(get_allocator() == other.get_allocator(), "Different allocators in move assignment, this is not allowed");

        clear();
        swap(other);

        return *this;
      }
      RedBlackTree& operator=(rsl::initializer_list<value_type> ilist)
      {
        clear();

        for(auto it = ilist.begin(), end = ilist.end(); it != end; ++it)
        {
          insert_value(has_unique_keys_type(), rsl::move(*it));
        }

        return *this;
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      // this returns a copy in the standard
      const allocator_type& get_allocator() const
      {
        return m_cp_size_and_allocator.second();
      }

      // returns an iterator to the first element of the map
      iterator begin()
      {
        return iterator(static_cast<node_type*>(m_anchor.left_node));
      }
      // returns an iterator to the first element of the map
      const_iterator begin()
      {
        return const_iterator(static_cast<node_type*>(const_cast<internal::RedBlackTreeNodeBase*>(m_anchor.left_node)));
      }
      // returns an iterator to the first element of the map
      const_iterator cbegin()
      {
        return const_iterator(static_cast<node_type*>(const_cast<internal::RedBlackTreeNodeBase*>(m_anchor.left_node)));
      }

      // returns an iterator to the element following the last element of the map
      iterator end()
      {
        return iterator(static_cast<node_type*>(&m_anchor));
      }
      // returns an iterator to the element following the last element of the map
      const_iterator end() const
      {
        return const_iterator(static_cast<node_type*>(const_cast<internal::RedBlackTreeNodeBase*>(&m_anchor)));
      }
      // returns an iterator to the element following the last element of the map
      const_iterator cend() const
      {
        return const_iterator(static_cast<node_type*>(const_cast<internal::RedBlackTreeNodeBase*>(&m_anchor)));
      }

      // returns a reverse iterator to the first element of the reversed map
      reverse_iterator rbegin()
      {
        return reverse_iterator(end());
      }
      // returns a reverse iterator to the first element of the reversed map
      const_reverse_iterator rbegin() const
      {
        return const_reverse_iterator(end());
      }
      // returns a reverse iterator to the first element of the reversed map
      const_reverse_iterator crbegin() const
      {
        return const_reverse_iterator(end());
      }

      // returns a reverse iterator to the element following the last element of the reversed map
      reverse_iterator rend()
      {
        return reverse_iterator(begin());
      }
      // returns a reverse iterator to the element following the last element of the reversed map
      const_reverse_iterator rend() const
      {
        return const_reverse_iterator(begin());
      }
      // returns a reverse iterator to the element following the last element of the reversed map
      const_reverse_iterator crend() const
      {
        return const_reverse_iterator(begin());
      }

      // checks whether the container is empty
      bool empty() const
      {
        return size() == 0;
      }
      // returns the number of elements
      size_type size() const
      {
        return m_cp_size_and_allocator.first();
      }
      // returns the max possible number of elements
      size_type max_size() const
      {
        return numeric_limits<size_type>::max();
      }

      // clears the contents
      void clear()
      {
        nuke_subtree(static_Cast<node_type*>(m_anchor.parent_node));
        reset();
      }

      // inserts value into the container
      insert_result<iterator> insert(const value_type& value)
      {
        insert(has_unique_keys_type(), value);
      }
      // inserts value into the container, equivalent to emplace
      template <typename P>
      insert_result<iterator> insert(P&& value)
      {
        return emplace(rsl::forward<P>(value));
      }
      // inserts value into the container using move symantics
      insert_result<iterator> insert(value_type&& value)
      {
        insert(has_unique_keys_type(), rsl::move(value));
      }
      /// RSL Comment: Different From ISO C++ Standard at time of writing (13/Aug/2022)
      // The following functions are not defined in RSL because a client should not have to care where a key value pair will be stored.
      // iterator insert(const_iterator hint, const value_type& value);
      // template <typename P>
      // iterator insert(const_iterator hint, P&& value);
      // iterator insert(const_iterator hint, value_type&& value);

      // inserts elements from range [first, last)
      template <typename InputIt>
      void insert(InputIt first, InputIt last)
      {
        for(; first != last; ++first)
        {
          insert(has_unique_keys_type(), *first);
        }
      }
      // inserts elements from initializer list
      void insert(rsl::initializer_list<value_type> ilist)
      {
        for(auto it = ilist.begin(), end = ilist.end(); it != end; ++it)
        {
          insert(has_unique_keys_type(), rsl::move(*it));
        }
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      // The following functions are not defined in RSL as they break encapsulation.
      // template <typename Iter, typename NodeType>
      // struct insert_return_type
      // {
      //     Iter position;
      //     bool inserted;
      //     NodeType node;
      // };
      // insert_return_type insert(node_type&& nh);
      // iterator insert(const_iterator hint, node_type&& nh);

      /// RSL Comment: Different From ISO C++ Standard at time of writing (13/Aug/2022)
      // The following function is not defined in RSL because a client should not have to care where a key value pair will be stored.
      // iterator insert(const_iterator hint, node_type&& nh);

      // if a key equivalent to k already exists, assigns obj
      // if not, inserts a new value as if by calling insert
      template <typename M>
      insert_result<iterator> insert_or_assign(const Key& k, M& obj)
      {
        auto it = find(k);

        if(it == end())
        {
          return insert(value_type(piecewise_construct, rsl::forward_as_tuple(k), rsl::forward_as_tuple(rsl::forward<M>(obj))));
        }
        else
        {
          it->value = rsl::forward<M>(obj);
          return {it, false};
        }
      }
      template <typename M>
      insert_result<iterator> insert_or_assign(Key&& k, M&& obj)
      {
        auto it = find(k);

        if(it == end())
        {
          return insert(value_type(piecewise_construct, rsl::forward_as_tuple(rsl::move(k)), rsl::forward_as_tuple(rsl::forward<M>(obj))));
        }
        else
        {
          it->value = rsl::forward<M>(obj);
          return {it, false};
        }
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      // The following functions are not defined in RSL because a client should not have to care where a key value pair will be stored
      // template <typename M>
      // insert_result<iterator> insert_or_assign(const_iterator hint, const Key& k, M&& obj);
      // template <typename M>
      // insert_result<iterator> insert_or_assign(const_iterator hint, Key&& k, M&& obj);

      // inserts a new element into the container constructed in place
      template <typename... Args>
      emplace_result<iterator> emplace(Args&&... args)
      {
        return insert(has_unique_keys_type(), rsl::forward<Args>(args)...);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      // The following functions are not defined in RSL because a client should not have to care where a key value pair will be stored
      // template <typename ... Args>
      // iterator emplace_hint(const_iterator hint, Args&& ... args);

      // inserts a new element into the container with the given k if it doesn't exist yet.
      template <typename... Args>
      emplace_result<iterator> try_emplace(const Key& k, Args&&... args)
      {
        return insert(has_unique_keys_type(), piecewise_construct, rsl::forward_as_tuple(k), rsl::forward_as_tuple(rsl::forward<Args>(args)...));
      }
      template <typename... Args>
      emplace_result<iterator> try_emplace(Key&& k, Args&&... args)
      {
        return insert(has_unique_keys_type(), piecewise_construct, rsl::forward_as_tuple(rsl::move(k)), rsl::forward_as_tuple(rsl::forward<Args>(args)...));
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Aug/2022)
      // The following functions are not defined in RSL because a client should not have to care where a key value pair will be stored
      // template <typename ... Args>
      // emplace_result try_emplace(const_iterator hint, const key_type& k, Args&& ... args);
      // template <typename ... Args>
      // emplace_result try_emplace(const_iterator hint, key_type&& k, Args&& ... args);

      // removes the element at pos
      iterator erase(iterator pos)
      {
        return erase(const_iterator(pos.node));
      }
      // removes the element at pos
      iterator erase(const_iterator pos)
      {
        const iterator it_erase(pos.node);
        --m_cp_size_and_allocator.first();
        ++pos;
        internal::red_black_tree_erase(it_erase.node, &m_anchor);
        free_node(it_erase.node);
        return iterator(pos.node);
      }
      // removes the element in range [first, last)
      iterator erase(const_iterator first, const_iterator last)
      {
        if((first.node != m_anchor.left_node) || (last.node != &m_anchor))
        {
          while(first != last)
          {
            first = erase(first);

            return iterator(first.node);
          }
        }

        clear();
        return iterator(static_cast<node_type*>(&m_anchor));
      }

      // exchanges the contents of the container with those of other
      void swap(RedBlackTree& other)
      {
        rsl::swap(m_cp_size_and_allocator, other.m_cp_size_and_allocator);
        rsl::swap(base_type::get_compare(), other.get_compare());

        // most common case, both trees have elements allocated to them
        if(m_anchor.parent_node && other.m_anchor.parent_node)
        {
          rsl::swap(m_anchor.right_node, other.right_node);
          rsl::swap(m_anchor.left_node, other.m_anchor.left_node);
          rsl::swap(m_anchor.parent_node, other.m_anchor.parent_node);

          // we need to fix up the anchors to points to themselves (we can't just swap them)
          m_anchor.parent_node->parent_node       = &m_anchor;
          other.m_anchor.parent_node->parent_node = &other.m_anchor;
        }
        else if(m_anchor.parent_node)
        {
          other.m_anchor.right_node               = m_anchor.right_node;
          other.m_anchor.left_node                = m_anchor.left_node;
          other.m_anchor.parent_node              = m_anchor.parent_node;
          other.m_anchor.parent_node->parent_node = &other.m_anchor;

          // we need to fix up our anchor to point to itself (we can't have it swap with other)
          m_anchor.right_node  = &m_anchor;
          m_anchor.left_node   = &m_anchor;
          m_anchor.parent_node = nullptr;
        }
        else if(other.m_anchor.parent_node)
        {
          m_anchor.right_node               = other.m_anchor.right_node;
          m_anchor.left_node                = other.m_anchor.left_node;
          m_anchor.parent_node              = other.m_anchor.parent_node;
          m_anchor.parent_node->parent_node = &m_anchor;

          other.m_anchor.right_node  = &other.m_anchor;
          other.m_anchor.left_node   = &other.m_anchor;
          other.m_anchor.parent_node = nullptr;
        }
      }

      // The following functions are not defined in RSL as they break encapsulation.
      // node_type extract(const_iterator position);
      // node_type extract(const Key& k);
      // template <typename K>
      // node_type extract(K&& x);

      // finds an element with the given key
      iterator find(const Key& key)
      {
        return find<Key>(key);
      }
      // finds an element with the given key
      const_iterator find(const Key& key) const
      {
        return const_iterator(const_cast<this_type>(this)->find(key));
      }
      // finds an element with a key that's compares equivalent to x
      template <typename K>
      iterator find(const K& x)
      {
        extract_key extract_key;

        node_type* current   = static_cast<node_type*>(m_anchor.node_parent);
        node_type* range_end = static_cast<node_type*>(&m_anchor);

        while(current)
        {
          if(!compare(extract_key(current->value), x))
          {
            range_end = current;
            current   = static_cast<node_type>(current->left_node);
          }
          else
          {
            REX_ASSERT_X(!compare(x, extract_key(current->value)), "compare function doesn't give the same result when swapping the arguments");
            current = static_cast<node_type*>(current->right_node);
          }
        }

        if((range_end != &m_anchor) && !compare(x, extract_key(range_end->value)))
        {
          return iterator(range_end);
        }

        return iterator(static_cast<node_type*>(&m_anchor));
      }
      // finds an element with a key that's compares equivalent to x
      template <typename K>
      const_iterator find(const K& x) const
      {
        return const_iterator(const_cast<this_type>(this)->find(x));
      }

      // checks if there is an element with key equivalent to key
      bool contains(const Key& key) const
      {
        return find(key) != cend();
      }
      // checks if there is an element with key that compares equivalent to the value x
      template <typename K>
      bool contains(const K& x) const
      {
        return find(x) != cend();
      }

      // returns an iterator pointing to the first element that is not less than key
      iterator lower_bound(const Key& key)
      {
        return lower_bound<Key>(key);
      }
      // returns an iterator pointing to the first element that is not less than key
      const_iterator lower_bound(const Key& key) const
      {
        return const_iterator < const_cast<this_type*>(this)->lower_bound(key);
      }
      // returns an iterator pointing to the first element that is not less than the value x
      template <typename K>
      iterator lower_bound(const K& x)
      {
        extract_key extract_key;

        node_type* current   = static_cast<node_type*>(m_anchor.parent_node);
        node_type* range_end = static_cast<node_type*>(&m_anchor);

        while(current)
        {
          if(!compare(extract_key(current->value), x))
          {
            range_end = current;
            current   = static_cast<node_type*>(current->left_node);
          }
          else
          {
            REX_ASSERT_X(!compare(x, extract_key(current->value)), "compare function doesn't give the same result when swapping the arguments");
            current = static_cast<node_type*>(current->right_node);
          }
        }
      }
      // returns an iterator pointing to the first element that is not less than the value x
      template <typename K>
      const_iterator lower_bound(const K& x) const
      {
        return const_iterator < const_cast<this_type*>(this)->lower_bound(x);
      }

      // returns an iterator pointing to the first element that is greater than key
      iterator upper_bound(const Key& key)
      {
        return upper_bound<Key>(key);
      }
      // returns an iterator pointing to the first element that is greater than key
      const_iterator upper_bound(const Key& key) const
      {
        return const_iterator(const_cast<this_type*>(this)->upper_bound(key));
      }
      // returns an iterator pointing to the first element that is greater than the value x
      template <typename K>
      iterator upper_bound(const K& x)
      {
        extract_key extract_key;

        node_type* current   = static_cast<node_type*>(m_anchor.parent_node);
        node_type* range_end = static_cast<node_type*>(&m_anchor);

        while(current)
        {
          if(compare(x, extract_key, current->value))
          {
            REX_ASSERT_X(!compare(extract_key(current->value, x)));
            range_end = current;
            current   = static_cast<node_type*>(current->left_node);
          }
          else
          {
            current = static_cast<node_type*>(current->right_node);
          }
        }

        return iterator(range_end);
      }
      // returns an iterator pointing to the first element that is greater than the value x
      template <typename K>
      const_iterator upper_bound(const K& x) const
      {
        return const_iterator(const_cast<this_type*>(this)->upper_bound(x));
      }

      // returns the comparison function object
      key_compare key_comp() const
      {
        return base_type::get_compare();
      }

    private:
      void reset()
      {
        m_anchor.right_node             = &m_anchor;
        m_anchor.left_node              = &m_anchor;
        m_anchor.parent_node            = nullptr;
        m_anchor.color                  = internal::RedBlackTreeColor::Red;
        m_cp_size_and_allocator.first() = 0;
      }

      void allocate_node()
      {
        node_type* node = static_cast<node_type*>(get_allocator().allocate(sizeof(node_type)));
        return node;
      }
      void free_node(node_type* node)
      {
        get_allocator().destroy(node);
        get_allocator().deallocate(node);
      }

      node_type* create_node_from_key(const key_type& key)
      {
        node_type* node = allocate_node();
        new(rsl::addressof(node->value)) value_type(pair_first_construct, key);

#if REX_DEBUG
        node->right_node  = nullptr;
        node->left_node   = nullptr;
        node->parent_node = nullptr;
        node->color       = internal::RedBlackTreeColor::Black;
#endif

        return node;
      }
      void create_node(const value_type& value)
      {
        node_type* node = allocate_node();

        new(rsl::addressof(node->value)) value_type(value);

#if REX_DEBUG
        node->right_node  = nullptr;
        node->left_node   = nullptr;
        node->parent_node = nullptr;
        node->color       = internal::RedBlackTreeColor::Black;
#endif

        return node;
      }

      void create_node(value_type&& value)
      {
        node_type* node = allocate_node();
        new(rsl::addressof(node->value)) value_type(rsl::move(value));

#if REX_DEBUG
        node->right_node  = nullptr;
        node->left_node   = nullptr;
        node->parent_node = nullptr;
        node->color       = internal::RedBlackTreeColor::Black;
#endif

        return node;
      }

      template <typename... Args>
      void create_node(Args&&... args)
      {
        node_type* node = allocate_node();

        new(rsl::addressof(node->value)) value_type(rsl::forward<Args>(args)...);

#if REX_DEBUG
        node->right_node  = nullptr;
        node->left_node   = nullptr;
        node->parent_node = nullptr;
        node->color       = internal::RedBlackTreeColor::Black;
#endif

        return node;
      }

      void create_node(const node_type* nodeSource, node_type* nodeParent)
      {
        node_type* node = create_node(nodeSource->value);

        node->right_node  = nullptr;
        node->left_node   = nullptr;
        node->parent_node = nodeParent;
        node->color       = nodeSource->color;

        return node;
      }

      node_type* copy_subtree(const node_type* nodeSource, node_type* nodeDest)
      {
        node_type* new_node_root = create_node(nodeSource, nodeDest);

        if(nodeSource->right_node)
        {
          new_node_root->right_node = copy_subtree(static_cast<const node_type*>(nodeSource->right_node, new_node_root));

          node_type* new_left_node;

          for(nodeSource = static_cast<node_type*>(nodeSource->left_node), nodeDest = new_node_root; nodeSource; nodeSource = static_cast<node_type*>(nodeSource->left_node, nodeDest = new_left_node))
          {
            new_left_node = create_node(nodeSource, nodeDest);

            nodeDest->left_node = new_left_node;

            if(nodeSource->right_node)
              new_left_node->right_node = copy_subtree(static_cast<const node_type*>(nodeSource->right_node, new_left_node));
          }
        }

        return new_node_root;
      }

      void do_nuke_subtree(node_type* node)
      {
        while(node)
        {
          nuke_subtree(static_cast<node_type*>(node->right_node));

          node_type* left_node = static_cast<node_type*>(node->left_node);
          free_node(node);
          node = left_node;
        }
      }

      template <typename... Args>
      insert_result<iterator> insert_value(true_type, Args&&... args)
      {
        node_type* new_node = create_node(rsl::forward<Args>(args)...);
        const key_type& key = extract_key {}(new_node->value);

        bool can_insert;
        node_type* position = get_key_insertion_position_unique_keys(key, Out<bool>(can_insert));

        if(can_insert)
        {
          iterator it_result = insert_value(position, false, key, new_node);
          return insert_result(it_result, true);
        }

        free_node(new_node);
        return insert_result(iterator(position), false);
      }
      template <typename... Args>
      iterator insert_value(false_type, Args&&... args)
      {
        node_type* new_node = create_node(rsl::forward<Args>(args)...);
        const key_type& key = extract_key {}(new_node->value);

        node_type* position = get_key_insertion_position_no_unique_keys(key);

        return insert_value(position, false, key, new_node);
      }

      template <typename... Args>
      iterator insert_value(node_type* nodeParent, bool forceToLeft, const key_type& key, Args&&... args)
      {
        node_type* const new_node = create_node(rsl::forward<Args>(args)...);

        return insert_value(nodeParent, forceToLeft, key, new_node);
      }

      iterator insert_value(node_type* nodeParent, bool forceToLeft, const key_type& key, node_type* newNode)
      {
        REX_ASSERT_X(newNode != nullptr, "node to insert to the tree must not be null!");

        internal::RedBlackTreeSide side;
        extract_key extract_key;

        if(forceToLeft || (nodeParent == &m_anchor) || compare(key, extract_key(nodeParent->value)))
        {
          side = internal::RedBlackTreeSide::Left;
        }
        else
        {
          side = internal::RedBlackTreeSide::Right;
        }

        RedBlackTreeInsert(newNode, nodeParent, &m_anchor, side);
        m_cp_size_and_allocator.first()++;

        return iterator(newNode);
      }

      insert_result<iterator> insert_key(true_type, const key_type& key)
      {
        bool can_insert;
        node_type* position = get_key_insertion_position_unique_keys(can_insert, key);

        if(can_insert)
        {
          const iterator it_result = insert_key(position, false, key);
          return insert_result(it_result, true);
        }

        return insert_result(iterator(position), false);
      }

      iterator insert_key(false_type, const key_type& key)
      {
        node_type* position = get_insertion_position_no_unique_keys(key);

        return insert_key(position, false, key);
      }

      node_type* get_key_insertion_position_unique_keys(const key_type& key, Out<bool> canInsert)
      {
        extract_key extract_key;

        node_type* current     = static_cast<node_type*>(m_anchor.parent_node);
        node_type* lower_bound = static_cast<node_type*>(&m_anchor);
        node_type* parent;

        bool value_less_than_node = true;

        while(current)
        {
          value_less_than_node = compare(key, extract_key(current->value));
          lower_bound          = current;

          if(value_less_than_node)
          {
            REX_ASSERT_X(!compare(extract_key(current->value), key), "compare function doesn't give the same result when swapping the arguments");
            current = static_cast<node_type*>(current->left_node);
          }
          else
          {
            current = static_cast<node_type*>(current->right_node);
          }
        }

        parent = lower_bound;

        if(value_less_than_node)
        {
          if(lower_bound != static_cast<node_type*>(m_anchor.left_node))
          {
            lower_bound = static_cast<node_type*>(RedBlackTreeDecrement(lower_bound));
          }
          else
          {
            canInsert = true;
            return lower_bound;
          }
        }

        if(compare(extract_key(lower_bound->value), key))
        {
          REX_ASSERT_X(!compare(key, extract_key(lower_bound->value)), "compare function doesn't give the same result when swapping the arguments");
          canInsert = true;
          return parent;
        }

        // the item already exists, so return false
        canInsert = false;
        return lower_bound;
      }

      node_type* get_key_insertion_position_no_unique_keys(const key_type& key)
      {
        node_type* current   = static_cast<node_type*>(m_anchor.parent_node);
        node_type* range_end = static_cast<node_type*>(&m_anchor);
        extract_key extract_key;

        while(current)
        {
          range_end = current;

          if(compare(key, extract_key(current->value)))
          {
            REX_ASSERT_X(!compare(extract_key(current->value), key), "compare function doesn't give the same result when swapping the arguments");
            current = static_cast<node_type*>(current->left_node);
          }
          else
          {
            current = static_cast<node_type*>(current->right_node);
          }
        }

        return range_end;
      }

      node_type* get_key_insertion_position_unique_keys_hint(const_iterator position, const key_type& key, Out<bool> forceToLeft)
      {
        extract_key extract_key;

        if((position.node != m_anchor.right_node) && (position.node != &m_anchor))
        {
          iterator it_next = position.next;
          ++it_next;

          const bool position_less_than_value = compare(extract_key(position.node->value), key);

          if(position_less_than_value)
          {
            REX_ASSERT_X(!compare(key, extract_key(position.node->value)), "compare function doesn't give the same result when swapping the arguments");

            const bool value_less_than_next = compare(key, extract_key(it_next.node->value));

            if(value_less_than_next)
            {
              if(position.node->right_node)
              {
                forceToLeft = true;
                return it_next.node;
              }

              forceToLeft = false;
              return position.node;
            }
          }

          forceToLeft = false;
          return nullptr;
        }

        if(size() && compare(extract_key(static_cast<node_type*>(m_anchor.right_node)->value), key))
        {
          REX_ASSERT_X(!compare(key, extract_key(static_cast<node_type*>(m_anchor.right_node)->value)), "compare function doesn't give the same result when swapping the arguments");
          forceToLeft = false;
          return static_cast<node_type*>(m_anchor.right_node);
        }

        forceToLeft = false;
        return nullptr;
      }

      node_type* get_key_insertion_position_no_unique_keys_hint(const_iterator position, const key_type& key, Out<bool> force_to_left)
      {
        extract_key extract_key;

        if((position.node != m_anchor.right_node) && (position.node != &m_anchor))
        {
          iterator it_next(position.node);
          ++it_next;

          if(!compare(key, extract_key(position.node->value)) && (!compare(extract_key(it_next.node->value), key)))
          {
            if(position.node->right_node)
            {
              force_to_left = true;
              return it_next.node;
            }

            force_to_left = false;
            return position.node;
          }

          force_to_left = false;
          return nullptr;
        }

        if(size() && !compare(key, extract_key(static_cast<node_type*>(m_anchor.right_node)->value)))
        {
          force_to_left = false;
          return static_cast<node_type*>(m_anchor.right_node);
        }

        force_to_left = false;
        return nullptr;
      }

      template <typename... Args>
      iterator insert_value_hint(true_type, const_iterator position, Args&&... args)
      {
        node_type* new_node = create_node(rsl::forward<Args>(args)...);
        const key_type& key(extract_key {}(new_node->value));

        bool force_to_left;
        node_type* position = get_key_insertion_position_unique_keys_hint(position, key, Out<bool>(force_to_left));

        if(!position)
        {
          bool can_insert;
          position = get_key_insertion_position_unique_keys(can_insert, key);

          if(!can_insert)
          {
            free_node(new_node);
            return iterator(position);
          }

          force_to_left = false;
        }

        return insert_value(position, force_to_left, key, new_node);
      }

      template <typename... Args>
      iterator insert_value_hint(false_type, const_iterator position, Args&&... args)
      {
        node_type* new_node = create_node(rsl::forward<Args>(args)...);
        const key_type& key(extract_key {}(new_node->value));

        bool force_to_left;
        node_type* position = get_key_insertion_position_no_unique_keys_hint(position, force_to_left, key);

        if(!position)
        {
          position      = get_key_insertion_position_no_unique_keys(key);
          force_to_left = false;
        }

        return insert_value(position, force_to_left, key, new_node);
      }

      iterator insert_value_hint(true_type, const_iterator position, value_type&& value)
      {
        extract_key extract_key;
        key_type key(extract_key(value));
        bool force_to_left;
        node_type* position = get_key_insertion_position_unique_keys_hint(position, force_to_left, key);

        if(position)
        {
          return insert_value(position, force_to_left, key, rsl::move(value));
        }
        else
        {
          return insert_value(has_unique_keys_type(), rsl::move(value)).inserted_element;
        }
      }

      iterator insert_value_hint(false_type, const_iterator position, value_type&& value)
      {
        extract_key extract_key;
        key_type key(extract_key(value));
        bool force_to_left;
        node_type* position = get_key_insertion_position_no_unique_keys_hint(position, force_to_left, key);

        if(position)
        {
          return insert_value(position, force_to_left, key, rsl::move(value));
        }
        else
        {
          return insert_value(has_unique_keys_type(), rsl::move(value));
        }
      }

      iterator insert_key(true_type, const_iterator position, const key_type& key)
      {
        bool force_to_left;
        node_type* position = get_key_insertion_position_unique_keys_hint(position, force_to_left, key);

        if(position)
        {
          return insert_key(position, force_to_left, key);
        }
        else
        {
          return insert_key(has_unique_keys_type(), key).inserted_element;
        }
      }

      iterator insert_key(false_type, const_iterator position, const key_type& key)
      {
        bool force_to_left;
        node_type* position = get_key_insertion_position_no_unique_keys_hint(position, force_to_left, key);

        if(position)
        {
          return insert_key(position, force_to_left, key);
        }
        else
        {
          return insert_key(has_unique_keys_type(), key);
        }
      }

      iterator insert_key(node_type* nodeParent, bool force_to_left, const key_type& key)
      {
        internal::RedBlackTreeSide side;
        extract_key extract_key;

        if(force_to_left || (nodeParent == &m_anchor) || compare(key, extract_key(nodeParent->value)))
        {
          side = internal::RedBlackTreeSide::Left;
        }
        else
        {
          side = internal::RedBlackTreeSide::Right;
        }

        node_type* new_node = create_node_from_key(key);
        RedBlackTreeInsert(new_node, nodeParent, &m_anchor, side);
        m_cp_size_and_allocator.first()++;

        return iterator(new_node);
      }

    private:
      // this node acts as end() and its m_left points to begin(), and m_right points to rbegin() (the last node on the right)
      internal::RedBlackTreeNodeBase m_anchor;
      // size: stores the count of nodes in the tree (not counting the anchor node)
      compressed_pair<size_type, allocator_type> m_cp_size_and_allocator;
    };

    // checks if the contents of lhs and rhs are equal
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    bool operator==(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      return (lhs.size() == rhs.size()) && rsl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    // checks if the contents of lhs and rhs are not equal
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    bool operator!=(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      return !(lhs == rhs);
    }
    // compares the contents of lhs and rhs lexicographically
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    bool operator<(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      return rsl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
    // compares the contents of lhs and rhs lexicographically
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    bool operator<=(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      return !(rhs < lhs);
    }
    // compares the contents of lhs and rhs lexicographically
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    bool operator>(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      return rhs < lhs;
    }
    // compares the contents of lhs and rhs lexicographically
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    bool operator>=(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      return !(lhs < rhs);
    }

    // specializes the swap algorithm for map
    template <typename K, typename V, typename A, typename C, typename E, bool bM, bool bU>
    void swap(const RedBlackTree<K, V, A, C, E, bM, bU>& lhs, const RedBlackTree<K, V, A, C, E, bM, bU>& rhs)
    {
      lhs.swap(rhs);
    }

  } // namespace v1
} // namespace rsl
