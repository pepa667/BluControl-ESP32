# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

list(APPEND EXTRA_COMPONENT_DIRS "../blu-common/blu-components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(blucontrol-switch)

# # Localiza a biblioteca do componente HOJA-LIB-ESP32
# idf_component_get_property(hoja_lib HOJA-LIB-ESP32 COMPONENT_LIB)

# # Se o componente for encontrado, desativa o erro de endereço apenas para ele
# if(hoja_lib)
#     target_compile_options(${hoja_lib} PRIVATE "-Wno-address")
#     message(STATUS "Patch bebopCORE: Desativado -Waddress para HOJA-LIB-ESP32")
# endif()
