//
// Created by petr on 10/27/20.
//

#ifndef PF_IMGUI_IMGUIINTERFACE_H
#define PF_IMGUI_IMGUIINTERFACE_H

#include "fwd.h"
#include "src/pf_imgui/dialogs/Dialog.h"
#include "src/pf_imgui/dialogs/FileDialog.h"
#include "src/pf_imgui/dialogs/Window.h"
#include <imgui.h>
#include <memory>
#include <pf_common/coroutines/Sequence.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/dialogs/MessageDialog.h>
#include <pf_imgui/elements/MenuBars.h>
#include <pf_imgui/interface/ElementContainer.h>
#include <string>
#include <toml++/toml.h>
#include <vector>

namespace pf::ui::ig {

/**
 * @brief Root class of an entire UI tree. Provides an ability to create windows and dialogs.
 *
 * The class has it's own menu bar, which is applied to the entire render window.
 *
 * It also takes care of config saving for Savable elements.
 *
 * For actual use it needs to be inherited from and render_impl should be reimplemented with your own rendering logic.
 *
 * @todo: change id type
 */
class PF_IMGUI_EXPORT ImGuiInterface : public Renderable {
 public:
  /**
   * Construct ImGuiInterface with given flags.
   * @param flags
   * @param tomlConfig config containing data of Savable elements
   */
  explicit ImGuiInterface(ImGuiConfigFlags flags, toml::table tomlConfig);

  [[nodiscard]] ImGuiIO &getIo() const;

  /**
   * Create a dialog. @see Dialog
   * The dialog should be built by the user.
   * @param elementName ID of the dialog
   * @param caption title
   * @param modal dialog modality
   * @return reference to the created dialog
   */
  Dialog &createDialog(const std::string &elementName, const std::string &caption, Modal modal = Modal::Yes);

  template<typename ButtonTypes = MessageButtons>
  MessageDialog<ButtonTypes> &createMsgDlg(const std::string &elementName, const std::string &title,
                                           const std::string &message, Flags<ButtonTypes> buttons,
                                           std::invocable<ButtonTypes> auto &&onDialogDone, Modal modal = Modal::Yes) {
    auto dialog = std::make_unique<MessageDialog<ButtonTypes>>(
        *this, elementName, title, message, buttons, std::forward<decltype(onDialogDone)>(onDialogDone), modal);
    const auto ptr = dialog.get();
    dialogs.emplace_back(std::move(dialog));
    return *ptr;
  }

  /**
   * Create a Window.
   * @param windowName ID of the window
   * @param title title
   * @return reference to the created window
   */
  Window &createWindow(const std::string &windowName, std::string title);
  /**
   * Remove the window from UI. This invalidates any references to the window.
   * @param name ID of the window to remove
   */
  void removeWindow(const std::string &name);

  /**
   * Find Window by its ID.
   * @param name ID of the window
   * @throws IdNotFoundException when the Window of given ID is not present
   * @return reference to the searched for Window
   */
  Window &windowByName(const std::string &name);

  /**
   * Get all windows present in the UI.
   * @return view to references of all windows
   */
  inline auto getWindows() {
    return windows | ranges::views::transform([](auto &window) -> Window & { return *window; });
  }
  /**
   * Get all windows present in the UI.
   * @return view to const references of all windows
   */
  inline auto getWindows() const {
    return windows | ranges::views::transform([](const auto &window) -> const Window & { return *window; });
  }

  /**
   * Get menu bar of the application. If the menubar doesn't exist, it is created.
   * @return reference to the applications menu bar
   */
  [[nodiscard]] AppMenuBar &getMenuBar();
  /**
   * Check if menu bar is present.
   * @return true if menu bar is present, false otherwise
   */
  [[nodiscard]] bool hasMenuBar() const;

  /**
   * Get config, which contains data of Savable elements.
   * @attention Call updateConfig() in order to collect data from the UI tree.
   * @return toml data of Savable elements
   */
  [[nodiscard]] const toml::table &getConfig() const;

