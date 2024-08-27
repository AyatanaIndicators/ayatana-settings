# ayatana-settings

set_source_files_properties ("${CMAKE_BINARY_DIR}/data/${CMAKE_PROJECT_NAME}-gresource.c" PROPERTIES GENERATED TRUE)
add_executable ("ayatana-settings" "main.c" "${CMAKE_BINARY_DIR}/data/${CMAKE_PROJECT_NAME}-gresource.c")
add_dependencies ("ayatana-settings" "data")
target_compile_definitions ("ayatana-settings" PUBLIC G_LOG_DOMAIN="${CMAKE_PROJECT_NAME}" LOCALEDIR="${CMAKE_INSTALL_FULL_LOCALEDIR}" GETTEXT_PACKAGE="${CMAKE_PROJECT_NAME}")
target_link_libraries ("ayatana-settings" ${DEPS_LIBRARIES})
target_include_directories ("ayatana-settings" PUBLIC ${DEPS_INCLUDE_DIRS})
install (TARGETS "ayatana-settings" RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
