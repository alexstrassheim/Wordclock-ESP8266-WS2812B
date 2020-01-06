$(document).ready(function(){

	// Settings
	var host = window.location.hostname;
	var ws_url = 'ws://' + host + ':81';
	var connection;
	var ws_waiting = 0;

	// https://iro.js.org/
    var colorPicker = new iro.ColorPicker("#color-picker-container",{
        width: 280,
        color: "#fff",
        borderColor: "#fff",
        borderWidth: 1.2,
        padding: 8,
        handleRadius: 10,
        sliderMargin: 80,
    });

    function onColorChange(color, changes) {
        // print the color's new hex value to the developer console
		// console.log(color.hexString);
		wsSetall(color.hexString);
    }
  
    // listen to a color picker's color:change event
    colorPicker.on('color:change', onColorChange);    

	console.log("Connection websockets to:", ws_url);
	connection = new WebSocket(ws_url, ['arduino']);

	connection.onopen = function () {
		console.log('WebSocket Open');
	};

	// Log errors
	connection.onerror = function (error) {
		console.log('WebSocket Error ' + error);

	};
	
	connection.onclose = function () {
		console.log('WebSocket Closed');
		setTimeout(function() {
			location.reload();
		  }, 1000);
	};

	// Log messages from the server
	connection.onmessage = function (e) {
		console.log('WebSocket from server: ' + e.data);
		ws_waiting = 0;
	};

	// ******************************************************************
	// WebSocket commands
	// ******************************************************************
	function wsSendCommand(cmd) {
		console.log("Send WebSocket command:", cmd);
		if (ws_waiting == 0)  {
			connection.send(cmd);
			ws_waiting++;
		} else {
			console.log("WS call waiting, skip")
		}
	}	
	
	function wsSetall(hexColor) {
		// console.log("wsSetAll() Set all colors to:", hexColor);
		wsSendCommand(hexColor);
	}

  });