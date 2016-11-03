import {AdministrationClient, ServiceLocatorClient} from 'spire-client';
import userService from 'services/user';
import preloaderTimer from 'utils/preloader-timer';
import HashMap from 'hashmap';
import {browserHistory} from 'react-router/es6';

class Controller {
  constructor(componentModel) {
    this.componentModel = cloneObject(componentModel);
    this.accountDirectoryEntries = new HashMap();
    this.adminClient = new AdministrationClient();
    this.serviceLocatorClient = new ServiceLocatorClient();
  }

  getView() {
    return this.view;
  }

  setView(view) {
    this.view = view;
  }

  isModelEmpty() {
    let model = cloneObject(this.componentModel);
    delete model.componentId;
    return $.isEmptyObject(model);
  }

  /** @private */
  getRequiredData() {
    let directoryEntry = this.componentModel.directoryEntry;
    let accountDirectoryEntries = this.accountDirectoryEntries;
    let groupedAccounts;
    let loadAllManagedAccounts = this.adminClient.loadManagedTradingGroups.apply(
      this.adminClient,
      [(directoryEntry)]
    ).then((managedGroups) => {
      let loadTradingGroupsPromises = [];
      for (let i=0; i<managedGroups.length; i++) {
        let managedGroupDirectoryEntry = managedGroups[i];
        loadTradingGroupsPromises.push(this.adminClient.loadTradingGroup.apply(this.adminClient, [managedGroupDirectoryEntry]));
      }
      return Promise.all(loadTradingGroupsPromises)
        .then((groupAccounts) => {
          return new Promise((resolve, reject) => {
            let mergedResults = mergeResults(managedGroups, groupAccounts);
            resolve(mergedResults);

            function mergeResults(managedGroups, groupAccounts) {
              for (let i=0; i<managedGroups.length; i++) {
                managedGroups[i].accounts = groupAccounts[i];
              }

              return managedGroups;
            }
          });
        });
    });

    loadAllManagedAccounts = loadAllManagedAccounts
      .then(loadRoles.bind(this))
      .then(rolesLoaded.bind(this));

    return Promise.all([
      loadAllManagedAccounts
    ]);

    function loadRoles(accounts) {
      groupedAccounts = accounts;
      let requestedRoles = new HashMap();
      let loadRolesPromises = [];
      for (let i=0; i<groupedAccounts.length; i++) {
        let groupAccounts = groupedAccounts[i];
        let groupTraders = groupAccounts.accounts.traders;
        for (let j=0; j<groupTraders.length; j++) {
          let traderDirectoryEntry = groupTraders[j];
          accountDirectoryEntries.set(traderDirectoryEntry.id, traderDirectoryEntry);
          if (!requestedRoles.has(traderDirectoryEntry.id)) {
            loadRolesPromises.push(this.adminClient.loadAccountRoles.apply(this.adminClient, [traderDirectoryEntry]));
            requestedRoles.set(traderDirectoryEntry.id, true);
          }
        }
      }
      return Promise.all(loadRolesPromises);
    }

    function rolesLoaded(roles) {
      let rolesMap = new HashMap();
      for (let i=0; i<roles.length; i++) {
        rolesMap.set(roles[i].id, roles[i]);
      }

      for (let i=0; i<groupedAccounts.length; i++) {
        let groupAccounts = groupedAccounts[i];
        let groupTraders = groupAccounts.accounts.traders;
        for (let j=0; j<groupTraders.length; j++) {
          let traderDirectoryEntry = groupTraders[j];
          traderDirectoryEntry.roles = rolesMap.get(traderDirectoryEntry.id);
        }
      }

      return groupedAccounts;
    }
  }

  componentDidMount() {
    this.componentModel = {
      directoryEntry: userService.getDirectoryEntry()
    };
    let requiredDataFetchPromise = this.getRequiredData();

    preloaderTimer.start(
      requiredDataFetchPromise,
      null,
      Config.WHOLE_PAGE_PRELOADER_WIDTH,
      Config.WHOLE_PAGE_PRELOADER_HEIGHT
    ).then((responses) => {
      this.componentModel.groupedAccounts = responses[0];
      this.componentModel.searchString = '';
      this.view.update(this.componentModel);
    });
  }

  componentWillUnmount() {
    this.view.dispose.apply(this.view);
  }

  search(searchString) {
    this.componentModel.searchString = searchString;
    this.view.update(this.componentModel);
  }

  navigateToProfile(traderId) {
    let directoryEntry = this.accountDirectoryEntries.get(traderId);
    browserHistory.push('/profile-account/' +
      directoryEntry.type +
      '/' + directoryEntry.id +
      '/' + directoryEntry.name);
  }

  navigateToNewAccount() {
    browserHistory.push('searchProfiles-newAccount');
  }

  createGroup(groupName) {
    this.serviceLocatorClient.createGroup.apply(this.serviceLocatorClient, [groupName])
      .then(this.view.closeCreateGroupModal.bind(this.view))
      .then(refreshSearchPage.bind(this));

    function refreshSearchPage() {
      this.getRequiredData()
        .then((responses) => {
          this.componentModel.groupedAccounts = responses[0];
          this.componentModel.searchString = '';
          this.view.update(this.componentModel);
        });
    }
  }
}

export default Controller;
