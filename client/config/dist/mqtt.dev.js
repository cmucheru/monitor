"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _mqtt = _interopRequireDefault(require("mqtt"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

var URL = "mqtt://localhost:1883";

var _callee = function _callee() {
  var client;
  return regeneratorRuntime.async(function _callee$(_context) {
    while (1) {
      switch (_context.prev = _context.next) {
        case 0:
          client = _mqtt["default"].connect(URL);
          client.on("connect", function (connack) {
            console.log("client connected", connack);
          });
          return _context.abrupt("return", client);

        case 3:
        case "end":
          return _context.stop();
      }
    }
  });
}; // socket.on('eject', (payload) => {
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


exports["default"] = _callee;