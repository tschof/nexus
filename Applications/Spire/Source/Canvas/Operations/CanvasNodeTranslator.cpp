#include "Spire/Canvas/Operations/CanvasNodeTranslator.hpp"
#include <Aspen/Aspen.hpp>
#include <Beam/IO/BasicIStreamReader.hpp>
#include <Beam/Parsers/ParserPublisher.hpp>
#include <Beam/Pointers/Ref.hpp>
#include <Beam/Reactors/AlarmReactor.hpp>
#include <Beam/Reactors/PublisherReactor.hpp>
#include <Beam/Reactors/QueueReactor.hpp>
#include <Beam/Reactors/TimerReactor.hpp>
#include <Beam/Threading/LiveTimer.hpp>
#include <Beam/TimeService/ToLocalTime.hpp>
#include <Beam/TimeService/VirtualTimeClient.hpp>
#include <Beam/Utilities/DateTime.hpp>
#include <Beam/Utilities/Math.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/copy.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/transform.hpp>
#include "Nexus/MarketDataService/MarketWideDataQuery.hpp"
#include "Nexus/MarketDataService/SecurityMarketDataQuery.hpp"
#include "Nexus/MarketDataService/VirtualMarketDataClient.hpp"
#include "Nexus/OrderExecutionService/OrderReactor.hpp"
#include "Nexus/OrderExecutionService/VirtualOrderExecutionClient.hpp"
#include "Spire/Canvas/Common/BreadthFirstCanvasNodeIterator.hpp"
#include "Spire/Canvas/Common/CanvasNodeOperations.hpp"
#include "Spire/Canvas/Common/CanvasNodeVisitor.hpp"
#include "Spire/Canvas/Common/CustomNode.hpp"
#include "Spire/Canvas/Common/FunctionNode.hpp"
#include "Spire/Canvas/Common/NoneNode.hpp"
#include "Spire/Canvas/Common/SignatureNode.hpp"
#include "Spire/Canvas/ControlNodes/AggregateNode.hpp"
#include "Spire/Canvas/ControlNodes/ChainNode.hpp"
#include "Spire/Canvas/ControlNodes/SpawnNode.hpp"
#include "Spire/Canvas/ControlNodes/UntilNode.hpp"
#include "Spire/Canvas/ControlNodes/WhenNode.hpp"
#include "Spire/Canvas/IONodes/FilePathNode.hpp"
#include "Spire/Canvas/IONodes/FileReaderNode.hpp"
#include "Spire/Canvas/LuaNodes/LuaInterop.hpp"
#include "Spire/Canvas/LuaNodes/LuaScriptNode.hpp"
#include "Spire/Canvas/LuaNodes/RecordLuaReactorParameter.hpp"
#include "Spire/Canvas/MarketDataNodes/BboQuoteNode.hpp"
#include "Spire/Canvas/MarketDataNodes/OrderImbalanceQueryNode.hpp"
#include "Spire/Canvas/MarketDataNodes/TimeAndSaleQueryNode.hpp"
#include "Spire/Canvas/Operations/Translation.hpp"
#include "Spire/Canvas/OrderExecutionNodes/DefaultCurrencyNode.hpp"
#include "Spire/Canvas/OrderExecutionNodes/ExecutionReportMonitorNode.hpp"
#include "Spire/Canvas/OrderExecutionNodes/MaxFloorNode.hpp"
#include "Spire/Canvas/OrderExecutionNodes/OptionalPriceNode.hpp"
#include "Spire/Canvas/OrderExecutionNodes/OrderWrapperTaskNode.hpp"
#include "Spire/Canvas/OrderExecutionNodes/SecurityPortfolioNode.hpp"
#include "Spire/Canvas/OrderExecutionNodes/SingleOrderTaskNode.hpp"
#include "Spire/Canvas/Records/QueryNode.hpp"
#include "Spire/Canvas/Records/RecordNode.hpp"
#include "Spire/Canvas/ReferenceNodes/ProxyNode.hpp"
#include "Spire/Canvas/ReferenceNodes/ReferenceNode.hpp"
#include "Spire/Canvas/StandardNodes/AbsNode.hpp"
#include "Spire/Canvas/StandardNodes/AdditionNode.hpp"
#include "Spire/Canvas/StandardNodes/AlarmNode.hpp"
#include "Spire/Canvas/StandardNodes/CeilNode.hpp"
#include "Spire/Canvas/StandardNodes/ComparisonSignatures.hpp"
#include "Spire/Canvas/StandardNodes/CurrentDateNode.hpp"
#include "Spire/Canvas/StandardNodes/CurrentDateTimeNode.hpp"
#include "Spire/Canvas/StandardNodes/CurrentTimeNode.hpp"
#include "Spire/Canvas/StandardNodes/DivisionNode.hpp"
#include "Spire/Canvas/StandardNodes/EqualsNode.hpp"
#include "Spire/Canvas/StandardNodes/FilterNode.hpp"
#include "Spire/Canvas/StandardNodes/FirstNode.hpp"
#include "Spire/Canvas/StandardNodes/FloorNode.hpp"
#include "Spire/Canvas/StandardNodes/FoldNode.hpp"
#include "Spire/Canvas/StandardNodes/FoldOperandNode.hpp"
#include "Spire/Canvas/StandardNodes/GreaterNode.hpp"
#include "Spire/Canvas/StandardNodes/GreaterOrEqualsNode.hpp"
#include "Spire/Canvas/StandardNodes/IfNode.hpp"
#include "Spire/Canvas/StandardNodes/LastNode.hpp"
#include "Spire/Canvas/StandardNodes/LesserNode.hpp"
#include "Spire/Canvas/StandardNodes/LesserOrEqualsNode.hpp"
#include "Spire/Canvas/StandardNodes/MathSignatures.hpp"
#include "Spire/Canvas/StandardNodes/MaxNode.hpp"
#include "Spire/Canvas/StandardNodes/MinNode.hpp"
#include "Spire/Canvas/StandardNodes/MultiplicationNode.hpp"
#include "Spire/Canvas/StandardNodes/NotNode.hpp"
#include "Spire/Canvas/StandardNodes/RangeNode.hpp"
#include "Spire/Canvas/StandardNodes/RoundNode.hpp"
#include "Spire/Canvas/StandardNodes/SubtractionNode.hpp"
#include "Spire/Canvas/StandardNodes/TimeRangeParameterNode.hpp"
#include "Spire/Canvas/StandardNodes/TimerNode.hpp"
#include "Spire/Canvas/StandardNodes/UnequalNode.hpp"
#include "Spire/Canvas/SystemNodes/BlotterTaskMonitorNode.hpp"
#include "Spire/Canvas/SystemNodes/InteractionsNode.hpp"
#include "Spire/Canvas/TaskNodes/IsTerminalNode.hpp"
#include "Spire/Canvas/TaskNodes/TaskStateMonitorNode.hpp"
#include "Spire/Canvas/Types/ExecutionReportRecordType.hpp"
#include "Spire/Canvas/Types/OrderImbalanceRecordType.hpp"
#include "Spire/Canvas/Types/ParserTypes.hpp"
#include "Spire/Canvas/Types/RecordType.hpp"
#include "Spire/Canvas/Types/TaskType.hpp"
#include "Spire/Canvas/Types/TimeAndSaleRecordType.hpp"
#include "Spire/Canvas/ValueNodes/BooleanNode.hpp"
#include "Spire/Canvas/ValueNodes/CurrencyNode.hpp"
#include "Spire/Canvas/ValueNodes/DateTimeNode.hpp"
#include "Spire/Canvas/ValueNodes/DecimalNode.hpp"
#include "Spire/Canvas/ValueNodes/DestinationNode.hpp"
#include "Spire/Canvas/ValueNodes/DurationNode.hpp"
#include "Spire/Canvas/ValueNodes/IntegerNode.hpp"
#include "Spire/Canvas/ValueNodes/MarketNode.hpp"
#include "Spire/Canvas/ValueNodes/MoneyNode.hpp"
#include "Spire/Canvas/ValueNodes/OrderStatusNode.hpp"
#include "Spire/Canvas/ValueNodes/OrderTypeNode.hpp"
#include "Spire/Canvas/ValueNodes/SecurityNode.hpp"
#include "Spire/Canvas/ValueNodes/SideNode.hpp"
#include "Spire/Canvas/ValueNodes/TaskStateNode.hpp"
#include "Spire/Canvas/ValueNodes/TextNode.hpp"
#include "Spire/Canvas/ValueNodes/TimeInForceNode.hpp"
#include "Spire/Canvas/ValueNodes/TimeNode.hpp"
#include "Spire/Canvas/ValueNodes/TimeRangeNode.hpp"
#include "Spire/Canvas/ValueNodes/ValueNode.hpp"
#include "Spire/Spire/ServiceClients.hpp"
#include "Spire/Spire/UserProfile.hpp"

