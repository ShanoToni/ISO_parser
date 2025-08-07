# ISO_parser
A small program to parse input from a text file, expecting ISO 15765-2 format frames into unique `ID: data` pairs

Example
 - 7400210C00000000000
 - ID: 740
 - type 0 (single frame)
 - size (2)
 - data(10C0)

Output of program:
```
740: 10C0
```
The data outputted when the text file is parsed is not converted from hex to binary.


## Example build process
```
$ mkdir build && cd build

# INPUT_FILE required to be set, default is "transcript.txt"
# Ninja: optional can be omitted 
$ cmake .. -DINPUT_FILE=path/to/transcript.txt -GNinja

# Ninja is optional; it can be omitted from the CMake command and compiled with `make`
$ ninja

# from build dir
$ ./ISOParser
```
