#pragma once

#include "boost_defs.hpp"

#include "event_queue.hpp"
#include "modifier_flag_manager.hpp"
#include "stream_utility.hpp"
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <json/json.hpp>
#include <unordered_set>

namespace krbn {
namespace manipulator {
namespace details {

class event_definition {
public:
  enum class type {
    none,
    key_code,
    consumer_key_code,
    pointing_button,
    any,
    shell_command,
    select_input_source,
    set_variable,
    mouse_key,
  };

  enum class modifier {
    any,
    caps_lock,
    command,
    control,
    fn,
    left_command,
    left_control,
    left_option,
    left_shift,
    option,
    right_command,
    right_control,
    right_option,
    right_shift,
    shift,
    end_,
  };

  virtual ~event_definition(void) {
  }

  type get_type(void) const {
    return type_;
  }

  boost::optional<key_code> get_key_code(void) const {
    if (type_ == type::key_code) {
      return boost::get<key_code>(value_);
    }
    return boost::none;
  }

  boost::optional<consumer_key_code> get_consumer_key_code(void) const {
    if (type_ == type::consumer_key_code) {
      return boost::get<consumer_key_code>(value_);
    }
    return boost::none;
  }

  boost::optional<pointing_button> get_pointing_button(void) const {
    if (type_ == type::pointing_button) {
      return boost::get<pointing_button>(value_);
    }
    return boost::none;
  }

  boost::optional<type> get_any_type(void) const {
    if (type_ == type::any) {
      return boost::get<type>(value_);
    }
    return boost::none;
  }

  boost::optional<std::string> get_shell_command(void) const {
    if (type_ == type::shell_command) {
      return boost::get<std::string>(value_);
    }
    return boost::none;
  }

  boost::optional<std::vector<input_source_selector>> get_input_source_selectors(void) const {
    if (type_ == type::select_input_source) {
      return boost::get<std::vector<input_source_selector>>(value_);
    }
    return boost::none;
  }

  boost::optional<std::pair<std::string, int>> get_set_variable(void) const {
    if (type_ == type::set_variable) {
      return boost::get<std::pair<std::string, int>>(value_);
    }
    return boost::none;
  }

  boost::optional<mouse_key> get_mouse_key(void) const {
    if (type_ == type::mouse_key) {
      return boost::get<mouse_key>(value_);
    }
    return boost::none;
  }

  boost::optional<event_queue::queued_event::event> to_event(void) const {
    switch (type_) {
      case type::none:
        return boost::none;
      case type::key_code:
        return event_queue::queued_event::event(boost::get<key_code>(value_));
      case type::consumer_key_code:
        return event_queue::queued_event::event(boost::get<consumer_key_code>(value_));
      case type::pointing_button:
        return event_queue::queued_event::event(boost::get<pointing_button>(value_));
      case type::any:
        return boost::none;
      case type::shell_command:
        return event_queue::queued_event::event::make_shell_command_event(boost::get<std::string>(value_));
      case type::select_input_source:
        return event_queue::queued_event::event::make_select_input_source_event(boost::get<std::vector<input_source_selector>>(value_));
      case type::set_variable:
        return event_queue::queued_event::event::make_set_variable_event(boost::get<std::pair<std::string, int>>(value_));
      case type::mouse_key:
        return event_queue::queued_event::event::make_mouse_key_event(boost::get<mouse_key>(value_));
    }
  }

