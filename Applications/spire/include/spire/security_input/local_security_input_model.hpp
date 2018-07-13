#ifndef SPIRE_LOCAL_SECURITY_INPUT_MODEL_HPP
#define SPIRE_LOCAL_SECURITY_INPUT_MODEL_HPP
#include <Beam/Utilities/Trie.hpp>
#include "spire/security_input/security_input.hpp"
#include "spire/security_input/security_input_model.hpp"

namespace spire {

  //! Implements the security input model using a local database.
  class local_security_input_model final : public security_input_model {
    public:

      //! Constructs an empty local security input model.
      local_security_input_model();

      //! Adds a security to this model.
      /*!
        \param security The security entry to add.
      */
      void add(Nexus::SecurityInfo security);

      std::vector<Nexus::SecurityInfo> autocomplete(
        const std::string& query) override;

    private:
      rtv::Trie<char, Nexus::SecurityInfo> m_securities;
  };
}

#endif