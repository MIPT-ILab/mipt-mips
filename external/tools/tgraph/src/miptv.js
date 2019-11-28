export default function loadConfig() {
    const config = {};
    $.getJSON("topology.json", function(json) {
        console.log(json);
        loadModules(config, json.modulemap);
    })
}

function loadModules(config, modulemap) {
    //console.log(Object.entries(modulemap));
    for (let [ key, value ] of Object.entries(modulemap)) {
        console.log(key, value);
    }
};