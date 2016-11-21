var addon = require('bindings')('kaa.node')

console.log('This should be eight:', addon.sendData("This is hello world test log"))
