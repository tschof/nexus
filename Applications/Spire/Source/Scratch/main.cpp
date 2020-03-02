#include <QApplication>
#include "Spire/Spire/Resources.hpp"

using namespace Spire;

#include <random>
#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceWidget.hpp"
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/OrderImbalanceIndicator/LocalOrderImbalanceIndicatorModel.hpp"
#include "Nexus/Definitions/Security.hpp"
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Nexus/Definitions/Side.hpp"
#include "Spire/Spire/Intervals.hpp"

auto make(boost::posix_time::ptime time) {
  auto rand = std::default_random_engine(std::random_device()());
  return Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    Nexus::Quantity(rand() % 10000), Nexus::Money(rand() % 100), time);
}

auto get_imbalances() {
  auto imbalances = std::vector<Nexus::OrderImbalance>();
  auto time = boost::posix_time::ptime({2005, 8, 1});
  for(auto i = 0; i < 500; ++i) {
    imbalances.emplace_back(make(time));
    time += boost::posix_time::hours(12);
  }
  return imbalances;
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 1})));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 1}, boost::posix_time::hours(12))));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 2})));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 2}, boost::posix_time::hours(12))));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 3})));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 3}, boost::posix_time::hours(12))));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 4})));
  //m_imbalances.emplace_back(
  //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
  //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
  //  boost::posix_time::ptime({2005, 9, 4}, boost::posix_time::hours(12))));
}

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Scratch"));
  initialize_resources();
  auto time = boost::posix_time::ptime({2005, 9, 1});
  auto widget = new HistoricalOrderImbalanceWidget(TimeInterval(time,
    time + boost::posix_time::hours(100)), get_imbalances());
  widget->resize(scale(700, 255));
  widget->show();
  application->exec();
}
