var addon = require('bindings')('configuration').MyObject,
    events = require('events');

inherits(addon, events);

module.exports = addon;

function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}
