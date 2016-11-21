var addon = require('./main');

var myObject = new addon();
myObject.startLoop();

module.exports = function(RED) {
    function KaaNotificationNode(config) {
        RED.nodes.createNode(this,config);
        var node = this;
        this.on('input', function(msg) {
            msg.payload = addon.hello();
            node.send(msg);
        });
        myObject.on("someEvent", function(data) {
            var msg = {payload : ""};
            msg.payload = data;
            node.send(msg);
            console.log(msg);
        });
    }
    RED.nodes.registerType("kaa-notification",KaaNotificationNode);
}
//setInterval(() => {}, 2000);

