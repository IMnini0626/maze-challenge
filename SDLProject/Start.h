// Start.h
#define GL_SILENCE_DEPRECATION
#include "Scene.h"

class Start : public Scene {
public:
    
    ~Start();
    
    void initialise() override;
    void update(float delta_time, int& lives) override;
    void render(ShaderProgram *program) override;
};
