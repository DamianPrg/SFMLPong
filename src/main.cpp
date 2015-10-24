
/***
 * Simple pong game in SFML.
 *
 * Author: Damian Balandowski
 
 * Version: 0.1
 */

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"

#define LEVEL_PADDING 30
#define LEVEL_PADDING_X 200
#define PADDLE_WIDTH  22
#define PADDLE_HEIGHT 100
#define BALL_SIZE     11
#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 720
#define BALL_SPEED    320.0f
#define PADDLE_SPEED  80.0f

#define BORDER_COLOR sf::Color(255.0f, 255.0f, 255.0f)


class Vector2
{
public:
    Vector2(float a = 0.0f, float b = 0.0f) : x(a), y(b) {}
    
    float x, y;
};

class Object
{
public:
    Object()
    {
        position = Vector2();
        velocity = Vector2();
        size     = Vector2();
        
    }
    
    Vector2 position;
    Vector2 velocity;
    Vector2 size;
};

sf::Shape * circle;
sf::Shape * rectangle;

int leftPaddlePoints, rightPaddlePoints;

void DrawPaddle(sf::RenderWindow * window, const Object& object)
{
    // implement
    assert(window != nullptr);
    
    // border trick
    rectangle->setFillColor(BORDER_COLOR);
    rectangle->setPosition(object.position.x, object.position.y + 1.0f);
    window->draw( *rectangle );
    
    rectangle->setFillColor(BORDER_COLOR);
    rectangle->setPosition(object.position.x, object.position.y - 1.0f);
    window->draw( *rectangle );
    
    rectangle->setFillColor(BORDER_COLOR);
    rectangle->setPosition(object.position.x + 1.0f, object.position.y);
    window->draw( *rectangle );
    
    rectangle->setFillColor(BORDER_COLOR);
    rectangle->setPosition(object.position.x - 1.0f, object.position.y);
    window->draw( *rectangle );
    
    
    rectangle->setPosition(object.position.x, object.position.y);
    
    rectangle->setFillColor(sf::Color(0.0f, 0.0f, 0.0f));
    window->draw( *rectangle );
    
    
}

void DrawBall(sf::RenderWindow * window, const Object& object)
{
    assert(window != nullptr);
    
    circle->setPosition(object.position.x - 1, object.position.y);
    circle->setFillColor(sf::Color(255.0f, 250.0f, 250.0f));
    window->draw( *circle );
    
    circle->setPosition(object.position.x + 1, object.position.y);
    circle->setFillColor(sf::Color(255.0f, 250.0f, 250.0f));

    window->draw( *circle );
    
    circle->setPosition(object.position.x, object.position.y - 1);
    circle->setFillColor(sf::Color(255.0f, 250.0f, 250.0f));

    window->draw( *circle );
    
    circle->setPosition(object.position.x, object.position.y + 1);
    circle->setFillColor(sf::Color(255.0f, 250.0f, 250.0f));

    window->draw( *circle );
    
    circle->setPosition(object.position.x, object.position.y);
    circle->setFillColor(sf::Color(0.0f, 0.0f, 0.0f));
    window->draw( *circle );
}

void PaddleControl(Object& object, float delta = 0.0f)
{
    
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        object.velocity.y += PADDLE_SPEED * delta;
    }
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        object.velocity.y += -PADDLE_SPEED * delta;
    }
    
    object.position.y += object.velocity.y * delta * PADDLE_SPEED;
    
    object.velocity.y -= delta * object.velocity.y * 7.0;
    
    if(object.position.y < 0.0f)
    {
      //  object.position.y = 0.0f;
        object.velocity.y += 3;
    }
    
    if(object.position.y + PADDLE_HEIGHT > WINDOW_HEIGHT )
    {
        object.velocity.y += -3;
        //object.position.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
    }

}

