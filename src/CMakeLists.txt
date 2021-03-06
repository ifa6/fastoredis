SET(HEADERS_GLOBAL
    global/global.h
    global/types.h
)
SET(SOURCES_GLOBAL
    global/global.cpp
    global/types.cpp
)

SET(HEADERS_GUI_TO_MOC
    gui/main_window.h
    gui/widgets/log_tab_widget.h
    gui/widgets/log_widget.h
    gui/widgets/commands_widget.h
    gui/property_table_model.h
    gui/explorer/explorer_tree_view.h
    gui/explorer/explorer_tree_model.h
    gui/dialogs/about_dialog.h
    gui/dialogs/input_dialog.h
    gui/dialogs/info_server_dialog.h
    gui/dialogs/history_server_dialog.h
    gui/dialogs/property_server_dialog.h
    gui/dialogs/preferences_dialog.h
    gui/dialogs/connections_dialog.h
    gui/dialogs/connection_dialog.h
    gui/dialogs/cluster_dialog.h
    gui/dialogs/connection_diagnostic_dialog.h
    gui/dialogs/discovery_dialog.h
    gui/dialogs/encode_decode_dialog.h
    gui/dialogs/load_contentdb_dialog.h
    gui/dialogs/create_dbkey_dialog.h
    gui/dialogs/view_keys_dialog.h
    gui/dialogs/change_password_server_dialog.h
    gui/widgets/main_widget.h
    gui/main_tab_bar.h
    gui/fasto_editor.h
    gui/fasto_text_view.h
    gui/widgets/query_widget.h
    gui/widgets/output_widget.h
    gui/keys_table_model.h
    gui/fasto_tree_view.h
    gui/fasto_common_model.h
    gui/fasto_table_view.h
)
SET(HEADERS_GUI
    gui/dialogs/connection_listwidget_items.h
    gui/gui_factory.h
    gui/fasto_common_item.h
    gui/shortcuts.h
)
SET(SOURCES_GUI
    gui/main_window.cpp
    gui/gui_factory.cpp
    gui/widgets/log_tab_widget.cpp
    gui/widgets/log_widget.cpp
    gui/widgets/commands_widget.cpp
    gui/property_table_model.cpp
    gui/dialogs/about_dialog.cpp
    gui/dialogs/input_dialog.cpp
    gui/dialogs/preferences_dialog.cpp
    gui/dialogs/connections_dialog.cpp
    gui/dialogs/connection_dialog.cpp
    gui/dialogs/cluster_dialog.cpp
    gui/dialogs/connection_diagnostic_dialog.cpp
    gui/dialogs/discovery_dialog.cpp
    gui/dialogs/connection_listwidget_items.cpp
    gui/dialogs/info_server_dialog.cpp
    gui/dialogs/property_server_dialog.cpp
    gui/dialogs/history_server_dialog.cpp
    gui/dialogs/encode_decode_dialog.cpp
    gui/dialogs/load_contentdb_dialog.cpp
    gui/dialogs/create_dbkey_dialog.cpp
    gui/dialogs/view_keys_dialog.cpp
    gui/dialogs/change_password_server_dialog.cpp
    gui/widgets/main_widget.cpp
    gui/main_tab_bar.cpp
    gui/fasto_editor.cpp
    gui/fasto_text_view.cpp
    gui/widgets/query_widget.cpp
    gui/keys_table_model.cpp
    gui/widgets/output_widget.cpp
    gui/explorer/explorer_tree_view.cpp
    gui/explorer/explorer_tree_model.cpp
    gui/fasto_tree_view.cpp
    gui/fasto_common_model.cpp
    gui/fasto_table_view.cpp
    gui/fasto_common_item.cpp
    gui/shortcuts.cpp
)

