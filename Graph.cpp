#include "Graph.h"

Graph::Graph(const string & airport, const string & route)
{
    parseAirport(airport);
    parseRoute(route);
}

/* Pulled from: https://www.geeksforgeeks.org/program-distance-two-points-earth/ 
 * Returns distance in km
 */
double Graph::distance(double lat1, double long1, double lat2, double long2) {
    double lat1rad = lat1 / 180 * M_PI;
    double long1rad = long1 / 180 * M_PI;
    
    double lat2rad = lat2 / 180 * M_PI;
    double long2rad = long2 / 180 * M_PI;

    double longdif = long2rad - long1rad;

    return 3963.0 * acos((sin(lat1rad) * sin(lat2rad)) + cos(lat1rad) * cos(lat2rad) * cos(longdif)) * 1.609344;
}

void Graph::parseAirport(const string & filename)
{

    ifstream file(filename);
    string data = "";
    int counter = 0;

    int airportID;
    string name;
    string city;
    string country;
    string IATA;
    string ICAO;
    double latitude;
    double longitude;
    
    if(file.is_open())
    {
        while(getline(file, data,','))
        {
            if(counter == 0)
            {
                airportID = std::stoi(data);
            }
            else if(counter == 1)
            {
                name = data;
                // Check if the name has a comma in it
                while (data[data.size() - 1] != '\"') {
                    getline(file, data, ',');
                    name += "," + data;
                }
            }
            else if(counter == 2)
            {
                city = data;
                // Check if the name has a comma in it
                while (data[data.size() - 1] != '\"') {
                    getline(file, data, ',');
                    name += "," + data;
                }
            }
            else if(counter == 3)
            {
                country = data;
                // Check if the name has a comma in it
                while (data[data.size() - 1] != '\"') {
                    getline(file, data, ',');
                    name += "," + data;
                }
            }
            else if(counter == 4)
            {
                IATA = data;
            }
            else if(counter == 5)
            {
                ICAO = data;
            }
            else if(counter == 6)
            {

                latitude = std::stod(data);
            }
            else if(counter == 7)
            {
                longitude = std::stod(data);
                insertVertex(Vertex(airportID, name, city, country, IATA, ICAO, latitude, longitude));
                // Move onto next line.
                getline(file, data);
                counter = 0;
                continue;
            }
            counter++;

        }
        file.close();
    }

}

void Graph::parseRoute(const string & filename)
{
    ifstream file(filename);
    string data = "";
    int counter = 0;
    int firstID;
    int secondID;

    if(file.is_open())
    {
        while(getline(file, data,','))
        {
            if(counter == 3)
            {
                if(data == "\\N")
                {
                    getline(file, data);
                    counter = 0;
                    continue;
                }
                else
                {
                    firstID = std::stoi(data);
                }
            }
            else if(counter == 5)
            {
                if(data == "\\N")
                {
                    getline(file, data);
                    counter = 0;
                    continue;
                }
                else
                {
                    secondID = std::stoi(data);
                }
                Vertex first_vertex = converter_.find(firstID)->second;
                Vertex second_vertex = converter_.find(secondID)->second;
                insertEdge(firstID, secondID, distance(first_vertex.latitude_, first_vertex.longitude_, 
                                                        second_vertex.latitude_, second_vertex.longitude_));

                getline(file, data);
                counter = 0;
                continue;
            }
            counter++;

        }
        file.close();
    }

}


void Graph::insertVertex(Vertex v)
{
    
    std::pair<Vertex, list<Edge *>> ret(v, list<Edge *>());
    adjacency_list_.insert(ret);
    std::pair<int, Vertex> ret2(v.airportID_, v);
    converter_.insert(ret2);
}

// Needs to check if either (u, v) or (v, u) is in the graph already.
void Graph::insertEdge(int first, int second, double weight)
{
    if(converter_.find(first) == converter_.end())
    {
       return;
    }

    if(converter_.find(second) == converter_.end())
    {
        return;
    }

    if(areAdjacent(converter_.find(first)->second, converter_.find(second)->second) != NULL)
    {
        return;
    }

    Vertex first_vertex = converter_.find(first)->second;
    Vertex second_vertex = converter_.find(second)->second;

    Edge e(&(adjacency_list_.find(first_vertex)->first), &(adjacency_list_.find(second_vertex)->first), weight);
    edge_list_.push_back(e);
    
    degree_map_[first_vertex]++;
    degree_map_[second_vertex]++;
    adjacency_list_.find(first_vertex)->second.push_back(&edge_list_.back());
    adjacency_list_.find(second_vertex)->second.push_back(&edge_list_.back());


}

Graph::Edge* Graph::areAdjacent(const Vertex & v1, const Vertex & v2)
{
    
    if(degree_map_[v1] <= degree_map_[v2]) {
        // Search v1's adjacency list
        list<Edge *> adj = adjacency_list_.find(v1)->second;
        for(auto iter = adj.begin(); iter != adj.end(); ++iter) {
            Edge * edge = *iter;
            // Found corresponding edge
            if(edge->firstID_->airportID_ == v2.airportID_ || edge->secondID_->airportID_ == v2.airportID_) {
                return edge;
            } 
        }
    } else {

        // Search v2's adjacency list
        list<Edge *> adj = adjacency_list_.find(v2)->second;
        for(auto iter = adj.begin(); iter != adj.end(); ++iter) {
            Edge * edge = *iter;
            // Found corresponding edge
            if(edge->firstID_->airportID_ == v1.airportID_ || edge->secondID_->airportID_ == v1.airportID_) {
                return edge;
            } 
        }
    }
    return NULL;
}

