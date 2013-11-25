<?php
/**
 * @file
 * Selenium base file.
 *
 * Set settings for your environment in selenium_settings.inc!
 */

require_once 'PHPUnit/Extensions/SeleniumTestCase.php';

/**
 * Expect error message on page load.
 * @see PxSeleniumTestCase::waitForPageToLoad().
 */
define('SELENIUM_EXPECT_ERROR', TRUE);

/**
 * Default Drupal roles.
 */
define('PX_SELENIUM_ROLE_ANONYMOUS_USER', 1);
define('PX_SELENIUM_ROLE_AUTHENTICATED_USER', 2);

/**
 * Singleton class for storing environment variables about the selenium
 * instance.
 */
class LocalSeleniumSettings {
  public $base_url = '';
  public $admin_username = '';
  public $admin_password = '';
  public $browser = '*firefox';

  private static $instance;

  /**
   * Get local settings instance.
   */
  public static function getInstance() {
    if (!self::$instance) {
      self::$instance = new LocalSeleniumSettings();
    }

    return self::$instance;
  }

  /**
   * Constructor.
   *
   * Read selenium settings from selenium_settings.inc.
   */
  public function __construct() {
    include 'selenium_settings.inc';

    $this->base_url = $base_url;
    $this->admin_username = $admin_username;
    $this->admin_password = $admin_password;
  }
}

abstract class PxSeleniumTestCase extends PHPUnit_Extensions_SeleniumTestCase {

  /**
   * Constructor
   *
   * Initialize environment variables.
   * @see LocalSeleniumSettings
   */
  public function __construct($name = NULL, array $data = array(), $dataName = '', array $browser = array()) {
    parent::__construct($name, $data, $dataName, $browser);

    $local_selenium_settings = LocalSeleniumSettings::getInstance();
    $settings = array(
      'browser',
      'base_url',
      'admin_username',
      'admin_password',
    );

    foreach ($settings as $setting) {
      $this->{$setting} = $local_selenium_settings->{$setting};
    }

    $this->setBrowserUrl($this->base_url);
    $this->setBrowser($this->browser);
  }

  /**
   * Override open() function to support base_url.
   */
  protected function open($url) {
    if (!empty($this->base_url)) {
      parent::open($this->base_url . '/' . $url);
    }
    else {
      parent::open($url);
    }
  }

  /**
   * Override function to check for Drupal error messages.
   *
   * @param int $timeout
   *   Timeout interval in milliseconds.
   * @param bool $expectError
   *   (Optional) Expect error on page load.
   *   Set to true when the error message is intentional, e.g.: testing failed
   *   user login.
   */
  protected function waitForPageToLoad($timeout, $expectError = FALSE) {
    parent::waitForPageToLoad($timeout);
    if (!$expectError) {
      $this->assertElementNotPresent("css=div.messages.warning", 'No warning message found on page');
      $this->assertElementNotPresent("css=div.messages.error", 'No error message found on page');
    }
  }

  /**
   * Helper function to log in as admin.
   */
  protected function adminLogin() {
    $this->login($this->admin_username, $this->admin_password);
  }

  /**
   * Creates a Drupal user account with the specified roles.
   *
   * @param array $roles
   *   (Optional) Drupal role ids.
   *   @see PX_SELENIUM_ROLE_AUTHENTICATED_USER
   *
   * @param array $groups
   *   (Optional) Drupal group names.
   *
   * @return object
   *   Account object.
   *   - username
   *   - email
   *   - password
   *   - roles
   *   - groups
   */
  protected function createUser(array $roles = array(), array $groups = array()) {
    $account = new stdClass();
    $account->username = $this->randomString();
    $account->first_name = $this->randomString();
    $account->surname = $this->randomString();
    $account->email = "{$account->username}@test.test";
    $account->password = $this->randomString();
    $account->roles = $roles;
    $account->groups = $groups;

    $this->adminLogin();

    // Create new user with all the roles available.
    $this->open('admin/people/create');
    $this->waitForPageToLoad("30000");
    if ($this->isElementPresent('id=edit-name')) {
      $this->type("id=edit-name", $account->username);
    }
    $this->type("id=edit-mail", $account->email);
    $this->type("id=edit-pass-pass1", $account->password);
    $this->type("id=edit-pass-pass2", $account->password);

    $this->type("id=edit-field-profile-first-name-und-0-value", $account->first_name);
    $this->type("id=edit-field-profile-surname-und-0-value", $account->surname);

    // Add roles.
    if (!empty($roles)) {
      foreach ($roles as $rid) {
        $this->click("id=edit-roles-$rid");
      }
    }

    // Add groups.
    if (!empty($groups)) {
      foreach ($groups as $group) {
        $this->addSelection("id=edit-og-user-node-und-0-default", "label={$group}");
      }
    }

    $this->click("id=edit-submit");
    $this->waitForPageToLoad("30000");

    return $account;
  }

