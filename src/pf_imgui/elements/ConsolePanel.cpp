//
// Created by Petr on 6/23/2022.
//

#include "ConsolePanel.h"
#include <pf_imgui/elements/details/TextUtils.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

namespace pf::ui::ig {

ConsolePanel::ConsolePanel(ConsolePanel::Config &&config)
    : ConsolePanel(config.name.value, config.size, config.persistent ? Persistent::Yes : Persistent::No) {}

ConsolePanel::ConsolePanel(std::string_view elementName, Size initialSize, Persistent persistent)
    : ElementWithID(elementName), Savable(persistent), size(initialSize), wrapTextToggle("wrapText"),
      scrollToEndToggle("scrollToEnd"), copyToClipboardButton("copyToClipboard"), clearButton("clear") {

  clearButton.clickEvent.addListener([this] { records.clear(); });
  copyToClipboardButton.clickEvent.addListener([this] { ImGui::SetClipboardText(getOutput().c_str()); });
  clearButton.setTooltip("Clear output");
  copyToClipboardButton.setTooltip("Copy output to clipboard");
  wrapTextToggle.setTooltip("Wrap text");
  scrollToEndToggle.setTooltip("Autoscroll down");
}

std::string ConsolePanel::getOutput() const {
  return records | ranges::views::transform(&std::pair<RecordType, Record>::second)
      | ranges::views::transform(&Record::message) | ranges::views::join('\n') | ranges::to<std::string>();
}

void ConsolePanel::renderImpl() {
  [[maybe_unused]] auto colorScoped = color.applyScoped();
  [[maybe_unused]] auto styleScoped = style.applyScoped();
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();

  ImGui::BeginHorizontal("layout", static_cast<ImVec2>(*size), 0);
  {
    ImGui::BeginChild("controls_area", ImVec2{25, 0});
    {
      ImGui::BeginVertical("controls", ImVec2{0, 0}, 0);
      {
        wrapTextToggle.render();
        ImGui::Spring(0);
        scrollToEndToggle.render();
        ImGui::Spring(0);
        copyToClipboardButton.render();
        ImGui::Spring(0);
        clearButton.render();
      }
      ImGui::EndVertical();
    }
    ImGui::EndChild();
    ImGui::Spring(0.f);
    ImGui::BeginVertical("output", ImVec2{0, 0}, 0);
    {
      {
        RAII end{ImGui::EndChild};
        const auto wrapEnabled = wrapTextToggle.getValue();
        if (ImGui::BeginChild("out_w", ImVec2{0, -25}, true,
                              wrapEnabled ? ImGuiWindowFlags{} : ImGuiWindowFlags_HorizontalScrollbar)) {
          std::ranges::for_each(records, [&](const auto &record) {
            std::string prefix{};
            if (record.first == RecordType::Input) { prefix = "# "; }
            if (record.second.color.has_value()) { ImGui::PushStyleColor(ImGuiCol_Text, *record.second.color); }
            if (record.second.backgroundColor.has_value()) {
              drawTextBackground(record.second.message.c_str(), *record.second.backgroundColor, wrapEnabled, false);
            }
            if (wrapEnabled) {
              ImGui::TextWrapped((prefix + record.second.message).c_str());
            } else {
              ImGui::Text((prefix + record.second.message).c_str());
            }
            if (record.second.color.has_value()) { ImGui::PopStyleColor(); }
          });
          if (scrollToEndToggle.getValue()) { ImGui::SetScrollHereY(1.0f); }
        }
      }

      ImGui::BeginHorizontal("user_in", ImVec2{0, 0}, 0);
      {
        ImGui::SetNextItemWidth(-60.f);
        if (ImGui::InputText("##input", inputBuffer, BUFFER_SIZE,
                             ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory
                                 | ImGuiInputTextFlags_CallbackCompletion,
                             CompletionCallback, this)) {
          handleSubmit();
          ImGui::SetKeyboardFocusHere(-1);
        }

        if (ImGui::Button("Submit")) { handleSubmit(); }
      }
      ImGui::EndHorizontal();
    }
    ImGui::EndVertical();
  }
  ImGui::EndHorizontal();
}

void ConsolePanel::handleSubmit() {
  if (inputBuffer[0] != '\0') {
    records.emplace_back(RecordType::Input, Record{inputBuffer, inputTextColor, inputTextBackgroundColor});
    if (auto response = inputHandler(std::string_view{inputBuffer}); response.has_value()) {
      records.emplace_back(RecordType::Output, std::move(*response));
    }
    history.emplace_back(inputBuffer);
    inputBuffer[0] = '\0';
    historyIndex = std::nullopt;
  }
}

int ConsolePanel::CompletionCallback(ImGuiInputTextCallbackData *data) {
  auto &self = *reinterpret_cast<ConsolePanel *>(data->UserData);
  auto historyChanged = false;
  switch (data->EventKey) {
    case ImGuiKey_UpArrow:
      if (!self.history.empty()) {
        if (!self.historyIndex.has_value()) {
          self.historyIndex = self.history.size() - 1;
          historyChanged = true;
        } else if (*self.historyIndex > 0) {
          --*self.historyIndex;
          historyChanged = true;
        }
      }
      break;
    case ImGuiKey_DownArrow:
      if (self.historyIndex.has_value() && *self.historyIndex < self.history.size() - 1) {
        ++*self.historyIndex;
        historyChanged = true;
      }
      break;
    case ImGuiKey_Tab:
      if (const auto iter = std::ranges::find_if(self.completionStrings,
                                                 [data](const auto &str) { return str.starts_with(data->Buf); });
          iter != self.completionStrings.end()) {
        strcpy(data->Buf, iter->c_str());
        data->BufDirty = true;
        data->BufTextLen = static_cast<int>(iter->size());
      }
      break;
  }
  if (historyChanged) {
    strcpy(data->Buf, self.history[*self.historyIndex].c_str());
    data->BufDirty = true;
    data->BufTextLen = static_cast<int>(self.history[*self.historyIndex].size());
    data->CursorPos = data->BufTextLen;
  }
  return 0;
}

toml::table ConsolePanel::toToml() const {
  return toml::table{{"textwrap", wrapTextToggle.getValue()}, {"autoscroll", scrollToEndToggle.getValue()}};
}

void ConsolePanel::setFromToml(const toml::table &src) {
  if (const auto iter = src.find("textwrap"); iter != src.end()) {
    if (const auto ptr = iter->second.as_boolean(); ptr != nullptr) { wrapTextToggle.setValue(ptr->get()); }
  }
  if (const auto iter = src.find("autoscroll"); iter != src.end()) {
    if (const auto ptr = iter->second.as_boolean(); ptr != nullptr) { scrollToEndToggle.setValue(ptr->get()); }
  }
}

void ConsolePanel::setInputTextColor(Color newColor) { inputTextColor = newColor; }
void ConsolePanel::setInputTextBackgroundColor(Color newColor) { inputTextBackgroundColor = newColor; }

}  // namespace pf::ui::ig