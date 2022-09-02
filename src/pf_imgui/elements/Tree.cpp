//
// Created by petr on 10/31/20.
//

#include "Tree.h"
#include <imgui.h>

namespace pf::ui::ig {

details::TreeRecord::TreeRecord(const std::string &elementName, const std::string &treeLabel,
                                const Flags<ImGuiTreeNodeFlags_> &defaultFlags)
    : ItemElement(elementName), label(treeLabel), flags(defaultFlags) {}

TreeLeaf::TreeLeaf(TreeLeaf::Config &&config)
    : TreeRecord(std::string{config.name.value}, std::string{config.label.value}, ImGuiTreeNodeFlags_Leaf),
      ValueObservable(config.selected), Savable(config.persistent ? Persistent::Yes : Persistent::No) {
  if (config.selected) {
    flags |= ImGuiTreeNodeFlags_Selected;
  } else {
    flags &= static_cast<ImGuiTreeNodeFlags_>(~ImGuiTreeNodeFlags_Selected);
  }
}

TreeLeaf::TreeLeaf(const std::string &elementName, const std::string &labelText, bool selected, Persistent persistent)
    : TreeRecord(elementName, labelText, ImGuiTreeNodeFlags_Leaf), ValueObservable(selected), Savable(persistent) {
  if (selected) {
    flags |= ImGuiTreeNodeFlags_Selected;
  } else {
    flags &= static_cast<ImGuiTreeNodeFlags_>(~ImGuiTreeNodeFlags_Selected);
  }
}

void TreeLeaf::setValue(const bool &newValue) {
  if (newValue == getValue()) { return; }
  if (newValue) {
    flags |= ImGuiTreeNodeFlags_Selected;
  } else {
    flags &= static_cast<ImGuiTreeNodeFlags_>(~ImGuiTreeNodeFlags_Selected);
  }
  if (limiter != nullptr && newValue) { limiter->selected = this; }
  ValueObservable::setValue(newValue);
}

toml::table TreeLeaf::toToml() const { return toml::table{{"value", getValue()}}; }

void TreeLeaf::setFromToml(const toml::table &src) {
  if (auto newValIter = src.find("value"); newValIter != src.end()) {
    if (auto newVal = newValIter->second.value<bool>(); newVal.has_value()) { setValue(*newVal); }
  }
}

void TreeLeaf::renderImpl() {
  [[maybe_unused]] auto colorScoped = color.applyScoped();
  [[maybe_unused]] auto styleScoped = style.applyScoped();
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();
  const auto pop = ImGui::TreeNodeEx(label.get().c_str(), *flags);
  RAII end{[pop] {
    if (pop) { ImGui::TreePop(); }
  }};
  if (ImGui::IsItemClicked()) { setValue(!getValue()); }
  if (limiter != nullptr && limiter->selected != this) { setValue(false); }
}

}  // namespace pf::ui::ig
