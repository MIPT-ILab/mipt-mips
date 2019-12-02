// Copyright 2019 MIPT-MIPS
/*
 * (Type docs)
 * @author Eric Konks konks.em@phystech.edu
*/
var dagre = require('dagre');

class BaseConfig {
    constructor(data, graph, instance, moduleWidth, moduleHeight, modulesLayout) {
        this.data = data;
        this.graph = graph;
        this.instance = instance;
        this.moduleWidth = moduleWidth;
        this.moduleHeight = moduleHeight;
        this.modulesLayout = modulesLayout;
    }

    modulesWithReadPort(portName, mName) {
        const modules = [];
        for (const [moduleName, ports] of Object.entries(this.data.modules)) {
            if (moduleName == mName || ports.read_ports =='') {
                continue;
            }
            if (portName in ports.read_ports && moduleName !== mName) {
                modules.push(moduleName);
            }
        }
        return modules;
    }

    initModule(el) {
        this.instance.draggable(el);
        this.instance.makeSource(el, {
            filter: ".ep",
            anchor: "Continuous",
            connectorStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 },
            connectionType:"basic0.5",
            extract:{
                "action":"the-action"
            },
        });
        this.instance.makeTarget(el, {
            dropOptions: { hoverClass: "dragHover" },
            anchor: "Continuous",
            allowLoopback: true
        });
    }

    initParent(module) {
        this.instance.addGroup({
            el: module,
            id: `${module.id}_group`,
            constrain: true,
            anchor:"Continuous",
            endpoint:[ "Dot", { radius:3 } ],
            droppable: false,
            ghost: true,
        });
    }

    newModule(name) {
        const d = document.createElement("div");
        const id = name;
        d.className = "w";
        d.id = id;
        d.innerHTML = id.substring(0, 10) + "<div class=\"ep\"></div>";
        const {
            x,
            y,
            width,
            height,
        } = this.modulesLayout[name];
        d.style.left = `${x - width / 2}px`;
        d.style.top = `${y - height / 2}px`;
        d.style.width = `${width}px`;
        d.style.height = `${height}px`;
        d.style.lineHeight = `${height}px`;
        this.instance.getContainer().appendChild(d);
        this.initModule(d);
        return d;
    }
}

class jsPlumbConfig extends BaseConfig {
    configureModules(modulemap, parent) {
        for (const [moduleName, children] of Object.entries(modulemap)) {
            let module = {}
            module = this.newModule(moduleName);
            if (children !== "") {
                this.initParent(module);
                this.configureModules(children, moduleName);
            }
            if (parent !== "") {
                this.instance.addToGroup(`${parent}_group`, module);
            }
        }
    }

    createConnectionTypeMap() {
        const connectionTypeMap = {};
        for (const source of Object.keys(this.data.modules)) {
            connectionTypeMap[source] = {};
            for (const target of Object.keys(this.data.modules)) {
                connectionTypeMap[source][target] = 5;
            }
        }
        return connectionTypeMap;
    }
    
    /**
     * Utility method that return the nearest to 5 free number for a specific pair of source and target.
     * This number is greater than 0 and less than 10. It's interpeted as the type of connection.
     * 
     * @private
     * @this {jsPlumbConfig}
     * @param {string} s - Source module.
     * @param {string} t - Target module.
     * @param {object} map - Map of connection type between each module.
     * @return {number} - Connection Type.
     */
    getConnectionType(s, t, map) {
        const tmp = map[s][t];
        if (5 - tmp < 0) {
            map[s][t] = 5 + (5 - tmp);
        } else {
            map[s][t] = 5 + 1 + (5 - tmp);
            map[s][t] = (map[s][t] === 10) ? 9 : map[s][t];
        }
        return tmp;
    }

