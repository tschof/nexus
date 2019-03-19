import { HBoxLayout, Padding } from 'dali';
import * as Nexus from 'nexus';
import * as React from 'react';
import { RoleIcon } from './role_icon';

interface Properties {

  /** The roles to highlight. */
  roles: Nexus.AccountRoles;

  /** Whether the roles can be selected. */
  readonly?: boolean;

  /** Called when the user clicks on a role
   * @param value - The role of the icon clicked.
   */
  onClick?: (role: Nexus.AccountRoles.Role) => void;
}

/** Displays a panel of icons highlighting an account's roles. */
export class RolesField extends React.Component<Properties, {}> {
  public static readonly defaultProps = {
    readonly: false,
    onClick: () => {}
  }

  public render(): JSX.Element {

    return (
      <HBoxLayout width={RolesField.COMPONENT_WIDTH}
          height={RolesField.IMAGE_SIZE}>
        <RoleIcon role={Nexus.AccountRoles.Role.TRADER}
          readonly={this.props.readonly}
          isSet={this.props.roles.test(Nexus.AccountRoles.Role.TRADER)}
          onClick={() => this.props.onClick(Nexus.AccountRoles.Role.TRADER)}/>
        <Padding size={RolesField.IMAGE_PADDING}/>
        <RoleIcon role={Nexus.AccountRoles.Role.MANAGER}
          readonly={this.props.readonly}
          isSet={this.props.roles.test(Nexus.AccountRoles.Role.MANAGER)}
          onClick={() => this.props.onClick(Nexus.AccountRoles.Role.MANAGER)}/>
        <Padding size={RolesField.IMAGE_PADDING}/>
        <RoleIcon role={Nexus.AccountRoles.Role.ADMINISTRATOR}
          readonly={this.props.readonly}
          isSet={this.props.roles.test(Nexus.AccountRoles.Role.ADMINISTRATOR)}
          onClick={() => 
            this.props.onClick(Nexus.AccountRoles.Role.ADMINISTRATOR)}/>
        <Padding size={RolesField.IMAGE_PADDING}/>
        <RoleIcon role={Nexus.AccountRoles.Role.SERVICE}
          readonly={this.props.readonly}
          isSet={this.props.roles.test(Nexus.AccountRoles.Role.SERVICE)}
          onClick={() => this.props.onClick(Nexus.AccountRoles.Role.SERVICE)}/>
      </HBoxLayout>
     );
  }

  private static readonly IMAGE_SIZE = '20px';
  private static readonly IMAGE_PADDING = '10px';
  private static readonly COMPONENT_WIDTH = '122px';
}
