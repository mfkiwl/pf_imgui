//
// Created by petr on 11/14/20.
//

#ifndef PF_IMGUI_ELEMENTS_INTERFACE_CLICKABLE_H
#define PF_IMGUI_ELEMENTS_INTERFACE_CLICKABLE_H

#include <functional>
#include <pf_common/Subscription.h>
#include <pf_common/coroutines/Sequence.h>
#include <unordered_map>
#include "Observable_impl.h"

namespace pf::ui::ig {

class Clickable {
 public:
  Subscription addClickListener(std::invocable auto fnc);

 protected:
  void notifyOnClick();

 private:
  Observable_impl<> observableImpl;
};

}// namespace pf::ui::ig

#endif//PF_IMGUI_ELEMENTS_INTERFACE_CLICKABLE_H
