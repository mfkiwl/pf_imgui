//
// Created by petr on 6/28/21.
//

#ifndef PF_IMGUI_SRC_PF_IMGUI_NODE_EDIT_TESTNODEEDIT_H
#define PF_IMGUI_SRC_PF_IMGUI_NODE_EDIT_TESTNODEEDIT_H

#include <imgui_node_editor.h>
#include <pf_imgui/interface/Element.h>

namespace pf::ui::ig {
class TestNodeEdit : public Element {
 public:
  explicit TestNodeEdit(const std::string &name);
  ~TestNodeEdit() override;

 protected:
  void renderImpl() override;

 private:
  ax::NodeEditor::EditorContext *context = ax::NodeEditor::CreateEditor();
};
}

#endif//PF_IMGUI_SRC_PF_IMGUI_NODE_EDIT_TESTNODEEDIT_H
