<?php
/**
 * @file
 * Rules UI implementation for Views Rules plugins.
 */

/**
 * Factory UI for a view loop.
 */
class ViewsRulesLoopFactoryUI extends RulesFactoryUI {
  /**
   * Builds the factory form for creating an instance of the plugin.
   *
   * @param $form
   *   The form array to add the factory form to.
   * @param $form_state
   *   The current form state.
   */
  public function factoryForm(&$form, &$form_state, $options = array()) {
    if (empty($options['init'])) {
      // Add view selector.
      $form['display'] = array(
        '#type' => 'select',
        '#title' => t('Views iterator display'),
        '#options' => views_rules_list_iterators(),
        '#description' => t('Select the view display to use for looping. To add a display to the list, add a "Rules" display to a view.'),
        '#required' => TRUE,
      );
    }
    else {
      // Display view display name.
      /** @var $view view */
      $view = $form_state['rules_factory_created']->getView();
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
  }

  /**
   * Processes the factory form submission as built in
   * RulesFactoryUIInterface::factoryForm(). Set 'rules_factory_created' to a
   * created instance for configuration to begin.
   */
  public function factoryFormSubmit(&$form, &$form_state, $options = array()) {
    $display = $form_state['values']['display'];
    list($viewName, $displayName) = explode(':', $display, 2);
    $form_state['rules_factory_created'] = new ViewsRulesLoop($viewName, $displayName);
  }
}

/**
 * Views loop administrative UI.
 */
class ViewsRulesLoopUI extends RulesActionContainerUI {
  public function operations() {
    $ops = parent::operations();
    if (module_exists('views_ui')) {
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

    // Format label with HTML tags.
    $content['label']['#markup'] = $this->element->label();

    return $content;
  }
}
