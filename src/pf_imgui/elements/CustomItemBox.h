/**
 * @file CustomItemBox.h
 * @brief Customizable item container.
 * @author Petr Flajšingr
 * @date 4.6.21
 */

#ifndef PF_IMGUI_ELEMENTS_CUSTOMITEMBOX_H
#define PF_IMGUI_ELEMENTS_CUSTOMITEMBOX_H

#include <concepts>
#include <functional>
#include <memory>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Renderable.h>
#include <range/v3/range/conversion.hpp>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace pf::ui::ig {

/**
 * Factory of Box's items.
 * @tparam F factory
 * @tparam T input/storage type
 * @tparam R resulting renderable for row
 */
template<typename F, typename T, typename R>
concept CustomItemBoxFactory = std::is_invocable_r_v<std::unique_ptr<R>, F, T>;

/**
 * @brief Box containing items provided by user based on stored type.
 *
 * Items are created using a factory from stored value.
 * @tparam T type stored in each row
 * @tparam R type stored in each row
 */
template<typename T, std::derived_from<Renderable> R>
class PF_IMGUI_EXPORT CustomItemBox : public ItemElement, public RenderablesContainer {
 public:
  /**
   * Construct CustomItemBox
   * @param elementName id of the element
   * @param rowFactory factory for row creation
   */
  CustomItemBox(std::string_view elementName, CustomItemBoxFactory<T, R> auto &&rowFactory);

  /**
   * Add item to the end of the list.
   * @param item item to be added
   * @return reference to the Renderable representing the newly created row
   */
  R &addItem(const T &item);

  /**
   * Add multiple item to the end of the list.
   * @param newItems items to be added
   * @return references to the newly created rows
   */
  auto addItems(std::ranges::range auto &&newItems)
    requires(std::convertible_to<std::ranges::range_value_t<decltype(newItems)>, T>)
  {
    std::ranges::for_each(newItems, [this](const auto &newItem) { items.emplace_back(newItem, factory(newItem)); });
    refilterItems();
    return items | ranges::views::transform([](auto &item) -> T & { return *item; });
  }

  /**
   * Clear the listbox and set new items.
   * @param newItems items to set
   * @return references to the newly created rows
   */
  auto setItems(std::ranges::range auto &&newItems)
    requires(std::convertible_to<std::ranges::range_value_t<decltype(newItems)>, T>)
  {
    items.clear();
    return addItems(std::forward<decltype(newItems)>(newItems));
  }

  /**
   * Remove an item.
   * @param itemToRemove item to remove
   */
  void removeItem(const T &itemToRemove)
    requires(std::equality_comparable<T>)
  {
    std::erase_if(items, [this, &itemToRemove](const auto &item) { return item.first == itemToRemove; });
    refilterItems();
  }
  /**
   * Remove all items for which predicate returns true.
   * @param predicate predicate for item removal
   */
  void removeItemIf(std::predicate<const T &> auto &&predicate);
  /**
   * Remove all items.
   */
  void clearItems();
  /**
   * Set filter by which shown items are filtered.
   * @param filterFnc
   */
  void setFilter(std::predicate<const T &> auto filterFnc);
  /**
   * Remove filter.
   */
  void clearFilter();
  /**
   * Get reference to all items.
   * @return references to all items
   */
  [[nodiscard]] auto getItems() {
    return items | std::views::transform([](auto &item) -> T & { return item.first; });
  }
  /**
   * Get reference to all items.
   * @return references to all items
   */
  [[nodiscard]] auto getItems() const {
    return items | std::views::transform([](auto &item) -> const T & { return item.first; });
  }

  /**
   * Set item factory which will be used for any items added after this.
   * @param rowFactory
   */
  void setItemFactory(CustomItemBoxFactory<T, R> auto &&rowFactory);

  std::vector<Renderable *> getRenderables() override;

 protected:
  virtual void refilterItems();

  using Item = std::pair<T, std::unique_ptr<R>>;
  std::vector<Item> items;
  std::vector<Item *> filteredItems;
  std::function<std::unique_ptr<R>(const T &)> factory;
  std::function<bool(const T &)> filter = [](const auto &) { return true; };
};

template<typename T, std::derived_from<Renderable> R>
CustomItemBox<T, R>::CustomItemBox(std::string_view elementName, CustomItemBoxFactory<T, R> auto &&rowFactory)
    : ItemElement(elementName), factory(std::forward<decltype(rowFactory)>(rowFactory)) {}

template<typename T, std::derived_from<Renderable> R>
R &CustomItemBox<T, R>::addItem(const T &item) {
  auto &result = *items.emplace_back(item, factory(item)).second.get();
  refilterItems();
  return result;
}

template<typename T, std::derived_from<Renderable> R>
void CustomItemBox<T, R>::removeItemIf(std::predicate<const T &> auto &&predicate) {
  std::erase_if(items, [p = std::forward<decltype(predicate)>(predicate)](const auto &item) { return p(item.first); });
  refilterItems();
}

template<typename T, std::derived_from<Renderable> R>
void CustomItemBox<T, R>::clearItems() {
  items.clear();
  refilterItems();
}

template<typename T, std::derived_from<Renderable> R>
void CustomItemBox<T, R>::setFilter(std::predicate<const T &> auto filterFnc) {
  filter = filterFnc;
  refilterItems();
}

template<typename T, std::derived_from<Renderable> R>
void CustomItemBox<T, R>::clearFilter() {
  filter = [](auto) { return true; };
  refilterItems();
}

template<typename T, std::derived_from<Renderable> R>
void CustomItemBox<T, R>::setItemFactory(CustomItemBoxFactory<T, R> auto &&rowFactory) {
  factory = std::forward<decltype(rowFactory)>(rowFactory);
}

template<typename T, std::derived_from<Renderable> R>
std::vector<Renderable *> CustomItemBox<T, R>::getRenderables() {
  return items | std::views::transform([](auto &child) -> Renderable * { return child.second.get(); })
      | ranges::to_vector;
}

template<typename T, std::derived_from<Renderable> R>
void CustomItemBox<T, R>::refilterItems() {
  filteredItems = items | std::views::filter([this](auto &item) { return filter(item.first); })
      | std::views::transform([](auto &item) { return &item; }) | ranges::to_vector;
}
}  // namespace pf::ui::ig

#endif  // PF_IMGUI_ELEMENTS_CUSTOMITEMBOX_H
