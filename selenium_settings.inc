<?php
/**
 * @file
 * Selenium settings.
 */

/**
 * Personal environmental setting.
 * Create a file with the following name: local.selenium_settings.php
 * And define these settings:
 *
 * define('SELENIUM_SETTINGS_BASE_URL', '');
 * define('SELENIUM_SETTINGS_BROWSER', '*firefox');
 * define('SELENIUM_SETTINGS_ADMIN_USERNAME', '');
 * define('SELENIUM_SETTINGS_ADMIN_PASSWORD', '');
 *
 * Then call:
 * $ phpunit TESTFILE.inc
 */

if (file_exists($local_settings_filename = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'local.selenium_settings.php')) {
  require_once $local_settings_filename;
}
// Search a directory up.
else if (file_exists($local_settings_filename = dirname(__FILE__) . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . 'local.selenium_settings.php')) {
  require_once $local_settings_filename;
}
