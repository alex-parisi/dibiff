# dibiff
![dibiff](dibiff.png)
### Directed Audio Graphs
#### Digital Biquad Filters in Float
#

# Building

To build, clone the repo and run CMake:

`mkdir build && cd build`
`cmake ..`
`make`

Run the test file:

`./Dibiff`

Right now, the `test.cpp` file is set to generate a few different wave sources infinitely, so after a few seconds, terminate the program with `^ + C` or `Ctrl + C`.

You can then find the output .wav files in the `build` folder.