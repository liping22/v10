/*
 *  Copyright (c) 2015-, Filippo Basso <bassofil@gmail.com>
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder(s) nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UNIPD_CALIBRATION_CALIBRATION_ROS_SENSORS_DEPTH_SENSOR_H_
#define UNIPD_CALIBRATION_CALIBRATION_ROS_SENSORS_DEPTH_SENSOR_H_

#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>

#include <image_transport/image_transport.h>

#include <calibration_common/depth/sensor.h>
#include <calibration_pcl/base/definitions.h>

#include <calibration_ros/sensors/ros_sensor.h>

namespace unipd
{
namespace calib
{

namespace ROSDepthSensorNS
{

enum Topic : int
{
  NONE = 0,
  IMAGE = 1,
  CAMERA_INFO = 2,
  POINT_CLOUD = 4,
  ALL = IMAGE | CAMERA_INFO | POINT_CLOUD,
};

struct Messages
{
  sensor_msgs::Image::ConstPtr image_msg;
  sensor_msgs::CameraInfo::ConstPtr camera_info_msg;
  sensor_msgs::PointCloud2::ConstPtr cloud_msg;
};

struct Data
{
  cv::Mat image;
  PCLCloud3::Ptr cloud;
};

}

class ROSDepthSensor : public ROSSensor<ROSDepthSensorNS::Topic, ROSDepthSensorNS::Messages, ROSDepthSensorNS::Data>
{
public:

  using Topic = ROSDepthSensorNS::Topic;
  using Messages = ROSDepthSensorNS::Messages;
  using Data = ROSDepthSensorNS::Data;

  ROSDepthSensor (const ros::NodeHandle & node_handle);

  virtual
  ~ROSDepthSensor () {}

  virtual void
  subscribe (int topic);

  inline const std::shared_ptr<PinholeDepthSensor> &
  sensor () const
  {
    return sensor_;
  }

  inline bool
  isSensorSet () const
  {
    return sensor_->cameraModel().initialized();
  }

  void
  imageCallback (const sensor_msgs::Image::ConstPtr & image_msg,
                 const sensor_msgs::CameraInfo::ConstPtr & camera_info_msg);

  void
  cameraInfoCallback (const sensor_msgs::CameraInfo::ConstPtr & camera_info_msg);

  void
  cloudCallback (const sensor_msgs::PointCloud2::ConstPtr & cloud_msg);

  virtual const Messages &
  lastMessages () const override
  {
    if (allMessagesReceived())
      resetReceivedMessages();
    return last_messages_;
  }

  virtual Data
  convertMessages (const Messages & messages) const override;

protected:

  ros::NodeHandle node_handle_;
  image_transport::ImageTransport image_transport_;

  image_transport::CameraSubscriber image_sub_;
  ros::Subscriber camera_info_sub_;
  ros::Subscriber cloud_sub_;

  Messages last_messages_;

  std::shared_ptr<PinholeDepthSensor> sensor_;

};

} // namespace calib
} // namespace unipd
#endif // UNIPD_CALIBRATION_CALIBRATION_ROS_SENSORS_DEPTH_SENSOR_H_
