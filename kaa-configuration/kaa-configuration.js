var addon = require('./main')

var myObject = new addon()
myObject.startLoop()

module.exports = function (RED) {
  function KaaConfigurationNode (config) {
    RED.nodes.createNode(this, config)
    var node = this
    myObject.on('kaaConfig', function (data) {
      var msg = {payload: ''}
      msg.payload = data
      node.send(msg)
      console.log(msg)
    })
  }
  RED.nodes.registerType('kaa-configuration', KaaConfigurationNode)
}
// setInterval(() => {}, 2000)
