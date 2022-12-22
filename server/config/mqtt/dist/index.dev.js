"use strict";

var mqtt = require("mqtt");

module.exports = function () {
  //mqtt client
  var client = mqtt.connect(process.env.BROKER_URL || "mqtt://localhost:1883"); // connect to same client and subscribe to same topic name

  client.on("connect", function (connack) {
    console.log("client connected", connack); // can also accept objects in the form {'topic': qos}

    client.subscribe("esp32/#", function (err, granted) {
      if (err) {
        console.log(err, "err");
      }

      console.log(granted, "granted");
    });
    socket.on("eject", function (payload) {
      console.log("eject payload", payload);
      client.publish("esp32/ejection", payload, {
        qos: 1,
        retain: false
      }, function (PacketCallback, err) {
        if (err) {
          console.log(err, "MQTT publish packet");
        }
      });
    });
  });
  client.on("altitude", function (topic, altitude, packet) {
    //cc this one
    console.log(packet, packet.payload.toString());

    if (topic === "esp32/altitude") {
      //esp32/message
      console.log("altitude", JSON.parse(altitude)); //cc this one

      socket.emit("altitude", JSON.parse(altitude));
    }
  }); //get latitude

  client.on("latitude", function (topic, latitude, packet) {
    //cc this one
    console.log(packet, packet.payload.toString());

    if (topic === "esp32/latitude") {
      //esp32/message
      console.log("latitude", JSON.parse(latitude)); //cc this one

      socket.emit("latitude", JSON.parse(latitude));
    }
  }); //get longitude

  client.on("longitude", function (topic, longitude, packet) {
    //cc this one
    console.log(packet, packet.payload.toString());

    if (topic === "esp32/longitude") {
      //esp32/message
      console.log("longitude", JSON.parse(longitude)); //cc this one

      socket.emit("longitude", JSON.parse(longitude));
    }
  });
};