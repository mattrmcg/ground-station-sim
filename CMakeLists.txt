cmake_minimum_required(VERSION 3.12)

project(udp_ground_station)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ground_station 
    apps/ground_station.cpp
    net/udp_link.cpp
)

add_executable(vehicle_sim
    apps/vehicle_sim.cpp
    net/udp_link.cpp
    )
