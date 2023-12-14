#include "LevelB.h"
#include "Utility.h"

#define LOG(argument) std::cout << argument << '\n'
#define LEVEL2_WIDTH 10
#define LEVEL2_HEIGHT 10
#define PLAYER_SPEED 1.5f
#define ENEMY_SPEED 1.7f

// —————— MAP ——————
unsigned int LEVEL_2_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 11, 11, 11, 12, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 26, 27, 27, 27, 27, 28, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 11, 11, 11, 12, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 26, 27, 27, 27, 27, 28, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelB::~LevelB()
{
    delete [] m_state.goals;
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
}

void LevelB::initialise() {
    LOG("Intializing LEVEL2");
    GOAL_COUNT = 8;
    ENEMY_COUNT = 3;

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tilemap_desert_packed.png");
    m_state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, LEVEL_2_DATA, map_texture_id, 1.0f, 16, 7);

    // ––––– ENEMIES ––––– //
    GLuint sprite_texture_id = Utility::load_texture("assets/tilemap-characters_packed.png");
    GLuint ophelia_texture_id = Utility::load_texture("assets/ophelia.png");
    m_state.enemies = new Entity[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].set_entity_type(ENEMY);
        

        if (i == 0 or i == 1) {
            m_state.enemies[i].set_height(0.9f);
            m_state.enemies[i].set_width(0.7f);
            m_state.enemies[i].set_speed(1.7f);
            
            m_state.enemies[i].m_texture_id = map_texture_id;
            m_state.enemies[i].m_animation = new int[2] { 90, 91 };
            m_state.enemies[i].m_animation_indices = m_state.enemies[i].m_animation;
            m_state.enemies[i].m_animation_frames = 2;
            m_state.enemies[i].m_animation_index  = 0;
            m_state.enemies[i].m_animation_time   = 0.0f;
            m_state.enemies[i].m_animation_cols   = 16;
            m_state.enemies[i].m_animation_rows   = 7;
            if (i == 0) {
                m_state.enemies[i].set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
                m_state.enemies[i].set_position(glm::vec3(9.0f, -2.0f, 0.0f));
                m_state.enemies[i].set_ai_type(WALKER2);
            }
            else if (i == 1) {
                m_state.enemies[i].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
                m_state.enemies[i].set_position(glm::vec3(0.0f, -6.0f, 0.0f));
                m_state.enemies[i].set_ai_type(WALKER2);
            }
        }
        else if (i == 2) {
            m_state.enemies[i].set_height(0.9f);
            m_state.enemies[i].set_width(0.9f);
            m_state.enemies[i].set_speed(2.0f);
            
            m_state.enemies[i].m_texture_id = ophelia_texture_id;
            m_state.enemies[i].m_animation = new int[4] { 0, 1, 2, 3 };
            m_state.enemies[i].m_animation_indices = m_state.enemies[i].m_animation;
            m_state.enemies[i].m_animation_frames = 4;
            m_state.enemies[i].m_animation_index  = 0;
            m_state.enemies[i].m_animation_time   = 0.0f;
            m_state.enemies[i].m_animation_cols   = 4;
            m_state.enemies[i].m_animation_rows   = 1;
            
            
            m_state.enemies[i].set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
            m_state.enemies[i].set_position(glm::vec3(9.0f, -9.0f, 0.0f));
            m_state.enemies[i].set_ai_type(WALKER1);
            
        }
//        else if (i == 3) {
//            m_state.enemies[i].set_height(0.9f);
//            m_state.enemies[i].set_width(0.9f);
//            m_state.enemies[i].set_speed(1.7f);
//
//
//
//            m_state.enemies[i].set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
//            m_state.enemies[i].set_position(glm::vec3(9.0f, -9.0f, 0.0f));
//            m_state.enemies[i].set_ai_type(WALKER1);
//        }
    }

    // —————— GOALS —————— //
    m_state.goals = new Entity[GOAL_COUNT];
    for (int i = 0; i < GOAL_COUNT; i++) {
        m_state.goals[i].m_texture_id = Utility::load_texture("assets/tile_0015.png");
        m_state.goals[i].set_entity_type(GOAL);
        m_state.goals[i].set_height(0.9f);
        m_state.goals[i].set_width(0.9f);

        m_state.goals[i].set_movement(glm::vec3(0.0f));

        switch (i) {
            case 0:
                m_state.goals[i].set_position(glm::vec3(9.0f, -1.0f, 0.0f));
                break;
            case 1:
                m_state.goals[i].set_position(glm::vec3(5.0f, -2.0f, 0.0f));
                break;
            case 2:
                m_state.goals[i].set_position(glm::vec3(0.0f, -3.0f, 0.0f));
                break;
            case 3:
                m_state.goals[i].set_position(glm::vec3(9.0f, -5.0f, 0.0f));
                break;
            case 4:
                m_state.goals[i].set_position(glm::vec3(4.0f, -6.0f, 0.0f));
                break;
            case 5:
                m_state.goals[i].set_position(glm::vec3(0.0f, -7.0f, 0.0f));
                break;
            case 6:
                m_state.goals[i].set_position(glm::vec3(6.0f, -9.0f, 0.0f));
                break;
            case 7:
                m_state.goals[i].set_position(glm::vec3(2.0f, 0.0f, 0.0f));
                break;
            default:
                break;
        }
    }

    // ––––– PLAYER ––––– //
    // Existing
    m_state.player = new Entity();
    m_state.player->set_position(glm::vec3(5.0f, -4.0f, 0.0f));
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

void LevelB::update(float delta_time, int& lives)
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
        LOG("Win LEVEL2");
        m_state.play_next_scene = true;
        m_state.next_scene_id = 2;
    }
    if (m_state.player->m_cost_life) {
        lives -= 1;
        m_state.player->m_cost_life = false;
    }
    for (int i = 0; i < GOAL_COUNT; i++) {
        m_state.goals[i].m_model_matrix = glm::scale((m_state.goals[i].m_model_matrix), glm::vec3(0.8f, 0.8f, 1.0f));
    }
}

void LevelB::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for (int i = 0; i < GOAL_COUNT; i++) m_state.goals[i].render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) m_state.enemies[i].render(program);
}
