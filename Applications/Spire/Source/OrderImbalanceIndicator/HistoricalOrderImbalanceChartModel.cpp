#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartModel.hpp"

using namespace boost::signals2;
using namespace Nexus;
using namespace Spire;

HistoricalOrderImbalanceChartModel::HistoricalOrderImbalanceChartModel(
  const Security& security,
  std::shared_ptr<OrderImbalanceIndicatorModel> model)
  : m_security(security),
    m_model(std::move(model)) {}

QtPromise<std::vector<OrderImbalance>>
    HistoricalOrderImbalanceChartModel::load(const TimeInterval& interval) {
  return m_model->load(m_security, interval);
}

QtPromise<std::vector<OrderImbalance>>
    HistoricalOrderImbalanceChartModel::resize(const TimeInterval& interval,
    int count) {

}

QtPromise<std::vector<OrderImbalance>>
    HistoricalOrderImbalanceChartModel::shift(const TimeInterval& interval,
    int value) {

}

connection HistoricalOrderImbalanceChartModel::connect_published_signal(
    const PublishedSignal::slot_type& slot) const {
  return m_published_signal.connect(slot);
}
