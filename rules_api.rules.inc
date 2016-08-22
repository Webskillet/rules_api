<?php

/**
 * implements hook_rules_action_info
 * creates custom action for rules
 */
function rules_api_rules_action_info() {

	return array(
		'rules_api_rules_make_request' => array(
			'label' => t('Make an external HTTP request'),
			'group' => t('API Hook'),
			'parameter' => array(
				'url' => array(
					'label' => t('URL'),
					'type' => 'text',
				),
				'method' => array(
					'label' => t('HTTP method to use'),
					'type' => 'text',
					'options list' => '_rules_api_rules_http_methods',
					'default value' => 'POST',
				),
				'headers' => array(
					'label' => t('Headers to send with request'),
					'type' => 'text',
					'optional' => true,
				),
				'data' => array(
					'label' => t('Data to send with request'),
					'type' => 'text',
					'description' => t('Supports tokens'),
				),
				'log_call' => array(
					'label' => t('Log API calls to Drupal Watchdog?'),
					'type' => 'boolean',
					'default value' => false,
				),
			),
			'provides' => array(
				'status_code' => array(
					'type' => 'integer',
					'label' => t('Status Code'),
				),
				'status_message' => array(
					'type' => 'text',
					'label' => t('Status Message'),
				),
				'error' => array(
					'type' => 'text',
					'label' => t('Error Message'),
				),
				'data' => array(
					'type' => 'text',
					'label' => t('Data'),
				),
			),
			'access callback' => '_rules_api_access_callback',
		),
	);

}

/**
 * callback function for making an http request
 */
function rules_api_rules_make_request($url, $method, $headers, $data, $log_call) {

	$return = array('status_code' => null, 'status_message' => null, 'error' => null, 'data' => null);

	if (!valid_url($url, true)) {
		$return['error'] = 'Invalid URL';
		return $return;
	}

	$headers_options = _rules_api_parse_headers($headers);

	$result = drupal_http_request($url, array('headers' => $headers_options, 'method' => $method, 'data' => $data));

	$return['status_code'] = isset($result->code) ? $result->code : null;
	$return['status_message'] = isset($result->status_message) ? $result->status_message : null;
	$return['error'] = isset($result->error) ? $result->error : null;
	$return['data'] = isset($result->data) ? $result->data : null;

	if ($log_call) {
		$log = $return;
		$log['data_returned'] = $log['data'];
		$log = array_merge($log, array(
			'hook' => 'hook_rules_action',
			'url' => $url,
			'method' => $method,
			'headers' => $headers,
			'data' => $data,
		));
		_rules_api_log_request($log);
	}

	return $return;

}

/**
 * helper function to provide list of HTTP methods
 */
function _rules_api_rules_http_methods() {
	$methods = array(
		'POST' => 'POST',
		'GET' => 'GET',
		'PUT' => 'PUT',
		'DELETE' => 'DELETE',
		'HEAD' => 'HEAD',
		'OPTIONS' => 'OPTIONS',
		'TRACE' => 'TRACE',
		'CONNECT' => 'CONNECT'
	);

	return $methods;
}

/**
 * helper function to provide access control
 */

function _rules_api_access_callback() {
	return user_access('administer rules api');
}

/**
 * helper function to parse headers from text to array
 */
function _rules_api_parse_headers($headers_text) {
	$headers_arr = explode("\n", $headers_text);
	$headers = array();
	foreach ($headers_arr as $header_line) {
		$header_components = explode(":", $header_line);
		if (count($header_components) < 2) { continue; }
		$header_header = trim(array_shift($header_components));
		if ($header_header) {
			$headers[$header_header] = trim(implode(":",$header_components));
		}
	}
	return $headers;
}

/**
 * helper function to log requests
 */
function _rules_api_log_request($log) {

	$object = new stdClass();
	$object->timestamp = time();
	$fields = array('hook','url','method','headers','data','callback','status_code',
		'status_method','error','data_returned');
	foreach ($fields as $field) {
		$object->$field = isset($log[$field]) ? $log[$field] : null;
	}

	watchdog('rules_api', '<pre>'.print_r($object, 1).'</pre>');
}
