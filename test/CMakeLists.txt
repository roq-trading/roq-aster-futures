include(RoqAutogen)

set(TARGET_NAME ${PROJECT_NAME}-test)

set(SOURCES
    # rest
    json_depth_ack.cpp
    json_exchange_info_ack.cpp
    # ws
    json_agg_trade.cpp
    json_book_ticker.cpp
    json_depth_update.cpp
    json_mark_price_update.cpp
    json_mini_ticker.cpp
    json_ticker.cpp
    # rest
    json_account_ack.cpp
    json_listen_key_ack.cpp
    json_open_orders_ack.cpp
    json_user_trades_ack.cpp
    json_place_order_ack.cpp
    json_cancel_order_ack.cpp
    # ws
    json_order_trade_update.cpp
    main.cpp)

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME})

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} ${PROJECT_NAME}-protocol-json roq-server::roq-server Catch2::Catch2)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
