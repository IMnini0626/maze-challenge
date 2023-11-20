/**
* Author: Yini Zhang
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include <cstdlib>
#include "Entity.h"
#include "Map.h"

// ––––– STRUCTS AND ENUMS ––––– //
struct GameState
{
    Entity* player;
    Entity* platforms;
    Entity* enemies;
    std::vector<Entity> traps;
    
    Map *map;
};

// ––––– CONSTANTS ––––– //
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char SPRITESHEET_FILEPATH[] = "assets/tilemap-characters_packed.png";
const char MAP_TILESET_FILEPATH[] = "assets/tilemap_blocks.png";
const char FONT_TILESET_FILEPATH[] = "assets/font1.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL  = 0;
const GLint TEXTURE_BORDER   = 0;

const int FONTBANK_SIZE = 16;


// ––––– GLOBAL VARIABLES ––––– //
GameState g_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// —————— MAP ——————
unsigned int LEVEL_1_DATA[] =
{
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    5, 6, 6, 6, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    25, 26, 26, 27, 0, 0, 9, 14, 15, 0, 0, 0, 0, 0,
    29, 30, 30, 31, 0, 0, 0, 29, 31, 0, 0, 0, 0, 0
};

// ––––– GENERAL FUNCTIONS ––––– //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return textureID;
}

void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int) text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;
        
        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Rise of the AI",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ––––– VIDEO ––––– //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // ————— MAP SET-UP ————— //
        GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
        g_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 8);
    
    // ––––– PLATFORMS ––––– //
    GLuint enemy_texture_id = load_texture(SPRITESHEET_FILEPATH);
////
    g_state.enemies = new Entity[ENEMY_COUNT];
////
////    // Set the type of every platform entity to PLATFORM
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        g_state.enemies[i].m_texture_id = enemy_texture_id;
        g_state.enemies[i].set_entity_type(ENEMY);
        g_state.enemies[i].set_height(0.9f);
        g_state.enemies[i].set_width(0.7f);
        
        g_state.enemies[i].set_movement(glm::vec3(0.0f));
        
        g_state.enemies[i].m_walking[g_state.enemies[i].LEFT]  = new int[2] { 2, 3 };
        g_state.enemies[i].m_walking[g_state.enemies[i].RIGHT] = new int[2] { 2, 3 };
        g_state.enemies[i].m_walking[g_state.enemies[i].UP]    = new int[2] { 2, 3 };
        g_state.enemies[i].m_walking[g_state.enemies[i].DOWN]  = new int[2] { 2, 3 };
        
        g_state.enemies[i].m_animation_indices = g_state.enemies[i].m_walking[g_state.player->LEFT];
        g_state.enemies[i].m_animation_frames = 2;
        g_state.enemies[i].m_animation_index  = 0;
        g_state.enemies[i].m_animation_time   = 0.0f;
        g_state.enemies[i].m_animation_cols   = 9;
        g_state.enemies[i].m_animation_rows   = 3;
        
        g_state.enemies[i].set_movement(glm::vec3(0.0f));
        g_state.enemies[i].set_speed(0.75f);
        g_state.enemies[i].set_acceleration(glm::vec3(0.0f, -4.905f, 0.0f));
        
//        g_state.enemies[i].set_font_texture(load_texture(FONT_TILESET_FILEPATH));
        if (i == 0) {
            g_state.enemies[i].set_position(glm::vec3(2.0f, 1.0f, 0.0f));
            g_state.enemies[i].set_ai_type(WALKER);
        }
        else if (i == 1) {
            g_state.enemies[i].set_position(glm::vec3(4.0f, 1.0f, 0.0f));
            g_state.enemies[i].set_ai_type(JUMPER);
        }
        else if (i == 2) {
            g_state.enemies[i].set_position(glm::vec3(7.0f, 1.0f, 0.0f));
            g_state.enemies[i].set_ai_type(GUARD);
            g_state.enemies[i].set_ai_state(IDLE);
        }
        
    }
    
    // ––––– PLAYER (GEORGE) ––––– //
    // Existing
    g_state.player = new Entity();
    g_state.player->set_position(glm::vec3(0.0f));
    g_state.player->set_movement(glm::vec3(0.0f));
    g_state.player->set_speed(1.0f);
    g_state.player->set_acceleration(glm::vec3(0.0f, -4.905f, 0.0f));
    g_state.player->m_texture_id = load_texture(SPRITESHEET_FILEPATH);
    
    // Walking
    g_state.player->m_walking[g_state.player->LEFT]  = new int[2] { 0, 1 };
    g_state.player->m_walking[g_state.player->RIGHT] = new int[2] { 0, 1 };
    g_state.player->m_walking[g_state.player->UP]    = new int[2] { 0, 1 };
    g_state.player->m_walking[g_state.player->DOWN]  = new int[2] { 0, 1 };

    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->LEFT];  // start George looking left
    g_state.player->m_animation_frames = 2;
    g_state.player->m_animation_index  = 0;
    g_state.player->m_animation_time   = 0.0f;
    g_state.player->m_animation_cols   = 9;
    g_state.player->m_animation_rows   = 3;
    g_state.player->set_height(0.9f);
    g_state.player->set_width(0.7f);
    g_state.player->set_entity_type(PLAYER);
    
    // Jumping
    g_state.player->m_jumping_power = 4.0f;
    
    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_game_is_running = false;
                        break;
                        
                    case SDLK_SPACE:
                        // Jump
                        if (g_state.player->m_collided_bottom)
                        {
                            std::cout << "JUMP" << std::endl;
                            g_state.player->m_is_jumping = true;
                            
                        }
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_state.player->move_left();
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_state.player->move_right();
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];
    }
    
    if (glm::length(g_state.player->get_movement()) > 1.0f)
    {
        g_state.player->set_movement(
                                     glm::normalize(
                                                    g_state.player->get_movement()
                                                    )
                                     );
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP)
    {
        g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map, &g_program);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, NULL, 0, g_state.map, &g_program);
        }
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // —————— MOVE CAMERA ——————
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-4.5f, 0.0f, 0.0f));
    
}

void render()
{
    g_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    
    g_state.map->render(&g_program);
    g_state.player->render(&g_program);
        
    for (int i = 0; i < ENEMY_COUNT; i++) g_state.enemies[i].render(&g_program);
    
    if (g_state.player->m_lose_state) {
        draw_text(&g_program, load_texture(FONT_TILESET_FILEPATH), "You", 2.0f, 0.0f, glm::vec3(1.0f, 1.3f, 0.0f));
        draw_text(&g_program, load_texture(FONT_TILESET_FILEPATH), "Lose", 2.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (g_state.player->m_win_state) {
        draw_text(&g_program, load_texture(FONT_TILESET_FILEPATH), "You", 2.0f, 0.0f, glm::vec3(1.4f, 1.3f, 0.0f));
        draw_text(&g_program, load_texture(FONT_TILESET_FILEPATH), "Win", 2.0f, 0.0f, glm::vec3(1.4f, 0.0f, 0.0f));
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete [] g_state.enemies;
    delete g_state.player;
    delete g_state.map;
}

// ––––– GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
