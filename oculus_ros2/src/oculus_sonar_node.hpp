#include <iostream>
#include <sstream>
#include <thread>
#include <future>

#include "rclcpp/rclcpp.hpp"

#include "conversions.h"

#include <oculus_driver/AsyncService.h>
#include <oculus_driver/SonarDriver.h>

#include "oculus_interfaces/msg/oculus_status.hpp"
#include "oculus_interfaces/msg/ping.hpp"

#include "rcl_interfaces/msg/parameter_descriptor.hpp"

class OculusSonarNode : public rclcpp::Node
{
public:
  OculusSonarNode();
  ~OculusSonarNode();

protected:
  const std::vector<std::string> parameters_names{"frame_id", "frequency_mode", "ping_rate", "data_depth", "nbeams", "send_gain", "gain_assist", "range", "gamma_correction", "gain_percent", "sound_speed", "use_salinity", "salinity"};
  oculus::SonarDriver::PingConfig currentConfig;

private:
  std::shared_ptr<oculus::SonarDriver> sonar_driver_;
  oculus::AsyncService io_service_;

  std::string ping_topic_ = "ping";
  std::string status_topic_ = "status";
  rclcpp::Publisher<oculus_interfaces::msg::OculusStatus>::SharedPtr status_publisher_{nullptr};
  rclcpp::Publisher<oculus_interfaces::msg::Ping>::SharedPtr ping_publisher_{nullptr};

  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_{nullptr};

  rcl_interfaces::msg::SetParametersResult set_config_callback(const std::vector<rclcpp::Parameter> &parameters);

  void publish_status(const OculusStatusMsg &status);
  void publish_ping(const oculus::PingMessage::ConstPtr &pingMetadata);
  void handle_dummy();
};
