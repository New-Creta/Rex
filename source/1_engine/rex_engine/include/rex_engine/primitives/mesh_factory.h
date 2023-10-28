#pragma once

#include "rex_engine/types.h"

#include <rex_std/vector.h>

#include <glm/glm.hpp>

namespace rex
{
    namespace mesh_factory
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 uv;
        };

        template<typename T>
        class MeshData
        {
        public:
            using index_type = T;

            MeshData();
            MeshData(rsl::vector<Vertex>&& vertices, rsl::vector<index_type>&& indices);

            void assign_vertices(Vertex* vertices, u32 numVerticess);
            void assign_indices(index_type* indices, u32 numIndices);

            void add_vertex(const Vertex& v);
            void add_index(index_type i);

            void emplace_vertex(glm::vec3&& pos, glm::vec3&& norm, glm::vec3&& tang, glm::vec2 uv);
            void emplace_index(index_type i);

            void resize_vertices(u32 num);
            void resize_indices(u32 num);

            const rsl::vector<Vertex>& vertices() const;
            rsl::vector<Vertex>& vertices();
            const rsl::vector<index_type>& indices() const;
            rsl::vector<index_type>& indices();

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
            :m_vertices(std::move(vertices))
            ,m_indices(std::move(indices))
        {}

        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::assign_vertices(Vertex* vertices, u32 numVertices)
        {
            m_vertices.assign(&vertices[0], &vertices[numVertices]);
        }
        //-----------------------------------------------------------------------
        template<typename T>
        void MeshData<T>::assign_indices(MeshData<T>::index_type* indices, u32 numIndices)
        {
            m_indices.assign(&indices[0], &indices[numIndices]);
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
        void MeshData<T>::emplace_vertex(glm::vec3&& pos, glm::vec3&& norm, glm::vec3&& tang, glm::vec2 uv)
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
        rsl::vector<Vertex>& MeshData<T>::vertices()
        {
            return m_vertices;
        }
        //-----------------------------------------------------------------------
        template<typename T>
        const rsl::vector<MeshData<T>::index_type>& MeshData<T>::indices() const
        {
            return m_indices;
        }
        //-----------------------------------------------------------------------
        template<typename T>
        rsl::vector<MeshData<T>::index_type>& MeshData<T>::indices()
        {
            return m_indices;
        }

        template<typename T>
        Vertex mid_point(const Vertex& v0, const Vertex& v1)
        {
            // Compute the midpoints of all the attributes.  Vectors need to be normalized
            // since linear interpolating can make them not unit length.  
            Vertex v;

            v.position = 0.5f * (v0.position + v1.position);
            v.normal = glm::normalize(0.5f * (v0.normal + v1.normal));
            v.tangent = glm::normalize(0.5f * (v0.tangent + v1.tangent));
            v.uv = 0.5f * (v0.uv + v1.uv);

            return v;
        }

        template<typename T>
        void subdivide(MeshData<T>& meshData)
        {
            // Save a copy of the input geometry.
            MeshData<T> input_copy = meshData;

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

            u32 num_tris = input_copy.indices().size() / 3;
            for (u32 i = 0; i < num_tris; ++i)
            {
                Vertex v0 = input_copy.vertices()[input_copy.indices()[i * 3 + 0]];
                Vertex v1 = input_copy.vertices()[input_copy.indices()[i * 3 + 1]];
                Vertex v2 = input_copy.vertices()[input_copy.indices()[i * 3 + 2]];

                //
                // Generate the midpoints.
                //

                Vertex m0 = mid_point<T>(v0, v1);
                Vertex m1 = mid_point<T>(v1, v2);
                Vertex m2 = mid_point<T>(v0, v2);

                //
                // Add new geometry.
                //

                meshData.add_vertex(v0); // 0
                meshData.add_vertex(v1); // 1
                meshData.add_vertex(v2); // 2
                meshData.add_vertex(m0); // 3
                meshData.add_vertex(m1); // 4
                meshData.add_vertex(m2); // 5

                meshData.add_index(i * 6 + 0);
                meshData.add_index(i * 6 + 3);
                meshData.add_index(i * 6 + 5);

                meshData.add_index(i * 6 + 3);
                meshData.add_index(i * 6 + 4);
                meshData.add_index(i * 6 + 5);

                meshData.add_index(i * 6 + 5);
                meshData.add_index(i * 6 + 4);
                meshData.add_index(i * 6 + 2);

                meshData.add_index(i * 6 + 3);
                meshData.add_index(i * 6 + 1);
                meshData.add_index(i * 6 + 4);
            }
        }
    }
}