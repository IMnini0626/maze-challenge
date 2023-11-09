#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity()
{
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{

}



void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Entity* scenes)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;



    // ––––– MOVE WITH ACCELERATION ––––– //
    if (m_entity_type == PLAYER) {
        m_velocity += m_acceleration * delta_time;

        m_position.y += m_velocity.y * delta_time;
        check_collision_y(collidable_entities, collidable_entity_count, scenes);

        m_position.x += m_velocity.x * delta_time;
        check_collision_x(collidable_entities, collidable_entity_count, scenes);
    }
    
    

    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    
    if (m_entity_type == SCENE) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
    }
    
    // FLIPING //
    if (m_velocity.x > 0 && m_entity_type == PLAYER) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count, Entity* scenes)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            // Check if collision with POINT
            if (collidable_entity->get_entity_type() == POINT)
            {
                // Mission Accomplished
                std::cout << "Mission Accomplished" << std::endl;
                scenes[0].activate();
                deactivate();
            }
            else {
                // Mission Failed
                std::cout << "Mission FAILED" << std::endl;
                scenes[1].activate();
                deactivate();
            }
            
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count, Entity* scenes)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            // Check if collision with POINT
            if (collidable_entity->get_entity_type() == POINT)
            {
                // Mission Accomplished
                std::cout << "Mission Accomplished" << std::endl;
                scenes[0].activate();
                deactivate();
//                scenes[0].set_scalewidth(6.5f);
//                scenes[0].set_scaleheight(10.0f);
            }
            else {
                // Mission Failed
                std::cout << "Mission FAILED" << std::endl;
                scenes[1].activate();
                deactivate();
//                scenes[1].set_scalewidth(6.5f);
//                scenes[1].set_scaleheight(10.0f);
            }
            
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (!m_is_active) { return; }
    

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

