#include <QApplication>
#include "Nexus/Definitions/DefaultMarketDatabase.hpp"
#include "Nexus/Definitions/Security.hpp"
#include "Nexus/Definitions/SecurityInfo.hpp"
#include "spire/charting/charting_window.hpp"
#include "spire/security_input/local_security_input_model.hpp"
#include "spire/spire/resources.hpp"
#include "spire/version.hpp"

using namespace Beam;
using namespace Nexus;
using namespace Spire;

int main(int argc, char** argv) {
  auto application = new QApplication(argc, argv);
  application->setOrganizationName(QObject::tr("Eidolon Systems Ltd"));
  application->setApplicationName(QObject::tr("Charting Window Tester"));
  initialize_resources();
  LocalSecurityInputModel model;
  model.add(SecurityInfo(
    Security("MSFT", DefaultMarkets::NASDAQ(), DefaultCountries::US()),
    "Microsoft Corp", "Software"));
  model.add(SecurityInfo(
    Security("MG", DefaultMarkets::TSX(), DefaultCountries::CA()),
    "Magna International Inc.", "Automotive, probably"));
  model.add(SecurityInfo(
    Security("MFC", DefaultMarkets::TSX(), DefaultCountries::CA()),
    "Manulife Financial Corporation", "Finance"));
  model.add(SecurityInfo(
    Security("MX", DefaultMarkets::TSX(), DefaultCountries::CA()),
    "Methanex Corporation", ""));
  model.add(SecurityInfo(
    Security("MRU", DefaultMarkets::TSX(), DefaultCountries::CA()),
    "Metro Inc.", ""));
  model.add(SecurityInfo(
    Security("MON", DefaultMarkets::NYSE(), DefaultCountries::US()),
    "Monsanto Co.", ""));
  model.add(SecurityInfo(
    Security("MS", DefaultMarkets::NYSE(), DefaultCountries::US()),
    "Morgan Stanley", "Finance"));
  auto window = new ChartingWindow(Ref(model));
  window->show();
  application->exec();
}