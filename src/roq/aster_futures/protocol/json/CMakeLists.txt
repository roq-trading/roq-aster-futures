set(TARGET_NAME ${PROJECT_NAME}-protocol-json)

set(SOURCES encoder.cpp map.cpp parser.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    account_ack.json
    account_ack_assets_item.json
    account_ack_positions_item.json
    account_config_update.json
    account_config_update_margin_mode.json
    account_config_update_symbol_leverage.json
    account_update.json
    account_update_data.json
    account_update_data_balances_item.json
    account_update_data_positions_item.json
    agg_trade.json
    book_ticker.json
    cancel_all_orders_ack.json
    cancel_order_ack.json
    contract_status.json
    contract_type.json
    depth_ack.json
    depth_update.json
    depth_update_item.json
    event_reason_type.json
    event_type.json
    exchange_info_ack.json
    exchange_info_ack_symbols_item.json
    exchange_info_ack_symbols_item_filters_item.json
    execution_type.json
    filter_type.json
    force_order.json
    force_order_order.json
    futures_type.json
    listen_key_ack.json
    listen_key_expired.json
    margin_call.json
    margin_call_positions_item.json
    margin_type.json
    mark_price_update.json
    mini_ticker.json
    modify_order_ack.json
    open_orders_ack.json
    open_orders_ack_item.json
    order_status.json
    order_trade_update.json
    order_trade_update_order.json
    order_type.json
    place_order_ack.json
    position_side.json
    side.json
    stp_mode.json
    ticker.json
    time_in_force.json
    underlying_type.json
    user_trades_ack.json
    working_type.json
    #
    ack.json
    pong.json)

roq_autogen_hpp(
  OUTPUT
  AUTOGEN_HEADERS
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

add_custom_target(protocol-json-autogen-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen_cpp(
  OUTPUT
  AUTOGEN_SOURCES
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME} ${AUTOGEN_HEADERS} ${AUTOGEN_SOURCES})

add_library(${TARGET_NAME} OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies(${TARGET_NAME} protocol-json-autogen-headers)

target_link_libraries(${TARGET_NAME} PRIVATE roq-core::roq-core roq-codec::roq-codec roq-utils::roq-utils roq-logging::roq-logging fmt::fmt)
