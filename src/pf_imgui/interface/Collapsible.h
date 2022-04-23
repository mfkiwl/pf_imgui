/**
 * @file Collapsible.h
 * @brief Interface for elements which can be collapsed.
 * @author Petr Flajšingr
 * @date 23.1.21
 */

#ifndef PF_IMGUI_INTERFACE_COLLAPSIBLE_H
#define PF_IMGUI_INTERFACE_COLLAPSIBLE_H

#include <pf_imgui/_export.h>
#include <pf_imgui/enums.h>
#include <pf_imgui/interface/Observable_impl.h>
#include <pf_imgui/interface/Savable.h>
#include <utility>

namespace pf::ui::ig {

/**
 * @brief Interface for collapsible elements
 *
 * Provides the ability to add multiple observers to the elements' collapse state. Allow for collapse control.
 *
 * @details Interface inherits Savable, in order to save it's collapsed state.
 * @see Savable
 */
class PF_IMGUI_EXPORT Collapsible : public Savable {
 public:
  /**
   * Create Collapsible.
   * @param allowCollapse enable collapse capabilities
   * @param persistent enable config saving capabilities
   */
  explicit Collapsible(AllowCollapse allowCollapse, Persistent persistent);

  /**
   * Add a listener, which is called every time the elements' collapse state is changed.
   * @param listener listener for collapse change event, true is passed when the element is collapsed, false otherwise
   * @return instance of Subscription, which allows to unsubscribe the listener
   * @see Subscription
   */
  Subscription addCollapseListener(std::invocable<bool> auto &&listener) {
    return observableImpl.addListener(std::forward<decltype(listener)>(listener));
  }

  /**
   * Check if element is currently collapsed.
   * @return true if element is collapsed, false otherwise
   */
  [[nodiscard]] bool isCollapsed() const;
  /**
   * Set collapse state.
   * @param collapsed element will be collapsed if true is provided, otherwise it will be extended
   */
  virtual void setCollapsed(bool collapsed);

  /**
   * Check if element has it's collapse capability enabled.
   * @return true if collapse capability is enabled, false otherwise
   */
  [[nodiscard]] bool isCollapsible() const;
  /**
   * Set collapse capability enabled state.
   * @param collapsible element will be collapsible if true is provided, otherwise it will not
   */
  virtual void setCollapsible(bool collapsible);

  [[nodiscard]] toml::table toToml() const override;
  void setFromToml(const toml::table &src) override;

 private:
  /**
   * Notify all listeners of change in collapse state.
   * @param collapse new collapse state
   */
  void notifyCollapseChanged(bool collapse);
  /**
   * Current state of Collapsible.
   */
  bool collapsed = false;
  /**
   * Current state of collapse capability.
   */
  bool collapsible;
  Observable_impl<bool> observableImpl;
};
}  // namespace pf::ui::ig

#endif  // PF_IMGUI_INTERFACE_COLLAPSIBLE_H
