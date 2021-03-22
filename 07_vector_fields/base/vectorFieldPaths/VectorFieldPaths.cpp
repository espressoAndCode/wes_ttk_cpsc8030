#include                  <VectorFieldPaths.h>

using namespace std;
using namespace ttk;

VectorFieldPaths::VectorFieldPaths(){

  triangulation_ = NULL;
}

VectorFieldPaths::~VectorFieldPaths(){
  
}

void VectorFieldPaths::setupRTree(){

    for(int i=0; i<triangulation_->getNumberOfVertices(); i++){
        polygon p;
        for(int j=0; j<3; j++){
            SimplexId v;
            triangulation_->getTriangleVertex(i,j,v);
            float x,y,z;
            triangulation_->getVertexPoint(v, x, y, z);
            p.outer().push_back(point(x,y));
        }

        box b = bg::return_envelope<box>(p);
        rtree.insert(std::make_pair(b, i));
    }

}

SimplexId VectorFieldPaths::queryRTree(float x, float y){

    vector<value> results;
    rtree.query(boost::geometry::index::intersects(point(x,y)), back_inserter(results));

    for(int i=0; i<results.size(); i++){

        for(int j=0; j<results.size(); j++)
            cout << results[j].second << " ";
        cout  << endl;

        SimplexId triangle = results[i].second;
        polygon p;
        for(int j=0; j<3; j++){

            SimplexId v;
            triangulation_->getTriangleVertex(triangle,j,v);
            float x,y,z;
            triangulation_->getVertexPoint(v, x, y, z);
            p.outer().push_back(point(x,y));
        }

        if(boost::geometry::touches(point(x,y), p) || boost::geometry::within(point(x,y), p))
            return triangle;
    }

    return -1;
}


double VectorFieldPaths::triangleArea(vector<pair<float,float> >& coordinates){
    polygon p;
    for(int j=0; j<3; j++){
        p.outer().push_back(point(coordinates[j].first,coordinates[j].second));
    }

    return abs(bg::area(p));
}