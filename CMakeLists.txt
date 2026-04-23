if(CONFIG_SHIELD_TORABO_TSUKI_LP_LEFT OR CONFIG_SHIELD_TORABO_TSUKI_LP_RIGHT OR CONFIG_SHIELD_TORABO_TSUKI_LP_DONGLE)
  zephyr_library()
  zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)

  if(CONFIG_SHIELD_TORABO_TSUKI_LP_LEFT OR CONFIG_SHIELD_TORABO_TSUKI_LP_RIGHT)
    zephyr_library_sources(src/board.c src/mini_trackpad_init_reg.c)
  endif()

  zephyr_library_sources(src/input_processor_dominant_axis.c)
endif()