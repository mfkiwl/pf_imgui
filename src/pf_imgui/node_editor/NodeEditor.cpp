//
// Created by xflajs00 on 27.03.2022.
//

#include "NodeEditor.h"
#include "Link.h"
#include "Node.h"
#include "Pin.h"
#include "pf_common/RAII.h"
#include "pf_imgui/unique_id.h"
#include <range/v3/view/cache1.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

namespace pf::ui::ig {

NodeEditor::NodeEditor(const std::string &name, Size size)
    : Element(name), Resizable(size), context(ax::NodeEditor::CreateEditor()) {}

NodeEditor::NodeEditor(NodeEditor::Config &&config)
    : Element(std::move(config.name)), Resizable(config.size), context(ax::NodeEditor::CreateEditor()) {}

NodeEditor::~NodeEditor() { ax::NodeEditor::DestroyEditor(context); }

void NodeEditor::renderImpl() {
  {
    ax::NodeEditor::SetCurrentEditor(context);
    auto unsetEditor = RAII{[] { ax::NodeEditor::SetCurrentEditor(nullptr); }};
    ax::NodeEditor::Begin(getName().c_str(), getSize().asImVec());
    auto end = RAII{ax::NodeEditor::End};
    {
      std::ranges::for_each(nodes, [](auto &node) { node->render(); });
      std::ranges::for_each(links, [](auto &link) { link->render(); });

      handleCreation();
      handleDeletion();

      ax::NodeEditor::NodeId contextNodeId;
      ax::NodeEditor::PinId contextPinId;
      ax::NodeEditor::LinkId contextLinkId;
      if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId)) {
        if (auto nodeOpt = findNodeById(contextNodeId); nodeOpt.has_value()) {
          if (nodeOpt.value()->hasPopupMenu()) {
            popupPtrs.node = nodeOpt.value();
            popupPtrs.node->popupMenu->open();
          }
        }
      } else if (ax::NodeEditor::ShowPinContextMenu(&contextPinId)) {
        if (auto pinOpt = findPinById(contextPinId); pinOpt.has_value()) {
          if (pinOpt.value()->hasPopupMenu()) {
            popupPtrs.pin = pinOpt.value();
            popupPtrs.pin->popupMenu->open();
          }
        }
      } else if (ax::NodeEditor::ShowLinkContextMenu(&contextLinkId)) {
        if (auto linkOpt = findLinkById(contextLinkId); linkOpt.has_value()) {
          if (linkOpt.value()->hasPopupMenu()) {
            popupPtrs.link = linkOpt.value();
            popupPtrs.link->popupMenu->open();
          }
        }
      }
    }
  }

  if (popupPtrs.node != nullptr) {
    if (popupPtrs.node->popupMenu == nullptr || !popupPtrs.node->popupMenu->isOpen()) {
      popupPtrs.node = nullptr;
    } else {
      popupPtrs.node->popupMenu->render();
    }
  }
  if (popupPtrs.pin != nullptr) {
    if (popupPtrs.pin->popupMenu == nullptr || !popupPtrs.pin->popupMenu->isOpen()) {
      popupPtrs.pin = nullptr;
    } else {
      popupPtrs.pin->popupMenu->render();
    }
  }
  if (popupPtrs.link != nullptr) {
    if (popupPtrs.link->popupMenu == nullptr || !popupPtrs.link->popupMenu->isOpen()) {
      popupPtrs.link = nullptr;
    } else {
      popupPtrs.link->popupMenu->render();
    }
  }

  cleanupLinks();
}

std::optional<Node *> NodeEditor::findNodeById(ax::NodeEditor::NodeId id) {
  const auto getNodeId = [](auto &node) { return node->id; };
  if (auto iter = std::ranges::find(nodes, id, getNodeId); iter != nodes.end()) { return iter->get(); }
  return std::nullopt;
}

std::optional<Pin *> NodeEditor::findPinById(ax::NodeEditor::PinId id) {
  const auto getPinId = [](auto &pin) { return pin->id; };
  auto pins = nodes
      | ranges::views::transform([](auto &node) { return ranges::views::concat(node->inputPins, node->outputPins); })
      | ranges::views::cache1 | ranges::views::join;
  if (auto iter = std::ranges::find(pins, id, getPinId); iter != pins.end()) { return iter->get(); }
  return std::nullopt;
}

std::optional<Link *> NodeEditor::findLinkById(ax::NodeEditor::LinkId id) {
  const auto getLinkId = [](auto &link) { return link->id; };
  if (auto iter = std::ranges::find(links, id, getLinkId); iter != links.end()) { return iter->get(); }
  return std::nullopt;
}

