import * as Nexus from 'nexus';
import * as React from 'react';
import { Transition } from 'react-transition-group';

interface Properties {

  /** The role the icon respresents. */
  role: Nexus.AccountRoles.Role;

  /** Whether the roles can be changed. */
  readonly?: boolean;

  /** Determines if the role is set of not. */
  isSet: boolean;

  /** Called when icon is clicked on. */
  onClick?: () => void;

  /** */
}

interface State {
  showToolTip: boolean;
  showToolTipMobile: boolean;
}

/** Displays a panel of icons highlighting an account's roles. */
export class RoleIcon extends React.Component<Properties, State> {
  public static readonly defaultProps = {
    readonly: false,
    onClick: () => {}
  };

  constructor(props: Properties) {
    super(props);
    this.state = {
      showToolTip: false,
      showToolTipMobile: false
    };
    console.log('HELLO WORLD!');
    this.showToolTip = this.showToolTip.bind(this);
    this.hideToolTip = this.hideToolTip.bind(this);
    this.onTouch = this.onTouch.bind(this);
    this.onClick = this.onClick.bind(this);
  }

  public render(): JSX.Element {
    const iconColor = (() => {
      if(this.props.isSet) {
        return 'purple';
      }
      return 'grey';
    })();
    const iconStyle = (() => {
      if(this.props.readonly) {
        return RoleIcon.STYLE.readonly;
      } else {
        return RoleIcon.STYLE.clickable;
      }
    })();
    return (
      <div style={RoleIcon.STYLE.iconBox}
          onClick={this.onClick}
          onTouchStart={this.onTouch}
          onMouseEnter={this.showToolTip}
          onMouseLeave={this.hideToolTip}>
        <img src={`${this.getSource(this.props.role)}${iconColor}.svg`}
          style={iconStyle}
          width={RoleIcon.IMAGE_SIZE}
          height={RoleIcon.IMAGE_SIZE}/>
        <div style={RoleIcon.STYLE.tooltipAnchor}>
         <Transition timeout={RoleIcon.TIMEOUT_MOBILE_TOOLIP}
            in={this.state.showToolTipMobile}>
            {(state) => (
                <div
                  style={{...RoleIcon.STYLE.animationBase,
                    ...RoleIcon.STYLE.imageTooltip,
                    ...(RoleIcon.ANIMATION_STYLE as any)[state]}}>
                    {this.getText(this.props.role)}
              </div>)}
          </Transition>
          <Transition timeout={RoleIcon.TIMEOUT}
            in={this.state.showToolTip &&
              !this.state.showToolTipMobile}>
            {(state) => (
                <div
                  style={{...RoleIcon.STYLE.animationBase,
                    ...RoleIcon.STYLE.imageTooltip,
                    ...(RoleIcon.ANIMATION_STYLE as any)[state]}}>
                    {this.getText(this.props.role)}
              </div>)}
          </Transition>
        </div>
      </div>);
  }

  private showToolTip() {
    console.log('Mouse entered');
    if(!this.state.showToolTip) {
      this.setState({
        showToolTip: true});
    }
  }

  private hideToolTip() {
    this.setState( {
      showToolTip: false});
    console.log('Mouse left');
  }

  private onClick() {
    console.log('CLICK CLICK!!!!!!');
    this.props.onClick();
  }

  private onTouch() {
    console.log('Button was touched.');
     event.preventDefault();
     this.setState({showToolTip: true});
    setTimeout(() => {
        console.log('Mobile Tooltip hiddenn');
        this.setState({showToolTip: false});
      },
      2000);
  }

  private getText(role: Nexus.AccountRoles.Role) {
    switch (role) {
      case Nexus.AccountRoles.Role.TRADER:
        return RoleIcon.TRADER_TOOLTIP_TEXT;
      case Nexus.AccountRoles.Role.MANAGER:
        return RoleIcon.MANAGER_TOOLTIP_TEXT;
      case Nexus.AccountRoles.Role.ADMINISTRATOR:
        return RoleIcon.ADMINISTRATOR_TOOLTIP_TEXT;
      case Nexus.AccountRoles.Role.SERVICE:
        return RoleIcon.SERVICE_TOOLTIP_TEXT;
    }
  }

  private getSource(role: Nexus.AccountRoles.Role) {
    switch (role) {
      case Nexus.AccountRoles.Role.TRADER:
        return 'resources/account/trader-';
      case Nexus.AccountRoles.Role.MANAGER:
        return 'resources/account/manager-';
      case Nexus.AccountRoles.Role.ADMINISTRATOR:
        return 'resources/account/admin-';
      case Nexus.AccountRoles.Role.SERVICE:
        return 'resources/account/service-';
    }
  }

  private static ANIMATION_STYLE = {
    entering: {
      opacity: 0
    },
    entered: {
      opacity: 1
    },
    exited: {
      display: 'none' as 'none'
    }
  };
  private static STYLE = {
    iconBox: {
      display: 'flex' as 'flex',
      justifyContent: 'center' as 'center',
      alignItems: 'center' as 'center',
      height: '24px',
      width: '24px'
    },
    tooltipAnchor: {
      position: 'relative' as 'relative',
      height: 0,
      width: 0
    },
    clickable: {
      cursor: 'pointer'
    },
    readonly: {
      cursor: 'inherit'
    },
    animationBase: {
      opacity: 0,
      transition: 'opacity 200ms ease-in-out'
    },
    imageTooltip: {
      display: 'flex' as 'flex',
      justifyContent: 'center' as 'center',
      alignItems: 'center' as 'center',
      font: '400 12px Roboto',
      paddingLeft: '15px',
      paddingRight: '15px',
      height: '22px',
      backgroundColor: '#4B23A0',
      color: '#FFFFFF',
      position: 'absolute' as 'absolute',
      top: '16px',
      left: '-20px',
      border: '1px solid #4B23A0',
      borderRadius: '1px',
      boxShadow: '0px 0px 2px #00000064'
    }
  };
  private static readonly TIMEOUT = {
    enter: 100,
    entered: 200,
    exit: 200,
    exited: 200
  };
  private static readonly TIMEOUT_MOBILE_TOOLIP = {
    enter: 1,
    entered: 200,
    exit: 200,
    exited: 200
  };
  private static readonly IMAGE_SIZE = '20px';
  private static readonly TRADER_TOOLTIP_TEXT = 'Trader';
  private static readonly MANAGER_TOOLTIP_TEXT = 'Manager';
  private static readonly ADMINISTRATOR_TOOLTIP_TEXT = 'Admin';
  private static readonly SERVICE_TOOLTIP_TEXT = 'Service';
}
