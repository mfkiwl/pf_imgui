//
// Created by petr on 6/28/21.
//

#include "TestNodeEdit.h"

namespace pf::ui::ig {
TestNodeEdit::TestNodeEdit(const std::string &name) : Element(name) {}
TestNodeEdit::~TestNodeEdit() { ax::NodeEditor::DestroyEditor(context); }
void TestNodeEdit::renderImpl() {
  uint32_t id = 0;
  ax::NodeEditor::SetCurrentEditor(context);
  ax::NodeEditor::Begin(getName().c_str() /* getSize() */);

  ax::NodeEditor::BeginNode(id++);
  ImGui::Text("Test");
  ax::NodeEditor::BeginPin(id++, ax::NodeEditor::PinKind::Input);
  ImGui::Button("pin in");
  ax::NodeEditor::EndPin();
  ImGui::SameLine();
  ax::NodeEditor::BeginPin(id++, ax::NodeEditor::PinKind::Output);
  ImGui::Text("out");
  ax::NodeEditor::EndPin();
  ax::NodeEditor::EndNode();

  ax::NodeEditor::BeginNode(id++);
  ImGui::Text("Test");
  ax::NodeEditor::BeginPin(id++, ax::NodeEditor::PinKind::Input);
  ImGui::Button("pin in");
  ax::NodeEditor::EndPin();
  ImGui::SameLine();
  ax::NodeEditor::BeginPin(id++, ax::NodeEditor::PinKind::Output);
  ImGui::Text("out");
  ax::NodeEditor::EndPin();
  ax::NodeEditor::EndNode();

  ax::NodeEditor::End();
}
}// namespace pf::ui::ig