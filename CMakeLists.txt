cmake_minimum_required(VERSION 3.1)
project (activityfox)

cmake_policy(SET CMP0059 NEW)

find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES})
 
set(SOURCE_FILES
        main.cpp
        BrowserManager.h
        BrowserManager.cpp
        FirefoxManager.h
        FirefoxManager.cpp
        ChromiumManager.h
        ChromiumManager.cpp
        GoogleChromeManager.h
        GoogleChromeManager.cpp
)
 
kde4_add_executable(activityfox ${SOURCE_FILES})
target_link_libraries(activityfox ${KDE4_KDEUI_LIBS})

set_property(TARGET activityfox PROPERTY CXX_STANDARD 11)