#ifndef NEXUS_ARCAFEETABLE_HPP
#define NEXUS_ARCAFEETABLE_HPP
#include <array>
#include <Beam/Utilities/YamlConfig.hpp>
#include <boost/rational.hpp>
#include "Nexus/Definitions/Money.hpp"
#include "Nexus/FeeHandling/FeeHandling.hpp"
#include "Nexus/FeeHandling/LiquidityFlag.hpp"
#include "Nexus/OrderExecutionService/ExecutionReport.hpp"
#include "Nexus/OrderExecutionService/OrderFields.hpp"

namespace Nexus {

  /*! \struct ArcaFeeTable
      \brief Stores the table of fees used by ARCA.
   */
  struct ArcaFeeTable {

    enum class Category : int {

      //! Default category.
      DEFAULT = 0,

      //! Order was routed to another market.
      ROUTED,

      //! Order participated in an auction.
      AUCTION
    };

    //! The number of categories enumerated.
    static const std::size_t CATEGORY_COUNT = 3;

    /*! \enum Tape
        \brief Enumerates the tapes.
     */
    enum class Tape : int {

      //! Tape A - NYSE
      A = 0,

      //! Tape B - AMEX, ARCA, BATS, etc...
      B,

      //! Tape C - NASDAQ
      C
    };

    //! The number of tapes enumerated.
    static const std::size_t TAPE_COUNT = 3;

    /*! \enum Type
        \brief Enumerates the types of trades.
     */
    enum class Type : int {

      //! Active.
      ACTIVE = 0,

      //! Passive.
      PASSIVE,

      //! Hidden Active.
      HIDDEN_ACTIVE,

      //! Hidden Passive.
      HIDDEN_PASSIVE,

      //! At the open.
      AT_THE_OPEN,

      //! At the close.
      AT_THE_CLOSE
    };

    //! The number of trade types enumerated.
    static const std::size_t TYPE_COUNT = 6;

    /*! \enum SubDollarType
        \brief Enumerates the types of sub dollar trades.
     */
    enum class SubDollarType {

      //! The order was routed to another market.
      ROUTED = 0,

      //! The order participated in an auction.
      AUCTION,

      //! The order removed liquidity.
      ACTIVE,

      //! The order added liquidity.
      PASSIVE
    };

    //! The number of sub dollar types enumerated.
    static const std::size_t SUB_DOLLAR_TYPE_COUNT = 4;

    //! The fee table.
    std::array<std::array<Money, TYPE_COUNT>, TAPE_COUNT> m_feeTable;

    //! The sub-dollar rates.
    std::array<boost::rational<int>, SUB_DOLLAR_TYPE_COUNT> m_subDollarTable;

    //! The fee for routed orders.
    Money m_routedFee;

    //! The fee for auction orders.
    Money m_auctionFee;
  };

  //! Parses an ArcaFeeTable from a YAML configuration.
  /*!
    \param config The configuration to parse the ArcaFeeTable from.
    \return The ArcaFeeTable represented by the <i>config</i>.
  */
  inline ArcaFeeTable ParseArcaFeeTable(const YAML::Node& config) {
    ArcaFeeTable feeTable;
    ParseFeeTable(config, "fee_table", Beam::Store(feeTable.m_feeTable));
    ParseFeeTable(config, "sub_dollar_table",
      Beam::Store(feeTable.m_subDollarTable));
    feeTable.m_routedFee = Beam::Extract<Money>(config, "routed_fee");
    feeTable.m_auctionFee = Beam::Extract<Money>(config, "auction_fee");
    return feeTable;
  }

  //! Looks up a fee.
  /*!
    \param feeTable The ArcaFeeTable used to lookup the fee.
    \param tape The tape the Security is listed on.
    \param type The trade's type.
    \return The fee corresponding to the specified <i>tape</i> and <i>type</i>.
  */
  inline Money LookupFee(const ArcaFeeTable& feeTable, ArcaFeeTable::Tape tape,
      ArcaFeeTable::Type type) {
    return feeTable.m_feeTable[static_cast<int>(tape)][
      static_cast<int>(type)];
  }

  //! Looks up a fee.
  /*!
    \param feeTable The ArcaFeeTable used to lookup the fee.
    \param type The trade's type.
    \return The fee corresponding to the specified <i>type</i>.
  */
  inline boost::rational<int> LookupFee(const ArcaFeeTable& feeTable,
      ArcaFeeTable::SubDollarType type) {
    return feeTable.m_subDollarTable[static_cast<int>(type)];
  }

  //! Tests if an OrderFields represents a hidden liquidity provider.
  /*!
    \param fields The OrderFields to test.
    \return <code>true</code> iff the <i>order</i> counts as a hidden liquidity
            provider.
  */
  inline bool IsArcaHiddenLiquidityProvider(
      const OrderExecutionService::OrderFields& fields) {
    return fields.m_type == OrderType::PEGGED &&
      OrderExecutionService::HasField(fields, Tag{18, "M"});
  }

