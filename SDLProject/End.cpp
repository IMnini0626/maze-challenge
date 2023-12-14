#include "End.h"
#include "Utility.h"

#define LOG(argument) std::cout << argument << '\n'

End::~End()
{
    delete m_state.player;
}

void End::initialise()
{
    GLuint start_texture_id = Utility::load_texture("assets/end.png");
    m_state.player = new Entity();
    m_state.player->set_position(glm::vec3(4.5f, -4.5f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->m_texture_id = start_texture_id;
    m_state.player->set_entity_type(PLAYER);
}

void End::update(float delta_time, int& lives)
{
    m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map);
    
    m_state.player->m_model_matrix = glm::scale(m_state.player->m_model_matrix, glm::vec3(10.0f, 10.0f, 1.0f));
}

void End::render(ShaderProgram *program)
{
    m_state.player->render(program);
}
