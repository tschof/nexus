#ifndef SPIRE_LUAINTEROP_HPP
#define SPIRE_LUAINTEROP_HPP
#include "Nexus/Definitions/Currency.hpp"
#include "Nexus/Definitions/Money.hpp"
#include "Nexus/Definitions/OrderStatus.hpp"
#include "Nexus/Definitions/OrderType.hpp"
#include "Nexus/Definitions/Security.hpp"
#include "Nexus/Definitions/Side.hpp"
#include "Nexus/Definitions/TimeInForce.hpp"
#include "Spire/Canvas/Records/Record.hpp"
#include "Spire/Canvas/LuaNodes/LuaReactor.hpp"
#include "Spire/Canvas/LuaNodes/NativeLuaReactorParameter.hpp"

namespace Spire {
  template<>
  struct PopLuaValue<Nexus::CurrencyId> {
    Nexus::CurrencyId operator ()(lua_State& state) const {
      return Nexus::CurrencyId{static_cast<int>(lua_tonumber(&state, -1))};
    }
  };

  template<>
  struct PushLuaValue<Nexus::CurrencyId> {
    void operator ()(lua_State& state, Nexus::CurrencyId value) const {
      lua_pushnumber(&state, static_cast<int>(value.m_value));
    }
  };

  template<>
  struct PopLuaValue<Nexus::Money> {
    Nexus::Money operator ()(lua_State& state) const {
      return Nexus::Money{Nexus::Quantity::FromRepresentation(
        lua_tonumber(&state, -1))};
    }
  };

  template<>
  struct PushLuaValue<Nexus::Money> {
    void operator ()(lua_State& state, Nexus::Money value) const {
      lua_pushnumber(&state,
        static_cast<Nexus::Quantity>(value).GetRepresentation());
    }
  };

  template<>
  struct PopLuaValue<Nexus::Quantity> {
    Nexus::Quantity operator ()(lua_State& state) const {
      return Nexus::Quantity::FromRepresentation(lua_tonumber(&state, -1));
    }
  };

  template<>
  struct PushLuaValue<Nexus::Quantity> {
    void operator ()(lua_State& state, Nexus::Quantity value) const {
      lua_pushnumber(&state,
        static_cast<Nexus::Quantity>(value).GetRepresentation());
    }
  };

  template<>
  struct PopLuaValue<Nexus::Security> {
    Nexus::Security operator ()(lua_State& state) const {

      // TODO
      return Nexus::Security();
    }
  };

  template<>
  struct PushLuaValue<Nexus::Security> {
    void operator ()(lua_State& state, const Nexus::Security& value) const {

      // TODO
    }
  };

  template<>
  struct PopLuaValue<Nexus::TimeInForce> {
    Nexus::TimeInForce operator ()(lua_State& state) const {

      // TODO
      return Nexus::TimeInForce();
    }
  };

  template<>
  struct PushLuaValue<Nexus::TimeInForce> {
    void operator ()(lua_State& state, const Nexus::TimeInForce& value) const {

      // TODO
    }
  };

  template<>
  struct PopLuaValue<Spire::Record> {
    Spire::Record operator ()(lua_State& state) const {

      // TODO
      return Spire::Record();
    }
  };

  void ExportOrderStatus(lua_State& state);
  void ExportOrderType(lua_State& state);
  void ExportSide(lua_State& state);
  void ExportTaskState(lua_State& state);
  void ExportLuaTypes(lua_State& state);
}

#endif