using namespace Beam;
using namespace Beam::IO;
using namespace Beam::Parsers;
using namespace Beam::Queries;
using namespace Beam::Reactors;
using namespace Beam::Threading;
using namespace Beam::TimeService;
using namespace boost;
using namespace boost::posix_time;
using namespace Nexus;
using namespace Nexus::MarketDataService;
using namespace Nexus::OrderExecutionService;
using namespace Spire;
using namespace std;

namespace {
  auto MakeAggregateOrderExecutionPublisher() {
    return std::make_shared<SpireAggregateOrderExecutionPublisher>(
      Initialize(UniqueFilter<const Order*>(), Initialize()));
  }

  class CanvasNodeTranslationVisitor final : private CanvasNodeVisitor {
    public:
      CanvasNodeTranslationVisitor(Ref<CanvasNodeTranslationContext> context,
        Ref<const CanvasNode> node);

      Translation Translate();
      void Visit(const AbsNode& node) override;
      void Visit(const AdditionNode& node) override;
      void Visit(const AggregateNode& node) override;
      void Visit(const AlarmNode& node) override;
      void Visit(const BlotterTaskMonitorNode& node) override;
      void Visit(const BooleanNode& node) override;
      void Visit(const CanvasNode& node) override;
      void Visit(const CeilNode& node) override;
      void Visit(const ChainNode& node) override;
      void Visit(const CurrencyNode& node) override;
      void Visit(const CurrentDateNode& node) override;
      void Visit(const CurrentDateTimeNode& node) override;
      void Visit(const CurrentTimeNode& node) override;
      void Visit(const CustomNode& node) override;
      void Visit(const DateTimeNode& node) override;
      void Visit(const DecimalNode& node) override;
      void Visit(const DefaultCurrencyNode& node) override;
      void Visit(const DestinationNode& node) override;
      void Visit(const DivisionNode& node) override;
      void Visit(const DurationNode& node) override;
      void Visit(const EqualsNode& node) override;
      void Visit(const ExecutionReportMonitorNode& node) override;
      void Visit(const FilePathNode& node) override;
      void Visit(const FileReaderNode& node) override;
      void Visit(const FilterNode& node) override;
      void Visit(const FirstNode& node) override;
      void Visit(const FloorNode& node) override;
      void Visit(const FoldNode& node) override;
      void Visit(const FoldOperandNode& node) override;
      void Visit(const GreaterNode& node) override;
      void Visit(const GreaterOrEqualsNode& node) override;
      void Visit(const IfNode& node) override;
      void Visit(const IntegerNode& node) override;
      void Visit(const IsTerminalNode& node) override;
      void Visit(const LastNode& node) override;
      void Visit(const LesserNode& node) override;
      void Visit(const LesserOrEqualsNode& node) override;
      void Visit(const LuaScriptNode& node) override;
      void Visit(const MarketNode& node) override;
      void Visit(const MaxFloorNode& node) override;
      void Visit(const MaxNode& node) override;
      void Visit(const MinNode& node) override;
      void Visit(const MoneyNode& node) override;
      void Visit(const MultiplicationNode& node) override;
      void Visit(const NoneNode& node) override;
      void Visit(const NotNode& node) override;
      void Visit(const OptionalPriceNode& node) override;
      void Visit(const OrderImbalanceQueryNode& node) override;
      void Visit(const OrderStatusNode& node) override;
      void Visit(const OrderTypeNode& node) override;
      void Visit(const OrderWrapperTaskNode& node) override;
      void Visit(const QueryNode& node) override;
      void Visit(const RangeNode& node) override;
      void Visit(const ReferenceNode& node) override;
      void Visit(const RoundNode& node) override;
      void Visit(const SecurityNode& node) override;
      void Visit(const SideNode& node) override;
      void Visit(const SingleOrderTaskNode& node) override;
      void Visit(const SpawnNode& node) override;
      void Visit(const SubtractionNode& node) override;
      void Visit(const TaskStateMonitorNode& node) override;
      void Visit(const TaskStateNode& node) override;
      void Visit(const TextNode& node) override;
      void Visit(const TimeAndSaleQueryNode& node) override;
      void Visit(const TimeInForceNode& node) override;
      void Visit(const TimeNode& node) override;
      void Visit(const TimeRangeNode& node) override;
      void Visit(const TimeRangeParameterNode& node) override;
      void Visit(const TimerNode& node) override;
      void Visit(const UnequalNode& node) override;
      void Visit(const UntilNode& node) override;
      void Visit(const WhenNode& node) override;

    private:
      CanvasNodeTranslationContext* m_context;
      const CanvasNode* m_node;
      std::optional<Translation> m_translation;

      Translation InternalTranslation(const CanvasNode& node);
      template<typename Translator>
      Translation TranslateFunction(const CanvasNode& node);
  };

  template<typename Translator>
  struct ParameterCount {
    static const int value = boost::mpl::size<typename boost::mpl::front<
      typename Translator::SupportedTypes>::type>::value - 1;
  };

  template<typename Translator, std::size_t N>
  struct FunctionTranslator {};

  template<typename Translator>
  struct FunctionTranslator<Translator, 1> {
    Translation operator ()(const vector<Translation>& arguments,
        const std::type_info& result,
        CanvasNodeTranslationContext& context) const {
      return Instantiate<Translator>(arguments[0].GetTypeInfo(), result)(
        arguments[0], context);
    }
  };

  template<typename Translator>
  struct FunctionTranslator<Translator, 2> {
    Translation operator ()(const vector<Translation>& arguments,
        const std::type_info& result,
        CanvasNodeTranslationContext& context) const {
      return Instantiate<Translator>(arguments[0].GetTypeInfo(),
        arguments[1].GetTypeInfo(), result)(arguments[0], arguments[1],
        context);
    }
  };

