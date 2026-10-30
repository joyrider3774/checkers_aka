file(GLOB GAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

idf_component_register(
    SRCS ${GAME_SOURCES} "assets/assets.S"
    INCLUDE_DIRS "." "assets"
    REQUIRES gamebuino app_update esp_partition fatfs esp_timer
)

# assets.S takes assets.bin in with .incbin, which the assembler looks for on its own path
set_source_files_properties("assets/assets.S" PROPERTIES
    COMPILE_OPTIONS "-Wa,-I${CMAKE_CURRENT_SOURCE_DIR}/assets"
    OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/assets.bin")

# the GP2X code as it was written, it is not this port's place to rewrite it to these warnings
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-unused-variable -Wno-unused-function -Wno-sign-compare -Wno-char-subscripts
    -Wno-missing-field-initializers -Wno-narrowing -Wno-error=format-truncation -Wno-format-truncation)
