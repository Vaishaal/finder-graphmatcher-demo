Finder Graph Matcher Demo
===========================

This demo runs in a virtual box vm provisioned running under vagrant. This is to minimize hassle with dependency installs.

Steps:
---------------
0. Install [virtual box](https://www.virtualbox.org/wiki/Downloads) and [vagrant](https://www.vagrantup.com/downloads.html)
1. From the root of this repository type in ```vagrant up```. This will take some time as it will provision the virtual machine 
and install all necessary dependencies
2. Type in vagrant ssh, this should open up an ssh session into your virtual machine. This also spins up a python webserver for query generation
3. Go to localhost:8000 in your browser (preferably google chrome). This interface allows you to draw
   graphs for the query. Press the blue download button to download the graph file.
4. Transfer the {query_name}.nodes.json file and {query_name}.edges.json file and move it to this directory.
5. ```cd ~/graphmatcher``` (inside your vm)
6. ``sbt compile;stage``
7. ```./target/start --neo4jpath /tmp/graph.db --db ~/data/sampleJordanGraph -gr --query ~/{query_name}``
8. The last step should run the graph matcher. This should take anywhere from 1 to 5 minutes. After a sucessful run, there will be a file
```{query_name}.subgraph.json```. This json should will be the same format (specified below) as the query graph and background graph.


JSON format
-------------
The standard input and output to the graph matcher is the json graph format. A graph consists of (atleast) two json files. A graph must have ```{graphname}.nodes.json``` and ```{graphname}.edges.json```. Note that ```{graphname}``` must be the same value or else the matcher won't know which nodes correspond to the edges and vice versa. Optionally the background, database graph produced by the linker can also take a ```{graphname}.weights.json```. The format of the jsons is explained below.


 Node JSON:
  ```
          {
            ------ Required Fields ------
            "key":           Long,    # unique identifier for node
            "attributes":    JSON     # Matchable attributes for node
            ------ Optional Fields ------
            "x":             Double   # longitude 
            "y":             Double   # latitude 
          }
          
  ```
  Node Attributes JSON:
  ```
            {
            ------ Required Fields ------
            "nodeType":      Int      # 0 --> Building, 1 --> Intersection, 2 ---> Road
            ------ Optional Fields ------
            "height":        Int      # Height of building in stories (default -1)
            "length":        Double   # Length of building facade in meters (default -1)
            "roadClass":     Int      # Road class according to: http://wiki.openstreetmap.org/wiki/Key:highway (starting at service road = 0) (default -1)
            "degree":        Int      # Degree of intersection, number of intersecting road segments (default -1)
            "angle":         Double   # Angle of road relative to north
            }
          
  ```
  Edge JSON
  Here is an example edges json. It simply maps a string int key to a list of ints which correspond to the nodes connected from the node corresponding to the key value
  ```
  
  {
  "1360": [1360, 1353, 1206426978, 1362], 
  "475": [474, 475, 476, 1205665845], 
  "479": [1048801, 490, 1048719, 1205666009, 443, 479], 
  "2916": [1048586, 2916, 1206401734, 735], 
  "1368": [1368, 1048690, 1387, 1358, 1172611975]
  }
  
  ```
  Note that the input (background) graph could also have an optional file ending in ```.weights.json```. This is analagous to the edges json, except it provides the weight for the respective edges in the ```.edges.json```. This is used for probabilsitic graphs. Here is the example weights json for the above edges json.
  
   ```
  {
  "1360": [1, 1, 1, 1], 
  "475": [1, 1, 1, 1], 
  "479": [1, 0.5, 1, 1, 0.5, 1], 
  "2916": [1, 1, 1, 1], 
  "1368": [1, 1, 0.5, 1, 1]
  }
  ```
  

