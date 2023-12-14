#include "LevelC.h"
#include "Utility.h"

#define LOG(argument) std::cout << argument << '\n'
#define LEVEL3_WIDTH 19
#define LEVEL3_HEIGHT 19

// —————— MAP ——————
unsigned int LEVEL_3_DATA[] =
{
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 52, 69, 70, 53, 0, 28, 39, 41, 0, 0, 28, 0, 0, 28, 0, 6,
    6, 0, 0, 84, 0, 0, 87, 0, 44, 0, 0, 12, 0, 44, 0, 0, 44, 0, 6,
    6, 0, 0, 84, 0, 0, 0, 0, 44, 39, 41, 0, 0, 44, 39, 41, 44, 0, 6,
    6, 0, 0, 84, 0, 0, 87, 0, 44, 0, 0, 0, 0, 44, 0, 0, 44, 0, 6,
    6, 0, 0, 100, 102, 102, 103, 0, 60, 0, 0, 0, 0, 60, 0, 0, 60, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 28, 0, 0, 0, 0, 61, 14, 63, 0, 0, 0, 57, 0, 0, 0, 6,
    6, 0, 0, 44, 0, 0, 0, 0, 0, 44, 0, 0, 0, 57, 0, 38, 0, 0, 6,
    6, 0, 0, 44, 0, 0, 0, 0, 0, 44, 0, 0, 0, 57, 0, 0, 28, 0, 6,
    6, 0, 0, 44, 0, 0, 0, 0, 0, 44, 0, 0, 0, 57, 39, 41, 44,0,  6,
    6, 0, 0, 45, 62, 63, 0, 0, 61, 46, 63, 0, 0, 57, 0, 0, 60, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
};

LevelC::~LevelC()
{
    delete [] m_state.goals;
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
}

void LevelC::initialise() {
    LOG("Intializing LEVEL3");
    GOAL_COUNT = 8;
    ENEMY_COUNT = 2;

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture("assets/tilemap_farm_packed.png");
    m_state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, LEVEL_3_DATA, map_texture_id, 1.0f, 16, 7);

    // ––––– ENEMIES ––––– //
    GLuint sprite_texture_id = Utility::load_texture("assets/tilemap-characters_packed.png");
//    GLuint ophelia_texture_id = Utility::load_texture("assets/ophelia.png");
    m_state.enemies = new Entity[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_height(0.9f);
        m_state.enemies[i].set_width(0.7f);

        m_state.enemies[i].m_texture_id = map_texture_id;
        m_state.enemies[i].m_animation = new int[2] { 90, 91 };
        m_state.enemies[i].m_animation_indices = m_state.enemies[i].m_animation;
        m_state.enemies[i].m_animation_frames = 2;
        m_state.enemies[i].m_animation_index  = 0;
        m_state.enemies[i].m_animation_time   = 0.0f;
        m_state.enemies[i].m_animation_cols   = 16;
        m_state.enemies[i].m_animation_rows   = 7;
        if (i == 0) {
            m_state.enemies[i].set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            m_state.enemies[i].set_position(glm::vec3(5.0f, -1.0f, 0.0f));
            m_state.enemies[i].set_speed(0.7f);
            m_state.enemies[i].set_ai_type(CHASER);
        }
        else if (i == 1) {
            m_state.enemies[i].set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            m_state.enemies[i].set_position(glm::vec3(11.0f, -15.0f, 0.0f));
            m_state.enemies[i].set_speed(1.0f);
            m_state.enemies[i].set_ai_type(CHASER);
        }
    }

    // —————— GOALS —————— //
    m_state.goals = new Entity[GOAL_COUNT];
    for (int i = 0; i < GOAL_COUNT; i++) {
        m_state.goals[i].m_texture_id = Utility::load_texture("assets/tile_0128.png");
        m_state.goals[i].set_entity_type(GOAL);
        m_state.goals[i].set_height(0.9f);
        m_state.goals[i].set_width(0.9f);

        m_state.goals[i].set_movement(glm::vec3(0.0f));

        switch (i) {
            case 0:
                m_state.goals[i].set_position(glm::vec3(11.0f, -2.0f, 0.0f));
                break;
            case 1:
                m_state.goals[i].set_position(glm::vec3(4.0f, -4.0f, 0.0f));
                break;
            case 2:
                m_state.goals[i].set_position(glm::vec3(14.0f, -4.0f, 0.0f));
                break;
            case 3:
                m_state.goals[i].set_position(glm::vec3(6.0f, -8.0f, 0.0f));
                break;
            case 4:
                m_state.goals[i].set_position(glm::vec3(2.0f, -10.0f, 0.0f));
                break;
            case 5:
                m_state.goals[i].set_position(glm::vec3(7.0f, -12.0f, 0.0f));
                break;
            case 6:
                m_state.goals[i].set_position(glm::vec3(12.0f, -14.0f, 0.0f));
                break;
            case 7:
                m_state.goals[i].set_position(glm::vec3(3.0f, -16.0f, 0.0f));
                break;
            default:
                break;
        }
    }

    // ––––– PLAYER ––––– //
    // Existing
    m_state.player = new Entity();
    m_state.player->m_is_levelc = true;
    m_state.player->set_position(glm::vec3(8.0f, -8.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(1.8f);
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
    m_state.player->set_width(0.7f);
    m_state.player->set_entity_type(PLAYER);
}

void LevelC::update(float delta_time, int& lives)
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
        LOG("Win LEVEL3");
        m_state.play_next_scene = true;
        m_state.next_scene_id = 3;
    }
    for (int i = 0; i < GOAL_COUNT; i++) {
        m_state.goals[i].m_model_matrix = glm::scale((m_state.goals[i].m_model_matrix), glm::vec3(0.8f, 0.8f, 1.0f));
    }
}

void LevelC::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for (int i = 0; i < GOAL_COUNT; i++) m_state.goals[i].render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) m_state.enemies[i].render(program);
}
