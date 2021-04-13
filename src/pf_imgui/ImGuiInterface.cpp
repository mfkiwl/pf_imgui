//
// Created by petr on 10/27/20.
//

#include "ImGuiInterface.h"
#include "serialization.h"
#include "src/pf_imgui/dialogs/Dialog.h"
#include <imgui_internal.h>
#include <implot.h>
#include <pf_common/algorithms.h>
#include <range/v3/view/addressof.hpp>
#include <utility>

#include <experimental/array>
#include <pf_imgui/elements/Button.h>
#include <pf_imgui/elements/Checkbox.h>
#include <pf_imgui/elements/Table.h>
#include <pf_imgui/elements/Text.h>

namespace pf::ui::ig {

ImGuiInterface::ImGuiInterface(ImGuiConfigFlags flags, toml::table tomlConfig)
    : Renderable("imgui_interface"), io(baseInit(flags)), config(std::move(tomlConfig)) {}

ImGuiIO &ImGuiInterface::baseInit(ImGuiConfigFlags flags) {

  using namespace std::string_literals;

  auto tab =
      Table<Text, Button, Checkbox>{"table2 man",
                                    TableSettings<3>{.header = std::experimental::make_array("txt"s, "btn", "chbox"),
                                                     .border = TableBorder::Full,
                                                     .resizableCols = true,
                                                     .reorderable = true,
                                                     .sortable = true,
                                                     .hideableCols = true}};

  auto hihi = tab.getRow(0);
  std::get<0>(hihi).setText("test");
  tab.removeRow(0);

  struct Factory {
    using CreateType = int;
    TableRow<Text, Button, Checkbox> create(CreateType) { return {}; }
  };

  auto tab2 = TableWithFactory<Factory, Text, Button, Checkbox>{
      "table2 man",
      TableSettings<3>{.header = std::experimental::make_array("txt"s, "btn", "chbox"),
                       .border = TableBorder::Full,
                       .resizableCols = true,
                       .reorderable = true,
                       .sortable = true,
                       .hideableCols = true}};

  auto hihi2 = tab.getRow(0);
  std::get<0>(hihi2).setText("test");
  tab.removeRow(0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  auto &imguiIo = ImGui::GetIO();
  imguiIo.ConfigFlags |= flags;
  ImGui::StyleColorsDark();
  return imguiIo;
}

ImGuiIO &ImGuiInterface::getIo() const { return io; }

Dialog &ImGuiInterface::createDialog(const std::string &elementName, const std::string &caption, Modal modal) {
  auto dialog = std::make_unique<Dialog>(*this, elementName, caption, modal);
  const auto ptr = dialog.get();
  dialogs.emplace_back(std::move(dialog));
  return *ptr;
}

AppMenuBar &ImGuiInterface::getMenuBar() {
  if (menuBar == nullptr) { menuBar = std::make_unique<AppMenuBar>("app_menu_bar"); }
  return *menuBar;
}
bool ImGuiInterface::hasMenuBar() const { return menuBar != nullptr; }
const toml::table &ImGuiInterface::getConfig() const { return config; }

void ImGuiInterface::updateConfig() {
  config.clear();
  std::ranges::for_each(windows, [this](auto &window) {
    auto serialised = serializeImGuiTree(*window);
    config.insert(serialised.begin(), serialised.end());
  });
}

void ImGuiInterface::setStateFromConfig() {
  std::ranges::for_each(windows, [this](auto &window) {
    traverseImGuiTree(*window, [this](Renderable &renderable) {
      if (auto ptrSavable = dynamic_cast<Savable *>(&renderable); ptrSavable != nullptr) {
        if (auto ptrElement = dynamic_cast<Element *>(&renderable); ptrElement != nullptr) {
          if (config.contains(ptrElement->getName())) {
            ptrSavable->unserialize(*config[ptrElement->getName()].as_table());
          }
        }
      }
    });
  });
}
void ImGuiInterface::renderFileDialogs() {
  std::ranges::for_each(fileDialogs, [](auto &dialog) { dialog.render(); });
  if (const auto iter = std::ranges::find_if(fileDialogs, [](auto &dialog) { return dialog.isDone(); });
      iter != fileDialogs.end()) {
    fileDialogs.erase(iter);
  }
}
bool ImGuiInterface::isWindowHovered() const { return io.WantCaptureMouse; }

bool ImGuiInterface::isKeyboardCaptured() const { return io.WantCaptureKeyboard; }

Window &ImGuiInterface::createWindow(const std::string &windowName, std::string title) {
  windows.emplace_back(std::make_unique<Window>(windowName, std::move(title)));
  return *windows.back();
}

void ImGuiInterface::removeWindow(const std::string &name) {
  if (auto iter = std::ranges::find_if(windows, [name](const auto &window) { return window->getName() == name; });
      iter != windows.end()) {
    windows.erase(iter);
  }
}
Window &ImGuiInterface::windowByName(const std::string &name) {
  if (auto window = findIf(getWindows() | ranges::views::addressof,
                           [name](const auto &window) { return window->getName() == name; });
      window.has_value()) {
    return **window;
  } else {
    throw IdNotFoundException("Child not found: {}", name);
  }
}

void ImGuiInterface::renderImpl() {
  std::ranges::for_each(windows, [](auto &window) { window->render(); });
}

void ImGuiInterface::removeDialog(Dialog &dialog) {
  if (const auto iter = std::ranges::find_if(dialogs, [&dialog](const auto &ptr) { return ptr.get() == &dialog; });
      iter != dialogs.end()) {
    dialogs.erase(iter);
  }
}

}// namespace pf::ui::ig