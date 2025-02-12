/**
 * @file Button.h
 * @brief Button element.
 * @author Petr Flajšingr
 * @date 31.10.20
 */

#ifndef PF_IMGUI_ELEMENTS_BUTTON_H
#define PF_IMGUI_ELEMENTS_BUTTON_H

#include <pf_common/Explicit.h>
#include <pf_imgui/_export.h>
#include <pf_imgui/common/Font.h>
#include <pf_imgui/common/Label.h>
#include <pf_imgui/common/Size.h>
#include <pf_imgui/common/Texture.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/style/ColorPalette.h>
#include <pf_imgui/style/StyleOptions.h>

namespace pf::ui::ig {
/** Button types in this file: */
class Button;
class SmallButton;
class ArrowButton;
class InvisibleButton;

/**
 * @brief Base class for buttons.
 *
 * A button with click event and an option to control repeatable click trigger.
 */
class PF_IMGUI_EXPORT ButtonBase : public ItemElement {
 public:
  /**
   * Construct ButtonBase.
   * @param elementName ID of the button
   * @param isRepeatable enable/disable button can repeat its on click event when a user holds it
   */
  explicit ButtonBase(std::string_view elementName, Repeatable isRepeatable = Repeatable::No);

  bool repeatable;

  Event<> clickEvent;

 protected:
  [[nodiscard]] RAII setButtonRepeat();
};

/**
 * @brief A button which is not rendered, can be used as a part of a new component for click&hover etc. detection.
 */
class PF_IMGUI_EXPORT InvisibleButton : public ButtonBase {
 public:
  /**
   * @brief Struct for construction of InvisibleButton.
   */
  struct Config {
    using Parent = InvisibleButton;
    Explicit<std::string_view> name;             /*!< Unique name of the element */
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
  explicit InvisibleButton(std::string_view elementName, Size s = Size::Auto(),
                           MouseButton clickButton = MouseButton::Left, Repeatable isRepeatable = Repeatable::No);

