/**
 * @file Table.h
 * @brief Table element.
 * @author Petr Flajšingr
 * @date 12.4.21
 */

#ifndef PF_IMGUI_ELEMENTS_TABLE_H
#define PF_IMGUI_ELEMENTS_TABLE_H

#include <imgui.h>
#include <memory>
#include <pf_common/Explicit.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/enums.h>
#include <pf_imgui/interface/ElementWithID.h>
#include <pf_imgui/interface/RenderablesContainer.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/cache1.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <utility>
#include <vector>

// TODO: styles/fonts
namespace pf::ui::ig {

template<std::size_t ColumnCount>
struct PF_IMGUI_EXPORT TableSettings {
  std::optional<std::array<std::string, ColumnCount>> header = std::nullopt;
  Flags<TableBorder> border;   /*!< Type of border rendering. */
  Flags<TableOptions> options; /*!< Interaction options. */
  Size size = Size::Auto();    /*!< Size of the table. */
};

template<std::size_t ColumnCount>
class Table;
template<std::size_t ColumnCount, std::size_t RemainingCount = ColumnCount>
class TableRowBuilder;

/**
 * @brief Object representing a row in Table.
 * @tparam ColumnCount count of columns in row
 */
template<std::size_t ColumnCount>
class PF_IMGUI_EXPORT TableRow {
  friend class TableRowBuilder<ColumnCount>;

 public:
  using Cells = std::array<std::unique_ptr<Element>, ColumnCount>;
  /**
   * Construct TableRow.
   * @param owner owning table
   * @param rowId unique id within the table
   * @param rowCells cells for this row
   */
  TableRow(Table<ColumnCount> *owner, std::size_t rowId, Cells &&rowCells);
  /**
   * @return unique id within Row's table
   */
  [[nodiscard]] std::size_t getId() const { return id; }
  /**
   * @return index of row within table
   */
  [[nodiscard]] std::size_t getIndex() const;
  /**
   * Move row up by given count. Stops at 0.
   * @param count amount of rows to move by
   */
  void moveUp(std::uint32_t count = 1) { moveImpl(-static_cast<std::int32_t>(count)); }
  /**
   * Move row down by given count. Stops at last row.
   * @param count amount of rows to move by
   */
  void moveDown(std::uint32_t count = 1) { moveImpl(count); }
  /**
   * Swap with provided row.
   */
  void swapWith(const TableRow &otherRow);
  /**
   * Swap with row on given index.
   */
  void swapWith(std::size_t index);
  /**
   * @return references to all cells in this row
   */
  [[nodiscard]] auto getCells() {
    return cells | ranges::views::transform([](const auto &cell) -> Element & { return *cell; });
  }
  /**
   * @return const references to all cells in this row
   */
  [[nodiscard]] auto getCells() const {
    return cells | ranges::views::transform([](const auto &cell) -> const Element & { return *cell; });
  }

  [[nodiscard]] Element &operator[](std::size_t index) { return *cells[index]; }
  [[nodiscard]] const Element &operator[](std::size_t index) const { return *cells[index]; }

 private:
  void moveImpl(std::int32_t delta);

  Table<ColumnCount> *table;
  std::size_t id;
  Cells cells;
};

/**
 * @brief Builder for Table's rows.
 * @tparam ColumnCount total column count in table
 * @tparam RemainingCount remaining column count, internal use to allow build() function
 * Example:
 * // can access the new button via this variable
 * auto builder2 = table.buildRow()
 *                     .cell<Text>("text", "Text")
 *                     .cell<Button>("btn", "Button");
 * // accessing Button with operator->()
 * builder2->setColor<style::ColorOf::Button>(Color::Red);
 * // finishing up the row
 * auto &row = builder2.cell<DragInput<float>>("drag", "Drag", 1.f, 0.f, 100.f).build();
 */
template<std::size_t ColumnCount, std::size_t RemainingCount>
class PF_IMGUI_EXPORT TableRowBuilder {
 public:
  using Cells = typename TableRow<ColumnCount>::Cells;
  /**
   * @brief Wrapper to allow access for newly created ElementWithID when building Row.
   * @tparam T type of ElementWithID
   */
  template<std::derived_from<Element> T>
  class Result : public TableRowBuilder<ColumnCount, RemainingCount - 1> {
   public:
    Result(T &newElement, Table<ColumnCount> *ownerTable, Cells &&initCells)
        : TableRowBuilder<ColumnCount, RemainingCount - 1>(ownerTable, std::move(initCells)), element(newElement) {}

    T *operator->() { return &element; }

   private:
    T &element;
  };

