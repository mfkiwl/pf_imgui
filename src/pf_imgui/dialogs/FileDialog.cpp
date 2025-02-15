//
// Created by petr on 11/7/20.
//

#include "FileDialog.h"
#include <ImGuiFileDialog.h>
#include <fmt/format.h>
#include <pf_imgui/managers/DialogManager.h>
#include <utility>

namespace pf::ui::ig {
// TODO: fix multi dir selection - when using GetSelection() it returns invalid path

FileDialog::FileDialog(FileType dialogFileType, std::string_view elementName, std::string_view labelText,
                       const std::vector<FileExtensionSettings> &extSettings,
                       std::function<void(std::vector<std::filesystem::path>)> onSelect, std::function<void()> onCancel,
                       Size initialSize, std::filesystem::path startPath, std::string startName, Modal modality,
                       uint32_t maxSelected)
    : Renderable(elementName), label(std::string{labelText}), size(initialSize), openPath(std::move(startPath)),
      defaultName(std::move(startName)), modal(modality), fileType(dialogFileType), maxSelectCount(maxSelected),
      onFilesSelected(std::move(onSelect)), onSelectCanceled(std::move(onCancel)),
      fileDialogInstance(new IGFD::FileDialog{}) {
  if (fileType == FileType::File) { prepareExtInfos(extSettings); }
}

FileDialog::~FileDialog() = default;

bool FileDialog::isDone() const { return done; }

std::string FileDialog::serializeBookmark() { return fileDialogInstance->SerializeBookmarks(); }

void FileDialog::deserializeBookmark(const std::string &bookmarkStr) {
  fileDialogInstance->DeserializeBookmarks(bookmarkStr);
}

void FileDialog::prepareExtInfos(const std::vector<FileExtensionSettings> &extSettings) {
  for (const auto &[extNames, desc, extColor] : extSettings) {
    if (!desc.empty()) {
      filters += desc;
      filters += '{';
      for (const auto &extName : extNames) {
        const auto extString = fmt::format(".{}", extName.string());
        filters += fmt::format("{},", extString);
        if (extColor.has_value()) { extColors.emplace_back(extString, *extColor); }
      }
      filters = filters.substr(0, filters.size() - 1);
      filters += "},";
    } else {
      for (const auto &extName : extNames) {
        filters += fmt::format("{},", extName.string());
        if (extColor.has_value()) { extColors.emplace_back(extName.string(), *extColor); }
      }
      filters = filters.substr(0, filters.size() - 1);
    }
  }
  if (!filters.empty()) { filters = filters.substr(0, filters.size() - 1); }
}

void FileDialog::renderImpl() {
  if (done) { return; }
  [[maybe_unused]] auto colorScoped = color.applyScoped();
  [[maybe_unused]] auto styleScoped = style.applyScoped();
  [[maybe_unused]] auto fontScoped = font.applyScopedIfNotDefault();
  switch (modal) {
    case Modal::Yes:
      fileDialogInstance->OpenModal(getName(), label->get(), fileType == FileType::File ? filters.c_str() : nullptr,
                                    openPath.string(), defaultName, static_cast<int>(maxSelectCount));
      break;
    case Modal::No:
      fileDialogInstance->OpenDialog(getName(), label->get(), fileType == FileType::File ? filters.c_str() : nullptr,
                                     openPath.string(), defaultName, static_cast<int>(maxSelectCount));
      break;
  }

  std::ranges::for_each(extColors, [this](const auto &extColor) {
    const auto &[ext, col] = extColor;
    fileDialogInstance->SetFileStyle(IGFD_FileStyleByExtention, ext.c_str(), static_cast<ImVec4>(col));
  });

  if (fileDialogInstance->Display(getName(), ImGuiWindowFlags_NoCollapse, static_cast<ImVec2>(*size))) {
    RAII end{[&] { fileDialogInstance->Close(); }};
    if (fileDialogInstance->IsOk()) {
      const auto filePathName = fileDialogInstance->GetFilePathName();
      const auto selection = fileDialogInstance->GetSelection();

      if (!selection.empty() && fileType != FileType::Directory) {
        auto selectionVec = std::vector<std::filesystem::path>();
        selectionVec.reserve(selection.size());
        for (auto &[_, path] : selection) { selectionVec.emplace_back(path); }
        onFilesSelected(selectionVec);
      } else {
        onFilesSelected({filePathName});
      }
    } else {
      onSelectCanceled();
    }
    done = true;
  }
}
}  // namespace pf::ui::ig
