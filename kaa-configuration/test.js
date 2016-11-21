var addon = require('./main');
var myObject = new addon();

myObject.on("kaaConfig", a => console.log("KaaConfig data received : " + a));
myObject.startLoop();
setInterval(() => {}, 5000);
