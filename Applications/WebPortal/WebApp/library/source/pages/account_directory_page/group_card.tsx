import { css, StyleSheet } from 'aphrodite/no-important';
import * as Beam from 'beam';
import { VBoxLayout, Padding } from 'dali';
import * as React from 'react';
import { Transition } from 'react-transition-group';
import { DisplaySize, DropDownButton } from '../../..';
import { AccountEntry } from '.';
import { RolePanel } from '../account_page/role_panel';
import { HLine, DropDownButton, DropDownButton } from '../../components';
import { inherits } from 'util';

interface Properties {

  /** Determines the size to display the component at. */
  displaySize: DisplaySize;

  /** The group the card belongs to. */
  group: Beam.DirectoryEntry;

  /** The accounts in the group. */
  accounts: AccountEntry[];

  /** The current filter used on the accounts. */
  filter: string;

  /**  Determines if the card is opened. */
  isOpen: boolean;

  /** Called when the card is clicked. */
  onClick: (group: Beam.DirectoryEntry) => void;
}

/** A card that displays a group and the accounts associated with it. */
export class GroupCard extends React.Component<Properties> {
  public render(): JSX.Element {
    const headerStyle = (() => {
      if(this.props.isOpen) {
        return GroupCard.STYLE.textOpen;
      } else {
        return GroupCard.STYLE.text;
      }
    })();
    const accountsLableStyle = (() => {
      switch(this.props.displaySize) {
        case(DisplaySize.SMALL):
          return GroupCard.STYLE.accountLabelSmall;
        case(DisplaySize.MEDIUM):
          return GroupCard.STYLE.accountLabelMedium;
        case(DisplaySize.LARGE):
          return GroupCard.STYLE.accountLabelLarge;
      }
    })();
    const accounts = (() => {
      const stuff = [];
      if(this.props.accounts.length > 0) {
        for(let i = 0; i < this.props.accounts.length; ++i) {
          stuff.push(
            <div style={GroupCard.STYLE.accountBox}
                key={this.props.accounts[i].account.id}>
              <div style={{...accountsLableStyle,
                ...GroupCard.STYLE.accountLabelText}}>
                {this.props.accounts[i].account.name}
              </div>
              <RolePanel roles={this.props.accounts[i].roles}/>
            </div>);
        }
      } else {
        stuff.push(<div style={{...accountsLableStyle,
                ...GroupCard.STYLE.emptyLableText}}>Empty</div>);
      }
      return stuff;
    })();
    return (
    <VBoxLayout width='100%'>
      <div style={GroupCard.STYLE.header}>
        <DropDownButton size='16px'
          isExpanded={this.props.isOpen}
          onClick={(event?: React.MouseEvent<any>) => {
            this.props.onClick(this.props.group);}
          }/>
        <div style={headerStyle}>{this.props.group.name}</div>
      </div>
      <VBoxLayout>
        <Transition in={this.props.isOpen}
            timeout={GroupCard.TRANSITION_LENGTH_MS}>
            {(state) => (
              <VBoxLayout
                  style={{...GroupCard.STYLE.animationBase
                    ...(GroupCard.ANIMATION_STYLE as any)[state]}}>
                <HLine color='#E6E6E6'/>
                <Padding size='10px'/>
                {accounts}
                <Padding size='20px'/>
              </VBoxLayout>)}
          </Transition>
        </VBoxLayout>
    </VBoxLayout>);
  }

  private static readonly STYLE = {
    box: {
      width: '100%'
    },
    header: {
      boxSizing: 'border-box' as 'border-box',
      height: '40px',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      flexWrap: 'nowrap' as 'nowrap',
      alignItems: 'center' as 'center',
      marginLeft: '10px',
      marginRight: '10px'
    },
    headerSmall: {
      boxSizing: 'border-box' as 'border-box',
      height: '34px',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      flexWrap: 'nowrap' as 'nowrap',
      alignItems: 'center' as 'center',
      marginLeft: '10px',
      marginRight: '10px'
    },
    textOpen: {
      marginLeft: '18px',
      font: '500 14px Roboto',
      color: '#4B23A0'
    },
    text: {
      marginLeft: '18px',
      font: '400 14px Roboto',
      color: '#000000'
    },
    accountLabelSmall: {
      marginLeft: '10px'
    },
    accountLabelMedium: {
      marginLeft: '34px'
    },
    accountLabelLarge: {
      marginLeft: '34px'
    },
    accountLabelText: {
      font: '400 14px Roboto',
      color: '#000000',
      marginRight: '10px'
    },
    emptyLableText: {
      font: '400 14px Roboto',
      color: '#8C8C8C'
    },
    accountBox: {
      boxSizing: 'border-box' as 'border-box',
      height: '34px',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      flexWrap: 'nowrap' as 'nowrap',
      alignItems: 'center' as 'center',
      justifyContent: 'space-between' as 'space-between',
      marginRight: '10px'
    },
    animationBase: {
      overflow: 'hidden' as 'hidden'
    }
  };
  private static readonly ANIMATION_STYLE = {
    entering: {
     height: '100%',
     transition: 'height 100ms ease',
     overflow: 'hidden' as 'hidden'
    },
    entered: {
     height: '100%'
    },
    exiting: {
      height: '0',
      transition: 'height 100ms ease'
    },
    exited: {
      height: '0'
    }
  };
  private static readonly TRANSITION_LENGTH_MS = 100;
  private dropDown: DropDownButton;
}
