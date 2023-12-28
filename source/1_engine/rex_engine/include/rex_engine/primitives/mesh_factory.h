#pragma once

#include "rex_engine/types.h"

#include "rex_std/vector.h"
#include "rex_std_extra/utility/casting.h"

#include <glm/glm.hpp>

namespace rex
{
    namespace mesh_factory
    {
        struct Vertex
        {
            Vertex();
            Vertex(const glm::vec3& inPos, const glm::vec3& inNorm, const glm::vec3& inTang, const glm::vec2& inUV);
            Vertex(f32 xp, f32 yp, f32 zp, f32 xn, f32 yn, f32 zn, f32 xt, f32 yt, f32 zt, f32 xu, f32 yu);

            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 uv;
        };

        Vertex mid_point(const Vertex& v0, const Vertex& v1);

        template<typename T>
        class MeshData
        {
        public:
            using index_type = T;

            MeshData();
            MeshData(rsl::vector<Vertex>&& vertices, rsl::vector<index_type>&& indices);

            void assign_vertices(const Vertex* vertices, u32 numVerticess);
            void assign_indices(const index_type* indices, u32 numIndices);

            void add_vertex(const Vertex& v);
            void add_index(index_type i);

            void emplace_vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& tang, const glm::vec2& uv);
            void emplace_index(index_type i);

            void insert_vertex(s32 idx, const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& tang, const glm::vec2& uv);
            void insert_index(s32 idx, index_type i);

            void reserve_vertices(u32 num);
            void reserve_indices(u32 num);

            void resize_vertices(u32 num);
            void resize_indices(u32 num);

            const rsl::vector<Vertex>& vertices() const;
            const rsl::vector<index_type>& indices() const;

        private:
            rsl::vector<Vertex> m_vertices;
            rsl::vector<index_type> m_indices;
        };

        using MeshData32 = MeshData<u32>;
        using MeshData16 = MeshData<u16>;

        //-----------------------------------------------------------------------
        template<typename T>
        MeshData<T>::MeshData() = default;

        //-----------------------------------------------------------------------
        template<typename T>
        MeshData<T>::MeshData(rsl::vector<Vertex>&& vertices, rsl::vector<MeshData<T>::index_type>&& indices)
            :m_vertices(rsl::move(vertices))
            ,m_indices(rsl::move(indices))
        {}

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::assign_vertices(const Vertex* vertices, u32 numVertices)
        {
            //m_vertices.assign(&vertices[0], &vertices[numVertices]);
            for (u32 i = 0; i < numVertices; ++i)
                m_vertices.push_back(vertices[i]);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::assign_indices(const MeshData<T>::index_type* indices, u32 numIndices)
        {
            //m_indices.assign(&indices[0], &indices[numIndices]);
            for (u32 i = 0; i < numIndices; ++i)
                m_indices.push_back(indices[i]);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::add_vertex(const Vertex& v)
        {
            m_vertices.push_back(v);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::add_index(MeshData<T>::index_type i)
        {
            m_indices.push_back(i);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::emplace_vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& tang, const glm::vec2& uv)
        {
            m_vertices.emplace_back(pos, norm, tang, uv);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::emplace_index(MeshData<T>::index_type i)
        {
            m_indices.emplace_back(i);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::insert_vertex(s32 idx, const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& tang, const glm::vec2& uv)
        {
            auto it = m_vertices.begin();
            rsl::advance(it, idx);
            m_vertices.insert(it, Vertex{ pos, norm, tang, uv });
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::insert_index(s32 idx, index_type i)
        {
            auto it = m_indices.begin();
            rsl::advance(it, idx);
            m_indices.insert(it, i);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::reserve_vertices(u32 num)
        {
            m_vertices.reserve(num);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::reserve_indices(u32 num)
        {
            m_indices.reserve(num);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::resize_vertices(u32 num)
        {
            m_vertices.resize(num);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::resize_indices(u32 num)
        {
            m_indices.resize(num);
        }

        //-----------------------------------------------------------------------
        template<typename T>
        const rsl::vector<Vertex>& MeshData<T>::vertices() const
        {
            return m_vertices;
        }

        //-----------------------------------------------------------------------
        template<typename T>
        const rsl::vector<typename MeshData<T>::index_type>& MeshData<T>::indices() const
        {
            return m_indices;
        }

        //-----------------------------------------------------------------------
        /**
         * @brief Subdivides each triangle in the mesh into four smaller triangles.
         *
         * @tparam T The type of data used for indices.
         * @param meshData The mesh data containing vertices and indices to subdivide.
         */
        template<typename T>
        void subdivide(MeshData<T>& meshData)
        {
            // Save a copy of the input geometry.
            MeshData<T> input_copy = meshData;

            // Clear existing vertices and indices.
            meshData.resize_vertices(0);
            meshData.resize_indices(0);

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
            s32 num_tris = input_copy.indices().size() / 3;
            for (s32 i = 0; i < num_tris; ++i)
            {
                // Retrieve vertices of the current triangle.
                const Vertex& v0 = input_copy.vertices()[input_copy.indices()[i * 3 + 0]];
                const Vertex& v1 = input_copy.vertices()[input_copy.indices()[i * 3 + 1]];
                const Vertex& v2 = input_copy.vertices()[input_copy.indices()[i * 3 + 2]];

                //
                // Generate the midpoints.
                //

                // Calculate midpoints of triangle edges.
                Vertex m0 = mid_point(v0, v1);
                Vertex m1 = mid_point(v1, v2);
                Vertex m2 = mid_point(v0, v2);

                //
                // Add new geometry.
                //

                // Add original vertices and midpoints to meshData.
                meshData.add_vertex(v0); // 0
                meshData.add_vertex(v1); // 1
                meshData.add_vertex(v2); // 2
                meshData.add_vertex(m0); // 3
                meshData.add_vertex(m1); // 4
                meshData.add_vertex(m2); // 5

                // Define indices for the new smaller triangles.
                meshData.add_index((T)i * 6 + 0);
                meshData.add_index((T)i * 6 + 3);
                meshData.add_index((T)i * 6 + 5);

                meshData.add_index((T)i * 6 + 3);
                meshData.add_index((T)i * 6 + 4);
                meshData.add_index((T)i * 6 + 5);

                meshData.add_index((T)i * 6 + 5);
                meshData.add_index((T)i * 6 + 4);
                meshData.add_index((T)i * 6 + 2);

                meshData.add_index((T)i * 6 + 3);
                meshData.add_index((T)i * 6 + 1);
                meshData.add_index((T)i * 6 + 4);
            }
        }
    }
}