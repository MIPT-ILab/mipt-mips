//import './css/jsplumb-doc.css'
import './css/jsplumb.css'
import './css/jsplumbtoolkit-defaults.css'
import './css/jsplumbtoolkit-demo.css'
import './css/main.css'
import './css/index.css'
import Topology from './topology'
jsPlumb.ready(function() {
    var instance = jsPlumb.getInstance({
        Endpoint: ["Dot",{radius: 2}],
        Connector: "Flowchart",
        HoverPaintStyle: {stroke: "#1e8151", strokeWidth: 2},
        ConnectionOverlays: [
            [ "Arrow", {
                location: 1,
                id: "arrow",
                lenght: 14,
                foldback: 0.8
            } ],
        ],
        Container: "canvas"
    });


    instance.registerConnectionType("basic0.1", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.1 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4} });
    instance.registerConnectionType("basic0.2", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.2 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.3", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.3 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.4", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.4 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.5", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.5 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.6", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.6 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.7", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.7 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.8", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.8 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.9", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.9 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });

    window.jps = instance;

    var canvas = document.getElementById("canvas");
    var windows = jsPlumb.getSelector(".statemachine-demo .w");

    instance.bind("connection", function (info) {
        info.connection.getOverlay("label").setLabel(info.connection.id);
    });

    var initNode = function(el) {

        // initialise draggable elements.
        instance.draggable(el);

        instance.makeSource(el, {
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

        instance.makeTarget(el, {
            dropOptions: { hoverClass: "dragHover" },
            anchor: "Continuous",
            allowLoopback: true
        });
    };

    var newNode = function(x, y) {
        var d = document.createElement("div");
        var id = jsPlumbUtil.uuid();
        d.className = "w";
        d.id = id;
        d.innerHTML = id.substring(0, 7) + "<div class=\"ep\"></div>";
        d.style.left = x + "px";
        d.style.top = y + "px";
        instance.getContainer().appendChild(d);
        initNode(d);
        return d;
    };
    // suspend drawing and initialise.
    instance.batch(function () {
        for (var i = 0; i < windows.length; i++) {
            initNode(windows[i], true);
        }
        $.getJSON('topology.json', function(data) {
            const topology = new Topology(data, instance);
            topology.configure();
        }).fail(function() {
            const err = `Couldn't open file ${process.env.path}`
            alert(err);
            throw(new Error(err));
        });
    });
});

