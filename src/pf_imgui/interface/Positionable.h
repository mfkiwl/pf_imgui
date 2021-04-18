/**
 * @file Positionable.h
 * @brief Interface for elements which can be positioned.
 * @author Petr Flajšingr
 * @date 25.1.21
 */

#ifndef PF_IMGUI_INTERFACE_POSITIONABLE_H
#define PF_IMGUI_INTERFACE_POSITIONABLE_H

#include "Observable_impl.h"
#include <imgui.h>

namespace pf::ui::ig {

/**
 * @brief Interface for positionable elements
 *
 * Provides a functionality to manage and observe position for the subclass.
 */
class Positionable {
 public:
  /**
   * Construct Positionable with starting position.
   * @param position starting position
   */
  explicit Positionable(const ImVec2 &position);

  /**
   * Add a listener, which is called every time the elements' position is changed.
   * @param fnc listener for position change event, new position is passed to the listener
   * @return instance of Subscription, which allows to unsubscribe the listener
   * @see Subscription
   */
  Subscription addPositionListener(std::invocable<ImVec2> auto listener) {
    return observableImpl.template addListener(listener);
  }

  /**
   * Get current position.
   * @return current position
   */
  [[nodiscard]] ImVec2 getPosition() const;
  /**
   * Set new position.
   * @param pos new position
   */
  virtual void setPosition(ImVec2 pos);

  virtual ~Positionable() = default;

 protected:
  /**
   * Checks if the new position is different from the current one. If it is, listeners are notified of this change.
   * @param pos new position
   */
  void updatePosition(ImVec2 pos);

 private:
  ImVec2 position;
  Observable_impl<ImVec2> observableImpl;

  /**
   * Notify listeners of position change.
   * @param pos position to used as a parameter
   */
  void notifyPositionChanged(ImVec2 pos);
};
}// namespace pf::ui::ig

#endif//PF_IMGUI_INTERFACE_POSITIONABLE_H
