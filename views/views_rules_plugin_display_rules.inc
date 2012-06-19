<?php
/**
 * @file
 * Configures views for use within Rules.
 */

/**
 * Rules display plugin.
 */
class views_rules_plugin_display_rules extends views_plugin_display implements views_rules_iterator {
  /**
   * Defines options for configuring the display with Rules.
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['rules_parameter'] = array('default' => array());
    $options['rules_variables'] = array('default' => array());

    return $options;
  }

  /**
   * Displays Rules configuration summary.
   */
  function options_summary(&$categories, &$options) {
    parent::options_summary($categories, $options);

    if ($this->uses_fields() || $entity_info = entity_get_info($this->view->base_table)) {
      // Add Rules category.
      $categories['rules'] = array(
        'title' => t('Rules settings'),
        'column' => 'second',
        'build' => array(
          '#weight' => -10,
        ),
      );

      // Add 'parameter' and 'provides' options.
      if ($this->get_handlers('argument')) {
        $options['rules_parameter'] = array(
          'category' => 'rules',
          'title' => t('Parameters'),
          'value' => t('edit argument info'),
        );
      }
      $options['rules_variables'] = array(
        'category' => 'rules',
        'title' => t('Row variables'),
        'value' => isset($entity_info) ? $entity_info['label'] : t('edit field info'),
      );
    }
  }

  /**
   * Builds display options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    switch ($form_state['section']) {
      case 'rules_parameter':
        $form['#tree'] = TRUE;
        $this->parameter_option_form($form, $form_state);
        break;

      case 'rules_variables':
        $form['#tree'] = TRUE;
        $this->variables_option_form($form, $form_state);
        break;
    }
  }

  /**
   * Validates submitted option values.
   */
  function options_validate(&$form, &$form_state) {
    parent::options_validate($form, $form_state);
    $options = $this->extract_enabled_options($form_state['values']['options']);

    $errors = array();
    switch ($form_state['section']) {
      case 'rules_parameter':
        $errors = $this->parameter_options_validate($options);
        break;

      case 'rules_variables':
        $errors = $this->variables_options_validate($options);
        break;
    }

    foreach ($errors as $error) {
      $element = &$form[$error['id']];
      if (isset($error['element'])) {
        $element = &$element[$error['element']];
      }
      form_error($element, $error['message']);
    }
  }

