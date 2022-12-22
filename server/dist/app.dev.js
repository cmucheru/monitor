"use strict";

require('dotenv').config();

var express = require('express');

var app = express();

var http = require('http');

var server = http.createServer(app);

var _require = require('socket.io'),
    Server = _require.Server;

var mqtt = require('./config/mqtt'); // socket.io


var io = new Server(server, {
  cors: {
    origin: process.env.ORIGIN || '*'
  }
}); //routes

app.get('/', function (req, res) {
  res.send('welcome to monitoring susbsystem');
}); // handle  websocket connections

//cc this one
io.on('connection', (socket) => {
	console.log(`user connected: ${socket.id}`);
	socket.on('disconnect', () => {
		console.log(`user has disconnected: ${socket.id}`);
	});
});

mqtt();
var PORT = process.env.PORT || 5000;
server.listen(PORT, function () {
  console.log("listening on *".concat(PORT));
});