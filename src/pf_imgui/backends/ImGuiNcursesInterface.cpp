//
// Created by petr on 6/29/21.
//

#include "ImGuiNcursesInterface.h"
#include <pf_imgui/backends/impl/imtui-impl-text.h>

namespace pf::ui::ig {

ImGuiNcursesInterface::ImGuiNcursesInterface(ImGuiConfigFlags flags, const toml::table &tomlConfig)
    : ImGuiInterface(flags, tomlConfig, false) {
  screen = ImTui_ImplNcurses_Init(true);
  ImTui_ImplText_Init();
}

void ImGuiNcursesInterface::updateFonts() {}

void ImGuiNcursesInterface::render() {

  ImTui_ImplNcurses_NewFrame();
  ImTui_ImplText_NewFrame();
  ImGui::NewFrame();
  if (getVisibility() == Visibility::Visible) {
    if (getEnabled() == Enabled::No) {
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
      auto raiiEnabled = pf::RAII([] {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
      });
      renderImpl();
    } else {
      renderImpl();
    }
  }
  ImGui::Render();
  ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
  ImTui_ImplNcurses_DrawScreen();
}

void ImGuiNcursesInterface::renderImpl() {
  if (hasMenuBar()) { menuBar->render(); }
  ImGuiInterface::renderImpl();
  renderDialogs();
}
ImGuiNcursesInterface::~ImGuiNcursesInterface() {
  ImTui_ImplText_Shutdown();
  ImTui_ImplNcurses_Shutdown();
}
}// namespace pf::ui::ig