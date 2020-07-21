import * as Beam from 'beam';
import { HBoxLayout, Padding, VBoxLayout } from 'dali';
import * as React from 'react';
import { DisplaySize, HLine } from '../..';
import { GroupSubPage } from './group_sub_page';
import { MenuBar } from './menu_bar';

interface Properties {

  /** The account being displayed. */
  account: Beam.DirectoryEntry;

  /** The sub page currently selected. */
  subPage: GroupSubPage;

  /** Determines the layout used to render the page. */
  displaySize: DisplaySize;

  /** Indicates a sub-page menu item was clicked.
   * @param subPage - The GroupSubPage that was clicked.
   */
  onMenuClick?: (subPage: GroupSubPage) => void;
}

/** Implements the container used to display account information. */
export class GroupPage extends React.Component<Properties> {
  public static readonly defaultProps = {
    onMenuClick: () => {}
  }

  public render(): JSX.Element {
    const Header = (() => {
      switch(this.props.displaySize) {
        case DisplaySize.LARGE:
          return LargeHeader;
        case DisplaySize.MEDIUM:
          return MediumHeader;
        case DisplaySize.SMALL:
          return SmallHeader;
      }
    })();
    return (
      <VBoxLayout height='100%' width='100%'
          style={{overflow: 'hidden' as 'hidden'}}>
        <Header name={this.props.account.name}>
          <MenuBar displaySize={this.props.displaySize}
            selected={this.props.subPage} onClick={this.props.onMenuClick}/>
        </Header>
        {this.props.children}
      </VBoxLayout>);
  }
}

class HeaderUnderline extends React.PureComponent {
  public render(): JSX.Element {
    return <HLine color='#E6E6E6' height='1px'/>;
  }
}

interface HeaderProps {
  name: string;
}

class LargeHeader extends React.Component<HeaderProps> {
  public render(): JSX.Element {
    return (
      <VBoxLayout width='100%'>
        <HBoxLayout width='100%' height='40px'>
          <Padding/>
          <HBoxLayout width='1036px' height='40px'>
            <Padding size='18px'/>
            {this.props.children}
            <Padding/>
            <div style={LargeHeader.STYLES.usernameContainer}>
              <div style={LargeHeader.STYLES.usernameWrapper}>
                <UsernameLabel name={this.props.name} height='40px'/>
              </div>
            </div>
            <Padding size='18px'/>
          </HBoxLayout>
          <Padding/>
        </HBoxLayout>
        <HeaderUnderline/>
      </VBoxLayout>);
  }

  private static readonly STYLES = {
    usernameWrapper: {
      height: '40px',
      width: 'auto',
      display: 'flex'
    } as React.CSSProperties,
    usernameContainer: {
      height: '40px',
      display: 'flex',
      flexWrap: 'wrap'
    } as React.CSSProperties
  }
}

class MediumHeader extends React.Component<HeaderProps> {
  public render(): JSX.Element {
    return (
      <VBoxLayout width='100%'>
        <HBoxLayout width='100%' height='40px'>
          <div style={MediumHeader.STYLES.headerPadding}/>
          <Padding size='18px'/>
          <HBoxLayout height='40px' width='732px'>
            {this.props.children}
            <div style={MediumHeader.STYLES.usernameContainer}>
              <div style={MediumHeader.STYLES.usernameWrapper}>
                <UsernameLabel name={this.props.name} height='40px'/>
              </div>
            </div>
          </HBoxLayout>
          <Padding size='18px'/>
          <div style={MediumHeader.STYLES.headerPadding}/>
        </HBoxLayout>
        <HeaderUnderline/>
      </VBoxLayout>);
  }

  private static readonly STYLES = {
    headerPadding: {
      width: 'calc(50% - 384px)',
      height: '100%'
    } as React.CSSProperties,
    usernameWrapper: {
      height: '40px',
      width: 'auto',
      display: 'flex'
    } as React.CSSProperties,
    usernameContainer: {
      height: '40px',
      width: '389px',
      display: 'flex',
      justifyContent: 'end',
      flexWrap: 'wrap'
    } as React.CSSProperties
  }
}

class SmallHeader extends React.Component<HeaderProps> {
  public render(): JSX.Element {
    return (
      <VBoxLayout width='100%'>
        <HBoxLayout width='100%' height='40px'>
          <div style={SmallHeader.STYLES.headerPadding}/>
          <Padding size='18px'/>
          <HBoxLayout height='40px' style={SmallHeader.STYLES.accountHeader}>
            {this.props.children}
            <div style={SmallHeader.STYLES.innerPadding}/>
            <div style={SmallHeader.STYLES.usernameContainer}>
              <div style={SmallHeader.STYLES.usernameWrapper}>
                <UsernameLabel name={this.props.name} height='40px'/>
              </div>
            </div>
          </HBoxLayout>
          <Padding size='18px'/>
          <div style={SmallHeader.STYLES.headerPadding}/>
        </HBoxLayout>
        <HeaderUnderline/>
      </VBoxLayout>);
  }

  private static readonly STYLES = {
    accountHeader: {
      width: '60%',
      minWidth: '284px',
      maxWidth: '424px'
    } as React.CSSProperties,
    headerPadding: {
      width: 'calc(23% - 18px)'
    } as React.CSSProperties,
    innerPadding: {
      width: '35%',
      height: '100%'
    } as React.CSSProperties,
    usernameWrapper: {
      height: '40px',
      width: 'auto',
      display: 'flex'
    } as React.CSSProperties,
    usernameContainer: {
      height: '40px',
      display: 'flex',
      justifyContent: 'end',
      flexWrap: 'wrap'
    } as React.CSSProperties
  }
}

interface UsernameProps {
  name: string;
  height: string;
}

class UsernameLabel extends React.Component<UsernameProps>  {
  public render(): JSX.Element {
    return (
      <VBoxLayout height={this.props.height}>
        <Padding/>
        <span style={UsernameLabel.STYLES.username}>
          {this.props.name}
        </span>
        <Padding/>
      </VBoxLayout>);
  }

  private static readonly STYLES = {
    username: {
      font: '500 14px Roboto',
      color: '#4B23A0',
      whiteSpace: 'nowrap'
    } as React.CSSProperties
  };
}
