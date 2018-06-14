import httpConnectionManager from '../commons/http-connection-manager';
import AccountRoles from '../../definitions/account-roles';
import AccountIdentity from './account-identity';
import RiskParameters from '../risk-service/risk-parameters';
import RiskModification from '../../definitions/risk-modification';
import DirectoryEntry from '../../definitions/directory-entry';
import AccountModificationRequest from '../../definitions/account-modification-request';
import AccModReqUpdate from '../../definitions/account-modification-request-update';
import EntitlementModification from '../../definitions/entitlement-modification';
import Message from '../../definitions/message';

/** Spire admin client class */
class Admin {
  constructor() {
    this.loadManagedTradingGroups = this.loadManagedTradingGroups.bind(this);
    this.loadTradingGroup = this.loadTradingGroup.bind(this);
    this.loadAccountRoles = this.loadAccountRoles.bind(this);
    this.loadRiskParameters = this.loadRiskParameters.bind(this);
    this.loadAccountIdentity = this.loadAccountIdentity.bind(this);
    this.storeAccountIdentity = this.storeAccountIdentity.bind(this);
    this.loadAccountEntitlements = this.loadAccountEntitlements.bind(this);
    this.storeAccountEntitlements = this.storeAccountEntitlements.bind(this);
    this.storeRiskParameters = this.storeRiskParameters.bind(this);
  }

  /** @private */
  logErrorAndThrow(xhr) {
    let errorMessage = 'Spire Admin Client: Unexpected error happened.';
    console.error(errorMessage);
    console.error(xhr);
    throw errorMessage;
  }

  loadAccountRoles(directoryEntry) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_account_roles';
    let payload = {
      account: directoryEntry.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(onResponse)
      .catch(this.logErrorAndThrow);

    function onResponse(roles) {
      let accRoles = AccountRoles.parse(roles)
      accRoles.id = directoryEntry.id;
      return accRoles;
    }
  }

  loadAccountIdentity(directoryEntry) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_account_identity';
    let payload = {
      account: directoryEntry.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(parseResponse)
      .catch(this.logErrorAndThrow);

    function parseResponse(response) {
      return AccountIdentity.fromData(response);
    }
  }

  storeAccountIdentity(directoryEntry, accountIdentity) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/store_account_identity';
    let payload = {
      account: directoryEntry.toData(),
      identity: {
        address_line_one: accountIdentity.addressLineOne,
        address_line_two: accountIdentity.addressLineTwo,
        address_line_three: accountIdentity.addressLineThree,
        city: accountIdentity.city,
        country: accountIdentity.country,
        e_mail: accountIdentity.email,
        first_name: accountIdentity.firstName,
        last_login_time: accountIdentity.lastLoginTime,
        last_name: accountIdentity.lastName,
        photo_id: accountIdentity.photoId,
        province: accountIdentity.province,
        registration_time: accountIdentity.registrationTime,
        user_notes: accountIdentity.userNotes
      }
    };

    return httpConnectionManager.send(apiPath, payload, false)
      .catch(this.logErrorAndThrow);
  }

  loadRiskParameters(directoryEntry) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_risk_parameters';
    let payload = {
      account: directoryEntry.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(parseResponse)
      .catch(this.logErrorAndThrow);

    function parseResponse(response) {
      return RiskParameters.fromData(response);
    }
  }

  storeRiskParameters(directoryEntry, riskParameters) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/store_risk_parameters';
    let payload = {
      account: directoryEntry.toData(),
      risk_parameters: riskParameters.toData()
    };

    return httpConnectionManager.send(apiPath, payload, false)
      .catch(this.logErrorAndThrow);
  }

  loadEntitlementsData() {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_entitlements_database';
    return httpConnectionManager.send(apiPath, null, true)
      .then(onResponse)
      .catch(this.logErrorAndThrow);

    function onResponse(response) {
      return response.entries;
    }
  }

  loadAccountEntitlements(directoryEntry) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_account_entitlements';
    let payload = {
      account: directoryEntry.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return response.map(value => {
          return DirectoryEntry.fromData(value);
        });
      })
      .catch(this.logErrorAndThrow);
  }

  storeAccountEntitlements(directoryEntry, entitlements) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/store_account_entitlements';
    let payload = {
      account: directoryEntry.toData(),
      entitlements: entitlements
    };

    return httpConnectionManager.send(apiPath, payload, false)
      .catch(this.logErrorAndThrow);
  }

  loadManagedTradingGroups(directoryEntry) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_managed_trading_groups';
    let payload = {
      account: directoryEntry.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }

  loadTradingGroup(directoryEntry) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_trading_group';
    let payload = {
      directory_entry: directoryEntry.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }

  storeAccountRoles(directoryEntry, roles) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/store_account_roles';
    let payload = {
      account: directoryEntry.toData(),
      roles: AccountRoles.encode(roles)
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }

  submitEntitlementModificationRequest(directoryEntry, entitlementModification, comment) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/submit_entitlement_modification_request';
    let payload = {
      account: directoryEntry.toData(),
      modification: entitlementModification.toData(),
      comment: comment.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return AccountModificationRequest.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  submitRiskModificationRequest(directoryEntry, riskModification, comment) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/submit_risk_modification_request';
    let payload = {
      account: directoryEntry.toData(),
      modification: riskModification.toData(),
      comment: comment.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return AccountModificationRequest.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadAccountModificationRequest(id) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_account_modification_request';
    let payload = {
      id: id
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return AccountModificationRequest.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadAccountModificationStatus(id) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_account_modification_request_status';
    let payload = {
      id: id
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return AccModReqUpdate.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadManagedAccountModificationRequests(directoryEntry, startId, maxCount) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_managed_account_modification_request_ids';
    let payload = {
      account: directoryEntry.toData(),
      start_id: startId,
      max_count: maxCount
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }

  loadAccountModificationRequestIds(directoryEntry, startId, maxCount) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_account_modification_request_ids';
    let payload = {
      account: directoryEntry.toData(),
      start_id: startId,
      max_count: maxCount
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }

  loadEntitlementModification(id) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_entitlement_modification';
    let payload = {
      id: id
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return EntitlementModification.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadRiskModification(id) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_risk_modification';
    let payload = {
      id: id
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return RiskModification.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadMessageIds(requestId) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_message_ids';
    let payload = {
      id: requestId
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }

  loadMessage(messageId) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_message';
    let payload = {
      id: messageId
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return Message.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  approveAccountModificationRequest(requestId, message) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/approve_account_modification_request';
    let payload = {
      id: requestId,
      comment: message.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return AccModReqUpdate.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  rejectAccountModificationRequest(requestId, message) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/reject_account_modification_request';
    let payload = {
      id: requestId,
      comment: message.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .then(response => {
        return AccModReqUpdate.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadOrganizationName() {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_organization_name';

    return httpConnectionManager.send(apiPath, null, true)
      .catch(this.logErrorAndThrow);
  }

  loadTradingGroupsRootEntry() {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_trading_groups_root_entry';

    return httpConnectionManager.send(apiPath, null, true)
      .then(response => {
        return DirectoryEntry.fromData(response);
      })
      .catch(this.logErrorAndThrow);
  }

  loadAccountsByRoles(accountRoles) {
    let apiPath = Config.BACKEND_API_ROOT_URL + 'administration_service/load_accounts_by_roles';
    let payload = {
      roles: accountRoles.toData()
    };

    return httpConnectionManager.send(apiPath, payload, true)
      .catch(this.logErrorAndThrow);
  }
}

export default Admin;