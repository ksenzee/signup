<?php
// $Id: node.inc,v 1.3 2009/01/22 18:59:29 dww Exp $


/**
 * @file
 * Theme functions when viewing a signup-enabled node.
 */

/**
 * Controls the output displayed if this node is closed for signups.
 *
 * @param $variables
 *   An array of variables containing:
 *   - 'node': The fully loaded node object.
 *   - 'current_signup': If the user already signed up, an HTML representation
 *     of their current signup information, otherwise an empty string.
 *
 * @return
 *   Themed output to display for a node with closed signups.
 *
 * @see _signup_node_output()
 * @see _signup_print_current_signup()
 */
function theme_signup_signups_closed($variables) {
  $node = $variables['node'];
  $current_signup = $variables['current_signup'];
  $output = '<h3>' . t('Signups closed for this %node_type', array('%node_type' => node_type_get_name($node->type))) . '</h3>';
  $output .= $current_signup;
  return $output;
}

/**
 * Controls the output for anonymous users who can't signup.
 *
 * @param $variables
 *   An array of variables containing:
 *   - 'anon_login_text': The translated HTML help text telling users to login
 *     (and if allowed on this site, register) so they can signup, including
 *     login/register links.
 *
 * @return
 *   The themed HTML to display the login (and maybe register) help text.
 */
function theme_signup_anonymous_user_login_text($variables) {
  if (!empty($variables['anon_login_text'])) {
    return '<div class="signup_anonymous_login">' . $variables['anon_login_text'] . '</div>';
  }
}

/**
 * Return HTML desired at the top of the signup output for a node.
 *
 * @param $variables
 *   An array of variables containing:
 *   - 'node': The fully loaded node object to generate a header for.
 *
 * @return
 *   HTML to display at the top of the signup output.
 *
 * @see _signup_node_output()
 */
function theme_signup_node_output_header($variables) {
  return '<a name="signup"></a>';
}
