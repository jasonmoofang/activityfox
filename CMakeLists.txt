project (activityfox)
 
find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES})
 
set(tutorial2_SRCS 
  main.cpp
)
 
kde4_add_executable(activityfox ${tutorial2_SRCS})
target_link_libraries(activityfox ${KDE4_KDEUI_LIBS})