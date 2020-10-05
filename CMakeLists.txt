cmake_minimum_required (VERSION 3.0)

project (ktip)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

find_package(KDE1 REQUIRED)
find_package(X11 REQUIRED)

include_directories (
    ${KDE1_INCLUDE_DIR}
)


include(CMakePackageConfigHelpers)
include(Qt1Macros)
include(KDE1Macros)
include(KDE1InstallDirs)

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++98 -Wno-write-strings")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif()

qt1_wrap_moc(MOC_FILES
    ktipwindow.h
)

add_executable(ktip
    ktipwindow.cpp

    ${MOC_FILES}
)
target_link_libraries(ktip
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    Qt::Qt1
    X11::X11
)

install(TARGETS ktip RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(ktip.xpm)
install(FILES ktip.kdelnk DESTINATION ${KDE1_APPSDIR}/Settings/)

install(FILES
    pics/wizard.gif
    DESTINATION ${KDE1_DATADIR}/ktip/pics/
)

install(FILES
    data/buttcoin
    data/dhammapada
    data/mechanicus
    data/oblique-ed4
    data/pcj
    data/pcj-titles
    data/vim-koans

    DESTINATION ${KDE1_DATADIR}/ktip/tips/
)
