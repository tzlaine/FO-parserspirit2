cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6.4)

project(parser_tests)

message("-- Configuring parser_test")

set(THIS_EXE_SOURCES
    ../../combat/CombatOrder.cpp
    ../../combat/OpenSteer/AsteroidBeltObstacle.cpp
    ../../combat/OpenSteer/CombatFighter.cpp
    ../../combat/OpenSteer/CombatObject.cpp
    ../../combat/OpenSteer/CombatShip.cpp
    ../../combat/OpenSteer/Missile.cpp
    ../../combat/OpenSteer/Obstacle.cpp
    ../../combat/OpenSteer/PathingEngine.cpp
    ../../combat/OpenSteer/SimpleVehicle.cpp
    ../../combat/OpenSteer/Vec3.cpp
    ../../combat/OpenSteer/Vec3Utilities.cpp
    ../../Empire/Empire.cpp
    ../../Empire/EmpireManager.cpp
    ../../Empire/ResourcePool.cpp
    ../../network/Message.cpp
    ../../network/MessageQueue.cpp
    ../../network/Networking.cpp
    ../../UI/StringTable.cpp
    ../../universe/Building.cpp
    ../../universe/Condition.cpp
    ../../universe/ConditionParser1.cpp
    ../../universe/ConditionParser2.cpp
    ../../universe/ConditionParser3.cpp
    ../../universe/ConditionParser.cpp
    ../../universe/Effect.cpp
    ../../universe/EffectAccounting.cpp
    ../../universe/EffectParser.cpp
    ../../universe/Enums.cpp
    ../../universe/Fleet.cpp
    ../../universe/Meter.cpp
    ../../universe/ObjectMap.cpp
    ../../universe/ParserUtil.cpp
    ../../universe/Planet.cpp
    ../../universe/PopCenter.cpp
    ../../universe/Predicates.cpp
    ../../universe/ResourceCenter.cpp
    ../../universe/Ship.cpp
    ../../universe/ShipDesign.cpp
    ../../universe/Special.cpp
    ../../universe/Species.cpp
    ../../universe/System.cpp
    ../../universe/Tech.cpp
    ../../universe/TopLevelParsers.cpp
    ../../universe/UniverseObject.cpp
    ../../universe/ValueRef.cpp
    ../../universe/ValueRefParser.cpp
    ../../util/DataTable.cpp
    ../../util/GZStream.cpp
    ../../util/Math.cpp
    ../../util/MultiplayerCommon.cpp
    ../../util/OptionsDB.cpp
    ../../util/Order.cpp
    ../../util/OrderSet.cpp
    ../../util/Process.cpp
    ../../util/Random.cpp
    ../../util/SerializeEmpire.cpp
    ../../util/SerializeMultiplayerCommon.cpp
    ../../util/SerializeOrderSet.cpp
    ../../util/SerializePathingEngine.cpp
    ../../util/SerializeUniverse.cpp
    ../../util/SitRepEntry.cpp
    ../../util/VarText.cpp
    ../../util/binreloc.c
    ../../util/Directories.cpp
    ../../util/XMLDoc.cpp
    ../../log4cpp/src/Appender.cpp
    ../../log4cpp/src/AppenderSkeleton.cpp
    ../../log4cpp/src/BasicLayout.cpp
    ../../log4cpp/src/Category.cpp
    ../../log4cpp/src/CategoryStream.cpp
    ../../log4cpp/src/Configurator.cpp
    ../../log4cpp/src/FactoryParams.cpp
    ../../log4cpp/src/FileAppender.cpp
    ../../log4cpp/src/HierarchyMaintainer.cpp
    ../../log4cpp/src/LayoutAppender.cpp
    ../../log4cpp/src/Localtime.cpp
    ../../log4cpp/src/LoggingEvent.cpp
    ../../log4cpp/src/NDC.cpp
    ../../log4cpp/src/PatternLayout.cpp
    ../../log4cpp/src/Priority.cpp
    ../../log4cpp/src/StringUtil.cpp
    ../../log4cpp/src/TimeStamp.cpp
    ../../combat/CombatSystem.cpp
    ../../network/ServerNetworking.cpp
    ../../server/SaveLoad.cpp
    ../../server/ServerApp.cpp
    ../../server/ServerFSM.cpp
    ../../universe/Universe.cpp
    ../../util/AppInterface.cpp

#    ../../universe/ValueRef_2.cpp
#    ../../universe/Condition_2.cpp
    ../../universe/Names.cpp

    test.cpp
    lexer_test_rules.cpp

    ../Lexer.cpp
    ../EnumParser.cpp
    ../IntValueRefParser.cpp
    ../DoubleValueRefParser.cpp
    ../StringValueRefParser.cpp
    ../PlanetSizeValueRefParser.cpp
    ../PlanetTypeValueRefParser.cpp
    ../PlanetEnvironmentValueRefParser.cpp
    ../UniverseObjectTypeValueRefParser.cpp
    ../StarTypeValueRefParser.cpp

    ../ConditionParser.cpp
    ../ConditionParser1.cpp
    ../ConditionParser2.cpp
    ../ConditionParser3.cpp

    ../EffectParser.cpp

    ../Parse.cpp
    ../BuildingsParser.cpp
    ../SpecialsParser.cpp
    ../SpeciesParser.cpp
    ../TechsParser.cpp
    ../ItemsParser.cpp
    ../ShipPartsParser.cpp

    ${CMAKE_BINARY_DIR}/util/Version.cpp
)

