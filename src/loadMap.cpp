#include "loadMap.h"
#include <thread>

using namespace std::chrono_literals;

LoadMap::LoadMap(std::shared_ptr<rclcpp::Client<nav2_msgs::srv::LoadMap>> client_load_map)
{
  m_client_load_map = client_load_map;
}


void LoadMap::startLoadMap(std::chrono::seconds timeout ){

   auto request = std::make_shared<nav2_msgs::srv::LoadMap::Request>();
  request->map_url = "/home/jakob/Documents/ros2_ws/src/edu_robocub_rescue_stack/config/map_ramp_start.yaml";


    if (!m_client_load_map->wait_for_service(timeout))
    {
      std::cout<<"Service "<<m_client_load_map->get_service_name() << "not available" << std::endl;
      return;
    }

   auto result = m_client_load_map->async_send_request(request, std::bind(&LoadMap::loadMap_callback, this, std::placeholders::_1));
}

void LoadMap::loadMap_callback(const rclcpp::Client<nav2_msgs::srv::LoadMap>::SharedFuture future)
{
  if(future.get()->result== nav2_msgs::srv::LoadMap::Response::RESULT_SUCCESS){
    std::cout << "load map successful" <<std::endl;
  }else{
    std::cout <<  "load map not successful" <<std::endl;
  }
}