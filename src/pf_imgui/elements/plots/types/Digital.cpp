//
// Created by petr on 11/8/20.
//

#include "Digital.h"
#include <implot.h>

namespace pf::ui::ig::plot_type {

Digital::Digital(std::string_view elementName, std::string_view caption) : LabeledPlotData(elementName, caption) {}

void Digital::renderImpl() {
  ImPlot::PlotDigital(label->get().c_str(), xData.data(), yData.data(), static_cast<int>(xData.size()));
}

}  // namespace pf::ui::ig::plot_type
