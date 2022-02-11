/**
* @file Image.h
* @brief Image element.
* @author Petr Flajšingr
* @date 31.10.20
*/

#ifndef PF_IMGUI_ELEMENTS_IMAGE_H
#define PF_IMGUI_ELEMENTS_IMAGE_H

#include <functional>
#include <pf_imgui/_export.h>
#include <pf_imgui/interface/Clickable.h>
#include <pf_imgui/interface/Customizable.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Resizable.h>
#include <string>
#include <utility>

namespace pf::ui::ig {

/**
* @brief Image rendered from a texture. It can serve as a button.
*
* Image rendering should be handled by the use when descending from ImGuiInterface.
*/
class PF_IMGUI_EXPORT Image
    : public ItemElement,
      public Resizable,
      public Clickable,
      public ColorCustomizable<style::ColorOf::Button, style::ColorOf::ButtonHovered, style::ColorOf::ButtonActive,
                               style::ColorOf::NavHighlight, style::ColorOf::Border, style::ColorOf::BorderShadow> {
 public:
  /**
  * Provider of UV mapping for textures. First is left upper corner, right is right lower.
  */
  using UvMappingProvider = std::function<std::pair<ImVec2, ImVec2>()>;

  struct Config {
    using Parent = Image;
    std::string_view name;
    ImTextureID textureId;
    Size size = Size::Auto();
    IsButton isButton = IsButton::No;
    UvMappingProvider uvTextureMappingProvider = [] { return std::pair(ImVec2{0, 0}, ImVec2{1, 1}); };
  };

  explicit Image(Config &&config);
  /**
  * Construct Image.
  * @param elementName ID of the element
  * @param imTextureId ID of a texture
  * @param size size of the image on screen
  * @param isBtn allow for usage as a button
  * @param uvTextureMappingProvider provider of UV coordinates
  */
  Image(
      const std::string &elementName, ImTextureID imTextureId, Size size = Size::Auto(), IsButton isBtn = IsButton::No,
      UvMappingProvider uvTextureMappingProvider = [] {
        return std::pair(ImVec2{0, 0}, ImVec2{1, 1});
      });

  /**
  * Check if the image acts as a button.
  * @return true of image is a button, false otherwise
  */
  [[nodiscard]] bool isButton() const;
  ///**
  // * Check if a tooltip activated by pressing shift while hovering an image is active.
  // * @return
  // */
  // [[nodiscard]] bool isPixelInspectionTooltipEnabled() const;
  ///**
  // * Enable/disable a tooltip activated by pressing shift while hovering an image.
  // * @param enabled
  // */
  // void setPixelInspectionTooltipEnabled(bool enabled);

  /**
  * Change texture ID.
  * @param imTextureId new id
  */
  void setTextureId(ImTextureID imTextureId);

 protected:
  void renderImpl() override;

 private:
  bool isButton_;
  ImTextureID textureId;
  UvMappingProvider uvMappingProvider;
  // bool pixelInspectionTooltipEnabled = false;
};

}  // namespace pf::ui::ig

#endif  // PF_IMGUI_ELEMENTS_IMAGE_H
