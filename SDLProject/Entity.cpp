/**
* Author: Yini Zhang
* Assignment: Maze Challenge
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

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
#include "Map.h"

Entity::Entity()
{
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete[] m_animation;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

// —————— AI —————— //
void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
        case WALKER1:
            ai_walk1();
            break;
        case WALKER2:
            ai_walk2();
            break;
        case CHASER:
            ai_chase(player);
            break;

    default:
        break;
    }
}

void Entity::ai_walk1()
{
    if (m_position.y > 0.0f && m_position.x == 0.0f) {
        m_position.y = 0.0f;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        move_right();
    }
    else if (m_position.x > 9.0f && m_position.y == 0.0f) {
        m_position.x = 9.0f;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        move_down();
    }
    else if (m_position.y < -9.0f && m_position.x == 9.0f) {
        m_position.y = -9.0f;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        move_left();
    }
    else if (m_position.x < 0.0f && m_position.y == -9.0f) {
        m_position.x = 0.0f;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        move_up();
    }
}

void Entity::ai_walk2()
{
    if (m_position.x < 0.0f) {
        m_position.x = 0.0f;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        move_right();
    }
    else if (m_position.x > 9.0f) {
        m_position.x = 9.0f;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        move_left();
    }
}

void Entity::ai_chase(Entity* player) {
    if (m_position.x > player->get_position().x) { m_movement.x = -1.0f; }
    else { m_movement.x = 1.0f; }
    if (m_position.y > player->get_position().y) { m_movement.y = -1.0f; }
    else { m_movement.y = 1.0f; }
}

// —————— UPDATE —————— //
void Entity::update(float delta_time, Entity *player, Entity *objects, int object_count, Map *map)
{
    m_time_passed += delta_time;
    
    if (!m_is_active) return;
    
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) {
        ai_activate(player);
    }
    
    // ––––– ANIMATION ––––– //
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }

    // ––––– MOVEMENT ––––– //
    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y = m_movement.y * m_speed;
    
    m_position.x += m_velocity.x * delta_time;
    
    if (m_entity_type == PLAYER and !m_is_levelc) {
        if (m_position.x < 0.0f) {
            m_position.x = 0.0f;
        }
        if (m_position.x > 9.0f) {
            m_position.x = 9.0f;
        }
        if (m_position.y > 0.0f) {
            m_position.y = 0.0f;
        }
        if (m_position.y < -9.0f) {
            m_position.y = -9.0f;
        }
    }
    
    check_collision_x(objects, object_count);
    check_collision_x(map);

    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    check_collision_y(map);
    
    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    
    // —————— FLIPPING ——————— //
    if (m_velocity.x > 0 && m_entity_type == PLAYER) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
}

// —————— COLLSION —————— //
void const Entity::check_collision_y(Map *map)
{
    if (map == NULL) return ;
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0.2f;
    float penetration_y = 0.2f;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
        if (m_entity_type == PLAYER) { LOG("colliding top"); }
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
        if (m_entity_type == PLAYER) { LOG("colliding top left"); }
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
        if (m_entity_type == PLAYER) { LOG("colliding top right"); }
    }
    
    
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        if (m_entity_type == PLAYER) { LOG("colliding bottom"); }
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        if (m_entity_type == PLAYER) { LOG("colliding bottom left"); }
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        if (m_entity_type == PLAYER) { LOG("colliding bottom right"); }
    }

}

void const Entity::check_collision_x(Map *map)
{
    if (map == NULL) return;

    // Probes for tiles to the left
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 left_top = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 left_bottom = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    // Probes for tiles to the right
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right_top = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 right_bottom = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0.2f;
    float penetration_y = 0.2f;

    // Check the left three points
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
        if (m_entity_type == PLAYER) { LOG("colliding left"); }
        
    }
    else if (map->is_solid(left_top, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
        if (m_entity_type == PLAYER) { LOG("colliding left top"); }
    }
    else if (map->is_solid(left_bottom, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
        if (m_entity_type == PLAYER) { LOG("colliding left bottom"); }
    }

    // Check the right three points
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
        if (m_entity_type == PLAYER) { LOG("colliding right"); }
    }
    else if (map->is_solid(right_top, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
        if (m_entity_type == PLAYER) { LOG("colliding right top"); }
    }
    else if (map->is_solid(right_bottom, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
        if (m_entity_type == PLAYER) { LOG("colliding right bottom"); }
    }
    
}


void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            if (collidable_entity->m_entity_type == GOAL and m_entity_type == PLAYER) {
                collidable_entity->deactivate();
                m_collided_goal = true;
                LOG("goal colliding");
                m_current_goal_count += 1;
            }
            else if (collidable_entity->m_entity_type == PLAYER and m_entity_type == ENEMY) {
                collidable_entity->m_lose_state = true;
                collidable_entity->deactivate(); // FAILING
                m_collided_enemy = true;
                LOG("enemy colliding");
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            if (collidable_entity->m_entity_type == GOAL and m_entity_type == PLAYER) {
                collidable_entity->deactivate();
                m_collided_goal = true;
                LOG("goal colliding");
                m_current_goal_count += 1;
            }
            else if (collidable_entity->m_entity_type == PLAYER and m_entity_type == ENEMY) {
                collidable_entity->m_lose_state = true;
                collidable_entity->deactivate(); // FAILING
                m_collided_enemy = true;
                LOG("enemy colliding");
            }
        }
    }
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (!m_is_active) { return; }
    
    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

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
