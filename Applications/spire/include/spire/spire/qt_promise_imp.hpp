#ifndef SPIRE_QT_PROMISE_IMP_HPP
#define SPIRE_QT_PROMISE_IMP_HPP
#include <functional>
#include <memory>
#include <utility>
#include <Beam/Routines/RoutineHandler.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <QCoreApplication>
#include <QEvent>
#include <QObject>
#include "spire/spire/spire.hpp"
#include "spire/spire/qt_promise_event.hpp"

namespace Spire {

  /** Specifies how a QtPromise's executor should be invoked. */
  enum class LaunchPolicy {

    /** The executor should be invoked within Qt's event loop. */
    DEFERRED,

    /** The executor should be invoked in a Routine. */
    ROUTINE
  };

namespace details {
  template<typename T>
  class BaseQtPromiseImp : public QObject, private boost::noncopyable {
    public:
      using Type = T;
      using ContinuationType = std::function<void (Beam::Expect<Type> value)>;

      virtual ~BaseQtPromiseImp() = default;

      virtual void then(ContinuationType continuation) = 0;

      virtual void disconnect() = 0;
  };

  template<>
  class BaseQtPromiseImp<void> : public QObject, private boost::noncopyable {
    public:
      using Type = void;
      using ContinuationType = std::function<void (Beam::Expect<void> value)>;

      virtual ~BaseQtPromiseImp() = default;

      virtual void then(ContinuationType continuation) = 0;

      virtual void disconnect() = 0;
  };

  template<typename Executor>
  class qt_promise_imp final :
      public BaseQtPromiseImp<std::invoke_result_t<Executor>> {
    public:
      using Super = BaseQtPromiseImp<std::invoke_result_t<Executor>>;
      using Type = typename Super::Type;
      using ContinuationType = typename Super::ContinuationType;

      template<typename ExecutorForward>
      explicit qt_promise_imp(ExecutorForward&& executor,
        LaunchPolicy launch_policy);

      ~qt_promise_imp() override;

      void bind(std::shared_ptr<void> self);

      void then(ContinuationType continuation) override;

      void disconnect() override;

    protected:
      bool event(QEvent* event) override;

    private:
      bool m_is_disconnected;
      Executor m_executor;
      LaunchPolicy m_launch_policy;
      boost::optional<Beam::Expect<Type>> m_value;
      boost::optional<ContinuationType> m_continuation;
      std::shared_ptr<void> m_self;
      Beam::Routines::RoutineHandler m_routine;
  };

  template<typename Executor>
  template<typename ExecutorForward>
  qt_promise_imp<Executor>::qt_promise_imp(ExecutorForward&& executor,
      LaunchPolicy launch_policy)
      : m_is_disconnected(false),
        m_executor(std::forward<ExecutorForward>(executor)),
        m_launch_policy(launch_policy) {}

  template<typename Executor>
  qt_promise_imp<Executor>::~qt_promise_imp() {
    disconnect();
  }

  template<typename Executor>
  void qt_promise_imp<Executor>::bind(std::shared_ptr<void> self) {
    m_self = std::move(self);
    if(m_launch_policy == LaunchPolicy::DEFERRED) {
      QCoreApplication::postEvent(this, new QtDeferredExecutionEvent());
    } else if(m_launch_policy == LaunchPolicy::ROUTINE) {
      m_routine = Beam::Routines::Spawn(
        [=] {
          QCoreApplication::postEvent(this,
            make_qt_promise_event(Beam::Try(m_executor)));
        });
    }
  }

  template<typename Executor>
  void qt_promise_imp<Executor>::then(ContinuationType continuation) {
    m_continuation.emplace(std::move(continuation));
    if(m_value.is_initialized()) {
      QCoreApplication::postEvent(this,
        make_qt_promise_event(std::move(*m_value)));
      m_value = boost::none;
    }
  }

  template<typename Executor>
  void qt_promise_imp<Executor>::disconnect() {
    m_is_disconnected = true;
  }

  template<typename Executor>
  bool qt_promise_imp<Executor>::event(QEvent* event) {
    if(event->type() == QtDeferredExecutionEvent::EVENT_TYPE) {
      QCoreApplication::postEvent(this,
        make_qt_promise_event(Beam::Try(m_executor)));
      return true;
    } else if(event->type() == QtBasePromiseEvent::EVENT_TYPE) {
      if(m_is_disconnected) {
        m_self = nullptr;
        return true;
      }
      auto& promise_event = *static_cast<QtPromiseEvent<Type>*>(event);
      if(m_continuation.is_initialized()) {
        (*m_continuation)(std::move(promise_event.get_result()));
        disconnect();
        m_self = nullptr;
      } else {
        m_value = std::move(promise_event.get_result());
      }
      return true;
    } else {
      return QObject::event(event);
    }
  }
}
}

#endif
