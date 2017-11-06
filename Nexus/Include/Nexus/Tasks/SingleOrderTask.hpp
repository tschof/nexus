#ifndef NEXUS_SINGLE_ORDER_TASK_HPP
#define NEXUS_SINGLE_ORDER_TASK_HPP
#include <Beam/Queues/QueueWriter.hpp>
#include <Beam/Queues/RoutineTaskQueue.hpp>
#include <Beam/Tasks/BasicTask.hpp>
#include <Beam/Utilities/AssertionException.hpp>
#include "Nexus/OrderExecutionService/ExecutionReport.hpp"
#include "Nexus/OrderExecutionService/Order.hpp"
#include "Nexus/OrderExecutionService/OrderFields.hpp"
#include "Nexus/Tasks/Tasks.hpp"

namespace Nexus {
namespace Tasks {

  /*! \class SingleOrderTask
      \brief Executes and manages a single Order.
      \tparam OrderExecutionClientType The OrderExecutionClient used to execute
              and cancel the Order.
   */
  template<typename OrderExecutionClientType>
  class SingleOrderTask : public Beam::Tasks::BasicTask {
      struct Guard {};
    public:

      //! The OrderExecutionClient used to execute and cancel the Order.
      using OrderExecutionClient = OrderExecutionClientType;

      //! Constructs a SingleOrderTask.
      /*!
        \param orderExecutionClient The OrderExecutionClient to use.
        \param orderExecutionPublisher Used to report Order executions.
        \param fields The Order's execution fields.
      */
      SingleOrderTask(Beam::RefType<OrderExecutionClient> orderExecutionClient,
        std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
        orderExecutionPublisher,
        const OrderExecutionService::OrderFields& fields);

      SingleOrderTask(Beam::RefType<OrderExecutionClient> orderExecutionClient,
        std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
        orderExecutionPublisher,
        const OrderExecutionService::OrderFields& fields, Quantity fillSize,
        Guard);

    protected:
      virtual void OnExecute() override final;

      virtual void OnCancel() override final;

    private:
      friend class SingleOrderTaskFactory<OrderExecutionClientType>;
      OrderExecutionClient* m_orderExecutionClient;
      std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
        m_orderExecutionPublisher;
      OrderExecutionService::OrderFields m_fields;
      const OrderExecutionService::Order* m_order;
      Quantity m_fillSize;
      bool m_cancellable;
      bool m_pendingCancel;
      int m_state;
      Beam::RoutineTaskQueue m_tasks;

      void OnExecutionReport(
        const OrderExecutionService::ExecutionReport& report);
      void S0();
      void S1(const std::string& message);
      void S2();
      void S3(OrderStatus state, const std::string& message);
      void S4();
  };

  template<typename T>
  class BaseSingleOrderTaskFactoryExports {
    public:

      //! The Order's Security.
      static const std::string SECURITY;

      //! The OrderType.
      static const std::string ORDER_TYPE;

      //! The Side of the Order.
      static const std::string SIDE;

      //! The destination.
      static const std::string DESTINATION;

      //! The quantity.
      static const std::string QUANTITY;

      //! The price.
      static const std::string PRICE;

      //! The currency the price the order in.
      static const std::string CURRENCY;

      //! The Order's TimeInForce specification.
      static const std::string TIME_IN_FORCE;
  };

  class BaseSingleOrderTaskFactory :
    public BaseSingleOrderTaskFactoryExports<void> {};

