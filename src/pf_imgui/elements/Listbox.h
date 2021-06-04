/**
 * @file ListBox.h
 * @brief ListBox element.
 * @author Petr Flajšingr
 * @date 1.11.20
 */

#ifndef PF_IMGUI_ELEMENTS_LISTBOX_H
#define PF_IMGUI_ELEMENTS_LISTBOX_H

#include <algorithm>
#include <pf_common/concepts/StringConvertible.h>
#include <pf_common/coroutines/Sequence.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/elements/CustomListbox.h>
#include <pf_imgui/elements/Selectable.h>
#include <pf_imgui/interface/DragNDrop.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/unique_id.h>
#include <range/v3/view/addressof.hpp>
#include <string>
#include <vector>

namespace pf::ui::ig {

namespace details {
template<ToStringConvertible T>
struct ListboxRowFactory {
  static inline cppcoro::generator<std::size_t> idGenerator = iota<std::size_t>();
  const std::string idStart = uniqueId();
  std::unique_ptr<Selectable> operator()(const T &item) {
    return std::make_unique<Selectable>(idStart + std::to_string(getNext(idGenerator)), toString(item));
  }
};
static_assert(CustomListboxRowFactory<ListboxRowFactory<int>, int, Selectable>);
}// namespace details

/**
 * @brief Container for strings shown as list box and selectable by user.
 *
 * User selection can be observed via listeners.
 * @todo: relax requires
 * @todo: handle pointers as if references
 */
template<ToStringConvertible T>
class PF_IMGUI_EXPORT Listbox : public CustomListbox<T, Selectable>,
                                public ValueObservable<T>,
                                public Savable,
                                public DragSource<T>,
                                public DropTarget<T> {
  using CustomListbox<T, Selectable>::filteredItems;
  using CustomListbox<T, Selectable>::items;
  using CustomListbox<T, Selectable>::filter;

 public:
  using Factory = details::ListboxRowFactory<T>;
  using CustomListbox<T, Selectable>::getName;
  using CustomListbox<T, Selectable>::getLabel;
  using CustomListbox<T, Selectable>::getSize;
  using CustomListbox<T, Selectable>::setItems;
  using CustomListbox<T, Selectable>::addItem;
  using CustomListbox<T, Selectable>::addItems;
  using CustomListbox<T, Selectable>::removeItem;
  using CustomListbox<T, Selectable>::removeItemIf;
  using CustomListbox<T, Selectable>::setFilter;
  using CustomListbox<T, Selectable>::clearFilter;
  using CustomListbox<T, Selectable>::getItems;
  /**
   * Construct Listbox.
   * @param elementName ID of the element
   * @param label text rendered at the top of the list
   * @param s size of the element
   * @param selectedIdx starting selected id
   * @param persistent enable/disable state saving to disk
   */
  Listbox(
      const std::string &elementName, const std::string &label, Size s = Size::Auto(),
      std::optional<int> selectedIdx = std::nullopt,
      Persistent persistent = Persistent::No) requires(std::is_default_constructible_v<T> &&std::copy_constructible<T>)
      : CustomListbox<T, Selectable>(elementName, label, Factory{}, s), ValueObservable<T>(),
        Savable(persistent), DragSource<T>(false), DropTarget<T>(false), selectedItemIndex(selectedIdx) {}

  /**
   * Add item to the end of the list.
   * @param item item to be added
   * @param selected when selected the item is activated in the same way as if the user clicked on it
   * and observers are notified
   */
  Selectable &addItem(const T &item, Selected selected) {
    auto &result = addItem(item);
    if (selected == Selected::Yes) { setSelectedItem(item); }
    return result;
  }
  /**
   * Get and item currently selected by the user.
   * @param data
   */
  [[nodiscard]] std::optional<std::reference_wrapper<T>> getSelectedItem() const {
    if (selectedItemIndex.has_value()) { return filteredItems[*selectedItemIndex]->first; }
    return std::nullopt;
  }

  /**
   * Set selected item by name. If no such item is found nothing happens.
   * @param itemToSelect item to select
   */
  void setSelectedItem(const T &itemToSelect) requires(!std::same_as<T, std::string>) {
    if constexpr (std::equality_comparable<T>) {
      if (const auto iter = std::ranges::find_if(
              filteredItems, [&itemToSelect](const auto &item) { return item->first == itemToSelect; });
          iter != filteredItems.end()) {
        const auto index = std::distance(filteredItems.begin(), iter);
        setSelectedItemByIndex(index);
      }
    } else {
      const auto itemAsString = toString(itemToSelect);
      setSelectedItem(itemAsString);
    }
  }

  /**
   * Set selected item by name. If no such item is found nothing happens.
   * @param itemAsString string representation of item to select
   */
  void setSelectedItem(const std::string &itemAsString) {
    if (const auto iter = std::ranges::find_if(
            filteredItems, [itemAsString](const auto &item) { return item->second == itemAsString; });
        iter != filteredItems.end()) {
      const auto index = std::distance(filteredItems.begin(), iter);
      setSelectedItemByIndex(index);
    }
  }

  /**
   * Select item by its index.
   * @param index index to select
   */
  void setSelectedItemByIndex(std::size_t index) {
    assert(index < items.size());
    if (index != selectedItemIndex) {
      if (selectedItemIndex.has_value()) { filteredItems[*selectedItemIndex]->second->setValue(false); }
      selectedItemIndex = index;
      filteredItems[*selectedItemIndex]->second->setValue(true);
      ValueObservable<T>::setValueInner(filteredItems[index]->first);
      ValueObservable<T>::notifyValueChanged();
    }
  }

 protected:
  void renderImpl() override {
    if (ImGui::BeginListBox(getLabel().c_str(), getSize().asImVec())) {
      std::ranges::for_each(filteredItems | ranges::views::enumerate, [this](const auto &itemIdx) {
        const auto &[idx, item] = itemIdx;
        item->second->render();
        if (item->second->getValue()) { setSelectedItemByIndex(idx); }
      });
      ImGui::EndListBox();
    }
    if (auto drop = DropTarget<T>::dropAccept(); drop.has_value()) { addItem(*drop); }
    if (selectedItemIndex.has_value()) { DragSource<T>::drag(filteredItems[*selectedItemIndex]->first); }
  }

  void refilterItems() override {
    auto origItem = selectedItemIndex.has_value() ? filteredItems[*selectedItemIndex] : nullptr;
    filteredItems = items | std::views::filter([this](auto &item) { return filter(item.first); })
        | std::views::transform([](auto &item) { return &item; }) | ranges::to_vector;
    selectedItemIndex = std::nullopt;
    if (origItem != nullptr) {
      for (const auto &[idx, item] : filteredItems | ranges::views::enumerate) {
        if (item == origItem) {
          selectedItemIndex = idx;
          break;
        }
      }
    }
  }

  void unserialize_impl(const toml::table &src) override {
    if (src.contains("selected")) {
      const auto idx = *src["selected"].value<int>();
      if (static_cast<std::size_t>(idx) < items.size()) {
        selectedItemIndex = idx;
        ValueObservable<T>::setValueAndNotifyIfChanged(items[idx].first);
      }
    }
  }

  toml::table serialize_impl() override {
    auto result = toml::table{};
    if (selectedItemIndex.has_value()) {
      const auto selectedItem = filteredItems[*selectedItemIndex];
      auto itemsWithIndices = items | ranges::views::addressof | ranges::views::enumerate | ranges::to_vector;
      const auto indexInAllItems =
          static_cast<int>(std::ranges::find_if(itemsWithIndices, [selectedItem](const auto &itemInfo) {
                             return itemInfo.second->second.get() == selectedItem->second.get();
                           })->first);
      result.insert_or_assign("selected", indexInAllItems);
    }
    return result;
  }

 private:
  std::optional<int> selectedItemIndex = std::nullopt;
};
}// namespace pf::ui::ig
#endif//PF_IMGUI_ELEMENTS_LISTBOX_H