void NodeEditor::handleCreation() {
  auto endCreate = RAII{ax::NodeEditor::EndCreate};
  if (ax::NodeEditor::BeginCreate()) {
    handleLinkCreation();
    handleNodeCreation();
  }
}

void NodeEditor::handleLinkCreation() {
  ax::NodeEditor::PinId inPinId;
  ax::NodeEditor::PinId outPinId;
  if (ax::NodeEditor::QueryNewLink(&inPinId, &outPinId)) {
    if (inPinId && outPinId) {
      auto inPinOpt = findPinById(inPinId);
      auto outPinOpt = findPinById(outPinId);

      if (inPinOpt.has_value() && outPinOpt.has_value()) {
        auto inPin = inPinOpt.value();
        const auto originPin = inPin;
        auto outPin = outPinOpt.value();
        if (&inPin->getNode() == &outPin->getNode()) { return; }
        if (inPin->type == Pin::Type::Output && outPin->type == Pin::Type::Input) { std::swap(inPin, outPin); }

        const auto isValidLink = inPin->type == Pin::Type::Input && outPin->type == Pin::Type::Output;
        if (!isValidLink) {
          ax::NodeEditor::RejectNewItem(originPin->getInvalidLinkPreviewColor(),
                                        originPin->getInvalidLinkPreviewThickness());
          return;
        }

        if (ax::NodeEditor::AcceptNewItem(originPin->getValidLinkPreviewColor(),
                                          originPin->getValidLinkPreviewThickness())) {
          auto &newLink = links.emplace_back(std::make_shared<Link>(uniqueId(), getNextId(), inPin, outPin));
          inPin->links.emplace_back(newLink);
          outPin->links.emplace_back(newLink);
          inPin->observableLink.notify(newLink);
          outPin->observableLink.notify(newLink);
        }
      }
    }
  }
}

void NodeEditor::handleNodeCreation() {
  ax::NodeEditor::PinId pinId;
  if (ax::NodeEditor::QueryNewNode(&pinId)) {
    const auto pinOpt = findPinById(pinId);
    if (pinOpt.has_value()) {
      auto pin = pinOpt.value();
      if (ax::NodeEditor::AcceptNewItem(pin->getUnconnectedLinkPreviewColor(),
                                        pin->getUnconnectedLinkPreviewThickness())) {
        suspend();
        if (createNodeRequestHandler(*pin)) { resume(); }
      }
    }
  }
}

void NodeEditor::handleDeletion() {
  auto endDelete = RAII{ax::NodeEditor::EndDelete};
  if (ax::NodeEditor::BeginDelete()) {
    handleLinkDeletion();
    handleNodeDeletion();
  }
}

void NodeEditor::handleLinkDeletion() {
  ax::NodeEditor::LinkId deletedLinkId;
  while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {
    if (ax::NodeEditor::AcceptDeletedItem()) {
      const auto getLinkId = [](auto &link) { return link->id; };
      auto iter = std::ranges::find(links, deletedLinkId, getLinkId);
      if (iter != links.end()) {
        iter->get()->invalidate();

        iter->get()->getInputPin().observableLink.notify(*iter);
        iter->get()->getOutputPin().observableLink.notify(*iter);

        links.erase(iter);
      }
    }
  }
}

void NodeEditor::handleNodeDeletion() {
  ax::NodeEditor::NodeId nodeId;
  while (ax::NodeEditor::QueryDeletedNode(&nodeId)) {
    if (ax::NodeEditor::AcceptDeletedItem()) {
      auto [rmBegin, rmEnd] = std::ranges::remove(nodes, nodeId, [](auto &node) { return node->getId(); });
      nodes.erase(rmBegin, rmEnd);
    }
  }
}

bool NodeEditor::isSuspended() const { return ax::NodeEditor::IsSuspended(); }

void NodeEditor::suspend() { ax::NodeEditor::Suspend(); }

void NodeEditor::resume() { ax::NodeEditor::Resume(); }

int NodeEditor::getNextId() { return idCounter++; }

void NodeEditor::cleanupLinks() {
  if (--sinceLastLinkCleanup == 0) {
    sinceLastLinkCleanup = LINK_CLEANUP_FREQUENCY;
    auto [beginRm, endRm] = std::ranges::remove_if(links, [](const auto &link) { return !link->isValid(); });
    links.erase(beginRm, endRm);
  }
}

}  // namespace pf::ui::ig