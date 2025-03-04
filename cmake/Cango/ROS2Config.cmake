include_guard()

macro(ROS2Config)
    set(ROS_NAME jazzy)
    set(ROS_ROOT /opt/ros/${ROS_NAME})
    set(ROS_LIBS_DIR ${ROS_ROOT}/lib)
    set(ROS_INCLUDE_DIR ${ROS_ROOT}/include)
    function(find_ros_target target)
        set(target_name ros_${target})
        set(target_alias ros::${target})
        add_library(${target_name} SHARED IMPORTED)
        add_library(${target_alias} ALIAS ${target_name})
        set_target_properties(${target_name} PROPERTIES
            IMPORTED_LOCATION ${ROS_LIBS_DIR}/lib${target}.so
            INTERFACE_INCLUDE_DIRECTORIES ${ROS_INCLUDE_DIR}/${target}
        )
    endfunction()
    function(find_ros_library target)
        set(target_name ros_${target})
        set(target_alias ros::${target})
        add_library(${target_name} SHARED IMPORTED)
        add_library(${target_alias} ALIAS ${target_name})
        set_target_properties(${target_name} PROPERTIES
            IMPORTED_LOCATION ${ROS_LIBS_DIR}/lib${target}.so
        )
    endfunction()
    function(find_my_ros_library target path)
        set(target_name ly_ros_${target})
        set(target_alias ly::ros::${target})
        add_library(${target_name} SHARED IMPORTED)
        add_library(${target_alias} ALIAS ${target_name})
        set_target_properties(${target_name} PROPERTIES
            IMPORTED_LOCATION ${path}/lib/lib${target}.so
        )
        file(COPY ${path}/lib/lib${target}.so DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endfunction()
    function(include_ros_directory target)
        include_directories(${ROS_INCLUDE_DIR}/${target})
    endfunction()
    function(include_my_ros_directory target path)
        include_directories(${path}/include/${target})
        message(STATUS "为消息(${target})包含头文件：${path}/${target}/include/${target}")
    endfunction()


    set(ROS_MESSAGE_LIBRARIES "")
    macro(find_ros_message target)
        include_ros_directory(${target})
        find_ros_library(${target}__rosidl_generator_c)
        find_ros_library(${target}__rosidl_generator_py)
        find_ros_library(${target}__rosidl_typesupport_cpp)
        find_ros_library(${target}__rosidl_typesupport_c)
        find_ros_library(${target}__rosidl_typesupport_fastrtps_cpp)
        find_ros_library(${target}__rosidl_typesupport_fastrtps_c)
        find_ros_library(${target}__rosidl_typesupport_introspection_cpp)
        list(APPEND ROS_MESSAGE_LIBRARIES
            ros::${target}__rosidl_generator_c
            ros::${target}__rosidl_generator_py
            ros::${target}__rosidl_typesupport_cpp
            ros::${target}__rosidl_typesupport_c
            ros::${target}__rosidl_typesupport_fastrtps_cpp
            ros::${target}__rosidl_typesupport_fastrtps_c
            ros::${target}__rosidl_typesupport_introspection_cpp
        )
    endmacro()

    macro(find_my_ros_message target path)
        include_my_ros_directory(${target} ${path}/${target})
        find_my_ros_library(${target}__rosidl_generator_c ${path}/${target})
        find_my_ros_library(${target}__rosidl_generator_py ${path}/${target})
        find_my_ros_library(${target}__rosidl_typesupport_cpp ${path}/${target})
        find_my_ros_library(${target}__rosidl_typesupport_c ${path}/${target})
        find_my_ros_library(${target}__rosidl_typesupport_fastrtps_cpp ${path}/${target})
        find_my_ros_library(${target}__rosidl_typesupport_fastrtps_c ${path}/${target})
        find_my_ros_library(${target}__rosidl_typesupport_introspection_cpp ${path}/${target})
        list(APPEND ROS_MESSAGE_LIBRARIES
            ly::ros::${target}__rosidl_generator_c
            ly::ros::${target}__rosidl_generator_py
            ly::ros::${target}__rosidl_typesupport_cpp
            ly::ros::${target}__rosidl_typesupport_c
            ly::ros::${target}__rosidl_typesupport_fastrtps_cpp
            ly::ros::${target}__rosidl_typesupport_fastrtps_c
            ly::ros::${target}__rosidl_typesupport_introspection_cpp
        )
    endmacro()

    find_package(fmt CONFIG REQUIRED)
    find_package(spdlog CONFIG REQUIRED)
    find_package(frozen CONFIG REQUIRED)
    find_package(matchit CONFIG REQUIRED)
    find_package(Boost REQUIRED COMPONENTS system filesystem)
    find_package(Python REQUIRED COMPONENTS Development Interpreter)
    find_package(pybind11 CONFIG REQUIRED)
    find_package(Threads REQUIRED)
    find_package(OpenCV 4 CONFIG REQUIRED)
    find_package(Eigen3 CONFIG REQUIRED)
    find_package(Sophus CONFIG REQUIRED)
    find_package(Ceres CONFIG REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(OpenVINO CONFIG REQUIRED)

    macro(find_ros_idl_libraries)
        include_ros_directory(rosidl_runtime_cpp)
        include_ros_directory(rosidl_typesupport_interface)
        find_ros_target(rosidl_runtime_c)
        find_ros_target(rosidl_typesupport_cpp)
        find_ros_target(rosidl_typesupport_c)
        find_ros_target(rosidl_typesupport_fastrtps_cpp)
        find_ros_target(rosidl_typesupport_fastrtps_c)
        find_ros_target(rosidl_typesupport_introspection_cpp)
        find_ros_target(rosidl_typesupport_introspection_c)
        find_ros_target(rosidl_dynamic_typesupport_fastrtps)
        find_ros_target(rosidl_dynamic_typesupport)
        set(ROS_IDL_LIBRARIES
            ros::rosidl_runtime_c
            ros::rosidl_typesupport_cpp
            ros::rosidl_typesupport_c
            ros::rosidl_typesupport_fastrtps_cpp
            ros::rosidl_typesupport_fastrtps_c
            ros::rosidl_typesupport_introspection_cpp
            ros::rosidl_typesupport_introspection_c
            ros::rosidl_dynamic_typesupport_fastrtps
            ros::rosidl_dynamic_typesupport
        )
    endmacro()

    macro(find_ros_rcl_libraries)
        include_ros_directory(rcl_interfaces)
        find_ros_target(rcl)
        find_ros_target(rcl_action)
        find_ros_target(rcl_lifecycle)
        find_ros_target(rclcpp)
        find_ros_target(rclcpp_action)
        find_ros_target(rclcpp_lifecycle)
        find_ros_target(rcl_logging_interface)
        #find_ros_target(rcl_logging_spdlog)
        find_ros_target(rcl_yaml_param_parser)
        set(ROS_RCL_LIBRARIES
            ros::rcl
            ros::rcl_action
            ros::rcl_lifecycle
            ros::rclcpp
            ros::rclcpp_action
            ros::rclcpp_lifecycle
            ros::rcl_logging_interface
            #ros::rcl_logging_spdlog
            ros::rcl_yaml_param_parser
        )
    endmacro()

    macro(find_ros_rmw_libraries)
        find_ros_target(rmw)
        find_ros_target(rmw_fastrtps_cpp)
        find_ros_target(rmw_fastrtps_shared_cpp)
        #find_ros_target(rmw_implementation)
        find_ros_target(rmw_dds_common)
        set(ROS_RMW_LIBRARIES
            ros::rmw
            ros::rmw_fastrtps_cpp
            ros::rmw_fastrtps_shared_cpp
            #ros::rmw_implementation
            ros::rmw_dds_common
        )
    endmacro()

    macro(find_ros_tracetools_libraries)
        find_ros_target(tracetools_image_pipeline)
        find_ros_target(tracetools)
        find_ros_library(tracetools_status)
        set(ROS_TRACETOOLS_LIBRARIES
            ros::tracetools_image_pipeline
            ros::tracetools
            ros::tracetools_status
        )
    endmacro()

    macro(find_ros_image_transport_libraries)
        find_ros_target(image_transport)
        find_ros_library(image_transport_plugins)
        set(ROS_IMAGE_TRANSPORT_LIBRARIES
            ros::image_transport
            ros::image_transport_plugins
        )
    endmacro()
endmacro()