  /*! \class SingleOrderTaskFactory
      \brief Implements the OrderTaskFactory for the SingleOrderTask.
      \tparam OrderExecutionClientType The OrderExecutionClient used to execute
              and cancel the Order.
   */
  template<typename OrderExecutionClientType>
  class SingleOrderTaskFactory : public Beam::Tasks::BasicTaskFactory<
      SingleOrderTaskFactory<OrderExecutionClientType>>,
      public BaseSingleOrderTaskFactory {
    public:

      //! The OrderExecutionClient used to execute and cancel the Order.
      using OrderExecutionClient = OrderExecutionClientType;

      //! Constructs a SingleOrderTaskFactory.
      /*!
        \param orderExecutionClient The OrderExecutionClient to use.
        \param orderExecutionPublisher Used to report Order executions.
        \param account The account to assign the Order to.
      */
      SingleOrderTaskFactory(
        Beam::RefType<OrderExecutionClient> orderExecutionClient,
        std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
        orderExecutionPublisher,
        const Beam::ServiceLocator::DirectoryEntry& account);

      //! Constructs a SingleOrderTaskFactory.
      /*!
        \param orderExecutionClient The OrderExecutionClient to use.
        \param orderExecutionPublisher Used to report Order executions.
        \param fields The initial set of OrderFields.
      */
      SingleOrderTaskFactory(
        Beam::RefType<OrderExecutionClient> orderExecutionClient,
        std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
        orderExecutionPublisher,
        const OrderExecutionService::OrderFields& fields);

      //! Sets the properties of this factory based on an OrderFields object.
      /*!
        \param fields The OrderFields specifying the properties to set.
      */
      void Set(const OrderExecutionService::OrderFields& fields);

      //! Adds a field used to submit an Order.
      /*!
        \param name The name of the field.
        \param key The field's key.
      */
      template<typename T>
      void AddField(const std::string& name, int key);

      virtual std::shared_ptr<Beam::Tasks::Task> Create() override final;

      virtual void PrepareContinuation(
        const Beam::Tasks::Task& task) override final;

      using VirtualTaskFactory::Set;
    private:
      struct AdditionalFieldEntry {
        int m_typeIndex;
        std::string m_name;
        int m_key;
      };
      OrderExecutionClient* m_orderExecutionClient;
      std::vector<AdditionalFieldEntry> m_additionalFields;
      std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
        m_orderExecutionPublisher;
      Beam::ServiceLocator::DirectoryEntry m_account;
      Quantity m_fillSize;
  };

