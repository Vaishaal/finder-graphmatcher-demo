Array.prototype.remove = function() {
    var what, a = arguments, L = a.length, ax;
    while (L && this.length) {
        what = a[--L];
        while ((ax = this.indexOf(what)) !== -1) {
            this.splice(ax, 1);
        }
    }
    return this;
};

var mode = {mode: -1}
var line_mode = -1;
var curr_line = [null,null]
var canvas = null;
var building = null;
var intersection = null;
var road = null;
var erase = null;
var link = null;
var buttons = null;
var button_canvas = null;
var pre_hover_color = null;
var pre_hover_width = null;
var curr_selection = null;
var colors = ["red","blue","gray", "black"]
var ALL_ATTRIBUTES = ["height", "length", "nodeType", "degree", "roadClass"]
var graph = {}
var graph_edges = {}
var attributes = {}
var hover = false;
var scale_x = 1
var scale_y = 1
function point_it(event){
    pos_x = event.offsetX?(event.offsetX):event.pageX-document.getElementById("pointer_div").offsetLeft;
    pos_y = event.offsetY?(event.offsetY):event.pageY-document.getElementById("pointer_div").offsetTop;
    document.getElementById("preview").style.left = (pos_x-1) ;
    document.getElementById("preview").style.top = (pos_y-15) ;
    document.getElementById("preview").style.visibility = "visible" ;
    click(pos_x,pos_y, mode)
}

function watch_mode (id, oldval, newval)
{
    console.log("Mode changed")
    // Handle mode clean up when mode changes
    if (oldval == 3) {
        line_mode = -1;
        curr_line[0]
        if (curr_line[0] != null) {
            s = curr_line[0]
            s.attr("fill",colors[graph[s.node.raphaelid].nodeType])
        }
        if (curr_line[1] != null) {
            e = curr_line[1]
            e.attr("fill",colors[graph[e.node.raphaelid].nodeType])
        }
        curr_line = [null, null]
    }
    return newval;
}

function hover_on() {
    hover = true;
    pre_hover_color = this.attr("stroke")
    pre_hover_width = this.attr("stroke-width")
    this.attr("stroke", "black");
    this.attr("stroke-width", pre_hover_width * 2);

}

function hover_off() {
    hover = false;
    this.attr("stroke", pre_hover_color);
    this.attr("stroke-width", pre_hover_width);
    pre_hover_color = null;
    pre_hover_width = null;
}
mode.watch("mode", watch_mode);

function click(x, y, mode) {
    if (0 <= mode.mode && mode.mode <= 2 && !hover) {
        circle = canvas.circle(x,y, 50);
        circle.attr("fill",colors[mode.mode])
        circle.attr("stroke",colors[mode.mode])
        circle.node.onclick = obj_click;
        circle.hover(hover_on, hover_off);
        graph[circle.node.raphaelid] = {nodeType:mode.mode, edges:[]}
    }
}

