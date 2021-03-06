CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
#############################################################
PROJECT("FastoRedis" VERSION 0.7.3.0 LANGUAGES CXX C)
#################### Project Settings ####################
#SET(PROJECT_NAME "FastoRedis" CACHE STRING "Project Name")
SET(PROJECT_NAME_TITLE ${PROJECT_NAME} CACHE STRING "Title for ${PROJECT_NAME}")
SET(PROJECT_DOMAIN "www.fastoredis.com" CACHE STRING "Internet domain name for ${PROJECT_NAME}")
SET(PROJECT_COMPANYNAME "Fasto" CACHE STRING "Company name for ${PROJECT_NAME}")
SET(PROJECT_COPYRIGHT "Copyright (C) 2014-2015 ${PROJECT_COMPANYNAME} All Rights Reserved.")
SET(PROJECT_COMPANYNAME_DOMAIN "www.fastoredis.com" CACHE STRING "Internet domain name for ${PROJECT_NAME}")
SET(PROJECT_MAINTAINER_MAIL "atopilski@fastoredis.com" CACHE STRING "Internet mail address for ${PROJECT_NAME} mainteiner")
SET(PROJECT_MAINTAINER_NAME "Alexandr Topilski" CACHE STRING "Name of ${PROJECT_NAME} mainteiner")
SET(PROJECT_GITHUB_FORK "www.github.com/fasto/fastoredis" CACHE STRING "Git fork of ${PROJECT_NAME}")
SET(PROJECT_GITHUB_ISSUES "www.github.com/fasto/fastoredis/issues" CACHE STRING "Git issue link for ${PROJECT_NAME}")
SET(PROJECT_BUILD_TYPE "release") #alfa, beta, rc, release
##########################################################

STRING(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERRCASE)
SET(PROJECT_CORE_LIBRARY ${PROJECT_NAME_LOWERCASE}core)

SET(PROJECT_VERSION_SHORT ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} CACHE STRING "${PROJECT_NAME} short version")
SET(PROJECT_VERSION_INTEGER ${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH})

MESSAGE(STATUS "PROJECT_VERSION: ${PROJECT_VERSION}")
MESSAGE(STATUS "CMAKE_OSX_SYSROOT: ${CMAKE_OSX_SYSROOT}")

IF(APPLE AND CMAKE_OSX_SYSROOT)
    STRING(REGEX REPLACE ".*MacOSX([0-9]+)\\.([0-9]+).*$" "\\1" DARWIN_MAJOR_SDK_VERSION "${CMAKE_OSX_SYSROOT}")
    STRING(REGEX REPLACE ".*MacOSX([0-9]+)\\.([0-9]+).*$" "\\2" DARWIN_MINOR_SDK_VERSION "${CMAKE_OSX_SYSROOT}")
    IF(EXISTS "${CMAKE_OSX_SYSROOT}")
        SET(MACOSX_DEPLOYMENT_TARGET ${DARWIN_MAJOR_SDK_VERSION}.${DARWIN_MINOR_SDK_VERSION})
        SET(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
        MESSAGE("Setting MACOSX_DEPLOYMENT_TARGET to '${MACOSX_DEPLOYMENT_TARGET}'.")
    ENDIF()
ENDIF(APPLE AND CMAKE_OSX_SYSROOT)

########################## Options #########################
OPTION(GENERATE_TRANSLATIONS "Generate ts-files for ${PROJECT_NAME}" OFF)
OPTION(USE_CXX_STANDART "Enable tests for ${PROJECT_NAME} project" ON)
OPTION(QT_ENABLED "Enable Qt support" ON)
OPTION(CPACK_SUPPORT "Enable package support" ON)
OPTION(DEVELOPER_DYNAMIC_RUNTIME "Build with dynamic C++ runtime" ON)
OPTION(DEVELOPER_FEATURES "Use some debug tricks for developers" OFF)
OPTION(DEVELOPER_ENABLE_TESTS "Enable tests for ${PROJECT_NAME_TITLE} project" OFF)

IF(MSVC)
    IF(CMAKE_CL_64)
        OPTION(BUILD_64X "Build 64bit project" ON)
    ELSE()
        OPTION(BUILD_64X "Build 64bit project" OFF)
    ENDIF()
ELSE(MSVC)
    IF("${OS_ARCH}" STREQUAL "64")
        OPTION(BUILD_64X "Build 64bit project" ON)
    ELSEIF("${OS_ARCH}" STREQUAL "32")
        OPTION(BUILD_64X "Build 64bit project" OFF)
    ELSE()
        IF(CMAKE_SIZEOF_VOID_P MATCHES 8) 
            OPTION(BUILD_64X "Build 64bit project" ON)
        ELSE(CMAKE_SIZEOF_VOID_P MATCHES 8) 
            OPTION(BUILD_64X "Build 64bit project" OFF)
        ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)
    ENDIF()
