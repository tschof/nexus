#ifndef NEXUS_XCX2_FEE_TABLE_HPP
#define NEXUS_XCX2_FEE_TABLE_HPP
#include <array>
#include <Beam/Utilities/YamlConfig.hpp>
#include "Nexus/Definitions/Money.hpp"
#include "Nexus/FeeHandling/FeeHandling.hpp"
#include "Nexus/FeeHandling/LiquidityFlag.hpp"
#include "Nexus/OrderExecutionService/ExecutionReport.hpp"
#include "Nexus/OrderExecutionService/OrderFields.hpp"

namespace Nexus {

  /* Stores the table of fees used by CX2 on TSX. */
  struct Xcx2FeeTable {

    /* Enumerates the types of price classes. */
    enum class PriceClass : int {

      //! Unknown.
      NONE = -1,

      //! Price >= $1.00.
      DEFAULT,

      //! Price < $1.00.
      SUB_DOLLAR,

      //! Price < $0.10.
      SUB_DIME,
    };

    //! The number of price classes enumerated.
    static constexpr std::size_t PRICE_CLASS_COUNT = 3;

    /* Enumerates the types of trades. */
    enum class Type : int {

      //! Unknown.
      NONE = -1,

      //! Active.
      ACTIVE = 0,

      //! Passive.
      PASSIVE,

      //! Large Size Trade Active.
      LARGE_ACTIVE,

      //! Large Size Trade Passive.
      LARGE_PASSIVE,

      //! Hidden Active.
      HIDDEN_ACTIVE,

      //! Hidden Passive.
      HIDDEN_PASSIVE,

      //! Odd-lot.
      ODD_LOT
    };

    //! The number of trade types enumerated.
    static constexpr std::size_t TYPE_COUNT = 7;

    //! The fee table for non-TSX listed securities.
    std::array<std::array<Money, TYPE_COUNT>, PRICE_CLASS_COUNT> m_defaultTable;

    //! The fee table for TSX listed securities.
    std::array<std::array<Money, TYPE_COUNT>, PRICE_CLASS_COUNT> m_tsxTable;

    //! The large trade size threshold.
    Quantity m_largeTradeSize;
  };

  //! Parses an Xcx2FeeTable from a YAML configuration.
  /*!
    \param config The configuration to parse the Xcx2FeeTable from.
    \return The Xcx2FeeTable represented by the <i>config</i>.
  */
  inline Xcx2FeeTable ParseXcx2FeeTable(const YAML::Node& config) {
    auto feeTable = Xcx2FeeTable();
    ParseFeeTable(config, "default_table",
      Beam::Store(feeTable.m_defaultTable));
    ParseFeeTable(config, "tsx_table", Beam::Store(feeTable.m_tsxTable));
    feeTable.m_largeTradeSize = Beam::Extract<Quantity>(config,
      "large_trade_size");
    return feeTable;
  }

  //! Looks up a fee.
  /*!
    \param feeTable The Xcx2FeeTable used to lookup the fee.
    \param orderFields The OrderFields the trade took place on.
    \param type The trade's Type.
    \param priceClass The trade's PriceClass.
    \return The fee corresponding to the specified <i>type</i> and
            <i>priceClass</i>.
  */
  inline Money LookupFee(const Xcx2FeeTable& feeTable,
      const OrderExecutionService::OrderFields& fields, Xcx2FeeTable::Type type,
      Xcx2FeeTable::PriceClass priceClass) {
    if(fields.m_security.GetMarket() == DefaultMarkets::TSX()) {
      return feeTable.m_tsxTable[static_cast<int>(priceClass)][
        static_cast<int>(type)];
    } else {
      return feeTable.m_defaultTable[static_cast<int>(priceClass)][
        static_cast<int>(type)];
    }
  }

  //! Calculates the fee on a trade executed on XCX2.
  /*!
    \param feeTable The Xcx2FeeTable used to calculate the fee.
    \param orderFields The OrderFields the trade took place on.
    \param executionReport The ExecutionReport to calculate the fee for.
    \return The fee calculated for the specified trade.
  */
  inline Money CalculateFee(const Xcx2FeeTable& feeTable,
      const OrderExecutionService::OrderFields& fields,
      const OrderExecutionService::ExecutionReport& executionReport) {
    if(executionReport.m_lastQuantity == 0) {
      return Money::ZERO;
    }
    auto priceClass = [&] {
      if(executionReport.m_lastPrice < 10 * Money::CENT) {
        return Xcx2FeeTable::PriceClass::SUB_DIME;
      } else if(executionReport.m_lastPrice < Money::ONE) {
        return Xcx2FeeTable::PriceClass::SUB_DOLLAR;
      } else {
        return Xcx2FeeTable::PriceClass::DEFAULT;
      }
    }();
    auto type = [&] {
      if(executionReport.m_lastQuantity < 100) {
        return Xcx2FeeTable::Type::ODD_LOT;
      } else if(executionReport.m_liquidityFlag.size() == 1) {
        if(executionReport.m_liquidityFlag[0] == 'P' ||
            executionReport.m_liquidityFlag[0] == 'S') {
          if(executionReport.m_lastQuantity >= feeTable.m_largeTradeSize) {
            return Xcx2FeeTable::Type::LARGE_PASSIVE;
          } else {
            return Xcx2FeeTable::Type::PASSIVE;
          }
        } else if(executionReport.m_liquidityFlag[0] == 'A' ||
            executionReport.m_liquidityFlag[0] == 'C') {
          if(executionReport.m_lastQuantity >= feeTable.m_largeTradeSize) {
            return Xcx2FeeTable::Type::LARGE_ACTIVE;
          } else {
            return Xcx2FeeTable::Type::ACTIVE;
          }
        } else if(executionReport.m_liquidityFlag[0] == 'a' ||
            executionReport.m_liquidityFlag[0] == 'd') {
          return Xcx2FeeTable::Type::HIDDEN_PASSIVE;
        } else if(executionReport.m_liquidityFlag[0] == 'r' ||
            executionReport.m_liquidityFlag[0] == 'D') {
          return Xcx2FeeTable::Type::HIDDEN_ACTIVE;
        } else {
          std::cout << "Unknown liquidity flag [XCX2]: \"" <<
            executionReport.m_liquidityFlag << "\"\n";
          return Xcx2FeeTable::Type::PASSIVE;
        }
      } else {
        std::cout << "Unknown liquidity flag [XCX2]: \"" <<
          executionReport.m_liquidityFlag << "\"\n";
        return Xcx2FeeTable::Type::PASSIVE;
      }
    }();
    auto fee = LookupFee(feeTable, fields, type, priceClass);
    return executionReport.m_lastQuantity * fee;
  }
}

#endif
