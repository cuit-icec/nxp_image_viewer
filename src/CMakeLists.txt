set(SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.ui
        imageviewer.cpp
        imageprocessor.cpp
        imagelistview.cpp
        imagelistmodel.cpp)
qt_add_resources(SOURCES res.qrc)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent)


set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )
