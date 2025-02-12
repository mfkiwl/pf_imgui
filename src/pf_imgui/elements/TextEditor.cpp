//
// Created by petr.flajsingr on 2/4/2022.
//

#include "TextEditor.h"
#include <string>

namespace pf::ui::ig {

TextEditor::Cursor &TextEditor::Cursor::insertTextAtCursorPos(const std::string &text) {
  owner.editor.InsertText(text);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveUp(std::uint32_t lineCnt, bool addToSelection) {
  owner.editor.MoveUp(static_cast<int>(lineCnt), addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveDown(std::uint32_t lineCnt, bool addToSelection) {
  owner.editor.MoveDown(static_cast<int>(lineCnt), addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveLeft(std::uint32_t charCnt, bool addToSelection) {
  owner.editor.MoveLeft(static_cast<int>(charCnt), addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveRight(std::uint32_t charCnt, bool addToSelection) {
  owner.editor.MoveRight(static_cast<int>(charCnt), addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveTop(bool addToSelection) {
  owner.editor.MoveTop(addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveBottom(bool addToSelection) {
  owner.editor.MoveBottom(addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveHome(bool addToSelection) {
  owner.editor.MoveHome(addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::moveEnd(bool addToSelection) {
  owner.editor.MoveEnd(addToSelection);
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::selectCurrentWord() {
  owner.editor.SelectWordUnderCursor();
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::cutToClipboard() {
  owner.editor.Cut();
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::pasteFromClipboard() {
  owner.editor.Paste();
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::deleteSelection() {
  owner.editor.Delete();
  return *this;
}

std::string TextEditor::Cursor::getSelectedText() const { return owner.editor.GetSelectedText(); }

TextEditor::Cursor::Cursor(TextEditor &parent)
    : position(TextCursorPosition{.line = static_cast<uint32_t>(parent.editor.GetCursorPosition().mLine),
                                  .column = static_cast<uint32_t>(parent.editor.GetCursorPosition().mColumn)}),
      owner(parent) {
  position.addListener([this](auto newPosition) {
    owner.editor.SetCursorPosition({static_cast<int>(newPosition.line), static_cast<int>(newPosition.column)});
  });
}

TextEditor::Cursor &TextEditor::Cursor::setSelectionStart(TextCursorPosition newPosition) {
  owner.editor.SetSelectionStart({static_cast<int>(newPosition.line), static_cast<int>(newPosition.column)});
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::setSelectionEnd(TextCursorPosition newPosition) {
  owner.editor.SetSelectionEnd({static_cast<int>(newPosition.line), static_cast<int>(newPosition.column)});
  return *this;
}

TextEditor::Cursor &TextEditor::Cursor::selectAll() {
  owner.editor.SelectAll();
  return *this;
}

bool TextEditor::Cursor::hasSelection() const { return owner.editor.HasSelection(); }

TextEditor::TextEditor(TextEditor::Config &&config)
    : TextEditor(config.name, config.value, config.size, config.persistent ? Persistent::Yes : Persistent::No) {}

TextEditor::TextEditor(std::string_view elementName, std::string_view value, Size s, Persistent persistent)
    : ElementWithID(elementName), Savable(persistent), size(s) {
  editor.SetText(value);
}

ImGuiColorTextEdit::TextEditor &TextEditor::getEditor() { return editor; }

const ImGuiColorTextEdit::TextEditor &TextEditor::getEditor() const { return editor; }

std::string TextEditor::getText() const { return editor.GetText(); }

void TextEditor::setText(const std::string &text) { editor.SetText(text); }

void TextEditor::renderImpl() {
  [[maybe_unused]] auto scopedFont = font.applyScopedIfNotDefault();
  editor.Render(getName().c_str(), static_cast<ImVec2>(*size));
  if (editor.IsTextChanged()) { Event_notify(textChangeEvent, std::string_view{getText()}); }
  if (editor.IsCursorPositionChanged()) {
    *cursor.position.modify() = TextCursorPosition{.line = static_cast<uint32_t>(editor.GetCursorPosition().mLine),
                                                   .column = static_cast<uint32_t>(editor.GetCursorPosition().mColumn)};
  }
}

void TextEditor::setHighlighting(TextEditorHighlight language) {
  using enum TextEditorHighlight;
  switch (language) {
    case GLSL: editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::GLSL()); break;
    case HLSL: editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::HLSL()); break;
    case AngelScript:
      editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::AngelScript());
      break;
    case Lua: editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::Lua()); break;
    case C: editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::C()); break;
    case CPP: editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::CPlusPlus()); break;
    case SQL: editor.SetLanguageDefinition(ImGuiColorTextEdit::TextEditor::LanguageDefinition::SQL()); break;
  }
}

void TextEditor::setHighlighting(const ImGuiColorTextEdit::TextEditor::LanguageDefinition &language) {
  editor.SetLanguageDefinition(language);
}

toml::table TextEditor::toToml() const { return toml::table{{"text", editor.GetText()}}; }

void TextEditor::setFromToml(const toml::table &src) {
  if (auto newTextIter = src.find("text"); newTextIter != src.end()) {
    if (auto newText = newTextIter->second.as_string(); newText != nullptr) { editor.SetText(newText->get()); }
  }
}

Color TextEditor::getColor(TextEditorColor colorType) const {
  return Color{editor.GetPalette()[static_cast<int>(colorType)]};
}

void TextEditor::setColor(TextEditorColor colorType, Color color) {
  auto palette = editor.GetPalette();
  palette[static_cast<int>(colorType)] = color;
  editor.SetPalette(palette);
}

void TextEditor::clearBreakpoints() { editor.SetBreakpoints({}); }

void TextEditor::addBreakpoint(const Breakpoint &breakpoint) {
  auto breakpoints = editor.GetBreakpoints();
  breakpoints.emplace(breakpoint.line);
  editor.SetBreakpoints(breakpoints);
}

void TextEditor::removeBreakpoint(std::uint32_t line) {
  auto breakpoints = editor.GetBreakpoints();
  breakpoints.erase(static_cast<int>(line));
  editor.SetBreakpoints(breakpoints);
}

void TextEditor::clearErrorMarkers() { editor.SetErrorMarkers({}); }

void TextEditor::addErrorMarker(const TextEditorMarker &marker) {
  auto markers = editor.GetErrorMarkers();
  markers.emplace(marker.line, marker.text);
  editor.SetErrorMarkers(markers);
}

void TextEditor::removeErrorMarker(std::uint32_t line) {
  auto markers = editor.GetErrorMarkers();
  markers.erase(static_cast<int>(line));
  editor.SetErrorMarkers(markers);
}

void TextEditor::clearWarningMarkers() { editor.SetWarningMarkers({}); }

void TextEditor::addWarningMarker(const TextEditorMarker &marker) {
  auto markers = editor.GetWarningMarkers();
  markers.emplace(marker.line, marker.text);
  editor.SetWarningMarkers(markers);
}

void TextEditor::removeWarningMarker(std::uint32_t line) {
  auto markers = editor.GetWarningMarkers();
  markers.erase(static_cast<int>(line));
  editor.SetWarningMarkers(markers);
}

std::uint32_t TextEditor::getTabSize() const { return static_cast<std::uint32_t>(editor.GetTabSize()); }

void TextEditor::setTabSize(std::uint32_t tabSize) { editor.SetTabSize(static_cast<int>(tabSize)); }

bool TextEditor::canUndo() const { return editor.CanUndo(); }

bool TextEditor::canRedo() const { return editor.CanRedo(); }

bool TextEditor::undo() {
  auto result = canUndo();
  editor.Undo();
  return result;
}

bool TextEditor::redo() {
  auto result = canRedo();
  editor.Redo();
  return result;
}

bool TextEditor::isReadOnly() const { return editor.IsReadOnly(); }

void TextEditor::setReadOnly(bool readOnly) { editor.SetReadOnly(readOnly); }

}  // namespace pf::ui::ig