list<Graph::Edge *> Graph::incidentEdges(const Vertex & v)
{
    return adjacency_list_[v];
}

double Graph::getWeight(const Graph::Edge & e) {
    return e.weight_;
}

vector<Graph::Vertex> Graph::getVertices() {
    vector<Vertex> ret;

    for(auto it = adjacency_list_.begin(); it != adjacency_list_.end(); ++it)
    {
        ret.push_back(it->first);
    }

    return ret;
}

vector<Graph::Edge *> Graph::getEdges() {
    if (edge_list_.empty()) {
        return vector<Graph::Edge *>();
    }

    vector<Edge*> ret;

    for (auto it = edge_list_.begin(); it != edge_list_.end(); ++it) {
        ret.push_back(&(*it));
    }

    return ret;
}

vector<Graph::Vertex> Graph::getAdjacent(const Vertex & v) {
    vector<Vertex> ret;
    list<Graph::Edge *> edges = incidentEdges(v);

    for (auto it = edges.begin(); it != edges.end(); ++it) {
        if (*((*it)->firstID_) == v) {
            ret.push_back(*((*it)->secondID_));
        } else {
            ret.push_back(*((*it)->firstID_));
        }
    }
    return ret;
}

Graph::v_label Graph::getVLabel(const Vertex & v) {
    return vertex_labels_[v];
}

void Graph::setVLabel(Vertex & v, v_label label) {
    vertex_labels_[v] = label;
}

Graph::e_label Graph::getELabel(const Edge *e) {
    return e->label_;
}

void Graph::setELabel(Edge *e, e_label label) {
    e->label_ = label;
}

void Graph::BFS() {
    for (Vertex & v : getVertices()) {
        setVLabel(v, UNEXPLORED);
    }

    for (Edge *e : getEdges()) {
        setELabel(e, UNDISCOVERED);
    }

    for (Vertex & v : getVertices()) {
        if (getVLabel(v) == UNEXPLORED) {
            BFS(v);
        }
    }
}

void Graph::BFS(Vertex & v) {
    queue<Vertex> q;
    setVLabel(v, VISITED);
    q.push(v);

    while (!q.empty()) {
        v = q.front();
        q.pop();
        // cout << v.airportID_ << " is labeled " << vertex_labels_[v] << endl;
        for (Vertex w : getAdjacent(v)) {
            if (getVLabel(w) == UNEXPLORED) {
                setELabel(areAdjacent(v, w), DISCOVERY);
                setVLabel(w, VISITED);
                q.push(w);
            } else if (getELabel(areAdjacent(v, w)) == UNDISCOVERED) {
                setELabel(areAdjacent(v, w), CROSS);
            }
        }
    }
    // for (auto it = edge_list_.begin(); it != edge_list_.end(); ++it) {
    //     cout << "Edge (" << it->firstID_->airportID_ << ", " << it->secondID_->airportID_ << ") is labeled " << it->label_ << endl;
    // }
}

void Graph::DFS() {
    for (Vertex & v : getVertices()) {
        setVLabel(v, UNEXPLORED);
    }

    for (Edge *e : getEdges()) {
        setELabel(e, UNDISCOVERED);
    }

    for (Vertex & v : getVertices()) {
        if (getVLabel(v) == UNEXPLORED) {
            DFS(v);
        }
    }
}

void Graph::DFS(Vertex & v) {
    stack<Vertex> s;
    setVLabel(v, VISITED);
    s.push(v);

    while (!s.empty()) {
        v = s.top();
        s.pop();
        // cout << v.airportID_ << " is labeled " << vertex_labels_[v] << endl;
        for (Vertex w : getAdjacent(v)) {
            if (getVLabel(w) == UNEXPLORED) {
                setELabel(areAdjacent(v, w), DISCOVERY);
                setVLabel(w, VISITED);
                s.push(w);
            } else if (getELabel(areAdjacent(v, w)) == UNDISCOVERED) {
                setELabel(areAdjacent(v, w), CROSS);
            }
        }
    }
    // for (auto it = edge_list_.begin(); it != edge_list_.end(); ++it) {
    //     cout << "Edge (" << it->firstID_->airportID_ << ", " << it->secondID_->airportID_ << ") is labeled " << it->label_ << endl;
    // }
}

void Graph::printVertex()
{
    for(auto it = adjacency_list_.begin(); it != adjacency_list_.end(); it++)
    {
        std::cout << it->first.airportID_ << " has degree " << degree_map_[it->first] << ". ";
        std::cout << it->first.airportID_ << " is adjacent to: " << std::endl;
        
        list<Edge *> adj = incidentEdges(it->first);
        for(Edge * edge : adj) {
            std::cout << "(" << edge->firstID_->airportID_ << ", " << edge->secondID_->airportID_ << ") " << std::endl;
        }
    }
}

void Graph::printEdge()
{
    for(auto it = edge_list_.begin(); it != edge_list_.end(); it++)
    {
        std::cout << it->firstID_->airportID_ << " connected to " << it->secondID_->airportID_ << ". Weight = " << it->weight_ << std::endl;
    }
    std::cout << "Number of airports: " << adjacency_list_.size() << std::endl;
}