  template<typename Translator>
  struct FunctionTranslator<Translator, 3> {
    Translation operator ()(const vector<Translation>& arguments,
        const std::type_info& result,
        CanvasNodeTranslationContext& context) const {
      return Instantiate<Translator>(arguments[0].GetTypeInfo(),
        arguments[1].GetTypeInfo(), arguments[2].GetTypeInfo(), result)(
        arguments[0], arguments[1], arguments[2], context);
    }
  };

  struct AbsTranslator {
    template<typename T, typename R>
    struct Operation {
      R operator()(const T& arg) const {
        return std::abs(arg);
      }
    };

    template<>
    struct Operation<Nexus::Quantity, Nexus::Quantity> {
      Nexus::Quantity operator ()(const Nexus::Quantity& arg) const {
        return Nexus::Abs(arg);
      }
    };

    template<>
    struct Operation<Nexus::Money, Nexus::Money> {
      Nexus::Money operator ()(const Nexus::Money& arg) const {
        return Nexus::Abs(arg);
      }
    };

    template<typename T, typename R>
    static Translation Template(const Translation& value,
        CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T, R>(), value.Extract<Aspen::Box<T>>());
    }

    using SupportedTypes = AbsNodeSignatures::type;
  };

  struct AdditionTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left + right;
      }
    };

    template<>
    struct Operation<double, Quantity, double> {
      double operator()(const double& left, const Quantity& right) const {
        return static_cast<double>(left + right);
      }
    };

    template<>
    struct Operation<Quantity, double, double> {
      double operator()(const Quantity& left, const double& right) const {
        return static_cast<double>(left + right);
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = AdditionNodeSignatures::type;
  };

  struct CeilTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator ()(const T0& value, const T1& places) const {
        return Beam::Ceil(value, static_cast<int>(places));
      }
    };

    template<>
    struct Operation<Quantity, Quantity, Quantity> {
      Quantity operator ()(const Quantity& value,
          const Quantity& places) const {
        return Nexus::Ceil(value, static_cast<int>(places));
      }
    };

    template<>
    struct Operation<Money, Quantity, Money> {
      Money operator ()(const Money& value, const Quantity& places) const {
        return Nexus::Ceil(value, static_cast<int>(places));
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = RoundingNodeSignatures::type;
  };

  struct ChainTranslator {
    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {

      // TODO: publisher
      return Aspen::chain(initial.Extract<Aspen::Box<T>>(),
        continuation.Extract<Aspen::Box<T>>());
    }

    using SupportedTypes = ValueTypes;
  };

  struct DivisionTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator ()(const T0& left, const T1& right) const {
        if(right == 0) {
          BOOST_THROW_EXCEPTION(std::runtime_error("Division by 0."));
        }
        return left / right;
      }
    };

    template<>
    struct Operation<Nexus::Money, Nexus::Money, double> {
      double operator ()(const Nexus::Money& left,
          const Nexus::Money& right) const {
        if(right == Nexus::Money::ZERO) {
          BOOST_THROW_EXCEPTION(std::runtime_error("Division by 0."));
        }
        return left / right;
      }
    };

    template<>
    struct Operation<double, Quantity, double> {
      double operator()(const double& left, const Quantity& right) const {
        return static_cast<double>(left / right);
      }
    };

    template<>
    struct Operation<Quantity, double, double> {
      double operator()(const Quantity& left, const double& right) const {
        return static_cast<double>(left / right);
      }
    };

    template<>
    struct Operation<time_duration, Quantity, time_duration> {
      time_duration operator()(const time_duration& left,
          const Quantity& right) const {
        return left / static_cast<int>(right);
      }
    };

    template<>
    struct Operation<boost::posix_time::time_duration,
        boost::posix_time::time_duration, double> {
      double operator ()(const boost::posix_time::time_duration& left,
          const boost::posix_time::time_duration& right) const {
        if(right.ticks() == 0) {
          BOOST_THROW_EXCEPTION(std::runtime_error("Division by 0."));
        }
        return static_cast<double>(left.ticks()) / right.ticks();
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = DivisionNodeSignatures::type;
  };

  struct EqualsTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left == right;
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = EqualitySignatures::type;
  };

  struct FileReaderTranslator {
    template<typename Parser>
    static auto BuildParser(const Parser& parser) ->
        decltype(tokenize >> parser >> ('\n' | eps_p)) {
      return tokenize >> parser >> ('\n' | eps_p);
    }

    template<typename T>
    static Translation Template(const NativeType& nativeType,
        Ref<ReactorMonitor> reactorMonitor, Ref<UserProfile> userProfile,
        ParserErrorPolicy errorPolicy, const string& path) {
      using BaseParser = ParserType<T>::type;
      auto parser = BuildParser(BaseParser());
      using Parser = decltype(parser);
      auto publisher = std::make_shared<
        ParserPublisher<BasicIStreamReader<ifstream>, Parser>>(path, parser,
        errorPolicy);
      return PublisherReactor(std::move(publisher));
    }

    template<>
    static Translation Template<CurrencyId>(const NativeType& nativeType,
        Ref<ReactorMonitor> reactorMonitor, Ref<UserProfile> userProfile,
        ParserErrorPolicy errorPolicy, const string& path) {
      using BaseParser = ParserType<CurrencyId>::type;
      auto parser = BuildParser(BaseParser(userProfile->GetCurrencyDatabase()));
      using Parser = decltype(parser);
      auto publisher = std::make_shared<
        ParserPublisher<BasicIStreamReader<ifstream>, Parser>>(path, parser,
        errorPolicy);
      return PublisherReactor(std::move(publisher));
    }

    template<>
    static Translation Template<MarketCode>(const NativeType& nativeType,
        Ref<ReactorMonitor> reactorMonitor, Ref<UserProfile> userProfile,
        ParserErrorPolicy errorPolicy, const string& path) {
      using BaseParser = ParserType<MarketCode>::type;
      auto parser = BuildParser(BaseParser(userProfile->GetMarketDatabase()));
      using Parser = decltype(parser);
      auto publisher = std::make_shared<
        ParserPublisher<BasicIStreamReader<ifstream>, Parser>>(path, parser,
        errorPolicy);
      return PublisherReactor(std::move(publisher));
    }

    template<>
    static Translation Template<Record>(const NativeType& nativeType,
        Ref<ReactorMonitor> reactorMonitor, Ref<UserProfile> userProfile,
        ParserErrorPolicy errorPolicy, const string& path) {
      using BaseParser = ParserType<Record>::type;
      auto parser = BuildParser(BaseParser(
        static_cast<const RecordType&>(nativeType), Ref(userProfile)));
      using Parser = decltype(parser);
      auto publisher = std::make_shared<
        ParserPublisher<BasicIStreamReader<ifstream>, Parser>>(path, parser,
        errorPolicy);
      return PublisherReactor(std::move(publisher));
    }

    template<>
    static Translation Template<Security>(const NativeType& nativeType,
        Ref<ReactorMonitor> reactorMonitor, Ref<UserProfile> userProfile,
        ParserErrorPolicy errorPolicy, const string& path) {
      using BaseParser = ParserType<Security>::type;
      auto parser = BuildParser(BaseParser(userProfile->GetMarketDatabase()));
      using Parser = decltype(parser);
      auto publisher = std::make_shared<
        ParserPublisher<BasicIStreamReader<ifstream>, Parser>>(path, parser,
        errorPolicy);
      return PublisherReactor(std::move(publisher));
    }

    using SupportedTypes = ValueTypes;
  };

  struct FilterTranslator {
    template<typename T>
    static Translation Template(const Translation& filter,
        const Translation& source) {
      return Aspen::discard(filter.Extract<Aspen::Box<bool>>(),
        source.Extract<Aspen::Box<T>>());
    }

    using SupportedTypes = ValueTypes;
  };

  struct FirstTranslator {
    template<typename T>
    static Translation Template(const Translation& source) {
      return Aspen::first(source.Extract<Aspen::Box<T>>());
    }

    using SupportedTypes = ValueTypes;
  };

  struct FloorTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator ()(const T0& value, const T1& places) const {
        return Beam::Floor(value, static_cast<int>(places));
      }
    };

    template<>
    struct Operation<Quantity, Quantity, Quantity> {
      Quantity operator ()(const Quantity& value,
          const Quantity& places) const {
        return Nexus::Floor(value, static_cast<int>(places));
      }
    };

    template<>
    struct Operation<Money, Quantity, Money> {
      Money operator ()(const Money& value, const Quantity& places) const {
        return Nexus::Floor(value, static_cast<int>(places));
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = RoundingNodeSignatures::type;
  };

  struct FoldParameterTranslator {
    template<typename T>
    static Translation Template() {
      return Aspen::make_fold_argument<T>();
    }

    using SupportedTypes = ValueTypes;
  };

  struct FoldTranslator {
    template<typename CombinerType, typename SourceType, typename Unused>
    static Translation Template(const Translation& combiner,
        const Translation& leftTrigger, const Translation& rightTrigger,
        const Translation& source) {
      return Aspen::fold(combiner.Extract<Aspen::Box<CombinerType>>(),
        leftTrigger.Extract<Aspen::Shared<Aspen::FoldArgument<CombinerType>>>(),
        rightTrigger.Extract<Aspen::Shared<Aspen::FoldArgument<SourceType>>>(),
        source.Extract<Aspen::Box<SourceType>>());
    }

    using SupportedTypes = FoldSignatures::type;
  };

  struct GreaterTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left > right;
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = ComparisonSignatures::type;
  };

  struct GreaterOrEqualsTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left >= right;
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = ComparisonSignatures::type;
  };

  struct IfTranslator {
    template<typename T0, typename T1, typename T2, typename R>
    struct Operation {
      R operator()(const T0& condition, const T1& consequent,
          const T2& default) const {
        return condition ? consequent : default;
      }
    };

    template<typename T0, typename T1, typename T2, typename R>
    static Translation Template(const Translation& condition,
        const Translation& consequent, const Translation& default,
        CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, T2, R>(),
        condition.Extract<Aspen::Box<T0>>(),
        consequent.Extract<Aspen::Box<T1>>(),
        default.Extract<Aspen::Box<T2>>());
    }

    using SupportedTypes = IfNodeSignatures::type;
  };

  struct LastTranslator {
    template<typename T>
    static Translation Template(const Translation& source) {
      return Aspen::last(source.Extract<Aspen::Box<T>>());
    }

    using SupportedTypes = ValueTypes;
  };

  struct LesserTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left < right;
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = ComparisonSignatures::type;
  };

  struct LesserOrEqualsTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left <= right;
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = ComparisonSignatures::type;
  };

  struct LuaParameterTranslator {
    template<typename T>
    static Translation Template(const Translation& reactor,
        const CanvasType& type) {
      return Aspen::Unique(NativeLuaReactorParameter(
        reactor.Extract<Aspen::Box<T>>()));
    }

    template<>
    static Translation Template<Record>(const Translation& reactor,
        const CanvasType& type) {
      return Aspen::Unique(RecordLuaReactorParameter(
        reactor.Extract<Aspen::Box<Record>>(),
        static_cast<const RecordType&>(type)));
    }

    using SupportedTypes = ValueTypes;
  };

  struct LuaScriptTranslator {
    template<typename T>
    static Translation Template(string name,
        vector<Aspen::Unique<LuaReactorParameter>> parameters,
        lua_State& luaState) {
      return LuaReactor<T>(std::move(name), std::move(parameters), luaState);
    }

    using SupportedTypes = ValueTypes;
  };

  struct MaxTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return std::max(left, right);
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = ExtremaNodeSignatures::type;
  };

  struct MinTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return std::max(left, right);
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = ExtremaNodeSignatures::type;
  };

  struct MultiplicationTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left * right;
      }
    };

    template<>
    struct Operation<Money, Quantity, Money> {
      Money operator()(const Money& left, const Quantity& right) const {
        return right * left;
      }
    };

    template<>
    struct Operation<Quantity, Money, Money> {
      Money operator()(const Quantity& left, const Money& right) const {
        return left * right;
      }
    };

    template<>
    struct Operation<Money, double, Money> {
      Money operator()(const Money& left, const double& right) const {
        return right * left;
      }
    };

    template<>
    struct Operation<Quantity, time_duration, time_duration> {
      time_duration operator()(const Quantity& left,
          const time_duration& right) const {
        return right * static_cast<int>(left);
      }
    };

    template<>
    struct Operation<time_duration, Quantity, time_duration> {
      time_duration operator()(const time_duration& left,
          const Quantity& right) const {
        return left * static_cast<int>(right);
      }
    };

    template<>
    struct Operation<double, Quantity, double> {
      double operator()(const double& left, const Quantity& right) const {
        return static_cast<double>(left * right);
      }
    };

    template<>
    struct Operation<Quantity, double, double> {
      double operator()(const Quantity& left, const double& right) const {
        return static_cast<double>(right * left);
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = MultiplicationNodeSignatures::type;
  };

  struct NoneTranslator {
    template<typename T>
    static Translation Template() {
      return Aspen::None<T>();
    }

    using SupportedTypes = ValueTypes;
  };

  struct NotTranslator {
    template<typename T, typename R>
    struct Operation {
      R operator()(const T& arg) const {
        return !arg;
      }
    };

    template<>
    struct Operation<Side, Side> {
      Side operator()(const Side& arg) const {
        return GetOpposite(arg);
      }
    };

    template<typename T0, typename R>
    static Translation Template(const Translation& value,
        CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, R>(), value.Extract<Aspen::Box<T0>>());
    }

    using SupportedTypes = NotNodeSignatures::type;
  };

  struct QueryTranslator {
    template<typename T>
    struct Operation {
      int m_index;

      Operation(int index)
        : m_index(index) {}

      T operator ()(const Record& record) const {
        return boost::get<T>(record.GetFields()[m_index]);
      }
    };

    template<typename T>
    static Translation Template(const Translation& record, int index) {
      return Aspen::Lift(Operation<T>(index),
        record.Extract<Aspen::Box<Record>>());
    }

    using SupportedTypes = ValueTypes;
  };

  struct RoundTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& value, const T1& places) const {
        return Beam::Round(value, static_cast<int>(places));
      }
    };

    template<>
    struct Operation<Quantity, Quantity, Quantity> {
      Quantity operator ()(const Quantity& value,
          const Quantity& places) const {
        return Nexus::Round(value, static_cast<int>(places));
      }
    };

    template<>
    struct Operation<Money, Quantity, Money> {
      Money operator()(const Money& value, const Quantity& places) const {
        return Nexus::Round(value, static_cast<int>(places));
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = RoundingNodeSignatures::type;
  };

  struct SubtractionTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left - right;
      }
    };

    template<>
    struct Operation<double, Quantity, double> {
      double operator()(const double& left, const Quantity& right) const {
        return static_cast<double>(left - right);
      }
    };

    template<>
    struct Operation<Quantity, double, double> {
      double operator()(const Quantity& left, const double& right) const {
        return static_cast<double>(left - right);
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = SubtractionNodeSignatures::type;
  };

  struct ThrowTranslator {
    template<typename T>
    static Translation Template(std::exception_ptr exception) {
      return Aspen::Throw<T>(std::move(exception));
    }

    using SupportedTypes = ValueTypes;
  };

  struct UnequalTranslator {
    template<typename T0, typename T1, typename R>
    struct Operation {
      R operator()(const T0& left, const T1& right) const {
        return left != right;
      }
    };

    template<typename T0, typename T1, typename R>
    static Translation Template(const Translation& left,
        const Translation& right, CanvasNodeTranslationContext& context) {
      return Aspen::Lift(Operation<T0, T1, R>(), left.Extract<Aspen::Box<T0>>(),
        right.Extract<Aspen::Box<T1>>());
    }

    using SupportedTypes = EqualitySignatures::type;
  };

  struct WhenTranslator {
    template<typename T>
    static Translation Template(Aspen::Box<bool> condition,
        const Translation& series) {
      return Translation(
        Aspen::when(condition, series.Extract<Aspen::Box<T>>()),
        series.GetPublisher());
    }

    using SupportedTypes = ValueTypes;
  };

  struct VariableTranslator {
    template<typename T>
    static Translation Template(CanvasNodeTranslationContext& context) {
      return Aspen::Queue<T>();
    }

    using SupportedTypes = ValueTypes;
  };

  struct MinOperation {
    Quantity operator()(const Quantity& left, const Quantity& right) const {
      return min(left, right);
    }
  };

#if 0
  struct PropertyBuilder {
    template<typename T>
    static ReactorProperty Template(const string& name,
        std::shared_ptr<BaseReactor> reactor) {
      return MakeReactorProperty(name, std::static_pointer_cast<Reactor<T>>(
        reactor));
    }

    using SupportedTypes = ValueTypes;
  };
#endif
}

