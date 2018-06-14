import './style.scss';
import React from 'react';
import ReactDOM from 'react-dom';
import UpdatableView from 'commons/updatable-view';
import deviceDetector from 'utils/device-detector';
import PrimaryButton from 'components/reusables/common/primary-button';
import CompliancePanel from 'components/reusables/common/compliance-panel';
import definitionsService from 'services/definitions';
import labelFormatter from 'utils/label-formatter';
import modal from 'utils/modal';
import uuid from 'uuid';

class View extends UpdatableView {
  constructor(react, controller, componentModel) {
    super(react, controller, componentModel);

    this.showSaveSuccessMessage = this.showSaveSuccessMessage.bind(this);
    this.onAddRuleCloseClick = this.onAddRuleCloseClick.bind(this);
  }

  componentDidUpdate() {
    $('#compliance-container').fadeIn({
      duration: Config.FADE_DURATION
    });
  }

  /** @private */
  getRuleEntryPanels() {
    let entryPanels = [];
    let entries = this.componentModel.complianceRuleEntries;
    let onRuleUpdate = this.controller.onRuleUpdate.bind(this.controller);
    for (let i=0; i<entries.length; i++) {
      let entry = entries[i];
      let model= {
        ruleEntryId: entry.id,
        schema: entry.schema,
        state: entry.state,
        isGroup: this.componentModel.isGroup,
        isAdmin: this.componentModel.isAdmin
      };
      entryPanels.push(
        <CompliancePanel key={i} model={model} onUpdate={onRuleUpdate}/>
      );
    }

    return entryPanels;
  }

  /** @private */
  onAddClick() {
    modal.show($('#add-rule-modal'));
  }

  /** @private */
  onNewRuleSelectBtnClick() {
    let ruleTypeName = $('#compliance-container .rule-types li.selected').text();
    if (ruleTypeName != '') {
      ruleTypeName = labelFormatter.toLowerCaseWithUnderscore(ruleTypeName);
      let $newEntryWrapper = $('<div class="new-entry-wrapper"></div>');
      $('#compliance-container .add-rule-wrapper').before($newEntryWrapper);
      this.controller.onRuleAdd(ruleTypeName);

      $('#compliance-container .rule-types li').removeClass('selected');
      modal.hide($('#add-rule-modal'));
    }
  }

  /** @private */
  onNewRuleCancelBtnClick() {
    $('#compliance-container .rule-types li').removeClass('selected');
    modal.hide($('#add-rule-modal'));
  }

  /** @private */
  onNewRuleTypeClick(event) {
    $('#compliance-container .rule-types li').removeClass('selected');
    $(event.currentTarget).addClass('selected');
  }

  showSaveSuccessMessage() {
    let $saveMessage = $('#compliance-container .save-message');
    $saveMessage
      .fadeOut(() => {
        $saveMessage
          .text('Saved')
          .removeClass('red')
          .addClass('purple')
          .fadeIn();
      });
  }

  showSaveFailedMessage(message) {
    let $saveMessage = $('#compliance-container .save-message');
    $saveMessage
      .fadeOut(() => {
        $saveMessage
          .text(message)
          .removeClass('purple')
          .addClass('red')
          .fadeIn();
      });
  }

  /** @private */
  onSaveClick() {
    let numInvalidInputs = $('#compliance-container .invalid-input').size();
    if (numInvalidInputs == 0) {
      this.controller.save();
      $('#compliance-container .save-message').text('').removeClass('red').css('display', 'none');
    } else {
      $('#compliance-container .save-message')
        .text('Invalid inputs exist.')
        .addClass('red')
        .css('display', 'inherit');
    }
  }

  /** @private */
  onAddRuleCloseClick() {
    modal.hide($('#add-rule-modal'));
  }

  render() {
    let content;

    if (this.controller.isModelInitialized()) {
      let complianceRuleEntries = this.getRuleEntryPanels();

      let onSave = this.onSaveClick.bind(this);

      let saveBtnModel = {
        label: 'Save Changes'
      };
      let addNewRule, saveButton, horizontalDivider;

      if (this.componentModel.isAdmin) {
        addNewRule =
          <div className="add-rule-wrapper">
            <span className="icon-add" onClick={this.onAddClick.bind(this)}></span>
            <span className="title">Add new rule</span>
          </div>
        saveButton = <PrimaryButton className="save-button" model={saveBtnModel} onClick={onSave}/>;
        horizontalDivider = <hr/>;
      }

      content =
        <div>
          {complianceRuleEntries}
          {addNewRule}
          {horizontalDivider}
          {saveButton}
          <div className="save-message"></div>
        </div>
    }

    let className = '';
    if (!deviceDetector.isMobile()) {
      className = 'container-fixed-width';
    }

    let newRuleSelectBtnModel = {
      label: 'Select'
    };
    let onNewRuleSelectBtnClick = this.onNewRuleSelectBtnClick.bind(this);

    let newRuleCancelBtnModel = {
      label: 'Cancel'
    };
    let onNewRuleCancelBtnClick = this.onNewRuleCancelBtnClick.bind(this);

    let schemas = definitionsService.getComplianceRuleSchemas();
    let ruleTypes = [];
    let onNewRuleTypeClick = this.onNewRuleTypeClick.bind(this);
    for (let i=0; i<schemas.length; i++) {
      let schemaName = labelFormatter.toCapitalWithSpace(schemas[i].name);
      ruleTypes.push(<li key={i} onClick={onNewRuleTypeClick}>{schemaName}</li>);
    }

    return (
      <div id="compliance-container" className={className}>
        {content}

        <div id="add-rule-modal" className="modal fade" tabIndex="-1" role="dialog">
          <div className="modal-dialog" role="document">
            <div className="modal-content">
              <div className="modal-header">
                Add new rule
                <span className="icon-close" onClick={this.onAddRuleCloseClick}></span>
              </div>
              <div className="modal-body">
                <div className="rule-types">
                  <ul>
                    {ruleTypes}
                  </ul>
                </div>
                <div className="divider"></div>
                <div className="buttons-wrapper">
                  <PrimaryButton className="new-rule-select-button button single-button" model={newRuleSelectBtnModel} onClick={onNewRuleSelectBtnClick}/>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
}

export default View;