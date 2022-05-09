/**
 * @file Button.h
 * @brief Button element.
 * @author Petr Flajšingr
 * @date 31.10.20
 */

#ifndef PF_IMGUI_ELEMENTS_BUTTON_H
#define PF_IMGUI_ELEMENTS_BUTTON_H

#include <functional>
#include <pf_imgui/_export.h>
#include <pf_imgui/interface/Clickable.h>
#include <pf_imgui/interface/Customizable.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Labellable.h>
#include <pf_imgui/interface/Resizable.h>
#include <string>

namespace pf::ui::ig {
/** Button types in this file: */
class Button;
class SmallButton;
class ArrowButton;
class InvisibleButton;

/**
 * @brief A typical button element.
 *
 * A button with a text label and an option to change its render type by using enum ButtonType.
 *
 * @see ButtonType
 */
class PF_IMGUI_EXPORT ButtonBase : public ItemElement, public Clickable {
 public:
  /**
   * Construct ButtonBase.
   * @param name ID of the button
   * @param isRepeatable enable/disable button can repeat its on click event when a user holds it
   */
  explicit ButtonBase(const std::string &name, Repeatable isRepeatable = Repeatable::No);

  /**
   * Check whether the button is repeatable or not
   * @return
   */
  [[nodiscard]] bool isRepeatable() const;
  /**
   * Set repeatable. If true then the button can be held by the user to trigger click events.
   * @param repeatable new state
   */
  void setRepeatable(bool newRepeatable);

 protected:
  [[nodiscard]] RAII setButtonRepeat();

 private:
  bool repeatable;
};

/**
 * @brief A button which is not rendered, can be used as a part of a new component for click&hover etc. detection.
 */
class PF_IMGUI_EXPORT InvisibleButton : public ButtonBase, public Resizable {
 public:
  /**
   * @brief Struct for construction of InvisibleButton.
   */
  struct Config {
    using Parent = InvisibleButton;
    std::string_view name;                       /*!< Unique name of the element */
    Size size = Size::Auto();                    /*!< Size of the element */
    MouseButton clickButton = MouseButton::Left; /*!< Mouse button to which the button reacts */
    bool repeatable = false;                     /*!< Enable repeated listener callback on mouse down */
  };
  /**
   * Construct InvisibleButton
   * @param config construction args @see InvisibleButton::Config
   */
  explicit InvisibleButton(Config &&config);
  /**
   * Create InvisibleButton.
   * @param elementName unique name
   * @param s size on screen
   * @param clickButton mouse button to register as a click
   * @param isRepeatable enable/disable button can repeat its on click event when a user holds it
   */
  explicit InvisibleButton(const std::string &elementName, const Size &s = Size::Auto(),
                           MouseButton clickButton = MouseButton::Left, Repeatable isRepeatable = Repeatable::No);

 protected:
  void renderImpl() override;