function save_attr(){
    var inputs = $('.building, .intersection, .road');
    if (! (curr_selection in attributes)) {
        attributes[curr_selection] = {}
    }
    for (i = 0; i < inputs.length; i++) {
        attributes[curr_selection][inputs[i].name] = inputs[i].value
        inputs[i].disabled = true;
        inputs[i].value = "";
    }
    curr_selection = null;
    $('#attr-window').modal('hide');

}
function reset_selection(e) {
    curr_selection = null;
    var inputs = $('.building, .intersection, .road');
    for (i = 0; i < inputs.length; i++) {
        inputs[i].disabled = true;
        inputs[i].value = "";
    }
}
function obj_click(e) {
    point = canvas.getById(e.srcElement.raphaelid);
    r_id = point.node.raphaelid;
    if (mode.mode == 5) {
        if (r_id in graph) {
            curr_selection = r_id;
            if (graph[r_id].nodeType == 0){
                var building_inputs = $('.building');
                for (i = 0; i < building_inputs.length; i++) {
                    building_inputs[i].disabled = false
                    console.log(building_inputs[i])
                    if (r_id in attributes) {
                        building_inputs[i].value = attributes[curr_selection][building_inputs[i].name]
                    }
                }
            }
            if (graph[r_id].nodeType == 1) {
                var intersection_inputs = $('.intersection');
                for (i = 0; i < intersection_inputs.length; i++) {
                    intersection_inputs[i].disabled = false
                    if (r_id in attributes) {
                        intersection_inputs[i].value = attributes[curr_selection][intersection_inputs[i].name]
                    }
                }

            }

            if (graph[r_id].nodeType == 2) {
                var road_inputs = $('.road');
                for (i = 0; i < road_inputs.length; i++) {
                    road_inputs[i].disabled = false
                    if (r_id in attributes) {
                        road_inputs[i].value = attributes[curr_selection][road_inputs[i].name]
                    }
                }

            }
            $('#attr-window').modal('show');
        }
    }
    if (mode.mode == 4) {
        if (r_id in graph) {
            node = graph[r_id];
            for (i = 0; i < node.edges.length; i++) {
                edge = node.edges[i];
                delete graph_edges[edge.line]
                canvas.getById(edge.line).remove()
                if (r_id != edge.start) {
                    console.log("one")
                    graph[edge.start].edges.remove(edge)
                } else {
                    console.log("two")
                    graph[edge.end].edges.remove(edge)
                }
            }
        delete graph[r_id];
        } else if (r_id in graph_edges){
            edge = graph_edges[r_id];
            graph[graph_edges[r_id].start].edges.remove(edge)
            graph[graph_edges[r_id].end].edges.remove(edge)
            delete graph_edges[r_id];
        }
        point.remove()
    }

    if (mode.mode == 3){
        if (point == curr_line[0] || r_id in graph_edges) {
            return;
        }
        line_mode += 1;
        point.attr("fill", "white");
        curr_line[line_mode] = point;
        if (line_mode == 1) {
            s = curr_line[0]
            e = curr_line[1]
            edge = make_edge(s,e)
            line_mode = -1
            s.attr("fill",colors[graph[s.node.raphaelid].nodeType])
            e.attr("fill",colors[graph[e.node.raphaelid].nodeType])
            s.toFront()
            e.toFront()
            graph_edges[edge.line] = edge;
            graph[curr_line[0].node.raphaelid].edges.push(edge)
            graph[curr_line[1].node.raphaelid].edges.push(edge)
            curr_line = [null, null]
        }
    }
}

function restore_state(raph_json, graph_json, edges_json) {
    line_mode = -1;
    curr_line = [null, null];
    canvas.clear();
    canvas.fromJSON(raph_json);
    graph = JSON.parse(graph_json);
    graph_edges = JSON.parse(edges_json);
}

function make_edge(start, end) {
    var path = "M" + start.attr("cx") + " " + start.attr("cy") 
            + " L" + end.attr("cx") + " " + end.attr("cy");
    var elem = canvas.path(path);
    elem.node.onclick = obj_click;
    elem.hover(hover_on, hover_off);
    elem.attr("stroke-width", "20");
    return {start: start.node.raphaelid, end: end.node.raphaelid, line: elem.node.raphaelid}
};

