<?php
/**
 * @file
 * Rules UI implementation for Views Rules plugins.
 */

/**
 * Views loop administrative UI.
 */
class ViewsRulesLoopUI extends RulesActionContainerUI {
  /**
   * @var ViewsRulesLoop
   */
  protected $element;

  /**
   * Overrides the default form to integrate with Views.
   */
  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    if (!empty($options['init']) && !empty($options['button']) && !$this->property('view')) {
      // Build iterator plugin form.
      $form['#views_iterator'] = TRUE;
    }
    $this->iteratorForm($form, $form_state, $options);

    // Add default form.
    parent::form($form, $form_state, $options, $iterator);
  }

  /**
   * Overrides the parent validator to validate iterator form.
   */
  public function form_validate($form, &$form_state) {
    if (!empty($form['#views_iterator'])) {
      $this->iteratorFormValidate($form, $form_state);
    }
  }

  /**
   * Builds the plugin setup form.
   */
  protected function iteratorForm(&$form, &$form_state, $options) {
    // Add view selector if view display has not been set.
    if (!$view = $this->property('view')) {
      $form['display'] = array(
        '#type' => 'select',
        '#title' => t('Views iterator display'),
        '#options' => views_rules_list_iterators(),
        '#description' => t('Select the view display to use for looping. To add a display to the list, add a "Rules" display to a view.'),
        '#required' => TRUE,
      );
      return;
    }

    // Display view display name.
    /** @var $view view */
    $form['view_name'] = array(
      '#type' => 'item',
      '#title' => t('View'),
      '#markup' => check_plain(($viewLabel = $view->get_human_name()) ? $viewLabel : $view->name),
    );
    $form['display_name'] = array(
      '#type' => 'item',
      '#title' => t('Display'),
      '#markup' => check_plain($view->display_handler->display->display_title),
    );
  }

  /**
   * Validates the plugin setup form.
   */
  protected function iteratorFormValidate(&$form, &$form_state) {
    if (!empty($form_state['values']['display'])) {
      // Finalize setup.
      $this->iteratorFormFinalize($form, $form_state);
      unset($form['#views_iterator']);
      // Rebuild form to add variable forms.
      $form_state['rebuild'] = TRUE;
    }
  }

  /**
   * Sets up the plugin given submitted values.
   */
  protected function iteratorFormFinalize(&$form, &$form_state) {
    if ($this->element instanceof ViewsRulesLoop) {
      $display = $form_state['values']['display'];
      list($viewName, $displayName) = explode(':', $display, 2);
      $loopViewName = &$this->property('viewName');
      $loopDisplayName = &$this->property('displayName');
      $loopViewName = $viewName;
      $loopDisplayName = $displayName;
      $this->element->setUpDisplay();
    }
  }
}
