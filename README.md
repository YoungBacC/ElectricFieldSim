# Electric Field Simulator

This is a personal project I made to simulate what the electric field would look like for a collection of point charges. All calculations were made using knowledge from my electromagnetism class (Coulomb's Law). Feel free to download it and use it for yourself. Make sure you have SFML installed, however. This project uses cmake to build, so you will need to set up the build file as well.  

## Build File
1. Go to project directory and make a build directory and go in it.
2. Run:
```
cmake ..
```

## Code to compile and run
```
g++ -std=c++17 main.cpp -I <sfml-path>/include -L <sfml-path>/lib -lsfml-graphics -lsfml-window -lsfml-system -o main"

./main
```

## Thank You!
