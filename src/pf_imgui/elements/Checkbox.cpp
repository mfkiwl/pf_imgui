//
// Created by petr on 10/31/20.
//

#include "Checkbox.h"
#include <imgui.h>
#include <pf_imgui/details/ToggleButton.h>

namespace pf::ui::ig {

Checkbox::Checkbox(Checkbox::Config &&config)
    : ItemElement(std::string{config.name}), ValueObservable(config.checked), Labellable(std::string{config.label}),
      Savable(config.persistent ? Persistent::Yes : Persistent::No) {}

Checkbox::Checkbox(const std::string &elementName, const std::string &label, bool value, Persistent persistent)
    : ItemElement(elementName), ValueObservable(value), Labellable(label), Savable(persistent) {}

void Checkbox::renderImpl() {
  auto colorStyle = setColorStack();
  auto style = setStyleStack();
  if (ImGui::Checkbox(getLabel().c_str(), getValueAddress())) { notifyValueChanged(); }
}

void Checkbox::unserialize_impl(const toml::table &src) {
  if (auto newValIter = src.find("checked"); newValIter != src.end()) {
    if (auto newVal = newValIter->second.value<bool>(); newVal.has_value()) { setValueAndNotifyIfChanged(*newVal); }
  }
}

toml::table Checkbox::serialize_impl() const { return toml::table{{"checked", getValue()}}; }

void Checkbox::setSelected(bool selected) { setValueAndNotifyIfChanged(selected); }

bool Checkbox::isSelected() const { return getValue(); }

void Checkbox::toggle() { setSelected(!isSelected()); }

}  // namespace pf::ui::ig
