#include "spire/book_view_ui_tester/random_book_view_model.hpp"
#include <Beam/Threading/LiveTimer.hpp>

using namespace Beam;
using namespace Beam::Threading;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::signals2;
using namespace Nexus;
using namespace Spire;

RandomBookViewModel::RandomBookViewModel(Security security,
    time_duration load_time, TimerThreadPool& timer_thread_pool)
    : m_security(std::move(security)),
      m_load_time(load_time),
      m_timer_thread_pool(&timer_thread_pool),
      m_is_loaded(false),
      m_bbo(Quote(100 * Money::ONE, 100, Side::BID),
        Quote(100 * Money::ONE + Money::CENT, 100, Side::ASK),
        second_clock::universal_time()),
      m_random_engine(std::random_device()()),
      m_loading_flag(std::make_shared<CallOnce<Mutex>>()),
      m_quote_count(0) {
  connect(&m_timer, &QTimer::timeout, [=] { on_timeout(); });
  set_period(seconds(1));
}

RandomBookViewModel::~RandomBookViewModel() {
  m_loading_flag->Call(
    [&] {
      m_is_loaded = true;
    });
}

time_duration RandomBookViewModel::get_period() const {
  return m_period;
}

void RandomBookViewModel::set_period(time_duration period) {
  m_period = period;
  m_timer.stop();
  if(m_period != pos_infin) {
    m_timer.start(m_period.total_milliseconds());
  }
}

void RandomBookViewModel::publish(const BookQuote& quote) {
  auto& quotes = Pick(quote.m_quote.m_side, m_asks, m_bids);
  auto direction = GetDirection(quote.m_quote.m_side);
  auto it = std::find(quotes.begin(), quotes.end(), quote);
  if(it == quotes.end()) {
    if(quote.m_quote.m_size != 0) {
      if(direction * quotes.front().m_quote.m_price > direction *
          quote.m_quote.m_price) {
        quotes.push_back(quote);
      } else {
        quotes.insert(quotes.begin(), quote);
      }
    } else {
      auto index = std::lower_bound(quotes.begin(), quotes.end(), quote,
        BookQuoteListingComparator);
      quotes.insert(index, quote);
    }
  }
  m_book_quote_signal(quote);
}

const Security& RandomBookViewModel::get_security() const {
  return m_security;
}

const BboQuote& RandomBookViewModel::get_bbo() const {
  return m_bbo;
}

const std::vector<BookQuote>& RandomBookViewModel::get_asks() const {
  return m_asks;
}

const std::vector<BookQuote>& RandomBookViewModel::get_bids() const {
  return m_bids;
}

optional<Money> RandomBookViewModel::get_high() const {
  return m_high;
}

optional<Money> RandomBookViewModel::get_low() const {
  return m_low;
}

optional<Money> RandomBookViewModel::get_open() const {
  return m_open;
}

optional<Money> RandomBookViewModel::get_close() const {
  return m_close;
}

Quantity RandomBookViewModel::get_volume() const {
  return m_volume;
}

QtPromise<void> RandomBookViewModel::load() {
  auto load_time = m_load_time;
  auto timer_thread_pool = m_timer_thread_pool;
  auto loading_flag = m_loading_flag;
  return make_qt_promise([=] {
    LiveTimer load_timer(load_time, Ref(*timer_thread_pool));
    load_timer.Start();
    load_timer.Wait();
    loading_flag->Call(
      [&] {
        for(auto i = 0; i < 1000; ++i) {
          update();
        }
        Quote bid_quote(Money(), 100, Side::BID);
        Quote ask_quote(Money(), 100, Side::ASK);
        for(auto i = 0; i < 100; ++i) {
          bid_quote.m_price = (100 * Money::ONE) - (i * Money::CENT);
          auto market = get_random_market();
          m_bids.push_back(BookQuote(market.GetData(), true, market, bid_quote,
            second_clock::universal_time()));
          market = get_random_market();
          ask_quote.m_price = (100 * Money::ONE) + (i * Money::CENT);
          m_asks.push_back(BookQuote(market.GetData(), true, market, ask_quote,
            second_clock::universal_time()));
        }
        m_is_loaded = true;
      });
  });
}

