//
// Created by petr on 11/8/20.
//

#include "PlotDataBase.h"
#include <string>

namespace pf::ui::ig::plot_type {

LabeledPlotData::LabeledPlotData(const std::string &elementName, const std::string &caption)
    : PlotData(elementName), Labellable(caption) {}

PlotData::PlotData(const std::string &elementName) : Element(elementName) {}

}  // namespace pf::ui::ig::plot_type
