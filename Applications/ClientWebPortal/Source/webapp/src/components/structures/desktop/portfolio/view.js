import './style.scss';
import React from 'react';
import UpdatableView from 'commons/updatable-view';
import PortfolioFilters from 'components/reusables/common/portfolio-filters';
import PortfolioChart from 'components/reusables/common/portfolio-chart';

class View extends UpdatableView {
  constructor(react, controller, componentModel) {
    super(react, controller, componentModel);
  }

  initialize() {
    $(window).resize(this.onWindowResize);
  }

  /** @private */
  onWindowResize() {
    let chartOuterWrapperOffset = $('#portfolio-container .chart-outer-wrapper').offset();
    $('#portfolio-container .chart-inner-wrapper').css('left', -1 * chartOuterWrapperOffset.left);

    let $container = $('#portfolio-container');
    let $chartOuterWrapper = $('#portfolio-container .chart-outer-wrapper');
    if ($chartOuterWrapper.outerWidth() < $container.outerWidth()) {
      $('#portfolio-container .chart-inner-wrapper').css('width', $container.outerWidth());
    } else {
      $('#portfolio-container .chart-inner-wrapper').css('width', '100%');
    }

    // $chartOuterWrapper.removeClass('stretch').addClass('stretch');
    let $portfolioParametersWrapper = $container.find('.porfolio-parameters-wrapper');
    let chartOuterWrapperHeight = $container.height() - $portfolioParametersWrapper.height();
    $chartOuterWrapper.height(chartOuterWrapperHeight);
  }

  dispose() {
    $(window).unbind('resize', this.onWindowResize);
  }

  componentDidUpdate() {
    $('#portfolio-container').fadeIn({
      duration: Config.FADE_DURATION
    });
    $(window).resize();
  }

  render() {
    let parametersModel = {
      groups: this.componentModel.managedGroups || [],
      currencies: this.componentModel.currencies || [],
      markets: this.componentModel.markets || [],
      filter: {}
    }

    let onParametersSave = this.controller.saveParameters.bind(this.controller);

    if (this.controller.isModelInitialized()) {
      $('#portfolio-container').css('display', 'flex');
    }

    let chartModel = {
      data: this.componentModel.portfolioData,
      filter: this.componentModel.filter
    };

    return (
      <div id="portfolio-container" className="container-fixed-width">
        <PortfolioFilters model={parametersModel} onSave={onParametersSave} className={"porfolio-parameters-wrapper"}/>
        <div className="chart-outer-wrapper">
          <div className="chart-inner-wrapper">
            <PortfolioChart model={chartModel} />
          </div>
        </div>
      </div>
    );
  }
}

export default View;
