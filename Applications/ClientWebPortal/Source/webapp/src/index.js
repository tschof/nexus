import ES6Promise from 'es6-promise';
import {render} from 'react-dom';
import React from 'react';
import {Router, browserHistory} from 'react-router/es6';
import rootRoute from 'routes/';
import 'jquery-ui';
import 'jquery-ui-css';
import 'jquery-tagit';
import 'jquery-tagit-css';
import 'index.html';
import 'bootstrap-css';
import 'styles/general.scss';
import configs from './configs';
import eventBus from 'utils/event-bus';
import event from 'utils/event-bus/events';
import userService from 'services/user';
import {ServiceLocatorClient, DirectoryEntry} from 'spire-client';
import sessionInitializer from 'commons/session-initializer';

window.cloneObject = (originalObj) => {
  return JSON.parse(JSON.stringify(originalObj));
};
window.EventBus = eventBus;
window.Event = event;
window.Config = configs;
ES6Promise.polyfill();

let serviceLocatorClient = new ServiceLocatorClient();

serviceLocatorClient.loadCurrentAccount.apply(serviceLocatorClient)
  .then((account) => {
    let accountDirectoryEntry = new DirectoryEntry(
      account.id,
      account.type,
      account.name
    );
    if (!isLoggedIn(accountDirectoryEntry)) {
      if (window.location.href != Config.HOME_URL) {
        window.location.href = Config.HOME_URL;
      } else {
        renderGUI();
      }
    } else {
      userService.initialize.apply(userService, [accountDirectoryEntry])
        .then(sessionInitializer.initialize.bind(sessionInitializer))
        .then(renderGUI)
        .then(publishSignedInEvent)
    }

    function isLoggedIn(accountDirectoryEntry) {
      return accountDirectoryEntry.type != -1;
    }

    function renderGUI() {
      render(
        <Router history={browserHistory} routes={rootRoute}/>,
        document.getElementById('root')
      );
    }

    function publishSignedInEvent() {
      let appRenderedEventListenerId;
      appRenderedEventListenerId = EventBus.subscribe(Event.Application.RENDERED, () => {
        EventBus.unsubscribe(appRenderedEventListenerId);
        EventBus.publish(Event.Application.SIGNED_IN);
      });
    }
  });