connection RandomBookViewModel::connect_bbo_slot(
    const BboSignal::slot_type& slot) const {
  return m_bbo_signal.connect(slot);
}

connection RandomBookViewModel::connect_book_quote_slot(
    const BookQuoteSignal::slot_type& slot) const {
  return m_book_quote_signal.connect(slot);
}

connection RandomBookViewModel::connect_high_slot(
    const PriceSignal::slot_type& slot) const {
  return m_high_signal.connect(slot);
}

connection RandomBookViewModel::connect_low_slot(
    const PriceSignal::slot_type& slot) const {
  return m_low_signal.connect(slot);
}

connection RandomBookViewModel::connect_open_slot(
    const PriceSignal::slot_type& slot) const {
  return m_open_signal.connect(slot);
}

connection RandomBookViewModel::connect_close_slot(
    const PriceSignal::slot_type& slot) const {
  return m_close_signal.connect(slot);
}

connection RandomBookViewModel::connect_volume_slot(
    const QuantitySignal::slot_type& slot) const {
  return m_volume_signal.connect(slot);
}

MarketCode RandomBookViewModel::get_random_market() {
  auto markets = GetDefaultMarketDatabase().GetEntries();
  return markets[m_random_engine() % markets.size()].m_code;
}

void RandomBookViewModel::update() {
  update_bbo();
  update_book_quote();
  update_time_and_sales();
}

void RandomBookViewModel::update_bbo() {
  auto random_num = m_random_engine() % 3;
  if(random_num == 0) {
    return;
  }
  auto& bid_price = m_bbo.m_bid.m_price;
  auto& ask_price = m_bbo.m_ask.m_price;
  if(random_num == 1) {
    if(bid_price > Money::CENT) {
      bid_price -= Money::CENT;
      ask_price -= Money::CENT;
      m_bbo_signal(m_bbo);
    }
  } else if(random_num == 2) {
    bid_price += Money::CENT;
    ask_price += Money::CENT;
    m_bbo_signal(m_bbo);
  }
}

void RandomBookViewModel::update_book_quote() {
  if(m_bids.size() == 0 || m_asks.size() == 0) {
    return;
  }
  auto side = [&] {
    if(m_random_engine() % 2 == 0) {
      return Side::BID;
    }
    return Side::ASK;
  }();
  auto& get_random_book_quote = [&] {
    if(side == Side::BID) {
      return m_bids[m_random_engine() % m_bids.size()];
    }
    return m_asks[m_random_engine() % m_asks.size()];
  };
  BookQuote book_quote;
  auto random_num = m_random_engine() % 100;
  if(random_num < 10) {
    book_quote = get_random_book_quote();
    book_quote.m_quote.m_size = (m_random_engine() % 200) + 1;
  } else if(random_num >= 10 && random_num < 45) {
    auto market = get_random_market();
    Quote quote((m_random_engine() % 200) * Money::ONE, 100, side);
    book_quote = BookQuote(market.GetData(), true, market, quote,
      second_clock::universal_time());
  } else {
    book_quote = get_random_book_quote();
    book_quote.m_quote.m_size = 0;
  }
  publish(book_quote);
}

void RandomBookViewModel::update_time_and_sales() {
  auto random_num = m_random_engine() % 3;
  if(random_num == 0) {
    return;
  }
  Money quote;
  if(random_num == 1) {
    quote = m_bbo.m_bid.m_price;
  } else if(random_num == 2) {
    quote = m_bbo.m_ask.m_price;
  }
  if(m_quote_count == 0) {
    ++m_quote_count;
    m_close = quote;
    m_close_signal(quote);
  } else if(m_quote_count == 1) {
    ++m_quote_count;
    m_open = quote;
    m_high = quote;
    m_low = quote;
    m_open_signal(m_open);
    m_high_signal(m_high);
    m_low_signal(m_low);
  }
  if(quote < m_low) {
    m_low = quote;
    m_low_signal(m_low);
  } else if(quote > m_high) {
    m_high = quote;
    m_high_signal(m_high);
  }
  m_volume += 100;
  m_volume_signal(m_volume);
}

void RandomBookViewModel::on_timeout() {
  if(!m_is_loaded) {
    return;
  }
  update();
  m_timer.start(m_period.total_milliseconds());
}