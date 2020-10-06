#pragma once

#include <glm/glm.hpp>

namespace rex
{
    namespace graphics
    {
        class OrthographicCamera
        {
        public:
            OrthographicCamera(float left, float right, float bottom, float top);
            OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar);

            void setPosition(const glm::vec3& position);
            void setRotation(float rotation);

            const glm::mat4& getViewProjectionMatrix() const;
            const glm::mat4& getProjectionMatrix() const;
            const glm::mat4& getViewMatrix() const;

            const glm::vec3& getPosition() const;

            float getRotation() const;

        private:
            void recalculateViewMatrix();

            glm::mat4 m_view_projection_matrix;
            glm::mat4 m_projection_matrix;
            glm::mat4 m_view_matrix;

            glm::vec3 m_position;

            float m_rotation;
        };
    }
}