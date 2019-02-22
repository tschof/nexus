#include "spire/spire/qt_promise_event.hpp"

using namespace Spire;
using namespace Spire::details;

const QEvent::Type QtDeferredExecutionEvent::EVENT_TYPE =
  static_cast<QEvent::Type>(QEvent::registerEventType());

QtDeferredExecutionEvent::QtDeferredExecutionEvent()
    : QEvent(EVENT_TYPE) {}

const QEvent::Type QtBasePromiseEvent::EVENT_TYPE =
  static_cast<QEvent::Type>(QEvent::registerEventType());

QtBasePromiseEvent::QtBasePromiseEvent()
    : QEvent(EVENT_TYPE) {}