function setup() {
    var img = $('#preview')[0];
    scale_x = img.width/640.0
    scale_y = img.height/480.0
    canvas = Raphael("canvas",img.width, img.height)
    buttons = Raphael(img.x+img.width, img.y, 320, img.height*2)
    building = buttons.path("M27.812,16l-3.062-3.062V5.625h-2.625v4.688L16,4.188L4.188,16L7,15.933v11.942h17.875V16H27.812zM16,26.167h-5.833v-7H16V26.167zM21.667,23.167h-3.833v-4.042h3.833V23.167z").attr({fill: "green", stroke: "green"});
    building.scale(2)
    building.translate(20,20)
    intersection = buttons.path("M25.545,23.328,17.918,15.623,25.534,8.007,27.391,9.864,29.649,1.436,21.222,3.694,23.058,5.53,15.455,13.134,7.942,5.543,9.809,3.696,1.393,1.394,3.608,9.833,5.456,8.005,12.98,15.608,5.465,23.123,3.609,21.268,1.351,29.695,9.779,27.438,7.941,25.6,15.443,18.098,23.057,25.791,21.19,27.638,29.606,29.939,27.393,21.5z").attr({fill: "green", stroke: "green"});
    intersection.scale(2)
    intersection.translate(20,80)
    erase = buttons.path("M24.778,21.419 19.276,15.917 24.777,10.415 21.949,7.585 16.447,13.087 10.945,7.585 8.117,10.415 13.618,15.917 8.116,21.419 10.946,24.248 16.447,18.746 21.948,24.248z").attr({fill: "red", stroke: "red"});
    erase.scale(2)
    erase.translate(20,230)
    road = buttons.path("M21.786,12.876l7.556-4.363l-7.556-4.363v2.598H2.813v3.5h18.973V12.876zM10.368,18.124l-7.556,4.362l7.556,4.362V24.25h18.974v-3.501H10.368V18.124z").attr({fill:"green", stroke:"green"});
    road.scale(2)
    road.translate(20,130)
    link = buttons.path("M23.188,3.735c0-0.975-0.789-1.766-1.766-1.766s-1.766,0.791-1.766,1.766s1.766,4.267,1.766,4.267S23.188,4.71,23.188,3.735zM20.578,3.734c0-0.466,0.378-0.843,0.844-0.843c0.467,0,0.844,0.377,0.844,0.844c0,0.466-0.377,0.843-0.844,0.843C20.956,4.578,20.578,4.201,20.578,3.734zM25.281,18.496c-0.562,0-1.098,0.046-1.592,0.122L11.1,13.976c0.199-0.181,0.312-0.38,0.312-0.59c0-0.108-0.033-0.213-0.088-0.315l8.41-2.239c0.459,0.137,1.023,0.221,1.646,0.221c1.521,0,2.75-0.485,2.75-1.083c0-0.599-1.229-1.083-2.75-1.083s-2.75,0.485-2.75,1.083c0,0.069,0.021,0.137,0.054,0.202L9.896,12.2c-0.633-0.188-1.411-0.303-2.265-0.303c-2.088,0-3.781,0.667-3.781,1.49c0,0.823,1.693,1.489,3.781,1.489c0.573,0,1.11-0.054,1.597-0.144l11.99,4.866c-0.19,0.192-0.306,0.401-0.306,0.623c0,0.188,0.096,0.363,0.236,0.532L8.695,25.415c-0.158-0.005-0.316-0.011-0.477-0.011c-3.241,0-5.87,1.037-5.87,2.312c0,1.276,2.629,2.312,5.87,2.312c3.241,0,5.87-1.034,5.87-2.312c0-0.22-0.083-0.432-0.229-0.633l10.265-5.214c0.37,0.04,0.753,0.066,1.155,0.066c2.414,0,4.371-0.771,4.371-1.723C29.65,19.268,27.693,18.496,25.281,18.496z").attr({fill:"green", stroke:"green"});
    link.scale(2)
    link.translate(20,180)
    tag = buttons.path("M14.263,2.826H7.904L2.702,8.028v6.359L18.405,30.09l11.561-11.562L14.263,2.826zM6.495,8.859c-0.619-0.619-0.619-1.622,0-2.24C7.114,6,8.117,6,8.736,6.619c0.62,0.62,0.619,1.621,0,2.241C8.117,9.479,7.114,9.479,6.495,8.859z").attr({fill:"green", stroke:"green"});
    tag.scale(2)
    tag.translate(70,20)
    download = buttons.path("M24.345,13.904c0.019-0.195,0.03-0.392,0.03-0.591c0-3.452-2.798-6.25-6.25-6.25c-2.679,0-4.958,1.689-5.847,4.059c-0.589-0.646-1.429-1.059-2.372-1.059c-1.778,0-3.219,1.441-3.219,3.219c0,0.21,0.023,0.415,0.062,0.613c-2.372,0.391-4.187,2.436-4.187,4.918c0,2.762,2.239,5,5,5h3.404l-0.707-0.707c-0.377-0.377-0.585-0.879-0.585-1.413c0-0.533,0.208-1.035,0.585-1.412l0.556-0.557c0.4-0.399,0.937-0.628,1.471-0.628c0.027,0,0.054,0,0.08,0.002v-0.472c0-1.104,0.898-2.002,2-2.002h3.266c1.103,0,2,0.898,2,2.002v0.472c0.027-0.002,0.054-0.002,0.081-0.002c0.533,0,1.07,0.229,1.47,0.63l0.557,0.552c0.78,0.781,0.78,2.05,0,2.828l-0.706,0.707h2.403c2.762,0,5-2.238,5-5C28.438,16.362,26.672,14.332,24.345,13.904z M21.033,20.986l-0.556-0.555c-0.39-0.389-0.964-0.45-1.276-0.137c-0.312,0.312-0.568,0.118-0.568-0.432v-1.238c0-0.55-0.451-1-1-1h-3.265c-0.55,0-1,0.45-1,1v1.238c0,0.55-0.256,0.744-0.569,0.432c-0.312-0.313-0.887-0.252-1.276,0.137l-0.556,0.555c-0.39,0.389-0.39,1.024-0.001,1.413l4.328,4.331c0.194,0.194,0.451,0.291,0.707,0.291s0.512-0.097,0.707-0.291l4.327-4.331C21.424,22.011,21.423,21.375,21.033,20.986z").attr({fill:"blue", stroke:"blue"});
    download.scale(2)
    download.translate(70,70)
    button_canvas = buttons;
    buttons = [building, intersection, road, link, erase, tag, download];
    for (i = 0; i < buttons.length; i++) {
        buttons[i].node.onclick = click_button;
        buttons[i].hover(hover_on, hover_off)
    }
}

