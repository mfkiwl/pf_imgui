/**
 * @file Observable_impl.h
 * @brief Implementation of observable.
 * @author Petr Flajšingr
 * @date 23.1.21
 */

#ifndef PF_IMGUI_INTERFACE_OBSERVABLE_IMPL_H
#define PF_IMGUI_INTERFACE_OBSERVABLE_IMPL_H

#include <algorithm>
#include <memory>
#include <pf_common/Subscription.h>
#include <pf_imgui/_export.h>
#include <ranges>
#include <unordered_map>
#include <utility>

namespace pf::ui::ig {
/**
 * @brief Implementation of observable design pattern.
 *
 * Allows multiple observers, each of which has a unique ID.
 * @tparam Args Types of arguments to be used as callback parameters
 */
template<typename... Args>
class PF_IMGUI_EXPORT Observable_impl {
 public:
  Observable_impl() = default;
  Observable_impl(const Observable_impl &) = delete;
  Observable_impl &operator=(const Observable_impl &) = delete;

  Observable_impl(Observable_impl &&other) noexcept
      : listeners(std::move(other.listeners)), idCounter(idCounter) {}
  Observable_impl &operator=(Observable_impl &&rhs) noexcept {
    listeners = std::move(rhs.listeners);
    idCounter = rhs.idCounter;
    return *this;
  }

  virtual ~Observable_impl() { *exists = false; }

  /**
   * Add a listener and return a Subscription object which can be used to unregister it.
   * @param fnc listener
   * @return Subscription for unregistration purposes
   * @see Subscription
   */
  Subscription addListener(std::invocable<const Args &...> auto &&fnc) {
    const auto id = generateListenerId();
    listeners[id] = std::forward<decltype(fnc)>(fnc);
    return Subscription([id, this, observableExists = exists] {
      if (!*observableExists) { return; }
      listeners.erase(id);
    });
  }

  /**
   * Notify all listeners with provided parameters.
   * @param args parameters to be passed to listeners
   */
  void notify(const Args &...args) {
    auto callables = listeners | std::views::values;
    std::ranges::for_each(callables, [&](const auto &callable) { callable(args...); });
  }

 private:
  using ListenerId = uint32_t;
  using Callback = std::function<void(const Args &...)>;
  ListenerId generateListenerId() { return idCounter++; }

  std::unordered_map<ListenerId, Callback> listeners{};
  ListenerId idCounter{};
  std::shared_ptr<bool> exists = std::make_shared<bool>(true);
};
}// namespace pf::ui::ig
#endif//PF_IMGUI_INTERFACE_OBSERVABLE_IMPL_H
