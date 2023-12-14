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
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include <cstdlib>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Start.h"
#include "End.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Effects.h"


// ––––– CONSTANTS ––––– //
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 640;

const float BG_RED     = 0.98f,
            BG_BLUE    = 0.761f,
            BG_GREEN   = 0.957f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

//const char V_SHADER_PATH[] = "shaders/vertex_lit.glsl",
//           F_SHADER_PATH[] = "shaders/fragment_lit.glsl";
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
 
const float MILLISECONDS_IN_SECOND = 1000.0;
const char FONT_TILESET_FILEPATH[] = "assets/font1.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL  = 0;
const GLint TEXTURE_BORDER   = 0;

const int FONTBANK_SIZE = 16;

const int CD_QUAL_FREQ    = 44100,
          AUDIO_CHAN_AMT  = 2,     // stereo
          AUDIO_BUFF_SIZE = 4096;

const int PLAY_ONCE = 0,    // play once, loop never
          NEXT_CHNL = -1,   // next available channel
          ALL_SFX_CHNL = -1;

const char BGM_FILEPATH[] = "assets/sugar.mp3",
           SFX_FILEPATH[] = "assets/pop.wav";

Mix_Music *g_music;
Mix_Chunk *g_collect_sfx;


// ––––– GLOBAL VARIABLES ––––– //
SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

int g_lives = 3;
bool g_fail = false;

// —————— SCENES ——————
Scene  *g_current_scene;
Start *g_start;
End *g_end;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;

Scene   *g_levels[4];

// —————— EFFECTS ——————
Effects *g_effects;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(FADEIN);
    g_current_scene->initialise();
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Maze Challenge!",
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
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -1.0f, 1.0f);
    
    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_program.get_program_id());
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    // ––––– BGM ––––– //
    Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);

    g_music = Mix_LoadMUS(BGM_FILEPATH); // works only with mp3 files

    Mix_PlayMusic(g_music, -1);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 3.0);
   
    // ––––– SFX ––––– //
    g_collect_sfx = Mix_LoadWAV(SFX_FILEPATH);

    // —————— Level Control ——————
    g_start = new Start();
    g_end = new End();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    
    g_levels[0] = g_level_a;
    g_levels[1] = g_level_b;
    g_levels[2] = g_level_c;
    g_levels[3] = g_end;

    switch_to_scene(g_start);
    
    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));

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
                        
                    case SDLK_RETURN:
                        if (g_current_scene == g_start) {
                            switch_to_scene(g_level_a);
                        }
                        else {
                            switch_to_scene(g_current_scene); // restart level
                        }

                    default:
                        break;
                }

            default:
                break;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    // —————— MOVING ——————

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->get_state().player->move_left();
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->get_state().player->move_right();
    }
    if (key_state[SDL_SCANCODE_UP])
    {
        g_current_scene->get_state().player->move_up();
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        g_current_scene->get_state().player->move_down();
    }

    if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
    {
        g_current_scene->get_state().player->set_movement(
                                     glm::normalize(
                                                    g_current_scene->get_state().player->get_movement()
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
        g_current_scene->update(FIXED_TIMESTEP, g_lives);
        g_effects->update(FIXED_TIMESTEP);
        if (g_current_scene)
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // —————— SOUND EFFECT ——————
    if (g_current_scene->get_state().player->m_collided_goal) {
        LOG("play sound effect");
        Mix_PlayChannel(NEXT_CHNL, g_collect_sfx, 0);
        g_current_scene->get_state().player->m_collided_goal = false;
    }
    
    // —————— MOVE CAMERA ——————
    g_view_matrix = glm::mat4(1.0f);
    if (g_current_scene == g_level_c) {
        
        g_view_matrix = glm::translate(g_view_matrix,
                                       glm::vec3(-g_current_scene->get_state().player->get_position().x+4.5f,
                                                 -g_current_scene->get_state().player->get_position().y-4.5f,
                                                 0.0f));
    }
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-4.5f, 4.5f, 0.0f));
}

void render()
{
    g_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    
    g_current_scene->render(&g_program);

    g_effects->render();
    
    
    
    // —————— CAMERA FOLLOWING ——————
    float x_position;
    float y_position;
    if (g_current_scene == g_level_c) {
        x_position = g_current_scene->get_state().player->get_position().x-4.5f;
        y_position = g_current_scene->get_state().player->get_position().y+4.5f;
    }
    else {
        x_position = 0;
        y_position = 0;
    }
    
    if (g_current_scene != g_start && g_current_scene != g_end) {
        Utility::draw_text(&g_program, Utility::load_texture(FONT_TILESET_FILEPATH), "Goal:",
                           0.8f, -0.4f, glm::vec3(x_position, y_position, 0.0f));
        Utility::draw_text(&g_program, Utility::load_texture(FONT_TILESET_FILEPATH), std::to_string(g_current_scene->get_state().player->m_current_goal_count),
                           1.0f, 0.0f, glm::vec3(x_position+2.0f, y_position, 0.0f));
        Utility::draw_text(&g_program, Utility::load_texture(FONT_TILESET_FILEPATH), "/", 1.0f, 0.0f, glm::vec3(x_position+2.4f, y_position, 0.0f));
        Utility::draw_text(&g_program, Utility::load_texture(FONT_TILESET_FILEPATH), std::to_string(g_current_scene->GOAL_COUNT),
                           1.0f, 0.0f, glm::vec3(x_position+2.8f, y_position, 0.0f));
    }

    if (g_current_scene->get_state().player->m_lose_state) {
        Utility::draw_text(&g_program, Utility::load_texture(FONT_TILESET_FILEPATH), "You Lose", 1.0f, -0.3f,
                           glm::vec3(x_position+2.0f, y_position-3.5f, 0.0f));
        Utility::draw_text(&g_program, Utility::load_texture(FONT_TILESET_FILEPATH), "Press enter to restart", 0.8f, -0.4f,
                           glm::vec3(x_position+0.4f, y_position-4.5f, 0.0f));
    }
//    else if (g_current_scene->get_state().player->m_win_state) {
//        Utility::draw_text(&g_program, font_texture, "You", 2.0f, 0.0f, glm::vec3(10.0f, 1.3f, 0.0f));
//        Utility::draw_text(&g_program, font_texture, "Win", 2.0f, 0.0f, glm::vec3(10.0f, 0.0f, 0.0f));
//    }
    
//    if (g_current_scene == g_level_c) {
//        g_program.set_light_position(g_current_scene->get_state().player->get_position());
//    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    delete g_start;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
    delete g_effects;
}

// ––––– GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        
        if (g_current_scene->m_state.play_next_scene) {
            LOG("switch to next scene");
            switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id]);
            g_current_scene->m_state.play_next_scene = false;
        }
            
        render();
    }
    
    shutdown();
    return 0;
}
