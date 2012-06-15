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

    $options['parameter'] = array('default' => array());
    $options['variables'] = array('default' => array());

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
      $options['parameter'] = array(
        'category' => 'rules',
        'title' => t('Parameters'),
        'value' => t('edit'),
      );
      $options['variables'] = array(
        'category' => 'rules',
        'title' => t('Row variables'),
        'value' => isset($entity_info) ? $entity_info['label'] : t('edit'),
      );
    }
  }

  /**
   * Builds display options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    switch ($form_state['section']) {
      case 'parameter':
        $form['#tree'] = TRUE;
        $this->parameter_option_form($form, $form_state);
        break;

      case 'variables':
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
      case 'parameter':
      case 'variables':
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
   * Builds parameter option form.
   */
  function parameter_option_form(&$form, &$form_state) {
    // TODO
  }

  /**
   * Builds row variables option form.
   */
  function variables_option_form(&$form, &$form_state) {
    // Add configuration for fields.
    if ($this->uses_fields()) {
      // Build variable forms.
      $option_variables = (array) $this->get_option('variables');
      foreach ($this->get_field_labels() as $variable => $label) {
        $option_variables += array($variable => array());
        $option_variables[$variable] += array(
          'type' => NULL,
        );

        $form[$variable] = array(
          '#type' => 'fieldset',
          '#title' => $label,
        );
        $form[$variable]['type'] = array(
          '#type' => 'select',
          '#title' => t('Variable type'),
          '#options' => views_rules_data_type_options(),
          '#default_value' => $option_variables[$variable]['type'],
        );
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
      $option_variables = (array) $this->get_option('variables');
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
