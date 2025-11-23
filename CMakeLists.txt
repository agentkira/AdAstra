cmake_minimum_required(VERSION 3.16)
project(RetroShooterQt LANGUAGES CXX)

if(APPLE)
    message(STATUS "Configuring for macOS — clean OpenGL link, no AGL")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-framework,AppKit -Wl,-framework,OpenGL -Wl,-framework,ImageIO -Wl,-framework,Metal"
    )
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(RetroShooterQt
    src/main.cpp
    src/Constants.h
    src/Game.h src/Game.cpp
    src/Player.h src/Player.cpp
    src/Bullet.h src/Bullet.cpp
    src/Asteroid.h src/Asteroid.cpp
    src/Mine.h src/Mine.cpp
    src/Explosion.h src/Explosion.cpp
    src/Debris.h src/Debris.cpp
    src/HUD.h src/HUD.cpp
    src/Star.h src/Star.cpp
    src/Background.h src/Background.cpp
    src/Planet.h src/Planet.cpp
    src/RasterGrid.h src/RasterGrid.cpp
    src/PixelFont.h src/PixelFont.cpp
    src/PixelArt.h
    
    # NEW FILE
    src/PowerUp.h src/PowerUp.cpp
    
    resources.qrc
)

target_link_libraries(RetroShooterQt PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(RetroShooterQt PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RetroShooterQt
)