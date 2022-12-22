const mqtt = require('mqtt');

module.exports = () => {
	//mqtt client
	// const client = mqtt.connect(
	// 	process.env.BROKER_URL || 'mqtt://localhost:1883'
	// );

	var client  = mqtt.connect('mqtt://broker.mqttdashboard.com',{

    });

	// connect to same client and subscribe to same topic name
	client.on('connect', (connack) => {
		console.log('client connected', connack);
		// can also accept objects in the form {'topic': qos}
		client.subscribe('esp32/#', (err, granted) => {
			if (err) {
				console.log(err, 'err');
			}
			console.log(granted, 'granted');
		});

		// socket.on('ignite', (payload) => {
		// 	console.log('ignition payload', payload);
		// 	client.publish(
		// 		'esp32/ignition',
		// 		payload,
		// 		{ qos: 1, retain: false },
		// 		(PacketCallback, err) => {
		// 			if (err) {
		// 				console.log(err, 'MQTT publish packet');
		// 			}
		// 		}
		// 	);
		// });
		// socket.on('eject', (payload) => {
		// 	console.log('eject payload', payload);
		// 	client.publish(
		// 		'esp32/ejection',
		// 		payload,
		// 		{ qos: 1, retain: false },
		// 		(PacketCallback, err) => {
		// 			if (err) {
		// 				console.log(err, 'MQTT publish packet');
		// 			}
		// 		}
		// 	);
		// });
	});

	client.on('message', (topic, message, packet) => {
		console.log(packet, packet.payload.toString());

		if (topic === 'esp32/altitude') {
			console.log('altitude', JSON.parse(altitude));
				socket.emit('altitude', JSON.parse(altitude));
		}

		////
		if (topic === 'esp32/humidity') {
			console.log('humidity', JSON.parse(humidity));
				socket.emit('humidity', JSON.parse(humidity));
		}
		if (topic === 'esp32/longitude') {
			console.log('longitude', JSON.parse(longitude));
				socket.emit('longitude', JSON.parse(longitude));
		}
	});
};

