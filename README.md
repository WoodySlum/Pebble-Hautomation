Pebble-Hautomation
==================

Home automation Pebble sample app

This app is an example for displaying Home Automation on your Pebble.
It's using Pebble SDK 2.0 and is an example for :

* App Message
* JS kit
* Web Services calls
* Custom images using font
* App configuration

This project uses : Font Awesome for Graphics (http://fontawesome.io/) and Pebble Pieces (https://github.com/partlyhuman/PebblePieces) a great lib from PartlyHuman

![](https://dl.dropbox.com/u/9858108/Pebble-Hautomation.JPG)

You can display in / out temperature, in house alarm status, electricity consumption and electricity mode.

**I can't currently provide the backoffice part, unstable and built for my own system.**

###How to use it

1/ you need to copy the hautomation-conf.html file on your WebServer, then in the JS file update the `configUrl` var.

2/ Create a backoffice URL in any language (PHP, JAVA, ...) that implements REST Web Services. In the JS file, update the `vWsURLBase` var with your own URL previously built. Keep the `[username]` and `[password]` var in the url, it will be replaced in the `defineWsUrl` method.

GET Parameters :

* username
* password
* method

POST Parameters :

* username
* password
* method
* status

**PHP Backoffice Code example :**


	<?php
		function getPost($var) {
	      if (isset($_POST[$var])) {
    	    return $_POST[$var];
	      }

    	  if (isset($_GET[$var])) {
		    return $_GET[$var];
    	  }

	      return false;
	   }
   
	   $username = getPost("username");
	   $password = getPost("password");
	   $method = getPost("method");
	   $status = getPost("status");
	   
	   switch ($method) {
	   		case "setLogin":
				...
			break;
			case "getTemperature":
				...
			break;
	   		case "getCurrentTeleinfo":
				...
			break;
			case "getAlarm":
				...
			break;
			case "getWeather":
				...
			break;
			case "setAlarm":
				...
			break;
	   }
	?>

For each method, here is the good return json payload :

* setLogin : `{}`
* getTemperature : `{"temperature":18.8}`
* getCurrentTeleinfo : `[{"consoWatts":"2690","mode":"HP"}]`
* getAlarm : `true`
* getWeather (Retreived from http://api.openweathermap.org/data/2.5/weather?q=Lyon,FR&units=metric) : `{"coord":{"lon":4.85,"lat":45.75},"sys":{"message":0.4377,"country":"FR","sunrise":1385794840,"sunset":1385827102},"weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04d"}],"base":"gdps stations","main":{"temp":4.48,"pressure":1017,"humidity":75,"temp_min":4,"temp_max":5},"wind":{"speed":7.7,"deg":10,"var_beg":350,"var_end":50},"rain":{"3h":0},"clouds":{"all":75},"dt":1385809200,"id":2996944,"name":"Lyon","cod":200}` 
* setAlarm : `{}`



