//
// Created by xflajs00 on 02.04.2022.
//

#include "Size.h"

namespace pf::ui::ig {

Size::Size(Width initialWidth, Height initialHeight) : width(initialWidth), height(initialHeight) {}

bool Size::operator==(const Size &rhs) const { return width == rhs.width && height == rhs.height; }

bool Size::operator!=(const Size &rhs) const { return !(rhs == *this); }

Size Size::Auto() { return {Width::Auto(), Height::Auto()}; }

Size Size::Fill() { return {Width::Fill(), Height::Fill()}; }

Size::operator ImVec2() const { return ImVec2{static_cast<float>(width), static_cast<float>(height)}; }

Size Size::FillWidth() { return {Width::Fill(), Height::Auto()}; }

Size::Size(ImVec2 vec) : width(vec.x), height(vec.y) {}

Size Size::FillHeight() { return {Width::Auto(), Height::Fill()}; }

}  // namespace pf::ui::ig