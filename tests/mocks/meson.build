arduino_mock_lib = static_library('arduino_mock',
  files('Arduino.cpp', 'SPI.cpp'),
)

arduino_mock_dep = declare_dependency(
  link_with : arduino_mock_lib,
  include_directories : include_directories('.'),
)