 public:
  explicit TableRowBuilder(Table<ColumnCount> *ownerTable, Cells &&initCells)
      : table(ownerTable), cells(std::move(initCells)) {}
  /**
   * Create a new element within the cell. This invalidates the current builder and returns a new one.
   * @tparam T type of ElementWithID to create
   * @tparam Args types of construction args
   * @param args construction args
   * @return wrapper object serving as builder for the next row and accessor for the newly created element
   */
  template<std::derived_from<Element> T, typename... Args>
  Result<T> cell(Args &&...args)
    requires(RemainingCount > 0 && std::constructible_from<T, Args...>)
  {
    auto newElement = std::make_unique<T>(std::forward<Args>(args)...);
    auto newElementPtr = newElement.get();
    cells[ColumnCount - RemainingCount] = std::move(newElement);
    return Result<T>{*newElementPtr, table, std::move(cells)};
  }
  /**
   * Create a new element within the cell. This invalidates the current builder and returns a new one.
   * @tparam T config type for the new element
   * @param config config
* @return wrapper object serving as builder for the next row and accessor for the newly created element
   */
  template<ElementConstructConfig T>
  Result<typename T::Parent> cell(T &&config) {
    auto newElement = std::make_unique<typename T::Parent>(std::forward<T>(config));
    auto newElementPtr = newElement.get();
    cells[ColumnCount - RemainingCount] = std::move(newElement);
    return Result<typename T::Parent>{*newElementPtr, table, std::move(cells)};
  }
  TableRow<ColumnCount> &build()
    requires(RemainingCount == 0)
  {
    return buildRow();
  }

 private:
  TableRow<ColumnCount> &buildRow() {
    return table->addRow(std::make_unique<TableRow<ColumnCount>>(table, table->idCounter++, std::move(cells)));
  }

  Table<ColumnCount> *table;
  Cells cells;
};

/**
 * @brief A table containing any renderable element in each cell.
 *
 * @tparam ColumnCount static column count of the table
 */
template<std::size_t ColumnCount>
class PF_IMGUI_EXPORT Table : public ElementWithID, public RenderablesContainer {
  friend class TableRowBuilder<ColumnCount, 0>;
  friend class TableRow<ColumnCount>;

 public:
  /**
   * @brief Struct for construction of Table.
   */
  struct Config {
    using Parent = Table;
    Explicit<std::string_view> name;     /*!< Unique name of the element */
    TableSettings<ColumnCount> settings; /*!< Table behavior settings */
  };
  /**
   * Construct Table
   * @param config construction args @see Table::Config
   */
  explicit Table(Config &&config);
  /**
   * Construct Table.
   * @param elementName unique name of the element
   * @param settings rendering and behavioral settings
   */
  Table(std::string_view elementName, const TableSettings<ColumnCount> &settings);
  /**
   * @return builder object to create a new row @see TableRowBuilder
   */
  [[nodiscard]] TableRowBuilder<ColumnCount> buildRow() { return TableRowBuilder<ColumnCount>{this, {}}; }

  std::vector<Renderable *> getRenderables() override;

  [[nodiscard]] constexpr static std::size_t getColumnCount() { return ColumnCount; }

  [[nodiscard]] std::size_t getRowCount() const { return rows.size(); }

  /**
   * Set a sorting function for given column. This enabled sorting for the column by clicking header cells.
   * @param columnIndex index of the column to apply sorting function too
   * @param pred sorting predicate
   */
  void setSortFncForColumn(std::size_t columnIndex, std::predicate<const Element &, const Element &> auto &&pred);

  Property<Size> size;

 protected:
  void renderImpl() override;

 private:
  [[nodiscard]] TableRow<ColumnCount> &addRow(std::unique_ptr<TableRow<ColumnCount>> newRow) {
    return *rows.emplace_back(std::move(newRow));
  }

  static ImGuiTableFlags CreateFlags(const Flags<TableBorder> &tableBorder, const Flags<TableOptions> &options);

  std::optional<std::array<std::string, ColumnCount>> header;
  ImGuiTableFlags flags;

  std::vector<std::unique_ptr<TableRow<ColumnCount>>> rows;

  using ColumnSortFnc = std::function<bool(const Element &, const Element &)>;
  std::array<std::optional<ColumnSortFnc>, ColumnCount> columnSortFunctions{};

