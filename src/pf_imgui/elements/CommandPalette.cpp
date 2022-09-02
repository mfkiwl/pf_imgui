//
// Created by petr.flajsingr on 2/8/2022.
//

#include "CommandPalette.h"

namespace pf::ui::ig {

CommandPalette::CommandPalette(CommandPalette::Config &&config)
    : ElementWithID(std::string{config.name.value}), context(ImCmd::CreateContext()) {}

CommandPalette::CommandPalette(const std::string &elementName)
    : ElementWithID(elementName), context(ImCmd::CreateContext()) {}

CommandPalette::~CommandPalette() { ImCmd::DestroyContext(context); }

void CommandPalette::removeCommand(const std::string &commandName) {
  ImCmd::SetCurrentContext(context);
  ImCmd::RemoveCommand(commandName.c_str());
}

void CommandPalette::moveFocusToSearchBox() {
  ImCmd::SetCurrentContext(context);
  ImCmd::SetNextCommandPaletteSearchBoxFocused();
}

void CommandPalette::setSearchText(const std::string &text) {
  ImCmd::SetCurrentContext(context);
  ImCmd::SetNextCommandPaletteSearch(text);
}
void CommandPalette::renderImpl() {
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();
  ImCmd::CommandPalette(getName().c_str());
  if (ImCmd::IsAnyItemSelected()) { observableInteract.notify(); }
}

}  // namespace pf::ui::ig
