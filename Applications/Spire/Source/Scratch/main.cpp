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

auto make(boost::posix_time::ptime time) {
  auto rand = std::default_random_engine(std::random_device()());
  return Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    Nexus::Quantity(rand() % 10000), Nexus::Money(rand() % 100), time);
}

auto test_model() {
  auto model = std::make_shared<LocalOrderImbalanceIndicatorModel>();
  auto time = boost::posix_time::ptime({2005, 10, 10});
  for(auto i = 0; i < 1000; ++i) {
    model->insert(make(time));
    time += boost::posix_time::hours(12);
  }
  return model;
}

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Scratch"));
  initialize_resources();
  auto widget = new HistoricalOrderImbalanceWidget(test_model());
  widget->setFixedSize(scale(700, 255));
  widget->show();
  application->exec();
}
