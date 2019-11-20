import draw2d from 'draw2d'
import loadConfig from './miptv'

loadConfig();

var router = new draw2d.layout.connection.CircuitConnectionRouter();
router.abortRoutingOnFirstVertexNode=false;
var createConnection=function(sourcePort, targetPort){
    var c = new draw2d.Connection({
        outlineColor:"#ffffff",
        outlineStroke:1,
        color:"#000000",
        router: router,
        stroke:1,
        radius:2
    });
    if(sourcePort) {
        c.setSource(sourcePort);
        c.setTarget(targetPort);
    }
    return c;
};

document.addEventListener("DOMContentLoaded",function () {
        // create the canvas for the user interaction
        //
        var canvas = new draw2d.Canvas("gfx_holder");
        canvas.installEditPolicy(new draw2d.policy.canvas.ShowGridEditPolicy());
        canvas.installEditPolicy(new draw2d.policy.canvas.FadeoutDecorationPolicy());

        // install a Connection create policy which matches to a "circuit like"
        // connections
        //
        canvas.installEditPolicy( new draw2d.policy.connection.ComposedConnectionCreatePolicy(
            [
                // create a connection via Drag&Drop of ports
                //
                new draw2d.policy.connection.DragConnectionCreatePolicy({
                    createConnection:createConnection
                }),
                // or via click and point
                //
                new draw2d.policy.connection.OrthogonalConnectionCreatePolicy({
                    createConnection:createConnection
                })
            ])
        );

        var amp =new draw2d.shape.analog.OpAmp();
        canvas.add(amp,90,50);

        var bridge=new draw2d.shape.analog.ResistorBridge()
        canvas.add(bridge,90,150);

        var voltage=new draw2d.shape.analog.VoltageSupplyHorizontal();
        canvas.add(voltage,230,110);

        var voltage2 = new draw2d.shape.analog.VoltageSupplyVertical();
        canvas.add(voltage2,20,350);
        canvas.add(new draw2d.shape.basic.Label({text:"draw2d.shape.analog.VoltageSupplyVertical"}),200,355);

        var resistor = new draw2d.shape.analog.ResistorVertical();
        canvas.add(resistor,50,450);
        canvas.add(new draw2d.shape.basic.Label({text:"draw2d.shape.analog.ResistorVertical"}),200,455);

        var fulcrum = new draw2d.shape.node.Fulcrum();
        canvas.add(fulcrum,280,270);

	   	 // Create a Connection and connect the Start and End node
	   	 //
	   	 // add the connection to the canvas as well
	   	 canvas.add(createConnection(amp.getInputPort(0),bridge.getHybridPort(2)));
         canvas.add(createConnection(voltage.getHybridPort(0),bridge.getHybridPort(1)));
         canvas.add(createConnection(voltage2.getHybridPort(1),voltage.getHybridPort(0)));
         canvas.add(createConnection(fulcrum.getHybridPort(0),amp.getOutputPort(0)));

	   	 canvas.getCommandStack().notifyListeners(null, draw2d.command.CommandStack.POST_EXECUTE);
});