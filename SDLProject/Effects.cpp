#include "Effects.h"

Effects::Effects(glm::mat4 projection_matrix, glm::mat4 view_matrix)
{
    // Non textured Shader
    m_program.load("shaders/vertex.glsl", "shaders/fragment.glsl");
    m_program.set_projection_matrix(projection_matrix);
    m_program.set_view_matrix(view_matrix);
    
    m_current_effect = NONE;
    m_alpha = 1.0f;
}

void Effects::draw_overlay()
{
    glUseProgram(m_program.get_program_id());

    float vertices[] =
    {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        
        -0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };

    glVertexAttribPointer(m_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(m_program.get_position_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(m_program.get_position_attribute());
}

void Effects::start(EffectType effect_type)
{
    m_current_effect = effect_type;

    switch (m_current_effect)
    {
        case NONE:   break;
        case FADEIN: break;
    }
}


void Effects::update(float delta_time)
{
   switch (m_current_effect)
   {
       case NONE:   break;
       case FADEIN:
           m_alpha -= delta_time;
           if (m_alpha <= 0) m_current_effect = NONE;
               
           break;
   }
}

void Effects::render()
{
    glm::mat4 model_matrix = glm::mat4(1.0f);

    switch (m_current_effect)
    {
        case NONE:   break;
        case FADEIN:
            // Scale the model matrix to cover the whole screen
            model_matrix = glm::scale(model_matrix, glm::vec3(20.0f, 20.0f, 0.0f));

            // Apply the model matrix to the overlay
            m_program.set_model_matrix(model_matrix);

            // Make it black (or whichever colour you want) and solid
            m_program.set_colour(0.0f, 0.0f, 0.0f, m_alpha); // current α-value
            
            // Draw
            draw_overlay();

            break;
    }
}
