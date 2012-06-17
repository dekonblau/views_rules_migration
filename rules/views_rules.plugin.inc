<?php
/**
 * @file
 * Views Rules plugin implementations.
 */

/**
 * Loop plugin for using results from a view with Rules displays.
 */
class ViewsRulesLoop extends RulesActionContainer implements ViewsRulesIterable {

  protected $itemName = 'view loop';

  public function __construct($settings = array(), $variables = NULL) {
    $this->setUp();
    // TODO
  }

  public function integrityCheck() {
    parent::integrityCheck();

    // TODO Check variable names.
    // $this->checkVarName($this->settings['...']);

    // TODO Check variables match the view.
  }

  public function evaluate(RulesState $state) {
    try {
      // TODO Evaluate view as loop.
    }
    catch (RulesEvaluationException $e) {
      rules_log($e->msg, $e->args, $e->severity);
      rules_log('Unable to evaluate %name.', array('%name' => $this->getPluginName()), RulesLog::WARN, $this);
    }
  }

  public function evaluateRow(array $data) {
    // TODO
  }

  public function pluginParameterInfo() {
    // TODO
    return array();
  }

  protected function stateVariables($element = NULL) {
    // TODO
    return parent::stateVariables($element);
  }

  public function label() {
    return !empty($this->label) ? $this->label : t('Views loop');
  }

  protected function exportSettings() {
    $export = parent::exportSettings();
    // TODO
    return $export;
  }

  protected function importSettings($export) {
    parent::importSettings($export);
    // TODO
  }
}

/**
 * Interface for an iterable Rules plugin for calling back from Views.
 */
interface ViewsRulesIterable {
  /**
   * Evaluates a view row in the loop.
   */
  public function evaluateRow(array $data);
}
