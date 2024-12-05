#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<iostream>
using namespace std;
using namespace sf;
// Constants for track dimensions
const int trackWidth = 800;
const int trackHeight = 600;

// Function to generate random obstacles
void generateObstacles(vector<RectangleShape>& obstacles, int numObstacles, int obstacleSize, Vector2f trackBounds) {
    obstacles.clear();
    srand(static_cast<unsigned>(time(0))); // Initialize random seed
    for (int i = 0; i < numObstacles; ++i) {
        RectangleShape obstacle(sf::Vector2f(obstacleSize, obstacleSize));
        obstacle.setFillColor(Color::Red);
        obstacle.setPosition(rand() % int(trackBounds.x), rand() % int(trackBounds.y));
        obstacles.push_back(obstacle);
    }
}

int main() {
    // Create game window
    RenderWindow window(VideoMode(trackWidth, trackHeight), "Car Game with SFML");

    // Asset Paths
    const string fontPath = "assets/fonts/arial.ttf";
    const string collisionSoundPath = "assets/sounds/collision.wav";
    const string backgroundMusicPath = "assets/sounds/background.ogg";

    // Load Font
    Font font;
    if (!font.loadFromFile(fontPath)) {
        cout << "Failed to load font from: " << fontPath << endl;
        return -1;
    }

    // Load Background Music
    Music bgMusic;
    if (!bgMusic.openFromFile(backgroundMusicPath)) {
        cout << "Failed to load background music from: " << backgroundMusicPath << endl;
        return -1;
    }
    bgMusic.setLoop(true);
    bgMusic.play();

    // Load Collision Sound Effect
    SoundBuffer collisionBuffer;
    if (!collisionBuffer.loadFromFile(collisionSoundPath)) {
        cout << "Failed to load collision sound from: " << collisionSoundPath << endl;
        return -1;
    }
    Sound collisionSound(collisionBuffer);

    // HUD
    Text scoreText("Score: 0", font, 20);
    scoreText.setPosition(10, 10);
    scoreText.setFillColor(Color::White);

    Text livesText("Lives: 3", font, 20);
    livesText.setPosition(10, 40);
    livesText.setFillColor(Color::White);

    // Player Car
    RectangleShape car(Vector2f(40, 40));
    car.setFillColor(Color::Blue);
    car.setPosition(trackWidth / 2, trackHeight - 60);

    // Obstacles
    vector<RectangleShape> obstacles;
    generateObstacles(obstacles, 5, 30, Vector2f(trackWidth, trackHeight));

    // Game variables
    int score = 0;
    int lives = 10;
    float carSpeed = 5.0f;
    Clock clock;

    // Game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        // Handle player movement
        if (Keyboard::isKeyPressed(Keyboard::Left) && car.getPosition().x > 0) {
            car.move(-carSpeed, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) && car.getPosition().x < trackWidth - car.getSize().x) {
            car.move(carSpeed, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Up) && car.getPosition().y > 0) {
            car.move(0, -carSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::Down) && car.getPosition().y < trackHeight - car.getSize().y) {
            car.move(0, carSpeed);
        }

        // Move obstacles and check collisions
        for (auto& obstacle : obstacles) {
            obstacle.move(0, carSpeed * 0.5f);
            if (obstacle.getPosition().y > trackHeight) {
                obstacle.setPosition(rand() % trackWidth, 0);
                score++;
            }

            if (car.getGlobalBounds().intersects(obstacle.getGlobalBounds())) {
                lives--;
                collisionSound.play();
                obstacle.setPosition(rand() % trackWidth, 0); // Reset obstacle
                if (lives <= 0) {
                    cout << "Game Over! Final Score: " << score << endl;
                    window.close(); // Game Over
                }
            }
        }

        // Update HUD
        scoreText.setString("Score: " + to_string(score));
        livesText.setString("Lives: " + to_string(lives));

        // Render
        window.clear();
        window.draw(car);
        for (const auto& obstacle : obstacles) {
            window.draw(obstacle);
        }
        window.draw(scoreText);
        window.draw(livesText);
        window.display();
    }

    return 0;
}
