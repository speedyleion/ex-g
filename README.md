# EX-G

Code for using Arduino to implement an EX-G trackball, using the pre-existing
Ex-G hardware.

# Compiling

Compilation can be achieved using the [Arduino
CLI](https://docs.arduino.cc/arduino-cli/)

```sh
arduino-cli compile --profile esp32s3
```

# Testing

The tests run on the local host and use
[meson](https://mesonbuild.com/index.html)

To build:
```sh
meson setup build
meson test -C build
```
`build` is the directory chosen, one could change this if desired

```sh
meson setup output
meson test -C output
```