function click_button(e) {
    for (i = 0; i < buttons.length; i++) {
         if (button_canvas.getById(e.srcElement.raphaelid) == buttons[i]){
             break;
         }
    }
    if (mode.mode >= 0) {
        if (mode.mode == 4){
            buttons[mode.mode].attr("fill","red");
        } else if (mode.mode == 6) {
            buttons[mode.mode].attr("fill", "blue");
        } else {
            buttons[mode.mode].attr("fill","green");
        }
    }
    if (i == 6) {
        var zip = new JSZip();
        var nodes = []
        var edges = {}
        zip.file(file_name+"_graph.json", JSON.stringify(graph))
        zip.file(file_name+"_graph-edges.json", JSON.stringify(graph_edges))
        zip.file(file_name+"_attributes.json", JSON.stringify(attributes))
        zip.file(file_name+"_graph.json", canvas.toJSON());
        for (key in graph) {
            var node = {}
            node['key'] = parseInt(key)
            // For compatibility with background
            node['x'] = -1
            node['y'] = -1
            this_node = graph[key]
            var attrs = attributes[key]
            if (attrs == undefined) {
                attributes[key] = {}
                attrs = attributes[key]
            }
            attrs["nodeType"] = this_node["nodeType"]
            node["attr"] = attrs
            fill_attributes(attrs)
            edges[node['key'].toString()] = this_node['edges'].map( function(x) { if (node['key'] == x['start']) {return parseInt(x['end'])} return parseInt(x['start']) })
            nodes.push(node)
        } zip.file(file_name+".nodes.json", JSON.stringify(nodes))
        zip.file(file_name+".edges.json", JSON.stringify(edges))
        var blob = zip.generate({type:"blob"});
        saveAs(blob, file_name + "_model.zip");
    }
    else if (mode.mode != i) {
        buttons[i].attr("fill", "white")
        mode.mode = i;
    } else {
        mode.mode = -1;
    }
}
function fill_attributes(attributes){
    for (key in attributes) {
        if (attributes[key] === "")
            attributes[key] = -1
        else
            attributes[key] = parseInt(attributes[key])
    }
    for (var i = 0; i < ALL_ATTRIBUTES.length; i++){
        if (attributes[ALL_ATTRIBUTES[i]] == undefined) {
            attributes[ALL_ATTRIBUTES[i]] = -1
        }
    }
}


