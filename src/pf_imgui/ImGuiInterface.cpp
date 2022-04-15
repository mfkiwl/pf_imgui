//
// Created by petr on 10/27/20.
//

#include <implot.h>
#include <pf_imgui/ImGuiInterface.h>
#include <pf_imgui/serialization.h>
#include <range/v3/view/addressof.hpp>
#include <range/v3/view/filter.hpp>
#include <utility>

namespace pf::ui::ig {

ImGuiInterface::ImGuiInterface(ImGuiConfig config)
    : Renderable("imgui_interface"), imguiContext(ImGui::CreateContext()), imPlotContext(ImPlot::CreateContext()),
      io(ImGui::GetIO()), fontManager(*this, config.iconFontDirectory, config.enabledIconPacks, config.iconSize),
      notificationManager(fontManager), config(std::move(config.config)) {
  io.ConfigFlags = *config.flags;
  io.IniFilename = nullptr;
  ImGui::StyleColorsDark();
}

ImGuiInterface::~ImGuiInterface() {
  ImGui::DestroyContext(imguiContext);
  ImPlot::DestroyContext(imPlotContext);
}

ImGuiIO &ImGuiInterface::getIo() const { return io; }

ModalDialog &ImGuiInterface::createDialog(const std::string &elementName, const std::string &caption) {
  auto dialog = std::make_unique<ModalDialog>(*this, elementName, caption);
  const auto ptr = dialog.get();
  dialogs.emplace_back(std::move(dialog));
  return *ptr;
}

AppMenuBar &ImGuiInterface::getMenuBar() {
  if (menuBar == nullptr) { menuBar = std::make_unique<AppMenuBar>("app_menu_bar"); }
  return *menuBar;
}

bool ImGuiInterface::hasMenuBar() const { return menuBar != nullptr; }

AppStatusBar &ImGuiInterface::createStatusBar(const std::string &barName) {
  statusBar = std::make_unique<AppStatusBar>(barName);
  return *statusBar;
}

void ImGuiInterface::removeStatusBar() { statusBar = nullptr; }

const toml::table &ImGuiInterface::getConfig() const { return config; }

void ImGuiInterface::updateConfig() {
  std::ranges::for_each(windows, [this](const auto &window) {
    auto serialisedTree = serializeImGuiTree(*window);
    for (const auto &[key, value] : serialisedTree) { config.insert_or_assign(key, value); }
    if (menuBar != nullptr) {
      auto serialisedAppBar = serializeImGuiTree(*menuBar);
      for (const auto &[key, value] : serialisedAppBar) { config.insert_or_assign(key, value); }
    }
  });
  if (fileDialogBookmark.has_value()) { config.insert_or_assign("file_dialog_bookmark", fileDialogBookmark.value()); }
  std::ranges::for_each(radioGroups, [this](const auto &radioGroup) {
    if (radioGroup->isPersistent()) { config.insert_or_assign(radioGroup->getGroupName(), radioGroup->toToml()); }
  });
  //  if (io.WantSaveIniSettings) {
  io.WantSaveIniSettings = false;
  std::size_t iniSize;
  const auto imguiIniDataRaw = ImGui::SaveIniSettingsToMemory(&iniSize);
  std::string imguiIniData{imguiIniDataRaw, iniSize};
  config.insert_or_assign("imgui_ini", imguiIniData);
  // }
}

void ImGuiInterface::setStateFromConfig() {
  const auto serialiseSubtree = [this](Renderable &root) {
    traverseImGuiTree(root, [this](Renderable &renderable) {
      if (auto ptrSavable = dynamic_cast<Savable *>(&renderable); ptrSavable != nullptr && ptrSavable->isPersistent()) {
        if (auto ptrElement = dynamic_cast<Element *>(&renderable); ptrElement != nullptr) {
          if (auto elemDataIter = config.find(ptrElement->getName()); elemDataIter != config.end()) {
            if (auto elemData = elemDataIter->second.as_table(); elemData != nullptr) {
              ptrSavable->setFromToml(*elemData);
            }
          }
        }
      }
    });
  };
  if (menuBar != nullptr) { serialiseSubtree(*menuBar); }
  std::ranges::for_each(windows, [&serialiseSubtree](auto &window) { serialiseSubtree(*window); });
  if (auto iter = config.find("file_dialog_bookmark"); iter != config.end()) {
    if (auto str = iter->second.as_string(); str != nullptr) { fileDialogBookmark = str->get(); }
  }
  std::ranges::for_each(radioGroups, [this](const auto &radioGroup) {
    if (auto iter = config.find(radioGroup->getGroupName()); iter != config.end() && radioGroup->isPersistent()) {
      if (auto data = iter->second.as_table(); data != nullptr) { radioGroup->setFromToml(*data); }
    }
  });
  if (auto imguiIniToml = config.find("imgui_ini"); imguiIniToml != config.end()) {
    if (auto imguiIni = imguiIniToml->second.as_string(); imguiIni != nullptr) {
      ImGui::LoadIniSettingsFromMemory(imguiIni->get().data(), imguiIni->get().size());
    }
  }
}

void ImGuiInterface::addFileDialog(FileDialog &&dialog) {
  auto &dialogRef = fileDialogs.emplace_back(std::move(dialog));
  if (fileDialogBookmark.has_value()) { dialogRef.deserializeBookmark(*fileDialogBookmark); }
}

FileDialogBuilder ImGuiInterface::buildFileDialog(FileDialogType type) { return FileDialogBuilder(this, type); }

void ImGuiInterface::renderDialogs() {
  std::ranges::for_each(fileDialogs, &FileDialog::render);
  if (const auto iter = std::ranges::find_if(fileDialogs, [](auto &dialog) { return dialog.isDone(); });
      iter != fileDialogs.end()) {
    fileDialogBookmark = iter->serializeBookmark();
    fileDialogs.erase(iter);
  }
  std::ranges::for_each(dialogs, [](auto &dialog) { dialog->render(); });
  if (const auto iter = std::ranges::find_if(dialogs, [](auto &dialog) { return dialog->isClosed(); });
      iter != dialogs.end()) {
    dialogs.erase(iter);
  }
}

bool ImGuiInterface::isWindowHovered() const { return io.WantCaptureMouse; }

bool ImGuiInterface::isKeyboardCaptured() const { return io.WantCaptureKeyboard; }

Window &ImGuiInterface::createWindow(const std::string &windowName, std::string title) {
  windows.emplace_back(std::make_unique<Window>(windowName, std::move(title)));
  return *windows.back();
}

void ImGuiInterface::removeWindow(const std::string &windowName) {
  windows.erase(std::ranges::find(windows, windowName, [](const auto &window) { return window->getName(); }));
}

void ImGuiInterface::removeWindow(const Window &window) {
  windows.erase(std::ranges::find(windows, &window, [](const auto &window) { return window.get(); }));
}

CommandPaletteWindow &ImGuiInterface::createCommandPalette(const std::string &windowName) {
  return *commandPalettes.emplace_back(std::make_unique<CommandPaletteWindow>(windowName));
}

void ImGuiInterface::removePaletteWindow(const std::string &windowName) {
  auto remove = std::ranges::remove(commandPalettes, windowName, [](const auto &window) { return window->getName(); });
  commandPalettes.erase(remove.begin());
}

void ImGuiInterface::removePaletteWindow(const CommandPaletteWindow &window) {
  auto remove = std::ranges::remove(commandPalettes, &window, &std::unique_ptr<CommandPaletteWindow>::get);
  commandPalettes.erase(remove.begin(), remove.end());
}

DockBuilder &ImGuiInterface::createDockBuilder(DockSpace &dockSpace) {
  return *dockBuilders.emplace_back(std::unique_ptr<DockBuilder>{new DockBuilder{dockSpace}});
}

std::optional<std::reference_wrapper<Window>> ImGuiInterface::windowByName(const std::string &windowName) {
  if (auto window = findIf(getWindows() | ranges::views::addressof,
                           [windowName](const auto &window) { return window->getName() == windowName; });
      window.has_value()) {
    return **window;
  } else {
    return std::nullopt;
  }
}

std::optional<std::reference_wrapper<const Window>> ImGuiInterface::windowByName(const std::string &windowName) const {
  if (auto window = findIf(getWindows() | ranges::views::addressof,
                           [windowName](const auto &window) { return window->getName() == windowName; });
      window.has_value()) {
    return **window;
  } else {
    return std::nullopt;
  }
}

void ImGuiInterface::renderImpl() {
  [[maybe_unused]] auto colorStyle = setColorStack();
  [[maybe_unused]] auto style = setStyleStack();
  if (hasMenuBar()) { menuBar->render(); }
  if (backgroundDockingArea != nullptr) { backgroundDockingArea->render(); }
  std::ranges::for_each(windows, [](auto &window) { window->render(); });
  std::ranges::for_each(commandPalettes, [](auto &window) { window->render(); });
  std::ranges::for_each(dragNDropGroups, &DragNDropGroup::frame, &std::unique_ptr<DragNDropGroup>::get);
  std::ranges::for_each(radioGroups, &RadioGroup::frame, &std::unique_ptr<RadioGroup>::get);

  auto anyDockBuilderRun = false;
  std::ranges::for_each(dockBuilders | ranges::views::filter([](std::unique_ptr<DockBuilder> &builder) {
                          return builder->dockSpaceRef.isInitialised();
                        }),
                        [&anyDockBuilderRun](std::unique_ptr<DockBuilder> &builder) {
                          builder->run();
                          anyDockBuilderRun = true;
                        });
  if (anyDockBuilderRun) {
    const auto [rmBeg, rmEnd] = std::ranges::remove_if(
        dockBuilders, [](std::unique_ptr<DockBuilder> &builder) { return builder->dockSpaceRef.isInitialised(); });
    dockBuilders.erase(rmBeg, rmEnd);
  }
  if (statusBar != nullptr) { statusBar->render(); }
  renderDialogs();
  notificationManager.renderNotifications();
}

void ImGuiInterface::removeDialog(ModalDialog &dialog) {
  if (const auto iter = std::ranges::find_if(dialogs, [&dialog](const auto &ptr) { return ptr.get() == &dialog; });
      iter != dialogs.end()) {
    dialogs.erase(iter);
  }
}
DragNDropGroup &ImGuiInterface::createDragNDropGroup() {
  return *dragNDropGroups.emplace_back(std::make_unique<DragNDropGroup>());
}

RadioGroup &ImGuiInterface::createRadioGroup(const std::string &groupName, Persistent persistent) {
  return *radioGroups.emplace_back(std::make_unique<RadioGroup>(groupName, std::vector<RadioButton *>{}, persistent));
}

BackgroundDockingArea &ImGuiInterface::createOrGetBackgroundDockingArea() {
  if (backgroundDockingArea == nullptr) {
    backgroundDockingArea = std::make_unique<BackgroundDockingArea>("background_docking_area");
  }
  return *backgroundDockingArea;
}

void ImGuiInterface::removeBackgroundDockingArea() { backgroundDockingArea = nullptr; }

Font ImGuiInterface::getGlobalFont() const { return globalFont; }

void ImGuiInterface::setGlobalFont(Font newFont) { globalFont = std::move(newFont); }

FontManager &ImGuiInterface::getFontManager() { return fontManager; }

const FontManager &ImGuiInterface::getFontManager() const { return fontManager; }

NotificationManager &ImGuiInterface::getNotificationManager() { return notificationManager; }

const NotificationManager &ImGuiInterface::getNotificationManager() const { return notificationManager; }

void ImGuiInterface::render() {
  if (shouldUpdateFontAtlas) {
    shouldUpdateFontAtlas = false;
    updateFonts();
  }
  newFrame_impl();
  ImGui::NewFrame();
  RAII endFrameRAII{[&] {
    ImGui::Render();
    renderDrawData_impl(ImGui::GetDrawData());
    if (getIo().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
    }
  }};
  if (getVisibility() == Visibility::Visible) {
    auto fontScoped = globalFont.applyScoped();
    if (getEnabled() == Enabled::No) {
      ImGui::BeginDisabled();
      RAII raiiEnabled{ImGui::EndDisabled};
      renderImpl();
    } else {
      renderImpl();
    }
  }
}

}  // namespace pf::ui::ig
