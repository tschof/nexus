import definitionsServiceClient from 'utils/spire-clients/definitions-service';
import adminClient from 'utils/spire-clients/admin';
import HashMap from 'hashmap';

/** Various definitions queried from back-end */
class DefinitionsService {
  /** @private */
  loadCountries() {
    return definitionsServiceClient.loadCountryData.apply(definitionsServiceClient).then(onResponse.bind(this));

    function onResponse(countries) {
      this.countries = countries;
      this.countriesByNumber = new HashMap();
      this.countriesByThreeLetterCode = new HashMap();
      for (let i=0; i<countries.length; i++) {
        let country = countries[i];
        this.countriesByNumber.set(country.code, country);
        this.countriesByThreeLetterCode.set(country.threeLetterCode, country);
      }
    }
  }

  /** @private */
  loadCurrencies() {
    return definitionsServiceClient.loadCurrencyData.apply(definitionsServiceClient).then(onResponse.bind(this));

    function onResponse(currencies) {
      this.currencies = currencies;
      this.currenciesById = new HashMap();
      this.currenciesByCode = new HashMap();
      for (let i=0; i<currencies.length; i++) {
        let currency = currencies[i];
        this.currenciesById.set(currency.id, currency);
        this.currenciesByCode.set(currency.code, currency);
      }
    }
  }

  /** @private */
  loadEntitlements() {
    return adminClient.loadEntitlementsData.apply(adminClient).then(onResponse.bind(this));

    function onResponse(entitlements) {
      this.entitlements = entitlements;
    }
  }

  /** @private */
  loadComplianceRuleSchemas() {
    return definitionsServiceClient.loadComplianceRuleSchemas.apply(definitionsServiceClient)
      .then(onResponse.bind(this));

    function onResponse(ruleSchemas) {
      this.complianceRuleSchemas = ruleSchemas;
    }
  }

  /** @private */
  loadMarkets() {
    return definitionsServiceClient.loadMarketDatabase.apply(definitionsServiceClient)
      .then(onResponse.bind(this));

    function onResponse(response) {
      this.markets = new HashMap();
      for (let i=0; i<response.entries.length; i++) {
        let marketCode = response.entries[i].code;
        this.markets.set(marketCode, response.entries[i]);
      }
      this.markets.set('*', {
        code: '*',
        country_code: 65535,
        currency: 65535,
        description: '*',
        display_name: '*'
      });
    }
  }

  initialize() {
    return Promise.all([
      this.loadCountries.apply(this),
      this.loadCurrencies.apply(this),
      this.loadEntitlements.apply(this),
      this.loadComplianceRuleSchemas.apply(this),
      this.loadMarkets(this)
    ]);
  }

  getCountries() {
    return this.countries;
  }

  getCountryThreeLetterCode(number) {
    return this.countriesByNumber.get(number).threeLetterCode;
  }

  getCountryNumber(threeLetterCode) {
    return this.countriesByThreeLetterCode.get(threeLetterCode).code;
  }

  getCountryName(number) {
    return this.countriesByNumber.get(number).name;
  }

  getAllCurrencyCodes() {
    let codes = [];
    for (let i=0; i<this.currencies.length; i++) {
      codes.push(this.currencies[i].code);
    }
    return codes;
  }

  getCurrencyCode(id) {
    return this.currenciesById.get(id).code;
  }

  getCurrencySign(id) {
    if (this.doesCurrencyExist(id)){
      return this.currenciesById.get(id).sign;
    } else {
      return '';
    }
  }

  getCurrencyNumber(code) {
    return this.currenciesByCode.get(code).id;
  }

  doesCurrencyExist(id) {
    if (this.currenciesById.get(id) == null) {
      return false;
    } else {
      return true;
    }
  }

  getEntitlements() {
    return this.entitlements;
  }

  getComplianceRuleSchemas() {
    return this.complianceRuleSchemas;
  }

  getMarket(marketCode) {
    return this.markets.get(marketCode);
  }
}

export default new DefinitionsService();
