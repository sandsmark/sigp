set(DEMO_SRCS
    main.cpp
    graphics.cpp
    sound.cpp)

include_directories(${GLUT_INCLUDE_DIR} ${ALSA_INCLUDE_DIR} ${GLU_INCLUDE_DIR})

add_executable(demo ${DEMO_SRCS})

target_link_libraries(demo ${GLUT_LIBRARIES} ${ALSA_LIBRARIES} ${GLU_LIBRARIES})