ENDIF(MSVC)

MESSAGE(STATUS "BUILD_64X: ${BUILD_64X}")

##################################DEFAULT VALUES##########################################
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE DEBUG)
ENDIF(NOT CMAKE_BUILD_TYPE)

# If the user did not customize the install prefix,
# set it to live under build so we don't inadvertently pollute /usr/local
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

IF("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    MESSAGE(SEND_ERROR "In-source builds are not allowed.")
ENDIF("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

############################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")
INCLUDE(fasto_config) ###################
DEFINE_DEFAULT_DEFINITIONS()

# Search libraries
# use Qt
DETECT_QT()
##################### Libraries search ###################
IF(DEVELOPER_QT5)
    SET(QT_COMPONENTS_TO_USE Qt5Core Qt5Gui Qt5Widgets)
    IF(OS_MACOSX)
        SET(QT_COMPONENTS_TO_USE ${QT_COMPONENTS_TO_USE} Qt5MacExtras)
    ELSEIF(OS_LINUX)
        SET(QT_COMPONENTS_TO_USE ${QT_COMPONENTS_TO_USE} Qt5DBus) #for libqxcb
    ENDIF(OS_MACOSX)
ELSE()
    SET(QT_COMPONENTS_TO_USE QtCore QtGui)
ENDIF(DEVELOPER_QT5)

INTEGRATE_QT(${QT_COMPONENTS_TO_USE})

#use OpenSSL
FIND_PACKAGE(OpenSSL REQUIRED)
##########################################################

IF(OS_ANDROID)
    SET(ARCH_TEXT_PACKAGE ${ANDROID_NDK_ABI_NAME})
ELSE()
    IF(BUILD_64X)
        SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS 1)
    #    IF(CMAKE_COMPILER_IS_GNUCXX)
    #        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    #        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
    #        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64")
    #        SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m64")
    #        SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m64")
    #    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
        IF(OS_MACOSX)
            SET(CMAKE_OSX_ARCHITECTURES x86_64)
        ENDIF(OS_MACOSX)

        SET(ARCH_TEXT_PACKAGE x86_64)#amd64
    ELSE()
        SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS 0)
     #   IF(CMAKE_COMPILER_IS_GNUCXX)
     #       SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
     #       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
     #       SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
     #       SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
     #       SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32")
     #   ENDIF(CMAKE_COMPILER_IS_GNUCXX)
        IF(OS_MACOSX)
            SET(CMAKE_OSX_ARCHITECTURES i386)
            #-DCMAKE_OSX_ARCHITECTURES= ;i386
        ENDIF(OS_MACOSX)

        SET(ARCH_TEXT_PACKAGE i386)
    ENDIF(BUILD_64X)
ENDIF(OS_ANDROID)

