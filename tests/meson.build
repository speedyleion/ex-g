catch2_dep = dependency('catch2-with-main',
  default_options : ['tests=false']
)

subdir('mocks')

test_motion_sensor = executable('test_motion_sensor',
  files('test_motion_sensor.cpp', '../MotionSensor.cpp'),
  include_directories : include_directories('..'),
  dependencies : [arduino_mock_dep, catch2_dep],
)

test('test_motion_sensor', test_motion_sensor)
