set(TARGET_NAME ${PROJECT_NAME}-protocol-json)

set(SOURCES encoder.cpp map.cpp parser.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    working_type.json
    account_data_item_coin_item.json
    account_data_item.json
    account_ack_assets_item.json
    account_ack_positions_item.json
    account_ack.json
    account.json
    account_mode.json
    ack_error.json
    ack.json
    ack_result.json
    ack_result_status.json
    action.json
    agg_trade.json
    arg.json
    asset_mode.json
    book_ticker.json
    cancel_all_orders_ack_data.json
    cancel_all_orders_ack_data_list_item.json
    cancel_all_orders_ack.json
    cancel_order_ack.json
    category.json
    category_ws.json
    contract_type.json
    delivery_period.json
    depth_ack.json
    depth_update_item.json
    depth_update.json
    event.json
    event_type.json
    exchange_info_ack.json
    exchange_info_ack_symbols_item_filters_item.json
    exchange_info_ack_symbols_item.json
    fill_data_item_fee_detail_item.json
    fill_data_item.json
    user_trades_ack.json
    listen_key_ack.json
    fill.json
    filter_type.json
    futures_type.json
    hold_mode.json
    hold_side.json
    login.json
    margin_mode.json
    mark_price_update.json
    message_field.json
    message_type.json
    mini_ticker.json
    modify_order_ack_data.json
    modify_order_ack.json
    open_orders_ack_item.json
    open_orders_ack.json
    order_trade_update_order.json
    order_trade_update.json
    order_source.json
    order_status.json
    order_type.json
    place_order_ack.json
    pong.json
    position_data_item.json
    position.json
    position_status.json
    pos_mode.json
    position_side.json
    pos_side.json
    side.json
    status.json
    stp_mode.json
    symbol_status.json
    symbol_type.json
    ticker.json
    time_in_force.json
    topic.json
    trade_scope.json
    trade_side.json
    trading_status.json
    type.json)

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
