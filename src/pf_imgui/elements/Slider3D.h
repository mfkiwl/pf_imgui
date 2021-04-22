/**
 * @file Slider3D.h
 * @brief Slider3D element.
 * @author Petr Flajšingr
 * @date 20.4.21
 */

#ifndef PF_IMGUI_ELEMENTS_SLIDER3D_H
#define PF_IMGUI_ELEMENTS_SLIDER3D_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <include/DearWidgets/dear_widgets.h>
#include <pf_common/concepts/OneOf.h>
#include <pf_imgui/interface/DragNDrop.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Labellable.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>

namespace pf::ui::ig {

/**
 * @brief Slider with 3D space.
 * @tparam T underlying type
 */
template<OneOf<float> T>
class Slider3D : public ItemElement,
                 public Labellable,
                 public ValueObservable<glm::vec3>,
                 public Savable,
                 public DragSource<glm::vec3>,
                 public DropTarget<glm::vec3> {
 public:
  /**
   * Construct Slider3D.
   * @param elementName ID of the element
   * @param label text rendered above the slider
   * @param minMaxX extremes on X axis
   * @param minMaxY extremes on Y axis
   * @param minMaxZ extremes on Z axis
   * @param value starting value
   * @param scale possible max size will be multiplied by this
   * @param persistent enable state saving to disk
   */
  Slider3D(const std::string &elementName, const std::string &label, const glm::vec2 &minMaxX, const glm::vec2 &minMaxY,
           const glm::vec2 &minMaxZ, const glm::vec3 &value = {}, float scale = 1.0f,
           Persistent persistent = Persistent::No)
      : ItemElement(elementName), Labellable(label), ValueObservable<glm::vec3>(value),
        Savable(persistent), DragSource<glm::vec3>(false), DropTarget<glm::vec3>(false), extremesX(minMaxX),
        extremesY(minMaxY), extremesZ(minMaxZ), sliderScale(scale) {}

 protected:
  void renderImpl() override {
    auto valueChanged = false;
    auto address = ValueObservable<glm::vec3>::getValueAddress();
    const auto oldValue = *address;
    if constexpr (std::same_as<T, float>) {
      valueChanged =
          ImWidgets::SliderScalar3D(getLabel().c_str(), &address->x, &address->y, &address->z, extremesX.x, extremesX.y,
                                    extremesY.x, extremesY.y, extremesZ.x, extremesZ.y, sliderScale);
    }
    DragSource<glm::vec3>::drag(ValueObservable<glm::vec3>::getValue());
    if (auto drop = DropTarget<glm::vec3>::dropAccept(); drop.has_value()) {
      ValueObservable<glm::vec3>::setValueAndNotifyIfChanged(*drop);
      return;
    }
    if (valueChanged && oldValue != *address) { ValueObservable<glm::vec3>::notifyValueChanged(); }
  }
  void unserialize_impl(const toml::table &src) override {
    const auto tomlVec = src["value"].as_array();
    const auto vec = deserializeGlmVec<glm::vec3>(*tomlVec);
    ValueObservable<glm::vec3>::setValueAndNotifyIfChanged(vec);
  }
  toml::table serialize_impl() override {
    const auto value = ValueObservable<glm::vec3>::getValue();
    return toml::table{{{"value", serializeGlmVec(value)}}};
  }

 private:
  glm::vec2 extremesX;
  glm::vec2 extremesY;
  glm::vec2 extremesZ;
  float sliderScale;
};
}// namespace pf::ui::ig

#endif//PF_IMGUI_ELEMENTS_SLIDER3D_H