IF(OS_WINDOWS)
    SET(RESOURCE_OS    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/win.rc")
    SET(RESOURCE_OS_IN "${CMAKE_SOURCE_DIR}/install/windows/winres.rc.in")
    SET(ICON_FILE      "${CMAKE_SOURCE_DIR}/install/windows/${PROJECT_NAME_LOWERCASE}.ico")
    SET(LIB_INSTALL_DESTINATION .)
    SET(TARGET_INSTALL_DESTINATION .)
ELSEIF(OS_MACOSX)
    SET(RESOURCE_OS		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME_LOWERCASE}.desktop")
    SET(RESOURCE_OS_IN	"${CMAKE_SOURCE_DIR}/install/${PROJECT_NAME_LOWERCASE}.desktop.in"	)
    SET(ICON_FILE		"${CMAKE_SOURCE_DIR}/install/osx/${PROJECT_NAME_LOWERCASE}.icns")
    SET(LIB_INSTALL_DESTINATION .)
    SET(TARGET_INSTALL_DESTINATION .)
ELSEIF(OS_LINUX)
    SET(RESOURCE_OS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME_LOWERCASE}.desktop")
    SET(RESOURCE_OS_IN "${CMAKE_SOURCE_DIR}/install/${PROJECT_NAME_LOWERCASE}.desktop.in")
    SET(ICON_FILE      "${CMAKE_SOURCE_DIR}/install/linux/${PROJECT_NAME_LOWERCASE}.png")
    SET(LIB_INSTALL_DESTINATION lib)
    SET(TARGET_INSTALL_DESTINATION bin)
ELSEIF(OS_ANDROID)
    SET(RESOURCE_OS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME_LOWERCASE}.desktop")
    SET(RESOURCE_OS_IN "${CMAKE_SOURCE_DIR}/install/${PROJECT_NAME_LOWERCASE}.desktop.in")
    SET(ICON_FILE      "${CMAKE_SOURCE_DIR}/install/android/${PROJECT_NAME_LOWERCASE}.png")
    SET(LIB_INSTALL_DESTINATION libs/${ANDROID_NDK_ABI_NAME})
    SET(TARGET_INSTALL_DESTINATION libs/${ANDROID_NDK_ABI_NAME}) #libs/armeabi-v7a
ENDIF(OS_WINDOWS)

# project sources
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

ADD_DEFINITIONS(-DPROJECT_BUILD_TYPE="${PROJECT_BUILD_TYPE}")
IF(PROJECT_BUILD_TYPE STREQUAL "release")
    ADD_DEFINITIONS(-DPROJECT_BUILD_RELEASE)
ENDIF()

ADD_SUBDIRECTORY(src)

#cpack

IF(CPACK_SUPPORT)
    SET(CPACK_PACKAGE_DESCRIPTION "Shell-centric cross-platform Redis and Memchached management tool.")
    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME_TITLE} cross-platform Redis and Memchached management tool.")
    #CPACK_DEBIAN_PACKAGE_DESCRIPTION CPACK_RPM_PACKAGE_SUMMARY
    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")#CPACK_RPM_PACKAGE_DESCRIPTION
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    SET(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    SET(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    SET(CPACK_PACKAGE_VENDOR "${PROJECT_COMPANYNAME}")#CPACK_RPM_PACKAGE_VENDOR
    SET(CPACK_PACKAGE_CONTACT "${PROJECT_MAINTAINER_NAME} <${PROJECT_MAINTAINER_MAIL}>")#CPACK_DEBIAN_PACKAGE_MAINTAINER
    
    SET(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    SET(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_SHORT})

    IF(NOT PROJECT_BUILD_TYPE STREQUAL "release")
        IF(PROJECT_VERSION_TWEAK)
            SET(CPACK_PACKAGE_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}-${PROJECT_BUILD_TYPE}${PROJECT_VERSION_TWEAK}")
        ENDIF(PROJECT_VERSION_TWEAK)
    ENDIF()

    SET(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_PATCH})#CPACK_DEBIAN_PACKAGE_VERSION CPACK_RPM_PACKAGE_VERSION
    SET(CPACK_PACKAGE_NAME ${PROJECT_NAME_LOWERCASE})#CPACK_DEBIAN_PACKAGE_NAME CPACK_RPM_PACKAGE_NAME
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${ARCH_TEXT_PACKAGE}")#out package name
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")  
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
    SET(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME_TITLE};${PROJECT_NAME}")
    MESSAGE(STATUS "CPACK_PACKAGE_FILE_NAME: ${CPACK_PACKAGE_FILE_NAME}") 
    SET(CPACK_MONOLITHIC_INSTALL ON)

    IF(OS_WINDOWS)
        IF(NOT CPACK_GENERATOR)
            SET(CPACK_GENERATOR NSIS)
        ENDIF(NOT CPACK_GENERATOR)	
        IF(CPACK_GENERATOR STREQUAL "NSIS")
            IF(BUILD_64X)
                SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
            ELSE()
                SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
            ENDIF()
            # There is a bug in NSI that does not handle full unix paths properly. Make
            # sure there is at least one set of four (4) backlasshes.
            #SET(CPACK_NSIS_MODIFY_PATH ON)
            SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/install/windows\\\\nsis-top-logo.bmp")
            SET(CPACK_NSIS_INSTALLED_ICON_NAME "\\\\${PROJECT_NAME}.exe")            
            SET(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}.exe")
            SET(CPACK_NSIS_CREATE_ICONS "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk\\\" \\\"$INSTDIR\\\\${PROJECT_NAME}.exe\\\"")
            SET(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut  \\\"$DESKTOP\\\\${PROJECT_NAME}.lnk\\\" \\\"$INSTDIR\\\\${PROJECT_NAME}.exe\\\"")
            SET(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete           \\\"$DESKTOP\\\\${PROJECT_NAME}.lnk\\\"")
            SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
            SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\${PROJECT_COMPANYNAME_DOMAIN}")
            SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\${PROJECT_DOMAIN}")
            SET(SIDEBAR_IMAGE ${CMAKE_SOURCE_DIR}/install/windows\\\\database.bmp)
            SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "!define MUI_WELCOMEFINISHPAGE_BITMAP \\\"${SIDEBAR_IMAGE}\\\"")
            SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")            
            SET(CPACK_NSIS_MUI_FINISHPAGE_RUN "..\\\\${PROJECT_NAME}.exe")
        ENDIF(CPACK_GENERATOR STREQUAL "NSIS")
    ELSEIF(OS_MACOSX)
        IF(NOT CPACK_GENERATOR)
            SET(CPACK_GENERATOR DragNDrop)
        ENDIF(NOT CPACK_GENERATOR)	
        #SET(CPACK_OSX_PACKAGE_VERSION "10.5")
    ELSEIF(OS_LINUX)
        SET(CPACK_STRIP_FILES ON)
        #SET(UBUNTU_LP_BUG 300472)
        #SET(CPACK_STRIP_FILES "bin/${PROJECT_NAME}")
        #SET(CPACK_SOURCE_STRIP_FILES "")
        IF(NOT CPACK_GENERATOR)
            SET(CPACK_GENERATOR DEB)
        ENDIF(NOT CPACK_GENERATOR)	
        IF(CPACK_GENERATOR STREQUAL "RPM" OR CPACK_GENERATOR STREQUAL "DEB")
            SET(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${PROJECT_NAME_LOWERCASE}")
        ENDIF(CPACK_GENERATOR STREQUAL "RPM" OR CPACK_GENERATOR STREQUAL "DEB")
        CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/install/linux/postinst.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/scripts/postinst" @ONLY IMMEDIATE)
        FILE(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/scripts/postinst DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} 
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/install/linux/prerm.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/scripts/prerm" @ONLY IMMEDIATE)
        FILE(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/scripts/prerm DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} 
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        IF(CPACK_GENERATOR STREQUAL "RPM")
	        #CPACK_RPM_PACKAGE_ARCHITECTURE CPACK_RPM_PACKAGE_LICENSE CPACK_RPM_PACKAGE_DESCRIPTION CPACK_RPM_SPEC_INSTALL_POST
	        #SET(CPACK_PACKAGE_RELEASE 1)
            SET(CPACK_RPM_PACKAGE_LICENSE "GPL v3")
            SET(CPACK_RPM_PACKAGE_AUTOREQPROV "no")
            SET(CPACK_RPM_PACKAGE_REQUIRES "libc.so.6 >= 2.12, libstdc++.so.6 >= 6.0.13")
            SET(CPACK_RPM_PACKAGE_RELEASE 1)
            SET(CPACK_RPM_PACKAGE_GROUP "Development/Libraries/${PROJECT_COMPANYNAME}")
            SET(CPACK_RPM_PACKAGE_ARCHITECTURE ${ARCH_TEXT_PACKAGE})
            SET(CPACK_RPM_PACKAGE_VERSION ${PROJECT_VERSION_SHORT})
            SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/postinst")
            SET(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/prerm")
        ELSEIF(CPACK_GENERATOR STREQUAL "DEB")
            CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/install/linux/fixup_deb.sh.in" "${CMAKE_CURRENT_BINARY_DIR}/fixup_deb.sh" @ONLY IMMEDIATE)
            IF(ARCH_TEXT_PACKAGE STREQUAL x86_64)
                SET(ARCH_TEXT_PACKAGE amd64)
            ENDIF()
            #SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
            #CPACK_DEBIAN_PACKAGE_HOMEPAGE The URL of the web site for this package
            #SET(CPACK_DEBIAN_PACKAGE_DEBUG ON)
	        SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.12), libstdc++6 (>= 4.4.7)")
            SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
            SET(CPACK_DEBIAN_PACKAGE_SECTION "database")#input our section
            SET(CPACK_DEBIAN_PACKAGE_RECOMMENDS "Unknown")
            SET(CPACK_DEBIAN_PACKAGE_SUGGESTS "Unknown")
            SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCH_TEXT_PACKAGE})#i386
            SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/postinst;${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/prerm")
        ENDIF(CPACK_GENERATOR STREQUAL "RPM")
    ELSEIF(OS_ANDROID)
        FIND_HOST_PROGRAM(ANDROID_DEPLOY_QT NAMES androiddeployqt PATHS ${QT_BINS_DIR})

        # Look for androiddeployqt program
        IF(NOT ANDROID_DEPLOY_QT)
            MESSAGE(FATAL_ERROR "Could not find androiddeployqt in ${QT_BINS_DIR} .")
        ENDIF(NOT ANDROID_DEPLOY_QT)

        # Set version
        SET(ANDROID_VERSION_NAME ${PROJECT_VERSION})

        # set android package source for androiddeployqt json file
        SET(PACKAGE_SOURCE_ANDROID ${CMAKE_CURRENT_BINARY_DIR}/android)
        SET(PACKAGE_OUTPUT_ANDROID ${CMAKE_INSTALL_PREFIX})
        CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/install/android/strings.xml.in ${PACKAGE_SOURCE_ANDROID}/strings.xml @ONLY)
        CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/install/android/AndroidManifest.xml.in ${PACKAGE_SOURCE_ANDROID}/AndroidManifest.xml @ONLY)

        # create json file parsed by the androiddeployqt
        SET(ANDROID_SDK $ENV{ANDROID_SDK})

        SET(ANDROID_TARGET_ARCH $ENV{ANDROID_TARGET_ARCH})
        SET(ANDROID_BUILD_TOOLS_REVISION $ENV{ANDROID_BUILD_TOOLS_REVISION})
        CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/install/android/configAndroid.json.in ${PACKAGE_SOURCE_ANDROID}/configAndroid.json @ONLY)

        SET(ANDROID_PACKAGE_RELEASE_NAME ${CPACK_PACKAGE_FILE_NAME}.apk)

        ADD_CUSTOM_COMMAND (
            OUTPUT createApkFromAndroidDeployQtRelease
            DEPENDS ${PACKAGE_SOURCE_ANDROID}/AndroidManifest.xml
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/install/android/res ${PACKAGE_SOURCE_ANDROID}/res
            COMMAND ${ANDROID_DEPLOY_QT} --output ${PACKAGE_OUTPUT_ANDROID}/ --input ${PACKAGE_SOURCE_ANDROID}/configAndroid.json --release
            COMMAND ${CMAKE_COMMAND} -E rename ${PACKAGE_OUTPUT_ANDROID}/bin/QtApp-release-unsigned.apk ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_NAME}
        )

        SET(ANDROID_PACKAGE_RELEASE_SIGNED_NAME ${CPACK_PACKAGE_FILE_NAME}_signed.apk)

        ADD_CUSTOM_COMMAND (
            OUTPUT createApkSignedFromAndroidDeployQtRelease
            DEPENDS ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_NAME} ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_NAME}
            COMMAND jarsigner -keystore ~/$ENV{ANDROID_KEYSTORE} -storepass $ENV{ANDROID_KEYSTORE_PASSWD}
            -keypass $ENV{ANDROID_KEY_PASSWD} ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_NAME} release gcompris
        )

        SET(ANDROID_PACKAGE_RELEASE_SIGNED_ALIGNED_NAME ${CPACK_PACKAGE_FILE_NAME}_signed_aligned.apk)

        ADD_CUSTOM_COMMAND (
            OUTPUT createApkSignedAlignedFromAndroidDeployQtRelease
            DEPENDS ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_NAME}
            COMMAND ${CMAKE_COMMAND} -E remove ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_ALIGNED_NAME}
            COMMAND ${ANDROID_SDK}/build-tools/${ANDROID_BUILD_TOOLS_REVISION}/zipalign -v 4 ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_NAME} ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_ALIGNED_NAME}
        )

        ADD_CUSTOM_COMMAND (
            OUTPUT apkInstall
            DEPENDS ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_ALIGNED_NAME}
            COMMAND ${ANDROID_SDK}/platform-tools/adb install -r ${PACKAGE_OUTPUT_ANDROID}/${ANDROID_PACKAGE_RELEASE_SIGNED_ALIGNED_NAME}
        )

        # Command to create apk from Makefile
        ADD_CUSTOM_TARGET(apk_release
            DEPENDS createApkFromAndroidDeployQtRelease
        )

        # Command to create signed apk from Makefile
        ADD_CUSTOM_TARGET(apk_signed
            DEPENDS createApkSignedFromAndroidDeployQtRelease
        )

        # Command to create signed aligned apk from Makefile
        ADD_CUSTOM_TARGET(apk_signed_aligned
            DEPENDS createApkSignedAlignedFromAndroidDeployQtRelease
        )

        # Command to install the signed aligned apk through adb from Makefile
        ADD_CUSTOM_TARGET(apk_install
            DEPENDS apkInstall
        )
    ENDIF(OS_WINDOWS)
    INCLUDE(CPack)
ENDIF(CPACK_SUPPORT)
