set(TARGET_NAME "${PROJECT_NAME}-json")

set(SOURCES encoder.cpp map.cpp parser.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    subscribe.json
    account_data_item_coin_item.json
    account_data_item.json
    account.json
    account_mode.json
    action.json
    arg.json
    asset_mode.json
    cancel_all_orders_ack_data.json
    cancel_all_orders_ack_data_list_item.json
    cancel_all_orders_ack.json
    cancel_order_ack_data.json
    cancel_order_ack.json
    error.json
    event.json
    fill_data_item_fee_detail_item.json
    fill_data_item.json
    fill.json
    get_market_summary_ack_item.json
    get_market_summary_ack.json
    get_open_orders_ack.json
    get_position_mode_ack_item.json
    get_position_mode_ack.json
    get_positions_ack.json
    get_wallet_ack_item_assets_item.json
    get_wallet_ack_item.json
    get_wallet_ack.json
    hold_mode.json
    hold_side.json
    login.json
    margin_mode.json
    modify_order_ack_data.json
    modify_order_ack.json
    order_data_item_fee_detail_item.json
    order_data_item.json
    order.json
    order_source.json
    order_status.json
    order_type.json
    place_order_ack_data.json
    place_order_ack.json
    pong.json
    position_data_item.json
    position.json
    position_status.json
    pos_mode.json
    pos_side.json
    side.json
    snapshot_l1_data_item.json
    snapshot_l1_data.json
    snapshot_l1.json
    stp_mode.json
    time_in_force.json
    topic.json
    trade_history_data_item.json
    trade_history.json
    trade_scope.json
    trade_side.json
    type.json
    update_data_item.json
    update_data.json
    update.json)

roq_autogen_hpp(
  OUTPUT
  AUTOGEN_HEADERS
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

add_custom_target(autogen-json-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen_cpp(
  OUTPUT
  AUTOGEN_SOURCES
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

roq_gitignore(OUTPUT .gitignore SOURCES "${TARGET_NAME}" ${AUTOGEN_HEADERS} ${AUTOGEN_SOURCES})

add_library("${TARGET_NAME}" OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies("${TARGET_NAME}" autogen-json-headers)

target_link_libraries("${TARGET_NAME}" PRIVATE roq-core::roq-core roq-codec::roq-codec roq-utils::roq-utils roq-logging::roq-logging fmt::fmt)
