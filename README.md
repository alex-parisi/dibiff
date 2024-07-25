# dibiff
![dibiff](dibiff.png)
## Directed Audio Graphs
<ins>Digital Biquad Filters in Float</ins>

Multi-threaded real-time processing of audio graphs, using [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) for SIMD vector math, [miniaudio](https://miniaud.io/) for audio device interactions, and [rtmidi](https://www.music.mcgill.ca/~gary/rtmidi/) for MIDI device interactions.
#

# Building

To build, clone the repo and run CMake:

`mkdir build && cd build`

`cmake ..`

`make`

# Running 

To run the test file, first make sure you have a MIDI device connected to your computer.

Then, run the test file:

`./Dibiff`

If your MIDI device was not recognized by rtmidi, you'll see an error similar to this:
```
There are 0 MIDI input sources available.

MidiInCore::openPort: no MIDI input sources found!
```
Make sure it's plugged in and on.

If instead you see this:
```
There are 1 MIDI input sources available.
        Input Port #0: <DEVICE_NAME>
```
play some notes on your MIDI device, and you should hear the Sine Generator output.

If your MIDI device input port is not 0, you'll need to change the port number in `test.cpp` and rebuild:
```
auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, <PORT_NUMBER>));
```
Right now, the `test.cpp` file is set to generate a MIDI-controlled Sine Wave, and write the output to a .wav file. Once you're done, terminate the program with `Ctrl + C` or `^ + C`.

You can then find the output .wav files in the `build` folder.

# Usage
TODO