import React from 'react';
import UpdatableView from 'commons/updatable-view';
import autosize from 'autosize';

class CommonView extends UpdatableView {
  constructor(react, controller, componentModel) {
    super(react, controller, componentModel);
    this.isLastInputLineBreak = false;
    this.containerClass = 'personal-details-container';
    if (this.componentModel.isReadOnly) {
      this.containerClass += ' readonly-view';
    }

    this.initialize = this.initialize.bind(this);
  }

  ignoreNewLineIfNecessary(event) {
    var numLineBreaks = (event.currentTarget.value.match(/\n/g) || []).length;
    if (event.keyCode === 13 && (numLineBreaks === 2 || this.isLastInputLineBreak || this.componentModel.addressLineOne.length === 0)) {
      event.preventDefault();
    }

    if (event.keyCode == 13) {
      this.isLastInputLineBreak = true;
    } else {
      this.isLastInputLineBreak = false;
    }
  }

  /** @private */
  onAddressInput(event) {
    this.onChange();
  }

  /** @private */
  onChange() {
    this.componentModel.firstName = $('#' + this.componentModel.componentId + ' .first-name-input').val().trim();
    this.componentModel.lastName = $('#' + this.componentModel.componentId + ' .last-name-input').val().trim();
    this.componentModel.email = $('#' + this.componentModel.componentId + ' .email-input').val().trim();
    this.componentModel.city = $('#' + this.componentModel.componentId + ' .city-input').val().trim();
    this.componentModel.province = $('#' + this.componentModel.componentId + ' .province-input').val().trim();
    this.componentModel.country = parseInt($('#' + this.componentModel.componentId + ' .country-input').val().trim());

    // address
    let addressLines = $('#' + this.componentModel.componentId + ' .address-input').val().trim().split('\n');
    for (let i=0; i<3; i++) {
      if (addressLines[i] != null) {
        if (i === 0) {
          this.componentModel.addressLineOne = addressLines[i];
        } else if (i === 1) {
          this.componentModel.addressLineTwo = addressLines[i];
        } else if (i === 2) {
          this.componentModel.addressLineThree = addressLines[i];
        }
      } else {
        if (i === 0) {
          this.componentModel.addressLineOne = '';
        } else if (i === 1) {
          this.componentModel.addressLineTwo = '';
        } else if (i === 2) {
          this.componentModel.addressLineThree = '';
        }
      }
    }

    // roles
    this.componentModel.roles.isAdmin = $('#' + this.componentModel.componentId + ' .icon-admin').hasClass('active');
    this.componentModel.roles.isManager = $('#' + this.componentModel.componentId + ' .icon-manager').hasClass('active');
    this.componentModel.roles.isTrader = $('#' + this.componentModel.componentId + ' .icon-trader').hasClass('active');
    this.componentModel.roles.isService = $('#' + this.componentModel.componentId + ' .icon-service').hasClass('active');

    this.controller.onPersonalDetailsChange(this.componentModel);
  }

  /** @private */
  onRoleClick(e) {
    if (!this.componentModel.isReadOnly) {
      let $role = $(e.currentTarget);
      if ($role.hasClass('active')) {
        $role.removeClass('active');
      } else {
        $role.removeClass('active').addClass('active');
      }
      this.onChange();
    }
  }

  /** @private */
  initializeAddressInput() {
    let address = '';
    if (this.componentModel.addressLineOne.length > 0) {
      address += this.componentModel.addressLineOne;
    }
    if (this.componentModel.addressLineTwo.length > 0) {
      address += '\n' + this.componentModel.addressLineTwo;
    }
    if (this.componentModel.addressLineThree.length > 0) {
      address += '\n' + this.componentModel.addressLineThree;
    }
    let $addressInput = $('#' + this.componentModel.componentId + ' .address-input');
    $addressInput
      .val(address)
      .keydown(this.ignoreNewLineIfNecessary.bind(this))
      .on('input', this.onAddressInput.bind(this));
    autosize($addressInput);

    setTimeout(() => {
      let event = document.createEvent('Event');
      event.initEvent('autosize:update', true, false);
      $addressInput[0].dispatchEvent(event);
    }, 0);
  }

  initialize() {
    this.initializeAddressInput();
  }

  dispose() {
    autosize.destroy($('#' + this.componentModel.componentId + ' .address-input'));
  }
}

export default CommonView;