  static std::unordered_set<modifier> make_modifiers(const nlohmann::json& json) {
    std::unordered_set<modifier> modifiers;

    for (const auto& j : json) {
      if (!j.is_string()) {
        logger::get_logger().error("complex_modifications json error: modifier should be string form: {0}", j.dump());

      } else {
        const std::string& name = j;
        if (name == "any") {
          modifiers.insert(modifier::any);
        } else if (name == "caps_lock") {
          modifiers.insert(modifier::caps_lock);
        } else if (name == "command") {
          modifiers.insert(modifier::command);
        } else if (name == "control") {
          modifiers.insert(modifier::control);
        } else if (name == "fn") {
          modifiers.insert(modifier::fn);
        } else if (name == "left_command") {
          modifiers.insert(modifier::left_command);
        } else if (name == "left_control") {
          modifiers.insert(modifier::left_control);
        } else if (name == "left_option") {
          modifiers.insert(modifier::left_option);
        } else if (name == "left_shift") {
          modifiers.insert(modifier::left_shift);
        } else if (name == "option") {
          modifiers.insert(modifier::option);
        } else if (name == "right_command") {
          modifiers.insert(modifier::right_command);
        } else if (name == "right_control") {
          modifiers.insert(modifier::right_control);
        } else if (name == "right_option") {
          modifiers.insert(modifier::right_option);
        } else if (name == "right_shift") {
          modifiers.insert(modifier::right_shift);
        } else if (name == "shift") {
          modifiers.insert(modifier::shift);
        } else {
          logger::get_logger().error("complex_modifications json error: Unknown modifier: {0}", name);
        }
      }
    }

    return modifiers;
  }

  static std::vector<modifier_flag> get_modifier_flags(modifier modifier) {
    switch (modifier) {
      case modifier::any:
        return {};

      case modifier::caps_lock:
        return {modifier_flag::caps_lock};

      case modifier::command:
        return {modifier_flag::left_command, modifier_flag::right_command};

      case modifier::control:
        return {modifier_flag::left_control, modifier_flag::right_control};

      case modifier::fn:
        return {modifier_flag::fn};

      case modifier::left_command:
        return {modifier_flag::left_command};

      case modifier::left_control:
        return {modifier_flag::left_control};

      case modifier::left_option:
        return {modifier_flag::left_option};

      case modifier::left_shift:
        return {modifier_flag::left_shift};

      case modifier::option:
        return {modifier_flag::left_option, modifier_flag::right_option};

      case modifier::right_command:
        return {modifier_flag::right_command};

      case modifier::right_control:
        return {modifier_flag::right_control};

      case modifier::right_option:
        return {modifier_flag::right_option};

      case modifier::right_shift:
        return {modifier_flag::right_shift};

      case modifier::shift:
        return {modifier_flag::left_shift, modifier_flag::right_shift};

      case modifier::end_:
        return {};
    }
  }

  static modifier get_modifier(modifier_flag modifier_flag) {
    switch (modifier_flag) {
      case modifier_flag::caps_lock:
        return modifier::caps_lock;

      case modifier_flag::fn:
        return modifier::fn;

      case modifier_flag::left_command:
        return modifier::left_command;

      case modifier_flag::left_control:
        return modifier::left_control;

      case modifier_flag::left_option:
        return modifier::left_option;

      case modifier_flag::left_shift:
        return modifier::left_shift;

      case modifier_flag::right_command:
        return modifier::right_command;

      case modifier_flag::right_control:
        return modifier::right_control;

      case modifier_flag::right_option:
        return modifier::right_option;

      case modifier_flag::right_shift:
        return modifier::right_shift;

      case modifier_flag::zero:
      case modifier_flag::end_:
        return modifier::end_;
    }
  }

protected:
  event_definition(void) : type_(type::none) {
  }

  event_definition(key_code key_code) : type_(type::key_code),
                                        value_(key_code) {
  }

  event_definition(consumer_key_code consumer_key_code) : type_(type::consumer_key_code),
                                                          value_(consumer_key_code) {
  }

  event_definition(pointing_button pointing_button) : type_(type::pointing_button),
                                                      value_(pointing_button) {
  }

