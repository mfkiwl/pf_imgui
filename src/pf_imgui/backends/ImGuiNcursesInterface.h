//
// Created by petr on 6/29/21.
//

#ifndef PF_IMGUI_SRC_PF_IMGUI_BACKENDS_IMGUINCURSESINTERFACE_H
#define PF_IMGUI_SRC_PF_IMGUI_BACKENDS_IMGUINCURSESINTERFACE_H

#include <optional>
#include <pf_imgui/ImGuiInterface.h>
#include <pf_imgui/backends/impl/imtui-impl-ncurses.h>

namespace pf::ui::ig {
class ImGuiNcursesInterface : public ImGuiInterface {
 public:
  ImGuiNcursesInterface(ImGuiConfigFlags flags, const toml::table &tomlConfig);
  ~ImGuiNcursesInterface() override;
  void updateFonts() override;
  void render() override;

 protected:
  void renderImpl() override;

 private:
  ImTui::TScreen *screen;
};
}// namespace pf::ui::ig
#endif//PF_IMGUI_SRC_PF_IMGUI_BACKENDS_IMGUINCURSESINTERFACE_H
