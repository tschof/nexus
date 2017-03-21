class Controller {
  constructor(react, componentModel) {
    this.componentModel = componentModel;
    this.onParametersSave = react.props.onSave;
    this.isInitialized = false;
  }

  getView() {
    return this.view;
  }

  setView(view) {
    this.view = view;
  }

  componentWillUpdate(model) {
    if (model != null) {
      this.componentModel.data = model;
      this.view.setComponentModel(this.componentModel);
    }
  }

  componentDidMount() {
    this.view.initialize.apply(this.view);
  }

  componentWillUnmount() {
    this.view.dispose.apply(this.view);
  }
}

export default Controller;
