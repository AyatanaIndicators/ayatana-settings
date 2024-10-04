# ayatana-settings.8

install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.8" DESTINATION "${CMAKE_INSTALL_FULL_MANDIR}/man8")

# ayatana-settings.desktop

find_package (Intltool REQUIRED)
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.desktop.intl" @ONLY)
intltool_merge_translations ("${CMAKE_PROJECT_NAME}.desktop.intl" "${CMAKE_PROJECT_NAME}.desktop" ALL UTF8)
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.desktop" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/applications")

# icons

install (DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}")

# ayatana-settings-gresource.c

find_program (GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

add_custom_command (
    OUTPUT "${CMAKE_PROJECT_NAME}-gresource.c"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${GLIB_COMPILE_RESOURCES}"
    ARGS --target "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}-gresource.c" --generate-source "${CMAKE_PROJECT_NAME}.xml"
    VERBATIM
    MAIN_DEPENDENCY "${CMAKE_PROJECT_NAME}.xml"
    DEPENDS "${CMAKE_PROJECT_NAME}.ui"
)

add_custom_target ("data" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}-gresource.c")