if (WIN32)
    list(APPEND THIS_EXE_SOURCES ../../log4cpp/src/MSThreads.cpp)
else ()
    list(APPEND THIS_EXE_SOURCES ../../log4cpp/src/PThreads.cpp)
endif ()

add_definitions(-DFREEORION_BUILD_SERVER -DADOBE_STD_SERIALIZATION)

if (WIN32)
    link_directories(${BOOST_LIBRARYDIR})
    set(THIS_EXE_LINK_LIBS ${GIGI_GIGI_LIBRARY} ${ZLIB_LIBRARY})
else ()
    set(THIS_EXE_LINK_LIBS ${GIGI_GIGI_LIBRARY} ${Boost_LIBRARIES} ${ZLIB_LIBRARY})
endif ()

executable_all_variants(parser_test)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE)
    set_target_properties(parser_test
        PROPERTIES
        COMPILE_DEFINITIONS BOOST_ALL_DYN_LINK
        LINK_FLAGS /NODEFAULTLIB:LIBCMT
    )
endif ()

macro (add_test_and_data_files name)
    set(suffix "")
    set(args "")
    foreach (arg ${ARGN})
        if (EXISTS ${CMAKE_SOURCE_DIR}/parse/test/${arg})
            get_filename_component(filename_without_path ${arg} NAME)
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${arg}
                           ${CMAKE_CURRENT_BINARY_DIR}/${filename_without_path}
                           COPYONLY)
            set(suffix ${suffix}-${filename_without_path})
            set(args "${args};${filename_without_path}")
        else ()
            set(suffix ${suffix}-${arg})
            set(args "${args};${arg}")
        endif ()
    endforeach ()
    add_test (${name}${suffix} ${CMAKE_BINARY_DIR}/${name} ${args})
endmacro ()

add_test_and_data_files(parser_test lexer -f lexer_tokens)
add_test_and_data_files(parser_test int_value_ref_parser -f int_arithmetic)
add_test_and_data_files(parser_test int_value_ref_parser -f int_variable)
add_test_and_data_files(parser_test int_value_ref_parser -f int_variable_arithmetic)
add_test_and_data_files(parser_test int_value_ref_parser -f int_statistic)
add_test_and_data_files(parser_test double_value_ref_parser -f double_arithmetic)
add_test_and_data_files(parser_test double_value_ref_parser -f double_variable)
add_test_and_data_files(parser_test double_value_ref_parser -f double_variable_arithmetic)
add_test_and_data_files(parser_test double_value_ref_parser -f double_statistic)
add_test_and_data_files(parser_test double_value_ref_parser -f int_arithmetic)
add_test_and_data_files(parser_test double_value_ref_parser -f int_variable)
add_test_and_data_files(parser_test double_value_ref_parser -f int_variable_arithmetic)
add_test_and_data_files(parser_test double_value_ref_parser -f int_statistic)
add_test_and_data_files(parser_test string_value_ref_parser -f string_constant)
add_test_and_data_files(parser_test string_value_ref_parser -f string_variable)
add_test_and_data_files(parser_test string_value_ref_parser -f int_variable)
add_test_and_data_files(parser_test string_value_ref_parser -f int_statistic)
add_test_and_data_files(parser_test string_value_ref_parser -f double_variable)
add_test_and_data_files(parser_test string_value_ref_parser -f double_statistic)
add_test_and_data_files(parser_test planet_size_value_ref_parser -f planet_size_constant)
add_test_and_data_files(parser_test planet_size_value_ref_parser -f planet_size_variable)
add_test_and_data_files(parser_test planet_size_value_ref_parser -f planet_size_statistic)
add_test_and_data_files(parser_test planet_type_value_ref_parser -f planet_type_constant)
add_test_and_data_files(parser_test planet_type_value_ref_parser -f planet_type_variable)
add_test_and_data_files(parser_test planet_type_value_ref_parser -f planet_type_statistic)
add_test_and_data_files(parser_test planet_environment_value_ref_parser -f planet_environment_constant)
add_test_and_data_files(parser_test planet_environment_value_ref_parser -f planet_environment_variable)
add_test_and_data_files(parser_test planet_environment_value_ref_parser -f planet_environment_statistic)
add_test_and_data_files(parser_test universe_object_type_value_ref_parser -f universe_object_type_constant)
add_test_and_data_files(parser_test universe_object_type_value_ref_parser -f universe_object_type_variable)
add_test_and_data_files(parser_test universe_object_type_value_ref_parser -f universe_object_type_statistic)
add_test_and_data_files(parser_test star_type_value_ref_parser -f star_type_constant)
add_test_and_data_files(parser_test star_type_value_ref_parser -f star_type_variable)
add_test_and_data_files(parser_test star_type_value_ref_parser -f star_type_statistic)
add_test_and_data_files(parser_test int_value_ref_evaluation -f int_arithmetic_evaluated)
add_test_and_data_files(parser_test double_value_ref_evaluation -f double_arithmetic_evaluated)
add_test_and_data_files(parser_test condition_parser -f condition_parser_1)
add_test_and_data_files(parser_test condition_parser -f condition_parser_2)
add_test_and_data_files(parser_test condition_parser -f condition_parser_3)
add_test_and_data_files(parser_test effect_parser -f effect_parser)
