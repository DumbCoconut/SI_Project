#GLEW_PATH  = ../ext/glew-1.9.0
GLM_PATH    = ../ext/glm-0.9.4.1

TEMPLATE    = app
TARGET      = terrain_rendering 

#LIBS       += -Wl,-rpath $${GLEW_PATH}/lib -L$${GLEW_PATH}/lib
LIBS        += -lGLEW -lGL -lGLU -lm
INCLUDEPATH += $${GLM_PATH}

SOURCES = shader.cpp meshLoader.cpp trackball.cpp camera.cpp main.cpp viewer.cpp grid.cpp \
    resolution.cpp
HEADERS = shader.h meshLoader.h trackball.h camera.h viewer.h grid.h \ 
    resolution.h

CONFIG  += qt opengl warn_on thread uic4 release
QT      *= xml opengl core
