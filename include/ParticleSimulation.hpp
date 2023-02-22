#ifndef PARTICLE_SIMULATION
#define PARTICLE_SIMULATION

#include "Simulation.hpp"
#include "Particle.hpp"
#include "QuadTree.hpp"
#include <vector>

class ParticleSimulation : Simulation
{
private:
    // Game Window
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    // Delta Time
    float timeStep;

    QuadTree quadTree;

    std::vector<Particle> particles;

    sf::Vector2f gravity;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

    bool isRightButtonPressed;
    sf::Vector2i current_mousePos;
    sf::Vector2f current_mousePosF;

    bool isAiming;
    sf::Vector2i initial_mousePos;
    sf::Vector2f initial_mousePosF;
    sf::Vector2i final_mousePos;
    sf::Vector2f final_mousePosF;

    bool showVelocity;
    bool showQuadTree;

    bool isPaused;
    
    float particleMass;

    sf::Font font;
    sf::Text particleCountText;
    sf::Text particleMassText;
    sf::Text velocityText;

    sf::Event event;

public:
    ParticleSimulation(float dt, const sf::Vector2f &g, sf::RenderWindow &window);
    virtual ~ParticleSimulation();
    void run();
    void pollUserEvent();
    void updateAndDraw();
    void drawAimLine();
    void drawParticleVelocity(Particle& particle);
    void attractParticleToMousePos(Particle& particle);
};

sf::Vector2f getMousePostion(const sf::RenderWindow &window, sf::Vector2i &mousePos);

#endif