  template<typename OrderExecutionClientType>
  SingleOrderTask<OrderExecutionClientType>::SingleOrderTask(
      Beam::RefType<OrderExecutionClient> orderExecutionClient,
      std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
      orderExecutionPublisher, const OrderExecutionService::OrderFields& fields)
      : m_orderExecutionClient{orderExecutionClient.Get()},
        m_orderExecutionPublisher{std::move(orderExecutionPublisher)},
        m_fields{fields},
        m_fillSize{0} {}

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::OnExecute() {
    m_tasks.Push(
      [=] {
        S0();
      });
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::OnCancel() {
    m_tasks.Push(
      [=] {
        m_pendingCancel = true;
        S4();
      });
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::OnExecutionReport(
      const OrderExecutionService::ExecutionReport& report) {
    if(report.m_status == OrderStatus::NEW) {
      m_cancellable = true;
    }
    m_fillSize += report.m_lastQuantity;
    if(m_state == 0) {
      if(report.m_status == OrderStatus::REJECTED) {
        return S1(report.m_text);
      } else if(report.m_status == OrderStatus::NEW) {
        return S2();
      }
    } else if(m_state == 2) {
      if(Nexus::IsTerminal(report.m_status)) {
        return S3(report.m_status, report.m_text);
      }
    } else if(m_state == 4) {
      if(report.m_status == OrderStatus::NEW) {
        return S4();
      } else if(Nexus::IsTerminal(report.m_status)) {
        return S3(report.m_status, report.m_text);
      }
    }
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::S0() {
    m_state = 0;
    m_cancellable = false;
    m_pendingCancel = false;
    SetActive();
    if(m_fields.m_quantity <= m_fillSize) {
      return S3(OrderStatus::FILLED, "");
    }
    OrderExecutionService::OrderFields fields = m_fields;
    fields.m_quantity -= m_fillSize;
    try {
      m_order = &m_orderExecutionClient->Submit(fields);
    } catch(const std::exception& e) {
      return S1(e.what());
    }
    m_orderExecutionPublisher->Push(m_order);
    m_order->GetPublisher().Monitor(
      m_tasks.GetSlot<OrderExecutionService::ExecutionReport>(
      std::bind(&SingleOrderTask::OnExecutionReport, this,
      std::placeholders::_1)));
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::S1(
      const std::string& message) {
    m_state = 1;
    SetTerminal(State::FAILED, message);
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::S2() {
    m_state = 2;
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::S3(OrderStatus state,
      const std::string& message) {
    m_state = 3;
    if(state == OrderStatus::FILLED) {
      SetTerminal(State::COMPLETE, message);
    } else if(m_pendingCancel && state == OrderStatus::CANCELED) {
      SetTerminal(State::CANCELED, message);
    } else {
      SetTerminal(State::EXPIRED, message);
    }
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTask<OrderExecutionClientType>::S4() {
    m_state = 4;

    // TODO: Proper cancel to avoid Terminate precondition.
    if(m_cancellable) {
      m_orderExecutionClient->Cancel(*m_order);
    }
  }

  template<typename OrderExecutionClientType>
  SingleOrderTask<OrderExecutionClientType>::SingleOrderTask(
      Beam::RefType<OrderExecutionClient> orderExecutionClient,
      std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
      orderExecutionPublisher, const OrderExecutionService::OrderFields& fields,
      Quantity fillSize, Guard)
      : m_orderExecutionClient{orderExecutionClient.Get()},
        m_orderExecutionPublisher{std::move(orderExecutionPublisher)},
        m_fields{fields},
        m_fillSize{fillSize} {}

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::SECURITY = "security";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::ORDER_TYPE = "type";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::SIDE = "side";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::DESTINATION =
    "destination";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::QUANTITY = "quantity";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::PRICE = "price";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::CURRENCY = "currency";

  template<typename T>
  const std::string BaseSingleOrderTaskFactoryExports<T>::TIME_IN_FORCE =
    "time_in_force";

  template<typename OrderExecutionClientType>
  SingleOrderTaskFactory<OrderExecutionClientType>::SingleOrderTaskFactory(
      Beam::RefType<OrderExecutionClient> orderExecutionClient,
      std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
      orderExecutionPublisher,
      const Beam::ServiceLocator::DirectoryEntry& account)
      : m_orderExecutionClient{orderExecutionClient.Get()},
        m_orderExecutionPublisher{std::move(orderExecutionPublisher)},
        m_account{account},
        m_fillSize{0} {
    this->template DefineProperty<Security>(SECURITY, Security());
    this->template DefineProperty<OrderType>(ORDER_TYPE, OrderType::NONE);
    this->template DefineProperty<Side>(SIDE, Side::NONE);
    this->template DefineProperty<std::string>(DESTINATION, "");
    this->template DefineProperty<Quantity>(QUANTITY, 0);
    this->template DefineProperty<Money>(PRICE, Money::ZERO);
    this->template DefineProperty<CurrencyId>(CURRENCY, CurrencyId::NONE());
    this->template DefineProperty<TimeInForce>(TIME_IN_FORCE,
      TimeInForce(TimeInForce::Type::DAY));
  }

  template<typename OrderExecutionClientType>
  SingleOrderTaskFactory<OrderExecutionClientType>::SingleOrderTaskFactory(
      Beam::RefType<OrderExecutionClient> orderExecutionClient,
      std::shared_ptr<Beam::QueueWriter<const OrderExecutionService::Order*>>
      orderExecutionPublisher, const OrderExecutionService::OrderFields& fields)
      : SingleOrderTaskFactory<OrderExecutionClientType>{
          Beam::Ref(orderExecutionClient), std::move(orderExecutionPublisher),
          fields.m_account} {
    Set(fields);
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTaskFactory<OrderExecutionClientType>::Set(
      const OrderExecutionService::OrderFields& fields) {
    m_account = fields.m_account;
    Set(SECURITY, fields.m_security);
    Set(ORDER_TYPE, fields.m_type);
    Set(SIDE, fields.m_side);
    Set(DESTINATION, fields.m_destination);
    Set(QUANTITY, fields.m_quantity);
    Set(PRICE, fields.m_price);
    Set(CURRENCY, fields.m_currency);
    Set(TIME_IN_FORCE, fields.m_timeInForce);
  }

  template<typename OrderExecutionClientType>
  template<typename T>
  void SingleOrderTaskFactory<OrderExecutionClientType>::AddField(
      const std::string& name, int key) {
    static_assert(std::is_same<T, int>::value ||
      std::is_same<T, double>::value || std::is_same<T, Quantity>::value ||
      std::is_same<T, Money>::value || std::is_same<T, std::string>::value,
      "Invalid field type specified.");
    AdditionalFieldEntry entry;
    if(std::is_same<T, int>::value) {
      entry.m_typeIndex = Tag::INT_INDEX;
    } else if(std::is_same<T, double>::value) {
      entry.m_typeIndex = Tag::DOUBLE_INDEX;
    } else if(std::is_same<T, Quantity>::value) {
      entry.m_typeIndex = Tag::QUANTITY_INDEX;
    } else if(std::is_same<T, Money>::value) {
      entry.m_typeIndex = Tag::MONEY_INDEX;
    } else if(std::is_same<T, std::string>::value) {
      entry.m_typeIndex = Tag::STRING_INDEX;
    }
    entry.m_name = name;
    entry.m_key = key;
    this->template DefineProperty<T>(entry.m_name, T());
    m_additionalFields.push_back(entry);
  }

  template<typename OrderExecutionClientType>
  std::shared_ptr<Beam::Tasks::Task> SingleOrderTaskFactory<
      OrderExecutionClientType>::Create() {
    OrderExecutionService::OrderFields fields;
    fields.m_account = m_account;
    fields.m_security = this->template Get<Security>(SECURITY);
    fields.m_type = this->template Get<OrderType>(ORDER_TYPE);
    fields.m_side = this->template Get<Side>(SIDE);
    fields.m_destination = this->template Get<std::string>(DESTINATION);
    fields.m_quantity = this->template Get<Quantity>(QUANTITY);
    fields.m_price = this->template Get<Money>(PRICE);
    fields.m_currency = this->template Get<CurrencyId>(CURRENCY);
    fields.m_timeInForce = this->template Get<TimeInForce>(TIME_IN_FORCE);
    for(const AdditionalFieldEntry& entry : m_additionalFields) {
      Tag::Type value;
      boost::any& property = FindProperty(entry.m_name);
      switch(entry.m_typeIndex) {
        case Tag::INT_INDEX:
          value = boost::any_cast<int>(property);
          break;
        case Tag::DOUBLE_INDEX:
          value = boost::any_cast<double>(property);
          break;
        case Tag::QUANTITY_INDEX:
          value = boost::any_cast<Quantity>(property);
          break;
        case Tag::MONEY_INDEX:
          value = boost::any_cast<Money>(property);
          break;
        case Tag::STRING_INDEX:
          value = boost::any_cast<std::string>(property);
          break;
        default:
          BEAM_ASSERT_MESSAGE(false, "Invalid Tag specified.");
      }
      fields.m_additionalFields.push_back(Tag(entry.m_key, value));
    }
    auto fillSize = m_fillSize;
    m_fillSize = 0;
    return std::make_shared<SingleOrderTask<OrderExecutionClient>>(
      Beam::Ref(*m_orderExecutionClient), m_orderExecutionPublisher, fields,
      fillSize, typename SingleOrderTask<OrderExecutionClient>::Guard{});
  }

  template<typename OrderExecutionClientType>
  void SingleOrderTaskFactory<OrderExecutionClientType>::PrepareContinuation(
      const Beam::Tasks::Task& task) {
    auto& singleOrderTask =
      static_cast<const SingleOrderTask<OrderExecutionClient>&>(task);
    Set(SECURITY, singleOrderTask.m_fields.m_security);
    Set(ORDER_TYPE, singleOrderTask.m_fields.m_type);
    Set(SIDE, singleOrderTask.m_fields.m_side);
    Set(DESTINATION, singleOrderTask.m_fields.m_destination);
    Set(QUANTITY, singleOrderTask.m_fields.m_quantity);
    Set(PRICE, singleOrderTask.m_fields.m_price);
    Set(CURRENCY, singleOrderTask.m_fields.m_currency);
    Set(TIME_IN_FORCE, singleOrderTask.m_fields.m_timeInForce);
    m_fillSize = singleOrderTask.m_fillSize;
    for(const AdditionalFieldEntry& field : m_additionalFields) {
      for(const Tag& tag : singleOrderTask.m_fields.m_additionalFields) {
        if(tag.GetKey() == field.m_key) {
          switch(tag.GetValue().which()) {
            case Tag::INT_INDEX:
              Set(field.m_name, boost::get<int>(tag.GetValue()));
              break;
            case Tag::DOUBLE_INDEX:
              Set(field.m_name, boost::get<double>(tag.GetValue()));
              break;
            case Tag::QUANTITY_INDEX:
              Set(field.m_name, boost::get<Quantity>(tag.GetValue()));
              break;
            case Tag::MONEY_INDEX:
              Set(field.m_name, boost::get<Money>(tag.GetValue()));
              break;
            case Tag::STRING_INDEX:
              Set(field.m_name, boost::get<std::string>(tag.GetValue()));
              break;
            default:
              BEAM_ASSERT_MESSAGE(false, "Invalid Tag specified.")
          }
          break;
        }
      }
    }
  }
}
}

#endif