  /**
   * Check if the window is hovered.
   * @return true if the window is hovered, false otherwise
   */
  [[nodiscard]] bool isWindowHovered() const;
  /**
   * Check if keyboard is currently being capture by UI.
   * @return true if the keyboard is captured, false otherwise
   */
  [[nodiscard]] bool isKeyboardCaptured() const;

  /**
   * Collect data from the UI tree and save serialized values into toml table.
   * @details Use getConfig() to collect saved data.
   */
  void updateConfig();
  /**
   * Set state from inner config file.
   * @todo: save state of ui and reload it using this?
   */
  void setStateFromConfig();

  /**
   * Set new style to the UI.
   * @param styleSetter callback modifying style of the UI
   */
  void setStyle(std::invocable<ImGuiStyle &> auto styleSetter) { styleSetter(ImGui::GetStyle()); }

  /**
   * Create and open a file selection dialog. @see FileDialog
   * @param caption Title
   * @param extSettings extension settings @see FileExtensionSettings
   * @param onSelect callback for when user selects files
   * @param onCancel callback for when user cancels selection
   * @param size size of the dialog
   * @param startPath path in which the dialog opens @todo: change to std::filesystem::path
   * @param startName default name for selected file/dir
   * @param modality modality of the dialog
   * @param maxSelectedFiles maximum amount of selected files
   */
  void openFileDialog(const std::string &caption, const std::vector<FileExtensionSettings> &extSettings,
                      std::invocable<std::vector<std::string>> auto onSelect, std::invocable auto onCancel,
                      ImVec2 size = {200, 150}, std::string startPath = ".", std::string startName = "",
                      Modal modality = Modal::No, uint32_t maxSelectedFiles = 1) {
    using namespace std::string_literals;
    fileDialogs.emplace_back("FileDialog"s + std::to_string(getNext(idGen)), caption, extSettings, onSelect, onCancel,
                             size, startPath, startName, modality, maxSelectedFiles);
  }

  /**
   * Create and open a directory selection dialog. @see FileDialog
   * @param caption Title
   * @param onSelect callback for when user selects files
   * @param onCancel callback for when user cancels selection
   * @param size size of the dialog
   * @param startPath path in which the dialog opens @todo: change to std::filesystem::path
   * @param startName default name for selected file/dir
   * @param modality modality of the dialog
   * @param maxSelectedDirs maximum amount of selected directories
   */
  void openDirDialog(const std::string &caption, std::invocable<std::vector<std::string>> auto onSelect,
                     std::invocable auto onCancel, ImVec2 size = {200, 150}, std::string startPath = ".",
                     std::string startName = "", Modal modality = Modal::No, uint32_t maxSelectedFiles = 1) {
    using namespace std::string_literals;
    fileDialogs.emplace_back("FileDialog"s + std::to_string(getNext(idGen)), caption, onSelect, onCancel, size,
                             startPath, startName, modality, maxSelectedFiles);
  }

 protected:
  std::unique_ptr<AppMenuBar> menuBar = nullptr;

  /**
   * Render all file dialogs which are currently open.
   */
  void renderFileDialogs();

  /**
   * @attention Override this function to provide rendering capabilities.
   */
  void renderImpl() override;

 private:
  friend class Dialog;
  std::vector<std::unique_ptr<Dialog>> dialogs;
  static ImGuiIO &baseInit(ImGuiConfigFlags flags);
  ImGuiIO &io;
  std::vector<FileDialog> fileDialogs;
  cppcoro::generator<std::size_t> idGen = iota<std::size_t>();

  std::vector<std::unique_ptr<Window>> windows;

  toml::table config;

  void removeDialog(Dialog &dialog);
};

}// namespace pf::ui::ig
#endif//PF_IMGUI_IMGUIINTERFACE_H
