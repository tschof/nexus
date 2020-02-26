#include <QApplication>
#include "Spire/Spire/Resources.hpp"

using namespace Spire;

#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceWidget.hpp"
#include "Spire/Spire/Dimensions.hpp"

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Scratch"));
  initialize_resources();
  auto widget = new HistoricalOrderImbalanceWidget();
  widget->setFixedSize(scale(700, 255));
  widget->show();
  application->exec();
}
