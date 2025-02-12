/**
 * @file Knob.h
 * @brief Knob element, similar to slider.
 * @author Petr Flajšingr
 * @date 9.5.22
 */

#ifndef PF_IMGUI_ELEMENTS_KNOB_H
#define PF_IMGUI_ELEMENTS_KNOB_H

#include <imgui-knobs.h>
#include <pf_common/Explicit.h>
#include <pf_common/concepts/OneOf.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueContainer.h>

namespace pf::ui::ig {
/**
 * @brief Type of Knob element.
 */
enum class KnobType {
  Tick = ImGuiKnobVariant_Tick,
  Dot = ImGuiKnobVariant_Dot,
  Wiper = ImGuiKnobVariant_Wiper,
  WiperOnly = ImGuiKnobVariant_WiperOnly,
  WiperDot = ImGuiKnobVariant_WiperDot,
  Stepped = ImGuiKnobVariant_Stepped,
  Space = ImGuiKnobVariant_Space
};
// TODO: styles
/**
 * @brief ElementWithID similar to slider.
 * @tparam T underlying value type
 * @todo: more types
 */
template<OneOf<int, float> T>
class Knob : public ItemElement, public ValueContainer<T>, public Savable {
 public:
  /**
   * @brief Construction args for Knob
   */
  struct Config {
    using Parent = Knob;
    Explicit<std::string_view> name; /*!< Unique name of the element */
    Explicit<std::string> label;     /*!< Label rendered above the element */
    Explicit<Size> size;             /*!< Size of the element */
    Explicit<KnobType> type;         /*!< Type of knob */
    Explicit<T> minValue;            /*!< Min allowed value */
    Explicit<T> maxValue;            /*!< Max allowed value */
    float speed = 1.f;               /*!< Value change speed */
    T value = T{};                   /*!< Initial value */
    bool persistent = false;         /*!< Enable state saving to disk */
  };
  /**
   * Construct Knob.
   * @param config construction args @see Knob::Config
   */
  explicit Knob(Config &&config);
  /**
   * Construct Knob
   * @param name unique name of the lement
   * @param label label rendered above the element
   * @param type type of knob
   * @param s size of the element
   * @param min min allowed value
   * @param max max allowed value
   * @param speed value change speed
   * @param value initial value
   * @param persistent enable state saving to disk
   */
  Knob(std::string_view name, std::string_view label, KnobType type, Size s, T min, T max, float speed = 1.f,
       T value = T{}, Persistent persistent = Persistent::No);

  [[nodiscard]] toml::table toToml() const override;
  void setFromToml(const toml::table &src) override;

  ColorPalette<ColorOf::ButtonActive, ColorOf::ButtonHovered, ColorOf::FrameBackground> color;
  Font font = Font::Default();
  Property<Label> label;

  Property<Size> size;
  Property<T> value;

  [[nodiscard]] const T &getValue() const override;
  void setValue(const T &newValue) override;

 protected:
  Subscription addValueListenerImpl(std::function<void(const T &)> listener) override;

  void renderImpl() override;

 private:
  T min;
  T max;
  float speed;
  KnobType type;
};

template<OneOf<int, float> T>
Knob<T>::Knob(Knob::Config &&config)
    : Knob(config.name, config.label, config.type, config.size, config.minValue, config.maxValue, config.speed,
           config.value, config.persistent ? Persistent::Yes : Persistent::No) {}

template<OneOf<int, float> T>
Knob<T>::Knob(std::string_view name, std::string_view label, KnobType type, Size s, T min, T max, float speed, T value,
              Persistent persistent)
    : ItemElement(name), Savable(persistent), label(std::string{label}), size(s), value(value), min(min), max(max),
      speed(speed), type(type) {}

template<OneOf<int, float> T>
toml::table Knob<T>::toToml() const {
  return toml::table{{"value", *value}};
}

template<OneOf<int, float> T>
void Knob<T>::setFromToml(const toml::table &src) {
  if (auto newValIter = src.find("value"); newValIter != src.end()) {
    if (auto newVal = newValIter->second.value<T>(); newVal.has_value()) { *value.modify() = *newVal; }
  }
}

template<OneOf<int, float> T>
void Knob<T>::renderImpl() {
  [[maybe_unused]] auto colorScoped = color.applyScoped();
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();
  const auto flags =
      label->getVisibility() == Visibility::Visible ? ImGuiKnobFlags_{} : ImGuiKnobFlags_::ImGuiKnobFlags_NoTitle;
  const auto knobSize = std::min(static_cast<float>(size->width), static_cast<float>(size->height));
  auto valueChanged = false;
  if constexpr (std::same_as<T, int>) {
    valueChanged = ImGuiKnobs::KnobInt(label->get().c_str(), &Prop_value(value), min, max, speed, nullptr,
                                       static_cast<ImGuiKnobVariant>(type), knobSize, flags);
  }
  if constexpr (std::same_as<T, float>) {
    valueChanged = ImGuiKnobs::Knob(label->get().c_str(), &Prop_value(value), min, max, speed, nullptr,
                                    static_cast<ImGuiKnobVariant>(type), knobSize, flags);
  }
  if (valueChanged) { Prop_triggerListeners(value); }
}

template<OneOf<int, float> T>
const T &Knob<T>::getValue() const {
  return *value;
}

template<OneOf<int, float> T>
void Knob<T>::setValue(const T &newValue) {
  *value.modify() = newValue;
}

template<OneOf<int, float> T>
Subscription Knob<T>::addValueListenerImpl(std::function<void(const T &)> listener) {
  return value.addListener(std::move(listener));
}

}  // namespace pf::ui::ig

#endif  //PF_IMGUI_ELEMENTS_KNOB_H
