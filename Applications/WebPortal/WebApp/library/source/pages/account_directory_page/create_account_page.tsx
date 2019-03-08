import { css, StyleSheet } from 'aphrodite';
import * as Beam from 'beam';
import * as Dali from 'dali';
import * as Nexus from 'nexus';
import * as React from 'react';
import { DisplaySize, HLine } from '../..';
import { CountrySelectionBox, TextField } from '../../components';
import { FormEntry, PhotoField, PhotoFieldDisplayMode, RolesField }
  from '../account_page/profile_page';
import { GroupSelectionBox } from './group_selection_box';
import { GroupSuggestionModel } from './group_suggestion_model';

interface Properties {

  /** The size of the element to display. */
  displaySize: DisplaySize;

  /** The database of available countries. */
  countryDatabase: Nexus.CountryDatabase;

  /** The status given back from the server on callback. */
  errorStatus?: string;

  /** The call to submit the profile page.
   * @param username - The username for the account.
   * @param groups - The groups the account belongs to.
   * @param identity - Contains extra details about the account.
   * @param roles - The roles associated with the account.
   */
  onSubmit?: (username: string, groups: Beam.DirectoryEntry[],
    identity: Nexus.AccountIdentity, roles: Nexus.AccountRoles) => void;

  /** The model that provides the group suggestions. */
  groupSuggestionModel?: GroupSuggestionModel;
}

interface State {
  roles: Nexus.AccountRoles;
  username: string;
  identity: Nexus.AccountIdentity;
  groupsValue: string;
  suggestedGroups: Beam.DirectoryEntry[];
  selectedGroups: Beam.DirectoryEntry[];
  isSubmitButtonDisabled: boolean;
  errorStatus: string;
  firstNameError: boolean;
  lastNameError: boolean;
  userNameError: boolean;
  roleError: string;
  groupError: boolean;
  emailError: boolean;
  photoUploaderMode: PhotoFieldDisplayMode;
  newPhoto: string;
  newScaling: number;
}

/** The page that is shown when the user wants to create a new account. */
export class CreateAccountPage extends React.Component<Properties, State> {
  public static readonly defaultProps = {
    errorStatus: '',
    onSubmit: () => {}
  };

  constructor(props: Properties) {
    super(props);
    this.state = {
      roles: new Nexus.AccountRoles(),
      username: '',
      identity: new Nexus.AccountIdentity(),
      groupsValue: '',
      suggestedGroups: new Array<Beam.DirectoryEntry>(),
      selectedGroups: new Array<Beam.DirectoryEntry>(),
      isSubmitButtonDisabled: true,
      errorStatus: '',
      roleError: '',
      firstNameError: false,
      lastNameError: false,
      userNameError: false,
      groupError: false,
      emailError: false,
      photoUploaderMode: PhotoFieldDisplayMode.DISPLAY,
      newPhoto: '',
      newScaling: 1
    };
    this.onPhotoFieldClick = this.onPhotoFieldClick.bind(this);
    this.onPhotoChange = this.onPhotoChange.bind(this);
    this.onScaleChange = this.onScaleChange.bind(this);
    this.onPhotoSubmit = this.onPhotoSubmit.bind(this);
    this.onRoleClick = this.onRoleClick.bind(this);
    this.onFirstNameChange = this.onFirstNameChange.bind(this);
    this.onLastNameChange = this.onLastNameChange.bind(this);
    this.onUsernameChange = this.onUsernameChange.bind(this);
    this.onGroupsValueChange = this.onGroupsValueChange.bind(this);
    this.filterGroups = this.filterGroups.bind(this);
    this.addGroup = this.addGroup.bind(this);
    this.removeGroup = this.removeGroup.bind(this);
    this.onEmailChange = this.onEmailChange.bind(this);
    this.onAddressChange = this.onAddressChange.bind(this);
    this.onCityChange = this.onCityChange.bind(this);
    this.onProvinceChange = this.onProvinceChange.bind(this);
    this.onCountryChange = this.onCountryChange.bind(this);
    this.checkInputs = this.checkInputs.bind(this);
    this.onSubmit = this.onSubmit.bind(this);
  }

