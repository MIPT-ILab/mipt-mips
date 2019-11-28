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
var Topology = require('../src/topology');
var expect = require('chai').expect;

const rdata = fs.readFileSync('../tests/topology_root_test.json', (err) => {if (err) throw err;});
const data = JSON.parse(rdata);

describe('Layout checking', function() {
    const instance = jsPlumb.getInstance({
        Container: "canvas"
    });
    const topology = new Topology(data, instance);
    topology.configureLayout();
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
        expect(topology.layout._edgeCount).to.equal(8);
        done();
    });
})
