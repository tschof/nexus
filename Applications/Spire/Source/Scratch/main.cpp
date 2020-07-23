#include <QApplication>
#include "Spire/Spire/Resources.hpp"

using namespace Spire;

#include <boost/date_time/posix_time/posix_time.hpp>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Ui/TimeInputWidget.hpp"

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Scratch"));
  initialize_resources();
  auto w = new QWidget();
  w->resize(400, 400);
  auto layout = new QHBoxLayout(w);
  auto label = new QLabel("Null", w);
  label->move(translate(10, 10));
  auto t = new TimeInputWidget(TimeInputWidget::TimeFormat::HM, w);
  t->connect_time_signal([&] (const auto& time) {
    auto hours = time.hours();
    auto minutes = [&] {
      if(time.minutes() < 10) {
        return QString("0%1").arg(time.minutes());
      }
      return QString("%1").arg(time.minutes());
    }();
    label->setText(QString("%1:%2").arg(hours).arg(minutes));
  });
  layout->addWidget(t);
  auto t2 = new TimeInputWidget(TimeInputWidget::TimeFormat::HMS, w);
  t2->connect_time_signal([&] (const auto& time) {
    auto hours = time.hours();
    auto minutes = [&] {
      if(time.minutes() < 10) {
        return QString("0%1").arg(time.minutes());
      }
      return QString("%1").arg(time.minutes());
    }();
    auto seconds = [&] {
      if(time.seconds() < 10) {
        return QString("0%1").arg(time.seconds());
      }
      return QString("%1").arg(time.seconds());
    }();
    label->setText(QString("%1:%2:%3").arg(hours).arg(minutes).arg(seconds));
  });
  layout->addWidget(t2);
  w->show();
  application->exec();
}