  public render(): JSX.Element {
    const contentWidth = (() => {
      switch(this.props.displaySize) {
        case DisplaySize.SMALL:
          return CreateAccountPage.STYLE.contentSmall;
        case DisplaySize.MEDIUM:
          return CreateAccountPage.STYLE.contentMedium;
        case DisplaySize.LARGE:
          return CreateAccountPage.STYLE.contentLarge;
      }
    })();
    const topPadding = (() => {
      if(this.props.displaySize === DisplaySize.SMALL) {
       return <Dali.Padding size='30px'/>;
      } else {
        return <Dali.Padding size='60px'/>;
      }
    })();
    const photo = (() => {
      return (
        <PhotoField
          displaySize={this.props.displaySize}
          displayMode={this.state.photoUploaderMode}
          imageSource={this.state.identity.photoId}
          onToggleUploader={this.onPhotoFieldClick}
          onSubmit={this.onPhotoSubmit}
          onNewPhotoChange={this.onPhotoChange}
          onNewScalingChange={this.onScaleChange}
          newImageSource={this.state.newPhoto}
          newScaling={this.state.newScaling}
          scaling={1}/>);
    })();
    const sidePanelPhoto = (() => {
      if(this.props.displaySize === DisplaySize.SMALL) {
        return null;
      } else {
        return photo;
      }
    })();
    const sidePanelPhotoPadding = (() => {
      switch(this.props.displaySize) {
        case DisplaySize.SMALL:
          return 0;
        case DisplaySize.MEDIUM:
          return '30px';
        case DisplaySize.LARGE:
          return '100px';
      }
    })();
    const topPanelPhoto = (() => {
      if(this.props.displaySize === DisplaySize.SMALL) {
        return (
          <Dali.VBoxLayout>
            {photo}
            <Dali.Padding size={CreateAccountPage.STANDARD_PADDING}/>
          </Dali.VBoxLayout>);
      } else {
        return null;
      }
    })();
    const buttonStyle = (() => {
      if(this.props.displaySize === DisplaySize.SMALL) {
        return CreateAccountPage.DYNAMIC_STYLE.buttonSmall;
      } else {
        return CreateAccountPage.DYNAMIC_STYLE.buttonLarge;
      }
    })();
    const errorStatus = (() => {
      if(this.state.errorStatus) {
        return (
          <Dali.VBoxLayout>
            <Dali.Padding size='18px'/>
            <div style={CreateAccountPage.STYLE.errorStatus}>
              {this.state.errorStatus}
            </div>
          </Dali.VBoxLayout>);
      } else if(this.props.errorStatus) {
        return (
          <Dali.VBoxLayout>
            <Dali.Padding size='18px'/>
            <div style={CreateAccountPage.STYLE.errorStatus}>
              {this.props.errorStatus}
            </div>
          </Dali.VBoxLayout>);
      } else {
        return null;
      }
    })();
    return (
      <div style={CreateAccountPage.STYLE.page}>
        <div style={CreateAccountPage.STYLE.pagePadding}/>
        <div style={contentWidth}>
          <Dali.VBoxLayout width='100%'>
            <Dali.Padding size='18px'/>
            <div style={CreateAccountPage.STYLE.headerStyler}>
              Create Account
            </div>
            {topPadding}
            <Dali.HBoxLayout>
              {sidePanelPhoto}
              <Dali.Padding size={sidePanelPhotoPadding}/>
              <Dali.VBoxLayout width='100%'>
                {topPanelPhoto}
                <FormEntry name='First Name'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.identity.firstName}
                    displaySize={this.props.displaySize}
                    isError={this.state.firstNameError}
                    onInput={this.onFirstNameChange}/>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Last Name'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.identity.lastName}
                    displaySize={this.props.displaySize}
                    isError={this.state.lastNameError}
                    onInput={this.onLastNameChange}/>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Username'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.username}
                    displaySize={this.props.displaySize}
                    isError={this.state.userNameError}
                    onInput={this.onUsernameChange}/>
                </FormEntry>
                 <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                 <FormEntry name='Role(s)'
                    displaySize={this.props.displaySize}>
                  <div style={CreateAccountPage.STYLE.rolesWrapper}>
                    <RolesField roles={this.state.roles}
                      onClick={this.onRoleClick}/>
                      <div style={CreateAccountPage.STYLE.filler}/>
                      <div style={CreateAccountPage.STYLE.roleErrorStatus}>
                        {this.state.roleError}
                      </div>
                    </div>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Groups(s)'
                    displaySize={this.props.displaySize}>
                  <GroupSelectionBox
                    value={this.state.groupsValue}
                    onValueChange={this.onGroupsValueChange}
                    displaySize={this.props.displaySize}
                    selectedGroups={this.state.selectedGroups}
                    suggestions={this.state.suggestedGroups}
                    addGroup={this.addGroup}
                    removeGroup={this.removeGroup}
                    isError={this.state.groupError}/>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Email'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.identity.emailAddress}
                    displaySize={this.props.displaySize}
                    isError={this.state.emailError}
                    onInput={this.onEmailChange}/>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Address'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.identity.addressLineOne}
                    displaySize={this.props.displaySize}
                    onInput={this.onAddressChange}/>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='City'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.identity.city}
                    displaySize={this.props.displaySize}
                    onInput={this.onCityChange}/>
                </FormEntry>
                <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Province/State'
                    displaySize={this.props.displaySize}>
                  <TextField
                    value={this.state.identity.province}
                    displaySize={this.props.displaySize}
                    onInput={this.onProvinceChange}/>
                </FormEntry>
              <Dali.Padding size={CreateAccountPage.SMALL_PADDING}/>
                <FormEntry name='Country'
                    displaySize={this.props.displaySize}>
                  <CountrySelectionBox
                    displaySize={this.props.displaySize}
                    countryDatabase={this.props.countryDatabase}
                    value={this.state.identity.country}
                    onChange={this.onCountryChange}/>
                </FormEntry>
              </Dali.VBoxLayout>
            </Dali.HBoxLayout>
            <Dali.Padding size={CreateAccountPage.STANDARD_PADDING}/>
            <HLine color='#E6E6E6'/>
            <Dali.Padding size={CreateAccountPage.STANDARD_PADDING}/>
            <div style={CreateAccountPage.STYLE.buttonBox}>
              <button className={css(buttonStyle)}
                  disabled={this.state.isSubmitButtonDisabled}
                  onClick={this.onSubmit}>
                Create Account
              </button>
            </div>
            {errorStatus}
            <Dali.Padding size={CreateAccountPage.BOTTOM_PADDING}/>
          </Dali.VBoxLayout>
        </div>
        <div style={CreateAccountPage.STYLE.pagePadding}/>
      </div>);
  }

  private onPhotoFieldClick() {
    if(this.state.photoUploaderMode === PhotoFieldDisplayMode.DISPLAY) {
      this.setState({
        photoUploaderMode: PhotoFieldDisplayMode.UPLOADING,
        newPhoto: this.state.identity.photoId,
        newScaling: 1
      });
    } else {
      this.setState({
        photoUploaderMode: PhotoFieldDisplayMode.DISPLAY,
        newPhoto: this.state.identity.photoId
      });
    }
  }

  private onPhotoChange(photo: string) {
    this.setState({newPhoto: photo});
  }

  private onScaleChange(scaling: number) {
    this.setState({newScaling: scaling});
  }

  private onPhotoSubmit(newFileLocation: string, scaling: number) {
    this.state.identity.photoId = newFileLocation;
    this.setState({identity: this.state.identity});
  }

  private onRoleClick(role: Nexus.AccountRoles.Role) {
    if(this.state.roles.test(role)) {
      this.state.roles.unset(role);
    } else {
      this.state.roles.set(role);
    }
    this.setState({roles: this.state.roles});
  }

  private onFirstNameChange(newValue: string) {
    this.state.identity.firstName = newValue;
    this.setState({ identity: this.state.identity });
    this.enableSubmit();
  }

  private onLastNameChange(newValue: string) {
    this.state.identity.lastName = newValue;
    this.setState({ identity: this.state.identity });
    this.enableSubmit();
  }

  private onUsernameChange(newValue: string) {
    this.setState({ username: newValue });
    this.enableSubmit();
  }

  private async onGroupsValueChange(newValue: string) {
    this.setState({
      groupsValue: newValue
    });
    this.filterGroups(newValue);
  }
  private async filterGroups(newValue: string) {
    const newSuggestions =
      await this.props.groupSuggestionModel.loadSuggestions(newValue);
    const filteredSuggestins = [];
    for (const group of newSuggestions) {
      if(this.state.selectedGroups.indexOf(group) < 0) {
        filteredSuggestins.push(group);
      }
    }
    this.setState({
      suggestedGroups: filteredSuggestins
    });
  }

  private async addGroup(group: Beam.DirectoryEntry) {
    if(this.state.selectedGroups.indexOf(group) < 0) {
      this.state.selectedGroups.push(group);
      this.setState({
        selectedGroups: this.state.selectedGroups,
        groupsValue: '',
        suggestedGroups: null
      });
    }
  }

  private removeGroup(group: Beam.DirectoryEntry) {
    this.state.selectedGroups.
      splice(this.state.selectedGroups.indexOf(group), 1);
    this.setState({
      selectedGroups: this.state.selectedGroups
    });
  }

  private onEmailChange(newValue: string) {
    this.state.identity.emailAddress = newValue;
    this.setState({ identity: this.state.identity });
    this.enableSubmit();
  }

  private onAddressChange(newValue: string) {
    this.state.identity.addressLineOne = newValue;
    this.setState({ identity: this.state.identity });
    this.enableSubmit();
  }

  private onCityChange(newValue: string) {
    this.state.identity.city = newValue;
    this.setState({ identity: this.state.identity });
    this.enableSubmit();
  }

  private onProvinceChange(newValue: string) {
    this.state.identity.province = newValue;
    this.setState({identity: this.state.identity});
    this.enableSubmit();
  }

  private onCountryChange(newValue: Nexus.CountryCode) {
    this.state.identity.country = newValue;
    this.setState({identity: this.state.identity});
    this.enableSubmit();
  }

  private enableSubmit() {
    if(this.state.identity.firstName || this.state.identity.lastName ||
        this.state.username || this.state.identity.addressLineOne ||
        this.state.identity.city || this.state.identity.province ||
        this.state.identity.emailAddress) {
      this.enableButton();
    } else {
      this.setState({isSubmitButtonDisabled: false});
    }
  }

  private enableButton() {
    this.setState({isSubmitButtonDisabled: false});
  }

  private onSubmit() {
    if(this.checkInputs()) {
      this.props.onSubmit(this.state.username, null,
        this.state.identity, this.state.roles);
    }
  }

  private checkInputs(): boolean {
    const errorFirstName = this.state.identity.firstName === '';
    const errorLastName = this.state.identity.lastName === '';
    const errorUsername = this.state.username === '';
    const errorEmail = this.state.identity.emailAddress === '' ||
      (!this.state.identity.emailAddress.includes('@') &&
        !this.state.identity.emailAddress.includes('.'));
    const errorRoles = (() => {
      if(this.state.roles.test(Nexus.AccountRoles.Role.ADMINISTRATOR) ||
          this.state.roles.test(Nexus.AccountRoles.Role.MANAGER) ||
          this.state.roles.test(Nexus.AccountRoles.Role.TRADER) ||
          this.state.roles.test(Nexus.AccountRoles.Role.SERVICE)) {
        return '';
      } else {
        return 'Select role(s)';
      }
    })();
    let errorGroups = true;
    for (const group of this.state.selectedGroups) {
      if(group.id) {
        errorGroups = false;
      }
      break;
    }
    if(errorFirstName || errorLastName || errorEmail || errorUsername ||
        errorRoles || errorGroups) {
      this.setState({
        errorStatus: 'Invalid inputs',
        firstNameError: errorFirstName,
        lastNameError: errorLastName,
        emailError: errorEmail,
        userNameError: errorUsername,
        groupError: errorGroups,
        roleError: errorRoles
      });
      return false;
    } else {
      this.setState({
        errorStatus: '',
        firstNameError: errorFirstName,
        lastNameError: errorLastName,
        emailError: errorEmail,
        userNameError: errorUsername,
        roleError: errorRoles
      });
      return true;
    }
  }

  private static readonly STYLE = {
    page: {
      height: '100%',
      width: '100%',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      justifyContent: 'space-between' as 'space-between',
      overflowY: 'auto' as 'auto'
    },
    headerStyler: {
      color: '#333333',
      font: '400 18px Roboto',
      width: '100%',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      justifyContent: 'center' as 'center',
      alignItems: 'center' as 'center'
    },
    contentSmall: {
      minWidth: '284px',
      flexShrink: 1,
      flexGrow: 1,
      maxWidth: '424px'
    },
    contentMedium: {
      width: '732px'
    },
    contentLarge: {
      width: '1000px'
    },
    pagePadding: {
      width: '18px'
    },
    rolesWrapper: {
      marginLeft: '11px',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      height: '34px',
      justifyContent: 'flex-start',
      alignItems: 'center'
    },
    filler: {
      flexGrow: 1
    },
    smallPadding: {
      width: '100%',
      height: '18px'
    },
    mediumPadding: {
      width: '100%',
      height: '30px'
    },
    buttonBox: {
      display: 'flex' as 'flex',
      justifyContent: 'center' as 'center',
      textAlign: 'center' as 'center'
    },
    errorStatus: {
      display: 'flex' as 'flex',
      justifyContent: 'center' as 'center',
      textAlign: 'center' as 'center',
      color: '#E63F44',
      font: '400 14px Roboto'
    },
    roleErrorStatus: {
      color: '#E63F44',
      font: '400 14px Roboto'
    }
  };
  private static DYNAMIC_STYLE = StyleSheet.create({
    buttonSmall: {
      boxSizing: 'border-box' as 'border-box',
      width: '100%',
      height: '34px',
      backgroundColor: '#684BC7',
      font: '400 14px Roboto',
      color: '#FFFFFF',
      border: 'none',
      outline: 0,
      borderRadius: 1,
      cursor: 'pointer' as 'pointer',
      ':active': {
        backgroundColor: '#4B23A0'
      },
      ':focus' : {
        backgroundColor: '#4B23A0'
      },
      ':hover' : {
        backgroundColor: '#4B23A0'
      },
      ':disabled': {
        backgroundColor: '#F8F8F8',
        color: '#8C8C8C',
        cursor: 'default' as 'default'
      }
    },
    buttonLarge: {
      margin: 0,
      padding: 0,
      boxSizing: 'border-box' as 'border-box',
      width: '200px',
      height: '34px',
      backgroundColor: '#684BC7',
      font: '400 14px Roboto',
      color: '#FFFFFF',
      border: 'none',
      outline: 0,
      borderRadius: 1,
      cursor: 'pointer' as 'pointer',
      ':active': {
        backgroundColor: '#4B23A0'
      },
      ':focus' : {
        backgroundColor: '#4B23A0'
      },
      ':hover' : {
        backgroundColor: '#4B23A0'
      },
      ':disabled': {
        backgroundColor: '#F8F8F8',
        color: '#8C8C8C',
        cursor: 'default' as 'default'
      }
    }
  });
  private static readonly SMALL_PADDING = '20px';
  private static readonly STANDARD_PADDING = '30px';
  private static readonly BOTTOM_PADDING = '60px';
}
