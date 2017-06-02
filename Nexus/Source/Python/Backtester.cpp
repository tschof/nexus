#include "Nexus/Python/Backtester.hpp"
#include <Beam/Python/BoostPython.hpp>
#include <Beam/Python/GilRelease.hpp>
#include "Nexus/Backtester/BacktesterEnvironment.hpp"
#include "Nexus/Backtester/BacktesterEventHandler.hpp"
#include "Nexus/Backtester/BacktesterServiceClients.hpp"
#include "Nexus/Python/PythonMarketDataClient.hpp"
#include "Nexus/Python/PythonOrderExecutionClient.hpp"
#include "Nexus/ServiceClients/VirtualServiceClients.hpp"

using namespace Beam;
using namespace Beam::Python;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::python;
using namespace Nexus;
using namespace Nexus::MarketDataService;
using namespace Nexus::Python;
using namespace std;

namespace {
  std::shared_ptr<BacktesterEnvironment> MakeBacktesterEnvironmentA(
      const ptime& startTime, VirtualServiceClients& serviceClients) {
    return std::make_shared<BacktesterEnvironment>(startTime,
      Ref(serviceClients));
  }

  std::shared_ptr<BacktesterEnvironment> MakeBacktesterEnvironmentB(
      const ptime& startTime, const ptime& endTime,
      VirtualServiceClients& serviceClients) {
    return std::make_shared<BacktesterEnvironment>(startTime, endTime,
      Ref(serviceClients));
  }

  std::shared_ptr<BacktesterEventHandler> MakeBacktesterEventHandlerA(
      const ptime& startTime) {
    return std::make_shared<BacktesterEventHandler>(startTime);
  }

  std::shared_ptr<BacktesterEventHandler> MakeBacktesterEventHandlerB(
      const ptime& startTime, const ptime& endTime) {
    return std::make_shared<BacktesterEventHandler>(startTime, endTime);
  }

  class PythonBacktesterServiceClients :
      public WrapperServiceClients<std::unique_ptr<VirtualServiceClients>> {
    public:
      PythonBacktesterServiceClients(
        std::unique_ptr<VirtualServiceClients> client,
        std::shared_ptr<BacktesterEnvironment> environment)
          : WrapperServiceClients<std::unique_ptr<VirtualServiceClients>>{
              std::move(client)},
            m_environment{std::move(environment)} {}

      virtual ~PythonBacktesterServiceClients() override {
        GilRelease gil;
        boost::lock_guard<GilRelease> lock{gil};
        Close();
      }

      virtual PythonMarketDataClient& GetMarketDataClient() override {
        if(m_marketDataClient == nullptr) {
          m_marketDataClient = std::make_unique<PythonMarketDataClient>(
            MakeVirtualMarketDataClient(
            &WrapperServiceClients<std::unique_ptr<VirtualServiceClients>>::
            GetMarketDataClient()));
        }
        return *m_marketDataClient;
      }

      virtual PythonOrderExecutionClient& GetOrderExecutionClient() override {
        if(m_orderExecutionClient == nullptr) {
          m_orderExecutionClient =
            std::make_unique<PythonOrderExecutionClient>(
            MakeVirtualOrderExecutionClient(
            &WrapperServiceClients<std::unique_ptr<VirtualServiceClients>>::
            GetOrderExecutionClient()));
        }
        return *m_orderExecutionClient;
      }

    private:
      std::shared_ptr<BacktesterEnvironment> m_environment;
      std::unique_ptr<PythonMarketDataClient> m_marketDataClient;
      std::unique_ptr<PythonOrderExecutionClient> m_orderExecutionClient;
  };

  VirtualServiceClients* BuildBacktesterServiceClients(
      std::shared_ptr<BacktesterEnvironment> environment) {
    auto baseClient = std::make_unique<BacktesterServiceClients>(
      Ref(*environment));
    return new PythonBacktesterServiceClients{
      MakeVirtualServiceClients(std::move(baseClient)), environment};
  }

