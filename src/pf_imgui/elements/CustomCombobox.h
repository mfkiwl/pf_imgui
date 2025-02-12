/**
 * @file CustomCombobox.h
 * @brief Customizable combobox.
 * @author Petr Flajšingr
 * @date 4.6.21
 */

#ifndef PF_IMGUI_ELEMENTS_CUSTOMCOMBOBOX_H
#define PF_IMGUI_ELEMENTS_CUSTOMCOMBOBOX_H

#include <imgui.h>
#include <pf_common/enums.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/common/Label.h>
#include <pf_imgui/elements/CustomItemBox.h>
#include <utility>

namespace pf::ui::ig {

enum class ComboBoxCount { Items4 = 1 << 1, Items8 = 1 << 2, Items20 = 1 << 3, ItemsAll = 1 << 4 };

// TODO:
// ImGuiComboFlags_PopupAlignLeft
// ImGuiComboFlags_NoArrowButton
// ImGuiComboFlags_NoPreview

/**
 * @brief Combobox where rows can be whatever is provided by the user.
 *
 * Rows are created using a factory from stored value.
 * @tparam T type stored in each row
 * @tparam R type stored in each row
 */
template<typename T, std::derived_from<Renderable> R>
class PF_IMGUI_EXPORT CustomCombobox : public CustomItemBox<T, R> {
 protected:
  PF_IMGUI_PROPERTY_OWNER_ALIASES(U)

 public:
  /**
   * Construct CustomCombobox.
   * @param elementName ID of the element
   * @param labelText text rendered next to the element
   * @param rowFactory factory for renderable rows
   * @param prevValue preview value
   * @param showItemCount amount of items shown when open
   */
  CustomCombobox(std::string_view elementName, std::string_view labelText, CustomItemBoxFactory<T, R> auto &&rowFactory,
                 std::string_view prevValue = "", ComboBoxCount showItemCount = ComboBoxCount::Items8);

  void setPreviewValue(std::string value);
  [[nodiscard]] const std::string &getPreviewValue() const { return previewValue; }

  /**
   * Get count of items shown when the element is unrolled.
   * @return count of items
   */
  [[nodiscard]] ComboBoxCount getShownItemCount() const;
  /**
   * Set count of items shown when the element is unrolled.
   * @param shownItemCount count of items
   */
  void setShownItemCount(ComboBoxCount shownItemCount);

  /**
   * Close the Combobox in the next render loop.
   */
  void close() { shouldClose = true; }

  Property<Label> label;

 protected:
  void renderImpl() override;

  /**
   * Call closing impl now.
   */
  void closeNow() { ImGui::CloseCurrentPopup(); }

  /**
   * Check for closing during rendering.
   */
  void checkClose();

  Flags<ImGuiComboFlags_> flags{};

 private:
  std::string previewValue{};
  bool shouldClose = false;
};

template<typename T, std::derived_from<Renderable> R>
CustomCombobox<T, R>::CustomCombobox(std::string_view elementName, std::string_view labelText,
                                     CustomItemBoxFactory<T, R> auto &&rowFactory, std::string_view prevValue,
                                     ComboBoxCount showItemCount)
    : CustomItemBox<T, R>(elementName, std::forward<decltype(rowFactory)>(rowFactory)), label(std::string{labelText}),
      flags(static_cast<ImGuiComboFlags_>(showItemCount)), previewValue(std::string{prevValue}) {
  if (previewValue.empty()) { flags |= ImGuiComboFlags_::ImGuiComboFlags_NoPreview; }
}

template<typename T, std::derived_from<Renderable> R>
void CustomCombobox<T, R>::setPreviewValue(std::string value) {
  previewValue = std::move(value);
  if (previewValue.empty()) { flags |= ImGuiComboFlags_::ImGuiComboFlags_NoPreview; }
}

template<typename T, std::derived_from<Renderable> R>
ComboBoxCount CustomCombobox<T, R>::getShownItemCount() const {
  return static_cast<ComboBoxCount>(*(flags & ImGuiComboFlags_::ImGuiComboFlags_HeightMask_));
}

template<typename T, std::derived_from<Renderable> R>
void CustomCombobox<T, R>::setShownItemCount(ComboBoxCount shownItemCount) {
  flags &= static_cast<ImGuiComboFlags_>(~ImGuiComboFlags_::ImGuiComboFlags_HeightMask_);
  flags |= static_cast<ImGuiComboFlags_>(shownItemCount);
}

template<typename T, std::derived_from<Renderable> R>
void CustomCombobox<T, R>::renderImpl() {
  [[maybe_unused]] auto colorScoped = this->color.applyScoped();
  [[maybe_unused]] auto styleScoped = this->style.applyScoped();
  [[maybe_unused]] auto fontScoped = this->font.applyScopedIfNotDefault();
  const char *previewPtr = previewValue.c_str();
  if (ImGui::BeginCombo(label->get().c_str(), previewPtr, *flags)) {
    RAII end{ImGui::EndCombo};
    checkClose();
    std::ranges::for_each(CustomItemBox<T, R>::filteredItems, [](auto item) { item->second->render(); });
  }
}

template<typename T, std::derived_from<Renderable> R>
void CustomCombobox<T, R>::checkClose() {
  if (shouldClose) {
    closeNow();
    shouldClose = false;
  }
}
}  // namespace pf::ui::ig

#endif  // PF_IMGUI_ELEMENTS_CUSTOMCOMBOBOX_H