Translation Spire::Translate(CanvasNodeTranslationContext& context,
    const CanvasNode& node) {
  auto visitor = CanvasNodeTranslationVisitor(Ref(context), Ref(node));
  return visitor.Translate();
}

CanvasNodeTranslationVisitor::CanvasNodeTranslationVisitor(
  Ref<CanvasNodeTranslationContext> context, Ref<const CanvasNode> node)
  : m_context(context.Get()),
    m_node(node.Get()) {}

Translation CanvasNodeTranslationVisitor::Translate() {
  return InternalTranslation(*m_node);
}

void CanvasNodeTranslationVisitor::Visit(const AbsNode& node) {
  m_translation = TranslateFunction<AbsTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const AdditionNode& node) {
  m_translation = TranslateFunction<AdditionTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const AggregateNode& node) {
//  auto orderExecutionPublisher = MakeAggregateOrderExecutionPublisher();
  auto publishers = std::vector<const Publisher<const Order*>*>();
  for(const auto& child : node.GetChildren()) {
    if(dynamic_cast<const NoneNode*>(&child) == nullptr) {
      auto translation = InternalTranslation(child);
      if(translation.GetPublisher() != nullptr) {
        publishers.push_back(translation.GetPublisher());
      }
    }
  }
/*
  auto translation = [&] {
    if(node.GetType().GetCompatibility(TaskType::GetInstance()) ==
        CanvasType::Compatibility::EQUAL) {
      return Translation(Task(Aspen::box(Aspen::group(
    }
  }();
*/
/*
  auto translation = Translation(
  TaskTranslation taskTranslation;
  taskTranslation.m_publisher = orderExecutionPublisher;
  taskTranslation.m_factory = OrderExecutionPublisherTaskFactory(
    AggregateTaskFactory(factories), orderExecutionPublisher);
  m_translation = std::move(translation);
*/
}

