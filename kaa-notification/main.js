var addon = require('bindings')('notification').MyObject,
    events = require('events');

inherits(addon, events);

module.exports = addon;

// extend prototype
function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}
