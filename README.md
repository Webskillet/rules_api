# Rules API Module

Version 7.x-1.0-beta

Drupal 7 module, provides a Rules action to make an external API call to any arbitrary URL using [drupal_http_request](https://api.drupal.org/api/drupal/includes%21common.inc/function/drupal_http_request/7.x).

## Warning

Users with "Administer Rules API" permissions can make arbitrary http requests to external websites, so grant this permission with extreme caution.

## Configuration

Rules variables can be inserted into the data which is passed to the request (so it can be JSON, XML, or a string key-value pairs). HTTP method defaults to POST, but can be configured to any valid HTTP method. Headers can also be configured for authentication (or any other purpose). There is also a switch on each rule to log calls and reponses to Drupal watchdog, for debugging (this should be turned off on production sites).

Developed by [Jonathan Kissam](http://jonathankissam.com)