  /**
   * Helper function to log in to Drupal.
   *
   * @param string $username
   *   Drupal username.
   * @param string $password
   *   Password.
   */
  protected function login($username, $password) {
    $this->open('user/logout');
    $this->waitForPageToLoad('30000', SELENIUM_EXPECT_ERROR);
    $this->open('user/login');

    $this->focus('id=edit-name');
    $this->type('id=edit-name', $username);

    $this->focus('id=edit-pass');
    $this->type('id=edit-pass', $password);

    $this->click('id=edit-submit');
    $this->waitForPageToLoad('30000', SELENIUM_EXPECT_ERROR);
  }

  /**
   * Get a random string.
   *
   * @see user_password()
   *
   * @param int $length
   *   Length of the random string to generate.
   *
   * @return string
   *   Random string.
   */
  protected function randomString($length = 10) {
    // This variable contains the list of allowable characters for the
    // password. Note that the number 0 and the letter 'O' have been
    // removed to avoid confusion between the two. The same is true
    // of 'I', 1, and 'l'.
    $allowable_characters = 'abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789';

    // Zero-based count of characters in the allowable list:
    $len = strlen($allowable_characters) - 1;

    // Declare the password as a blank string.
    $pass = '';

    // Loop the number of times specified by $length.
    for ($i = 0; $i < $length; $i++) {

      // Each iteration, pick a random character from the
      // allowable string and append it to the password:
      $pass .= $allowable_characters[mt_rand(0, $len)];
    }

    return $pass;
  }

  /**
   * Implement pause command.
   *
   * This command is not availible in selenium RC.
   * @see https://groups.google.com/forum/?fromgroups=#!topic/selenium-users/bxStWltjBcU
   */
  protected function pause($milliseconds) {
    sleep($milliseconds/1000);
  }

  /**
   * Get the relative url of the page.
   *
   * @return string|bool
   */
  protected function getRelativeUrl() {
    $location = $this->getLocation();
    return preg_replace("#^{$this->base_url}#", '', $location);
  }

  /**
   * Fills in a file upload form.
   *
   * To properly generate the file needed for uploading the selenium server and
   * the running environment has to be on the same server.
   * This restriction does not apply for files existing on the test
   * environment.
   *
   * @param string $selector
   *   Selenium selector.
   * @param string $extension
   *   File extension (without dot).
   * @param string $filename
   *   (Optional) File name to upload.
   *   If not given it will be automatically generated.
   */
  protected function fileUpload($selector, $extension, $filename = NULL) {
    if ($filename === NULL) {
      $filename = $this->randomString();
    }

    $full_filepath = sys_get_temp_dir() . '/' . $filename . '.' . $extension;

    if (!file_exists($full_filepath)) {
      touch($full_filepath);
    }

    $this->type($selector, $full_filepath);
    $this->pause(1000);
  }

  /**
   * Tests that the admin page is not accessible for anonymous users.
   */
  public function testAdminPageIsInaccessibleForAnonymous() {
    $this->open('user/logout');
    $this->waitForPageToLoad('30000', SELENIUM_EXPECT_ERROR);

    $this->open('admin');
    $this->verifyTextPresent('Access denied');
  }
}
