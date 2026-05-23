cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

#find_package(Boost 1.78.0 REQUIRED)
#if(Boost_FOUND)
#  include_directories(${Boost_INCLUDE_DIRS})
#endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include_directories(src)

add_library(GameLib STATIC
	src/domain/model.h
	src/domain/model.cpp
	src/domain/model_properties.h
	src/infrastructure/sdk.h
	src/utils/random_generator.h
	src/utils/tagged.h
)

add_executable(game_server
	src/main.cpp
	src/utils/command_line_args.h
	src/infrastructure/http_server.h
	src/infrastructure/http_server.cpp
	src/infrastructure/request_handler.h
	src/infrastructure/request_handler.cpp
	src/infrastructure/http_utils.h
	src/infrastructure/http_utils.cpp
	src/infrastructure/http_content_type.h
	src/infrastructure/http_content_type.cpp
	src/app/app.h
	src/infrastructure/boost_json.cpp
	src/infrastructure/json_fields.h
	src/infrastructure/json_loader.h
	src/infrastructure/json_loader.cpp
	src/infrastructure/json_builder.h
	src/infrastructure/json_builder.cpp
	src/utils/logger.h
	src/utils/logger.cpp
	src/domain/loot_generator.h
	src/domain/loot_generator.cpp
	src/domain/collision_detector.h
	src/domain/collision_detector.cpp
	src/app/game_item_player_provider.h
	src/app/game_office_player_provider.h
	src/app/update_items.h
	src/app/update_items.cpp
	src/infrastructure/extra_data.h
	src/infrastructure/endpoint.h
	src/domain/geom.h
	src/infrastructure/save_manager.h
	src/infrastructure/save_manager.cpp
	src/infrastructure/serializer.h
	src/infrastructure/serializer.cpp
	src/app/database.h
)

add_executable(loot_generator_test
	tests/loot_generator_tests.cpp
	src/domain/loot_generator.h
	src/domain/loot_generator.cpp
)

add_executable(collision_detector_test
	tests/collision-detector-tests.h
	tests/collision-detector-tests.cpp
	src/domain/collision_detector.h
	src/domain/collision_detector.cpp
)

add_executable(http_utils_tests
	tests/http_utils_tests.cpp
	src/infrastructure/http_utils.h
	src/infrastructure/http_utils.cpp
)

add_executable(serialization_tests
	tests/serialization_tests.cpp
	src/infrastructure/save_manager.h
	src/infrastructure/save_manager.cpp
	src/infrastructure/serializer.h
	src/infrastructure/json_loader.h
	src/infrastructure/json_loader.cpp
)

target_link_libraries(game_server PUBLIC GameLib PRIVATE Threads::Threads PUBLIC CONAN_PKG::boost PRIVATE CONAN_PKG::libpqxx)
target_link_libraries(loot_generator_test PRIVATE CONAN_PKG::catch2)
target_link_libraries(collision_detector_test PRIVATE CONAN_PKG::catch2)
target_link_libraries(http_utils_tests PRIVATE CONAN_PKG::catch2)
target_link_libraries(serialization_tests PRIVATE CONAN_PKG::catch2 PRIVATE CONAN_PKG::boost PUBLIC GameLib)