  void handle_json(const nlohmann::json& json,
                   std::function<bool(const std::string&, const nlohmann::json&, const nlohmann::json&)> extra_json_handler) {
    if (!json.is_object()) {
      logger::get_logger().error("complex_modifications json error: Invalid form of event_definition: {0}", json.dump());
      return;
    }

    for (auto it = std::begin(json); it != std::end(json); std::advance(it, 1)) {
      // it.key() is always std::string.
      const auto& key = it.key();
      const auto& value = it.value();

      // Set type_ and values.
      if (key == "key_code") {
        if (type_ != type::none) {
          logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
          continue;
        }
        if (!value.is_string()) {
          logger::get_logger().error("complex_modifications json error: Invalid form of key_code: {0}", json.dump());
          continue;
        }

        if (auto key_code = types::make_key_code(value.get<std::string>())) {
          type_ = type::key_code;
          value_ = *key_code;
        }

      } else if (key == "consumer_key_code") {
        if (type_ != type::none) {
          logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
          continue;
        }
        if (!value.is_string()) {
          logger::get_logger().error("complex_modifications json error: Invalid form of consumer_key_code: {0}", json.dump());
          continue;
        }

        if (auto consumer_key_code = types::make_consumer_key_code(value.get<std::string>())) {
          type_ = type::consumer_key_code;
          value_ = *consumer_key_code;
        }

      } else if (key == "pointing_button") {
        if (type_ != type::none) {
          logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
          continue;
        }
        if (!value.is_string()) {
          logger::get_logger().error("complex_modifications json error: Invalid form of pointing_button: {0}", json.dump());
          continue;
        }

        if (auto pointing_button = types::make_pointing_button(value.get<std::string>())) {
          type_ = type::pointing_button;
          value_ = *pointing_button;
        }

      } else if (key == "description") {
        // Do nothing

      } else {
        if (!extra_json_handler(key, value, json)) {
          logger::get_logger().error("complex_modifications json error: Unknown key: {0} in {1}", key, json.dump());
        }
      }
    }
  }

  type type_;
  boost::variant<key_code,
                 consumer_key_code,
                 pointing_button,
                 type, // For any
                 std::string, // For shell_command
                 std::vector<input_source_selector>, // For select_input_source
                 std::pair<std::string, int>, // For set_variable
                 mouse_key // For mouse_key
                 >
      value_;
}; // namespace details

class from_event_definition final : public event_definition {
public:
  from_event_definition(const nlohmann::json& json) {
    handle_json(json,
                [&](const std::string& key, const nlohmann::json& value, const nlohmann::json& json) {
                  return extra_json_handler(key, value, json);
                });
  }

  from_event_definition(key_code key_code,
                        const std::unordered_set<modifier>& mandatory_modifiers,
                        const std::unordered_set<modifier>& optional_modifiers) : event_definition(key_code),
                                                                                  mandatory_modifiers_(mandatory_modifiers),
                                                                                  optional_modifiers_(optional_modifiers) {
  }

  virtual ~from_event_definition(void) {
  }

  const std::unordered_set<modifier>& get_mandatory_modifiers(void) const {
    return mandatory_modifiers_;
  }

  const std::unordered_set<modifier>& get_optional_modifiers(void) const {
    return optional_modifiers_;
  }

  boost::optional<std::unordered_set<modifier_flag>> test_modifiers(const modifier_flag_manager& modifier_flag_manager) const {
    std::unordered_set<modifier_flag> modifier_flags;

    // If mandatory_modifiers_ contains modifier::any, return all active modifier_flags.

    if (mandatory_modifiers_.find(modifier::any) != std::end(mandatory_modifiers_)) {
      for (auto i = static_cast<uint32_t>(modifier_flag::zero) + 1; i != static_cast<uint32_t>(modifier_flag::end_); ++i) {
        auto flag = modifier_flag(i);
        if (modifier_flag_manager.is_pressed(flag)) {
          modifier_flags.insert(flag);
        }
      }
      return modifier_flags;
    }

    // Check modifier_flag state.

    for (int i = 0; i < static_cast<int>(modifier::end_); ++i) {
      auto m = modifier(i);

      if (mandatory_modifiers_.find(m) != std::end(mandatory_modifiers_)) {
        auto pair = test_modifier(modifier_flag_manager, m);
        if (!pair.first) {
          return boost::none;
        }
        if (pair.second != modifier_flag::zero) {
          modifier_flags.insert(pair.second);
        }
      }
    }

    // If optional_modifiers_ does not contain modifier::any, we have to check modifier flags strictly.

    if (optional_modifiers_.find(modifier::any) == std::end(optional_modifiers_)) {
      std::unordered_set<modifier_flag> extra_modifier_flags;
      for (auto m = static_cast<uint32_t>(modifier_flag::zero) + 1; m != static_cast<uint32_t>(modifier_flag::end_); ++m) {
        extra_modifier_flags.insert(modifier_flag(m));
      }

      for (int i = 0; i < static_cast<int>(modifier::end_); ++i) {
        auto m = modifier(i);

        if (mandatory_modifiers_.find(m) != std::end(mandatory_modifiers_) ||
            optional_modifiers_.find(m) != std::end(optional_modifiers_)) {
          for (const auto& flag : get_modifier_flags(m)) {
            extra_modifier_flags.erase(flag);
          }
        }
      }

      for (const auto& flag : extra_modifier_flags) {
        if (modifier_flag_manager.is_pressed(flag)) {
          return boost::none;
        }
      }
    }

    return modifier_flags;
  }

