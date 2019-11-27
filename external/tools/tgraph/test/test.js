const jsdom = require("jsdom");
const { JSDOM } = jsdom;    
const dom = new JSDOM(`
<!doctype html>
<html></html>
    <head></head>
    <body data-demo-id="statemachine">
        <div class="jtk-demo-main">
            <div id="infobox">  
            </div>
            <div class="jtk-demo-canvas canvas-wide statemachine-demo jtk-surface jtk-surface-nopan" id="canvas">
            </div>
        </div>
    </body>
</html>
`);

global.document = dom.window.document;
global.navigator = dom.window.navigator;

var jsPlumb = require('jsplumb').jsPlumb;
var Topology = require('../src/topology');
var expect = require('chai').expect;

const data = {
    modules: {
        A: {
            write_ports: {
                AB: ""
            },
            read_ports: {
                BA: ""
            }
        },
        B: {
            write_ports: {
                BA: ""
            },
            read_ports: {
                AB: ""
            }
        },
        C: {
            write_ports: {
                CD: ""
            },
            read_ports: {
                DC: ""
            }
        },
        D: {
            write_ports: {
                DC: ""
            },
            read_ports: {
                CD: ""
            }
        }
    },
    portmap: {
        AB: {
            write_port: {
                fanout: "1",
                bandwidth: "1"
            },
            read_ports: {
                latency: "1"
            }
        },
        BA: {
            write_port: {
                fanout: "1",
                bandwidth: "1"
            },
            read_ports: {
                latency: "1"
            }
        },
        CD: {
            write_port: {
                fanout: "1",
                bandwidth: "1"
            },
            read_ports: {
                latency: "1"
            }
        },
        DC: {
            write_port: {
                fanout: "1",
                bandwidth: "1"
            },
            read_ports: {
                latency: "1"
            }
        }
    },
    modulemap: {
        A: {
            C: "",
        },
        B: {
            D: "",
        },
    }
}

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
        expect(topology.layout._nodeCount).to.equal(4);
        done();
    });
    it('parents', function(done) {
        expect(topology.layout._parent.C).to.equal('A');
        expect(topology.layout._parent.D).to.equal('B');
        done();
    });
    it('edges', function(done) {
        expect(topology.layout._edgeCount).to.equal(4);
        done();
    });
})