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

  protected $viewName;
  protected $displayName;
  protected $view;

  /**
   * @var RulesState
   */
  protected $viewLoopState;

  public function __construct($viewName = NULL, $displayName = NULL, $settings = array()) {
    $this->viewName = $viewName;
    $this->displayName = $displayName;
    $this->settings = (array) $settings + $this->settings;
    $this->setUp();
  }

  public function __sleep() {
    return parent::__sleep() + array('viewName' => 'viewName', 'displayName' => 'displayName');
  }

  protected function setUp() {
    parent::setUp();
    $this->setUpDisplay();
  }

  protected function setUpDisplay() {
    if (!$this->view && $this->viewName && $this->displayName) {
      $view = views_get_view($this->viewName);
      if ($view->set_display($this->displayName)) {
        $this->view = $view;
      }
    }
  }

  public function integrityCheck() {
    parent::integrityCheck();

    // Check view is configured.
    $view = $this->getView();
    if (!$view) {
      throw new RulesIntegrityException(t('%plugin: View display is not configured.', array('%plugin' => $this->getPluginName())), $this);
    }

    // Check view display is an iterator.
    $display = $view->display_handler;
    if (!$display instanceof views_rules_iterator) {
      throw new RulesIntegrityException(t('%plugin: Configured view display is not a Rules iterator.', array('%plugin' => $this->getPluginName())), $this);
    }

    // Validate view display.
    /** @var $display views_plugin_display */
    if (!$view->validate()) {
      throw new RulesIntegrityException(t('%plugin: Configured view does not validate.', array('%plugin' => $this->getPluginName())), $this);
    }

    // Check row variables.
    /** @var $display views_rules_iterator */
    foreach ($display->get_rules_variable_info() as $name => $info) {
      if (isset($this->settings[$name . ':var'])) {
        $this->checkVarName($this->settings[$name . ':var']);
      }
    }
  }

  public function evaluate(RulesState $state) {
    try {
      if ($iterator = $this->getViewIterator()) {
        // Prepare view arguments.
        $arguments = array();
        foreach ($iterator->get_rules_parameter_info() as $name => $info) {
          $arguments[] = $this->getArgument($name, $info, $state);
        }
        // Execute view.
        $this->viewLoopState = $state;
        try {
          $iterator->execute_iterator($arguments, $this);
        }
        catch (views_rules_iterator_exception $e) {
          throw new RulesEvaluationException($e->getMessage(), array(), $this);
        }
        unset($this->viewLoopState);
      }
      else {
        throw new RulesEvaluationException('Unable to evaluate invalid view iterator.', array(), $this);
      }
    }
    catch (RulesEvaluationException $e) {
      rules_log($e->msg, $e->args, $e->severity);
      rules_log('Unable to evaluate %name.', array('%name' => $this->getPluginName()), RulesLog::WARN, $this);
    }
  }

  public function evaluateRow(array $data) {
    // Clone state to evaluate children in a sandbox.
    $rowState = clone $this->viewLoopState;

    // Fill in state data.
    foreach ($this->getViewIterator()->get_rules_variable_info() as $name => $info) {
      $rowState->addVariable($this->settings[$name . ':var'], $data[$name], $info);
    }
    parent::evaluate($rowState);

    // Update variables from parent scope.
    foreach ($this->viewLoopState->variables as $key => &$value) {
      if (array_key_exists($key, $rowState->variables)) {
        $value = $rowState->variables[$key];
      }
    }
  }

  public function pluginParameterInfo() {
    if ($iterator = $this->getViewIterator()) {
      return $iterator->get_rules_parameter_info();
    }
    return array();
  }

  protected function stateVariables($element = NULL) {
    $variables = parent::stateVariables($element);

    // Add row variables to state.
    if ($iterator = $this->getViewIterator()) {
      $settings = $this->settings;
      foreach ($iterator->get_rules_variable_info() as $name => $info) {
        if (isset($settings[$name . ':var'])) {
          $variables[$settings[$name . ':var']] = array(
            'type' => $info['type'],
            'label' => $settings[$name . ':label'],
          );
        }
      }
    }

    return $variables;
  }

  /**
   * @return view
   */
  public function getView() {
    $this->forceSetUp();
    return $this->view;
  }

  /**
   * @return views_rules_iterator
   */
  public function getViewIterator() {
    if ($view = $this->getView()) {
      return $view->display_handler;
    }
    return NULL;
  }

  public function label() {
    $view = $this->getView();
    return t('Views loop: @view - @display', array(
      '@view' => ($viewLabel = $view->get_human_name()) ? $viewLabel : $view->name,
      '@display' => $view->display_handler->display->display_title,
    ));
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
