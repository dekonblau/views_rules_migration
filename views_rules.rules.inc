<?php
/**
 * @file
 * Provides Rules plugins for using Views.
 */

/**
 * Implements hook_rules_plugin_info().
 */
function views_rules_rules_plugin_info() {
  return array(
    'views loop' => array(
      'class' => 'ViewsRulesLoop',
      'embeddable' => 'RulesActionContainer',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'ViewsRulesLoopUI',
        ),
      ),
    ),
  );
}
