/**
 * @file TextEditor.h
 * @brief TextEdiitor element.
 * @author Petr Flajšingr
 * @date 2.4.22
 */

#ifndef PF_IMGUI_ELEMENTS_TEXTEDITOR_H
#define PF_IMGUI_ELEMENTS_TEXTEDITOR_H

#include "common/TextEditorTypes.h"
#include <ImGuiColorTextEditor.h>
#include <pf_common/Explicit.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/common/Color.h>
#include <pf_imgui/common/Font.h>
#include <pf_imgui/common/Position.h>
#include <pf_imgui/common/Size.h>
#include <pf_imgui/interface/ElementWithID.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/reactive/Observable.h>
#include <range/v3/view/transform.hpp>

namespace pf::ui::ig {
// TODO: change observables to properties
/**
 * @brief Text editor with highlighting.
 */
class PF_IMGUI_EXPORT TextEditor : public ElementWithID, public Savable {
 public:
  /**
   * @brief Control struct for editor's cursor. Allows for control of its movement and selection adjustments.
   */
  class Cursor : protected PropertyOwner {
    friend class TextEditor;

   public:
    /**
     * Insert text at current cursor's position.
     * @param text text to insert
     */
    Cursor &insertTextAtCursorPos(const std::string &text);
    /**
     * Functions to move cursor.
     */
    Cursor &moveUp(std::uint32_t lineCnt = 1, bool addToSelection = false);
    Cursor &moveDown(std::uint32_t lineCnt = 1, bool addToSelection = false);
    Cursor &moveLeft(std::uint32_t charCnt = 1, bool addToSelection = false);
    Cursor &moveRight(std::uint32_t charCnt = 1, bool addToSelection = false);
    Cursor &moveTop(bool addToSelection = false);
    Cursor &moveBottom(bool addToSelection = false);
    Cursor &moveHome(bool addToSelection = false);
    Cursor &moveEnd(bool addToSelection = false);
    /**
     * Selection constrols.
     */
    Cursor &setSelectionStart(TextCursorPosition newPosition);
    Cursor &setSelectionEnd(TextCursorPosition newPosition);
    Cursor &selectAll();
    [[nodiscard]] bool hasSelection() const;
    Cursor &selectCurrentWord();
    Cursor &cutToClipboard();
    Cursor &pasteFromClipboard();
    Cursor &deleteSelection();
    [[nodiscard]] std::string getSelectedText() const;

    Property<TextCursorPosition> position;

   private:
    explicit Cursor(TextEditor &parent);
    TextEditor &owner;
  };
  /**
   * @brief Struct for construction of TextEditor.
   */
  struct Config {
    using Parent = TextEditor;
    Explicit<std::string_view> name; /*!< Unique name of the element */
    std::string_view value{};        /*!< Initial text inside the editor */
    Size size = Size::Auto();        /*!< Size of the element */
    bool persistent = false;         /*!< Allow state saving to disk */
  };
  /**
   * Construct TextEditor
   * @param config construction args @see TextEditor::Config
   */
  explicit TextEditor(Config &&config);
  /**
   * Construct TextEditor.
   * @param elementName unique name of the element
   * @param value initial text
   * @param s size of the element
   * @param persistent allow state saving to disk
   */
  TextEditor(std::string_view elementName, std::string_view value, Size s = Size::Auto(),
             Persistent persistent = Persistent::No);

  /**
   * Get color of token type.
   * @param colorType token type
   * @return
   */
  [[nodiscard]] Color getColor(TextEditorColor colorType) const;
  /**
   * Set color for given token type.
   * @param colorType token type
   * @param color new color
   */
  void setColor(TextEditorColor colorType, Color color);
  /**
   * @return all breakpoints currently active within the editor
   */
  [[nodiscard]] auto getBreakpoints() const {
    return editor.GetBreakpoints() | ranges::views::transform([](int bpLine) {
             return Breakpoint{static_cast<uint32_t>(bpLine), true, {}};
           });
  }
  /**
   * Remove all breakpoints.
   */
  void clearBreakpoints();
  /**
   * Add a breakpoint, which is rendered as a highlighted line.
   */
  void addBreakpoint(const Breakpoint &breakpoint);
  /**
   * Remove breakpoint on given line.
   */
  void removeBreakpoint(std::uint32_t line);
  /**
   * @return markers for errors
   */
  [[nodiscard]] auto getErrorMarkers() const {
    return editor.GetErrorMarkers() | ranges::views::transform([](const auto &errMarker) {
             return TextEditorMarker{static_cast<uint32_t>(errMarker.first), errMarker.second};
           });
  }
  /**
   * Remove all error markers.
   */
  void clearErrorMarkers();
  /**
   * @param marker error marker to add
   */
  void addErrorMarker(const TextEditorMarker &marker);
  /**
   * Remove error marker on given line.
   * @param line
   */
  void removeErrorMarker(std::uint32_t line);
  /**
   * @return markers for warnings
   */
  [[nodiscard]] auto getWarningMarkers() const {
    return editor.GetWarningMarkers() | ranges::views::transform([](const auto &errMarker) {
             return TextEditorMarker{static_cast<uint32_t>(errMarker.first), errMarker.second};
           });
  }
  /**
   * Remove all warning markers
   */
  void clearWarningMarkers();
  /**
   * @param marker marker to add
   */
  void addWarningMarker(const TextEditorMarker &marker);
  /**
   * Remove warning marker on given line.
   */
  void removeWarningMarker(std::uint32_t line);
  /**
   * @return size of tabs used for text rendering
   */
  [[nodiscard]] std::uint32_t getTabSize() const;
  /**
   * Set tab size used for text rendering.
   */
  void setTabSize(std::uint32_t tabSize);

  [[nodiscard]] ImGuiColorTextEdit::TextEditor &getEditor();
  [[nodiscard]] const ImGuiColorTextEdit::TextEditor &getEditor() const;
  /**
   * @return text inside the editor
   */
  [[nodiscard]] std::string getText() const;
  /**
   * Set text in the editor, replacing the old one.
   */
  void setText(const std::string &text);
  /**
   * @return true if undo can be used, false otherwise
   */
  [[nodiscard]] bool canUndo() const;
  /**
   * @return true if redo can be used, false otherwise
   */
  [[nodiscard]] bool canRedo() const;
  /**
   * Undo the last action.
   * @return true if anything was undone
   */
  bool undo();
  /**
   * Redo the last action.
   * @return true if anything was redone
   */
  bool redo();
  /**
   * @return true if the editor is in read only mode, false otherwise
   */
  [[nodiscard]] bool isReadOnly() const;
  /**
   * In read only mode user can not input any new data.
   */
  void setReadOnly(bool readOnly);

  /**
   * Set on of the premade highlights.
   * @param language premade language highlight
   */
  void setHighlighting(TextEditorHighlight language);

  void setHighlighting(const ImGuiColorTextEdit::TextEditor::LanguageDefinition &language);

  [[nodiscard]] toml::table toToml() const override;
  void setFromToml(const toml::table &src) override;

  Font font = Font::Default();

 private:
  ImGuiColorTextEdit::TextEditor editor;

 public:
  Cursor cursor{*this};

  Property<Size> size;

  Event<std::string_view> textChangeEvent;  // TODO: maybe replace with property with custom getter&setter?

 protected:
  void renderImpl() override;
};

}  // namespace pf::ui::ig

#endif  // PF_IMGUI_ELEMENTS_TEXTEDITOR_H
