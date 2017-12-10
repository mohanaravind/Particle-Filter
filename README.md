

## Introduction
In this project we try to locate a moving vehicle using particle filter technique. We have noisy sensor data as inputs and initial GPS location. Velocity and yaw rate are control data that are available to help make vehicles location in the next instant of time. Using a discrete distribution of weighted probability of the particles we locate the vehicle

### Observations
Using `15 particles` I managed to get a descent run-time and accuracy. While trying out with the number of particles I once saw the vehicle passing the necessary criteria even with `10 particles`. The number of particles thus becomes a hyper parameter for our vehicle detection model 

### Localization
![Pass](/assets/pass.png)

### The Pipeline
![Algorithm](/assets/algorithm.png)

### The equations for updating x, y and the yaw angle when the yaw rate is not equal to zero
![Prediction](/assets/prediction.png)

### Coordinate transformation
![Transformation](/assets/transformation.png)
[Equation Source](http://planning.cs.uiuc.edu/node99.html)
[Theory about transformation](https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm)

## Running the Code
This project requires a Simulator which can be downloaded [here](https://github.com/udacity/self-driving-car-sim/releases)

This repository includes two files that can be used to set up and intall uWebSocketIO for either Linux or Mac systems. For windows you can use either Docker, VMware, or even Windows 10 Bash on Ubuntu to install uWebSocketIO.

Once the install for uWebSocketIO is complete, the main program can be built and ran by doing the following from the project top directory.

1. mkdir build
2. cd build
3. cmake ..
4. make
5. ./particle_filter

Alternatively some scripts have been included to streamline this process, these can be leveraged by executing the following in the top directory of the project:

1. ./clean.sh
2. ./build.sh
3. ./run.sh

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)

Note that the programs that need to be written to accomplish the project are src/particle_filter.cpp, and particle_filter.h

The program main.cpp has already been filled out, but feel free to modify it.

Here is the main protcol that main.cpp uses for uWebSocketIO in communicating with the simulator.

INPUT: values provided by the simulator to the c++ program

// sense noisy position data from the simulator

["sense_x"] 

["sense_y"] 

["sense_theta"] 

// get the previous velocity and yaw rate to predict the particle's transitioned state

["previous_velocity"]

["previous_yawrate"]

// receive noisy observation data from the simulator, in a respective list of x/y values

["sense_observations_x"] 

["sense_observations_y"] 


OUTPUT: values provided by the c++ program to the simulator

// best particle values used for calculating the error evaluation

["best_particle_x"]

["best_particle_y"]

["best_particle_theta"] 

//Optional message data used for debugging particle's sensing and associations

// for respective (x,y) sensed positions ID label 

["best_particle_associations"]

// for respective (x,y) sensed positions

["best_particle_sense_x"] <= list of sensed x positions

["best_particle_sense_y"] <= list of sensed y positions


# Implementing the Particle Filter
The directory structure of this repository is as follows:

```
root
|   build.sh
|   clean.sh
|   CMakeLists.txt
|   README.md
|   run.sh
|
|___data
|   |   
|   |   map_data.txt
|   
|   
|___src
    |   helper_functions.h
    |   main.cpp
    |   map.h
    |   particle_filter.cpp
    |   particle_filter.h
```


## Inputs to the Particle Filter
You can find the inputs to the particle filter in the `data` directory. 

#### The Map*
`map_data.txt` includes the position of landmarks (in meters) on an arbitrary Cartesian coordinate system. Each row has three columns
1. x position
2. y position
3. landmark id

### All other data the simulator provides, such as observations and controls.

> * Map data provided by 3D Mapping Solutions GmbH.


## Creating a Visual Studio Profile
1. Download the contents from [here](https://github.com/fkeidel/CarND-Term2-ide-profile-VisualStudio/tree/master/VisualStudio) to a folder named `ide_profiles` from your project root directory
2. Rename the files as per your project name
3. Update the `install-windows.bat` as per your project name
4. Edit `projectname.sln`, `projectname.vcxproj`, , `projectname.filters` by renaming with your project name
5. Update the `header` and `cpp` file references in the above two files
6. Run the `install-windows.bat` file
7. Open the `projectname.sln` and start coding
8. You could check if your external dependencies are correctly loaded to verify if you did all these steps right

### Future research
* Can improve the performance of the detection model further
* Dynamic particle size ?