void CanvasNodeTranslationVisitor::Visit(const AlarmNode& node) {
  auto userProfile = &m_context->GetUserProfile();
  auto timerFactory = [=] (time_duration interval) {
    return make_unique<LiveTimer>(interval,
      Ref(userProfile->GetTimerThreadPool()));
  };
  auto expiry = InternalTranslation(
    node.GetChildren().front()).Extract<Aspen::Box<ptime>>();
  m_translation = AlarmReactor(
    &m_context->GetUserProfile().GetServiceClients().GetTimeClient(),
    timerFactory, std::move(expiry));
}

void CanvasNodeTranslationVisitor::Visit(const BlotterTaskMonitorNode& node) {
  m_translation = InternalTranslation(node.GetChildren().front());
}

void CanvasNodeTranslationVisitor::Visit(const BooleanNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const CanvasNode& node) {
  if(node.GetType().GetCompatibility(TaskType::GetInstance()) ==
      CanvasType::Compatibility::EQUAL) {
    m_translation = Task(Aspen::Box<void>(Aspen::none<void>()));
  } else {
    auto& nativeType = static_cast<const NativeType&>(node.GetType());
    m_translation = Instantiate<ThrowTranslator>(nativeType.GetNativeType())(
      std::make_exception_ptr(
      std::runtime_error("Canvas node not supported.")));
  }
}

void CanvasNodeTranslationVisitor::Visit(const CeilNode& node) {
  m_translation = TranslateFunction<CeilTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const ChainNode& node) {
#if 0 // TODO
  if(node.GetType().GetCompatibility(TaskType::GetInstance()) ==
      CanvasType::Compatibility::EQUAL) {
    vector<TaskFactory> factories;
    auto orderExecutionPublisher = MakeAggregateOrderExecutionPublisher();
    for(auto& child : node.GetChildren()) {
      if(dynamic_cast<const NoneNode*>(&child) == nullptr) {
        auto translation = get<TaskTranslation>(InternalTranslation(child));
        factories.push_back(translation.m_factory);
        orderExecutionPublisher->Add(*translation.m_publisher);
      }
    }
    TaskTranslation taskTranslation;
    taskTranslation.m_publisher = orderExecutionPublisher;
    taskTranslation.m_factory = OrderExecutionPublisherTaskFactory(
      ChainedTaskFactory(factories), orderExecutionPublisher);
    m_translation = taskTranslation;
  } else {
    auto previousReactor = boost::get<std::shared_ptr<BaseReactor>>(
      InternalTranslation(node.GetChildren().front()));
    if(node.GetChildren().size() == 1) {
      m_translation = previousReactor;
      return;
    }
    auto& nativeType = static_cast<const NativeType&>(node.GetType());
    for(auto i = std::size_t{1}; i < node.GetChildren().size() - 1; ++i) {
      auto currentReactor = boost::get<std::shared_ptr<BaseReactor>>(
        InternalTranslation(node.GetChildren()[i]));
      auto chainReactor = Instantiate<ChainTranslator>(
        nativeType.GetNativeType())(previousReactor, currentReactor,
        *m_context);
      previousReactor = chainReactor;
    }
    m_translation = previousReactor;
  }
#endif
}

void CanvasNodeTranslationVisitor::Visit(const CurrencyNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const CurrentDateNode& node) {
#if 0 // TODO
  m_translation = CurrentTimeReactor(
    m_context->GetUserProfile().GetServiceClients().GetTimeClient());
#endif
}

