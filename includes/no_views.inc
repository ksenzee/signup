<?php
// $Id: no_views.inc,v 1.17 2009/08/10 17:45:47 dww Exp $


/**
 * @file
 * Provides all the code for required UI elements for sites that do
 * not have views.module enabled. If views is enabled, there are
 * default views for all of these things (which are therefore
 * customizable and more powerful) in signup/views/views.inc.
 *
 */

/**
 * Implements hook_block_info().
 *
 * @ingroup signup_core
 *
 * @param $op
 *   The operation that is being requested. This defaults to 'list', which
 *   indicates that the method should return which blocks are available.
 * @param $delta
 *   The specific block to display (the offset into an array).
 *
 * @return
 *   One of two possibilities. The first is an array of available blocks.
 *   The other is an array containing a block.
 */
function signup_block_info() {
  $blocks['current']['info'] = t('Current signups');
  $blocks['recent']['info'] = t('Recent signups');
  return $blocks; 
}

/**
 * Implements hook_block_view().
 */
function signup_block_view($delta = '') {
  switch ($delta) {
    case 'current':
      global $user;
      $titles = signup_list_user_signups($user->uid);
      if (count($titles)) {
        $block['subject'] = t('Current signups');
        $block['content'] = theme('item_list', array('items' => $titles)) . l(t('View signup schedule'), "user/$user->uid/signups");
        return $block;
      }
      break;
    case 'recent':
      $block['subject'] = t('Recent signups');

      $sql = "SELECT n.nid, n.title, s_l.sid, u.name, u.uid, s_l.form_data FROM {node} n INNER JOIN {signup_log} s_l ON n.nid = s_l.nid INNER JOIN {users} u ON s_l.uid = u.uid ORDER BY n.nid";
      $signups = db_query($sql);
      if ($signups) {
        $block['content'] = theme('signups_recent_block', array('signups' => $signups));
      }
      else {
        $block['content'] = t('No signups.');
      }
      return $block;
      break;
  }
}

/**
 * Private helper as a partial implementation of hook_user_view().
 *
 * @see signup_user_view()
 */
function _signup_user_view_no_views(&$account, $view_mode) {
  // grab list of nodes the user signed up for.
  $signups = signup_list_user_signups($account->uid);
  if (count($signups) && variable_get('signup_no_views_user_info', TRUE)) {
    $account->content['signup'] = array(
      '#type' => 'user_profile_category',
      '#attributes' => array('class' => 'signup'),
      '#weight' => 5,
      '#title' => t('Signup information'),
    );
    $account->content['signup']['current'] = array(
      '#type' => 'user_profile_item',
      '#attributes' => array('class' => 'signup-current'),
      '#title' => t('Current signups'),
      '#value' => theme_item_list($signups),
      '#weight' => 0,
    );
    $account->content['signup']['schedule'] = array(
      '#type' => 'user_profile_item',
      '#attributes' => array('class' => 'signup-schedule'),
      '#title' => t('Signup schedule'),
      '#value' => l(t('View full signup schedule'), 'user/' . $account->uid . '/signups'),
      '#weight' => 2,
    );
  }
}

/**
 * Add menu items we only need to define if views is not enabled.
 */
function signup_no_views_menu(&$items) {
  // User signup schedule callback
  $items['user/%user/signups'] = array(
    'title' => 'Signups',
    'page callback' => 'signup_user_schedule',
    'page arguments' => array(1),
    'type' => MENU_CALLBACK,
    'access callback' => '_signup_no_views_user_menu_access',
    'access arguments' => array(1),
    'file' => 'includes/no_views.inc',
  );
}

function _signup_no_views_user_menu_access($account) {
  return variable_get('signup_no_views_user_info', TRUE) && _signup_user_menu_access($account);
}

/**
 * Print a schedule of the given user's signups.
 *
 * @ingroup signup_callback
 */
function signup_user_schedule($account) {
  $output = '';
  drupal_set_title(t('Signups for @user', array('@user' => $account->name)), PASS_THROUGH);
  $titles = signup_list_user_signups($account->uid);
  foreach ($titles as $nid => $title) {
    $node = node_load($nid);
    $output .= theme('signup_user_schedule', array('node' => $node));
  }
  return $output;
}

/**
 * Return an array of all nodes the specified user has signed up for.
 *
 * @param $uid
 *   The ID of the user to generate a list of signups for.
 *
 * @return
 *   Array of all nodes the given user has signed up for. The array is indexed
 *   by node ID, and contains titles as links to each node.
 */
function signup_list_user_signups($uid) {
  $titles = array();

  // We don't want to return anything for anon users.
  if ($uid != 0) {
    $sql = "SELECT n.nid, n.title FROM {node} n INNER JOIN {signup_log} s_l ON n.nid = s_l.nid WHERE s_l.uid = :uid ORDER BY n.nid";
    // TODO: add back the equivalent of db_rewrite_sql().
    $result = db_query($sql, array(':uid' => $uid));
    foreach ($result as $node) {
      $titles[$node->nid] = l($node->title, 'node/' . $node->nid);
    }
  }
  return $titles;
}

function theme_signups_recent_block($variables) {
  $signups = $variables['signups'];
  $output = '';
  if (empty($signups)) {
    return $output;
  }
  $l_options = array('query' => drupal_get_destination());
  foreach ($signups as $signup) {
    $form_data = unserialize($signup->form_data);
    $row = array();
    $row[] = array(
      'data' => '<div>' . l($signup->title, 'node/' . $signup->nid . '/signups') . '</div><div>' . check_plain($form_data['Name']). '</div>',
    );
    $row[] = array(
      'data' => l(t('edit'), 'signup/edit/' . $signup->sid, $l_options),
      'class' => 'edit',
    );
    $token = signup_get_token($signup->sid, 'cancel');
    $row[] = array(
      'data' => l(t('cancel'), 'signup/cancel/' . $signup->sid . '/' . $token, $l_options),
      'class' => 'cancel',
    );
    $rows[] = $row;
  }

  if ($rows) {
    $output = theme('table', array('rows' => $rows));
  }

  return $output;
}
