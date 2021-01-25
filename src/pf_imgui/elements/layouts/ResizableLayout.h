//
// Created by petr on 1/24/21.
//

#ifndef PF_IMGUI_SRC_PF_IMGUI_ELEMENTS_LAYOUTS_RESIZABLELAYOUT_H
#define PF_IMGUI_SRC_PF_IMGUI_ELEMENTS_LAYOUTS_RESIZABLELAYOUT_H

#include "Layout.h"
#include <pf_imgui/_export.h>
#include <pf_imgui/elements/interface/Element.h>
#include <pf_imgui/elements/interface/Resizable.h>

namespace pf::ui::ig {

class PF_IMGUI_EXPORT ResizableLayout : public Layout, public Resizable {
 public:
  ResizableLayout(const std::string &elementName, const ImVec2 &size, bool showBorder);
};

}// namespace pf::ui::ig
#endif//PF_IMGUI_SRC_PF_IMGUI_ELEMENTS_LAYOUTS_RESIZABLELAYOUT_H
