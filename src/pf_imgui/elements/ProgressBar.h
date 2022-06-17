/**
 * @file ProgressBar.h
 * @brief ProgressBar element.
 * @author Petr Flajšingr
 * @date 31.10.20
 */

#ifndef PF_IMGUI_ELEMENTS_PROGRESSBAR_H
#define PF_IMGUI_ELEMENTS_PROGRESSBAR_H

#include <algorithm>
#include <imgui.h>
#include <pf_common/Explicit.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/interface/Customizable.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Resizable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <string>

namespace pf::ui::ig {
/**
 * Required operations for type to be usable as ProgressBar value.
 * @tparam T
 */
template<typename T>
concept ProgressBarCompatible = requires(T t, float f) {
                                  { t + t } -> std::same_as<T>;
                                  { t *= f };
                                  { std::clamp(t, t, t) } -> std::convertible_to<T>;
                                } && std::convertible_to<float, T>;

/**
 * @brief Progress bar for notifying a user of operation progress.
 * @tparam T type storing the progress value
 */
template<ProgressBarCompatible T>
class PF_IMGUI_EXPORT ProgressBar
    : public ItemElement,
      public ValueObservable<T>,
      public Resizable,
      public ColorCustomizable<style::ColorOf::Text, style::ColorOf::TextDisabled, style::ColorOf::FrameBackground,
                               style::ColorOf::FrameBackgroundHovered, style::ColorOf::FrameBackgroundActive,
                               style::ColorOf::Border, style::ColorOf::BorderShadow, style::ColorOf::PlotHistogram>,
      public StyleCustomizable<style::Style::FramePadding, style::Style::FrameRounding, style::Style::FrameBorderSize>,
      public FontCustomizable {
 public:
  /**
   * Construct ProgressBar
   * @param config construction args @see ProgressBar::Config
   */
  struct Config {
    using Parent = ProgressBar;
    Explicit<std::string_view> name; /*!< Unique name of the element */
    Explicit<T> step;                /*!< Step taken when @code step() @endcode is called */
    Explicit<T> min;                 /*!< Lowest value representing 0% */
    Explicit<T> max;                 /*!< Highest value representing 100% */
    T value = min;                   /*!< Initial value within [min, max] */
    std::string overlay = "";        /*!< Text rendered on top of the element */
    Size size = Size::Auto();        /*!< Size of the element */
  };
  /**
   * Construct ProgressBar
   * @param config construction args @see ProgressBar::Config
   */
  explicit ProgressBar(Config &&config);
  /**
   * Construct ProgressBar.
   * @param elementName ID of the progress bar
   * @param stepValue step added to the current value when step() is called
   * @param min min value - used as a starting value when no value is provided
   * @param max max value
   * @param value starting value - min <= value <=max
   * @param overlayStr text rendered on top of the element
   * @param size size of the progress bar
   */
  ProgressBar(const std::string &elementName, T stepValue, T min, T max, std::optional<T> value = std::nullopt,
              std::string overlayStr = "", const Size &size = Size::Auto());

  /**
   * Set current percentage where min = 0% and max = 100%.
   * @param percentage <0.0f-1.0f>, gets clamped if outside this interval
   * @return current inner value of the progress bar
   */
  T setPercentage(float percentage);

  /**
   * Move inner value by step provided in constructor.
   * @return current inner value of the progress bar
   */
  T step();

  /**
   * Get min progress bar value.
   * @return min progress bar value
   */
  [[nodiscard]] T getMin() const { return min; }
  /**
   * Set min progress bar value.
   * @param newMin min progress bar value
   */
  void setMin(T newMin) { min = newMin; }
  /**
   * Get max progress bar value.
   * @return max progress bar value
   */
  [[nodiscard]] T getMax() const { return max; }
  /**
   * Set max progress bar value.
   * @param newMax min progress bar value
   */
  void setMax(T newMax) { max = newMax; }

  /**
   * Get step value which is used in step().
   * @return step value
   */
  [[nodiscard]] T getStep() const { return stepValue; }
  /**
   * Set new step value used for step().
   * @param step new step value
   */
  void setStep(T step) { stepValue = step; }

  /**
   * Get current percentage of the progress bar <0.0f-1.0f>.
   * @return current percentage of the progress bar <0.0f-1.0f>
   */
  [[nodiscard]] float getCurrentPercentage() const;

 protected:
  void renderImpl() override;

 private:
  T stepValue;
  T min;
  T max;
  std::string overlay;
};

template<ProgressBarCompatible T>
ProgressBar<T>::ProgressBar(ProgressBar::Config &&config)
    : ItemElement(std::string{config.name.value}), ValueObservable<T>(config.value), Resizable(config.size),
      stepValue(config.step), min(config.min), max(config.max), overlay(std::move(config.overlay)) {}

template<ProgressBarCompatible T>
ProgressBar<T>::ProgressBar(const std::string &elementName, T stepValue, T min, T max, std::optional<T> value,
                            std::string overlayStr, const Size &size)
    : ItemElement(elementName), ValueObservable<T>(value.value_or(min)), Resizable(size), stepValue(stepValue),
      min(min), max(max), overlay(std::move(overlayStr)) {}

template<ProgressBarCompatible T>
T ProgressBar<T>::setPercentage(float percentage) {
  percentage = std::clamp(percentage, 0.f, 1.f);
  const auto oldValue = ValueObservable<T>::getValue();
  const auto newValue = min + (max - min) * percentage;
  ValueObservable<T>::setValueInner(static_cast<T>(newValue));
  if (ValueObservable<T>::getValue() != oldValue) { ValueObservable<T>::notifyValueChanged(); }
  return ValueObservable<T>::getValue();
}

template<ProgressBarCompatible T>
T ProgressBar<T>::step() {
  const auto oldValue = ValueObservable<T>::getValue();
  const auto newValue = std::clamp(oldValue + stepValue, min, max);
  ValueObservable<T>::setValueInner(newValue);
  if (ValueObservable<T>::getValue() != oldValue) { ValueObservable<T>::notifyValueChanged(); }
  return newValue;
}

template<ProgressBarCompatible T>
float ProgressBar<T>::getCurrentPercentage() const {
  const auto diff = max - min;
  return (ValueObservable<T>::getValue() - min) / static_cast<float>(diff);
}

template<ProgressBarCompatible T>
void ProgressBar<T>::renderImpl() {
  [[maybe_unused]] auto colorStyle = setColorStack();
  [[maybe_unused]] auto style = setStyleStack();
  [[maybe_unused]] auto scopedFont = applyFont();
  ImGui::ProgressBar(getCurrentPercentage(), static_cast<ImVec2>(getSize()), overlay.c_str());
}

extern template class ProgressBar<float>;
extern template class ProgressBar<int>;

}  // namespace pf::ui::ig
#endif  // PF_IMGUI_ELEMENTS_PROGRESSBAR_H
