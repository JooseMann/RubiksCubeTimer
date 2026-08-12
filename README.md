# QubeTimer

A Qt desktop application for timing Rubik's Cube solves

## Features

1. Generate a random 3x3 scramble.
2. Start and stop a timer to time your solves.
3. Compute Ao5, Ao12, Ao100, and overall average for a session.

## Building with CMake

This project uses CMake and Qt, so building it requires those utilities installed.

Compiling the project on Unix[^1] takes the following steps:

1. Clone the repository

```
git clone https://github.com/JooseMann/QubeTimer.git 
cd QubeTimer
```

2. Build with CMake and Make

```
mkdir build
cmake -B build
cd build
make
```

This generates the `QubeTimer` executable in the `build/` directory.

[^1]: I am not sure how these tools differ on Windows, so building there will most likely be at least a little different.

## Project Progress

- [X] A randomly generated 3x3 scramble.
- [X] A running timer that shows the time it takes to solve the cube.
  - [X] Keybinds to start and stop the timer on command.
- [X] A system that records your solves over time, and computes statistics about them.
  - [X] Show the statistics (current Ao5, Ao12, Ao100, and session average) on screen.
- [ ] A method of showing past solves in a list, and being able to click on one for more information on an individual solve.
  - [ ] Allow individual solves to be marked as +2 or DNF.
- [X] A scramble display that shows the correct scramble state, to verify that the scramble was done correctly.
- [ ] Allow different sessions to be created and switched between.
  - [ ] Allow these sessions to be named.
- [ ] Allow randomly generated scrambles for other cubes (e.g., 2x2 and 4x4 scrambles).
- [ ] An overall statistics page, showing best ever solve, Ao5, Ao12, Ao100, and session average, etc.
- [ ] Miscellaneous bug fixes.
