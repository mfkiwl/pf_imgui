//
// Created by petr on 6/27/21.
//

#include "DockSpace.h"
#include <imgui_internal.h>
#include <pf_common/RAII.h>

namespace pf::ui::ig {

DockSpace::DockSpace(const std::string &name, Size s, const Flags<DockType> &dockFlags)
    : Element(name), Resizable(s), flags(*dockFlags) {}

void DockSpace::render() {
  if (getVisibility() == Visibility::Visible) {
    auto colorSet = setColorStack();
    if (getEnabled() == Enabled::No) {
      ImGui::BeginDisabled();
      auto raiiEnabled = RAII([] { ImGui::EndDisabled(); });
      renderImpl();
    } else {
      renderImpl();
    }
  }
}

void DockSpace::renderImpl() {
  id = ImGui::GetID(getName().c_str());
  ImGui::DockSpace(id, getSize().asImVec(), flags);
}

DockSpace::Id DockSpace::getDockId() const { return id; }
}// namespace pf::ui::ig