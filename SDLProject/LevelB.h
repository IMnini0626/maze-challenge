// LevelB.h
#define GL_SILENCE_DEPRECATION
#include "Scene.h"

class LevelB : public Scene {
public:
    ~LevelB();
    
    void initialise() override;
    void update(float delta_time, int& lives) override;
    void render(ShaderProgram *program) override;
};
