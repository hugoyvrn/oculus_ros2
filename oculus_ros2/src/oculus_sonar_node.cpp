using namespace std;

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include "oculus_sonar_node.hpp"

using SonarDriver = oculus::SonarDriver;

OculusSonarNode::OculusSonarNode() : Node("oculus_sonar")
{

    if (!this->has_parameter("frame_id"))
    {
        this->declare_parameter<string>("frame_id", "sonar");
    }
    if (!this->has_parameter("frequency_mode"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::IntegerRange range;
        range.set__from_value(1).set__to_value(2).set__step(1);
        param_desc.name = "frequency_mode";
        param_desc.type = rclcpp::ParameterType::PARAMETER_INTEGER;
        param_desc.description = "Sonar beam frequency mode.\n\t1: Low frequency (1.2MHz, wide aperture).\n\t2: High frequency (2.1Mhz, narrow aperture).";
        param_desc.integer_range = {range};
        this->declare_parameter<int>("frequency_mode", 1, param_desc);
    }
    if (!this->has_parameter("ping_rate"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::IntegerRange range;
        range.set__from_value(0).set__to_value(5).set__step(1);
        param_desc.name = "ping_rate";
        param_desc.type = rclcpp::ParameterType::PARAMETER_INTEGER;
        param_desc.description = "Frequency of ping fires.\n\t0: 10Hz max ping rate.\n\t1: 15Hz max ping rate.\n\t2: 40Hz max ping rate.\n\t3: 5Hz max ping rate.\n\t4: 2Hz max ping rate.\n\t5: Standby mode (no ping fire).";
        param_desc.integer_range = {range};
        this->declare_parameter<int>("ping_rate", 0, param_desc);
    }
    if (!this->has_parameter("data_depth"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::IntegerRange range;
        range.set__from_value(0).set__to_value(1).set__step(1);
        param_desc.name = "data_depth";
        param_desc.type = rclcpp::ParameterType::PARAMETER_INTEGER;
        param_desc.description = "Ping data encoding bit count.\n\t0: Ping data encoded on 8bits.\n\t1: Ping data encoded on 16bits.";
        param_desc.integer_range = {range};
        this->declare_parameter<int>("data_depth", 0, param_desc);
    }
    if (!this->has_parameter("nbeams"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::IntegerRange range;
        range.set__from_value(0).set__to_value(1).set__step(1);
        param_desc.name = "nbeams";
        param_desc.type = rclcpp::ParameterType::PARAMETER_INTEGER;
        param_desc.description = "Number of ping beams.\n\t0: Oculus outputs 256 beams.\n\t1: Oculus outputs 512 beams.";
        param_desc.integer_range = {range};
        this->declare_parameter<int>("nbeams", 0, param_desc);
    }
    if (!this->has_parameter("send_gain"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        param_desc.name = "send_gain";
        param_desc.type = rclcpp::ParameterType::PARAMETER_BOOL;
        param_desc.description = "Send range gain with data.";
        this->declare_parameter<bool>("send_gain", false);
    }
    if (!this->has_parameter("gain_assist"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        param_desc.name = "gain_assist";
        param_desc.type = rclcpp::ParameterType::PARAMETER_BOOL;
        param_desc.description = "Enable auto gain.";
        this->declare_parameter<bool>("gain_assist", false, param_desc);
    }
    if (!this->has_parameter("range"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::FloatingPointRange range;
        range.set__from_value(0.3).set__to_value(40.0).set__step(0.1);
        param_desc.name = "nbeams";
        param_desc.type = rclcpp::ParameterType::PARAMETER_DOUBLE;
        param_desc.description = "Sonar range (in meters), min=0.3, max=40.0.";
        param_desc.floating_point_range = {range};
        this->declare_parameter<double>("range", 40., param_desc);
    }
    if (!this->has_parameter("gamma_correction"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::IntegerRange range;
        range.set__from_value(0).set__to_value(255).set__step(1);
        param_desc.name = "gamma_correction";
        param_desc.type = rclcpp::ParameterType::PARAMETER_INTEGER;
        param_desc.description = "Gamma correction, min=0, max=255.";
        param_desc.integer_range = {range};
        this->declare_parameter<int>("gamma_correction", 127, param_desc);
    }
    if (!this->has_parameter("gain_percent"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::FloatingPointRange range;
        range.set__from_value(0.1).set__to_value(100.0).set__step(0.1);
        param_desc.name = "gain_percent";
        param_desc.type = rclcpp::ParameterType::PARAMETER_DOUBLE;
        param_desc.description = "Gain percentage (%), min=0.1, max=100.0.";
        param_desc.floating_point_range = {range};
        this->declare_parameter<double>("gain_percent", 50.0, param_desc);
    }
    if (!this->has_parameter("sound_speed"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::FloatingPointRange range;
        range.set__from_value(0.0).set__to_value(1600.0).set__step(0.1); // min = 1400.0 but must include 0.0 for configuration
        param_desc.name = "sound_speed";
        param_desc.type = rclcpp::ParameterType::PARAMETER_DOUBLE;
        param_desc.description = "Sound speed (in m/s, set to 0 for it to be calculated using salinity), min=1400.0, max=1600.0.";
        param_desc.floating_point_range = {range};
        this->declare_parameter<double>("sound_speed", 0.0, param_desc);
    }
    if (!this->has_parameter("use_salinity"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        param_desc.name = "use_salinity";
        param_desc.type = rclcpp::ParameterType::PARAMETER_BOOL;
        param_desc.description = "Use salinity to calculate sound_speed.";
        this->declare_parameter<bool>("use_salinity", true, param_desc);
    }
    if (!this->has_parameter("salinity"))
    {
        rcl_interfaces::msg::ParameterDescriptor param_desc;
        rcl_interfaces::msg::FloatingPointRange range;
        range.set__from_value(0.0).set__to_value(100.0).set__step(0.1);
        param_desc.name = "salinity";
        param_desc.type = rclcpp::ParameterType::PARAMETER_DOUBLE;
        param_desc.description = "Salinity (in parts per thousand (ppt,ppm,g/kg), used to calculate sound speed if needed), min=0.0, max=100";
        param_desc.floating_point_range = {range};
        this->declare_parameter<double>("salinity", 0.0, param_desc);
    }
    this->get_parameter("ping_topic", ping_topic_);
    this->get_parameter("status_topic", status_topic_);

    this->param_cb_ = this->add_on_set_parameters_callback(std::bind(&OculusSonarNode::set_config_callback, this, std::placeholders::_1));

    this->ping_publisher_ = this->create_publisher<oculus_interfaces::msg::Ping>(ping_topic_, 100);
    this->status_publisher_ = this->create_publisher<oculus_interfaces::msg::OculusStatus>(status_topic_, 100);

    this->sonar_driver_ = std::make_shared<SonarDriver>(this->io_service_.io_service());
    this->io_service_.start();
    if (!this->sonar_driver_->wait_next_message())
    {
        std::cerr << "Timeout reached while waiting for a connection to the Oculus sonar. "
                  << "Is it properly connected ?" << std::endl;
    }
    currentConfig = this->sonar_driver_->current_ping_config();
    set_config_callback(this->get_parameters(parameters_names));

    this->sonar_driver_->add_status_callback(std::bind(&OculusSonarNode::publish_status, this, std::placeholders::_1));
    this->sonar_driver_->add_ping_callback(std::bind(&OculusSonarNode::publish_ping, this, std::placeholders::_1));
    // callback on dummy messages to reactivate the pings as needed
    this->sonar_driver_->add_dummy_callback(std::bind(&OculusSonarNode::handle_dummy, this));
}

OculusSonarNode::~OculusSonarNode()
{
    this->io_service_.stop();
}

void OculusSonarNode::publish_status(const OculusStatusMsg &status)
{
    static oculus_interfaces::msg::OculusStatus msg;

    oculus::copy_to_ros(msg, status);

    this->status_publisher_->publish(msg);
}

inline rclcpp::Time to_ros_stamp(const SonarDriver::TimePoint &stamp)
{
    size_t nano = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      stamp.time_since_epoch())
                      .count();
    size_t seconds = nano / 1000000000;
    return rclcpp::Time(seconds, nano - 1000000000 * seconds);
}

void OculusSonarNode::publish_ping(const oculus::PingMessage::ConstPtr &ping)
{
    static oculus_interfaces::msg::Ping msg;

    if (this->ping_publisher_->get_subscription_count() == 0)
    {
        cout << "Going to standby mode" << endl;
        this->sonar_driver_->standby();
        return;
    }

    oculus::copy_to_ros(msg, ping);
    this->ping_publisher_->publish(msg);
}

void OculusSonarNode::handle_dummy()
{

    if (this->ping_publisher_->get_subscription_count() > 0)
    {
        cout << "Exiting standby mode" << endl;
        this->sonar_driver_->resume();
    }
}

rcl_interfaces::msg::SetParametersResult OculusSonarNode::set_config_callback(const std::vector<rclcpp::Parameter> &parameters)
{
   std::cout << '[set_config_callback] parameters = ' << parameters << std::endl;
    SonarDriver::PingConfig newConfig = currentConfig;
    // flags
    newConfig.flags = 0x09; // always in meters, simple ping

    bool use_salinity;
    for (const rclcpp::Parameter &param : parameters)
    {
        if (param.get_name() == "frequency_mode")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating frequency_mode to " << param.as_int() << " (1: 1.2MHz, 2: 2.1MHz).");
            newConfig.masterMode = param.as_int();
        }
        else if (param.get_name() == "ping_rate")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating ping_rate to " << param.as_int() << " (0: 10Hz, 1: 15Hz, 2: 40Hz, 3: 5Hz, 4: 2Hz, 5: Standby mode).");
            switch (param.as_int())
            {
            case 0:
                newConfig.pingRate = pingRateNormal;
                break; // 10Hz
            case 1:
                newConfig.pingRate = pingRateHigh;
                break; // 15Hz
            case 2:
                newConfig.pingRate = pingRateHighest;
                break; // 40Hz
            case 3:
                newConfig.pingRate = pingRateLow;
                break; // 5Hz
            case 4:
                newConfig.pingRate = pingRateLowest;
                break; // 2Hz
            case 5:
                newConfig.pingRate = pingRateStandby;
                break; // standby mode
            default:
                break;
            }
        }
        else if (param.get_name() == "data_depth")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating data_depth to " << param.as_int() << " (0: 8 bits, 1: 16 bits).");
            switch (param.as_int())
            {
            case 0: // 8 bits
                break;
            case 1: // 16 bits
                newConfig.flags |= 0x02;
                break;
            default:
                break;
            }
        }
        else if (param.get_name() == "nbeams")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating nbeams to " << param.as_int() << " (0: 256 beams, 1: 512 beams).");
            switch (param.as_int())
            {
            case 0: // 256 beams
                break;
            case 1: // 512 beams
                newConfig.flags |= 0x40;
                break;
            default:
                break;
            }
        }
        else if (param.get_name() == "send_gain")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating send_gain to " << param.as_bool());
            if (param.as_bool())
                newConfig.flags |= 0x04;
        }
        else if (param.get_name() == "gain_assist")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating gain_assist to " << param.as_bool());
            if (param.as_bool())
                newConfig.flags |= 0x10;
        }
        else if (param.get_name() == "range")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating range to " << param.as_double() << "m.");
            newConfig.range = param.as_double();
        }
        else if (param.get_name() == "gamma_correction")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating gamma_correction to " << param.as_int());
            newConfig.gammaCorrection = param.as_int();
        }
        else if (param.get_name() == "gain_percent")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating gain_percent to " << param.as_double() << "%.");
            newConfig.gainPercent = param.as_double();
        }
        else if (param.get_name() == "use_salinity")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating use_salinity to " << param.as_bool());
            use_salinity = param.as_bool();
            if (use_salinity)
                newConfig.speedOfSound = 0.0;
        }
        else if (param.get_name() == "sound_speed")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating sound_speed to " << param.as_double() << "m/s.");
            if (!use_salinity)
            {
                if (param.as_double() >= 1400.0 && param.as_double() <= 1600.0)
                    newConfig.speedOfSound = param.as_double();
                else
                    RCLCPP_INFO_STREAM(this->get_logger(), "Speed of sound must be between 1400.0 and 1600.0.");
            }
        }
        else if (param.get_name() == "salinity")
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Updating salinity to " << param.as_double() << " parts per thousand (ppt,ppm,g/kg).");
            newConfig.salinity = param.as_double();
        }
    }
    // send config to Oculus sonar and wait for feedback
    SonarDriver::PingConfig feedback = this->sonar_driver_->request_ping_config(newConfig);

    currentConfig = feedback;

    // std::vector<rclcpp::Parameter> feedback_parameters = this->get_parameters(parameters_names);
    // for (const rclcpp::Parameter &param_feedback : feedback)
    // {
    //     this->set_parameter(rclcpp::Parameter(param_feedback.name, param_feedback.value));
    // }
    // // this->set_parameter(rclcpp::Parameter("frame_id", feedback.frameId));
    // this->set_parameter(rclcpp::Parameter("frequency_mode", feedback.masterMode));
    // // this->set_parameter(rclcpp::Parameter("data_depth", feedback.dataDepth));
    // // this->set_parameter(rclcpp::Parameter("nbeams", feedback.nBeams));
    // // this->set_parameter(rclcpp::Parameter("send_gain", feedback.sendGain));
    // // this->set_parameter(rclcpp::Parameter("gain_assist", feedback.gainAssist));
    // this->set_parameter(rclcpp::Parameter("range", feedback.range));
    // this->set_parameter(rclcpp::Parameter("gamma_correction", feedback.gammaCorrection));
    // // this->set_parameter(rclcpp::Parameter("gain_percent", feedback.gainPercent));
    // // this->set_parameter(rclcpp::Parameter("sound_speed", feedback.soundSpeed));
    // // this->set_parameter(rclcpp::Parameter("use_salinity", feedback.useSalinity));
    // this->set_parameter(rclcpp::Parameter("salinity", 50.1));


    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    result.reason = "";

    if (newConfig.masterMode != feedback.masterMode)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update frequency_mode")
        result.reason.append("Could not update frequency_mode.\n");
    }
    // newConfig.pingRate      != feedback.pingRate // is broken (?) sonar side TODO
    if ((newConfig.flags & 0x02) ? 1 : 0 != (feedback.flags & 0x02) ? 1
                                                                    : 0)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update data_depth")
        result.reason.append("Could not update data_depth.\n");
    }
    if ((newConfig.flags & 0x04) ? 1 : 0 != (feedback.flags & 0x04) ? 1
                                                                    : 0) // TODO
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update send_gain")
        result.reason.append("Could not update send_gain.\n");
    }
    if ((newConfig.flags & 0x10) ? 1 : 0 != (feedback.flags & 0x10) ? 1
                                                                    : 0)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update gain_assist")
        result.reason.append("Could not update gain_assist.\n");
    }
    if ((newConfig.flags & 0x40) ? 1 : 0 != (feedback.flags & 0x40) ? 1
                                                                    : 0)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update nbeams")
        result.reason.append("Could not update nbeams.\n");
    }
    if (newConfig.range != feedback.range)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update range")
        result.reason.append("Could not update range.\n");
    }
    if (newConfig.gammaCorrection != feedback.gammaCorrection)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update gamma_correction")
        result.reason.append("Could not update gamma_correction.\n");
    }
    if (newConfig.gainPercent != feedback.gainPercent)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update gain_percent")
        result.reason.append("Could not update gain_percent.\n");
    }
    if (newConfig.speedOfSound != feedback.speedOfSound)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update sound_speed")
        result.reason.append("Could not update sound_speed.\n");
    }
    if (newConfig.salinity != feedback.salinity)
    {
        // result.successful = false;
        RCLCPP_WARN("Could not update salinity")
        result.reason.append("Could not update salinity.\n");
    }

    return result;
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OculusSonarNode>());
    rclcpp::shutdown();
    return 0;
}
