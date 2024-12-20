#include <SFML/Graphics.hpp>
#include <cmath>
#include<string>
#include <vector>

// const variables
const float e = 1.602 * pow(10, -19);
const sf::Color ELEC_COLOR = sf::Color::Red;
const sf::Color PRO_COLOR = sf::Color::Blue;
const unsigned RADIUS = 9;
const std::string ELEC_TYPE = "electron";
const std::string PROT_TYPE = "proton";

struct PointCharge {
  float charge;
  sf::CircleShape circle;

  PointCharge(sf::Vector2f pos, std::string type){
    if(type == ELEC_TYPE){
      charge = -e;
      circle.setFillColor(sf::Color::Red);
    }
    else if(type == PROT_TYPE){
      charge = e;
      circle.setFillColor(sf::Color::Blue);
    }

    circle.setRadius(RADIUS);
    circle.setPosition(pos);
  }
};

struct Observer {
  sf::CircleShape circle = sf::CircleShape(RADIUS, 30);
};

struct Button{
  sf::RectangleShape rect; 
  sf::Text buttonText;
  std::string tag;

  //constructor
  Button(const sf::Vector2f& pos, const std::string& text, const sf::Font& font, float fontSize, std::string t){
    tag = t;
    rect = sf::RectangleShape(sf::Vector2f(100, 40));
    rect.setPosition(pos);
    rect.setFillColor(sf::Color::Green);
    buttonText = sf::Text(text, font);
    buttonText.setCharacterSize(fontSize);
    buttonText.setFillColor(sf::Color::Black);

    sf::FloatRect textBounds = buttonText.getLocalBounds();

    //automatically fit text to button
    float deltaX = rect.getSize().x / 2.0 - textBounds.width / 2.0 - textBounds.left;
    float deltaY = rect.getSize().y / 2.0 - textBounds.height / 2.0 - textBounds.top;

    buttonText.setPosition(sf::Vector2f(rect.getPosition().x + deltaX, rect.getPosition().y + deltaY));
  }

  void draw(sf::RenderWindow& win){
    win.draw(rect);
    win.draw(buttonText);
  }

};

// function definitions
sf::Vector2f fieldCalc(const std::vector<PointCharge *>&, const Observer&, float&);
void normalizeVec(sf::Vector2f&);
sf::Vector2f getCircleMid(const sf::CircleShape &);
bool isMouseOnCharge(const std::vector<PointCharge*> &, const sf::RenderWindow &, PointCharge*&);
float vecMag(const sf::Vector2f &);
void lockChargeToMouse(PointCharge* , const sf::RenderWindow &, const sf::Event &);
void drawField(const std::vector<PointCharge *> &, sf::RenderWindow&, Observer&);
bool isMouseOnButton(const std::vector<Button *> &, const sf::RenderWindow &, std::string &);
void addCharge(const std::string, std::vector<PointCharge *> &);
void drawArrow(const sf::Vector2f &, const sf::Vector2f &, sf::RenderWindow &, float);

int main() {
  std::vector<PointCharge *> allCharges;
  std::vector<Button *> allButtons;

  // main window
  sf::RenderWindow window(sf::VideoMode(1000, 800), "My Window");

  sf::Font font;

  if (!font.loadFromFile("../fonts/toxi.otf")) {
    return EXIT_FAILURE;
  }

  // title text
  sf::Text text("Electric Field Simulator", font, 50);
  sf::Rect textBounds = text.getLocalBounds();
  float textMidToScreenMid = window.getSize().x / 2.0 - textBounds.width / 2.0;
  text.setPosition(textMidToScreenMid, 10);

  //define buttons here
  Button addElecButton(sf::Vector2f(10,10), "Add Electron", font, 13, ELEC_TYPE);
  Button addProButton(sf::Vector2f(890, 10), "Add Proton", font, 13, PROT_TYPE);
  Button trash(sf::Vector2f(890, 750), "Trash", font, 13, "trash");
  allButtons.push_back(&addElecButton);
  allButtons.push_back(&addProButton);
  allButtons.push_back(&trash);

  Observer obs;
  obs.circle.setFillColor(sf::Color::White);
  obs.circle.setPosition(100, 300);

  sf::VertexArray line(sf::Lines, 2);

  // boolean variables
  bool clickedCharge = false;
  bool clickedButton = false;


  PointCharge* chargeCurr = nullptr; 
  std::string buttonTag; 



  // main loop
  while (window.isOpen()) {
    // process events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if(isMouseOnButton(allButtons, window, buttonTag)){
        if(event.type == sf::Event::MouseButtonPressed){
          clickedButton = true;
        }
        else{
          clickedButton = false;
        }
      }

      if (event.type == sf::Event::MouseButtonPressed) {
       
        if (isMouseOnCharge(allCharges, window, chargeCurr)) {
          clickedCharge = true;
        }
        else {
          clickedCharge = false;
        }
      }

      if (clickedCharge) {
        lockChargeToMouse(chargeCurr, window, event);
        if(isMouseOnButton(allButtons, window, buttonTag) && buttonTag == "trash")
        {
          allCharges.erase(std::find(allCharges.begin(), allCharges.end(), chargeCurr));
          delete chargeCurr;
          chargeCurr = nullptr;
          clickedCharge = false;
        }
      }
      else if(clickedButton){
        addCharge(buttonTag, allCharges);
      }
    }

    // update screen
    window.setActive();
    window.clear();

       //draw text
    window.draw(text);

    //update field every frame
    drawField(allCharges, window, obs);

    for(auto i : allButtons)
    {
      i->draw(window);
    }

    //draw all charges every frame
    for(auto i : allCharges)
    {
      window.draw(i->circle);
    }
 


    window.display();
  }

  return 0;
}