SET(HEADERS_CORE_TO_MOC
    core/command_logger.h
    core/idriver.h
    core/iserver.h
    core/servers_manager.h
)
SET(HEADERS_CORE
    core/connection_types.h
    core/core_fwd.h
    core/icluster.h
    core/idatabase.h
    core/settings_manager.h
    core/connection_settings.h
    core/events/events.h
    core/events/events_info.h
    core/types.h
    core/ssh_info.h
    core/scp_file_transfer.h
)
SET(SOURCES_CORE
    core/events/events_info.cpp
    core/command_logger.cpp
    core/connection_types.cpp
    core/settings_manager.cpp
    core/connection_settings.cpp
    core/idriver.cpp
    core/iserver.cpp
    core/icluster.cpp
    core/idatabase.cpp
    core/servers_manager.cpp
    core/types.cpp
    core/ssh_info.cpp
    core/scp_file_transfer.cpp
)

SET(HEADERS_SHELL_TO_MOC
    shell/shell_widget.h
    shell/redis_shell.h
    shell/redis_lexer.h
    shell/memcached_shell.h
    shell/memcached_lexer.h
    shell/ssdb_lexer.h
    shell/ssdb_shell.h
)
SET(HEADERS_SHELL
)
SET(SOURCES_SHELL
    shell/shell_widget.cpp
    shell/redis_shell.cpp
    shell/redis_lexer.cpp
    shell/memcached_shell.cpp
    shell/memcached_lexer.cpp
    shell/ssdb_lexer.cpp
    shell/ssdb_shell.cpp
)

SET(HEADERS_TRANSLATIONS
    translations/global.h
)
SET(SOURCES_TRANSLATIONS
    translations/global.cpp
)

SET_DESKTOP_TARGET()

IF(OS_WINDOWS)
    SET(PLATFORM_HDRS)
    SET(PLATFORM_SRCS)
    SET(PLATFORM_LIBRARIES ws2_32)
ELSEIF(OS_MACOSX)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    SET(PLATFORM_LIBRARIES ${COCOA_LIBRARY})
ELSEIF(OS_LINUX)
    SET(PLATFORM_HDRS)
    SET(PLATFORM_SRCS)
    SET(PLATFORM_LIBRARIES)
ENDIF(OS_WINDOWS)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR} ${OPENSSL_INCLUDE_DIR})

ADD_SUBDIRECTORY(fasto)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/fasto)
ADD_SUBDIRECTORY(third-party/libssh2)
ADD_SUBDIRECTORY(third-party/redis)

#redis
SET(HEADERS_REDIS_TO_MOC
    core/redis/redis_cluster.h
    core/redis/redis_server.h
    core/redis/redis_driver.h
)
SET(HEADERS_REDIS
    core/redis/redis_infos.h
    core/redis/redis_config.h
    core/redis/redis_database.h
    core/redis/redis_settings.h
    core/redis/redis_cluster_settings.h
)
SET(SOURCES_REDIS
    core/redis/redis_config.cpp
    core/redis/redis_infos.cpp
    core/redis/redis_cluster.cpp
    core/redis/redis_server.cpp
    core/redis/redis_driver.cpp
    core/redis/redis_database.cpp
    core/redis/redis_settings.cpp
    core/redis/redis_cluster_settings.cpp
)

#memcached
FIND_PACKAGE(Sasl QUIET)
IF(SASL_FOUND)
    ADD_DEFINITIONS(-DHAVE_LIBSASL)
    INCLUDE_DIRECTORIES(${SASL_INCLUDE_DIR})
ENDIF(SASL_FOUND)

ADD_SUBDIRECTORY(third-party/memcached)

SET(HEADERS_MEMCACHED_TO_MOC
    core/memcached/memcached_server.h
    core/memcached/memcached_driver.h
)
SET(HEADERS_MEMCACHED
    core/memcached/memcached_infos.h
    core/memcached/memcached_config.h
    core/memcached/memcached_database.h
    core/memcached/memcached_settings.h
)
SET(SOURCES_MEMCACHED
    core/memcached/memcached_config.cpp
    core/memcached/memcached_infos.cpp
    core/memcached/memcached_server.cpp
    core/memcached/memcached_driver.cpp
    core/memcached/memcached_database.cpp
    core/memcached/memcached_settings.cpp
)

#ssdb

ADD_SUBDIRECTORY(third-party/ssdb)

