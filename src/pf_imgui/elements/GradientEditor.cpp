//
// Created by xflajs00 on 03.04.2022.
//

#include "GradientEditor.h"
#include <imgui_color_gradient.h>

namespace pf::ui::ig {

bool GradientPoint::operator==(const GradientPoint &rhs) const {
  return color == rhs.color && position == rhs.position;  //-V550
}

std::optional<GradientPoint> GradientPoint::FromToml(const toml::table &src) {
  GradientPoint result;
  if (auto colorIter = src.find("color"); colorIter != src.end()) {
    if (auto colorArr = colorIter->second.as_array(); colorArr != nullptr && colorArr->size() == 4) {
      result.color = Color::RGB((*colorArr)[0].value_or(1.f), (*colorArr)[1].value_or(1.f),
                                (*colorArr)[2].value_or(1.f), (*colorArr)[3].value_or(1.f));
    } else {
      return std::nullopt;
    }
  } else {
    return std::nullopt;
  }
  if (auto posIter = src.find("position"); posIter != src.end()) {
    if (auto posVal = posIter->second.value<float>(); posVal.has_value()) {
      result.position = posVal.value();
    } else {
      return std::nullopt;
    }
  } else {
    return std::nullopt;
  }
  return result;
}

toml::table GradientPoint::toToml() const {
  return toml::table{{"color", toml::array{color.red(), color.green(), color.blue(), color.alpha()}},
                     {"position", position}};
}

GradientPoint details::GradientMarkToGradientPoint::operator()(ImGradientMark *mark) const {
  return {Color::RGB(mark->color[0], mark->color[1], mark->color[2], mark->color[3]), mark->position};
}

bool GradientPointsViewComparator::operator()(GradientPointsView lhs, GradientPointsView rhs) {
  return std::ranges::all_of(ranges::views::zip(lhs, rhs), [](const auto &val) { return val.first == val.second; });
}

GradientEditor::GradientEditor(GradientEditor::Config &&config)
    : GradientEditor(config.name, config.persistent ? Persistent::Yes : Persistent::No) {}

GradientEditor::GradientEditor(std::string_view elementName, Persistent persistent)
    : ElementWithID(elementName), Savable(persistent), points(getPointsView()) {}

Color GradientEditor::getColorAt(float percentage) const {
  float color[4];
  gradient->getColorAt(percentage, color);
  return Color::RGB(color[0], color[1], color[2], color[3]);
}

void GradientEditor::addGradientPoint(GradientPoint gradientPoint) {
  gradient->addMark(gradientPoint.position, static_cast<ImVec4>(gradientPoint.color));
}

void GradientEditor::removeGradientPoint(GradientPoint gradientPoint) {
  auto tomlPoints = getPointsView();
  auto iter = tomlPoints.begin();
  for (; iter != tomlPoints.end(); ++iter) {
    if (*iter == gradientPoint) { break; }
  }
  if (iter == tomlPoints.end()) { return; }
  const auto removeIndex = std::ranges::distance(tomlPoints.begin(), iter);
  const auto removeIter = std::ranges::next(gradient->getMarks().begin(), removeIndex);
  gradient->removeMark(*removeIter);
}

void GradientEditor::renderImpl() {
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();
  if (ImGui::GradientEditor(gradient.get(), draggingMark, selectedMark)) { *Prop_modify(points) = getPointsView(); }
  *Prop_modify(hovered) = gradient->hovered;
  *Prop_modify(focused) = gradient->focused;
  if (*focused && ImGui::IsKeyPressed(ImGuiKey_Delete) && selectedMark != nullptr) {
    gradient->removeMark(selectedMark);
    *Prop_modify(points) = getPointsView();
  }
}

toml::table GradientEditor::toToml() const {
  auto tomlPoints = toml::array{};
  std::ranges::for_each(getPointsView(), [&tomlPoints](const auto &point) { tomlPoints.push_back(point.toToml()); });
  return toml::table{{"points", tomlPoints}};
}

void GradientEditor::setFromToml(const toml::table &src) {
  if (auto pointsIter = src.find("points"); pointsIter != src.end()) {
    if (auto pointsArr = pointsIter->second.as_array(); pointsArr != nullptr) {
      std::ranges::for_each(*pointsArr, [this](const auto &pointToml) {
        if (auto pointTable = pointToml.as_table(); pointTable != nullptr) {
          if (auto point = GradientPoint::FromToml(*pointTable); point.has_value()) { addGradientPoint(*point); }
        }
      });
    }
  }
}

GradientPointsView GradientEditor::getPointsView() const {
  return gradient->getMarks() | ranges::views::transform(details::GradientMarkToGradientPoint{});
}
const GradientPointsView &GradientEditor::getValue() const { return *points; }

Subscription GradientEditor::addValueListenerImpl(std::function<void(const GradientPointsView &)> listener) {
  return points.addListener(std::move(listener));
}

}  // namespace pf::ui::ig
