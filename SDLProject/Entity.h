/**
* Author: Yini Zhang
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#pragma once
#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY, GOAL };
enum AIType     { WALKER1, WALKER2, CHASER };
enum AIState    { WALKING, IDLE, ATTACKING };

class Entity
{
private:
    bool m_is_active = true;

    // ––––– PHYSICS ––––– //
    glm::vec3 m_position;
    glm::vec3 m_velocity;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    
    // ————— ENEMY AI ————— //
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;

    float m_width = 1;
    float m_height = 1;


public:
    glm::mat4 m_model_matrix;
    
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;

    // ————— ANIMATION ————— //
    int* m_animation = NULL;

    int m_animation_frames = 0,
        m_animation_index = 0,
        m_animation_cols = 0,
        m_animation_rows = 0;

    int*    m_animation_indices = m_animation;
    float   m_animation_time = 0.0f;

    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;
    bool m_collided_left_bottom = false;
    bool m_collided_right_bottom = false;
    bool m_collided_enemy = false;
    bool m_collided_goal = false;
    
    // —————— LEVELS ——————
    bool m_is_levelc = false;
    
    // —————— GOALS ——————
    int m_current_goal_count = 0;
    
    bool m_win_state = false;
    bool m_lose_state = false;
    bool m_pass_state = false;
    bool m_cost_life = false;
    
    float m_time_passed = 0;
    int m_enemies_defeated = 0;
    
    // FONT
    int FONTBANK_SIZE = 16;

    GLuint    m_texture_id;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position);
    
    // Overload update for MAP
    void update(float delta_time, Entity *player, Entity *objects, int object_count, Map *map);

    void render(ShaderProgram* program);
    
    bool const check_collision(Entity* other) const;
    // Collision for entity
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    // Overloading collision for map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);

    void move_left()    { m_movement.x = -1.0f; };
    void move_right()   { m_movement.x = 1.0f; };
    void move_up()      { m_movement.y = 1.0f; };
    void move_down()    { m_movement.y = -1.0f; };

    void ai_activate(Entity* player);
    void ai_walk1();
    void ai_walk2();
    void ai_chase(Entity* player);

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;     };
    AIType     const get_ai_type()        const { return m_ai_type;         };
    AIState    const get_ai_state()       const { return m_ai_state;        };
    glm::vec3  const get_position()       const { return m_position;        };
    glm::vec3  const get_movement()       const { return m_movement;        };
    glm::vec3  const get_velocity()       const { return m_velocity;        };
    float      const get_speed()          const { return m_speed;           };
    int        const get_width()          const { return m_width;           };
    int        const get_height()         const { return m_height;          };

    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;      };
    void const set_ai_type(AIType new_ai_type)              { m_ai_type = new_ai_type;              };
    void const set_ai_state(AIState new_state)              { m_ai_state = new_state;               };
    void const set_position(glm::vec3 new_position)         { m_position = new_position;            };
    void const set_movement(glm::vec3 new_movement)         { m_movement = new_movement;            };
    void const set_velocity(glm::vec3 new_velocity)         { m_velocity = new_velocity;            };
    void const set_speed(float new_speed)                   { m_speed = new_speed;                  };
    void const set_width(float new_width)                   { m_width = new_width;                  };
    void const set_height(float new_height)                 { m_height = new_height;                };
};
