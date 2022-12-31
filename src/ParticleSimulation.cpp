#include "ParticleSimulation.hpp"

ParticleSimulation::ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window)
{
    gameWindow = &window;
    WINDOW_WIDTH = window.getSize().x;
    WINDOW_HEIGHT = window.getSize().y;


    timeStep = dt;
    gravity = g;
    particleMass = 5.0f;
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, 255);

    !font.loadFromFile("fonts/ALGER.TTF");

    particleCountText.setFont(font); // Set the font of the text
    particleCountText.setCharacterSize(24); // Set the size of the text
    particleCountText.setFillColor(sf::Color::White); // Set the fill color of the text
    particleCountText.setOutlineColor(sf::Color::Blue);
    particleCountText.setOutlineThickness(1.0f);

    particleMassText.setFont(font); // Set the font of the text
    particleMassText.setCharacterSize(12); // Set the size of the text
    particleMassText.setFillColor(sf::Color::White); // Set the fill color of the text
    particleMassText.setPosition(0, 100);
    particleMassText.setOutlineColor(sf::Color::Blue);
    particleMassText.setOutlineThickness(1.0f);

    velocityText.setFont(font);
    velocityText.setCharacterSize(10); // Set the size of the text
    velocityText.setFillColor(sf::Color::White); // Set the fill color of the text

    int level = 0;
    quadTree = QuadTree(level, WINDOW_WIDTH, WINDOW_HEIGHT);
}

ParticleSimulation::~ParticleSimulation()
{
    if (!particles.empty())
    {
        particles.clear();
    }
}

void ParticleSimulation::run()
{
    // Run the program as long as the window is open
    while (gameWindow->isOpen())
    {
        pollUserEvent();
        updateAndDraw();
    }
}

void ParticleSimulation::pollUserEvent()
{
    // Check for events
    while (gameWindow->pollEvent(event))
    {
        switch (event.type) {
            case sf::Event::Closed:
                gameWindow->close(); // Close the window
                break;

            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                {
                    if (particleMass < 10) { particleMass += 0.5; }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                {
                    if (particleMass > 1) { particleMass -= 0.5; }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
                {
                    showVelocity = (showVelocity) ? false : true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                {
                    showQuadTree = (showQuadTree) ? false : true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    isPaused = (isPaused) ? false : true;
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (isRightButtonPressed && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = false;
                }
                if (isAiming && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isAiming = false;
                    final_mousePosF = getMousePostion(*gameWindow, final_mousePos);
                    particles.emplace_back(Particle(initial_mousePosF, initial_mousePosF-final_mousePosF, particleMass, gen , dis));
                }
                break;

            case sf::Event::MouseButtonPressed:
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = true;
                    current_mousePosF = getMousePostion(*gameWindow, current_mousePos);
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isAiming = true;
                    initial_mousePosF = getMousePostion(*gameWindow, initial_mousePos);
                }
                break;

            case sf::Event::MouseWheelScrolled:
                current_mousePosF = getMousePostion(*gameWindow, current_mousePos);

                // Add a new particle with 0 velocity
                particles.emplace_back(Particle(current_mousePosF, sf::Vector2f(0,0), particleMass, gen , dis));
                break;

            default:
                break;
        }
    }
}

void ParticleSimulation::updateAndDraw()
{
    gameWindow->clear(); // Clear the window
    quadTree.deleteTree();

    // If LMB is pressed, create line for aim and show angle
    if (isAiming) {
        current_mousePosF = getMousePostion(*gameWindow, current_mousePos);

        velocityText.setPosition(initial_mousePosF.x+5, initial_mousePosF.y);
        velocityText.setString(std::to_string( abs( (atan( (initial_mousePosF.y - current_mousePosF.y)/(initial_mousePosF.x - current_mousePosF.x) ) * 180) / 3.14) ));

        sf::VertexArray line(sf::Lines, 2);
        line[0].position = initial_mousePosF;
        line[1].position = current_mousePosF;
        line[0].color  = sf::Color(0, 255, 0, 155);
        line[1].color = sf::Color(0, 255, 0, 25);
        
        gameWindow->draw(velocityText);
        gameWindow->draw(line);
    }
    
    // Update the particle count & mass text
    particleCountText.setString("Particle count: " + std::to_string(particles.size()));
    particleMassText.setString("Particle mass: " + std::to_string( int(particleMass) ));
    

    // Draw the particle count & mass text
    gameWindow->draw(particleCountText);
    gameWindow->draw(particleMassText);

    // Insert particles into QuadTree
    for (auto it = particles.begin(); it != particles.end(); ++it)
    {
        // Check if the particle's position is outside the window bounds
        if (it->position.x < 0 || it->position.x > WINDOW_WIDTH || it->position.y > WINDOW_HEIGHT)
        {
            // If the particle is outside the window bounds, erase it from the list and continue
            it = particles.erase(it);
            continue;
        }

        // Insert valid particle into QuadTree
        quadTree.insert(*it);
    }
    
    for (auto it = particles.begin(); it != particles.end(); ++it)
    {   
        if (!isPaused)
        {
            it->velocity += gravity; // Apply gravity to the velocity

            // Update the particle's position based on other particles O(n^2)
            // it->update(timeStep, particles);

            // Update position of particle based on Quadtree
            quadTree.update(timeStep, *it);

            // If RMB Pressed apply attractive force
            if (isRightButtonPressed)
            {
                current_mousePosF = getMousePostion(*gameWindow, current_mousePos);
                sf::Vector2f tempForce = sf::Vector2f(0.1 * (it->position.x - current_mousePosF.x), 0.1 * (it->position.y - current_mousePosF.y));
                it->velocity -= tempForce;
            }
        }

        it->shape.setPosition(it->position);
        gameWindow->draw(it->shape); // Draw the particle's shape

        // create visual for particle's velocity vector if toggled
        if (showVelocity)  {
            sf::VertexArray line(sf::Lines, 2);
            line[1].position.x = (it->position.x + it->velocity.x/10);
            line[1].position.y = (it->position.y + it->velocity.y/10);
            line[0].position = it->position;
            line[0].color  = sf::Color(0,0,255,255);
            line[1].color = sf::Color(255,0,0,0);
            
            gameWindow->draw(line); // Draw the velocity vector
        }
    }

    //Recursively draw QuadTree rectangles
    if (showQuadTree) {
        quadTree.display(gameWindow);
    }

    gameWindow->display(); // Display the window
}

sf::Vector2f getMousePostion(const sf::RenderWindow &window, sf::Vector2i &mousePos)
{
    // Get the mouse position
    mousePos = sf::Mouse::getPosition(window);
    // Convert the mouse position to a float vector
    return sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}