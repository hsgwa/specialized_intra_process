// Copyright 2021 Research Institute of Systems Planning, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SPECIALIZED_INTRA_PROCESS__CREATE_INTRA_PROCESS_SUBSCRIPTION_HPP_
#define SPECIALIZED_INTRA_PROCESS__CREATE_INTRA_PROCESS_SUBSCRIPTION_HPP_

#include <memory>
#include <string>

#include "subscription_wrapper.hpp"

namespace feature
{
template<
  typename MessageT, typename CallbackT,
  typename CallbackMessageT =
  typename rclcpp::subscription_traits::has_message_type<CallbackT>::type,
  typename SubscriptionT = feature::Subscription<MessageT>>
typename std::shared_ptr<SubscriptionT> create_intra_process_subscription(
  rclcpp::Node * node, const std::string & topic_name, const rclcpp::QoS & qos, CallbackT callback)
{
  auto sub_wrapper = std::make_shared<SubscriptionT>();

  auto callback_wrapper = [callback,
      sub_wrapper](notification_msgs::msg::Notification::UniquePtr msg) {
      callback(sub_wrapper->consume_unique(msg->seq));
    };
  auto sub = node->create_subscription<notification_msgs::msg::Notification>(
    topic_name, qos, callback_wrapper);

  // This is used for setting up things like intra process comms which
  // require this->shared_from_this() which cannot be called from
  // the constructor.
  // And because of circular reference.
  sub_wrapper->post_init_setup(node, sub);

  return sub_wrapper;
}
}  // namespace feature

#endif  // SPECIALIZED_INTRA_PROCESS__CREATE_INTRA_PROCESS_SUBSCRIPTION_HPP_