void CanvasNodeTranslationVisitor::Visit(const CurrentDateTimeNode& node) {
  m_translation = CurrentTimeReactor(
    &m_context->GetUserProfile().GetServiceClients().GetTimeClient());
}

void CanvasNodeTranslationVisitor::Visit(const CurrentTimeNode& node) {
#if 0 // TODO
  auto currentTime = m_context->GetUserProfile().GetServiceClients().
    GetTimeClient().GetTime().time_of_day();
  m_translation = MakeConstantReactor(currentTime);
#endif
}

void CanvasNodeTranslationVisitor::Visit(const CustomNode& node) {
  m_translation = InternalTranslation(node.GetChildren().front());
#if 0 // TODO
  if(node.GetType().GetCompatibility(TaskType::GetInstance()) ==
      CanvasType::Compatibility::EQUAL) {
    auto baseTranslation = boost::get<TaskTranslation>(m_translation);
    vector<ReactorProperty> properties;
    for(auto i = node.GetChildren().begin() + 1;
        i != node.GetChildren().end(); ++i) {
      auto reactor = get<std::shared_ptr<BaseReactor>>(InternalTranslation(*i));
      auto property = Instantiate<PropertyBuilder>(reactor->GetType())(
        i->GetName(), reactor);
      properties.push_back(property);
    }
    TaskTranslation taskTranslation;
    taskTranslation.m_publisher = baseTranslation.m_publisher;
    taskTranslation.m_factory = ReactorTaskFactory(
      Ref(m_context->GetReactorMonitor()), properties,
      baseTranslation.m_factory);
    m_translation = taskTranslation;
  }
#endif
}

void CanvasNodeTranslationVisitor::Visit(const DateTimeNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const DecimalNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const DefaultCurrencyNode& node) {
  auto source = InternalTranslation(
    node.GetChildren().front()).Extract<Aspen::Box<Security>>();
  m_translation = Aspen::lift(
    [=] (const Security& security) {
      return m_context->GetUserProfile().GetMarketDatabase().FromCode(
        security.GetMarket()).m_currency;
    }, source);
}

