#include <QApplication>
#include "Spire/Spire/Resources.hpp"

using namespace Spire;

#include <QHBoxLayout>
#include <QWidget>
#include "Spire/Ui/TimeInputWidget.hpp"

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Scratch"));
  initialize_resources();
  auto w = new QWidget();
  w->resize(400, 400);
  auto layout = new QHBoxLayout(w);
  auto t = new TimeInputWidget(TimeInputWidget::TimeFormat::HM, w);
  layout->addWidget(t);
  auto t2 = new TimeInputWidget(TimeInputWidget::TimeFormat::HMS, w);
  layout->addWidget(t2);
  w->show();
  application->exec();
}
