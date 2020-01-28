// Copyright 2019 MIPT-MIPS
/*
 * (Type docs)
 * @author Eric Konks konks.em@phystech.edu
*/
const jsdom = require("jsdom");
const fs = require("fs");
const { JSDOM } = jsdom;

const rdom = fs.readFileSync('../tests/dom_test.html', (err) => {if (err) throw err;})
const dom = new JSDOM(rdom);

global.document = dom.window.document;
global.navigator = dom.window.navigator;

var jsPlumb = require('jsplumb').jsPlumb;
var Biltong = require('jsplumb').Biltong;
var Topology = require('../src/topology');
var expect = require('chai').expect;

global.jsPlumb = jsPlumb;
global.Biltong = Biltong;

const rdata = fs.readFileSync('../tests/topology_root_test.json', (err) => {if (err) throw err;});
const data = JSON.parse(rdata);

data.modules.B.write_ports['B_to_D_2'] = '';
data.modules.D.read_ports['B_to_D_2'] = '';
data.portmap['B_to_D_2'] = {
    write_port: {
        fanout: '1',
        bandwidth: '1'
    },
    read_ports: {
        latency: '1'
    }
}

describe('Layout checking', function() {
    const instance = jsPlumb.getInstance({
        Container: "canvas"
    });
    const topology = new Topology(data, instance);
    instance.registerConnectionType("basic0.1", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.1 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4} });
    instance.registerConnectionType("basic0.2", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.2 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.3", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.3 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.4", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.4 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.5", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.5 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.6", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.6 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.7", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.7 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.8", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.8 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });
    instance.registerConnectionType("basic0.9", { anchor:"Continuous", connector: [ "Flowchart", { stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true, midpoint: 0.9 } ], paintStyle: { stroke: "#5c96bc", strokeWidth: 2, outlineStroke: "transparent", outlineWidth: 4 } });

    dom.window.jps = instance;

    topology.configure();

    it('nodes', function(done) {
        expect(Object.keys(topology.layout._nodes).includes('A')).to.equal(true);
        expect(Object.keys(topology.layout._nodes).includes('B')).to.equal(true);
        expect(Object.keys(topology.layout._nodes).includes('C')).to.equal(true);
        expect(Object.keys(topology.layout._nodes).includes('D')).to.equal(true);
        expect(Object.keys(topology.layout._nodes).includes('test-root')).to.equal(true);
        expect(topology.layout._nodeCount).to.equal(5);
        done();
    });
    it('parents', function(done) {
        expect(topology.layout._parent.A).to.equal('test-root');
        expect(topology.layout._parent.C).to.equal('test-root');
        expect(topology.layout._parent.B).to.equal('A');
        expect(topology.layout._parent.D).to.equal('C');
        done();
    });
    it('edges', function(done) {
        expect(topology.layout._edgeCount).to.equal(2);
        done();
    });
    // This test dispatches mouse events for each connection and then tests handler
    it('mouse event handlers', function(done) {
        const info = document.querySelector('#infobox');
        for (const conn of instance.getAllConnections()) {
            conn._listeners.mouseover[1]({}, {clientX: 0, clientY: 0});
            expect(info.style.animation).to.equal('infobox_appear 0.5s ease-in-out 0s 1 normal forwards');
            conn._listeners.mouseout[1]({}, {});
            expect(info.style.animation).to.equal('infobox_disappear 0.5s ease-in-out 0s 1 normal forwards')
        }
        done();
    });
})
