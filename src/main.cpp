#include <iostream>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>

const unsigned RADIUS = 5;

struct PointCharge
{
    float charge; 
    sf::CircleShape circle = sf::CircleShape(RADIUS,30);
    sf::Vector2f position = circle.getPosition();
};

struct Observer
{
    sf::Vector2f position;
};

//function definitions
sf::Vector2f fieldCalc(const std::vector<PointCharge>& pc, const Observer& obs);
sf::Vector2f normalizeVec(const sf::Vector2f& vec);

//const variables
const float e = 1.602 * pow(10, -19);
const sf::Color ELEC_COLOR = sf::Color::Red;
const sf::Color PRO_COLOR = sf::Color::Blue;

int main()
{
    std::vector<PointCharge> allCharges;

    //main window
    sf::RenderWindow window(sf::VideoMode(800,600), "My Window");

    sf::Font font;

    if(!font.loadFromFile("../fonts/toxi.otf"))
    {
        return EXIT_FAILURE;
    }

    //title text
    sf::Text text("Electric Field Simulator", font, 50);
    sf::Rect textBounds = text.getLocalBounds();
    float textMidToScreenMid = window.getSize().x / 2.0 - textBounds.width / 2.0;
    text.setPosition(textMidToScreenMid, 10);

    //define charges here:
    PointCharge elec;
    allCharges.push_back(elec);
    elec.circle.setFillColor(ELEC_COLOR);
    elec.circle.setPosition(sf::Vector2f(40, 300));

    Observer obs;
    obs.position = sf::Vector2f(100,300);

    sf::Vector2f fieldAtObs = fieldCalc(allCharges, obs); 

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = obs.position;
    line[1].position = obs.position + fieldAtObs;

    //main loop
    while(window.isOpen())
    {
        //process events
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }

        }

        //update screen
        window.setActive();
        
        window.draw(text); 
        window.draw(elec.circle);
        window.draw(line);

        window.display();
    }

    return 0;
}

sf::Vector2f fieldCalc(const std::vector<PointCharge>& pc, const Observer& obs)
{
    float k = 1/(4*M_PI)*(8.85*pow(10,-12));
    sf::Vector2f fieldVector;

    for(auto i : pc)
    {
        float x_dist = abs(i.position.x - obs.position.x);
        float y_dist = abs(i.position.y - obs.position.y);

        float dist = sqrt(pow(x_dist, 2) + pow(y_dist, 2));

        float angle = std::asin(y_dist/dist);
        sf::Vector2f unit = sf::Vector2f(std::cos(angle), std::sin(angle));

        float fieldMagWithSign = k * i.charge / pow(dist, 2);

        fieldVector += (unit * 5.f);

    }
    return fieldVector;
}

//turns passed in vector normal
void normalizeVec(sf::Vector2f& vec)
{
    float vecMag = sqrt(pow(vec.x,2)+pow(vec.y,2));
    vec /= vecMag;
    
}
