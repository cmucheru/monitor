"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _socket = require("socket.io");

var _mqtt = _interopRequireDefault(require("../../config/mqtt"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

var SocketHandler = function SocketHandler(req, res) {
  var client, io;
  return regeneratorRuntime.async(function SocketHandler$(_context) {
    while (1) {
      switch (_context.prev = _context.next) {
        case 0:
          _context.next = 2;
          return regeneratorRuntime.awrap((0, _mqtt["default"])());

        case 2:
          client = _context.sent;

          if (res.socket.server.io) {
            console.log('Websocket is already running');
          } else {
            console.log('Websocket is initializing');
            io = new _socket.Server(res.socket.server);
            res.socket.server.io = io;
            io.on('connection', function (socket) {
              socket.on('telemetry', function (msg) {
                console.log(msg);
              });
              socket.on('ignite', function (payload) {
                console.log('ignition payload', payload);
                client.publish('esp32/ignition', payload, {
                  qos: 1,
                  retain: false
                }, function (err, packet) {
                  if (err) {
                    console.log(err, 'Ignition MQTT publish error');
                  }

                  console.log(packet, 'Ignition MQTT publish success');
                });
              });
              socket.on('eject', function (payload) {
                console.log('eject payload', payload);
                client.publish('esp32/ejection', payload, {
                  qos: 1,
                  retain: false
                }, function (err, packet) {
                  if (err) {
                    console.log(err, 'Ejection MQTT publish error');
                  }

                  console.log(packet, 'Ejection MQTT publish success');
                });
              }); // socket.on('eject2', (payload) => {
              // 	console.log('eject2 payload', payload);
              // 	client.publish(
              // 		'esp32/ejection2',
              // 		payload,
              // 		{ qos: 1, retain: false },
              // 		(err, packet) => {
              // 			if (err) {
              // 				console.log(err, 'Ejection2 MQTT publish error');
              // 			}
              // 			console.log(packet, 'Ejection2 MQTT publish success');
              // 		}
              // 	);
              // });
            });
          }

          res.end();

        case 5:
        case "end":
          return _context.stop();
      }
    }
  });
};

var _default = SocketHandler;
exports["default"] = _default;