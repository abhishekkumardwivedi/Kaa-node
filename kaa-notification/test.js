var addon = require('./main');

var myObject = new addon();
myObject.on("someEvent", a => console.log("Event emitted from C Land with argument " + a));
myObject.startLoop();

setInterval(() => {}, 5000);