SET(HEADERS_SSDB_TO_MOC
    core/ssdb/ssdb_server.h
    core/ssdb/ssdb_driver.h
)
SET(HEADERS_SSDB
    core/ssdb/ssdb_infos.h
    core/ssdb/ssdb_config.h
    core/ssdb/ssdb_database.h
    core/ssdb/ssdb_settings.h
)
SET(SOURCES_SSDB
    core/ssdb/ssdb_config.cpp
    core/ssdb/ssdb_infos.cpp
    core/ssdb/ssdb_server.cpp
    core/ssdb/ssdb_driver.cpp
    core/ssdb/ssdb_database.cpp
    core/ssdb/ssdb_settings.cpp
)

SET(HEADERS ${HEADERS} ${HEADERS_GLOBAL} ${HEADERS_GUI} ${HEADERS_CORE} ${HEADERS_SHELL} ${HEADERS_REDIS} ${HEADERS_MEMCACHED} ${HEADERS_SSDB} ${HEADERS_TRANSLATIONS})
SET(SOURCES ${SOURCES} ${SOURCES_GLOBAL} ${SOURCES_GUI} ${SOURCES_CORE} ${SOURCES_SHELL} ${SOURCES_REDIS} ${SOURCES_MEMCACHED} ${SOURCES_SSDB} ${SOURCES_TRANSLATIONS})
SET(HEADERS_TOMOC ${HEADERS_GUI_TO_MOC} ${HEADERS_CORE_TO_MOC} ${HEADERS_SHELL_TO_MOC} ${HEADERS_REDIS_TO_MOC} ${HEADERS_MEMCACHED_TO_MOC} ${HEADERS_SSDB_TO_MOC})

QTX_WRAP_CPP(MOC_FILES ${HEADERS_TOMOC})
CONFIGURE_FILE("resources/gui.qrc.in" "${CMAKE_CURRENT_LIST_DIR}/resources/gui.qrc" @ONLY IMMEDIATE)
SET(RESOURCES ${CMAKE_CURRENT_LIST_DIR}/resources/gui.qrc)
QTX_ADD_RESOURCES(RES_SRC ${RESOURCES})
SET(INCLUDE_DIRS ${INCLUDE_DIRS} third-party/redis/redis/src third-party/redis/redis/deps fasto/third-party/qscintilla/Qt4Qt5 third-party/memcached/src third-party/ssdb/src)
INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

SOURCE_GROUP("Header Files" FILES ${HEADERS} ${HEADERS_TOMOC})
SOURCE_GROUP("Source Files" FILES ${SOURCES})
SOURCE_GROUP("Generated" FILES ${MOC_FILES})
SOURCE_GROUP("Platform Files" FILES ${PLATFORM_HDRS} ${PLATFORM_SRCS})
SOURCE_GROUP("Resources" FILES ${RES_SRC} ${RESOURCE_OS})

SET(TRANSLATIONS
    translations/${PROJECT_NAME_LOWERCASE}_ru.ts
    translations/${PROJECT_NAME_LOWERCASE}_fr.ts
)

FIND_PACKAGE(Qt5LinguistTools)

IF(GENERATE_TRANSLATIONS)
    QTX_CREATE_TRANSLATION(QM_FILES ${HEADERS} ${SOURCES} ${TRANSLATIONS} OPTIONS -no-obsolete)
ELSE(GENERATE_TRANSLATIONS)
    QTX_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS})
ENDIF(GENERATE_TRANSLATIONS)

SET(ALL_SOURCES ${ALL_SOURCES} ${HEADERS} ${HEADERS_TOMOC} ${SOURCES} ${MOC_FILES} ${PLATFORM_HDRS} ${PLATFORM_SRCS})
SET(ALL_LIBS ${ALL_LIBS} fasto ${QT_LIBRARIES} ${PLATFORM_LIBRARIES} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARY})
IF(SASL_FOUND)
    SET(ALL_LIBS ${ALL_LIBS} ${SASL_LIBRARIES})
ENDIF(SASL_FOUND)