  Property<Size> size;

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
class PF_IMGUI_EXPORT Button : public ButtonBase {
 public:
  /**
   * @brief Struct for construction of Button.
   */
  struct Config {
    using Parent = Button;
    Explicit<std::string_view> name;  /*!< Unique name of the element */
    Explicit<std::string_view> label; /*!< Text rendered on the button */
    Size size = Size::Auto();         /*!< Size of the button */
    bool repeatable = false;          /*!< Enable repeated listener callback on mouse down */
  };
  /**
   * Construct Button
   * @param config construction args @see Button::Config
   */
  explicit Button(Config &&config);
  /**
   * Construct Button
   * @param elementName unique name of the element
   * @param labelText text rendered on the button
   * @param s size of the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  Button(std::string_view elementName, std::string_view labelText, Size s = Size::Auto(),
         Repeatable isRepeatable = Repeatable::No);

  ColorPalette<ColorOf::Text, ColorOf::TextDisabled, ColorOf::Button, ColorOf::ButtonHovered, ColorOf::ButtonActive,
               ColorOf::NavHighlight, ColorOf::Border, ColorOf::BorderShadow>
      color;
  StyleOptions<StyleOf::FramePadding, StyleOf::FrameRounding, StyleOf::FrameBorderSize, StyleOf::ButtonTextAlign> style;
  Font font = Font::Default();
  Property<Label> label;

  Property<Size> size;

 protected:
  void renderImpl() override;
};

/**
 * @brief A small button.
 *
 * A button with a text label.
 */
class PF_IMGUI_EXPORT SmallButton : public ButtonBase {
 public:
  /**
   * @brief Struct for construction of SmallButton.
   */
  struct Config {
    using Parent = SmallButton;
    Explicit<std::string_view> name;  /*!< Unique name of the element */
    Explicit<std::string_view> label; /*!< Text rendered on the button */
    bool repeatable = false;          /*!< Enable repeated listener callback on mouse down */
  };
  explicit SmallButton(Config &&config);
  /**
   * Construct SmallButton
   * @param elementName unique name of the element
   * @param labelText text rendered on the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  SmallButton(std::string_view elementName, std::string_view labelText, Repeatable isRepeatable = Repeatable::No);

  ColorPalette<ColorOf::Text, ColorOf::TextDisabled, ColorOf::Button, ColorOf::ButtonHovered, ColorOf::ButtonActive,
               ColorOf::NavHighlight, ColorOf::Border, ColorOf::BorderShadow>
      color;
  StyleOptions<StyleOf::FramePadding, StyleOf::FrameRounding, StyleOf::FrameBorderSize, StyleOf::ButtonTextAlign> style;
  Font font = Font::Default();
  Property<Label> label;

 protected:
  void renderImpl() override;
};
/**
 * @brief A small button with an arrow.
 *
 * A button with an arrow rendered on top of it.
 */
class PF_IMGUI_EXPORT ArrowButton : public ButtonBase {
 public:
  enum class Dir { Up = ImGuiDir_Up, Left = ImGuiDir_Left, Right = ImGuiDir_Right, Down = ImGuiDir_Down };
  struct Config {
    using Parent = ArrowButton;
    Explicit<std::string_view> name; /*!< Unique name of the element */
    Explicit<Dir> direction;         /*!< Direction of the arrow rendered on the button */
    bool repeatable = false;         /*!< Enable repeated listener callback on mouse down */
  };
  /**
   * Construct ArrowButton
   * @param config construction args @see ArrowButton::Config
   */
  explicit ArrowButton(Config &&config);
  /**
   * Construct ArrowButton
   * @param elementName unique name of the element
   * @param direction direction of the arrow rendered on the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  ArrowButton(std::string_view elementName, Dir direction, Repeatable isRepeatable = Repeatable::No);

  ColorPalette<ColorOf::Text, ColorOf::TextDisabled, ColorOf::Button, ColorOf::ButtonHovered, ColorOf::ButtonActive,
               ColorOf::NavHighlight, ColorOf::Border, ColorOf::BorderShadow>
      color;
  StyleOptions<StyleOf::FramePadding, StyleOf::FrameRounding, StyleOf::FrameBorderSize, StyleOf::ButtonTextAlign> style;

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
class PF_IMGUI_EXPORT ImageButton : public ButtonBase {
 public:
  struct Config {
    using Parent = ImageButton;
    Explicit<std::string_view> name;            /*!< Unique name of the element */
    Explicit<std::shared_ptr<Texture>> texture; /*!< Texture to render */
    Size size = Size::Auto();                   /*!< Size of the element */
    bool repeatable = false;                    /*!< Enable repeated listener callback on mouse down */
  };
  /**
   * Construct ImageButton
   * @param config construction args @see ImageButton::Config
   */
  explicit ImageButton(Config &&config);
  /**
   * Construct ArrowButton
   * @param elementName unique name of the element
   * @param tex texture to render
   * @param s size of the button
   * @param isRepeatable if No, then only click notifies, otherwise mouse down repeatedly calls listeners
   */
  ImageButton(std::string_view elementName, std::shared_ptr<Texture> tex, Size s = Size::Auto(),
              Repeatable isRepeatable = Repeatable::No);

  /**
   * Change texture.
   * @param tex new texture
   */
  void setTexture(std::shared_ptr<Texture> tex);
  /**
   * Set UVs according to which the image will get sampled.
   */
  void setUVs(ImVec2 leftTop, ImVec2 rightBottom);

  ColorPalette<ColorOf::Button, ColorOf::ButtonHovered, ColorOf::ButtonActive, ColorOf::NavHighlight, ColorOf::Border,
               ColorOf::BorderShadow>
      color;
  StyleOptions<StyleOf::FramePadding, StyleOf::FrameRounding, StyleOf::FrameBorderSize, StyleOf::ButtonTextAlign> style;

  Property<Size> size;

 protected:
  void renderImpl() override;

 private:
  std::shared_ptr<Texture> texture;
  ImVec2 uvLeftTop{0.f, 0.f};
  ImVec2 uvRightBottom{1.f, 1.f};
};

}  // namespace pf::ui::ig
#endif  // PF_IMGUI_ELEMENTS_BUTTON_H
