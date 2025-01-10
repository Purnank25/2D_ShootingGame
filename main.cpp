#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Bullet Class
class Bullet {
public:
    Bullet(float x, float y) {
        shape.setSize(sf::Vector2f(5.f, 15.f));
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y);
        velocity.y = -10.f; // Move upward
    }

    void update() {
        shape.move(velocity);
    }

    void render(sf::RenderWindow& window) {
        window.draw(shape);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }

    bool isOffScreen() {
        return shape.getPosition().y + shape.getSize().y < 0;
    }

private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
};

// Asteroid Class
class Asteroid {
public:
    Asteroid(float x, float y) {
        shape.setRadius(20.f);
        shape.setFillColor(sf::Color(169, 169, 169)); // Gray color
        shape.setPosition(x, y);
        velocity.y = 3.f + static_cast<float>(rand() % 3); // Random speed between 3 and 5
    }

    void update() {
        shape.move(velocity);
    }

    void render(sf::RenderWindow& window) {
        window.draw(shape);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }

    bool isOffScreen(float screenHeight) {
        return shape.getPosition().y > screenHeight;
    }

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for randomness

    // Create the game window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Game with Collisions");
    window.setFramerateLimit(60);

    // Player setup
    sf::RectangleShape player(sf::Vector2f(40.f, 20.f));
    player.setFillColor(sf::Color::Cyan);
    player.setPosition(400.f, 550.f);

    // Bullet list
    std::vector<Bullet> bullets;

    // Asteroid list
    std::vector<Asteroid> asteroids;
    float asteroidSpawnTimer = 0.f;
    float asteroidSpawnInterval = 1.f; // Spawn every 1 second

    // Clock for timing bullets and asteroids
    sf::Clock shotClock;
    sf::Clock asteroidClock;
    float shotInterval = 0.1f; // 0.3 seconds between shots

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.getPosition().x > 0) {
            player.move(-13.f, 0.f); // Move left
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && player.getPosition().x + player.getSize().x < window.getSize().x) {
            player.move(13.f, 0.f); // Move right
        }

        // Shooting bullets with shot interval
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (shotClock.getElapsedTime().asSeconds() > shotInterval) {
                bullets.emplace_back(player.getPosition().x + player.getSize().x / 2 - 2.5f, player.getPosition().y);
                shotClock.restart(); // Reset the clock
            }
        }

        // Spawn asteroids
        if (asteroidClock.getElapsedTime().asSeconds() > asteroidSpawnInterval) {
            float spawnX = static_cast<float>(rand() % (window.getSize().x - 40)); // Random X position
            asteroids.emplace_back(spawnX, 0.f);
            asteroidClock.restart();
        }

        // Update bullets
        for (size_t i = 0; i < bullets.size();) {
            bullets[i].update();
            if (bullets[i].isOffScreen()) {
                bullets.erase(bullets.begin() + i);
            }
            else {
                ++i;
            }
        }

        // Update asteroids
        for (size_t i = 0; i < asteroids.size();) {
            asteroids[i].update();
            if (asteroids[i].isOffScreen(window.getSize().y)) {
                asteroids.erase(asteroids.begin() + i);
            }
            else {
                ++i;
            }
        }

        // Collision detection: bullets and asteroids
        for (size_t i = 0; i < asteroids.size();) {
            bool destroyed = false;
            for (size_t j = 0; j < bullets.size(); ++j) {
                if (asteroids[i].getBounds().intersects(bullets[j].getBounds())) {
                    destroyed = true;
                    bullets.erase(bullets.begin() + j);
                    break;
                }
            }

            if (destroyed) {
                asteroids.erase(asteroids.begin() + i);
            }
            else {
                ++i;
            }
        }

        // Render
        window.clear(sf::Color::Black);

        // Draw player
        window.draw(player);

        // Draw bullets
        for (auto& bullet : bullets) {
            bullet.render(window);
        }

        // Draw asteroids
        for (auto& asteroid : asteroids) {
            asteroid.render(window);
        }

        window.display();
    }

    return 0;
}
