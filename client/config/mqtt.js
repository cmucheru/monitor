import mqtt from 'mqtt';

const URL = 'mqtt://localhost:1883';

export default async () => {
	const client = mqtt.connect(URL);

	client.on('connect', (connack) => {
		console.log('client connected', connack);
	});
	return client;
};

// //cc this one
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
// 		

// }
// 	);
// });
