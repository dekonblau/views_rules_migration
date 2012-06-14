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

    $options['arguments'] = array('default' => array());
    $options['variables'] = array('default' => array());

    return $options;
  }

  /**
   * Displays Rules configuration summary.
   */
  function options_summary(&$categories, &$options) {
    parent::options_summary($categories, $options);

    // TODO
  }

  /**
   * Builds display options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }

  /**
   * Consolidates submitted option values.
   */
  function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);
  }

  /**
   * Validates display options.
   */
  function validate(&$form, &$form_state) {
    $errors = parent::validate();

    // TODO

    return $errors;
  }
}
