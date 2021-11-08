//
// Created by petr on 1/23/21.
//

#include "Tooltip.h"
#include <imgui.h>

namespace pf::ui::ig {

Tooltip::Tooltip(const std::string &elementName) : Element(elementName) {}

void Tooltip::renderImpl() {
  auto colorStyle = setColorStack();
  auto style = setStyleStack();
  ImGui::BeginTooltip();
  RAII end{[] { ImGui::EndTooltip(); }};
  std::ranges::for_each(getChildren(), [&](auto &child) { child.render(); });
}

}// namespace pf::ui::ig