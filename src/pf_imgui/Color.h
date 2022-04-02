//
// Created by xflajs00 on 02.04.2022.
//

#ifndef PF_IMGUI_COLOR_H
#define PF_IMGUI_COLOR_H

#include <concepts>
#include <cstdint>
#include <imgui.h>
#include <pf_imgui/_export.h>

namespace pf::ui::ig {

/**
 * @brief Color for pf_imgui, optimized compared to using ImVec4.
 */
class PF_IMGUI_EXPORT Color {
 public:
  Color();
  explicit Color(ImColor value);
  explicit Color(const ImVec4 &value);

  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color Black;
  static const Color White;

  [[nodiscard]] static Color RGB(std::same_as<std::uint8_t> auto red, std::same_as<std::uint8_t> auto green,
                                 std::same_as<std::uint8_t> auto blue, std::same_as<std::uint8_t> auto alpha = 255u) {
    return Color{IM_COL32(red, green, blue, alpha)};
  }
  [[nodiscard]] static Color RGB(std::same_as<float> auto red, std::same_as<float> auto green,
                                 std::same_as<float> auto blue, std::same_as<float> auto alpha = 1.f) {
    return Color{ImColor(red, green, blue, alpha)};
  }

  [[nodiscard]] static Color HSV(std::same_as<std::uint8_t> auto hue, std::same_as<std::uint8_t> auto saturation,
                                 std::same_as<std::uint8_t> auto value, std::same_as<std::uint8_t> auto alpha = 255u) {
    float r;
    float g;
    float b;
    ImGui::ColorConvertHSVtoRGB(static_cast<float>(hue) / 255.f, static_cast<float>(saturation) / 255.f,
                                static_cast<float>(value) / 255.f, r, g, b);
    return RGB(r, g, b, alpha);
  }
  [[nodiscard]] static Color HSV(std::same_as<float> auto hue, std::same_as<float> auto saturation,
                                 std::same_as<float> auto value, std::same_as<float> auto alpha = 1.f) {
    float r;
    float g;
    float b;
    ImGui::ColorConvertHSVtoRGB(hue, saturation, value, r, g, b);
    return RGB(r, g, b, alpha);
  }

  [[nodiscard]] float red() const;
  [[nodiscard]] float green() const;
  [[nodiscard]] float blue() const;
  [[nodiscard]] float alpha() const;

  void setRed(std::same_as<std::uint8_t> auto red) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.x = static_cast<float>(red) / 1.f;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }
  void setGreen(std::same_as<std::uint8_t> auto green) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.y = static_cast<float>(green) / 1.f;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }
  void setBlue(std::same_as<std::uint8_t> auto blue) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.z = static_cast<float>(blue) / 1.f;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }
  void setAlpha(std::same_as<std::uint8_t> auto alpha) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.w = static_cast<float>(alpha) / 1.f;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }

  void setRed(std::same_as<float> auto red) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.x = red;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }
  void setGreen(std::same_as<float> auto green) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.y = green;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }
  void setBlue(std::same_as<float> auto blue) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.z = blue;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }
  void setAlpha(std::same_as<float> auto alpha) {
    auto col = static_cast<ImVec4>(ImColor{color});
    col.w = alpha;
    color = ImGui::ColorConvertFloat4ToU32(col);
  }

  operator ImU32() const;

 private:
  ImU32 color;
};

}  // namespace pf::ui::ig

#endif  //PF_IMGUI_COLOR_H