  static std::pair<bool, modifier_flag> test_modifier(const modifier_flag_manager& modifier_flag_manager,
                                                      modifier modifier) {
    if (modifier == modifier::any) {
      return std::make_pair(true, modifier_flag::zero);
    }

    auto modifier_flags = get_modifier_flags(modifier);
    if (!modifier_flags.empty()) {
      for (const auto& m : modifier_flags) {
        if (modifier_flag_manager.is_pressed(m)) {
          return std::make_pair(true, m);
        }
      }
    }

    return std::make_pair(false, modifier_flag::zero);
  }

private:
  bool extra_json_handler(const std::string& key,
                          const nlohmann::json& value,
                          const nlohmann::json& json) {
    if (key == "modifiers") {
      if (!value.is_object()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of modifiers: {0}", value.dump());
        return true;
      }

      for (auto it = std::begin(value); it != std::end(value); std::advance(it, 1)) {
        // it.key() is always std::string.
        const auto& k = it.key();
        const auto& v = it.value();

        if (k == "mandatory") {
          mandatory_modifiers_ = make_modifiers(v);
        } else if (k == "optional") {
          optional_modifiers_ = make_modifiers(v);
        } else {
          logger::get_logger().error("complex_modifications json error: Unknown key: {0} in {1}", k, value.dump());
        }
      }
      return true;

    } else if (key == "any") {
      if (type_ != type::none) {
        logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
        return true;
      }
      if (!value.is_string()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of any: {0}", json.dump());
        return true;
      }

      if (value == "key_code") {
        type_ = type::any;
        value_ = type::key_code;
      } else if (value == "consumer_key_code") {
        type_ = type::any;
        value_ = type::consumer_key_code;
      } else if (value == "pointing_button") {
        type_ = type::any;
        value_ = type::pointing_button;
      } else {
        logger::get_logger().error("complex_modifications json error: Unknown value of any: {0}", json.dump());
      }

      return true;
    }

    return false;
  }

  std::unordered_set<modifier> mandatory_modifiers_;
  std::unordered_set<modifier> optional_modifiers_;
};

class to_event_definition final : public event_definition {
public:
  to_event_definition(const nlohmann::json& json) : lazy_(false),
                                                    repeat_(true) {
    handle_json(json,
                [&](const std::string& key, const nlohmann::json& value, const nlohmann::json& json) {
                  return extra_json_handler(key, value, json);
                });
  }

  to_event_definition(key_code key_code,
                      const std::unordered_set<modifier>& modifiers) : event_definition(key_code),
                                                                       modifiers_(modifiers),
                                                                       lazy_(false),
                                                                       repeat_(true) {
  }

  virtual ~to_event_definition(void) {
  }

  const std::unordered_set<modifier>& get_modifiers(void) const {
    return modifiers_;
  }

  bool get_lazy(void) const {
    return lazy_;
  }

  bool get_repeat(void) const {
    return repeat_;
  }

