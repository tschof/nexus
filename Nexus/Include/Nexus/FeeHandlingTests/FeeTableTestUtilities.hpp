#ifndef NEXUS_FEETABLETESTUTILITIES_HPP
#define NEXUS_FEETABLETESTUTILITIES_HPP
#include <array>
#include <Beam/Pointers/Out.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include "Nexus/Definitions/DefaultCountryDatabase.hpp"
#include "Nexus/Definitions/DefaultCurrencyDatabase.hpp"
#include "Nexus/Definitions/DefaultDestinationDatabase.hpp"
#include "Nexus/Definitions/DefaultMarketDatabase.hpp"
#include "Nexus/Definitions/Security.hpp"
#include "Nexus/Definitions/Money.hpp"
#include "Nexus/FeeHandling/LiquidityFlag.hpp"
#include "Nexus/FeeHandlingTests/FeeHandlingTests.hpp"
#include "Nexus/OrderExecutionService/ExecutionReport.hpp"
#include "Nexus/OrderExecutionService/OrderFields.hpp"

namespace Nexus {
namespace Tests {

  //! Populates a fee table with incremental CENT values.
  /*!
    \param feeTable The fee table to populate.
  */
  template<std::size_t COLUMNS>
  void PopulateFeeTable(Beam::Out<std::array<Money, COLUMNS>> feeTable) {
    auto fee = Money::CENT;
    for(auto i = 0; i < COLUMNS; ++i) {
      (*feeTable)[i] = fee;
      fee += Money::CENT;
    }
  }

  //! Populates a fee table with incremental CENT values.
  /*!
    \param feeTable The fee table to populate.
  */
  template<std::size_t ROWS, std::size_t COLUMNS>
  void PopulateFeeTable(
      Beam::Out<std::array<std::array<Money, COLUMNS>, ROWS>> feeTable) {
    auto fee = Money::CENT;
    for(auto i = 0; i < ROWS; ++i) {
      for(auto j = 0; j < COLUMNS; ++j) {
        (*feeTable)[i][j] = fee;
        fee += Money::CENT;
      }
    }
  }

  //! Tests indexing into a fee table.
  /*!
    \param parentTable The parent table containing all the fees.
    \param feeTable The specific fee table within the <i>parentTable</i> to
           test.
    \param indexFunction The function used to index the <i>parentTable</i>.
    \param expectedRows The expected number of rows.
    \param expectedColumns The expected number of columns.
  */
  template<typename FeeTable, std::size_t ROWS, std::size_t COLUMNS,
    typename IndexFunction>
  void TestFeeTableIndex(const FeeTable& parentTable,
      const std::array<std::array<Money, COLUMNS>, ROWS>& feeTable,
      const IndexFunction& indexFunction, std::size_t expectedColumns,
      std::size_t expectedRows) {
    using ColumnType = typename boost::function_traits<
      IndexFunction>::arg2_type;
    using RowType = typename boost::function_traits<IndexFunction>::arg3_type;
    CPPUNIT_ASSERT(COLUMNS == expectedColumns);
    CPPUNIT_ASSERT(ROWS == expectedRows);
    for(auto i = 0; i < ROWS; ++i) {
      for(auto j = 0; j < COLUMNS; ++j) {
        auto row = static_cast<RowType>(i);
        auto column = static_cast<ColumnType>(j);
        auto fee = indexFunction(parentTable, column, row);
        auto expectedFee = feeTable[i][j];
        CPPUNIT_ASSERT(expectedFee == fee);
      }
    }
  }

  //! Tests a fee calculation.
  /*!
    \param feeTable The fee table to test.
    \param price The price of the trade to test.
    \param quantity The trade's quantity.
    \param liquidityFlag The trade's LiquidityFlag.
    \param calculateFee The function used to calculate the fee.
    \param expectedFee The expected fee.
  */
  template<typename FeeTable, typename CalculateFeeType>
  void TestFeeCalculation(const FeeTable& feeTable, Money price,
      Quantity quantity, LiquidityFlag liquidityFlag,
      CalculateFeeType&& calculateFee, Money expectedFee) {
    auto executionReport = OrderExecutionService::ExecutionReport::
      BuildInitialReport(0, boost::posix_time::second_clock::universal_time());
    executionReport.m_lastPrice = price;
    executionReport.m_lastQuantity = quantity;
    executionReport.m_liquidityFlag = ToString(liquidityFlag);
    auto calculatedTotal = calculateFee(feeTable, executionReport);
    CPPUNIT_ASSERT(calculatedTotal == expectedFee);
  }

