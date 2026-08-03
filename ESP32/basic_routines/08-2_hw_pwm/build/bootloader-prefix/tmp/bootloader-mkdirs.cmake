# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "I:/Espressif/frameworks/esp-idf-v5.1.2/components/bootloader/subproject"
  "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader"
  "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix"
  "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix/tmp"
  "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix/src/bootloader-stamp"
  "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix/src"
  "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "J:/github/test-code/ESP32/basic_routines/08-2_hw_pwm/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
