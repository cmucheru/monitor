const express = require('express');
const router = express.Router();

router.get('/', function (req, res) {
	res.send('welcome  to monitoring subsystem');
});

module.exports = router;