  //! Tests a notional value fee calculation.
  /*!
    \param feeTable The fee table to test.
    \param orderFields The OrderFields used to produce an ExecutionReport.
    \param calculateFee The function used to calculate the fee.
    \param expectedRate The expected rate to use in the calculation.
  */
  template<typename FeeTable, typename CalculateFeeType>
  void TestNotionalValueFeeCalculation(const FeeTable& feeTable,
      const OrderExecutionService::OrderFields& orderFields,
      CalculateFeeType&& calculateFee, Money expectedRate) {
    auto executionReport = OrderExecutionService::ExecutionReport::
      BuildInitialReport(0, boost::posix_time::second_clock::universal_time());
    executionReport.m_lastPrice = orderFields.m_price;
    executionReport.m_lastQuantity = orderFields.m_quantity;
    auto calculatedTotal = calculateFee(feeTable, executionReport);
    auto expectedTotal = expectedRate *
      (executionReport.m_lastQuantity * executionReport.m_lastPrice);
    CPPUNIT_ASSERT(calculatedTotal == expectedTotal);
  }

  //! Tests a per share fee calculation.
  /*!
    \param feeTable The fee table to test.
    \param orderFields The OrderFields used to produce an ExecutionReport.
    \param liquidityFlag The trade's LiquidityFlag.
    \param calculateFee The function used to calculate the fee.
    \param expectedFee The expected fee to use in the calculation.
  */
  template<typename FeeTable, typename CalculateFeeType>
  void TestPerShareFeeCalculation(const FeeTable& feeTable,
      const OrderExecutionService::OrderFields& orderFields,
      const std::string& liquidityFlag, CalculateFeeType&& calculateFee,
      Money expectedFee) {
    auto executionReport = OrderExecutionService::ExecutionReport::
      BuildInitialReport(0, boost::posix_time::second_clock::universal_time());
    executionReport.m_lastPrice = orderFields.m_price;
    executionReport.m_lastQuantity = orderFields.m_quantity;
    executionReport.m_liquidityFlag = liquidityFlag;
    auto calculatedTotal = calculateFee(feeTable, orderFields, executionReport);
    auto expectedTotal = executionReport.m_lastQuantity * expectedFee;
    CPPUNIT_ASSERT(calculatedTotal == expectedTotal);
  }

  //! Tests a per share fee calculation.
  /*!
    \param feeTable The fee table to test.
    \param orderFields The OrderFields used to produce an ExecutionReport.
    \param liquidityFlag The trade's LiquidityFlag.
    \param calculateFee The function used to calculate the fee.
    \param expectedFee The expected fee to use in the calculation.
  */
  template<typename FeeTable, typename CalculateFeeType>
  void TestPerShareFeeCalculation(const FeeTable& feeTable,
      const OrderExecutionService::OrderFields& orderFields,
      LiquidityFlag liquidityFlag, CalculateFeeType&& calculateFee,
      Money expectedFee) {
    TestPerShareFeeCalculation(feeTable, orderFields, ToString(liquidityFlag),
      std::forward<CalculateFeeType>(calculateFee), expectedFee);
  }

  //! Tests a per share fee calculation.
  /*!
    \param feeTable The fee table to test.
    \param price The price of the trade to test.
    \param quantity The trade's quantity.
    \param liquidityFlag The trade's LiquidityFlag.
    \param calculateFee The function used to calculate the fee.
    \param expectedFee The expected fee to use in the calculation.
  */
  template<typename FeeTable, typename CalculateFeeType>
  void TestPerShareFeeCalculation(const FeeTable& feeTable, Money price,
      Quantity quantity, const std::string& liquidityFlag,
      CalculateFeeType&& calculateFee, Money expectedFee) {
    auto executionReport = OrderExecutionService::ExecutionReport::
      BuildInitialReport(0, boost::posix_time::second_clock::universal_time());
    executionReport.m_lastPrice = price;
    executionReport.m_lastQuantity = quantity;
    executionReport.m_liquidityFlag = liquidityFlag;
    auto calculatedTotal = calculateFee(feeTable, executionReport);
    auto expectedTotal = executionReport.m_lastQuantity * expectedFee;
    CPPUNIT_ASSERT(calculatedTotal == expectedTotal);
  }

  //! Tests a per share fee calculation.
  /*!
    \param feeTable The fee table to test.
    \param price The price of the trade to test.
    \param quantity The trade's quantity.
    \param liquidityFlag The trade's LiquidityFlag.
    \param calculateFee The function used to calculate the fee.
    \param expectedFee The expected fee to use in the calculation.
  */
  template<typename FeeTable, typename CalculateFeeType>
  void TestPerShareFeeCalculation(const FeeTable& feeTable, Money price,
      Quantity quantity, LiquidityFlag liquidityFlag,
      CalculateFeeType&& calculateFee, Money expectedFee) {
    TestPerShareFeeCalculation(feeTable, price, quantity,
      ToString(liquidityFlag), std::forward<CalculateFeeType>(calculateFee),
      expectedFee);
  }
}
}

#endif
