// LevelB.h
#define GL_SILENCE_DEPRECATION
#include "Scene.h"

class LevelC : public Scene {
public:
    ~LevelC();
    
    void initialise() override;
    void update(float delta_time, int& lives) override;
    void render(ShaderProgram *program) override;
};
