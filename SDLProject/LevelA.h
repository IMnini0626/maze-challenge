// LevelA.h
#define GL_SILENCE_DEPRECATION
#include "Scene.h"

class LevelA : public Scene {
public:
    ~LevelA();
    
    void initialise() override;
    void update(float delta_time, int& lives) override;
    void render(ShaderProgram *program) override;
};
