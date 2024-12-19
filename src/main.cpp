#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>

const unsigned RADIUS = 5;

struct PointCharge {
  float charge;
  sf::CircleShape circle = sf::CircleShape(RADIUS, 30);
};

struct Observer {
  sf::CircleShape circle = sf::CircleShape(RADIUS, 30);
};

// function definitions
sf::Vector2f fieldCalc(const std::vector<PointCharge *>&, const Observer&);
void normalizeVec(sf::Vector2f&);
sf::Vector2f getCircleMid(const sf::CircleShape &);
bool isMouseOnCharge(const std::vector<PointCharge*> &, const sf::RenderWindow &, PointCharge*&);
float vecMag(const sf::Vector2f &);
void lockChargeToMouse(PointCharge* , const sf::RenderWindow &, const sf::Event &);
void drawField(const std::vector<PointCharge *>&, sf::RenderWindow&, Observer&);

// const variables
const float e = 1.602 * pow(10, -19);
const sf::Color ELEC_COLOR = sf::Color::Red;
const sf::Color PRO_COLOR = sf::Color::Blue;

int main() {
  std::vector<PointCharge *> allCharges;

  // main window
  sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");

  sf::Font font;

  if (!font.loadFromFile("../fonts/toxi.otf")) {
    return EXIT_FAILURE;
  }

  // title text
  sf::Text text("Electric Field Simulator", font, 50);
  sf::Rect textBounds = text.getLocalBounds();
  float textMidToScreenMid = window.getSize().x / 2.0 - textBounds.width / 2.0;
  text.setPosition(textMidToScreenMid, 10);

  // define charges here:
  PointCharge elec;
  allCharges.push_back(&elec);
  elec.circle.setFillColor(ELEC_COLOR);
  elec.circle.setPosition(sf::Vector2f(160, 300));
  elec.charge = -e;

  PointCharge elec1;
  allCharges.push_back(&elec1);
  elec1.circle.setFillColor(ELEC_COLOR);
  elec1.circle.setPosition(sf::Vector2f(320, 300));
  elec1.charge = -e;
  
  PointCharge elec2;
  allCharges.push_back(&elec2);
  elec2.circle.setFillColor(ELEC_COLOR);
  elec2.circle.setPosition(sf::Vector2f(480, 300));
  elec2.charge = -e;

  

  Observer obs;
  obs.circle.setFillColor(sf::Color::White);
  obs.circle.setPosition(100, 300);

  sf::VertexArray line(sf::Lines, 2);

  // boolean variables
  bool clickedCharge = false;
  PointCharge* chargeCurr = nullptr; 


  // main loop
  while (window.isOpen()) {
    sf::Vector2f fieldAtObs = fieldCalc(allCharges, obs);

    // process events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::MouseButtonPressed) {
       
        if (isMouseOnCharge(allCharges, window, chargeCurr)) {
          clickedCharge = true;
        } else {
          clickedCharge = false;
        }
      }

      if (clickedCharge) {
        lockChargeToMouse(chargeCurr, window, event);
      }
    }

    // update screen
    window.setActive();
    window.clear();


    window.draw(text);
    window.draw(obs.circle);
    window.draw(elec.circle);
    window.draw(elec1.circle);
    window.draw(elec2.circle);

    drawField(allCharges, window, obs);

    window.display();
  }

  return 0;
}

sf::Vector2f fieldCalc(const std::vector<PointCharge *> &pc, const Observer &obs) {
  float k = 8.99 * pow(10,9);

  sf::Vector2f fieldVector;

  for (auto i : pc) {
    float x_dist =
        abs(getCircleMid(i->circle).x - obs.circle.getPosition().x);
    float y_dist =
        abs(getCircleMid(i->circle).y - obs.circle.getPosition().y);

    float dist = sqrt(pow(x_dist, 2) + pow(y_dist, 2));

    sf::Vector2f unit = getCircleMid(obs.circle) - getCircleMid(i->circle);
    normalizeVec(unit);

    float fieldMagWithSign = k * (i->charge / pow(dist, 2));

    fieldVector += fieldMagWithSign * unit * float(pow(10,15));
  }

  return fieldVector;
}

// turns passed in vector normal
void normalizeVec(sf::Vector2f &vec) {
  float vecMag = sqrt(pow(vec.x, 2) + pow(vec.y, 2));
  vec /= vecMag;
}

sf::Vector2f getCircleMid(const sf::CircleShape &cir) {
  sf::Vector2f mid;

  mid = cir.getPosition() + sf::Vector2f(RADIUS, RADIUS);

  return mid;
}

float vecMag(const sf::Vector2f &vec) {
  return sqrtf(pow(vec.x, 2) + pow(vec.y, 2));
}

// checks if the mouse pos is within the radius of the point charge
bool isMouseOnCharge(const std::vector<PointCharge*> &pc, const sf::RenderWindow &win, PointCharge*& pcCurr) {
  for (auto i : pc) {
    // get vector between mouse pos and center of point charge
    sf::Vector2 vec = getCircleMid(i->circle) - sf::Vector2f(sf::Mouse::getPosition(win));

    // if mag of vector is within the radius, return true
    if (vecMag(vec) < RADIUS) {
      pcCurr = i;
      return true;
    }
  }
  return false;
}

void lockChargeToMouse(PointCharge* charge, const sf::RenderWindow &window, const sf::Event &event) {
  if (event.type == sf::Event::MouseMoved) {
    charge->circle.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
  }
}

void drawField(const std::vector<PointCharge *>& pc, sf::RenderWindow& window, Observer& obs){

  //get window size for the loop
  sf::Vector2u windowSize = window.getSize();

  //for the vector lines
  sf::VertexArray vecLines(sf::Lines, 2);

  for(int i = 50; i <= windowSize.y; i+=20){
    for(int j = 0; j <= windowSize.x; j+=20){
      
      //move the observer to j,i
      obs.circle.setPosition(sf::Vector2f(j,i)); 

      //calculate field at j,i
      sf::Vector2f field = fieldCalc(pc, obs);

      normalizeVec(field);
      field *= 20.f;

      vecLines[0].position = obs.circle.getPosition();
      vecLines[1].position = obs.circle.getPosition() + field;

      window.draw(vecLines);
    }
  }
}