SET(OBJECT_LIBS ${OBJECT_LIBS} $<TARGET_OBJECTS:redis_cli> $<TARGET_OBJECTS:libssh2> $<TARGET_OBJECTS:libmemcached> $<TARGET_OBJECTS:ssd_cli>)

SET(EXE_SOURCES main.cpp ${RES_SRC} ${ICON_FILE} ${RESOURCE_OS} ${QM_FILES})

IF(MINGW OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
    ADD_APP_LIBRARY_OBJECT(${PROJECT_CORE_LIBRARY} "${ALL_SOURCES}" "${ALL_LIBS}" "${OBJECT_LIBS}")
    IF(OS_ANDROID)
        ADD_APP_SHARED_LIBRARY(${PROJECT_NAME} "${EXE_SOURCES}" "${PROJECT_CORE_LIBRARY}")
    ELSE()
        ADD_APP_EXECUTABLE(${PROJECT_NAME} "${EXE_SOURCES}" "${PROJECT_CORE_LIBRARY}")
    ENDIF(OS_ANDROID)
ELSE()
    MESSAGE(FATAL_ERROR "NOT SUPPORTED COMPILER!!!")
ENDIF(MINGW OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)

IF(OS_WINDOWS)
    SET_SOURCE_FILES_PROPERTIES(${RESOURCE_OS} PROPERTIES LANGUAGE RC)
ELSEIF(OS_MACOSX)
    SET_SOURCE_FILES_PROPERTIES(${RESOURCE_OS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    SET_SOURCE_FILES_PROPERTIES(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF(OS_WINDOWS)

GET_FILENAME_COMPONENT(ICON_FILE_NAME ${ICON_FILE} NAME)
IF(OS_MACOSX)
    SET(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_NAME})
    SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_VERSION},${PROJECT_COPYRIGHT}" )
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${SHORT_VERSION}" )
    SET(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    SET(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    SET(MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT})
    SET(MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME})
ENDIF(OS_MACOSX)

# Start to install

VersionConf(${PROJECT_NAME} ${RESOURCE_OS_IN} ${RESOURCE_OS} ${ICON_FILE_NAME})
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${TARGET_INSTALL_DESTINATION} COMPONENT APPLICATIONS)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION . COMPONENT LICENSE)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/COPYRIGHT DESTINATION . COMPONENT LICENSE)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/changelog.txt DESTINATION . COMPONENT LICENSE RENAME CHANGELOG)
INSTALL_QT(${PROJECT_NAME} ${LIB_INSTALL_DESTINATION})

IF(OS_WINDOWS)
    DEPLOY_QT_PLUGIN("platforms/qwindows" ${QT_PLUGINS_DIR} ${LIB_INSTALL_DESTINATION})
    DEPLOY_QT_PLUGIN("imageformats/qgif" ${QT_PLUGINS_DIR} ${LIB_INSTALL_DESTINATION})
    SET(PLACE_WHERE_SEARCH ${QT_BINS_DIR})

    #find runtime zlib
    SET(SHARED_ZLIB_NAMES z.dll zlib.dll zdll.dll zlib1.dll zlibd.dll zlibd1.dll)
    FIND_RUNTIME_LIBRARY(SHARED_ZLIB_LIBRARY SHARED_ZLIB_NAMES)
    INSTALL(FILES ${SHARED_ZLIB_LIBRARY} DESTINATION ${LIB_INSTALL_DESTINATION} COMPONENT RUNTIME)

    #find runtime msys
    SET(SHARED_MSYS_NAMES msys-2.0.dll)
    FIND_RUNTIME_LIBRARY(SHARED_MSYS_LIBRARY SHARED_MSYS_NAMES)
    INSTALL(FILES ${SHARED_MSYS_LIBRARY} DESTINATION ${LIB_INSTALL_DESTINATION} COMPONENT RUNTIME)

    #find runtime sasl
    SET(SHARED_SASL_NAMES msys-sasl2-3.dll)
    FIND_RUNTIME_LIBRARY(SHARED_SASL_LIBRARY SHARED_SASL_NAMES)
    INSTALL(FILES ${SHARED_SASL_LIBRARY} DESTINATION ${LIB_INSTALL_DESTINATION} COMPONENT RUNTIME)

