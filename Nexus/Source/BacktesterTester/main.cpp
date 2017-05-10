#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "Nexus/BacktesterTests/BacktesterMarketDataClientTester.hpp"
#include "Nexus/BacktesterTests/BacktesterOrderExecutionClientTester.hpp"
#include "Nexus/BacktesterTests/BacktesterTimerTester.hpp"

using namespace Nexus::Tests;

int main() {
  CPPUNIT_NS::TextUi::TestRunner runner;
  CppUnit::BriefTestProgressListener listener;
  runner.addTest(BacktesterMarketDataClientTester::suite());
  runner.addTest(BacktesterOrderExecutionClientTester::suite());
  runner.addTest(BacktesterTimerTester::suite());
  runner.eventManager().addListener(&listener);
  runner.setOutputter(new CPPUNIT_NS::CompilerOutputter(&runner.result(),
    CPPUNIT_NS::stdCOut()));
  auto wasSucessful = runner.run();
  return wasSucessful ? 0 : -1;
}
