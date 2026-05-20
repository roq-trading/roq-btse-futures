include(RoqAutogen)

set(TARGET_NAME ${PROJECT_NAME}-test)

set(SOURCES
    json_all_position.cpp
    json_cancel_order_ack.cpp
    json_error.cpp
    json_fills.cpp
    json_get_market_summary_ack.cpp
    json_get_open_orders_ack.cpp
    json_get_position_mode_ack.cpp
    json_get_positions_ack.cpp
    json_get_wallet_ack.cpp
    json_modify_order_ack.cpp
    json_notification.cpp
    json_place_order_ack.cpp
    json_pong.cpp
    json_positions.cpp
    json_snapshot_l1.cpp
    json_subscribe.cpp
    json_trade_history.cpp
    json_update.cpp
    main.cpp)

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME})

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} ${PROJECT_NAME}-json roq-server::roq-server Catch2::Catch2)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
