<?php

/**
 * Implements hook_rules_plugin_info().
 */
function views_rules_rules_plugin_info() {
  return array(
    'view loop' => array(
      'class' => 'ViewsRulesLoop',
      'embeddable' => 'RulesActionContainer',
      'extenders' => array(
        'RulesPluginUIInterface' => array(
          'class' => 'ViewsRulesLoopUI',
        ),
        'RulesFactoryUIInterface' => array(
          'class' => 'ViewsRulesLoopFactoryUI',
        ),
      ),
    ),
  );
}
