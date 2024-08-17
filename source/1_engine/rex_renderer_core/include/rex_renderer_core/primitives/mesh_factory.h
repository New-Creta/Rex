#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_std/bonus/utility/casting.h"
#include "rex_std/vector.h"

#include <glm/glm.hpp>

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      // calculate the mid point of a vertex.
      // all elements of the vertex are halved, position is the middle point between the 2
      VertexPosNormTex mid_point(const VertexPosNormTex& v0, const VertexPosNormTex& v1);

      // A mesh data object is simply a wrapper around a vertex and index buffer.
      template <typename T>
      class MeshData
      {
      public:
        using index_type = T;

        // Create a default mesh data object with no vertices nor indices
        MeshData();
        // Create a mesh data object with given vertices and indices
        MeshData(rsl::vector<VertexPosNormTex>&& vertices, rsl::vector<index_type>&& indices);

        // Overwrite the existing vertices of a MeshData object
        void assign_vertices(const VertexPosNormTex* vertices, u32 numVerticess);
        // Overwrite the existing indices of a MeshData object
        void assign_indices(const index_type* indices, u32 numIndices);

        // Add a single vertex to the internal vertex buffer
        void add_vertex(const VertexPosNormTex& v);
        // Add a single index to the internal index buffer
        void add_index(index_type i);

        // Add a single vertex inplace to the internal vertex buffer
        void emplace_vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col);
        // Add a single index inplace to the internal index buffer
        void emplace_index(index_type i);

        // Insert a single vertex into the internal vertex buffer at a given position
        void insert_vertex(s32 idx, const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col);
        // Insert a single index into the internal index buffer at a given position
        void insert_index(s32 idx, index_type i);

        // Reserve a given amount of vertices into the internal vertex buffer.
        // This does a simple allocation to prepare for future insertion.
        // The size of the vertex buffer remains 0
        void reserve_vertices(s32 num);
        // Reserve a given amount of indices into the internal index buffer.
        // This does a simple allocation to prepare for future insertion.
        // The size of the index buffer remains 0
        void reserve_indices(s32 num);

        // Resizes the internal vertex buffer, either extending or shrinking what's already there.
        // On growth, the new added vertices are default initialized
        void resize_vertices(s32 num);
        // Resizes the internal index buffer, either extending or shrinking what's already there.
        // On growth, the new added indices are default initialized
        void resize_indices(s32 num);

        // Return the vertex buffer
        rsl::vector<VertexPosNormTex>& vertices();
        const rsl::vector<VertexPosNormTex>& vertices() const;
        // Return the index buffer
        rsl::vector<index_type>& indices();
        const rsl::vector<index_type>& indices() const;

        // Return the size of a single vertex
        s32 vertex_size() const;

      private:
        rsl::vector<VertexPosNormTex> m_vertices;
        rsl::vector<index_type> m_indices;
      };

      using MeshData32 = MeshData<u32>;
      using MeshData16 = MeshData<u16>;

      //-----------------------------------------------------------------------
      template <typename T>
      MeshData<T>::MeshData() = default;

      //-----------------------------------------------------------------------
      template <typename T>
      MeshData<T>::MeshData(rsl::vector<VertexPosNormTex>&& vertices, rsl::vector<MeshData<T>::index_type>&& indices)
        : m_vertices(rsl::move(vertices))
        , m_indices(rsl::move(indices))
      {
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::assign_vertices(const VertexPosNormTex* vertices, u32 numVertices)
      {
        m_vertices.assign(&vertices[0], &vertices[numVertices]);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::assign_indices(const MeshData<T>::index_type* indices, u32 numIndices)
      {
        m_indices.assign(&indices[0], &indices[numIndices]);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::add_vertex(const VertexPosNormTex& v)
      {
        m_vertices.push_back(v);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::add_index(MeshData<T>::index_type i)
      {
        m_indices.push_back(i);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::emplace_vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col)
      {
        m_vertices.emplace_back(pos, norm, col);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::emplace_index(MeshData<T>::index_type i)
      {
        m_indices.emplace_back(i);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::insert_vertex(s32 idx, const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col)
      {
        auto it = m_vertices.begin();
        rsl::advance(it, idx);
        m_vertices.insert(it, VertexPosNormTex{ pos, norm, col });
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::insert_index(s32 idx, index_type i)
      {
        auto it = m_indices.begin();
        rsl::advance(it, idx);
        m_indices.insert(it, i);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::reserve_vertices(s32 num)
      {
        m_vertices.reserve(num);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::reserve_indices(s32 num)
      {
        m_indices.reserve(num);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::resize_vertices(s32 num)
      {
        m_vertices.resize(num);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      void MeshData<T>::resize_indices(s32 num)
      {
        m_indices.resize(num);
      }

      //-----------------------------------------------------------------------
      template <typename T>
      rsl::vector<VertexPosNormTex>& MeshData<T>::vertices()
      {
        return m_vertices;
      }
      //-----------------------------------------------------------------------
      template <typename T>
      const rsl::vector<VertexPosNormTex>& MeshData<T>::vertices() const
      {
        return m_vertices;
      }

      //-----------------------------------------------------------------------
      template <typename T>
      rsl::vector<typename MeshData<T>::index_type>& MeshData<T>::indices()
      {
        return m_indices;
      }
      //-----------------------------------------------------------------------
      template <typename T>
      const rsl::vector<typename MeshData<T>::index_type>& MeshData<T>::indices() const
      {
        return m_indices;
      }

      //-----------------------------------------------------------------------
      template <typename T>
      s32 MeshData<T>::vertex_size() const
      {
        using vertex_type = typename decltype(m_vertices)::value_type;
        return sizeof(vertex_type);
      }

      //-----------------------------------------------------------------------
      /**
       * @brief Subdivides each triangle in the mesh into four smaller triangles.
       *
       * @tparam T The type of data used for indices.
       * @param meshData The mesh data containing vertices and indices to subdivide.
       */
      template <typename T>
      MeshData<T> subdivide(const MeshData<T>& meshData)
      {
        // Save a copy of the input geometry.
        MeshData<T> res;

        // Clear existing vertices and indices.
        res.reserve_vertices(meshData.vertices().size());
        res.resize_indices(meshData.indices().size());

        //       v1
        //       *
        //      / \
        //     /   \
        //  m0*-----*m1
        //   / \   / \
        //  /   \ /   \
        // *-----*-----*
        // v0    m2     v2

        // Loop through each triangle in the input mesh.
        const s32 num_tris = meshData.indices().size() / 3;
        for (s32 i = 0; i < num_tris; ++i)
        {
          // Retrieve vertices of the current triangle.
          const VertexPosNormTex& v0 = meshData.vertices()[meshData.indices()[i * 3 + 0]];
          const VertexPosNormTex& v1 = meshData.vertices()[meshData.indices()[i * 3 + 1]];
          const VertexPosNormTex& v2 = meshData.vertices()[meshData.indices()[i * 3 + 2]];

          //
          // Generate the midpoints.
          //

          // Calculate midpoints of triangle edges.
          const VertexPosNormTex m0 = mid_point(v0, v1);
          const VertexPosNormTex m1 = mid_point(v1, v2);
          const VertexPosNormTex m2 = mid_point(v0, v2);

          //
          // Add new geometry.
          //

          // Add original vertices and midpoints to meshData.
          res.add_vertex(v0); // 0
          res.add_vertex(v1); // 1
          res.add_vertex(v2); // 2
          res.add_vertex(m0); // 3
          res.add_vertex(m1); // 4
          res.add_vertex(m2); // 5

          // Define indices for the new smaller triangles.
          res.add_index((T)i * 6 + 0);
          res.add_index((T)i * 6 + 3);
          res.add_index((T)i * 6 + 5);

          res.add_index((T)i * 6 + 3);
          res.add_index((T)i * 6 + 4);
          res.add_index((T)i * 6 + 5);

          res.add_index((T)i * 6 + 5);
          res.add_index((T)i * 6 + 4);
          res.add_index((T)i * 6 + 2);

          res.add_index((T)i * 6 + 3);
          res.add_index((T)i * 6 + 1);
          res.add_index((T)i * 6 + 4);
        }

        return res;
      }
    } // namespace mesh_factory
  }
} // namespace rex