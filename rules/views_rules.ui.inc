<?php
/**
 * @file
 * Rules UI implementation for Views Rules plugins.
 */

/**
 * Views loop factory UI.
 */
class ViewsRulesLoopFactoryUI extends RulesPluginFactoryUI {
  /**
   * Builds the factory form for creating an instance of the plugin.
   *
   * @param $form
   *   The form array to add the factory form to.
   * @param $form_state
   *   The current form state.
   */
  public function factoryForm(&$form, &$form_state) {
    // Add view selector.
    $form['views_rules_display'] = array(
      '#type' => 'select',
      '#title' => t('Views iterator display'),
      '#options' => views_rules_list_iterators(),
      '#description' => t('Select the view display to use for looping. To add a display to the list, add a "Rules" display to a view.'),
      '#required' => TRUE,
      '#ajax' => rules_ui_form_default_ajax() + array(
        'trigger_as' => array('name' => 'continue'),
      ),
    );
    $form['continue'] = array(
      '#type' => 'submit',
      '#name' => 'continue',
      '#value' => t('Continue'),
      '#ajax' => rules_ui_form_default_ajax(),
    );
  }

  /**
   * Creates a plugin given submitted form.
   *
   * @param $form
   *   The form array.
   * @param $form_state
   *   The current form state.
   * @return RulesPlugin
   *   Fully created plugin, or NULL (e.g. due to invalid form state).
   */
  public function factoryCreatePlugin($form, $form_state) {
    $display = $form_state['values']['views_rules_display'];
    list($viewName, $displayName) = explode(':', $display, 2);
    return views_rules_loop($viewName, $displayName);
  }
}

/**
 * Views loop administrative UI.
 */
class ViewsRulesLoopUI extends RulesActionContainerUI {
  /**
   * @var ViewsRulesLoop
   */
  protected $element;

  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    // Build form with "Save" button.
    $options['init'] = FALSE;
    parent::form($form, $form_state, $options, $iterator);

    // Add row variable form.
    if ($display = $this->element->getViewIterator()) {
      $settings = $this->element->settings;
      foreach ($display->get_rules_variable_info() as $var_name => $var_info) {
        $form['views_row'][$var_name] = array(
          '#type' => 'fieldset',
          '#title' => check_plain($var_info['label']),
          '#description' => filter_xss(isset($var_info['description']) ? $var_info['description'] : ''),
        );
        $form['views_row'][$var_name]['label'] = array(
          '#type' => 'textfield',
          '#title' => t('Variable label'),
          '#default_value' => isset($settings[$var_name . ':label']) ? $settings[$var_name . ':label'] : $var_info['label'],
          '#required' => TRUE,
        );
        $form['views_row'][$var_name]['var'] = array(
          '#type' => 'textfield',
          '#title' => t('Variable name'),
          '#default_value' => isset($settings[$var_name . ':var']) ? $settings[$var_name . ':var'] : $var_name,
          '#description' => t('The variable name must contain only lowercase letters, numbers, and underscores and must be unique in the current scope.'),
          '#element_validate' => array('rules_ui_element_machine_name_validate'),
          '#required' => TRUE,
        );
      }
      if (element_children($form['views_row'])) {
        $help = '<div class="description">' . t('Adjust the names and labels of row variables (from the view) available in each iteration of the view loop, but note that renaming of already utilizied variables invalidates the existing uses.') . '</div>';
        $form['views_row'] += array(
          '#tree' => TRUE,
          '#prefix' => '<h4 class="rules-form-heading">' . t('Row variables') . '</h4>' . $help,
        );
      }
    }
  }

  public function help() {
    return t('Configure parameters for input into the view and row variables available in loop iterations.');
  }

  public function form_extract_values($form, &$form_state) {
    parent::form_extract_values($form, $form_state);
    $form_values = RulesPluginUI::getFormStateValues($form, $form_state);

    // Extract row variable settings.
    /** @var $display views_rules_iterator */
    $display = $this->element->getViewIterator();
    foreach ($display->get_rules_variable_info() as $var_name => $var_info) {
      $this->element->settings[$var_name . ':var'] = $form_values['views_row'][$var_name]['var'];
      $this->element->settings[$var_name . ':label'] = $form_values['views_row'][$var_name]['label'];
    }
  }

  public function form_validate($form, &$form_state) {
    parent::form_validate($form, $form_state);

    // Validate row variable names.
    $vars = $this->element->availableVariables();
    /** @var $display views_rules_iterator */
    $display = $this->element->getViewIterator();
    foreach ($display->get_rules_variable_info() as $var_name => $var_info) {
      $name = $this->element->settings[$var_name . ':var'];
      if (isset($vars[$name])) {
        form_error($form['views_row'][$var_name]['var'], t('The variable name %name is already taken.', array('%name' => $name)));
      }
    }
  }

  public function operations() {
    $ops = parent::operations();
    if (module_exists('views_ui') && user_access('administer views')) {
      /** @var $view view */
      $view = $this->element->getView();
      $link = array(
        'title' => t('edit view display'),
        'href' => 'admin/structure/views/view/' . $view->name . '/edit/' . $view->current_display,
      );
      array_unshift($ops['#links'], $link);
    }
    return $ops;
  }

  public function buildContent() {
    $content = parent::buildContent();

    // Build row variables.
    /** @var $display views_rules_iterator */
    $display = $this->element->getViewIterator();
    $content['description']['views_row'] = array(
      '#caption' => t('Row variables'),
      '#theme' => 'rules_content_group',
    );
    foreach ($display->get_rules_variable_info() as $var_name => $var_info) {
      if (isset($this->element->settings[$var_name . ':var'])) {
        $content['description']['views_row'][$var_name] = array(
          '#theme' => 'rules_variable_view',
          '#info' => array(
            'type' => $var_info['type'],
            'label' => $this->element->settings[$var_name . ':label'],
          ),
          '#name' => $this->element->settings[$var_name . ':var'],
        );
      }
    }

    return $content;
  }
}

/**
 * Alters action form for views_rules_collect_rows().
 */
function views_rules_collect_rows_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  if (!empty($options['init']) && !isset($form_state['rules_element_step'])) {
    $form_state['rules_element_step'] = 1;
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#limit_validation_errors' => array(array('parameter', 'views_rules_display')),
      '#submit' => array('rules_form_submit_rebuild'),
    );
  }
  else {
    // Add parameter fieldset label.
    if (array_diff(element_children($form['parameter']), array('views_rules_display'))) {
      $help = '<div class="description">' . t('Adjust the names and labels of row variables (from the view) available in each iteration of the view loop, but note that renaming of already utilizied variables invalidates the existing uses.') . '</div>';
      $form['parameter']['views_rules_display']['#suffix'] = '<h4 class="rules-form-heading">' . t('Parameters') . '</h4>' . $help;
    }
    // Change the display parameter to be not editable any more.
    $form['parameter']['views_rules_display']['settings']['#access'] = FALSE;
    $options = views_rules_list_iterators(FALSE);
    $display_label = isset($options[$element->settings['views_rules_display']]) ? $options[$element->settings['views_rules_display']] : '';
    $form['parameter']['views_rules_display']['info'] = array(
      '#prefix' => '<p>',
      '#markup' => t('!selector_label %selector', array('!selector_label' => '<strong>' . t('Selected view display:') . '</strong>', '%selector' => $display_label)),
      '#suffix' => '</p>',
    );
  }
}
