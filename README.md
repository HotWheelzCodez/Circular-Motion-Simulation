# Circular-Motion-Simulation
This is a physics simulation I created for a highschool physics lab do demnstrate circular motion using two cars going around a race-track where you, the black car (GTR-R35) is who you 'play' as, the white car (Mustang) is the car you have to try to get around. Your goal is to find the right speed and tyre type to make it aruond the white car without understeering off of the track. This simulation does compile for the web as well as for Windows and Unix machines.

## Try It Now (on the web)!
Vist the simulation through my itch.io page [here](https://hotwheelzcodz.itch.io/circ-motion)

## Installation
### Setting up on your local machine
Once you have installed the project all you have to do is run the run.sh file in the project

#### On Unix
```bash
bash run.sh
```
This will install Raylib and then compile it to ensure that it works on all machines and then it will compile the project then run it

#### On Windows
I don't have a run.bat file for this project yet, how ever you could easily make one for Windows based off of the run.sh file

### Compiling it for the web
If you want to compile the project to the web yourself, you can run this command
```bash
emcc -o webversion/index.html src/main.cpp -Os -Wall ./lib/libraylib.a -I. -Iinclude -L. -Llib -s --preload-file images --preload-file fonts  USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB
```

## License
This project is open-source, you can use, change it, whatever, just please don't write it off as your own

## Contributions
### Images
The images were from the internet, both the top-down-gtr view and the top-down-mustang view

### Raylib
This project uses [Raylib](https://github.com/raysan5/raylib) with C++ as well as [Raygui](https://github.com/raysan5/raygui) for the gui.

## Example Usage
### Setting up your values
![GUIScreen](https://github.com/HotWheelzCodez/Circular-Motion-Simulation/blob/main/images/thumbnails/mainScreenImage.png)
### Simulation In Progress
![Running](https://github.com/HotWheelzCodez/Circular-Motion-Simulation/blob/main/images/thumbnails/runningImage.png)
### Lost Screen
If you lose you get a tooltip at the bottom left to help guide you to find the correct values to win
![LostScreen](https://github.com/HotWheelzCodez/Circular-Motion-Simulation/blob/main/images/thumbnails/lostImage.png)
### Changing Vales
This allows you to change the tires following the way the Gran Turismo has their tire system down, how ever mine has the friction coefficient along with the tire so that you can use it in the equations needed to find things such as friction force, allowing you to find the centripetal force.
![ChangeValueScreen](https://github.com/HotWheelzCodez/Circular-Motion-Simulation/blob/main/images/thumbnails/changeValuesImage.png)
