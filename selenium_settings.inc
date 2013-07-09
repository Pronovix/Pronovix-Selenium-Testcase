<?php
/**
 * @file
 * Selenium settings.
 */

/**
 * Personal environmental setting.
 * Create a file with the following name: local.selenium_settings.php
 * And add these settings:
 *
 * $base_url = '';
 * $browser = '*firefox';
 * $admin_username = '';
 * $admin_password = '';
 *
 * Then call:
 * $ phpunit TESTFILE.inc
 */

$local_settings_filename = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'local.selenium_settings.php';
if (file_exists($local_settings_filename)) {
  require_once $local_settings_filename;
  exit();
}

// Search a directory up.
$local_settings_filename = dirname(__FILE__) . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . 'local.selenium_settings.php';
if (file_exists($local_settings_filename)) {
  require_once $local_settings_filename;
}
