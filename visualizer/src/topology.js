// Copyright MIPTV 2019
/*
 * (Type docs)
 * @author Eric Konks exdevour@gmail.com
*/
var dagre = require('dagre');
var jsPlumb = require('jsplumb').jsPlumb;
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
        this.moduleWidth = 100;
        this.moduleHeight = 100;
        this.modulesLayout = {};
    }

    get layout() {
        return this.graph;
    }

    configure() {
        this.configureLayout();
        dagre.layout(this.graph);
        this.configureModules(this.data.modulemap, "", false);
        this.configureConnections(false);
    }

    configureLayout() {
        this.configureModules(this.data.modulemap, "", true);
        this.configureConnections(true);
        this.graph.nodes().forEach(function(v) {
            this.modulesLayout[v] = this.graph.node(v);
        }, this);
    }

    configureModules(modulemap, parent, layoutmode) {
        for (const [moduleName, children] of Object.entries(modulemap)) {
            let module = {}
            if (layoutmode) {
                this.graph.setNode(moduleName, { label: `${moduleName}_L`, width: this.moduleWidth, height: this.moduleHeight});
            } else {
                module = this.newModule(moduleName);
            }
            if (children !== "") {
                if (!layoutmode) {
                    this.initParent(module);
                }
                this.configureModules(children, moduleName, layoutmode);
            }
            if (parent !== "") {
                if (layoutmode) {
                    this.graph.setParent(moduleName, parent);
                } else {
                    this.instance.addToGroup(`${parent}_group`, module);
                }
            }
        }
    }

    configureConnections(layoutmode) {
        const moduleMapConnections = {};
        for (const pName of Object.keys(this.data.modules)) {
            moduleMapConnections[pName] = {};
            for (const cName of Object.keys(this.data.modules)) {
                moduleMapConnections[pName][cName] = 5;
            }
        }
        // Output 5 6 4 7 3 8 2 9 1
        const getType = function(s, t, map) {
            const tmp = map[s][t];
            if (5 - tmp < 0) {
                map[s][t] = 5 + (5 - tmp);
            } else {
                map[s][t] = 5 + 1 + (5 - tmp);
                map[s][t] = (map[s][t] === 10) ? 9 : map[s][t];
            }
            return tmp;
        }
        for (const [portName, portinfo] of Object.entries(this.data.portmap)) {
            for (const [moduleName, ports] of Object.entries(this.data.modules)) {
                if (ports.write_ports !== '' && portName in ports.write_ports) {
                    for (const targetName of this.modulesWithReadPort(portName, moduleName)) {
                        if (layoutmode) {
                            if (this.graph.parent(moduleName) !== targetName) {
                                this.graph.setEdge(moduleName, targetName);
                            }
                        } else {
                            let c = this.instance.connect({
                                source: moduleName,
                                target: targetName,
                                type: `basic0.${getType(moduleName, targetName, moduleMapConnections)}`,
                            });
                            c.bind('mouseover', (conn, event) => {
                                const info = document.querySelector('#infobox');
                                info.style.left = `${event.clientX - info.offsetWidth / 2}px`;
                                info.style.top = `${event.clientY - 80}px`;
                                info.style.animation = 'infobox_appear 0.5s ease-in-out 0s 1 normal forwards';
                                info.innerHTML = `
                                    <div class='portinfo'>${portName}</div>
                                    <div class='portinfo'>Bandwidth: ${portinfo.write_port.bandwidth}</div>
                                    <div class='portinfo'>Latency: ${portinfo.read_ports.latency}</div>`;
                            });
                            c.bind('mouseout', (conn) => {
                                const info = document.querySelector('#infobox');
                                info.style.animation = 'infobox_disappear 0.5s ease-in-out 0s 1 normal forwards';
                            })
                        }
                    }
                }
            }
        }
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
            maxConnections: 5,
            onMaxConnections: function (info, e) {
                alert("Maximum connections (" + info.maxConnections + ") reached");
            }
        });
        this.instance.makeTarget(el, {
            dropOptions: { hoverClass: "dragHover" },
            anchor: "Continuous",
            allowLoopback: true
        });
    }

    initParent(module) {
        const test = this.instance.addGroup({
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

    moduleConnectionNumber(mName) {
        const connectionsNumber = {};
        for (const moduleName of Object.keys(this.data.modules)) {
            connectionsNumber[moduleName] = 0;
        }
        for (const portName of Object.keys(this.data.modules[mName])) {
            for (const moduleName of this.modulesWithReadPort(portName, mName)) {
                connectionsNumber[moduleName] += 1;
            }
        }
        return connectionsNumber;
    }

}
