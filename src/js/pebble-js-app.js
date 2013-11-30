var vTemperature = "-";
var vConso = "-";
var vConsoHp = "0";
var vAlarm = "-";
var vExternalTemperature = "-";
var vWsURLBase = 'http://xx.xx.xx.xx/ws.php?username=[username]&password=[password]&method=';
var configUrl = 'https://xx.xx.xx.xx/hautomation-conf.html';
var user = '';
var password = '';
var vWsURL = '';

/*******
 * Pebble events
 *******/

Pebble.addEventListener("ready",
    function(e) {
    	console.log("Starting WS !");
    	user = localStorage.getItem("user");
    	password = localStorage.getItem("password");
    	refresh();							
    }
    
    
);

Pebble.addEventListener("appmessage", function(e) {
			
			console.log("Received message: " + e.payload.dummy);
			if (e.payload.sending == "alOn") {
				setAlarm("on");
			}

			if (e.payload.sending == "alOff") {
				setAlarm("off");
			}

			if (e.payload.sending == "ref") {
				refresh();
			}
		}
);

Pebble.addEventListener("showConfiguration", function(e) {
		console.log("Conf showed");
		Pebble.openURL(configUrl+"?username="+user+"&password="+password);
	}
);

Pebble.addEventListener("configurationClosed", function(e) {
		refresh();
	}
);

Pebble.addEventListener("webviewclosed", function(e) {
    var response = JSON.parse(e.response);
    if (response) {
    	localStorage.setItem("user", response.user);
    	localStorage.setItem("password", response.password);
    	user = localStorage.getItem("user");
    	password = localStorage.getItem("password");
    };

    console.log(response);
	refresh();
  }
);


/*******
 * JS Methods
 *******/

function sendError(err) {
	console.log("Error : "+err);
	Pebble.sendAppMessage({ "error":err});
}

function defineWsUrl() {
	vWsURL = vWsURLBase.replace("[username]",user);
	vWsURL = vWsURL.replace("[password]",password);
}

function refresh() {
	defineWsUrl();
	getLogin();
}

function getLogin() {
	// Login
	console.log(vWsURL+'setLogin');
	var reqLogin = new XMLHttpRequest();
	  reqLogin.open('GET', vWsURL+'setLogin', true);
	  failed = true;
	  reqLogin.onload = function(e) {
	    if (reqLogin.readyState == 4 && reqLogin.status == 200) {
	      if(reqLogin.status == 200) {
			failed = false;   
	      }
	    }

	    if (failed) {
	  		sendError("Auth. failed !");
		  } else {
		  	getTemperature();
		 }
	  }

	  
	  reqLogin.send(null);
}

function getTemperature() {
	// Temperature
		var reqTemperature = new XMLHttpRequest();
		  reqTemperature.open('GET', vWsURL+'getTemperature', true);
		  reqTemperature.onload = function(e) {
		    if (reqTemperature.readyState == 4 && reqTemperature.status == 200) {
		      if(reqTemperature.status == 200) {
				console.log(" - Temp OK");
		        var response = JSON.parse(reqTemperature.responseText);
		        var temperature = response.temperature;
		        vTemperature = temperature + "";
		      } else { 
		      	console.log("Error");
		      	sendError("Error");
		      }
		    }
		    getConso();
		  }
		  reqTemperature.send(null);
}

function getConso() {
	// Conso Electrique
        var reqConso = new XMLHttpRequest();
		  reqConso.open('GET', vWsURL+'getCurrentTeleinfo', true);
		  reqConso.onload = function(e) {
		    if (reqConso.readyState == 4 && reqConso.status == 200) {
		      if(reqConso.status == 200) {
				console.log(" - Conso OK");
		        var response = JSON.parse(reqConso.responseText);
		        if (response.length > 0) {
			        var conso = response[0].consoWatts;
			        var mode = response[0].mode;
					vConso = conso + " w";
					if (mode == 'HP') vConsoHp = "1";
				}
		        //Pebble.sendAppMessage({ "3":mode});

		      } else { console.log("Error"); }
		    }
		    getAlarm();
		  }
		  reqConso.send(null);
}

function getAlarm() {
	// Alarme

        var reqAlarm = new XMLHttpRequest();
		  reqAlarm.open('GET', vWsURL+'getAlarm', true);
		  reqAlarm.onload = function(e) {
		    if (reqAlarm.readyState == 4 && reqAlarm.status == 200) {
		      if(reqAlarm.status == 200) {
				console.log(" - Alarm OK");
		        var response = reqAlarm.responseText;
		        if (response == "true") {
		        	vAlarm = "ON";
		        } else {
		        	vAlarm = "OFF";
		        }
		        //Pebble.sendAppMessage({ "3":mode});
		      } else { console.log("Error"); }
		    }
		    getExtTemperature();
		  }
		  reqAlarm.send(null);
}

function getExtTemperature() {
	// Alarme
        var reqExtTemperature = new XMLHttpRequest();
		  reqExtTemperature.open('GET', vWsURL+'getWeather', true);

		  reqExtTemperature.onload = function(e) {
		    if (reqExtTemperature.readyState == 4 && reqExtTemperature.status == 200) {
		      if(reqExtTemperature.status == 200) {
				console.log(" - External temp OK");
		        var response = JSON.parse(reqExtTemperature.responseText);
		        vExternalTemperature = Math.round(response.main.temp) + "";

		      } else { console.log("Error"); }
		    }
		    sendMessageToPebble();
		  }
		  reqExtTemperature.send(null);
}

function sendMessageToPebble() {
	console.log('{ "1":'+vTemperature+', "2":'+vConso+', "3":'+vAlarm+', "4":'+vExternalTemperature+', "5": '+vConsoHp+'}');
	Pebble.sendAppMessage({ "temperature":vTemperature, "conso":vConso, "alarm":vAlarm, "extTemperature":vExternalTemperature, "consoHp": vConsoHp});
}

function setAlarm(status) {
	// Alarme
        var reqAlarm = new XMLHttpRequest();
		  reqAlarm.open('GET', vWsURL+'setAlarm&status='+status, true);
		  reqAlarm.onload = function(e) {
		    if (reqAlarm.readyState == 4 && reqAlarm.status == 200) {
		      if(reqAlarm.status == 200) {
		      } else { console.log("Error"); }
		    }
			getTemperature();
		  }
		  reqAlarm.send(null);
}