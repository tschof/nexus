import { css, StyleSheet } from 'aphrodite';
import { Center, HBoxLayout, Padding, VBoxLayout } from 'dali';
import * as Beam from 'beam';
import * as Nexus from 'nexus';
import * as React from 'react';
import { CurrencySelectionField, DisplaySize, IntegerInputBox,
  MoneyInputBox } from '../../..';

interface Properties {

  /** The type of display to render on. */
  displaySize: DisplaySize;

  /** The parameters to display. */
  parameters: Nexus.RiskParameters;

  /** Used to lookup currency names and symbols. */
  currencyDatabase: Nexus.CurrencyDatabase;
}

enum TimeUnit {
  SECONDS,
  MINUTES,
  HOURS
}

/** Implements a React component to display a set of RiskParameters. */
export class RiskParametersView extends React.Component<Properties> {
  constructor(props: Properties) {
    super(props);
    this.onCurrencyChange = this.onCurrencyChange.bind(this);
    this.onBuyingPowerChange = this.onBuyingPowerChange.bind(this);
    this.onNetLossChange = this.onNetLossChange.bind(this);
    this.onTransitionTimeChange = this.onTransitionTimeChange.bind(this);
  }

  public render(): JSX.Element {
    const splitTransitionTime = this.props.parameters.transitionTime.split();
    const currencySign = this.props.currencyDatabase.fromCurrency(
      this.props.parameters.currency).sign;
    return (
      <HBoxLayout width='100%'>
        <Padding/>
        <VBoxLayout width='246px'>
          <Label text='Currency'/>
          <Padding size='12px'/>
          <CurrencySelectionField className={
            css(RiskParametersView.STYLE.dropdownButton)}
            currencyDatabase={this.props.currencyDatabase}
            value={this.props.parameters.currency} onChange={
            this.onCurrencyChange}/>
          <Padding size='30px'/>
          <Label text={`Buying Power (${currencySign})`}/>
          <Padding size='12px'/>
          <MoneyInputBox
            className={css(RiskParametersView.STYLE.inputBox)}
            value={this.props.parameters.buyingPower}
            onChange={this.onBuyingPowerChange}/>
          <Padding size='30px'/>
          <Label text={`Net Loss (${currencySign})`}/>
          <Padding size='12px'/>
          <MoneyInputBox
            className={css(RiskParametersView.STYLE.inputBox)}
            value={this.props.parameters.netLoss}
            onChange={this.onNetLossChange}/>
          <Padding size='30px'/>
          <Label text='Transition Time'/>
          <Padding size='12px'/>
          <HBoxLayout width='100%'>
            <VBoxLayout>
              <IntegerInputBox min={0} value={splitTransitionTime.hours}
                padding={2} className={
                  css(RiskParametersView.STYLE.inputBox)}
                onChange={(value) => this.onTransitionTimeChange(
                  value, TimeUnit.HOURS)}/>
              <Padding size='10px'/>
              <span className={
                  css(RiskParametersView.
                    TRANSITION_TIME_STYLE.label)}>
                HOUR
              </span>
            </VBoxLayout>
            <Padding size='10px'/>
            <Center height='34px' className={
                css(RiskParametersView.TRANSITION_TIME_STYLE.colon)}>
              :
            </Center>
            <Padding size='10px'/>
            <VBoxLayout>
              <IntegerInputBox min={0} max={59} value={
                splitTransitionTime.minutes} padding={2}
                className={css(RiskParametersView.STYLE.inputBox)}
                onChange={(value) => this.onTransitionTimeChange(
                  value, TimeUnit.MINUTES)}/>
              <Padding size='10px'/>
              <span className={
                  css(RiskParametersView.
                    TRANSITION_TIME_STYLE.label)}>
                MINUTE
              </span>
            </VBoxLayout>
            <Padding size='10px'/>
            <Center height='34px' className={
                css(RiskParametersView.TRANSITION_TIME_STYLE.colon)}>
              :
            </Center>
            <Padding size='10px'/>
            <VBoxLayout>
              <IntegerInputBox min={0} max={59} value={
                splitTransitionTime.seconds} padding={2}
                className={css(RiskParametersView.STYLE.inputBox)}
                onChange={(value) => this.onTransitionTimeChange(
                  value, TimeUnit.SECONDS)}/>
              <Padding size='10px'/>
              <span className={
                  css(RiskParametersView.TRANSITION_TIME_STYLE.label)}>
                SECOND
              </span>
            </VBoxLayout>
          </HBoxLayout>
        </VBoxLayout>
        <Padding/>
      </HBoxLayout>);
  }

  private onCurrencyChange(value: Nexus.Currency) {
    this.props.parameters.currency = value;
    this.forceUpdate();
  }

  private onBuyingPowerChange(value: Nexus.Money) {
    this.props.parameters.buyingPower = value;
  }

  private onNetLossChange(value: Nexus.Money) {
    this.props.parameters.netLoss = value;
  }

  private onTransitionTimeChange(value: number, timeUnit: TimeUnit) {
    const timeJSON = this.props.parameters.transitionTime.split();
    const newTimeJSON = (() => {
      switch (timeUnit) {
        case TimeUnit.HOURS:
          return {
            hours: value,
            minutes: timeJSON.minutes,
            seconds: timeJSON.seconds
          };
        case TimeUnit.MINUTES:
          return {
            hours: timeJSON.hours,
            minutes: value,
            seconds: timeJSON.seconds
          };
        case TimeUnit.SECONDS:
          return {
            hours: timeJSON.hours,
            minutes: timeJSON.minutes,
            seconds: value
          };
        }
    })();
    this.props.parameters.transitionTime = Beam.Duration.HOUR.multiply(
      newTimeJSON.hours).add(Beam.Duration.MINUTE.multiply(
      newTimeJSON.minutes)).add(Beam.Duration.SECOND.multiply(
      newTimeJSON.seconds));
  }

  private static TRANSITION_TIME_STYLE = StyleSheet.create({
    label: {
      font: '400 10px Roboto',
      color: '#8C8C8C',
      width: '100%',
      textAlign: 'center'
    },
    colon: {
      font: '400 16px Roboto'
    }
  });
  private static STYLE = StyleSheet.create({
    dropdownButton: {
      backgroundColor: '#F8F8F8',
      border: '1px solid #C8C8C8',
      outline: 0
    },
    inputBox: {
      outline: 0,
      color: '#333333',
      border: '1px solid #C8C8C8',
      ':focus': {
        border: '1px solid #684BC7',
        '-webkit-box-shadow': '0px 0px 1px 0px #684BC7',
        '-moz-box-shadow': '0px 0px 1px 0px #684BC7',
        boxShadow: '0px 0px 1px 0px #684BC7'
      }
    }
  });
}

interface LabelProperties {
  text: string;
}

class Label extends React.Component<LabelProperties> {
  public render(): JSX.Element {
    return (
      <HBoxLayout width='100%'>
        <span className={css(Label.STYLE.text)}>
          {this.props.text}
        </span>
        <Padding/>
      </HBoxLayout>);
  }

  private static STYLE = StyleSheet.create({
    text: {
      font: '400 14px Roboto',
      color: '#333333',
      whiteSpace: 'nowrap'
    }
  });
}
