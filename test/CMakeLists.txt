include(RoqAutogen)

set(TARGET_NAME "${PROJECT_NAME}-test")

set(SOURCES
    json_agg_trade.cpp
    json_book_ticker.cpp
    json_depth_ack.cpp
    json_depth_update.cpp
    json_exchange_info_ack.cpp
    json_mark_price_update.cpp
    json_mini_ticker.cpp
    json_ticker.cpp
    main.cpp)

roq_gitignore(OUTPUT .gitignore SOURCES "${TARGET_NAME}")

add_executable("${TARGET_NAME}" ${SOURCES})

target_link_libraries("${TARGET_NAME}" "${PROJECT_NAME}-json" Catch2::Catch2)

set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS_RELEASE -s)

# run

add_test(NAME "${TARGET_NAME}" COMMAND "${TARGET_NAME}")