sf::Vector2f fieldCalc(const std::vector<PointCharge *> &pc, const Observer &obs, float &strength) {
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
    
    fieldVector += fieldMagWithSign * unit;
  }

  strength = vecMag(fieldVector);

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
  pcCurr = nullptr;
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
  //sf::VertexArray vecLines(sf::Lines, 2);

  float relativeStrength;

  float proConst = 3.922 * pow(10, -16);

  float alpha;

  for(int i = 80; i <= windowSize.y; i+=20){
    for(int j = 0; j <= windowSize.x; j+=20){
      
      //move the observer to j,i
      obs.circle.setPosition(sf::Vector2f(j,i)); 

      //calculate field at j,i
      sf::Vector2f field = fieldCalc(pc, obs, relativeStrength);

      normalizeVec(field);
      field *= 15.f;

      //vecLines[0].position = getCircleMid(obs.circle);
      //vecLines[1].position = getCircleMid(obs.circle) + field;

      alpha = relativeStrength / proConst;

      if(alpha > 255){
        alpha = 255;
      }

      drawArrow(getCircleMid(obs.circle), getCircleMid(obs.circle) + field, window, alpha);

      //vecLines[0].color = sf::Color::Red;
      //vecLines[1].color = sf::Color::Blue;

      //vecLines[0].color.a = alpha; 
      //vecLines[1].color.a = alpha; 

      //window.draw(vecLines);
    }
  }
}

bool isMouseOnButton(const std::vector<Button *> & but, const sf::RenderWindow & win, std::string & butCurr){
  
  //check if mouse is on a button
  for(auto i : but){
    sf::FloatRect bound = i->rect.getGlobalBounds();

    float x1 = bound.left;
    float x2 = bound.left + bound.width;

    float y1 = bound.top;
    float y2 = bound.top + bound.height;

    bool inX = sf::Mouse::getPosition(win).x > x1 && sf::Mouse::getPosition(win).x < x2;
    bool inY = sf::Mouse::getPosition(win).y > y1 && sf::Mouse::getPosition(win).y < y2;

    if(inX && inY){
        i->rect.setFillColor(sf::Color::Cyan);
        butCurr = i->tag;
        return true;
      }
    else{
      i->rect.setFillColor(sf::Color::Green);
    }
  }
  return false;
}

void addCharge(const std::string chargeTag, std::vector<PointCharge *> &pc){
  
  if(chargeTag == ELEC_TYPE)
  {
    PointCharge* elec = new PointCharge(sf::Vector2f(10, 70), ELEC_TYPE);
    pc.push_back(elec);
  }
  else if(chargeTag == PROT_TYPE){
    PointCharge* prot = new PointCharge(sf::Vector2f(890, 70), PROT_TYPE);
    pc.push_back(prot);
  }
}

void drawArrow(const sf::Vector2f & fPos, const sf::Vector2f & sPos, sf::RenderWindow & win, float alpha){

  //make line
  sf::VertexArray line(sf::Lines, 2);

  line[0].position = fPos;
  line[1].position = sPos;

  sf::Vector2f dir = sPos - fPos;
  sf::Vector2f perpDir(-dir.y, dir.x);
  normalizeVec(perpDir);
  float triSide = vecMag(dir)/4.0;

  normalizeVec(dir);


  sf::Vector2f leftPoint(sPos + (perpDir * float(-triSide/2.0)));
  sf::Vector2f rightPoint(sPos + (perpDir * float(triSide/2.0)));
  sf::Vector2f topPoint(sPos + (float(sqrt(3)/2.0 * triSide) * dir));
  
  //define arrow head
  sf::VertexArray head(sf::Triangles, 3);

  head[0].position = leftPoint;
  head[1].position = rightPoint;
  head[2].position = topPoint;

  line[0].color = sf::Color::Blue;
  line[1].color = sf::Color::Red;

  line[0].color.a = alpha;
  line[1].color.a = alpha;

  head[0].color.a = alpha;
  head[1].color.a = alpha;
  head[2].color.a = alpha;

  win.draw(line);
  win.draw(head);
}

