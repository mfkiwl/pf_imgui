/**
 * @file PlotDataBase.h
 * @brief Types for data storage in plots and base class used in most storage types.
 * @author Petr Flajšingr
 * @date 8.11.20
 */

#ifndef PF_IMGUI_ELEMENTS_PLOTS_TYPES_PLOTDATABASE_H
#define PF_IMGUI_ELEMENTS_PLOTS_TYPES_PLOTDATABASE_H

#include <algorithm>
#include <concepts>
#include <pf_imgui/_export.h>
#include <pf_imgui/common/Label.h>
#include <pf_imgui/interface/ElementWithID.h>
#include <pf_imgui/reactive/Observable.h>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <vector>

namespace pf::ui::ig::plot_type {
/**
 * @brief A type which can be rendered using Plot and its descendants.
 */
template<typename T>
concept Plottable = std::convertible_to<T, double>;

/**
 * @brief Simple 2D data sample.
 */
template<Plottable T>
struct PF_IMGUI_EXPORT XYPlotData {
  T x;
  T y;
};

/**
 * @brief 2D data sample with an error marker.
 */
template<Plottable T>
struct PF_IMGUI_EXPORT XYErrorPlotData : XYPlotData<T> {
  T error;
};

/**
 * Type of Bar plot.
 */
enum class BarType { Horizontal, Vertical };

namespace details {
/**
 * @brief Default setter for data storage.
 */
class PF_IMGUI_EXPORT DefaultPlotDataSetting {
 public:
  /**
   * Set new plot data.
   * @param newData new data
   * @tparam type of data to plot
   * @todo plot data settings (line width etc.)
   */
  template<Plottable T>
  void setData(const std::ranges::range auto &newData)
    requires(std::same_as<std::ranges::range_value_t<decltype(newData)>, XYPlotData<T>>)
  {
    xData = newData | ranges::views::transform([](const auto &val) { return static_cast<double>(val.x); })
        | ranges::to_vector;
    const auto extremes = ranges::minmax(xData);
    width = extremes.max - extremes.min;
    yData = newData | ranges::views::transform([](const auto &val) { return static_cast<double>(val.y); })
        | ranges::to_vector;
  }

 protected:
  std::vector<double> xData;
  std::vector<double> yData;
  double width;
};
}  // namespace details

/**
 * @brief Base class for all types of plot data.
 */
class PF_IMGUI_EXPORT PlotData : public ElementWithID {
 public:
  explicit PlotData(std::string_view elementName);
};

/**
 * @brief Base class for plot data wich support data labeling.
 */
class PF_IMGUI_EXPORT LabeledPlotData : public PlotData {
 public:
  LabeledPlotData(std::string_view elementName, std::string_view caption);

  Property<Label> label;
};

}  // namespace pf::ui::ig::plot_type

#endif  // PF_IMGUI_ELEMENTS_PLOTS_TYPES_PLOTDATABASE_H
