set(
  SOURCES
  audio_json.cpp
  BaseMuteAction.cpp
  DefaultAudioDevices.cpp
  MuteAction.cpp
  AudioMuteStreamDeckPlugin.cpp
  ToggleMuteAction.cpp
  UnmuteAction.cpp
  main.cpp
)

if(WIN32)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    @ONLY
  )

  list(
    APPEND SOURCES
    PlayWavFileWindows.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
  )
endif()

if(APPLE)
  list(
    APPEND SOURCES
    PlayWavFileMacOS.cpp
  )
endif()

add_executable(
  audiotoggle
  ${SOURCES}
)

target_link_libraries(audiotoggle StreamDeckSDK AudioDeviceLib)

if(APPLE)
  find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
  target_link_libraries(
    audiotoggle
    ${AUDIOTOOLBOX_FRAMEWORK}
  )
elseif(WIN32)
  target_link_libraries(audiotoggle Winmm)
endif()

target_compile_definitions(audiotoggle PRIVATE -DHAVE_FEEDBACK_SOUNDS=1)
install(TARGETS audiotoggle DESTINATION ${CMAKE_INSTALL_PREFIX})
sign_target(audiotoggle)

if(WIN32)
  install(FILES "$<TARGET_PDB_FILE:audiotoggle>" DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
