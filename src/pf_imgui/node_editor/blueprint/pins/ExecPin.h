//
// Created by xflajs00 on 29.03.2022.
//

#ifndef PF_IMGUI_BLUEPRINT_EXECPIN_H
#define PF_IMGUI_BLUEPRINT_EXECPIN_H

#include "Pin.h"
#include <imgui_internal.h>

namespace pf::ui::ig::bp {

class ExecPin : public Pin {
 public:
  ExecPin(const std::string &name, const std::string &label, const ImColor &pinColor);

  [[nodiscard]] bool acceptsLinkWith(ig::Pin &other) const override;

 protected:
  void renderIcon() override;

  void addLink(std::shared_ptr<Link> link) override;
};

}  // namespace pf::ui::ig::bp

#endif  //PF_IMGUI_BLUEPRINT_EXECPIN_H
