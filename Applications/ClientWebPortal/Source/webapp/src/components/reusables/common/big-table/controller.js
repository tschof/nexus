import View from './view';
import TableModel from './table-model';

class Controller {
  constructor(react, componentModel) {
    this.componentModel = componentModel;
    this.tableModel = new TableModel(this.componentModel.dataModel, this.componentModel.columnTypes);
    this.componentModel.setReference(this);
    this.columnSortOrders = [];
    this.changeSortOrder = react.props.changeSortOrder;
  }

  getView() {
    return this.view;
  }

  setView(view) {
    this.view = view;
    this.view.setTableModel(this.tableModel);
  }

  componentDidMount() {
    this.view.initialize();
  }

  componentWillUnmount() {
    this.view.dispose();
  }

  rowUpdate(index) {
    this.tableModel.rowUpdate(index);
  }

  rowAdd(index) {
    this.tableModel.rowAdd(index);
  }

  rowRemove(index) {
    this.tableModel.rowRemove(index);
  }

  rowMove(fromIndex, toIndex) {
    this.tableModel.rowMove(fromIndex, toIndex);
  }

  resize() {
    this.view.setViewingRegionDimensions();
  }

  updateColumnChange(dataModel, omittedColumns) {
    this.tableModel.updateColumnChange(dataModel);
    this.removeFromSortOrders(omittedColumns);
    this.view.updateColumnChange();
  }

  onSortColumnSelected(columnIndex) {
    let sortOrderIndex = null;
    for (let i=0; i<this.columnSortOrders.length; i++) {
      if (this.columnSortOrders[i].index == columnIndex) {
        sortOrderIndex = i;
        break;
      }
    }

    if (sortOrderIndex == null) {
      this.columnSortOrders.unshift({
        index: columnIndex,
        isAsc: true
      });
    } else if (sortOrderIndex == 0) {
      this.columnSortOrders[0].isAsc = !this.columnSortOrders[0].isAsc;
    } else {
      this.columnSortOrders.splice(sortOrderIndex, 1);
      this.columnSortOrders.unshift({
        index: columnIndex,
        isAsc: true
      });
    }

    this.changeSortOrder(this.columnSortOrders);
  }

  getColumnSortOrders() {
    return this.columnSortOrders;
  }

  /** @private */
  removeFromSortOrders(omittedColumns) {
    this.columnSortOrders = this.columnSortOrders.filter(element => {
      return !omittedColumns.includes(element.index);
    });
  }
}

export default Controller;