void CanvasNodeTranslationVisitor::Visit(const DestinationNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const DivisionNode& node) {
  m_translation = TranslateFunction<DivisionTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const DurationNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const EqualsNode& node) {
  m_translation = TranslateFunction<EqualsTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(
    const ExecutionReportMonitorNode& node) {
  auto taskTranslation = InternalTranslation(node.GetChildren().front());
  m_translation = Aspen::lift(ExecutionReportToRecordConverter(),
    Aspen::group(Aspen::lift(
    [] (const Order* order) {
      return Aspen::box(PublisherReactor(order->GetPublisher()));
    }, PublisherReactor(*taskTranslation.GetPublisher()))));
}

void CanvasNodeTranslationVisitor::Visit(const FilePathNode& node) {
  m_translation = Aspen::constant(node.GetPath());
}

void CanvasNodeTranslationVisitor::Visit(const FileReaderNode& node) {
  auto path = InternalTranslation(
    node.GetChildren().front()).Extract<Aspen::Box<std::string>>();
  auto& nativeType = static_cast<const NativeType&>(node.GetType());
  m_translation = Instantiate<FileReaderTranslator>(nativeType.GetNativeType())(
    nativeType, Ref(m_context->GetReactorMonitor()),
    Ref(m_context->GetUserProfile()), node.GetErrorPolicy(), path.eval());
}

void CanvasNodeTranslationVisitor::Visit(const FilterNode& node) {
  auto filter = InternalTranslation(node.GetChildren().front());
  auto source = InternalTranslation(node.GetChildren().back());
  m_translation = Instantiate<FilterTranslator>(
    static_cast<const NativeType&>(node.GetType()).GetNativeType())(filter,
    source);
}

void CanvasNodeTranslationVisitor::Visit(const FirstNode& node) {
  auto reactor = InternalTranslation(node.GetChildren().front());
  m_translation = Instantiate<FirstTranslator>(
    static_cast<const NativeType&>(node.GetType()).GetNativeType())(reactor);
}

void CanvasNodeTranslationVisitor::Visit(const FloorNode& node) {
  m_translation = TranslateFunction<FloorTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const FoldNode& node) {
  auto combiner = InternalTranslation(node.GetChildren().front());
  auto leftTrigger = InternalTranslation(*node.FindLeftOperand());
  auto rightTrigger = InternalTranslation(*node.FindRightOperand());
  auto source = InternalTranslation(node.GetChildren().back());
  m_translation = Instantiate<FoldTranslator>(combiner.GetTypeInfo(),
    source.GetTypeInfo(), source.GetTypeInfo())(combiner, leftTrigger,
    rightTrigger, source);
}

void CanvasNodeTranslationVisitor::Visit(const FoldOperandNode& node) {
  m_translation = Instantiate<FoldParameterTranslator>(
    static_cast<const NativeType&>(node.GetType()).GetNativeType())();
}

void CanvasNodeTranslationVisitor::Visit(const GreaterNode& node) {
  m_translation = TranslateFunction<GreaterTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const GreaterOrEqualsNode& node) {
  m_translation = TranslateFunction<GreaterOrEqualsTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const IfNode& node) {
  m_translation = TranslateFunction<IfTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const IntegerNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const IsTerminalNode& node) {
  auto state = InternalTranslation(
    node.GetChildren().front()).Extract<Aspen::Box<Task::State>>();
  m_translation = Aspen::lift(Spire::IsTerminal, state);
}

void CanvasNodeTranslationVisitor::Visit(const LastNode& node) {
  m_translation = Instantiate<LastTranslator>(static_cast<const NativeType&>(
    node.GetType()).GetNativeType())(InternalTranslation(
    node.GetChildren().front()));
}

void CanvasNodeTranslationVisitor::Visit(const LesserNode& node) {
  m_translation = TranslateFunction<LesserTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const LesserOrEqualsNode& node) {
  m_translation = TranslateFunction<LesserOrEqualsTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const LuaScriptNode& node) {
#if 0 // TODO
  auto luaState = luaL_newstate();
  luaL_openlibs(luaState);
  ExportLuaTypes(*luaState);
  auto& nativeType = static_cast<const NativeType&>(node.GetType());
  if(luaL_dofile(luaState, node.GetPath().string().c_str()) != 0) {
    m_translation = Instantiate<ThrowTranslator>(nativeType.GetNativeType())(
      ReactorException(lua_tostring(luaState, -1)));
    lua_close(luaState);
    return;
  }
  vector<unique_ptr<LuaReactorParameter>> parameters;
  for(const auto& child : node.GetChildren()) {
    auto reactor = boost::get<std::shared_ptr<BaseReactor>>(
      InternalTranslation(child));
    auto parameter = Instantiate<LuaParameterTranslator>(reactor->GetType())(
      reactor, child.GetType());
    parameters.emplace_back(std::move(parameter));
  }
  m_translation = Instantiate<LuaScriptTranslator>(nativeType.GetNativeType())(
    node.GetName(), std::move(parameters), *luaState);
#endif
}

void CanvasNodeTranslationVisitor::Visit(const MarketNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const MaxFloorNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const MaxNode& node) {
  m_translation = TranslateFunction<MaxTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const MinNode& node) {
  m_translation = TranslateFunction<MinTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const MoneyNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const MultiplicationNode& node) {
  m_translation = TranslateFunction<MultiplicationTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const NoneNode& node) {
  m_translation = Instantiate<NoneTranslator>(
    static_cast<const NativeType&>(node.GetType()).GetNativeType())();
}

void CanvasNodeTranslationVisitor::Visit(const NotNode& node) {
  m_translation = TranslateFunction<NotTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const OptionalPriceNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const OrderImbalanceQueryNode& node) {
#if 0 // TODO
  auto market = std::static_pointer_cast<Reactor<MarketCode>>(
    boost::get<std::shared_ptr<BaseReactor>>(
    InternalTranslation(node.GetChildren()[0])));
  auto range = std::static_pointer_cast<Reactor<Queries::Range>>(
    boost::get<std::shared_ptr<BaseReactor>>(
    InternalTranslation(node.GetChildren()[1])));
  auto marketDataClient =
    &m_context->GetUserProfile().GetServiceClients().GetMarketDataClient();
  auto reactorMonitor = &m_context->GetReactorMonitor();
  auto orderImbalancePublisher = MakeFunctionReactor(
    [=] (MarketCode market, const Queries::Range& range) {
      MarketWideDataQuery query;
      query.SetIndex(market);
      query.SetRange(range);
      query.SetSnapshotLimit(SnapshotLimit::Unlimited());
      auto queue = std::make_shared<Queue<SequencedOrderImbalance>>();
      marketDataClient->QueryOrderImbalances(query, queue);
      return MakeQueueReactor(queue);
    }, std::move(market), std::move(range));
  auto query = MakeSwitchReactor(orderImbalancePublisher);
  m_translation = MakeFunctionReactor(OrderImbalanceToRecordConverter{}, query);
#endif
}

void CanvasNodeTranslationVisitor::Visit(const OrderStatusNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const OrderTypeNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const OrderWrapperTaskNode& node) {
#if 0 // TODO
  auto orderExecutionPublisher =
    std::make_shared<SequencePublisher<const Order*>>();
  OrderWrapperTaskFactory<VirtualOrderExecutionClient> orderWrapperTaskFactory(
    Ref(m_context->GetUserProfile().GetServiceClients().
    GetOrderExecutionClient()), node.GetOrder());
  vector<ReactorProperty> properties;
  for(const auto& child : node.GetChildren()) {
    auto reactor = get<std::shared_ptr<BaseReactor>>(
      InternalTranslation(child));
    auto property = Instantiate<PropertyBuilder>(reactor->GetType())(
      child.GetName(), reactor);
    properties.push_back(property);
  }
  TaskTranslation taskTranslation;
  taskTranslation.m_publisher = orderExecutionPublisher;
  taskTranslation.m_factory = ReactorTaskFactory(
    Ref(m_context->GetReactorMonitor()), std::move(properties),
    orderWrapperTaskFactory);
  orderExecutionPublisher->Push(&node.GetOrder());
  m_translation = taskTranslation;
#endif
}

void CanvasNodeTranslationVisitor::Visit(const QueryNode& node) {
#if 0 // TODO
  auto& recordNode = node.GetChildren().front();
  auto recordReactor = std::static_pointer_cast<Reactor<Record>>(
    boost::get<std::shared_ptr<BaseReactor>>(InternalTranslation(recordNode)));
  auto& recordType = static_cast<const RecordType&>(recordNode.GetType());
  auto fieldIterator = find_if(recordType.GetFields().begin(),
    recordType.GetFields().end(),
    [&] (const RecordType::Field& field) {
      return field.m_name == node.GetField();
    });
  m_translation = Instantiate<QueryTranslator>(
    static_cast<const NativeType&>(node.GetType()).GetNativeType())(
    recordReactor,
    std::distance(recordType.GetFields().begin(), fieldIterator));
#endif
}

void CanvasNodeTranslationVisitor::Visit(const RangeNode& node) {
  auto lower = InternalTranslation(
    node.GetChildren().front()).Extract<Aspen::Box<Quantity>>();
  auto upper = InternalTranslation(
    node.GetChildren().back()).Extract<Aspen::Box<Quantity>>();
  m_translation = Aspen::range(lower, upper);
}

void CanvasNodeTranslationVisitor::Visit(const ReferenceNode& node) {
#if 0 // TODO
  auto& anchor = *FindAnchor(node);
  auto& referent = *node.FindReferent();
  m_translation = InternalTranslation(referent);
  if(referent.GetType().GetCompatibility(TaskType::GetInstance()) !=
      CanvasType::Compatibility::EQUAL) {
    auto parent = referent.GetParent();
    if(parent.is_initialized() &&
        dynamic_cast<const SpawnNode*>(&*parent) != nullptr) {
      if(&referent == &parent->GetChildren().front()) {
        m_translation = Instantiate<FirstTranslator>(
          static_cast<const NativeType&>(node.GetType()).GetNativeType())(
          boost::get<std::shared_ptr<BaseReactor>>(m_translation));
      }
    }
  }
#endif
}

void CanvasNodeTranslationVisitor::Visit(const RoundNode& node) {
  m_translation = TranslateFunction<RoundTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const SecurityNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const SideNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const SingleOrderTaskNode& node) {
#if 0 // TODO
  auto orderExecutionPublisher =
    std::make_shared<SequencePublisher<const Order*>>();
  SingleOrderTaskFactory<VirtualOrderExecutionClient> singleOrderTaskFactory(
    Ref(m_context->GetUserProfile().GetServiceClients().
    GetOrderExecutionClient()), orderExecutionPublisher,
    m_context->GetExecutingAccount());
  for(const auto& field : node.GetFields()) {
    if(field.m_type->GetCompatibility(IntegerType::GetInstance()) ==
        CanvasType::Compatibility::EQUAL) {
      singleOrderTaskFactory.AddField<Quantity>(field.m_name, field.m_key);
    } else if(field.m_type->GetCompatibility(
        Spire::DecimalType::GetInstance()) ==
        CanvasType::Compatibility::EQUAL) {
      singleOrderTaskFactory.AddField<double>(field.m_name, field.m_key);
    } else if(field.m_type->GetCompatibility(Spire::MoneyType::GetInstance()) ==
        CanvasType::Compatibility::EQUAL) {
      singleOrderTaskFactory.AddField<Money>(field.m_name, field.m_key);
    } else if(field.m_type->GetCompatibility(Spire::TextType::GetInstance()) ==
        CanvasType::Compatibility::EQUAL) {
      singleOrderTaskFactory.AddField<string>(field.m_name, field.m_key);
    }
  }
  SingleRedisplayableOrderTaskFactory singleRedisplayableOrderTaskFactory(
    singleOrderTaskFactory);
  vector<ReactorProperty> properties;
  std::shared_ptr<Reactor<Quantity>> quantityReactor;
  for(const auto& child : node.GetChildren()) {
    auto reactor = boost::get<std::shared_ptr<BaseReactor>>(
      InternalTranslation(child));
    if(child.GetName() == BaseSingleOrderTaskFactory::QUANTITY) {
      quantityReactor = std::static_pointer_cast<Reactor<Quantity>>(reactor);
    }
    string propertyName;
    if(child.GetName() == SingleOrderTaskNode::VOLUME_PROPERTY) {
      propertyName = SingleRedisplayableOrderTaskFactory::DISPLAY;
      reactor = MakeFunctionReactor(MinOperation(),
        std::static_pointer_cast<Reactor<Quantity>>(reactor),
        std::static_pointer_cast<Reactor<Quantity>>(quantityReactor));
    } else {
      propertyName = child.GetName();
    }
    if(child.GetName() != SingleOrderTaskNode::DISPLAY_PROPERTY) {
      auto property = Instantiate<PropertyBuilder>(reactor->GetType())(
        propertyName, reactor);
      properties.push_back(property);
    }
  }
  TaskTranslation taskTranslation;
  taskTranslation.m_publisher = orderExecutionPublisher;
  taskTranslation.m_factory = OrderExecutionPublisherTaskFactory(
    ReactorTaskFactory(Ref(m_context->GetReactorMonitor()),
    std::move(properties), singleRedisplayableOrderTaskFactory),
    orderExecutionPublisher);
  m_translation = taskTranslation;
#endif
}

void CanvasNodeTranslationVisitor::Visit(const SpawnNode& node) {
#if 0 // TODO
  auto trigger = boost::get<std::shared_ptr<BaseReactor>>(
    InternalTranslation(node.GetChildren().front()));
  CanvasNodeTaskFactory taskFactory(Ref(*m_context),
    Ref(node.GetChildren().back()));
  TaskTranslation taskTranslation;
  taskTranslation.m_factory = SpawnTaskFactory(
    Ref(m_context->GetReactorMonitor()), trigger, taskFactory);
  taskTranslation.m_publisher = taskFactory.GetOrderExecutionPublisher();
  m_translation = taskTranslation;
#endif
}

void CanvasNodeTranslationVisitor::Visit(const SubtractionNode& node) {
  m_translation = TranslateFunction<SubtractionTranslator>(node);
}

void CanvasNodeTranslationVisitor::Visit(const TaskStateMonitorNode& node) {
#if 0
  auto translation = boost::get<TaskTranslation>(
    InternalTranslation(node.GetChildren().front()));
  auto task =
    translation.m_factory.DynamicCast<IndirectTaskFactory>()->GetTask();
  auto publisher = MakePublisherReactor(task->GetPublisher());
  m_translation = publisher;
#endif
}

void CanvasNodeTranslationVisitor::Visit(const TaskStateNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const TextNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const TimeAndSaleQueryNode& node) {
#if 0
  auto security = std::static_pointer_cast<Reactor<Security>>(
    boost::get<std::shared_ptr<BaseReactor>>(
    InternalTranslation(node.GetChildren()[0])));
  auto range = std::static_pointer_cast<Reactor<Queries::Range>>(
    boost::get<std::shared_ptr<BaseReactor>>(
    InternalTranslation(node.GetChildren()[1])));
  auto marketDataClient = &m_context->GetUserProfile().GetServiceClients().
    GetMarketDataClient();
  auto reactorMonitor = &m_context->GetReactorMonitor();
  auto timeAndSalePublisher = MakeFunctionReactor(
    [=] (const Security& security, const Queries::Range& range) {
      SecurityMarketDataQuery query;
      query.SetIndex(security);
      query.SetRange(range);
      query.SetSnapshotLimit(SnapshotLimit::Unlimited());
      auto queue = std::make_shared<Queue<SequencedTimeAndSale>>();
      marketDataClient->QueryTimeAndSales(query, queue);
      return MakeQueueReactor(queue);
    }, std::move(security), std::move(range));
  auto query = MakeSwitchReactor(timeAndSalePublisher);
  m_translation = MakeFunctionReactor(TimeAndSaleToRecordConverter{}, query);
#endif
}

void CanvasNodeTranslationVisitor::Visit(const TimeInForceNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const TimeNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const TimeRangeNode& node) {
  m_translation = Aspen::constant(node.GetValue());
}

void CanvasNodeTranslationVisitor::Visit(const TimeRangeParameterNode& node) {
  m_translation = Aspen::constant(node.GetTimeRangeQuery(
    m_context->GetUserProfile().GetServiceClients().GetTimeClient()));
}

void CanvasNodeTranslationVisitor::Visit(const TimerNode& node) {
#if 0
  auto period = std::static_pointer_cast<Reactor<time_duration>>(
    boost::get<std::shared_ptr<BaseReactor>>(InternalTranslation(
    node.GetChildren().front())));
  auto timerThreadPool = &m_context->GetUserProfile().GetTimerThreadPool();
  auto timerFactory = [=] (time_duration interval) {
    return make_unique<LiveTimer>(interval, Ref(*timerThreadPool));
  };
  auto reactor = MakeTimerReactor<Quantity>(timerFactory, period);
  m_translation = reactor;
#endif
}

void CanvasNodeTranslationVisitor::Visit(const UnequalNode& node) {
#if 0
  m_translation = TranslateFunction<UnequalTranslator>(node);
#endif
}

void CanvasNodeTranslationVisitor::Visit(const UntilNode& node) {
#if 0
  CanvasNodeTaskFactory taskFactory(Ref(*m_context),
    Ref(node.GetChildren().back()));
  auto condition = std::static_pointer_cast<Reactor<bool>>(
    boost::get<std::shared_ptr<BaseReactor>>(
    InternalTranslation(node.GetChildren().front())));
  m_context->GetReactorMonitor().Add(condition);
  TaskTranslation taskTranslation;
  taskTranslation.m_factory = UntilTaskFactory(
    Ref(m_context->GetReactorMonitor()), condition, taskFactory);
  taskTranslation.m_publisher = taskFactory.GetOrderExecutionPublisher();
  m_translation = taskTranslation;
#endif
}

void CanvasNodeTranslationVisitor::Visit(const WhenNode& node) {
  auto condition = InternalTranslation(
    node.GetChildren().front()).Extract<Aspen::Box<bool>>();
  auto series = InternalTranslation(node.GetChildren().back());
  m_translation = Instantiate<WhenTranslator>(series.GetTypeInfo())(condition,
    series);
}

Translation CanvasNodeTranslationVisitor::InternalTranslation(
    const CanvasNode& node) {
  auto existingTranslation = m_context->FindTranslation(node);
  auto isRootTask = false;
  if(existingTranslation.is_initialized()) {
    m_translation = *existingTranslation;
    m_context->Add(Ref(node), *m_translation);
  } else if(dynamic_cast<const ReferenceNode*>(&node) == nullptr &&
      node.GetType().GetCompatibility(TaskType::GetInstance()) ==
      CanvasType::Compatibility::EQUAL) {

    // TODO.
#if 0
    auto publisher = MakeAggregateOrderExecutionPublisher();
    auto proxyReactor = Aspen::Shared(Aspen::proxy<Task::State>());
    auto selfTranslation = Translation(proxyReactor, publisher);
    m_context->Add(Ref(node), selfTranslation);
    node.Apply(*this);
    auto subTranslation = std::move(*m_translation);
    m_translation = std::nullopt;
    proxyReactor->set_reactor(
      subTranslation.Extract<Aspen::Box<Task::State>>());
    publisher->Add(subTranslation.GetPublisher());
    m_translation = subTranslation;
#endif
  } else {
    node.Apply(*this);
    m_context->Add(Ref(node), *m_translation);
  }
  return *m_translation;
}

template<typename Translator>
Translation CanvasNodeTranslationVisitor::TranslateFunction(
    const CanvasNode& node) {
  auto arguments = std::vector<Translation>();
  for(const auto& child : node.GetChildren()) {
    arguments.push_back(InternalTranslation(child));
  }
  auto& result = static_cast<const NativeType&>(node.GetType()).GetNativeType();
  return FunctionTranslator<Translator, ParameterCount<Translator>::value>()(
    arguments, result, *m_context);
}
