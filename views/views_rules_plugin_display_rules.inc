<?php
/**
 * @file
 * Configures views for use within Rules.
 */

/**
 * Rules display plugin.
 */
class views_rules_plugin_display_rules extends views_plugin_display {
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

    // TODO
  }

  /**
   * Consolidates submitted option values.
   */
  function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);

    switch ($form_state['section']) {
      case 'rules_parameter':
      case 'rules_variables':
        $option = $form_state['values']['options'];
        $this->set_option($form_state['section'], $option);
        break;
    }
  }

  /**
   * Validates display options.
   */
  function validate() {
    $errors = parent::validate();

    // TODO

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
        $form[$variable] = $this->get_variable_form($variable, $label, $option_variables[$variable]);
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
    );

    $form = array(
      '#type' => 'fieldset',
      '#title' => $label,
    );
    $form['_pre_wrap'] = array(
      '#markup' => '<div class="clearfix">',
    );
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Data type'),
      '#options' => views_rules_data_type_options(),
      '#default_value' => $info['type'],
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
    );
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => $info['label'],
      '#size' => 25,
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
    );
    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $info['name'],
      '#size' => 25,
      '#prefix' => '<div class="views-left-30">',
      '#suffix' => '</div>',
    );
    $form['_post_wrap'] = array(
      '#markup' => '</div>',
    );

    return $form;
  }

  /**
   * Validates parameter options.
   */
  function parameter_options_validate($options) {
    // TODO
    return FALSE;
  }

  /**
   * Validates parameter options.
   */
  function variables_validate_options($options) {
    // TODO
    return FALSE;
  }

  /**
   * Gets parameter info for Rules.
   */
  function get_rules_parameter_info() {
    // TODO
    return array();
  }

  /**
   * Gets row variable info for Rules.
   */
  function get_rules_variable_info() {
    // Return configured field variables.
    if ($this->uses_fields()) {
      $option_variables = (array) $this->get_option('rules_variables');
      $info = array();
      foreach ($this->get_field_labels() as $variable => $label) {
        if (isset($option_variables[$variable])) {
          $info[$variable] = $option_variables[$variable] + array(
            'label' => $label,
          );
        }
      }
      return $info;
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
}

/**
 * Validates a variable machine name.
 */
function views_rules_element_machine_name_validate($element, &$form_state) {
  module_load_include('inc', 'rules', 'ui/ui.forms');
  rules_ui_element_machine_name_validate($element, $form_state);
}