  void BacktesterEventHandlerAddEvents(BacktesterEventHandler& eventHandler,
      const object& events) {
    stl_input_iterator<std::shared_ptr<BacktesterEvent>> begin{events};
    stl_input_iterator<std::shared_ptr<BacktesterEvent>> end;
    vector<std::shared_ptr<BacktesterEvent>> e{begin, end};
    eventHandler.Add(std::move(e));
  }
}

void Nexus::Python::ExportBacktester() {
  ExportBacktesterEnvironment();
  ExportBacktesterEventHandler();
  ExportBacktesterServiceClients();
}

void Nexus::Python::ExportBacktesterEnvironment() {
  class_<BacktesterEnvironment, std::shared_ptr<BacktesterEnvironment>,
      boost::noncopyable>("BacktesterEnvironment", no_init)
    .def("__init__", make_constructor(&MakeBacktesterEnvironmentA))
    .def("__init__", make_constructor(&MakeBacktesterEnvironmentB))
    .add_property("event_handler", make_function(
      static_cast<BacktesterEventHandler& (BacktesterEnvironment::*)()>(
      &BacktesterEnvironment::GetEventHandler), return_internal_reference<>()))
    .add_property("market_data_server", make_function(
      static_cast<BacktesterMarketDataService& (BacktesterEnvironment::*)()>(
      &BacktesterEnvironment::GetMarketDataService),
      return_internal_reference<>()))
    .def("open", BlockingFunction(&BacktesterEnvironment::Open))
    .def("close", BlockingFunction(&BacktesterEnvironment::Close));
}

void Nexus::Python::ExportBacktesterEventHandler() {
  class_<BacktesterEventHandler, std::shared_ptr<BacktesterEventHandler>,
      boost::noncopyable>("BacktesterEventHandler", no_init)
    .def("__init__", make_constructor(&MakeBacktesterEventHandlerA))
    .def("__init__", make_constructor(&MakeBacktesterEventHandlerB))
    .add_property("start_time", &BacktesterEventHandler::GetStartTime)
    .add_property("end_time", &BacktesterEventHandler::GetEndTime)
    .def("add", static_cast<void (BacktesterEventHandler::*)(
      std::shared_ptr<BacktesterEvent>)>(&BacktesterEventHandler::Add))
    .def("add", &BacktesterEventHandlerAddEvents)
    .def("open", BlockingFunction(&BacktesterEventHandler::Open))
    .def("close", BlockingFunction(&BacktesterEventHandler::Close));
}

void Nexus::Python::ExportBacktesterServiceClients() {
  class_<PythonBacktesterServiceClients, boost::noncopyable,
      bases<VirtualServiceClients>>("BacktesterServiceClients", no_init)
    .def("__init__", make_constructor(&BuildBacktesterServiceClients))
    .def("get_service_locator_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetServiceLocatorClient,
      return_internal_reference<>()))
    .def("get_registry_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetRegistryClient,
      return_internal_reference<>()))
    .def("get_administration_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetAdministrationClient,
      return_internal_reference<>()))
    .def("get_definitions_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetDefinitionsClient,
      return_internal_reference<>()))
    .def("get_market_data_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetMarketDataClient,
      return_internal_reference<>()))
    .def("get_charting_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetChartingClient,
      return_internal_reference<>()))
    .def("get_compliance_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetComplianceClient,
      return_internal_reference<>()))
    .def("get_order_execution_client",
      BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetOrderExecutionClient,
      return_internal_reference<>()))
    .def("get_risk_client", BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetRiskClient,
      return_internal_reference<>()))
    .def("get_time_client", BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::GetTimeClient,
      return_internal_reference<>()))
    .def("build_timer", ReleaseUniquePtr<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::BuildTimer))
    .def("open", BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::Open))
    .def("close", BlockingFunction<PythonBacktesterServiceClients>(
      &PythonBacktesterServiceClients::Close));
}