    configureConnection(portName, portInfo, moduleName) {
        const connectionTypeMap = this.createConnectionTypeMap();
        for (const targetName of this.modulesWithReadPort(portName, moduleName)) {
            let c = this.instance.connect({
                source: moduleName,
                target: targetName,
                type: `basic0.${this.getConnectionType(moduleName, targetName, connectionTypeMap)}`,
            });
            c.bind('mouseover', (conn, event) => {
                const info = document.querySelector('#infobox');
                info.style.left = `${event.clientX - info.offsetWidth / 2}px`;
                info.style.top = `${event.clientY - 80}px`;
                info.style.animation = 'infobox_appear 0.5s ease-in-out 0s 1 normal forwards';
                info.innerHTML = `
                    <div class='portinfo'>${portName}</div>
                    <div class='portinfo'>Bandwidth: ${portInfo.write_port.bandwidth}</div>
                    <div class='portinfo'>Latency: ${portInfo.read_ports.latency}</div>`;
            });
            c.bind('mouseout', () => {
                const info = document.querySelector('#infobox');
                info.style.animation = 'infobox_disappear 0.5s ease-in-out 0s 1 normal forwards';
            })
        }
    }

    configureConnections() {
        for (const [portName, portInfo] of Object.entries(this.data.portmap)) {
            for (const [moduleName, ports] of Object.entries(this.data.modules)) {
                if (ports.write_ports !== '' && portName in ports.write_ports) {
                    this.configureConnection(portName, portInfo, moduleName);
                }
            }
        }
    }
}

class dagreConfig extends BaseConfig {
    configureModules(modulemap, parent) {
        for (const [moduleName, children] of Object.entries(modulemap)) {
            this.graph.setNode(moduleName, { label: `${moduleName}_L`, width: this.moduleWidth, height: this.moduleHeight});
            if (children !== "") {
                this.configureModules(children, moduleName);
            }
            if (parent !== "") {
                this.graph.setParent(moduleName, parent);
            }
        }
    }

    configureConnection(portName, moduleName) {
        for (const targetName of this.modulesWithReadPort(portName, moduleName)) {
            // Waiting fix : https://github.com/dagrejs/dagre/issues/236
            const haveNoChildren = (graph, source, target) => {
                let sourceC = Object.entries(graph._children[source]).length;
                let targetC = Object.entries(graph._children[target]).length;
                return sourceC === 0 && targetC === 0;
            }
            if (haveNoChildren(this.graph, moduleName, targetName)) {
                this.graph.setEdge(moduleName, targetName);
            }
        }
    }

    configureConnections() {
        for (const portName of Object.keys(this.data.portmap)) {
            for (const [moduleName, ports] of Object.entries(this.data.modules)) {
                if (ports.write_ports !== '' && portName in ports.write_ports) {
                    this.configureConnection(portName, moduleName);
                }
            }
        }
    }
}

module.exports = class Topology {
    constructor(data, instance) {
        this.data = data;
        this.instance = instance;
        this.graph = new dagre.graphlib.Graph({
            directed: true,
            compound: true,
            multigraph: true,
        });
        this.graph.setGraph({
            rankdir: 'LR',
            ranker: 'network-simplex',
            ranksep: 100,
            edgesep: 10,
            nodesep: 120,
            marginx: 50,
            marginy: 10,
            align: 'DL'
        });
        this.graph.setDefaultEdgeLabel(function() { return {}; });
        this.moduleWidth = 125;
        this.moduleHeight = 125;
        this.modulesLayout = {};
        this.jsPlumbConfig = new jsPlumbConfig(data, this.graph, instance, this.moduleWidth, this.moduleHeight, this.modulesLayout);
        this.dagreConfig = new dagreConfig(data, this.graph, instance, this.moduleWidth, this.moduleHeight, this.modulesLayout);
    }

    get layout() {
        return this.graph;
    }

    configure() {
        this.configureLayout();
        dagre.layout(this.graph);
        this.jsPlumbConfig.configureModules(this.data.modulemap, "");
        this.jsPlumbConfig.configureConnections();
    }

    configureLayout() {
        this.dagreConfig.configureModules(this.data.modulemap, "");
        this.dagreConfig.configureConnections();
        this.graph.nodes().forEach(function(v) {
            this.modulesLayout[v] = this.graph.node(v);
        }, this);
    }
}