 private:
  MouseButton clickBtn;
};

/**
 * @brief A typical button element.
 *
 * A button with a text label.
 */
class PF_IMGUI_EXPORT Button
    : public ButtonBase,
      public Labellable,
      public Resizable,
      public ColorCustomizable<style::ColorOf::Text, style::ColorOf::TextDisabled, style::ColorOf::Button,
                               style::ColorOf::ButtonHovered, style::ColorOf::ButtonActive,
                               style::ColorOf::NavHighlight, style::ColorOf::Border, style::ColorOf::BorderShadow>,
      public StyleCustomizable<style::Style::FramePadding, style::Style::FrameRounding, style::Style::FrameBorderSize,
                               style::Style::ButtonTextAlign> {
 public:
  /**
   * @brief Struct for construction of Button.
   */
  struct Config {
    using Parent = Button;
    std::string_view name;    /*!< Unique name of the element */
    std::string_view label;   /*!< Text rendered on the button */
    Size size = Size::Auto(); /*!< Size of the button */
    bool repeatable = false;  /*!< Enable repeated listener callback on mouse down */
  };
  /**
   * Construct Button
   * @param config construction args @see Button::Config
   */
  explicit Button(Config &&config);
  /**
   * Construct Button
   * @param name unique name of the element
   * @param label text rendered on the button
   * @param s size of the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  Button(const std::string &name, const std::string &label, const Size &s = Size::Auto(),
         Repeatable isRepeatable = Repeatable::No);

 protected:
  void renderImpl() override;
};

/**
 * @brief A small button.
 *
 * A button with a text label.
 */
class PF_IMGUI_EXPORT SmallButton
    : public ButtonBase,
      public Labellable,
      public ColorCustomizable<style::ColorOf::Text, style::ColorOf::TextDisabled, style::ColorOf::Button,
                               style::ColorOf::ButtonHovered, style::ColorOf::ButtonActive,
                               style::ColorOf::NavHighlight, style::ColorOf::Border, style::ColorOf::BorderShadow>,
      public StyleCustomizable<style::Style::FramePadding, style::Style::FrameRounding, style::Style::FrameBorderSize,
                               style::Style::ButtonTextAlign> {
 public:
  /**
   * @brief Struct for construction of SmallButton.
   */
  struct Config {
    using Parent = SmallButton;
    std::string_view name;   /*!< Unique name of the element */
    std::string_view label;  /*!< Text rendered on the button */
    bool repeatable = false; /*!< Enable repeated listener callback on mouse down */
  };
  explicit SmallButton(Config &&config);
  /**
   * Construct SmallButton
   * @param name unique name of the element
   * @param label text rendered on the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  SmallButton(const std::string &name, const std::string &label, Repeatable isRepeatable = Repeatable::No);

 protected:
  void renderImpl() override;
};
/**
 * @brief A small button with an arrow.
 *
 * A button with an arrow rendered on top of it.
 */
class PF_IMGUI_EXPORT ArrowButton
    : public ButtonBase,
      public ColorCustomizable<style::ColorOf::Text, style::ColorOf::TextDisabled, style::ColorOf::Button,
                               style::ColorOf::ButtonHovered, style::ColorOf::ButtonActive,
                               style::ColorOf::NavHighlight, style::ColorOf::Border, style::ColorOf::BorderShadow>,
      public StyleCustomizable<style::Style::FramePadding, style::Style::FrameRounding, style::Style::FrameBorderSize,
                               style::Style::ButtonTextAlign> {
 public:
  enum class Dir { Up = ImGuiDir_Up, Left = ImGuiDir_Left, Right = ImGuiDir_Right, Down = ImGuiDir_Down };
  struct Config {
    using Parent = ArrowButton;
    std::string_view name;   /*!< Unique name of the element */
    Dir direction;           /*!< Direction of the arrow rendered on the button */
    bool repeatable = false; /*!< Enable repeated listener callback on mouse down */
  };
  /**
   * Construct ArrowButton
   * @param config construction args @see ArrowButton::Config
   */
  explicit ArrowButton(Config &&config);
  /**
   * Construct ArrowButton
   * @param name unique name of the element
   * @param direction direction of the arrow rendered on the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  ArrowButton(const std::string &name, Dir direction, Repeatable isRepeatable = Repeatable::No);

 protected:
  void renderImpl() override;

 private:
  Dir dir;
};
/**
 * @brief A button with an image
 *
 * A button with an image rendered on top of it.
 */
class PF_IMGUI_EXPORT ImageButton
    : public ButtonBase,
      public Resizable,
      public ColorCustomizable<style::ColorOf::Button, style::ColorOf::ButtonHovered, style::ColorOf::ButtonActive,
                               style::ColorOf::NavHighlight, style::ColorOf::Border, style::ColorOf::BorderShadow>,
      public StyleCustomizable<style::Style::FramePadding, style::Style::FrameRounding, style::Style::FrameBorderSize,
                               style::Style::ButtonTextAlign> {
 public:
  /**
   * Provider of UV mapping for textures. First is left upper corner, right is right lower.
   */
  using UvMappingProvider = std::function<std::pair<ImVec2, ImVec2>()>;
  struct Config {
    using Parent = ImageButton;
    std::string_view name;    /*!< Unique name of the element */
    ImTextureID textureId;    /*!< Id of the texture to render */
    Size size = Size::Auto(); /*!< Size of the element */
    bool repeatable = false;  /*!< Enable repeated listener callback on mouse down */
    UvMappingProvider uvTextureMappingProvider = [] {
      return std::pair(ImVec2{0, 0}, ImVec2{1, 1});
    }; /*!< Provider of UV coordinates */
  };
  /**
   * Construct ImageButton
   * @param config construction args @see ImageButton::Config
   */
  explicit ImageButton(Config &&config);
  /**
   * Construct ArrowButton
   * @param name unique name of the element
   * @param textureId texture to render
   * @param s size of the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   * @param uvTextureMappingProvider uv mapping provider
   */
  ImageButton(
      const std::string &name, ImTextureID texId, Size s = Size::Auto(), Repeatable isRepeatable = Repeatable::No,
      UvMappingProvider uvTextureMappingProvider = [] {
        return std::pair(ImVec2{0, 0}, ImVec2{1, 1});
      });

  /**
   * Change texture ID.
   * @param imTextureId new id
   */
  void setTextureId(ImTextureID imTextureId);

 protected:
  void renderImpl() override;

 private:
  ImTextureID textureId;
  UvMappingProvider uvMappingProvider;
};

}  // namespace pf::ui::ig
#endif  // PF_IMGUI_ELEMENTS_BUTTON_H