ELSEIF(OS_MACOSX)
    SET(BUNDLE_NAME ${MACOSX_BUNDLE_BUNDLE_NAME}.app)
    DEPLOY_QT_PLUGIN("platforms/qcocoa" ${QT_PLUGINS_DIR} ${BUNDLE_NAME}/Contents/plugins)
    DEPLOY_QT_PLUGIN("accessible/qtaccessiblewidgets" ${QT_PLUGINS_DIR} ${BUNDLE_NAME}/Contents/plugins)
    DEPLOY_QT_PLUGIN("imageformats/qgif" ${QT_PLUGINS_DIR} ${BUNDLE_NAME}/Contents/plugins)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/install/qt.conf DESTINATION ${BUNDLE_NAME}/Contents/Resources COMPONENT RESOURCES)
ELSEIF(OS_LINUX)
    CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/install/linux/${PROJECT_NAME_LOWERCASE}.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}.sh" @ONLY IMMEDIATE)
    INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWERCASE}.sh DESTINATION ${TARGET_INSTALL_DESTINATION})
    INSTALL(FILES ${RESOURCE_OS} DESTINATION share/applications COMPONENT RESOURCES)
    INSTALL(FILES ${ICON_FILE} DESTINATION share/icons COMPONENT RESOURCES)
    DEPLOY_QT_PLUGIN("platforms/qxcb" ${QT_PLUGINS_DIR} ${LIB_INSTALL_DESTINATION})
    DEPLOY_QT_PLUGIN("platformthemes/qgtk2" ${QT_PLUGINS_DIR} ${LIB_INSTALL_DESTINATION})
    DEPLOY_QT_PLUGIN("imageformats/qgif" ${QT_PLUGINS_DIR} ${LIB_INSTALL_DESTINATION})
    SET(PLACE_WHERE_SEARCH ${QT_LIBS_DIR})
ENDIF(OS_WINDOWS)

IF(OS_MACOSX)
    INSTALL(FILES ${QM_FILES} DESTINATION ${BUNDLE_NAME}/Contents/MacOS/translations COMPONENT TRANSLATIONS)
ELSE()
    INSTALL(FILES ${QM_FILES} DESTINATION ${TARGET_INSTALL_DESTINATION}/translations COMPONENT TRANSLATIONS)
ENDIF(OS_MACOSX)

FOREACH(miscLib ${MISC_LIBS})
    DEPLOY_QT_PLUGIN(${miscLib} ${PLACE_WHERE_SEARCH} ${LIB_INSTALL_DESTINATION})
ENDFOREACH(miscLib ${MISC_LIBS})

INSTALL_RUNTIME_LIBRARIES()
TARGET_BUNDLEFIX(${PROJECT_NAME} ${LIB_INSTALL_DESTINATION})

IF(DEVELOPER_ENABLE_TESTS)
########## PREPARE GTEST LIBRARY ##########
    IF(MSVC)
        SET(gtest_force_shared_crt ON CACHE BOOL "Build shared gtest library")
        ADD_DEFINITIONS(-D_VARIADIC_MAX=10)
    ENDIF(MSVC)
    ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/tests/gtest gtest)
    INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
########## PREPARE GTEST LIBRARY ##########

    ADD_EXECUTABLE(unit_tests
        ${CMAKE_SOURCE_DIR}/tests/test_fasto_objects.cpp
        ${CMAKE_SOURCE_DIR}/tests/unit_test_common_net.cpp
        ${CMAKE_SOURCE_DIR}/tests/unit_test_common_strings.cpp
        global/global.cpp
    )

    TARGET_LINK_LIBRARIES(unit_tests gtest gtest_main fasto)

    ADD_TEST(NAME unit_tests COMMAND tests)
    SET_PROPERTY(TARGET unit_tests PROPERTY FOLDER "Unit tests")
ENDIF(DEVELOPER_ENABLE_TESTS)
