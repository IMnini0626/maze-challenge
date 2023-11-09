enum EntityType { PLATFORM, PLAYER, ENEMY, POINT, SCENE };

class Entity
{
private:
    bool m_is_active = true;

    // ––––– PHYSICS (GRAVITY) ––––– //
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_model_matrix;

    // ————— ENEMY AI ————— //
    EntityType m_entity_type;

    float m_width = 1;
    float m_height = 1;
    float m_scalewidth = 1;
    float m_scaleheight = 1;


public:
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int    LEFT = 0,
                        RIGHT = 1,
                        UP = 2,
                        DOWN = 3;

    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;

    GLuint    m_texture_id;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Entity* scenes);
    void render(ShaderProgram* program);

    bool const check_collision(Entity* other) const;
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count, Entity* scenes);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count, Entity* scenes);

    void move_left()    { m_acceleration.x = -0.5f; };
    void move_right()   { m_acceleration.x = 0.5f; };
    void move_up()      { m_acceleration.y = 0.5f; };
    void move_down()    { m_acceleration.y = -0.5f; };
    void stay_still_x() { m_acceleration.x = 0.0f; };
    void stay_still_y(float gravity) { m_acceleration.y = gravity; };

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;     };
    glm::vec3  const get_position()       const { return m_position;        };
    glm::vec3  const get_movement()       const { return m_movement;        };
    glm::vec3  const get_velocity()       const { return m_velocity;        };
    glm::vec3  const get_acceleration()   const { return m_acceleration;    };
//    float      const get_jumping_power()  const { return m_jumping_power;   };
    float      const get_speed()          const { return m_speed;           };
    int        const get_width()          const { return m_width;           };
    int        const get_height()         const { return m_height;          };

    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;      };
    void const set_position(glm::vec3 new_position)         { m_position = new_position;            };
    void const set_movement(glm::vec3 new_movement)         { m_movement = new_movement;            };
    void const set_velocity(glm::vec3 new_velocity)         { m_velocity = new_velocity;            };
    void const set_speed(float new_speed)                   { m_speed = new_speed;                  };
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration;    };
    void const set_width(float new_width)                   { m_width = new_width;                  };
    void const set_height(float new_height)                 { m_height = new_height;                };
    
    
    // FOR TEST
//    void const aoaojiao() const;
};