  /**
   * Consolidates submitted option values.
   */
  function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);

    switch ($form_state['section']) {
      case 'rules_parameter':
      case 'rules_variables':
        $options = $form_state['values']['options'];
        $this->set_option($form_state['section'], $options);
        break;
    }
  }

  /**
   * Validates display options.
   */
  function validate() {
    $errors = parent::validate();

    $options = $this->get_option('rules_parameter');
    $options = $this->extract_enabled_options($options);
    if ($this->parameter_options_validate($options)) {
      $errors[] = t('Rules parameters are not correctly configured.');
    }

    $options = $this->get_option('rules_variables');
    $options = $this->extract_enabled_options($options);
    if ($this->variables_options_validate($options)) {
      $errors[] = t('Rules row variables are not correctly configured.');
    }

    return $errors;
  }

  /**
   * Gets a list of argument labels.
   */
  function get_argument_labels() {
    $options = array();
    foreach ($this->get_handlers('relationship') as $relationship => $handler) {
      $relationships[$relationship] = $handler->label();
    }

    foreach ($this->get_handlers('argument') as $id => $handler) {
      $options[$id] = $handler->ui_name();
      if (!empty($handler->options['relationship']) && !empty($relationships[$handler->options['relationship']])) {
        $options[$id] = '(' . $relationships[$handler->options['relationship']] . ') ' . $options[$id];
      }
    }
    return $options;
  }

  /**
   * Builds parameter option form.
   */
  function parameter_option_form(&$form, &$form_state) {
    $form['#title'] = t('Rules: parameters');
    $form['help'] = array(
      '#prefix' => '<p>',
      '#markup' => t('Configure the variable info for each argument as they would be used as parameters in Rules.'),
      '#suffix' => '</p>',
    );

    // Build variable forms.
    $option_parameter = (array) $this->get_option('rules_parameter');
    foreach ($this->get_argument_labels() as $variable => $label) {
      $option_parameter += array($variable => array());
      $form[$variable] = $this->get_variable_form($variable, $label, $option_parameter[$variable]);
    }
  }

  /**
   * Builds row variables option form.
   */
  function variables_option_form(&$form, &$form_state) {
    $form['#title'] = t('Rules: row variables');

    // Add configuration for fields.
    if ($this->uses_fields()) {
      $form['help'] = array(
        '#prefix' => '<p>',
        '#markup' => t('Configure the variable info for each field as they would be used in Rules.'),
        '#suffix' => '</p>',
      );

      // Build variable forms.
      $option_variables = (array) $this->get_option('rules_variables');
      foreach ($this->get_field_labels() as $variable => $label) {
        $option_variables += array($variable => array());
        $form[$variable] = $this->get_variable_form($variable, $label, $option_variables[$variable], TRUE);
      }
    }
    // Show notice for default entity type.
    elseif ($entity_info = entity_get_info($entity_type = $this->view->base_table)) {
      $form['notice'] = array(
        '#markup' => '<p>' . t('The row style does not use fields. The loop item variable will be the entity variable.') . '</p>',
      );
      $form['variable'] = array(
        '#prefix' => '<p>' . t('Variable details:') . '</p>',
        '#markup' => '<dl>' .
          '<dt>' . t('Type') . '</dt>' .
          '<dd>' . $entity_info['label'] . '</dd>' .
          '<dt>' . t('Label') . '</dt>' .
          '<dd>' . $entity_info['label'] . '</dd>' .
          '<dt>' . t('Name') . '</dt>' .
          '<dd>' . $entity_type . '</dd>' .
          '</dl>',
      );
    }
  }

  /**
   * Builds the form element for a single variable.
   */
  function get_variable_form($name, $label, $info, $optional = FALSE) {
    $info += array(
      'type' => NULL,
      'label' => $label,
      'name' => $name,
      'enabled' => TRUE,
    );

    $form = array(
      '#type' => 'fieldset',
      '#title' => $label,
    );
    if (!$optional) {
      $form['enabled'] = array(
        '#type' => 'value',
        '#value' => TRUE,
      );
    }
    else {
      $enabled_css_id = drupal_html_id('views-rules-variable-enabled');
      $form['enabled'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enabled'),
        '#default_value' => $info['enabled'],
        '#description' => t('Uncheck this box to make this variable unavailable for use in Rules.'),
        '#attributes' => array('id' => $enabled_css_id),
      );
    }
    $form['_pre_wrap'] = array(
      '#markup' => '<div class="clearfix">',
    );
    $states = !isset($enabled_css_id) ? NULL : array(
      'visible' => array(
        '#' . $enabled_css_id => array('checked' => TRUE),
      ),
    );
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Data type'),
      '#options' => views_rules_data_type_options(),
      '#default_value' => $info['type'],
      '#required' => empty($optional),
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
      '#states' => $states,
    );
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => $info['label'],
      '#size' => 25,
      '#required' => empty($optional),
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
      '#states' => $states,
    );
    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $info['name'],
      '#size' => 25,
      '#required' => empty($optional),
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
      '#states' => $states,
    );
    $form['_post_wrap'] = array(
      '#markup' => '</div>',
    );

    return $form;
  }

  /**
   * Extracts options configured as enabled.
   */
  function extract_enabled_options($options) {
    $enabled_options = array();
    foreach ($options as $id => $option) {
      if (!empty($option['enabled'])) {
        $enabled_options[$id] = $option;
      }
    }
    return $enabled_options;
  }

  /**
   * Validates parameter options.
   */
  function parameter_options_validate($options) {
    $errors = array();

    if (array_diff_key($this->get_argument_labels(), $options)) {
      $errors[] = t('Some arguments are not configured.');
    }
    $errors = array_merge($errors, $this->validate_machine_name($options));
    $errors = array_merge($errors, $this->validate_unique_names($options));

    return $errors;
  }

  /**
   * Validates row variable options.
   */
  function variables_options_validate($options) {
    $errors = array();

    $options = $this->extract_enabled_options($options);
    $errors = array_merge($errors, $this->validate_machine_name($options));
    $errors = array_merge($errors, $this->validate_unique_names($options));
    // TODO Ensure enabled options are fully specified.

    return $errors;
  }

  /**
   * Validates variable uniqueness.
   */
  function validate_machine_name($options) {
    $errors = array();
    foreach ($options as $id => $info) {
      $name = $info['name'];
      if (!preg_match('!^[a-z0-9_]+$!', $name)) {
        $errors[] = array(
          'id' => $id,
          'element' => 'name',
          'message' => t('The machine-readable name %name contains invalid characters (valid characters are lowercase letters, numbers, and underscores).'),
        );
      }
    }
    return $errors;
  }

  /**
   * Validates variable uniqueness.
   */
  function validate_unique_names($options) {
    $errors = array();
    $names = array();
    foreach ($options as $id => $info) {
      $name = $info['name'];
      if (isset($names[$name])) {
        $errors[] = array(
          'id' => $id,
          'element' => 'name',
          'message' => t('The machine-readable name %name is already taken.', array('%name' => $name)),
        );
      }
      else {
        $names[$name] = $name;
      }
    }
    return $errors;
  }

  /**
   * Gets parameter info for Rules.
   */
  function get_rules_parameter_info() {
    $option_parameter = (array) $this->get_option('rules_parameter');
    $option_parameter = $this->extract_enabled_options($option_parameter);
    $option_parameter = array_intersect_key($option_parameter, $this->get_argument_labels());
    return $this->get_rules_info_from_option($option_parameter);
  }

  /**
   * Gets row variable info for Rules.
   */
  function get_rules_variable_info() {
    // Return configured field variables.
    if ($this->uses_fields()) {
      $option_variables = (array) $this->get_option('rules_variables');
      $option_variables = $this->extract_enabled_options($option_variables);
      $option_variables = array_intersect_key($option_variables, $this->get_field_labels());
      return $this->get_rules_info_from_option($option_variables);
    }
    // Return row variable.
    elseif ($entity_info = entity_get_info($entity_type = $this->view->base_table)) {
      $info = array(
        $entity_type => array(
          'type' => $entity_type,
          'label' => $entity_info['label'],
        ),
      );
      return $info;
    }

    // Return no variable otherwise.
    return array();
  }

  /**
   * Gets processed Rules info from an option value.
   */
  function get_rules_info_from_option($option) {
    $info = array();
    foreach ($option as $var => $var_info) {
      $var_info += array(
        'type' => NULL,
        'name' => NULL,
        'label' => NULL,
      );
      $info[$var_info['name']] = array(
        'type' => $var_info['type'],
        'label' => $var_info['label'],
      );
    }
    return $info;
  }

  /**
   * Executes the iterator display.
   *
   * @param ViewsRulesIterable $iterable
   */
  function execute_iterator($iterable) {
    // TODO
  }
}

/**
 * Interface for a Views iterator.
 */
interface views_rules_iterator {
  /**
   * Executes the iterator display.
   *
   * @param ViewsRulesIterable $iterable
   */
  function execute_iterator($iterator);

  /**
   * Returns parameter info for use with Rules.
   */
  function get_rules_parameter_info();

  /**
   * Returns row variable info for use with Rules.
   */
  function get_rules_variable_info();
}
