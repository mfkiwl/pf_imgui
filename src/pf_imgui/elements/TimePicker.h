/**
 * @file TimePicker.h
 * @brief TimePicker element.
 * @author Petr Flajšingr
 * @date 11.4.22
 */

#ifndef PF_IMGUI_ELEMENTS_TIMEPICKER_H
#define PF_IMGUI_ELEMENTS_TIMEPICKER_H

#include <chrono>
#include <pf_common/Explicit.h>
#include <pf_imgui/elements/SpinInput.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueContainer.h>

namespace pf::ui::ig {

using TimeOfDay = std::chrono::hh_mm_ss<std::chrono::seconds>;
namespace details {
class TimeOfDayChangeDetector {
 public:
  explicit TimeOfDayChangeDetector(TimeOfDay value) : initialValue(value) {}

  [[nodiscard]] bool hasValueChanged(const TimeOfDay &newValue) {
    return initialValue.to_duration() != newValue.to_duration();
  }

 private:
  TimeOfDay initialValue;
};
}  // namespace details

// TODO: styles
/**
 * @brief Simple HH MM SS 24 time picker.
 */
class TimePicker : public ItemElement, public ValueContainer<TimeOfDay>, public Savable {
 public:
  /**
   * @brief Construction config for TimePicker.
   */
  struct Config {
    using Parent = TimePicker;
    Explicit<std::string_view> name;  /*!< Unique name of the element */
    Explicit<std::string_view> label; /*!< Text rendered next to the input */
    TimeOfDay value{};                /*!< Initial value */
    bool persistent = false;          /*!< Enable disk state saving */
  };
  /**
   * Construct TimePicker
   * @param config construction args @see TimePicker::Config
   */
  explicit TimePicker(Config &&config);
  /**
   * Construct TimePicker
   * @param elementName unique name of the element
   * @param labelText text rendered next to the input
   * @param initialValue initial value
   * @param persistent enable disk state saving
   */
  TimePicker(std::string_view elementName, std::string_view labelText, TimeOfDay initialValue,
             Persistent persistent = Persistent::No);

  [[nodiscard]] toml::table toToml() const override;
  void setFromToml(const toml::table &src) override;

  void setValue(const TimeOfDay &newValue) override;
  [[nodiscard]] const TimeOfDay &getValue() const override;

 protected:
  Subscription addValueListenerImpl(std::function<void(const TimeOfDay &)> listener) override;

 public:
  Font font = Font::Default();
  Property<Label> label;

  Property<TimeOfDay, details::TimeOfDayChangeDetector> time;

 protected:
  void renderImpl() override;

 private:
  void inputChanged();

  void updateInnerValues();

  int hours;
  int minutes;
  int seconds;
};

}  // namespace pf::ui::ig

#endif  //PF_IMGUI_ELEMENTS_TIMEPICKER_H