void PaddleAIControl(Object& object, Object& ball, Object& player, float delta = 0.0f)
{
    static float _speed = PADDLE_SPEED;
    
    
    if(object.position.x - ball.position.x < WINDOW_WIDTH/2)
    {
        _speed = 1.2f;
    }
    else
    {
        _speed = 1.0f;
    }
    
    if(object.position.x - ball.position.x < WINDOW_WIDTH/3)
    {
       // _speed = 3.0f;
    }
    
    
    
    // ai logic
    if(ball.position.y > object.position.y)
    {
        object.velocity.y += 0.5 * _speed;
    }
    
    if(ball.position.y < object.position.y)
    {
        object.velocity.y += -0.5 * _speed;
    }
    
    
    //object.position.y += object.velocity.y * delta * 128.0f * _speed;
    object.position.y += object.velocity.y * delta * PADDLE_SPEED;
    
    object.velocity.y -= delta * object.velocity.y * 7.0;

    
    if(object.position.y < 0.0f)
    {
        object.position.y = 0.0f;
        
    }
    
    if(object.position.y + PADDLE_HEIGHT > WINDOW_HEIGHT )
    {
        object.position.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
    }
    
}

void UpdateObject(Object& object, float delta)
{
    object.position.x += object.velocity.x * delta;
    object.position.y += object.velocity.y * delta;
}

void UpdateBall(Object& object, float delta)
{
    object.position.x += object.velocity.x * delta * BALL_SPEED;
    object.position.y += object.velocity.y * delta * BALL_SPEED;
    
    if(object.position.y < 0.0f)
    {
        object.velocity.y = 1;
    }
    
    if(object.position.y > WINDOW_HEIGHT)
    {
        object.velocity.y = -1;
    }
    
    if(object.position.x + BALL_SIZE > WINDOW_WIDTH)
    {
        object.velocity.x = -1;
    }
    
    if(object.position.x + BALL_SIZE/2 < 0.0f)
    {
        object.velocity.x = 1;
    }
}

bool CheckCollision (const Object& a, const Object& b)
{
    if (a.position.x+a.size.x > b.position.x
        &&
        a.position.x < b.position.x + b.size.x
        &&
        a.position.y+a.size.y>b.position.y && a.position.y < b.position.y+b.size.y)
    {
        return true;
    }
    
    return false;
}

