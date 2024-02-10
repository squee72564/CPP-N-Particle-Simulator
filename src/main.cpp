#include "ParticleSimulation.hpp"
#include <iostream>

// Delta Time
const float TIME_STEP = 0.0007f;

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main()
{
    //Gravity for particles
    sf::Vector2f grav = sf::Vector2f(0,0);

    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    //Start Particle Simulation
    ParticleSimulation particleSimulation(TIME_STEP, grav, window, 8, 7, 128);
    std::cout << "Starting particle sim...\n";
    particleSimulation.run();
    std::cout << "Particle sim ended\n";

    return 0;
}
