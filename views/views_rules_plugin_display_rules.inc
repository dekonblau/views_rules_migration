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
      'value' => t('edit'),
    );
  }

  /**
   * Builds display options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    switch ($form_state['section']) {
      case 'parameter':
        $this->parameter_option_form($form, $form_state);
        break;

      case 'variables':
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

    // TODO
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
      // TODO Add form for each view field.
    }
    // Add configuration for entity types.
    elseif (entity_get_info($this->view->base_table)) {
      // TODO Add form for a default row variable.
    }
  }

  /**
   * Builds form for a variable.
   */
  function get_variable_form($name, $settings = array()) {
    // TODO
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
    // TODO
    return array();
  }
}