int main(int, char const**)
{
    int page = -1;
    // pages
    // -1 game
    // 0 menu
    // 1 start_game_ai
    // 1 about_game
    // 2 exit
    bool updateBall = false; // when game starts don't update ball
    
    
    // resets
    circle = nullptr;
    rectangle = nullptr;
    leftPaddlePoints = 0;
    rightPaddlePoints = 0;
    
    // initialization
    rectangle = new sf::RectangleShape(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    circle    = new sf::CircleShape(BALL_SIZE);
    
    rectangle->setFillColor(sf::Color(120.0f, 120.0f, 120.0f));
    
    sf::RenderWindow gameWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "FastPoing");
    
    gameWindow.setVerticalSyncEnabled(true);
    gameWindow.setFramerateLimit(60);
    
    Object leftPaddle, rightPaddle;
    
    rightPaddle.velocity.x = 0;
    rightPaddle.velocity.y = 0;
    
    Object ball;
    
    ball.size = Vector2(BALL_SIZE * 1.1, BALL_SIZE * 1.1);
    
    leftPaddle.size = Vector2(PADDLE_WIDTH + 2, PADDLE_HEIGHT + 2);
    rightPaddle.size = Vector2(PADDLE_WIDTH + 2, PADDLE_HEIGHT + 2);
    
    ball.velocity.x = 1;
    ball.velocity.y = 1;
    
    ball.position.x = 100.0f;
    ball.position.y = 100.0f;
    
    leftPaddle.position.x = LEVEL_PADDING + LEVEL_PADDING_X;
    rightPaddle.position.x = gameWindow.getSize().x - PADDLE_WIDTH - LEVEL_PADDING - LEVEL_PADDING_X;
    
    leftPaddle.position.y = rightPaddle.position.y = LEVEL_PADDING;
    
    Object* startingPaddle = &leftPaddle;
    
    float dt = 0.0f;
    
    sf::Clock clock;

    // text
    sf::Font font;
    font.loadFromFile(resourcePath() + "sansation.ttf");

    
    sf::Text text;
    text.setFont(font);
    text.setColor(sf::Color(100.0f, 100.0f, 100.0f));
    text.setString(sf::String("0 - 0"));
    text.setCharacterSize(40);
    text.setPosition((WINDOW_WIDTH / 2) - text.getLocalBounds().width / 2, LEVEL_PADDING / 2);
    
    
    sf::Clock clock2;
    
    while(gameWindow.isOpen())
    {
        sf::Event e;
        
        while(gameWindow.pollEvent(e))
        {
            if(e.type == sf::Event::Closed)
            {
                gameWindow.close();
            }
        }
        
        if(updateBall == false)
        {
            if(startingPaddle == &leftPaddle)
            {
                ball.position.x = leftPaddle.position.x + PADDLE_WIDTH + LEVEL_PADDING;
                ball.position.y = leftPaddle.position.y + (PADDLE_HEIGHT / 2) - BALL_SIZE;
            }
            else if(startingPaddle == &rightPaddle)
            {
                ball.position.x = rightPaddle.position.x - PADDLE_WIDTH - LEVEL_PADDING;
                ball.position.y = rightPaddle.position.y + (PADDLE_HEIGHT / 2) - BALL_SIZE;
            }

        }
        
        PaddleAIControl(rightPaddle, ball, leftPaddle, dt);
        PaddleControl(leftPaddle, dt);
        if(updateBall)
        {
        
            UpdateBall(ball, dt);
        }
            
        std::string scoresStr;
        scoresStr += std::to_string(leftPaddlePoints);
        scoresStr += " - ";
        scoresStr += std::to_string(rightPaddlePoints);
        
        text.setString(sf::String(scoresStr));
        
        if(CheckCollision(leftPaddle, ball) && ball.velocity.x == -1)
        {
            leftPaddlePoints ++;
            ball.velocity.x = 1;
            //leftPaddlePoints += 1;
        }
        
        if(CheckCollision(rightPaddle, ball) && ball.velocity.x == 1)
        {
            rightPaddlePoints++;
            ball.velocity.x = -1;
            //rightPaddlePoints += 1;
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && startingPaddle == &leftPaddle && updateBall == false)
        {
            ball.velocity.x = 1;
            ball.velocity.y = 1;
            
            updateBall = true;
        
        }
        
        // code for checking points and also level reserting
        
        // ball did go over right paddle, point for leftPaddle and give ball to rightPaddle
        if( ball.position.x > rightPaddle.position.x + PADDLE_WIDTH + 50.0f )
        {
            updateBall = false;
            // point ++ ?
            startingPaddle = &rightPaddle;
            
            //leftPaddlePoints ++;
            
            rightPaddlePoints --;
            
            clock2.restart();
        }
        
        if( ball.position.x < leftPaddle.position.x - PADDLE_WIDTH - 50.0f )
        {
            updateBall = false;
            startingPaddle = &leftPaddle;
            
           //rightPaddlePoints++;
            leftPaddlePoints--;
        }
        
        if( !updateBall && startingPaddle == &rightPaddle && clock2.getElapsedTime().asSeconds() > 2.0f)
        {
            ball.velocity.x = -1;
            ball.velocity.y = -1;
            
            updateBall = true;
        }
        
       // gameWindow.clear(sf::Color(20.0f, 20.0f, 20.0f));
        gameWindow.clear(sf::Color(20.0f, 20.0f, 20.0f));
        
        DrawPaddle(&gameWindow, leftPaddle);
        DrawPaddle(&gameWindow, rightPaddle);
        DrawBall(&gameWindow, ball);
        
        //DrawPaddle(&gameWindow, leftPaddle);
        
        gameWindow.draw(text);
        
        gameWindow.display();
        
        dt = clock.restart().asSeconds();
    }
    
    
    delete rectangle;
    delete circle;
    
    return 0;
}
