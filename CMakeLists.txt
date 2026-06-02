cmake_minimum_required(VERSION 3.16)
project(BibliotecaQt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Fișierele de bază ale aplicației (Core Logic) - aceleași ca în varianta Consolă
set(CORE_SOURCES
    Biblioteca.cpp
    Carte.cpp
    CarteFizica.cpp
    CarteDigitala.cpp
    CarteAudio.cpp
    Imprumut.cpp
    Utilizator.cpp
    Director.cpp
    Bibliotecar.cpp
    Ingrijitor.cpp
    Cititor.cpp
)

# Fișierele specifice interfeței grafice Qt
set(QT_SOURCES
    main_qt.cpp
    QtMainWindow.cpp
    QtDialogs.cpp
)

add_executable(biblioteca_qt ${CORE_SOURCES} ${QT_SOURCES})
target_link_libraries(biblioteca_qt PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets sqlite3)
