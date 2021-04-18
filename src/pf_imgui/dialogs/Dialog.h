/**
 * @file Dialog.h
 * @brief Dialog built by a user.
 * @author Petr Flajšingr
 * @date 31.10.20
 */

#ifndef PF_IMGUI_ELEMENTS_DIALOG_H
#define PF_IMGUI_ELEMENTS_DIALOG_H

#include <pf_imgui/_export.h>
#include <pf_imgui/fwd.h>
#include <pf_imgui/interface/ElementContainer.h>
#include <pf_imgui/interface/Labellable.h>
#include <pf_imgui/interface/Renderable.h>
#include <string>

namespace pf::ui::ig {

/**
 * @brief Simple dialog which can be modal. Contains Elements.
 *
 * This dialog is shown on top of the other windows. It should be created via ImGuiInterface::createDialog(...). Closing of the dialog should be managed by user.
 */
class PF_IMGUI_EXPORT Dialog : public Renderable, public ElementContainer, public Labellable {
 public:
  /**
   * Create Dialog with given name and label.
   * @param parent owner of the dialog - has to be present in order to render it
   * @param elementName ID of the dialog
   * @param label title
   * @param modal modality
   */
  Dialog(ImGuiInterface &parent, const std::string &elementName, const std::string &label, Modal modal = Modal::Yes);

  /**
   * Set flag for closing the dialog, which invalidates the dialog.
   * Dialog is invalidated in the next render call.
   * @warning The dialog should not be accessed after calling close().
   */
  void close();

  /**
   * Check if the dialog is closed.
   * @return true if closed false otherwise
   */
  [[nodiscard]] bool isClosed() const;

 protected:
  void renderImpl() override;

 private:
  Modal modal;
  bool closed = false;
  ImGuiInterface &owner;
};
}// namespace pf::ui::ig
#endif//PF_IMGUI_ELEMENTS_DIALOG_H
