#include "LevelA.h"
#include "Utility.h"

#define LOG(argument) std::cout << argument << '\n'
#define LEVEL1_WIDTH 10
#define LEVEL1_HEIGHT 10
#define PLAYER_SPEED 1.5f
#define ENEMY_SPEED 1.7f

// —————— MAP ——————
unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 9, 29, 9, 29, 0, 0, 0, 9, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 29, 0,
    0, 0, 0, 47, 0, 0, 47, 0, 9, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 29, 0,
    0, 9, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 29, 0, 47, 0, 0, 47, 0, 0, 0,
    0, 9, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 29, 0, 0, 0, 9, 29, 9, 29, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelA::~LevelA()
{
    delete [] m_state.goals;
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
}

void LevelA::initialise() {
    LOG("Intializing LEVEL1");
    GOAL_COUNT = 5;
    ENEMY_COUNT = 2;
    
    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tilemap_packed.png");
    m_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 20, 9);
    
    // ––––– ENEMIES ––––– //
    GLuint sprite_texture_id = Utility::load_texture("assets/tilemap-characters_packed.png");
    m_state.enemies = new Entity[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].m_texture_id = sprite_texture_id;
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_height(0.9f);
        m_state.enemies[i].set_width(0.7f);
        
        m_state.enemies[i].m_animation = new int[2] { 2, 3 };
        
        m_state.enemies[i].m_animation_indices = m_state.enemies[i].m_animation;
        m_state.enemies[i].m_animation_frames = 2;
        m_state.enemies[i].m_animation_index  = 0;
        m_state.enemies[i].m_animation_time   = 0.0f;
        m_state.enemies[i].m_animation_cols   = 9;
        m_state.enemies[i].m_animation_rows   = 3;
        
        m_state.enemies[i].set_speed(1.7f);

        if (i == 0) {
            m_state.enemies[i].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
            m_state.enemies[i].set_position(glm::vec3(0.0f, 0.0f, 0.0f));
            m_state.enemies[i].set_ai_type(WALKER1);
        }
        else if (i == 1) {
            m_state.enemies[i].set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
            m_state.enemies[i].set_position(glm::vec3(9.0f, -9.0f, 0.0f));
            m_state.enemies[i].set_ai_type(WALKER1);
        }
    }
    
    // —————— GOALS —————— //
    m_state.goals = new Entity[GOAL_COUNT];
    for (int i = 0; i < GOAL_COUNT; i++) {
        m_state.goals[i].m_texture_id = Utility::load_texture("assets/tile_0067.png");
        m_state.goals[i].set_entity_type(GOAL);
        m_state.goals[i].set_height(0.8f);
        m_state.goals[i].set_width(0.8f);
        
        m_state.goals[i].set_movement(glm::vec3(0.0f));
        
        if (i == 0) {
            m_state.goals[i].set_position(glm::vec3(4.0f, -4.0f, 0.0f));
        }
        else if (i == 1) {
            m_state.goals[i].set_position(glm::vec3(6.0f, 0.0f, 0.0f));
        }
        else if (i == 2) {
            m_state.goals[i].set_position(glm::vec3(9.0f, -2.0f, 0.0f));
        }
        else if (i == 3) {
            m_state.goals[i].set_position(glm::vec3(6.0f, -9.0f, 0.0f));
        }
        else if (i == 4) {
            m_state.goals[i].set_position(glm::vec3(0.0f, -3.0f, 0.0f));
        }
    }
    
    // ––––– PLAYER ––––– //
    // Existing
    m_state.player = new Entity();
    m_state.player->set_position(glm::vec3(5.0f, -5.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(1.5f);
    m_state.player->m_texture_id = sprite_texture_id;
    
    // Walking
    m_state.player->m_animation = new int[2] { 0, 1 };

    m_state.player->m_animation_indices = m_state.player->m_animation;
    m_state.player->m_animation_frames = 2;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 9;
    m_state.player->m_animation_rows   = 3;
    m_state.player->set_height(0.9f);
    m_state.player->set_width(0.9f);
    m_state.player->set_entity_type(PLAYER);
}

void LevelA::update(float delta_time, int& lives)
{
    m_state.player->update(delta_time, m_state.player, m_state.goals, GOAL_COUNT, m_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map);
    }
    for (int i = 0; i < GOAL_COUNT; i++) {
        m_state.goals[i].update(delta_time, m_state.player, NULL, 0, m_state.map);
    }
    if (m_state.player->m_current_goal_count == GOAL_COUNT) {
        // go to next scene
        LOG("Win LEVEL1");
        m_state.play_next_scene = true;
        m_state.next_scene_id = 1;
    }
    if (m_state.player->m_cost_life) {
        lives -= 1;
        m_state.player->m_cost_life = false;
    }
}

void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for (int i = 0; i < GOAL_COUNT; i++) m_state.goals[i].render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) m_state.enemies[i].render(program);
}
