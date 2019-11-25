class MIPTV {
    constructor(data, instance) {
        this.data = data;
        this.instance = instance;
    }

    configure() {
        this.configureModules(this.data.modulemap, "");
        this.configureConnections();
    }

    configureModules(modulemap, parent) {
        for (const [moduleName, children] of Object.entries(modulemap)) {
            let module = {}
            if (children !== "") {
                module = this.newModule(moduleName, 0, 0);
                this.initParent(module);
                this.configureModules(children, moduleName);
            } else {
                module = this.newModule(moduleName, 0 , 0);
            }
            if (parent !== "") {
                this.instance.addToGroup(`${parent}_group`, module);
            }
        }
    }

    configureConnections() {
        for (const portName of Object.keys(this.data.portmap)) {
            for (const [moduleName, ports] of Object.entries(this.data.modules)) {
                if (ports.write_ports !== '' && portName in ports.write_ports) {
                    for (const targetName of this.modulesWithReadPort(portName)) {
                        let c = this.instance.connect({
                            source: moduleName,
                            target: targetName,
                            type: "basic0.5"
                        });
                        c.getOverlay("label").setLabel(portName);
                    }
                }
            }
        }
    }

    modulesWithReadPort(portName) {
        const modules = [];
        for (const [moduleName, ports] of Object.entries(this.data.modules)) {
            if (portName in ports.read_ports) {
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
        var test = this.instance.addGroup({
            el: module,
            id: `${module.id}_group`,
            constrain: true,
            anchor:"Continuous",
            endpoint:[ "Dot", { radius:3 } ],
            droppable: false,
            ghost: true,
        });
    }

    newModule(name, x, y) {
        var d = document.createElement("div");
        var id = name;
        d.className = "w";
        d.id = id;
        d.innerHTML = id.substring(0, 7) + "<div class=\"ep\"></div>";
        d.style.left = x + "px";
        d.style.top = y + "px";
        this.instance.getContainer().appendChild(d);
        this.initModule(d);
        return d;
    }

}

export default MIPTV;