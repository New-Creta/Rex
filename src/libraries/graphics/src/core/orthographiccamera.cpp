#include "rex_graphics_pch.h"

#include "core/orthographiccamera.h"

#include <glm/gtc/matrix_transform.hpp>

//-------------------------------------------------------------------------
rex::graphics::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : m_projection_matrix(glm::ortho(left, right, bottom, top))
    , m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f)
{
    recalculateViewMatrix();
}
//-------------------------------------------------------------------------
rex::graphics::OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
    : m_projection_matrix(glm::ortho(left, right, bottom, top, zNear, zFar))
    , m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f)
{
    recalculateViewMatrix();
}

//-------------------------------------------------------------------------
void rex::graphics::OrthographicCamera::setPosition(const glm::vec3& position)
{
    m_position = position;
    recalculateViewMatrix();
}

//-------------------------------------------------------------------------
void rex::graphics::OrthographicCamera::setRotation(float rotation)
{
    m_rotation = rotation;
    recalculateViewMatrix();
}

//-------------------------------------------------------------------------
const glm::mat4& rex::graphics::OrthographicCamera::getViewProjectionMatrix() const
{
    return m_view_projection_matrix;
}

//-------------------------------------------------------------------------
const glm::mat4& rex::graphics::OrthographicCamera::getProjectionMatrix() const
{
    return m_projection_matrix;
}

//-------------------------------------------------------------------------
const glm::mat4& rex::graphics::OrthographicCamera::getViewMatrix() const
{
    return m_view_matrix;
}

//-------------------------------------------------------------------------
const glm::vec3& rex::graphics::OrthographicCamera::getPosition() const
{
    return m_position;
}

//-------------------------------------------------------------------------
float rex::graphics::OrthographicCamera::getRotation() const
{
    return m_rotation;
}

//-------------------------------------------------------------------------
void rex::graphics::OrthographicCamera::recalculateViewMatrix()
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0, 0, 1));

    m_view_matrix = glm::inverse(translate * rotate);
    m_view_projection_matrix = m_projection_matrix * m_view_matrix;
}