  bool needs_virtual_hid_pointing(void) const {
    if (type_ == type::pointing_button ||
        type_ == type::mouse_key) {
      return true;
    }
    return false;
  }

private:
  bool extra_json_handler(const std::string& key,
                          const nlohmann::json& value,
                          const nlohmann::json& json) {
    if (key == "modifiers") {
      modifiers_ = make_modifiers(value);
      return true;

    } else if (key == "shell_command") {
      if (type_ != type::none) {
        logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
        return true;
      }
      if (!value.is_string()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of shell_command: {0}", json.dump());
        return true;
      }

      type_ = type::shell_command;
      value_ = value.get<std::string>();

      return true;

    } else if (key == "select_input_source") {
      if (type_ != type::none) {
        logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
        return true;
      }

      std::vector<input_source_selector> input_source_selectors;

      if (value.is_object()) {
        input_source_selectors.emplace_back(value);
      } else if (value.is_array()) {
        for (const auto& v : value) {
          input_source_selectors.emplace_back(v);
        }
      } else {
        logger::get_logger().error("complex_modifications json error: Invalid form of select_input_source: {0}", json.dump());
        return true;
      }

      type_ = type::select_input_source;
      value_ = input_source_selectors;

      return true;

    } else if (key == "set_variable") {
      if (type_ != type::none) {
        logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
        return true;
      }
      if (!value.is_object()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of set_variable: {0}", json.dump());
        return true;
      }

      if (auto n = json_utility::find_optional<std::string>(value, "name")) {
        if (auto v = json_utility::find_optional<int>(value, "value")) {
          type_ = type::set_variable;
          value_ = std::make_pair(*n, *v);
        } else {
          logger::get_logger().error("complex_modifications json error: valid `value` is not found in set_variable: {0}", json.dump());
        }
      } else {
        logger::get_logger().error("complex_modifications json error: valid `name` is not found in set_variable: {0}", json.dump());
      }

      return true;

    } else if (key == "mouse_key") {
      if (type_ != type::none) {
        logger::get_logger().error("complex_modifications json error: Duplicated type definition: {0}", json.dump());
        return true;
      }
      if (!value.is_object()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of mouse_key: {0}", json.dump());
        return true;
      }

      type_ = type::mouse_key;
      value_ = mouse_key(value);

      return true;

    } else if (key == "lazy") {
      if (!value.is_boolean()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of lazy: {0}", json.dump());
        return true;
      }

      lazy_ = value;

      return true;

    } else if (key == "repeat") {
      if (!value.is_boolean()) {
        logger::get_logger().error("complex_modifications json error: Invalid form of repeat: {0}", json.dump());
        return true;
      }

      repeat_ = value;

      return true;
    }

    return false;
  }

  std::unordered_set<modifier> modifiers_;
  bool lazy_;
  bool repeat_;
};

inline std::ostream& operator<<(std::ostream& stream, const event_definition::modifier& value) {
#define KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(MODIFIER) \
  case event_definition::modifier::MODIFIER:               \
    stream << #MODIFIER;                                   \
    break;

  switch (value) {
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(any);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(caps_lock);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(command);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(control);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(fn);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(left_command);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(left_control);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(left_option);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(left_shift);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(option);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(right_command);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(right_control);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(right_option);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(right_shift);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(shift);
    KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT(end_);
  }

#undef KRBN_MANIPULATOR_DETAILS_MODIFIER_OUTPUT

  return stream;
}

template <template <class T, class A> class container>
inline std::ostream& operator<<(std::ostream& stream, const container<event_definition::modifier, std::allocator<event_definition::modifier>>& values) {
  return stream_utility::output_enums(stream, values);
}

template <template <class T, class H, class K, class A> class container>
inline std::ostream& operator<<(std::ostream& stream,
                                const container<event_definition::modifier,
                                                std::hash<event_definition::modifier>,
                                                std::equal_to<event_definition::modifier>,
                                                std::allocator<event_definition::modifier>>& values) {
  return stream_utility::output_enums(stream, values);
}
} // namespace details
} // namespace manipulator
} // namespace krbn
