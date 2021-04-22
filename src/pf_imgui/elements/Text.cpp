//
// Created by petr on 10/31/20.
//

#include "Text.h"
#include <imgui.h>
#include <utility>

namespace pf::ui::ig {

Text::Text(const std::string &elementName, std::string text)
    : ItemElement(elementName), DragSource<std::string>(false), DropTarget<std::string>(false), text(std::move(text)),
      color(std::nullopt) {}

Text::Text(const std::string &elementName, std::string text, ImVec4 textColor)
    : ItemElement(elementName), DragSource<std::string>(false), DropTarget<std::string>(false), text(std::move(text)),
      color(textColor) {}

const std::string &Text::getText() const { return text; }

void Text::renderImpl() {
  if (color.has_value()) {
    ImGui::TextColored(*color, "%s", text.c_str());
  } else {
    ImGui::Text("%s", text.c_str());
  }
  drag(getText());
  if (auto drop = dropAccept(); drop.has_value()) { setText(*drop); }
}

}// namespace pf::ui::ig