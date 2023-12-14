// End.h
#define GL_SILENCE_DEPRECATION
#include "Scene.h"

class End : public Scene {
public:
    
    ~End();
    
    void initialise() override;
    void update(float delta_time, int& lives) override;
    void render(ShaderProgram *program) override;
};
