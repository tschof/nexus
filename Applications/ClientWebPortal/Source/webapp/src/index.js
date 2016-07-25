import ES6Promise from 'es6-promise';
import {render} from 'react-dom';
import React from 'react';
import {Router, browserHistory} from 'react-router/es6';
import rootRoute from './routes';
import 'index.html';
import 'bootstrap-css';
import 'general.scss';
import configs from './configs';
import eventBus from 'utils/event-bus';
import event from 'utils/event-bus/events';
import routeParameters from 'utils/route-parameters';

let previousPush = browserHistory.push;
browserHistory.push = (path, parameters) => {
  routeParameters.set(parameters);
  previousPush(path);
};

window.cloneObject = (originalObj) => {
  return JSON.parse(JSON.stringify(originalObj));
};
window.EventBus = eventBus;
window.Event = event;
window.Config = configs;
ES6Promise.polyfill();

render(
  <Router history={browserHistory} routes={rootRoute}/>,
  document.getElementById('root')
);