import {Component} from 'react';
import Controller from './controller';
import uuid from 'uuid';

class NotificationDisplayPanel extends Component {
  constructor(props) {
    super(props);
  }

  componentWillMount() {
    let componentModel = this.props.model || {};
    componentModel.componentId = uuid.v4();
    componentModel.className = this.props.className;
    this.controller = new Controller(this, componentModel);
  }

  componentDidMount() {
    this.controller.componentDidMount.apply(this.controller);
  }

  componentWillUnmount() {
    this.controller.componentWillUnmount.apply(this.controller);
  }

  render() {
    return this.controller.getView().render();
  }
}

export default NotificationDisplayPanel;