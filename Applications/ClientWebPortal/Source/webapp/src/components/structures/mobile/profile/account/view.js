import './style.scss';
import React from 'react';
import PersonalDetails from 'components/reusables/mobile/personal-details';
import AccountPicture from 'components/reusables/common/account-picture';
import UserInfoNav from 'components/reusables/common/user-info-nav';
import moment from 'moment';
import PrimaryButton from 'components/reusables/common/primary-button';
import UserNotes from 'components/reusables/common/user-notes';
import userService from 'services/user';
import CommonView from 'components/structures/common/profile/account/common-view';

class MobileView extends CommonView {
  constructor(react, controller, componentModel) {
    super(react, controller, componentModel);
  }

  render() {
    let content;
    let userInfoNavModel,
      lastSignin,
      personalDetailsModel,
      accountPictureModel,
      saveButtonModel,
      userNotesModel,
      changePasswordButtonModel,
      saveButton;
    if (this.controller.isModelInitialized()) {
      userInfoNavModel = {
        userName: this.componentModel.userName,
        roles: this.componentModel.roles
      };

      personalDetailsModel = JSON.parse(JSON.stringify(this.componentModel));
      personalDetailsModel.id = personalDetailsModel.directoryEntry.id;
      delete personalDetailsModel.directoryEntry;
      personalDetailsModel.isReadOnly = !userService.isAdmin();

      lastSignin = moment(this.componentModel.lastLoginTime, moment.ISO_8601).toDate().toLocaleString();

      accountPictureModel = {
        picture: this.componentModel.photoId,
        showLabel: false,
        isReadOnly: !userService.isAdmin()
      };

      userNotesModel = {
        userNotes: this.componentModel.userNotes,
        isReadOnly: !userService.isAdmin()
      };

      saveButtonModel = {
        label: 'Save Changes'
      };

      changePasswordButtonModel = {
        label: 'Save Password'
      };

      let onAccountPictureChange = this.controller.onAccountPictureChange.bind(this.controller);
      let onPersonalDetailsChange = this.controller.onPersonalDetailsChange.bind(this.controller);
      let onUserNotesChange = this.controller.onUserNotesChange.bind(this.controller);
      let save = this.controller.save.bind(this.controller);

      if (userService.isAdmin()) {
        saveButton =
          <div className="row save-button-wrapper">
            <PrimaryButton className="save-button" model={saveButtonModel} onClick={save}/>
            <div className="save-message">
              Changes saved
            </div>
          </div>
      }

      content =
        <div>
          <div className="row">
            <UserInfoNav model={userInfoNavModel}/>
          </div>
          <div className="row">
            Last sign-in: {lastSignin}
          </div>
          <div className="header row">
            Account Information
          </div>
          <div className="account-picture-wrapper row">
            <AccountPicture model={accountPictureModel} onPictureChange={onAccountPictureChange}/>
          </div>
          <div className="personal-details-wrapper">
            <PersonalDetails model={personalDetailsModel} onChange={onPersonalDetailsChange}/>
          </div>
          <div className="header row">
            User Notes
          </div>
          <div className="row">
            <UserNotes model={userNotesModel} onChange={onUserNotesChange}/>
          </div>
          {saveButton}
          <hr className="row"/>
          <div className="header row">
            Change Password
          </div>
          <div className="change-password-wrapper row">
            <input className="new-password-input" type="password" placeholder="New Password" onInput={this.hideSavePasswordMessage}/>
            <input className="confirm-password-input" type="password" placeholder="Confirm New Password" onInput={this.hideSavePasswordMessage}/>
            <PrimaryButton className="change-button" model={changePasswordButtonModel} onClick={this.onPasswordChangeClick.bind(this)}/>
            <div className="message-wrapper">
              <div className="message"></div>
            </div>
          </div>
        </div>
    }

    return (
      <div id="account-container">
        {content}
      </div>
    );
  }
}

export default MobileView;
