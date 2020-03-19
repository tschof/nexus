import * as Nexus from 'nexus';
import * as React from 'react';
import { Checkmark, CurrencySelectionField, DateTimeField, DisplaySize,
  DurationInputField, MoneyField, NumberInput, SecuritiesInput,
  SecurityInput, TextInputField } from '../../..';

interface Properties {

  /** The size at which the component should be displayed at. */
  displaySize: DisplaySize;

  /** The parameter to be displayed. */
  parameter?: Nexus.ComplianceParameter;

  /** The set of available currencies to select. */
  currencyDatabase: Nexus.CurrencyDatabase;

  /** Indicates if the component is readonly. */
  readonly?: boolean;

  /** The event handler called when the parameter changes. */
  onChange?: (newParameter: Nexus.ComplianceParameter) => void;
}

/** Displays a specific parameter. */
export class ParameterEntry extends React.Component<Properties> {
  public static readonly defaultProps = {
    onChange: () => {}
  };

  constructor(props: Properties) {
    super(props);
    this.onChange = this.onChange.bind(this);
    this.onCheckmarkChange = this.onCheckmarkChange.bind(this);
    this.onSecurityListChange = this.onSecurityListChange.bind(this);
  }

  public render(): JSX.Element {
    const rowStyle = (() => {
      if(this.props.parameter.value.type !== 
          Nexus.ComplianceValue.Type.DATE_TIME) {
        if(this.props.displaySize === DisplaySize.SMALL) {
          return ParameterEntry.STYLE.rowSmall;
        } else {
          return ParameterEntry.STYLE.rowLarge;
        }
      } else {
        return ParameterEntry.STYLE.dateTimeRow;
      }
    })();
    const inputWrapper = (() => {
      if(this.props.displaySize === DisplaySize.SMALL) {
        return ParameterEntry.STYLE.inputSmall;
      } else {
        return ParameterEntry.STYLE.inputLarge;
      }
    })();
    const input = (() => {
      switch(this.props.parameter.value.type) {
        case Nexus.ComplianceValue.Type.BOOLEAN:
          return <Checkmark
            onClick={this.onCheckmarkChange}
            displaySize={this.props.displaySize}
            readonly={this.props.readonly}
            isChecked={this.props.parameter.value.value}/>;
        case Nexus.ComplianceValue.Type.CURRENCY:
          return <CurrencySelectionField
            value={this.props.parameter.value.value}
            style={inputWrapper}
            onChange={this.onChange}
            readonly={this.props.readonly}
            currencyDatabase={this.props.currencyDatabase}/>;
        case Nexus.ComplianceValue.Type.DATE_TIME:
          return <DateTimeField
            value={this.props.parameter.value.value}
            displaySize={this.props.displaySize}
            readonly={this.props.readonly}
            onChange={this.onChange}/>;
        case Nexus.ComplianceValue.Type.DOUBLE:
          return <NumberInput 
            value={this.props.parameter.value.value}
            readonly={this.props.readonly}
            onChange={this.onChange}/>;
        case Nexus.ComplianceValue.Type.DURATION:
          return <DurationInputField
            displaySize={this.props.displaySize}
            value={this.props.parameter.value.value}
            readonly={this.props.readonly}
            onChange={this.onChange}/>;
        case Nexus.ComplianceValue.Type.MONEY:
          return <MoneyField
            value={this.props.parameter.value.value}
            readonly={this.props.readonly}
            onChange={this.onChange}/>;
        case Nexus.ComplianceValue.Type.QUANTITY:
          return <NumberInput 
            value={this.props.parameter.value.value}
            readonly={this.props.readonly}
            onChange={this.onChange}/>;
        case Nexus.ComplianceValue.Type.STRING:
          return <TextInputField
            displaySize={this.props.displaySize}
            value={this.props.parameter.value.value}
            onInput={this.onChange}
            readonly={this.props.readonly}
            style={inputWrapper}/>;
        case Nexus.ComplianceValue.Type.SECURITY:
          return <SecurityInput
            displaySize={this.props.displaySize}
            onChange={this.onChange}
            value={this.props.parameter.value.value}/>;
        case Nexus.ComplianceValue.Type.LIST:
          if(this.props.parameter.value.value.length > 0) {
            if(this.props.parameter.value.value[0].type ===
                Nexus.ComplianceValue.Type.SECURITY) {
              return <SecuritiesInput
                displaySize={this.props.displaySize}
                onChange={this.onSecurityListChange}
                readonly={this.props.readonly}
                value={this.convertFromParameterList(
                  this.props.parameter.value.value)}/>;
            }
          } else {
            return <SecuritiesInput
              displaySize={this.props.displaySize}
              onChange={this.onSecurityListChange}
              readonly={this.props.readonly}
              value={[]}/>;
          }
        default:
          return <div/>;
      }
    })();
    return (
      <div style={rowStyle}>
        <div style={ParameterEntry.STYLE.label}>
          {this.props.parameter.name}
        </div>
        <div style={inputWrapper}>
          {input}
        </div>
      </div>);
  }

  private onChange(newValue: any) {
    this.props.onChange(new Nexus.ComplianceParameter(this.props.parameter.name,
      new Nexus.ComplianceValue(this.props.parameter.value.type, newValue)));
  }

  private onCheckmarkChange() {
    this.onChange(!this.props.parameter.value.value);
  }

  private convertFromParameterList(complianceValues: Nexus.ComplianceValue[]) {
    const securityList = [] as Nexus.Security[];
    for(let i = 0; i < complianceValues.length; ++i) {
      securityList.push(complianceValues[i].value);
    }
    return securityList;
  }

  private onSecurityListChange(newValues: Nexus.Security[]) {
    const newParameterList = [];
    for(let i = 0; i < newValues.length; ++i) {
      newParameterList.push(
        new Nexus.ComplianceValue(
          Nexus.ComplianceValue.Type.SECURITY, newValues[i]));
    }
    this.props.onChange(new Nexus.ComplianceParameter(this.props.parameter.name,
      new Nexus.ComplianceValue(
        Nexus.ComplianceValue.Type.LIST, newParameterList)));
  }

  private static readonly STYLE = {
    inputSmall: {
      minWidth: '184px',
      flexBasis: '184px',
      maxWidth: '246px',
      flexGrow: 1,
      flexShrink: 1
    },
    inputLarge: {
      width: '246px',
      minWidth: '246px',
      maxWidth: '246px',
      flexBasis: '246px',
      flexGrow: 0,
      flexShrink: 0
    },
    rowSmall: {
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      flexBasis: '284px',
      flexGrow: 1,
      flexShrink: 1,
      maxWidth: '424px',
      width: '100%',
      height: '34px',
      font: '400 14px Roboto',
      alignItems: 'center' as 'center'
    },
    rowLarge: {
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      width: '100%',
      height: '34px',
      font: '400 14px Roboto',
      alignItems: 'center' as 'center'
    },
    dateTimeRow: {
      boxSizing: 'border-box' as 'border-box',
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      width: '100%',
      height: '78px',
      font: '400 14px Roboto',
      alignItems: 'center' as 'center'
    },
    label: {
      boxSizing: 'border-box' as 'border-box',
      width: '100px',
      font: '400 14px Roboto',
      flexGrow: 0,
      flexShrink: 0,
      display: 'flex' as 'flex',
      flexDirection: 'row' as 'row',
      alignItems: 'center' as 'center',
      height: '34px',
      alignSelf: 'flex-start' as 'flex-start',
      color: '#333333',
      cursor: 'default' as 'default'
    },
  };
}
