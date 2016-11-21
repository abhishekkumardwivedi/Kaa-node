var addon = new require('bindings')('datacollection.node');

module.exports = function(RED) {
    function KaaDataCollectionNode(config) {
        RED.nodes.createNode(this,config);
        var node = this;
        this.on('input', function(msg) {
            addon.sendData(msg.payload);
        });
    }
    RED.nodes.registerType("data-collection",KaaDataCollectionNode);
}
