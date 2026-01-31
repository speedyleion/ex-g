catch2_dep = dependency('catch2-with-main',
  default_options : ['tests=false']
)

subdir('mocks')

test_mouse_sensor = executable('test_mouse_sensor',
  files('test_mouse_sensor.cpp', '../MouseSensor.cpp'),
  include_directories : include_directories('..'),
  dependencies : [arduino_mock_dep, catch2_dep],
)

test('test_mouse_sensor', test_mouse_sensor)
