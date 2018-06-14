#include <QApplication>
#include "spire/spire/resources.hpp"
#include "spire/toolbar/recently_closed_model.hpp"
#include "spire/toolbar/toolbar_window.hpp"
#include "spire/toolbar_ui_tester/toolbar_ui_tester.hpp"
#include "spire/ui/window.hpp"

using namespace Beam;
using namespace Beam::ServiceLocator;
using namespace spire;

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Toolbar UI Tester"));
  initialize_resources();
  recently_closed_model model;
  auto account = DirectoryEntry::MakeAccount(1, "demo_account");
  toolbar_window tw(model, account);
  tw.show();
  toolbar_ui_tester tester(&tw, model);
  tester.show();
  application->exec();
}