  std::size_t idCounter = 0;
};

template<std::size_t ColumnCount>
Table<ColumnCount>::Table(Table::Config &&config) : Table(config.name, config.settings) {}

template<std::size_t ColumnCount>
Table<ColumnCount>::Table(std::string_view elementName, const TableSettings<ColumnCount> &settings)
    : ElementWithID(elementName), size(settings.size), header(settings.header),
      flags(CreateFlags(settings.border, settings.options)) {}

template<std::size_t ColumnCount>
std::vector<Renderable *> Table<ColumnCount>::getRenderables() {
  return rows | ranges::views::transform([](auto &row) { return row->getCells(); }) | ranges::views::cache1
      | ranges::views::join | ranges::views::transform([](auto &child) -> Renderable * { return &child; })
      | ranges::to_vector;
}

template<std::size_t ColumnCount>
void Table<ColumnCount>::setSortFncForColumn(std::size_t columnIndex,
                                             std::predicate<const Element &, const Element &> auto &&pred) {
  columnSortFunctions[columnIndex] = std::forward<decltype(pred)>(pred);
}

template<std::size_t ColumnCount>
void Table<ColumnCount>::renderImpl() {
  [[maybe_unused]] auto colorScoped = color.applyScoped();
  [[maybe_unused]] auto styleScoped = style.applyScoped();
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();
  if (ImGui::BeginTable(getName().c_str(), ColumnCount, flags, static_cast<ImVec2>(*size))) {
    RAII end{ImGui::EndTable};

    std::ranges::for_each(std::views::iota(std::size_t{0}, ColumnCount), [&](const auto index) {
      const char *name = nullptr;
      if (header.has_value()) { name = (*header)[index].c_str(); }
      const auto columnFlags =
          columnSortFunctions[index].has_value() ? ImGuiTableColumnFlags_None : ImGuiTableColumnFlags_NoSort;
      ImGui::TableSetupColumn(name, columnFlags, 0.f, static_cast<ImGuiID>(index));
    });
    if (header.has_value()) { ImGui::TableHeadersRow(); }

    std::ranges::for_each(rows, [](auto &row) {
      ImGui::TableNextRow();
      std::ranges::for_each(row->getCells(), [](auto &cell) {
        if (ImGui::TableNextColumn()) { cell.render(); }
      });
    });

    if (const auto sortSpecs = ImGui::TableGetSortSpecs(); sortSpecs->SpecsDirty) {
      const auto sortPred = [&](const auto &lhs, const auto &rhs) {
        for (auto specsIndex : std::views::iota(0, sortSpecs->SpecsCount)) {
          const auto columnSpecs = sortSpecs->Specs[specsIndex];
          const auto columnIndex = columnSpecs.ColumnUserID;
          if (columnSortFunctions[columnIndex].has_value()) {
            auto comparisonResult = columnSortFunctions[columnIndex].value()((*lhs)[columnIndex], (*rhs)[columnIndex]);
            if (columnSpecs.SortDirection == ImGuiSortDirection_Descending) { comparisonResult = !comparisonResult; }
            if (comparisonResult) { return true; }
          }
        }
        return false;
      };

      std::ranges::sort(rows, sortPred);

      sortSpecs->SpecsDirty = false;
    }
  }
}

template<std::size_t ColumnCount>
ImGuiTableFlags Table<ColumnCount>::CreateFlags(const Flags<TableBorder> &tableBorder,
                                                const Flags<TableOptions> &options) {
  return ImGuiTableFlags{*tableBorder | *options | ImGuiTableFlags_Sortable};
}

template<std::size_t ColumnCount>
TableRow<ColumnCount>::TableRow(Table<ColumnCount> *owner, std::size_t rowId, TableRow::Cells &&rowCells)
    : table(owner), id(rowId), cells(std::move(rowCells)) {}

template<std::size_t ColumnCount>
std::size_t TableRow<ColumnCount>::getIndex() const {
  return std::ranges::distance(table->rows.begin(), std::ranges::find(table->rows, id, &TableRow::id));
}

template<std::size_t ColumnCount>
void TableRow<ColumnCount>::swapWith(const TableRow &otherRow) {
  const auto thisIter = std::ranges::find(table->rows, id, &TableRow::id);
  const auto otherIter = std::ranges::find(table->rows, otherRow.getId(), &TableRow::id);
  ranges::iter_swap(thisIter, otherIter);
}

template<std::size_t ColumnCount>
void TableRow<ColumnCount>::swapWith(std::size_t index) {
  const auto thisIter = std::ranges::find(table->rows, id, &TableRow::id);
  const auto otherIter = table->rows.begin() + index;
  ranges::iter_swap(thisIter, otherIter);
}

template<std::size_t ColumnCount>
void TableRow<ColumnCount>::moveImpl(std::int32_t delta) {
  const auto targetIndex =
      std::clamp(static_cast<std::int32_t>(getIndex()) + delta, 0, static_cast<std::int32_t>(table->rows.size()));
  const auto targetPosIter = table->rows.begin() + targetIndex;
  const auto targetIter = table->rows.insert(targetPosIter, nullptr);

  const auto originalPosIter = std::ranges::find(table->rows, id, [](const auto &cell) {
    if (cell == nullptr) { return static_cast<std::size_t>(-1); }
    return cell->id;
  });
  auto thisUnique = std::move(*originalPosIter);
  *targetIter = std::move(thisUnique);
  table->rows.erase(originalPosIter, originalPosIter + 1);
}

}  // namespace pf::ui::ig

#endif  // PF_IMGUI_ELEMENTS_TABLE_H
