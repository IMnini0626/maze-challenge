/**
* Author: Yini Zhang
* Assignment: Pong Clone
* Date due: 2023-10-21, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <cmath>
#include <iostream>

const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 0.9608f,
            BG_BLUE    = 0.9608f,
            BG_GREEN   = 0.9608f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char PLAYER_SPRITE[] = "sprites/half_pray.png";
const char BALL_SPRITE[] = "sprites/sweat_smile.png";
const char END_SPRITE[] = "sprites/Winning.png";

const glm::vec3 PLAYER1_INIT_POS = glm::vec3(-4.0f, 0.0f, 0.0f),
                PLAYER1_INIT_SCA = glm::vec3(0.8f, 2.4f, 0.0f);

const glm::vec3 PLAYER2_INIT_POS = glm::vec3(4.0f, 0.0f, 0.0f),
                PLAYER2_INIT_SCA = glm::vec3(-0.8f, 2.4f, 0.0f);

const glm::vec3 BALL_INIT_POS = glm::vec3(2.0f, 3.0f, 0.0f),
                BALL_INIT_SCA = glm::vec3(1.0f, 1.0f, 1.0f);

const glm::vec3 END_INIT_POS = glm::vec3(0.0f, 0.0f, 0.0f);

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL  = 0,
            TEXTURE_BORDER   = 0;

const float MILLISECONDS_IN_SECOND = 1000.0;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;

GLuint        g_player_texture_id,
              g_ball_texture_id,
              g_end_texture_id;

glm::mat4 g_view_matrix,
          g_player1_model_matrix,
          g_player2_model_matrix,
          g_ball_model_matrix,
          g_end_model_matrix,
          g_projection_matrix;

float g_previous_ticks  = 0.0f;
float g_rot_angle = 0.0f;
float g_speed = 1.0f;

glm::vec3 g_player1_movement = glm::vec3(0.0f, 0.0f, 0.0f),
          g_player1_position = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_player2_movement = glm::vec3(0.0f, 0.0f, 0.0f),
          g_player2_position = glm::vec3(0.0f, 0.0f, 0.0f);


glm::vec3 g_ball_movement = glm::vec3(-1.7f, -1.7f, 0.0f),
          g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);

bool g_game_end = false;
bool g_single_player = false;

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
    
    stbi_image_free(image);
    
    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Ping Pong!",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    g_player_texture_id = load_texture(PLAYER_SPRITE);
    g_ball_texture_id = load_texture(BALL_SPRITE);
    g_end_texture_id = load_texture(END_SPRITE);
    
    // SET INIT POSITION
    g_player1_position = PLAYER1_INIT_POS;
    g_player2_position = PLAYER2_INIT_POS;
    g_ball_position = BALL_INIT_POS;
    
    // ———————————————— GENERAL ———————————————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = !g_game_is_running;
                break;
            
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_game_is_running = !g_game_is_running;
                        break;
                        
                    case SDLK_t:
                        g_single_player = !g_single_player;
                        g_player2_movement.y = 2.0f;
                        
                    default: break;
                }
        }
    }
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
//    // for test ——————————————
//    if (key_state[SDL_SCANCODE_UP])
//    {
//        g_ball_movement.y = 2.0f;
//    }
//    else if (key_state[SDL_SCANCODE_DOWN])
//    {
//        g_ball_movement.y = -2.0f;
//    }
//    else if (key_state[SDL_SCANCODE_LEFT])
//    {
//        g_ball_movement.x = -2.0f;
//    }
//    else if (key_state[SDL_SCANCODE_RIGHT])
//    {
//        g_ball_movement.x = 2.0f;
//    }
//     ——————————————————————————
    if (!g_game_end) {
        if (key_state[SDL_SCANCODE_W])
        {
            g_player1_movement.y = 2.0f;
        }
        else if (key_state[SDL_SCANCODE_S])
        {
            g_player1_movement.y = -2.0f;
        }
        
        if (!g_single_player) {
            if (key_state[SDL_SCANCODE_UP])
            {
                g_player2_movement.y = 2.0f;
            }
            else if (key_state[SDL_SCANCODE_DOWN])
            {
                g_player2_movement.y = -2.0f;
            }
        }
    }
}

void update()
{
    // ———————————————— COLLISION DETECTION ———————————————— //
    
    float x_distance_with_1 = fabs(g_player1_position.x - g_ball_position.x) - ((PLAYER1_INIT_SCA.x + BALL_INIT_SCA.x) / 2.0f);
    float y_distance_with_1 = fabs(g_player1_position.y - g_ball_position.y) - ((PLAYER1_INIT_SCA.y + BALL_INIT_SCA.y) / 2.0f);
        
    float x_distance_with_2 = fabs(g_ball_position.x - g_player2_position.x) - ((-PLAYER2_INIT_SCA.x + BALL_INIT_SCA.x) / 2.0f);
    float y_distance_with_2 = fabs(g_ball_position.y - g_player2_position.y) - ((PLAYER2_INIT_SCA.y + BALL_INIT_SCA.y) / 2.0f);
    
    // BALL COLLISION WITH WALL
    // COLLISION with upper and lower wall
    if ((g_ball_position.y + BALL_INIT_SCA.y * 0.5) > 3.75f) { // collision with upper wall
//        std::cout << "REFLECT!" << std::endl;
        g_ball_movement.y = -g_ball_movement.y;
    }
    if ((g_ball_position.y - BALL_INIT_SCA.y * 0.5) < -3.75f) { // collision with lower wall
//        std::cout << "REFLECT!" << std::endl;
        g_ball_movement.y = -g_ball_movement.y;
    }
    
    // COLLSION with left and right wall
    if ((g_ball_position.x + BALL_INIT_SCA.x * 0.5) > 5.0f) { // collsion with left wall
        g_game_end = true;
    }
    if ((g_ball_position.x - BALL_INIT_SCA.x * 0.5) < -5.0f) { // collsion with right wall
        g_game_end = true;
    }
    
    
    // BALL COLLISION WITH BOARD
    // COLLISION with 2 boards
    if (x_distance_with_1 < 0 && y_distance_with_1 < 0) { // collision with left player
//        std::cout << "REFLECT with board 1" << std::endl;
        g_ball_movement.x = -g_ball_movement.x;
    }
    if (x_distance_with_2 < 0 && y_distance_with_2 < 0) { // collision with right player
//        std::cout << "REFLECT with board 2" << std::endl;
        g_ball_movement.x = -g_ball_movement.x;
    }
    
    // BOARD COLLISION WITH WALL
    if ((g_player1_position.y + PLAYER1_INIT_SCA.y * 0.5) > 3.75f) {
//        std::cout << "HAND1 REFLECT!" << std::endl;
        g_player1_position.y -= 0.03f;
        g_player1_movement.y = 0.0f;
    }
    else if ((g_player1_position.y - BALL_INIT_SCA.y * 0.5) < -3.75f) {
//        std::cout << "HAND1 REFLECT!" << std::endl;
        g_player1_position.y += 0.03f;
        g_player1_movement.y = 0.0f;
    }
    if ((g_player2_position.y + PLAYER2_INIT_SCA.y * 0.5) > 3.75f) { // collision with lower wall
//        std::cout << "HAND2 REFLECT!" << std::endl;s
        g_player2_position.y -= 0.03f;
        if (g_single_player) {
            std::cout << g_player2_movement.y << std::endl;
            g_player2_movement.y = -2.0f;
            std::cout << g_player2_movement.y << std::endl;
        }
        else {
            g_player2_movement.y = 0.0f;
        }
    }
    else if ((g_player2_position.y - BALL_INIT_SCA.y * 0.5) < -3.75f) {
//        std::cout << "HAND2 REFLECT!" << std::endl;
        g_player2_position.y += 0.03f;
        if (g_single_player) {
            g_player2_movement.y = 2.0f;
            
        }
        else {
            g_player2_position.y += 0.03f;
            g_player2_movement.y = 0.0f;
        }
    }
    
    // ———————————————— DELTA TIME CALCULATIONS ———————————————— //
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    // ———————————————— RESETTING MODEL MATRIX ———————————————— //
    g_player1_model_matrix = glm::mat4(1.0f);
    g_player2_model_matrix = glm::mat4(1.0f);
    g_ball_model_matrix = glm::mat4(1.0f);
    
    // ———————————————— TRANSLATIONS ———————————————— //
    
    // Player translation
    g_player1_position += g_player1_movement * g_speed * delta_time;
    g_player1_model_matrix = glm::translate(g_player1_model_matrix, g_player1_position);
    g_player1_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    
    g_player2_position += g_player2_movement * g_speed * delta_time;
    g_player2_model_matrix = glm::translate(g_player2_model_matrix, g_player2_position);
    if (!g_single_player) {
        g_player2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    // Ball translation
    g_ball_position += g_ball_movement * g_speed * delta_time;
    g_ball_model_matrix = glm::translate(g_ball_model_matrix, g_ball_position);
//    g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    
    g_player1_model_matrix = glm::scale(g_player1_model_matrix, PLAYER1_INIT_SCA);
    g_player2_model_matrix = glm::scale(g_player2_model_matrix, PLAYER2_INIT_SCA);
    g_ball_model_matrix = glm::scale(g_ball_model_matrix, BALL_INIT_SCA);
    
    g_end_model_matrix = glm::mat4(1.0f);
    g_end_model_matrix = glm::translate(g_end_model_matrix, END_INIT_POS);
    if (g_game_end) {
        g_end_model_matrix = glm::scale(g_end_model_matrix, glm::vec3(5.0f*2, 3.75f*2, 1.0f));
    }
    else {
        g_end_model_matrix = glm::scale(g_end_model_matrix, glm::vec3(0.01f, 0.01f, 0.01f));
    }
    
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ———————————————— FLOWER ———————————————— //
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    g_shader_program.set_model_matrix(g_player1_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_player_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // PLAYER2
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    g_shader_program.set_model_matrix(g_player2_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_player_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // ———————————————— ball ———————————————— //
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    g_shader_program.set_model_matrix(g_ball_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_ball_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // ———————————————— END PAGE ———————————————— //
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    g_shader_program.set_model_matrix(g_end_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_end_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // ———————————————— GENERAL ———————————————— //
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

/**
 Start here—we can see the general structure of a game loop without worrying too much about the details yet.
 */
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