  //! Calculates the fee on a trade executed on ARCA.
  /*!
    \param feeTable The ArcaFeeTable used to calculate the fee.
    \param fields The OrderFields used to place the Order.
    \param executionReport The ExecutionReport to calculate the fee for.
    \return The fee calculated for the specified trade.
  */
  inline Money CalculateFee(const ArcaFeeTable& feeTable,
      const OrderExecutionService::OrderFields& fields,
      const OrderExecutionService::ExecutionReport& executionReport) {
    if(executionReport.m_lastQuantity == 0) {
      return Money::ZERO;
    }
    auto isRouted = executionReport.m_liquidityFlag == "C" ||
      executionReport.m_liquidityFlag == "F" ||
      executionReport.m_liquidityFlag == "H" ||
      executionReport.m_liquidityFlag == "N" ||
      executionReport.m_liquidityFlag == "W" ||
      executionReport.m_liquidityFlag == "X";
    auto isAuction = !isRouted && (executionReport.m_liquidityFlag == "G" ||
      executionReport.m_liquidityFlag == "Z");
    auto isHidden = IsArcaHiddenLiquidityProvider(fields);
    auto isOpen = executionReport.m_liquidityFlag == "G";
    auto isClose = executionReport.m_liquidityFlag == "U" ||
      executionReport.m_liquidityFlag == "Y" ||
      executionReport.m_liquidityFlag == "Z";
    auto tape =
      [&] {
        if(fields.m_security.GetMarket() == DefaultMarkets::NYSE()) {
          return ArcaFeeTable::Tape::A;
        } else if(fields.m_security.GetMarket() == DefaultMarkets::NASDAQ()) {
          return ArcaFeeTable::Tape::C;
        } else {
          return ArcaFeeTable::Tape::B;
        }
      }();
    auto isActive = executionReport.m_liquidityFlag == "E" ||
      executionReport.m_liquidityFlag == "L" ||
      executionReport.m_liquidityFlag == "R" ||
      executionReport.m_liquidityFlag == "T";
    auto isPassive = !isActive && (executionReport.m_liquidityFlag == "A" ||
      executionReport.m_liquidityFlag == "B" ||
      executionReport.m_liquidityFlag == "D" ||
      executionReport.m_liquidityFlag == "F" ||
      executionReport.m_liquidityFlag == "M" ||
      executionReport.m_liquidityFlag == "S");
    if(executionReport.m_lastPrice < Money::ONE) {
      auto type =
        [&] {
          if(isRouted) {
            return ArcaFeeTable::SubDollarType::ROUTED;
          } else if(isAuction) {
            return ArcaFeeTable::SubDollarType::AUCTION;
          } else if(isActive) {
            return ArcaFeeTable::SubDollarType::ACTIVE;
          } else if(isPassive) {
            return ArcaFeeTable::SubDollarType::PASSIVE;
          } else {
            std::cout << "Unknown liquidity flag [ARCA]: \"" <<
              executionReport.m_liquidityFlag << "\"\n";
            return ArcaFeeTable::SubDollarType::ACTIVE;
          }
        }();
      auto rate = LookupFee(feeTable, type);
      return rate *
        (executionReport.m_lastQuantity * executionReport.m_lastPrice);
    } else {
      if(isRouted) {
        return executionReport.m_lastQuantity * feeTable.m_routedFee;
      } else if(isAuction) {
        return executionReport.m_lastQuantity * feeTable.m_auctionFee;
      }
      auto type =
        [&] {
          if(isHidden) {
            if(isActive) {
              return ArcaFeeTable::Type::HIDDEN_ACTIVE;
            } else if(isPassive) {
              return ArcaFeeTable::Type::HIDDEN_PASSIVE;
            } else {
              std::cout << "Unknown liquidity flag [ARCA]: \"" <<
                executionReport.m_liquidityFlag << "\"\n";
              return ArcaFeeTable::Type::ACTIVE;
            }
          } else if(isOpen) {
            return ArcaFeeTable::Type::AT_THE_OPEN;
          } else if(isClose) {
            return ArcaFeeTable::Type::AT_THE_CLOSE;
          } else if(isActive) {
            return ArcaFeeTable::Type::ACTIVE;
          } else if(isPassive) {
            return ArcaFeeTable::Type::PASSIVE;
          } else {
            std::cout << "Unknown liquidity flag [ARCA]: \"" <<
              executionReport.m_liquidityFlag << "\"\n";
            return ArcaFeeTable::Type::ACTIVE;
          }
        }();
      auto fee = LookupFee(feeTable, tape, type);
      return executionReport.m_lastQuantity * fee;
    }
  }
}

#endif
