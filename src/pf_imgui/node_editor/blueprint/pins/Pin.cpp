//
// Created by xflajs00 on 30.03.2022.
//

#include "Pin.h"

namespace pf::ui::ig::bp {

Pin::Pin(const std::string &name, const std::string &label, const ImColor &color)
    : pf::ui::ig::Pin(name, label), color(color) {}

const ImColor &Pin::getColor() const { return color; }

void Pin::setColor(const ImColor &newColor) { color = newColor; }

bool Pin::acceptsLinkWith(pf::ui::ig::Pin &other) const {
  if (&getNode() == &other.getNode()) { return false; }
  if (getType() == other.getType()) { return false; }
  return true;
}

}  // namespace pf::ui::ig::bp