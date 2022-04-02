/**
* @file NodeEditor.h
* @brief Node for NodeEditor.
* @author Petr Flajšingr
* @date 27.3.22
*/

#ifndef PF_IMGUI_NODE_EDITOR_NODE_H
#define PF_IMGUI_NODE_EDITOR_NODE_H

#include "NodeBase.h"
#include "Pin.h"
#include "fwd.h"
#include <imgui_node_editor.h>
#include <memory>
#include <pf_common/concepts/ranges.h>
#include <pf_imgui/elements/PopupMenu.h>
#include <pf_imgui/interface/Positionable.h>
#include <pf_imgui/interface/Renderable.h>
#include <pf_imgui/interface/Resizable.h>
#include <string>
#include <vector>

namespace pf::ui::ig {

/**
 * @brief A node in NodeEditor containing Pins
 */
class Node : public NodeBase {
  friend class NodeEditor;

 public:
  /**
   * Construct Node
   * @param name unique name of the element
   */
  explicit Node(const std::string &name);

  /**
   * Get all input pins
   * @return view to input pins
   */
  [[nodiscard]] auto getInputPins() {
    return inputPins | ranges::views::transform([](auto &pin) -> Pin & { return *pin; });
  }
  /**
   * Get all output pins
   * @return view to output pins
   */
  [[nodiscard]] auto getOutputPins() {
    return outputPins | ranges::views::transform([](auto &pin) -> Pin & { return *pin; });
  }

  /**
   * Get all input pins
   * @return view to input pins
   */
  [[nodiscard]] auto getInputPins() const {
    return inputPins | ranges::views::transform([](auto &pin) -> const Pin & { return *pin; });
  }
  /**
   * Get all output pins
   * @return view to output pins
   */
  [[nodiscard]] auto getOutputPins() const {
    return outputPins | ranges::views::transform([](auto &pin) -> const Pin & { return *pin; });
  }

  /**
   * Create a new input pin of given type.
   * @tparam T  type of the pin to create
   * @tparam Args types of arguments for Pin's constructor
   * @param args arguments for Pin's constructor
   * @return reference to the newly created Pin
   */
  template<std::derived_from<Pin> T = Pin, typename... Args>
  T &addInputPin(Args &&...args)
    requires std::constructible_from<T, Args...>
  {
    auto newPin = std::make_unique<T>(std::forward<Args>(args)...);
    auto result = newPin.get();
    inputPins.emplace_back(std::move(newPin));
    result->id = getNextPinId();
    result->parent = this;
    result->type = Pin::Type::Input;
    return *result;
  }

  /**
   * Create a new output pin of given type.
   * @tparam T  type of the pin to create
   * @tparam Args types of arguments for Pin's constructor
   * @param args arguments for Pin's constructor
   * @return reference to the newly created Pin
   */
  template<std::derived_from<Pin> T = Pin, typename... Args>
  T &addOutputPin(Args &&...args)
    requires std::constructible_from<T, Args...>
  {
    auto newPin = std::make_unique<T>(std::forward<Args>(args)...);
    auto result = newPin.get();
    outputPins.emplace_back(std::move(newPin));
    result->id = getNextPinId();
    result->parent = this;
    result->type = Pin::Type::Output;
    return *result;
  }

  /**
   * Add a listener called when the Node is de/selected.
   * @param listener listener
   * @return Subscription for listener unsubscription
   */
  Subscription addSelectionListener(std::invocable<bool> auto &&listener) {
    return observableSelected.addListener(std::forward<decltype(listener)>(listener));
  }

  /**
   * Add a listener called when the Node is double clicked.
   * @param listener listener
   * @return Subscription for listener unsubscription
   */
  Subscription addDoubleClickListener(std::invocable auto &&listener) {
    return observableDoubleClick.addListener(std::forward<decltype(listener)>(listener));
  }

  /**
   * Add a listener called when the Node is deleted.
   * @param listener listener
   * @return Subscription for listener unsubscription
   */
  Subscription addDeleteListener(std::invocable auto &&listener) {
    return observableDelete.addListener(std::forward<decltype(listener)>(listener));
  }

 protected:
  void renderImpl() override;

  /**
   * Render top part of the node.
   */
  virtual void renderHeader();
  /**
   * Render input pins on the left side of the node.
   */
  virtual void renderInputs();
  /**
   * Render space between input and output nodes.
   */
  virtual void renderMiddle();
  /**
   * Render output pins on the right side of the node.
   */
  virtual void renderOutputs();

 private:
  int getNextPinId();

  std::vector<std::unique_ptr<Pin>> inputPins;
  std::vector<std::unique_ptr<Pin>> outputPins;
};

}  // namespace pf::ui::ig

#endif  //PF_IMGUI_NODE_EDITOR_NODE_H
