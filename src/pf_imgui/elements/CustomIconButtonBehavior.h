//
// Created by xflajs00 on 17.04.2022.
//

#ifndef PF_IMGUI_ELEMENTS_CUSTOMICONBUTTONBEHAVIOR_H
#define PF_IMGUI_ELEMENTS_CUSTOMICONBUTTONBEHAVIOR_H

#include <pf_imgui/Color.h>
#include <pf_imgui/Size.h>
#include <pf_imgui/interface/ItemElement.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/reactive/Event.h>
#include <pf_imgui/style/ColorPalette.h>

namespace pf::ui::ig {

class CustomIconButtonBehavior : public ItemElement {
 protected:
  enum class State { None, Hovered, MouseDown, Clicked };

 public:
  explicit CustomIconButtonBehavior(const std::string &elementName);
  [[nodiscard]] Color getIconColor() const;
  void setIconColor(Color newColor);

  ColorPalette<ColorOf::Button, ColorOf::ButtonActive, ColorOf::ButtonHovered> color;

 protected:
  virtual void renderIcon(ImDrawList *drawList, ImVec2 cursorPos) = 0;
  virtual void update(State state) = 0;

  [[nodiscard]] Size getSize() const;
  void setSize(Size newSize);

  void renderImpl() override;

  [[nodiscard]] Color getBackgroundColor() const;

  bool keepHighlighted = false;

 private:
  Color iconColor = Color::White;
  Color backgroundColor = Color::White;
  Size size{22, 22};
};

class CustomIconButton : public CustomIconButtonBehavior {
  using ClickEvent = ClassEvent<CustomIconButton>;

 public:
  explicit CustomIconButton(const std::string &elementName);

  ClickEvent clickEvent;

 protected:
  void update(State state) override;
  void notifyClickEvent();
};

class CustomIconToggle : public CustomIconButtonBehavior, public ValueObservable<bool>, public Savable {
 public:
  explicit CustomIconToggle(const std::string &elementName, bool initValue = false,
                            Persistent persistent = Persistent::No);

  [[nodiscard]] toml::table toToml() const override;

  void setFromToml(const toml::table &src) override;

 protected:
  void update(State state) override;
};

}  // namespace pf::ui::ig

#endif  //PF_IMGUI_ELEMENTS_CUSTOMICONBUTTONBEHAVIOR_H
