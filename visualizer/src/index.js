// Copyright 2019 MIPT-MIPS
/*
 * (Type docs)
 * @author Eric Konks konks.em@phystech.edu
*/
import './css/jsplumb.css'
import './css/jsplumbtoolkit-defaults.css'
import './css/jsplumbtoolkit-demo.css'
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

    instance.bind("connection", function (info) {
        info.connection.getOverlay("label").setLabel(info.connection.id);
    });

    instance.batch(function () {
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
